/*
 * @file 	hal_gpio.c
 * @author	Hassan Abid
 * @date	Mar 2, 2018
 *
 * 
 */

#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "hal_defs.h"
#include "hal_gpio.h"



/**
 * @defgroup hal_gpio_Private_Functions	hal_gpio Private Functions
 * @{
 */


/** @}*/

/**
 * @defgroup hal_gpio_Public_Functions	hal_gpio Public Functions
 * @{
 */

HAL_Return_t HAL_GPIO_Init()
{
	//initialize LEDs.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIOF_BASE, GPIO_PIN_3);
	
	//Initialize ESP8266 GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIOB_BASE, GPIO_PIN_5);
	return HAL_OK;
}


HAL_Return_t HAL_GPIO_PinSet(HAL_GPIO_Port_t port, HAL_GPIO_Pins_t pins)
{
	GPIOPinWrite((uint32_t)port, pins, pins);
	return HAL_OK;
}


HAL_Return_t HAL_GPIO_PinReset(HAL_GPIO_Port_t port, HAL_GPIO_Pins_t pins)
{
	GPIOPinWrite((uint32_t)port, pins, 0);
	return HAL_OK;
}

HAL_Return_t HAL_GPIO_PinWrite(HAL_GPIO_Port_t port, HAL_GPIO_Pins_t pins, uint8_t state)
{
	if (state)
		HAL_GPIO_PinSet(port, pins);
	else
		HAL_GPIO_PinReset(port, pins);
}

/** @}*/
