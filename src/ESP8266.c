/*
 * @file 	ESP8266.c
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "ESP8266.h"
#include "BSP.h"
#include "AT.h"

#ifdef ESP8266_DEBUG

#define printd(...)	\
	printf(__VA_ARGS__)

#else
#define	printd(...)	
#endif

#define AT_DELAY_DEFAULT			((uint32_t)100)


typedef enum {
	
	//arguments for AT+CWMODE command
	
	ESP8266_WIFI_MODE_STATION = 1,
	ESP8266_WIFI_MODE_SOFTAP = 2,
	ESP8266_WIFI_MODE_SOFTAP_STATION = 3,
	
}ESP8266_WiFi_Mode_t;

typedef enum{
	
	ES8266_ENCRYPTION_OPEN = 0,
	ESP8266_ENCRYPTION_WPA_PSK = 2,
	ESP8266_ENCRYPTION_WPA2_PSK = 3,
	ESP8266_ENCRYPTION_WPA_WPA2_PSK = 4,
	
}ESP8266_Encryption_t;

typedef struct{
	
	char* ssid;
	char* password;
	uint8_t channelID;
	ESP8266_Encryption_t encryption;
	
}ESP8266_APConfig_t;

typedef struct ESP8266_WiFi_t{
	
	uint32_t dummy;
	AT_t* AT;
	char IP[16];
	
}ESP8266_WiFi_t;



ESP8266_WiFi_t esp8266;

/**
 * @defgroup ESP8266_Private_Functions	ESP8266 Private Functions
 * @{
 */


ESP8266_Return_t ESP8266_setWiFiMode(ESP8266_WiFi_t* esp, 
																			ESP8266_WiFi_Mode_t wifiMode)
{
	HAL_ASSERT_RETURN(
		AT_OK != AT_sendCommandf(esp->AT, 
														AT_DELAY_DEFAULT, 
														"AT+CWMODE=%d\r\n",
														wifiMode),
		ESP8266_ERROR);
	
	return ESP8266_OK;
}

ESP8266_Return_t ESP8266_JoinAccessPoint(ESP8266_WiFi_t* esp, 
																					char* ssid,
																					char* password)
{
	HAL_ASSERT_RETURN(
			AT_OK != AT_sendCommandf(esp->AT, 
																AT_DELAY_DEFAULT,
																"AT+CWJAP=\"%s\",\"%s\"\r\n", 
																ssid, password),
		ESP8266_ERROR);
															
	return ESP8266_OK;
}

ESP8266_Return_t ESP8266_getIP(ESP8266_WiFi_t* esp, char* IP)
{
	char* rsp;
	
	HAL_ASSERT_RETURN(
		AT_OK != AT_sendCommand(esp->AT, 
															AT_DELAY_DEFAULT,
															"AT+CIFSR\r\n"),
	ESP8266_ERROR);
	
	rsp = AT_checkResponse(AT_getResponseStruct(esp->AT), "+CIFSR:APIP");
	HAL_ASSERT_RETURN(rsp == NULL, ESP8266_ERROR);
	
	sscanf(rsp, "+CIFSR:APIP,\"%[^\"]\"", IP);
	printd("Access Point IP : %s\r\n", IP);
	return ESP8266_OK;
	
}

ESP8266_Return_t ESP8266_configureAP(ESP8266_WiFi_t* esp, ESP8266_APConfig_t* config)
{
	
	HAL_ASSERT_RETURN(
		AT_OK != AT_sendCommandf(esp->AT, 
															AT_DELAY_DEFAULT,
															"AT+CWSAP_CUR=\"%s\",\"%s\",%u,%u\r\n", 
															config->ssid, 
															config->password,
															config->channelID,
															config->encryption),
	ESP8266_ERROR);
															
	return ESP8266_OK;
	
}

ESP8266_Return_t ESP8266_setMultipleConnections(ESP8266_WiFi_t* esp, uint8_t enable)
{
	if (enable != 0) enable = 1;
	HAL_ASSERT_RETURN(
		AT_OK != AT_sendCommandf(esp->AT, 
															AT_DELAY_DEFAULT,
															"AT+CIPMUX=%u\r\n", 
															enable),
	ESP8266_ERROR);
															
	return ESP8266_OK;
}

ESP8266_Return_t ESP8266_Server(ESP8266_WiFi_t* esp, uint8_t start, uint32_t port)
{
	if (start != 0) start = 1;
	
	if (port)
	{
		HAL_ASSERT_RETURN(
			AT_OK != AT_sendCommandf(esp->AT, 
																AT_DELAY_DEFAULT,
																"AT+CIPSERVER=%u,%u\r\n", 
																start, port),
		ESP8266_ERROR);
	}
	else
	{
		HAL_ASSERT_RETURN(
			AT_OK != AT_sendCommandf(esp->AT, 
																AT_DELAY_DEFAULT,
																"AT+CIPSERVER=%u\r\n", 
																start),
		ESP8266_ERROR);
	}
															
	return ESP8266_OK;
}

ESP8266_Return_t ESP8266_startUDPServer(ESP8266_WiFi_t* esp, uint32_t port)
{
	ESP8266_Return_t ret;
	
	//enable multiple tcp/ip connections
	ret = ESP8266_setMultipleConnections(esp, true);
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);
	
	
	//start udp server
	ret = ESP8266_Server(esp, true, port);
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);

	HAL_ASSERT_RETURN(
	AT_OK != AT_sendCommandf(esp->AT, 
														AT_DELAY_DEFAULT,
														"AT+CIPSTART=0,\"UDP\",\"0.0.0.0\",%u,%u,2\r\n", 
														port, port),
	ESP8266_ERROR);
	
	return ret;
}

/** @}*/

/**
 * @defgroup ESP8266_Public_Functions	ESP8266 Public Functions
 * @{
 */

ESP8266_Return_t ESP8266_Init(ESP8266_WiFi_t* esp)
{
	ESP8266_Return_t ret;
	ESP8266_APConfig_t apConfig;
	
	esp->AT = AT_Init(&uart2);
	
	ret = ESP8266_Reset(esp);
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);
	
	//Configure Wifi as access point and station
	ret = ESP8266_setWiFiMode(esp, ESP8266_WIFI_MODE_SOFTAP_STATION);
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);
	
	
	//configure access point and start wifi station
	apConfig.ssid = "ESP8266";
	apConfig.password = "redhat@123";
	apConfig.channelID = 1;
	apConfig.encryption = ESP8266_ENCRYPTION_WPA_WPA2_PSK;
	
	ret = ESP8266_configureAP(esp, &apConfig);
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);
	
	//Get IP address
	ret = ESP8266_getIP(esp, esp->IP);
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);
	

	ret = ESP8266_startUDPServer(esp, 5683);	
	HAL_ASSERT_RETURN(ret != ESP8266_OK, ret);
	
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
