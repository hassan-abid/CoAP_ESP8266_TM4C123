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
#include "OS.h"


#include "ESP8266.h"
#include "coap.h"
#include "CAN_Msg_Defs.h"
#include "can0.h"

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
uint8_t PC_txComplete = 0;

char cmd[64];
	
OS_FIFO_Def(CAN_TxFifo, CAN_Message_t, 64);
OS_FIFO_Def(CAN_RxFifo, CAN_Message_t, 64);


uint8_t scratch_raw[1024];
coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};
	
	
ESP8266_Return_t ESP8266_IPDCallback(ESP8266_WiFi_t* esp, uint32_t linkID, uint8_t* data, uint32_t length)
{
	int rc;
	coap_packet_t pkt, rsppkt;
	size_t rsplen;
	
	coap_dump(data, length, true);
	printf("\r\n");
	if (0 != (rc = coap_parse(&pkt, data, length)))
            printf("Bad packet rc=%d\n", rc);
	else
	{
		coap_dumpPacket(&pkt);
	
		coap_handle_req(&scratch_buf, &pkt, &rsppkt);
		rsplen = 1024;
		
		if (0 != (rc = coap_build(data, &rsplen, &rsppkt)))
				printf("coap_build failed rc=%d\n", rc);
		else
		{
				printf("Sending: ");
				coap_dump(data, rsplen, true);
				printf("\n");
				coap_dumpPacket(&rsppkt);

				//sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
				ESP8266_sendData(esp, linkID, data, rsplen);
		}
	}
	return ESP8266_OK;
}

uint32_t put_light_Callback(uint8_t state)
{
	if (state == 2)
	{
		HAL_GPIO_PinToggle(GPIOF, GPIO_PIN_3);
	}
	else
		HAL_GPIO_PinWrite(GPIOF, GPIO_PIN_3, state);
	
	return HAL_GPIO_PinRead(GPIOF, GPIO_PIN_3);
}

void put_robot_motor_Callback(int32_t speed)
{
	static CAN_Message_t msg;
	msg.u16Id = CAN_MSG_SET_DIRECTION;
	msg.i32Msg = speed;
	OS_FIFO_Put(&CAN_TxFifo, &msg);
	
}

void put_robot_servo_Callback(int32_t angle)
{
	static CAN_Message_t msg;
	msg.u16Id = CAN_MSG_SET_ANGLE;
	msg.i32Msg = angle;
	OS_FIFO_Put(&CAN_TxFifo, &msg);
}




extern ESP8266_WiFi_t esp8266;
void wifiThread(void)
{
	coap_setup();
	ESP8266_Init(&esp8266);
	
	
	while(1)
	{
		OS_Sleep(1000);
		//HAL_UART_SendBlocking(&uart2, cmd, strlen(cmd), 1000);
	}
	
}


void transparentUART(void)
{
	
	while(1)
	{
		gets(cmd);
	}
}



void CAN_TxTask(void)
{
	CAN_Message_t txMsg;
	for(;;)
	{
		OS_FIFO_Get(&CAN_TxFifo, &txMsg);
		CAN0_SendData(txMsg.u16Id, txMsg.u8Msg);
		HAL_GPIO_PinToggle(GPIOF, GPIO_PIN_3);
	}
}


void CAN_RxCallback(uint16_t id, uint8_t* data)
{
	CAN_Message_t msg;
	msg.u16Id = id;
	memcpy(msg.u8Msg, data, 4);
	OS_FIFO_Put(&CAN_RxFifo, &msg);
}


void blink(void)
{
		CAN_Message_t txMsg;
		int32_t duty;
	
		while(1)
		{
			OS_Sleep(1000);
			txMsg.u16Id = CAN_MSG_SET_DIRECTION;
			txMsg.i32Msg = duty;
			OS_FIFO_Put(&CAN_TxFifo, &txMsg);
		duty += 1000;
		if (duty > 11000)
			duty = -12000;
//			//printf("Hello World\r\n");
//			GPIOPinWrite(GPIOF_BASE, GPIO_PIN_3, GPIO_PIN_3);

//			OS_Sleep(1000);
//			
//			GPIOPinWrite(GPIOF_BASE, GPIO_PIN_3, 0x0);
//			
//			OS_Sleep(1000);
		}

	
}

//debug code
int main(void){

	//configure system clock
	SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 80e6);
	
	OS_Init();
	
	OS_FIFO_Init(&CAN_RxFifo);
	OS_FIFO_Init(&CAN_TxFifo);
	
	//OS_AddThread(&blink, "Blink", OS_TASK_PRIORITY_HIGH);
	OS_AddThread(&wifiThread, "WiFi", OS_TASK_PRIORITY_HIGH);
	OS_AddThread(CAN_TxTask, "CAN Tx",  OS_TASK_PRIORITY_REALTIME);
	
	
	HAL_GPIO_Init();
	HAL_UART0_Init();
	HAL_UART2_Init();	
	CAN0_Open();
	//HAL_UART_Receive(&uart0, rxStr, RX_LENGTH, PC_rxCompleteCallback);
	
	
	OS_Start(1000);
	
	for(;;){}

}
