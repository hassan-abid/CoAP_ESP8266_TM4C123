/*
 * @file 	OS_Config.h
 * @author	Hassan Abid
 * @date	Mar 1, 2018
 *
 * 
 */

#ifndef INC_OS_CONFIG_H_
#define INC_OS_CONFIG_H_


/**
 * @defgroup	OS_Configuration		OS Configuration
 * @{
 */


#define OS_MAX_NUM_TASKS					((uint32_t)10)
#define OS_STACK_SIZE							((uint32_t)512)
#define	OS_USE_PREEMPTION						1
#define OS_USE_PRIORITY_SCHEDULER		1

/** @} */


#endif /* INC_OS_CONFIG_H_ */
