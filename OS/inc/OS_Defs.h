/*
 * @file 	OS_Defs.h
 * @author	Hassan Abid
 * @date	Feb 8, 2018
 *
 * 
 */

#ifndef INC_OS_DEFS_H_
#define INC_OS_DEFS_H_

typedef enum OS_Task_Priority_t{

	OS_TASK_PRIORITY_IDLE,
	OS_TASK_PRIORITY_LOW,
	OS_TASK_PRIORITY_MEDIUM,
	OS_TASK_PRIORITY_HIGH,
	OS_TASK_PRIORITY_REALTIME,


	OS_NUM_TASK_PRIORITIES

}OS_Task_Priority_t;



struct TCB_t;
struct PeriodicTCB_t;

typedef void (*TaskFunction_t)(void);
typedef struct TCB_t TaskHandle_t;
typedef struct PeriodicTCB_t 	PeriodicTaskHandle_t;

#endif /* INC_OS_DEFS_H_ */
