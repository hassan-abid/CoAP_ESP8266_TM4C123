/*
 * @file 	OS.h
 * @author	Hassan Abid
 * @date	Feb 1, 2018
 *
 * 
 */

#ifndef INC_OS_H_
#define INC_OS_H_

#include "OS_Signal.h"
#include "OS_FIFO.h"
#include "OS_Mailbox.h"
#include "OS_Defs.h"



#ifdef __cplusplus
extern "C"{
#endif 
 

/**
 *	Initialize the OS Base clock (systick timer) to a granularity of 1ms.
 */
int OS_Init(void);

/**
 * Add a thread to the OS kernel for execution.
 * @param taskFunction		Pointer to task function
 * @param name				Name of the thread (for debugging purposes)
 * @param priority			Task priority
 * 							@see OS_Task_Priority_t
 * @return			1 if the thread was created successfully
 *   						0 if there were no more TCBs available to be alloted to the thread
 */
uint32_t OS_AddThread(TaskFunction_t taskFunction, char* name, uint32_t priority);

/**
 *	Adds a task to the OS, which is to be executed periodically at
 *	the specified time intervals.
 *
 * @param task		Pointer to the task function
 * @param name		Name of the periodic task
 * @param priority	Priority of the periodic task. Determines order of execution if
 * 					multiple periodic threads are due.
 * @param period	Time period of execution of task in OS Time Base Units (currently milliseconds).
 *
 */
PeriodicTaskHandle_t* OS_AddPeriodicThread(TaskFunction_t taskFunction, char* name,
									uint32_t priority, uint32_t period);

/**
 * Add a task to be executed when SW1 is pressed.
 * @param taskFunction	Pointer to task function
 */
int OS_AddSW1Task(TaskFunction_t taskFunction);

/**
 * Add a task to be executed when SW2 is pressed.
 * @param taskFunction		Pointer to task function
 */
int OS_AddSW2Task(TaskFunction_t taskFunction);

/**
 * Unlink the SW2 task. Once unlinked, 
 * the task function must be passed again through
 * OS_AddSW2Task() function.
 */
int OS_RemoveSW2Task(void);


/**
 * Reset the OS base clock to 0.
 */
void OS_ClearPeriodicTime(void);


/**
 * Suspend the current task.
 */
void OS_Suspend(void);

/**
 * Start the OS kernel and start executing the first task in queue.
 * @param baseFrequency		Base frequency of the OS system clock.
 */
void OS_Start(uint32_t baseFrequency);

void OS_PeriodicTimerInit(uint32_t freq);

/**
 * Get the current time of the periodic timer.
 */
uint32_t OS_PeriodicTimer_Time(void);

/**
 * Get the time difference between two timer clock readings.
 *
 * @return		currenTime - lastTime
 */
uint32_t OS_PeriodicTimer_TimeDifference(uint32_t lastTime, uint32_t currenTime);

/**
 * Get the current OS clock time.
 */
uint32_t OS_getTickCount(void);

/**
 * Clear the OS clock.
 * Resets the clock to 0.
 */
uint32_t OS_clearTickCount(void);


/**
 * Suspend the current task for the specified number of ticks.
 * The function does not start executing exactly after ticks.
 * It will be ready to run after ticks period.
 * @param ticks		Number of ticks (has units of OS base time period)
 */
void OS_Sleep(uint32_t ticks);

/**
 * Stop executing the current task and remove from OS kernel.
 */
void OS_Kill(void);
	
	
uint32_t OS_getMaxInterruptDisableTime(void);
uint32_t OS_getTotalInterruptDisableTime(void);
void		 OS_clearInterruptDisable(void);

void OS_printLog(void);

void OS_clearLog(void);

#ifdef __cplusplus
}
#endif


#endif /* INC_OS_H_ */
