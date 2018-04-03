/*
 * @file 	hal_uart_int.c
 * @author	Hassan Abid
 * @date	Mar 28, 2018
 *
 * 
 */

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/uart.h"
#include "hal_uart.h"
#include "hal_uart_private.h"



/**
 * @defgroup hal_uart_int_Private_Functions	hal_uart_int Private Functions
 * @{
 */

void HAL_UART_TxCallback(HAL_UART_t* uart);
void HAL_UART_RxCallback(HAL_UART_t* uart);
void HAL_UART_IdleCallback(HAL_UART_t* uart);

static inline void UART_Handler(HAL_UART_t* uart)
{
	uint32_t irqStatus;

	irqStatus = UARTIntStatus((uint32_t)uart->base, true);

	if (irqStatus & UART_INT_TX)
	{
		UARTIntClear((uint32_t)uart->base, UART_INT_TX);
		HAL_UART_TxCallback(uart);
	}
	
	if (irqStatus & UART_INT_RX)
	{
		UARTIntClear((uint32_t)uart->base, UART_INT_RX);
		HAL_UART_RxCallback(uart);
	}
	
	if (irqStatus & UART_INT_RT)
	{
		UARTIntClear((uint32_t)uart->base, UART_INT_RT);
		HAL_UART_IdleCallback(uart);
	}

}

/** @}*/

/**
 * @defgroup hal_uart_int_Public_Functions	hal_uart_int Public Functions
 * @{
 */



void UART0_Handler(void)
{	
	UART_Handler(&uart0);
}

void UART1_Handler(void)
{
	UART_Handler(&uart1);
}

void UART2_Handler(void)
{
	UART_Handler(&uart2);
}


/** @}*/
