// UARTIntsTestMain.c
// Runs on LM4F120/TM4C123
// Tests the UART0 to implement bidirectional data transfer to and from a
// computer running HyperTerminal.  This time, interrupts and FIFOs
// are used.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 5.11 Section 5.6, Program 3.10

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "hal_defs.h"
#include "hal_uart.h"
#include "hal_gpio.h"

#include "AT.h"
#include "ESP8266.h"
#include "OS.h"


#define sizeof_array(arr)		(sizeof(arr)/sizeof(arr[0]))

#define RX_LENGTH			((uint32_t)5)

#define fifoPush(fifo, data)	\
	(fifo)->buff[(fifo)->end++] = data;\
	if ((fifo)->end >= sizeof((fifo)->buff))\
		(fifo)->end = 0;\
	
	
#define FIFO_DEF(name, size, type)	\
	struct{\
		type buff[size];\
		uint32_t count;\
		uint32_t end;\
	}	name = {.count = 0, .end = 0}
		
		
FIFO_DEF(rxPCFIFO, 512, char);
FIFO_DEF(rxESP8266FIFO, 512, char);
	
uint8_t rxStr[64];
uint8_t txStr[64];

uint8_t rxPC, txPC, rxESP8266, txESP8266;


char cmd[64];

static AT_t* AT;

void PC_rxComplete(HAL_UART_t* uart)
{
	txESP8266 = rxPC;
	fifoPush(&rxPCFIFO, rxPC);
	HAL_UART_Send(&uart2, &txESP8266, 1, NULL);
	HAL_UART_Receive(&uart0, &rxPC, 1, PC_rxComplete);
}


void ESP8266_rxComplete(HAL_UART_t* uart)
{
	fifoPush(&rxESP8266FIFO, rxESP8266);
	HAL_UART_Receive(&uart2, &rxESP8266, 1, ESP8266_rxComplete);
}

void ESP8266_txComplete(HAL_UART_t* uart)
{
	
}

void blink(void)
{
	  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    GPIOPinTypeGPIOOutput(GPIOF_BASE, GPIO_PIN_3);
     
		while(1)
		{
			GPIOPinWrite(GPIOF_BASE, GPIO_PIN_3, GPIO_PIN_3);

			OS_Sleep(1000);
			
			GPIOPinWrite(GPIOF_BASE, GPIO_PIN_3, 0x0);
			
			OS_Sleep(1000);
		}

	
}


extern ESP8266_WiFi_t esp8266;
void wifiThread(void)
{
	static char cmd[] = "AT\r\n";
	ESP8266_Init(&esp8266);
	while(1)
	{
		OS_Sleep(1000);
		HAL_UART_SendBlocking(&uart2, cmd, strlen(cmd), 1000);
	}
	
}


void transparentUART(void)
{
	
	while(1)
	{
		gets(cmd);
	}
}
//debug code
int main(void){

	//configure system clock
	SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 80e6);
	
	OS_Init();
	OS_AddThread(&blink, "Blink", OS_TASK_PRIORITY_HIGH);
	OS_AddThread(&wifiThread, "WiFi", OS_TASK_PRIORITY_HIGH);
	
	HAL_GPIO_Init();
	HAL_UART0_Init();
	HAL_UART2_Init();
	
	
	
	HAL_UART_Receive(&uart2, &rxESP8266, 1, &ESP8266_rxComplete);
	
	
	OS_Start(1000);
	
	for(;;){}

}
