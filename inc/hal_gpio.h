/*
 * @file 	hal_gpio.h
 * @author	Hassan Abid
 * @date	Mar 2, 2018
 *
 * 
 */

#ifndef INC_HAL_GPIO_H_
#define INC_HAL_GPIO_H_

typedef GPIOA_Type* HAL_GPIO_Port_t;
typedef uint32_t   HAL_GPIO_Pins_t;
typedef uint32_t   HAL_GPIO_AF_t;

typedef struct{

	HAL_GPIO_Port_t port;
	HAL_GPIO_Pins_t pins;

}HAL_GPIO_t;

#ifdef __cplusplus
extern "C"{
#endif 
 
HAL_Return_t HAL_GPIO_Init(void);

HAL_Return_t HAL_GPIO_PinSet(HAL_GPIO_Port_t port, HAL_GPIO_Pins_t pin);

HAL_Return_t HAL_GPIO_PinReset(HAL_GPIO_Port_t port, HAL_GPIO_Pins_t pin);

HAL_Return_t HAL_GPIO_PinWrite(HAL_GPIO_Port_t port, HAL_GPIO_Pins_t pins, uint8_t state);
#ifdef __cplusplus
}
#endif


#endif /* INC_HAL_GPIO_H_ */
