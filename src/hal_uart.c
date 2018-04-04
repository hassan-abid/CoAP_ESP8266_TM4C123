/*
 * @file 	hal_uart.c
 * @author	Hassan Abid
 * @date	Mar 2, 2018
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"


#include "hal_defs.h"
#include "hal_uart.h"
#include "hal_uart_private.h"



#define UART_TX_FIFO_SIZE_DEFAULT			((uint32_t)64)
#define UART_RX_FIFO_SIZE_DEFAULT			((uint32_t)64)


#define HAL_UART_LOCK(uart)				
#define HAL_UART_UNLOCK(uart)			



#define __HAL_UART_DEFINE(index, ...) \
	HAL_UART_t uart##index = {.base = UART##index, \
														};\



/**
 * @defgroup	hal_uart_Private_Members		hal_uart Private Members
 * @{
 */

/** @} */


HAL_UART_LIST(__HAL_UART_DEFINE)



/**
 * @defgroup hal_uart_Private_Functions	hal_uart Private Functions
 * @{
 */

uint32_t txFIFOPut(HAL_UART_t* uart, uint8_t** data, uint32_t* length)
{
	uint32_t count = 0;
	while (UARTCharPutNonBlocking((uint32_t)uart->base, **data) && (*length))
	{
		(*length)--;
		(*data)++;
		count++;
	}
	
	return count;
}

uint32_t rxFIFOGet(HAL_UART_t* uart, uint8_t** data, uint32_t* length)
{
	uint32_t count = 0;
	int32_t ch;

	while(*length)
	{
		ch = UARTCharGetNonBlocking((uint32_t)uart->base);
		if (ch == -1)
			break;
		
		count++;
		**data = (uint8_t)(ch & 0xFF);
		(*data)++;
		(*length)--;
	
	}
	
	return count;
}

HAL_Return_t HAL_UART_Init(HAL_UART_t* uart)
{
	UARTConfigSetExpClk((uint32_t)uart->base, SysCtlClockGet(), 
													uart->baudrate, uart->config);

	//UARTFIFODisable((uint32_t)uart->base);
	UARTFIFOEnable((uint32_t)uart->base);
	
	uart->state = UART_READY;
	
	return HAL_OK;
}


HAL_Return_t HAL_UART_TxCallback(HAL_UART_t* uart)
{
	if (uart->txCount)
	{
		txFIFOPut(uart, &uart->txBuffer, &uart->txCount);
	}
	else
	{
		UARTIntDisable((uint32_t)uart->base, UART_INT_TX);
		uart->state &= ~UART_BUSY_TX;
		if (uart->txCompleteCallback)
			uart->txCompleteCallback(uart);
		
	}
	
	return HAL_OK;
}

HAL_Return_t HAL_UART_RxCallback(HAL_UART_t* uart)
{
	rxFIFOGet(uart, &uart->rxBuffer, &uart->rxCount);

	//if the rxCount is zero, there is no more space 
	//left in the buffer, in which case, the UART rx 
	//process should stop, whether it is waiting for 
	//the idle interrupt or not.
	if (uart->rxCount == 0)
	{
		UARTIntDisable((uint32_t)uart->base, UART_INT_RX);
		uart->state &= ~(UART_BUSY_RX | UART_BUSY_RX_IDLE);
		if (uart->state & UART_BUSY_RX && uart->rxCompleteCallback)
			uart->rxCompleteCallback(uart);
		else if(uart->state & UART_BUSY_RX_IDLE && uart->idleCallback)
			uart->idleCallback(uart);
	}
	return HAL_OK;
		
}

HAL_Return_t HAL_UART_IdleCallback(HAL_UART_t* uart)
{
	//The idle interrupt is fired whether we are waiting
	//for a fixed length string or a variable length string.
	
	//For a fixed length string, the idle interrupt may fire 
	//multiple time, (i.e in case of PC UART rx).
	//In such a case, we need to read the fifo into the buffer 
	//and disable the uart interrupt when the specified number of
	//characters have been received.
	
	rxFIFOGet(uart, &uart->rxBuffer, &uart->rxCount);

	
	if (uart->state & UART_BUSY_RX &&
			uart->rxCount == 0)
	{		
	
		UARTIntDisable((uint32_t)uart->base, UART_INT_RX);
		uart->state &= ~UART_BUSY_RX;
		if (uart->rxCompleteCallback)
			uart->rxCompleteCallback(uart);
	
	}
	else if (uart->state & UART_BUSY_RX_IDLE)
	{
		//For variable length string, stop the receive process
		//whenever the idle interrupt is fired.
		//It is possible that the fifo may overflow before the
		//idle interrupt is fired. So, the rx fifo interrupt has
		//to be handled by reading the fifo into the buffer
		//in the HAL_UART_RxCallback()
		UARTIntDisable((uint32_t)uart->base, UART_INT_RX);
		uart->state &= ~UART_BUSY_RX_IDLE;
		if (uart->idleCallback)
			uart->idleCallback(uart);
	}
		
	return HAL_OK;
}

/** @}*/

/**
 * @defgroup hal_uart_Public_Functions	hal_uart Public Functions
 * @{
 */

/**
 *
 * @return
 */
HAL_Return_t HAL_UART0_Init(void)
{

	uart0.baudrate = 115200;
	
	uart0.config = 	UART_CONFIG_PAR_NONE | 
									UART_CONFIG_STOP_ONE | 
									UART_CONFIG_WLEN_8;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIOA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	HAL_UART_Init(&uart0);
	IntEnable(INT_UART0);
	UARTIntEnable((uint32_t)uart0.base, UART_INT_RT);

	return HAL_OK;
}


/**
 *
 * @return
 */
HAL_Return_t HAL_UART1_Init(void)
{
	uart1.baudrate = 9600;
	uart1.config = 	UART_CONFIG_PAR_NONE |
									UART_CONFIG_STOP_ONE | 
									UART_CONFIG_WLEN_8;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);


	GPIOPinTypeUART(GPIOB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	HAL_UART_Init(&uart1);
	IntEnable(INT_UART1);
	//UARTIntEnable((uint32_t)uart1.base, UART_INT_RT);
	
	return HAL_OK;
}


/**
 *
 * @return
 */
HAL_Return_t HAL_UART2_Init(void)
{
	uart2.baudrate = 115200;
	uart2.config = 	UART_CONFIG_PAR_NONE |
									UART_CONFIG_STOP_ONE | 
									UART_CONFIG_WLEN_8;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	//PD7 must be unlocked
	HWREG(GPIOD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIOD_BASE + GPIO_O_CR) = (0x01 << 7);

	GPIOPinConfigure(GPIO_PD6_U2RX);
	GPIOPinConfigure(GPIO_PD7_U2TX);
	GPIOPinTypeUART(GPIOD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	HWREG(GPIOD_BASE + GPIO_O_CR) = 0x00;
	HWREG(GPIOD_BASE + GPIO_O_LOCK) = 0;

	HAL_UART_Init(&uart2);
	IntEnable(INT_UART2);
	UARTIntEnable((uint32_t)uart2.base, UART_INT_RT);
	
	return HAL_OK;
}


/**
 *
 * @param uart
 * @param str
 * @param len
 * @param timeout
 * @return
 */
HAL_Return_t HAL_UART_SendBlocking(HAL_UART_t* uart, uint8_t* str, uint32_t len, uint32_t timeout)
{
	HAL_UART_LOCK(uart);
	///@todo : Add timeout functionality
	uart->state |= UART_BUSY_TX;
	for(int i = 0 ; i < len ; i++)
	{
		UARTCharPut((uint32_t)uart->base, str[i]);
	}
	
	uart->state &= ~UART_BUSY_TX;
	
	HAL_UART_UNLOCK(uart);
	return HAL_OK;
}


/**
 *
 * @param uart
 * @param str
 * @param len
 * @param timeout
 * @return
 */
HAL_Return_t HAL_UART_ReceiveBlocking(HAL_UART_t* uart, uint8_t* str, uint32_t len, uint32_t timeout)
{

	HAL_UART_LOCK(uart);
	uart->state |= UART_BUSY_RX;
	///@todo : add timeout functionality
	for(int i = 0 ; i < len ; i++)
	{
		str[i] = UARTCharGet((uint32_t)uart->base);
	}
	
	uart->state = UART_BUSY_RX;
	HAL_UART_UNLOCK(uart);
	return HAL_OK;
}


HAL_Return_t HAL_UART_Send(HAL_UART_t* uart,
							uint8_t* str,
							uint32_t len,
							TxCompleteCallback txCompleteCallback)
{
	HAL_Return_t ret;
	HAL_UART_LOCK(uart);
	
	if (uart->state & UART_BUSY_TX)
	{
		ret = HAL_BUSY;
		goto error;
	}
	
	uart->state |= UART_BUSY_TX;
	
	uart->txBuffer = str;
	uart->txCount = len;
	uart->txCompleteCallback = txCompleteCallback;

	//The interrupt must be enabled first in the HAL_UARTx_Init() function
	//by calling INTEnable(INT_UARTx).	
	
	UARTTxIntModeSet((uint32_t)uart->base, UART_TXINT_MODE_EOT);
	UARTIntEnable((uint32_t)uart->base, UART_INT_TX);
	txFIFOPut(uart, &uart->txBuffer, &uart->txCount);
		

	ret = HAL_OK;
	error:
	HAL_UART_UNLOCK(uart);
	return ret;
}

HAL_Return_t HAL_UART_Receive(HAL_UART_t* uart,
								uint8_t* str,
								uint32_t len,
								RxCompleteCallback rxCompleteCallback)
{
	HAL_Return_t ret;
	uint32_t txFifoLevel, rxFifoLevel;
	uint8_t tmp[16];
	uint32_t tmpCount = 16;
	
	HAL_UART_LOCK(uart);
	
	if (uart->state & (UART_BUSY_RX | UART_BUSY_RX_IDLE))
	{
		ret = HAL_BUSY;
		goto error;
	}
	
	uart->state |= UART_BUSY_RX;
	
	uart->rxBuffer = str;
	uart->rxCount = len;
	uart->rxCompleteCallback = rxCompleteCallback;
	uart->idleCallback = NULL;
	
	//clear rxFIFO
	rxFIFOGet(uart, (uint8_t**)&tmp, &tmpCount);
	
	UARTFIFOLevelGet((uint32_t)uart->base, &txFifoLevel, &rxFifoLevel);
	UARTFIFOLevelSet((uint32_t)uart->base, txFifoLevel, UART_FIFO_RX7_8);
	UARTIntEnable((uint32_t)uart->base, UART_INT_RX);
	
	ret = HAL_OK;
	error:
	HAL_UART_UNLOCK(uart);
	return ret;
}


HAL_Return_t HAL_UART_ReceiveUntilIdle(HAL_UART_t* uart, 
																				uint8_t* str, 
																				uint32_t len,
																				IdleCallback idleCallback)
{
		
	HAL_Return_t ret;
	uint32_t txFifoLevel, rxFifoLevel;
	
	HAL_UART_LOCK(uart);
	
	if (uart->state & (UART_BUSY_RX | UART_BUSY_RX_IDLE))
	{
		ret = HAL_BUSY;
		goto error;
	}
	
	uart->state |= UART_BUSY_RX_IDLE;
	
	uart->rxBuffer = str;
	uart->rxCount = len;
	uart->rxCompleteCallback = NULL;
	uart->idleCallback = idleCallback;
	
	UARTFIFOLevelGet((uint32_t)uart->base, &txFifoLevel, &rxFifoLevel);
	UARTFIFOLevelSet((uint32_t)uart->base, txFifoLevel, UART_FIFO_RX7_8);
	UARTIntEnable((uint32_t)uart->base, UART_INT_RX);
	
	ret = HAL_OK;
	error:
	HAL_UART_UNLOCK(uart);
	return ret;
	
}


/** @}*/
