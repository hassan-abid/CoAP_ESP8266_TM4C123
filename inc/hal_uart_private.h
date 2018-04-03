/*
 * @file 	hal_uart_private.h
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#ifndef INC_HAL_UART_PRIVATE_H_
#define INC_HAL_UART_PRIVATE_H_




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

	uint8_t* txBuffer;
	uint32_t txCount;

	uint8_t* rxBuffer;
	uint32_t rxCount;

	RxCompleteCallback rxCompleteCallback;
	TxCompleteCallback txCompleteCallback;

	void* parent;

}HAL_UART_t;



#endif /* INC_HAL_UART_PRIVATE_H_ */
