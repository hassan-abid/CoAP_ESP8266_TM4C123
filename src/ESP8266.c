/*
 * @file 	ESP8266.c
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#include "ESP8266.h"
#include "BSP.h"

typedef struct ESP8266_WiFi_t{
	
	uint32_t dummy;
	
}ESP8266_WiFi_t;


ESP8266_WiFi_t esp8266;

/**
 * @defgroup ESP8266_Private_Functions	ESP8266 Private Functions
 * @{
 */



/** @}*/

/**
 * @defgroup ESP8266_Public_Functions	ESP8266 Public Functions
 * @{
 */

ESP8266_Return_t ESP8266_Init(ESP8266_WiFi_t* esp)
{
	ESP8266_Return_t ret;

	ret = ESP8266_Reset(esp);

	return ret;
}


ESP8266_Return_t ESP8266_Reset(ESP8266_WiFi_t* esp)
{
	uint32_t retries = 3;

	while (retries--)
	{
		ESP8266_GPIO_Reset(ESP8266_RST);
		ESP8266_Delay(20);
		ESP8266_GPIO_Set(ESP8266_RST);
	}

	ESP8266_Delay(100);

	return ESP8266_OK;
}


/** @}*/
