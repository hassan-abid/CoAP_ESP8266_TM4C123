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

#include "hal_defs.h"
#include "hal_uart.h"



#define __HAL_UART_DEFINE(index, ...) \
	HAL_UART_t uart##index = {.base = UART##index, \
							};\






HAL_UART_LIST(__HAL_UART_DEFINE)


/**
 * @defgroup hal_uart_Private_Functions	hal_uart Private Functions
 * @{
 */


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

	UARTConfigSetExpClk((uint32_t)uart0.base, SysCtlClockGet(), uart0.baudrate, uart0.config);

	return HAL_OK;
}

/**
 *
 * @return
 */
HAL_Return_t HAL_UART1_Init(void)
{
	uart1.baudrate = 115200;
	uart1.config = 	UART_CONFIG_PAR_NONE | 
									UART_CONFIG_STOP_ONE | 
									UART_CONFIG_WLEN_8;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIOB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk((uint32_t)uart1.base, SysCtlClockGet(), uart1.baudrate, uart1.config);

	return HAL_OK;
}



HAL_Return_t HAL_UART_Send(HAL_UART_t* uart, char* str, uint32_t len)
{
	for(int i = 0 ; i < len ; i++)
	{
		UARTCharPut((uint32_t)uart->base, str[i]);
	}

	return HAL_OK;
}

/** @}*/
