/*
 * @file 	retarget.c
 * @author	Hassan Abid
 * @date	Jan 30, 2018
 *
 * 
 */

#include <stdio.h>
#include <stdint.h>

#include "hal_uart.h"

/**
 * @defgroup retarget_Private_Functions	retarget Private Functions
 * @{
 */


/** @}*/

/**
 * @defgroup retarget.c_Public_Functions	retarget.c Public Functions
 * @{
 */
int fputc(int ch, FILE *f) 
{

	//UART_OutChar(ch);
	HAL_UART_SendBlocking(&uart0, (uint8_t*)&ch, 1, 1000);
	return (1);

}

int fgetc (FILE *f) {

	uint8_t ch;
	HAL_UART_ReceiveBlocking(&uart0, &ch, 1, 1000); 
	if (ch == '\r')
		ch = '\n';
	return ch;

}

int ferror(FILE *f) {
 /* Your implementation of ferror */
	return EOF;
}

/** @}*/
