/*
 * @file 	AT.c
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


#include "hal_uart.h"
#include "hal_uart_private.h"
#include "AT.h"

#include "OS.h"


#define sizeof_array( arr )					(sizeof(arr) / sizeof(arr[0]))

typedef struct AT_Response_t{
	
	char str[512];
	uint32_t count;
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


void AT_uartRxCompleteCallback(HAL_UART_t* uart, uint32_t bytesReceived)
{
	OS_Signal(&((AT_t*)uart->parent)->rxComplete);
	((AT_t*)uart->parent)->response.count = bytesReceived;
	((AT_t*)uart->parent)->response.str[bytesReceived] = '\0';
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

AT_Return_t AT_waitResponse(AT_t* AT, uint32_t timeout)
{
	HAL_ASSERT_RETURN(
			HAL_OK != HAL_UART_ReceiveUntilIdle(AT->uart, (uint8_t*)AT->response.str, sizeof(AT->response.str), AT_uartRxCompleteCallback),
			AT_ERROR);
	
	OS_Sleep(40);
	OS_Wait(&AT->rxComplete);
	
	AT->response.argv[0] = AT->response.str;
	AT->response.argc = 1;
	return AT_OK;
}

AT_Return_t AT_sendRaw(AT_t* AT, uint32_t timeout, uint8_t* data, uint32_t length)
{

	HAL_ASSERT_RETURN(
			HAL_OK != HAL_UART_Send(AT->uart, data, length, NULL),
			AT_ERROR);

	AT_waitResponse(AT, timeout);
	AT_splitLines(&AT->response);
	
	HAL_ASSERT_RETURN(
			NULL == AT_checkResponse(&AT->response, "OK"),
			AT_ERROR);

	
			
	return AT_OK;
}

inline AT_Return_t AT_sendCommand(AT_t* AT, uint32_t timeout, char* cmd)
{
	return AT_sendRaw(AT, timeout, cmd, strlen(cmd));
}

AT_Return_t AT_sendCommandf(AT_t* AT, uint32_t timeout, char* cmd, ...)
{
		//process the formatted string
	static char buff[512 + 1];	//buffer to store the final string to be sent.
	__va_list args;
	va_start(args, cmd);
	vsnprintf(buff, sizeof(buff) - 1, cmd, args);
	va_end(args);
	
	return AT_sendCommand(AT, timeout, buff);
}

char* AT_checkResponse(AT_Response_t* response, char* str)
{
	uint32_t index = 0;
	char* responseStr = NULL;

	for(index = 0; index < response->argc; index++)
	{
		responseStr = strstr(response->argv[index],  (char*)str);
		if (responseStr != NULL)
		{
			response->iter = index;
			break;
		}
	}
	return responseStr;
}

AT_Response_t* AT_getResponseStruct(AT_t* AT)
{
	return &AT->response;
}

void AT_splitLines(AT_Response_t* response)
{
	response->argc = split(response->str, response->argv, sizeof_array(response->argv), "\r\n");
	response->iter = 0;
}

/** @}*/
