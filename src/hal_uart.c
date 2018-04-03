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

HAL_Return_t HAL_UART_Init(HAL_UART_t* uart)
{
	UARTConfigSetExpClk((uint32_t)uart->base, SysCtlClockGet(), 
													uart->baudrate, uart->config);

	UARTFIFODisable((uint32_t)uart->base);
	
	return HAL_OK;
}


HAL_Return_t HAL_UART_TxCallback(HAL_UART_t* uart)
{
	if (uart->txCount)
	{
		uart->txCount--;
		uart->txBuffer++;
		UARTCharPutNonBlocking((uint32_t)uart->base, *(uart->txBuffer));
	}
	else
	{
		UARTIntDisable((uint32_t)uart->base, UART_INT_TX);
		if (uart->txCompleteCallback)
			uart->txCompleteCallback(uart);
	}
	
	return HAL_OK;
}

HAL_Return_t HAL_UART_RxCallback(HAL_UART_t* uart)
{
	if (uart->rxCount)
	{
		uart->rxCount--;
		*uart->rxBuffer = UARTCharGetNonBlocking((uint32_t)uart->base);
		uart->rxBuffer++;
	}
	
	if (uart->rxCount == 0)
	{
		UARTIntDisable((uint32_t)uart->base, UART_INT_RX);
		if (uart->rxCompleteCallback)
			uart->rxCompleteCallback(uart);
	}
	
	return HAL_OK;
		
}

HAL_Return_t HAL_UART_IdleCallback(HAL_UART_t* uart)
{
	//if the idle interrupt is enabled,
	//we are expecting variable length data, 
	//in which case, call the rxCompleteCallback
	//when this interrupt is fired.
	
	UARTIntDisable((uint32_t)uart->base, UART_INT_RX);
	uart->rxCompleteCallback(uart);
		
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
	///@todo : Add timeout functionality
	for(int i = 0 ; i < len ; i++)
	{
		UARTCharPut((uint32_t)uart->base, str[i]);
	}
	
	
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
	///@todo : add timeout functionality
	for(int i = 0 ; i < len ; i++)
	{
		str[i] = UARTCharGet((uint32_t)uart->base);
	}
	
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
	
	if (uart->txCount)
	{
		ret = HAL_BUSY;
		goto error;
	}
	
	uart->txBuffer = str;
	uart->txCount = len;
	uart->txCompleteCallback = txCompleteCallback;

	//The interrupt must be enabled first in the HAL_UARTx_Init() function
	//by calling INTEnable(INT_UARTx).	
	
	UARTIntEnable((uint32_t)uart->base, UART_INT_TX);
	UARTCharPutNonBlocking((uint32_t)uart->base, *uart->txBuffer);
		

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
	HAL_UART_LOCK(uart);
	
	if (uart->rxCount)
	{
		ret = HAL_BUSY;
		goto error;
	}
	
	uart->rxBuffer = str;
	uart->rxCount = len;
	uart->rxCompleteCallback = rxCompleteCallback;
	
	UARTIntEnable((uint32_t)uart->base, UART_INT_RX);
	
	ret = HAL_OK;
	error:
	HAL_UART_UNLOCK(uart);
	return ret;
}


/** @}*/
