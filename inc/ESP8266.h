/*
 * @file 	ESP8266.h
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_

typedef enum{

	ESP8266_OK = 0,
	ESP8266_BUSY,
	ESP8266_ERROR

}ESP8266_Return_t;


struct ESP8266_WiFi_t;
typedef struct ESP8266_WiFi_t ESP8266_WiFi_t;


#ifdef __cplusplus
extern "C"{
#endif 
 

ESP8266_Return_t ESP8266_Init(ESP8266_WiFi_t* esp);
ESP8266_Return_t ESP8266_Reset(ESP8266_WiFi_t* esp);

ESP8266_Return_t ESP8266_sendData(ESP8266_WiFi_t* esp, uint8_t linkID,
																			uint8_t* data, uint32_t length);
	
#ifdef __cplusplus
}
#endif


#endif /* INC_ESP8266_H_ */
