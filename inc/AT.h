/*
 * @file 	AT.h
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#ifndef INC_AT_H_
#define INC_AT_H_


typedef enum{

	AT_OK,
	AT_BUSY,
	AT_ERROR

}AT_Return_t;

struct AT_Response_t;
typedef struct AT_Response_t AT_Response_t;

struct AT_t;
typedef struct AT_t AT_t;

#ifdef __cplusplus
extern "C"{
#endif 
 
AT_t* AT_Init(HAL_UART_t* uart);
AT_Return_t AT_sendCommand(AT_t* AT, char* cmd, uint32_t timeout);
char* AT_checkResponse(AT_Response_t* response, char* str);

#ifdef __cplusplus
}
#endif


#endif /* INC_AT_H_ */
