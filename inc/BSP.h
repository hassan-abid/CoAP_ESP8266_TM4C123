/*
 * @file 	BSP.h
 * @author	Hassan Abid
 * @date	Apr 2, 2018
 *
 * 
 */

#ifndef INC_BSP_H_
#define INC_BSP_H_

#include <stdbool.h>
#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "driverlib\gpio.h"

#include "hal_defs.h"
#include "hal_gpio.h"

#include "OS.h"

#define ESP8266_RST						GPIOB, GPIO_PIN_5

#define ESP8266_GPIO_Set(pin)				HAL_GPIO_SetPin(pin)
#define ESP8266_GPIO_Reset(pin)			HAL_GPIO_ResetPin(pin)

#define ESP8266_Delay(ms)				OS_Sleep(ms)

#endif /* INC_BSP_H_ */
