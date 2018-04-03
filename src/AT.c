/*
 * @file 	AT.c
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#include <stdint.h>
#include <string.h>

#include "hal_uart.h"
#include "hal_uart_private.h"
#include "AT.h"

#include "OS.h"



typedef struct AT_t{

	HAL_UART_t* uart;

	OS_Semaphore_t rxComplete;

	uint8_t rxBuffer[512];

}AT_t;


static AT_t AT;

/**
 * @defgroup AT_Private_Functions	AT Private Functions
 * @{
 */

void AT_uartRxCompleteCallback(HAL_UART_t* uart)
{
	OS_Signal(&((AT_t*)uart->parent)->rxComplete);
}

static inline void AT_waitResponse(AT_t* AT, uint32_t timeout)
{
	OS_Wait(&AT->rxComplete);
}

/** @}*/

/**
 * @defgroup AT_Public_Functions	AT Public Functions
 * @{
 */

AT_t* AT_Init(HAL_UART_t* uart)
{
	AT.uart = uart;
	AT.uart->parent = &AT;

	OS_InitSemaphore(&AT.rxComplete, 0);

	return AT_OK;
}


AT_Return_t AT_sendCommand(AT_t* AT, char* cmd, uint32_t timeout)
{
	HAL_Return_t hal_ret;
	uint32_t endTime = 0;

	HAL_ASSERT_RETURN(
			HAL_OK != HAL_UART_Send(AT->uart, (uint8_t*)cmd, strlen(cmd), NULL),
			AT_ERROR);

	HAL_ASSERT_RETURN(
			HAL_OK != HAL_UART_Receive(AT->uart, AT->rxBuffer, sizeof(AT->rxBuffer), AT_uartRxCompleteCallback),
			AT_ERROR);

	AT_waitResponse(AT, timeout);

	return AT_OK;
}


/** @}*/
