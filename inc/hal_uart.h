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
#include "hal_defs.h"

struct HAL_UART_t;
typedef struct HAL_UART_t HAL_UART_t;

#define HAL_UART_LIST(UART_FUNC, ...) \
\
	UART_FUNC(0, __VA_ARGS__)\
	UART_FUNC(1, __VA_ARGS__)\
	UART_FUNC(2, __VA_ARGS__)\


#define __HAL_UART_EXPORT(index, ...) \
	extern HAL_UART_t uart##index;

HAL_UART_LIST(__HAL_UART_EXPORT);


typedef void (*TxCompleteCallback)(HAL_UART_t* uart);
typedef void (*RxCompleteCallback)(HAL_UART_t* uart);



#ifdef __cplusplus
extern "C"{
#endif 
 
HAL_Return_t HAL_UART0_Init(void);

HAL_Return_t HAL_UART1_Init(void);
	
HAL_Return_t HAL_UART2_Init(void);

	
	
HAL_Return_t HAL_UART_Send(HAL_UART_t* uart,
							uint8_t* str,
							uint32_t len,
							TxCompleteCallback txCompleteCallback);

HAL_Return_t HAL_UART_SendBlocking(HAL_UART_t* uart, uint8_t* str, uint32_t len, uint32_t timeout);

HAL_Return_t HAL_UART_Receive(HAL_UART_t* uart,
								uint8_t* str,
								uint32_t len,
								RxCompleteCallback rxCompleteCallback);


HAL_Return_t HAL_UART_ReceiveBlocking(HAL_UART_t* uart, uint8_t* str, uint32_t len, uint32_t timeout);


#ifdef __cplusplus
}
#endif


#endif /* INC_HAL_UART_H_ */
