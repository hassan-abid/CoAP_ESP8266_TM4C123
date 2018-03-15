/*
 * @file 	hal_gpio.c
 * @author	Hassan Abid
 * @date	Mar 2, 2018
 *
 * 
 */

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "hal_gpio.h"
#include "hal_defs.h"


/**
 * @defgroup hal_gpio_Private_Functions	hal_gpio Private Functions
 * @{
 */


/** @}*/

/**
 * @defgroup hal_gpio_Public_Functions	hal_gpio Public Functions
 * @{
 */

HAL_Return_t HAL_GPIO_Init(HAL_GPIO_Port_t* port, HAL_GPIO_Pins_t pins)
{

	return HAL_OK;
}


HAL_Return_t HAL_GPIO_SetPin(HAL_GPIO_Port_t* port, HAL_GPIO_Pins_t pins)
{
	GPIOPinWrite(port, pins, 1);
	return HAL_OK;
}


HAL_Return_t HAL_GPIO_ResetPin(HAL_GPIO_Port_t* port, HAL_GPIO_Pins_t pins)
{
	GPIOPinWrite(port, pins, 0);
	return HAL_OK;
}


/** @}*/
