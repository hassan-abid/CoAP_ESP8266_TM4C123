/*
 * @file 	hal_uart.h
 * @author	Hassan Abid
 * @date	Mar 2, 2018
 *
 * 
 */

#ifndef INC_HAL_UART_H_
#define INC_HAL_UART_H_

#include "tm4c123gh6pm.h"

struct HAL_UART_t;
typedef struct HAL_UART_t HAL_UART_t;

#define HAL_UART_LIST(UART_FUNC, ...) \
\
	UART_FUNC(0, __VA_ARGS__)\
	UART_FUNC(1, __VA_ARGS__)\


#define __HAL_UART_EXPORT(index, ...) \
	extern HAL_UART_t uart##index;

HAL_UART_LIST(__HAL_UART_EXPORT);



typedef struct HAL_UART_t{

	UART0_Type* const base;


	uint32_t baudrate;
	
	union
	{
		struct
		{
			uint32_t __UNUSED0 	: 1;
			uint32_t parity   	: 2;
			uint32_t stopBits   : 1;
			uint32_t __UNUSED1	: 1;
			uint32_t wordLength : 2;

		};
		
		uint32_t config;
	};
	
}HAL_UART_t;


#ifdef __cplusplus
extern "C"{
#endif 
 
HAL_Return_t HAL_UART0_Init(void);

HAL_Return_t HAL_UART1_Init(void);

HAL_Return_t HAL_UART_Send(HAL_UART_t* uart, char* str, uint32_t len);

HAL_Return_t HAL_UART_SendBlocking(HAL_UART_t* uart, char* str, uint32_t len, uint32_t timeout);

HAL_Return_t HAL_UART_Receive(HAL_UART_t* uart, char* str, uint32_t len);

HAL_Return_t HAL_UART_ReceiveBlocking(HAL_UART_t* uart, char* str, uint32_t len, uint32_t timeout);


#ifdef __cplusplus
}
#endif


#endif /* INC_HAL_UART_H_ */
