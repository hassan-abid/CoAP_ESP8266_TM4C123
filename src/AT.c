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


typedef struct AT_Response_t{
	
	char str[512];
	char* argv[10];
	uint32_t argc;
	uint32_t iter;
	
}AT_Response_t;


typedef struct AT_t{

	HAL_UART_t* uart;

	OS_Semaphore_t rxComplete;

	AT_Response_t response;

}AT_t;


static AT_t AT;

/**
 * @defgroup AT_Private_Functions	AT Private Functions
 * @{
 */

uint16_t split(char *buffer, char **_argv, uint32_t _argc, const char *tokens);


void AT_uartRxCompleteCallback(HAL_UART_t* uart)
{
	OS_Signal(&((AT_t*)uart->parent)->rxComplete);
}

static inline void AT_waitResponse(AT_t* AT, uint32_t timeout)
{
	OS_Wait(&AT->rxComplete);

	AT->response.argc = split(AT->response.str, AT->response.argv, sizeof(AT->response.argv), "\r\n");
	AT->response.iter = 0;
}

uint16_t split(char *buffer, char **_argv, uint32_t _argc, const char *tokens)
{
    uint16_t i;
    uint16_t counter = 0;

    char *result = strtok( buffer, tokens );

    for( i=0; i < _argc; i++ )
    {
        if ( result )
            counter++;

        _argv[ i ] = result;
        result = strtok( NULL, tokens );
    }
    return counter;
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

	return &AT;
}


AT_Return_t AT_sendCommand(AT_t* AT, char* cmd, uint32_t timeout)
{
	HAL_Return_t hal_ret;
	uint32_t endTime = 0;

	HAL_ASSERT_RETURN(
			HAL_OK != HAL_UART_Send(AT->uart, (uint8_t*)cmd, strlen(cmd), NULL),
			AT_ERROR);

	HAL_ASSERT_RETURN(
			HAL_OK != HAL_UART_ReceiveUntilIdle(AT->uart, (uint8_t*)AT->response.str, sizeof(AT->response.str), AT_uartRxCompleteCallback),
			AT_ERROR);

	AT_waitResponse(AT, timeout);

	HAL_ASSERT_RETURN(
			NULL == AT_checkResponse(&AT->response, "OK"),
			AT_ERROR);

	return AT_OK;
}

char* AT_checkResponse(AT_Response_t* response, char* str)
{
	uint32_t index = 0;
	char* responseStr = NULL;

	for(index = 0; index < response->argc; index++)
	{
		if (strstr(response->argv[index],  (char*)str) != NULL)
		{
			responseStr = response->argv[index];
			response->iter = index;
			break;
		}
	}
	return responseStr;
}

/** @}*/
