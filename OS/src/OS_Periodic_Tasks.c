/*
 * @file 	OS_Periodic_Tasks.c
 * @author	Hassan Abid
 * @date	Feb 22, 2018
 *
 * 
 */

#include <stdint.h>
#include <string.h>

#include "List.h"
#include "OS_Config.h"
#include "OS_Defs.h"
#include "OS_Periodic_Tasks.h"


#define addPeriodicTaskToList( task )		\
				(task)->listItem.value = (task)->priority;\
				List_InsertSorted( &periodicTasksList, &(task)->listItem )

#define getNextPeriodicTask()		\
			((PeriodicTCB_t*)List_getElement(List_Iterate(&periodicTasksList)))


PeriodicTCB_t  periodicTCBs[OS_MAX_NUM_TASKS];
PeriodicTCB_t* nextPeriodicTask;
List_t periodicTasksList;

uint32_t periodicTaskCount = 0;

/**
 * @defgroup OS_Periodic_Tasks_Private_Functions	OS_Periodic_Tasks Private Functions
 * @{
 */

//void Timer1A_IRQ_Callback(void)
//{
//	PeriodicTCB_t* currentTask = nextPeriodicTask;
//	nextPeriodicTask = getNextPeriodicTask();
//	//Timer1A_updatePeriod(nextPeriodicTask->period - currentTask->period);
//	
//	//currentTask->task();
//}


void Timer1A_IRQ_Callback(void)
{
	for(int i = 0; i < periodicTaskCount; i++){
		if(((Timer1_getCount() - periodicTCBs[i].start_time) >= periodicTCBs[i].period)){
			periodicTCBs[i].task();
			periodicTCBs[i].start_time = Timer1_getCount();
		}
	}
}



/** @}*/

/**
 * @defgroup OS_Periodic_Tasks_Public_Functions	OS_Periodic_Tasks Public Functions
 * @{
 */

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
PeriodicTCB_t* OS_AddPeriodicThread(TaskFunction_t taskFunction, char* name,
									uint32_t priority, uint32_t period)
{
	for (int i = 0 ; i < OS_MAX_NUM_TASKS ; i++)
	{

		if (periodicTCBs[i].listItem.list == NULL)
		{
			strncpy(periodicTCBs[i].name, name, sizeof(periodicTCBs[i].name));
			periodicTCBs[i].priority = priority;
			periodicTCBs[i].listItem.parent = &periodicTCBs[i];
			periodicTCBs[i].task = taskFunction;
			periodicTCBs[i].period = period;
			periodicTCBs[i].start_time = Timer1_getCount();

			addPeriodicTaskToList( &periodicTCBs[i] );
			periodicTaskCount++;
			
			return &periodicTCBs[i];
		}

	}

	return NULL;
}


/**
 * Get the current time of the periodic timer.
 */
uint32_t OS_PeriodicTimer_Time()
{
	return Timer1_getTime();
}

/**
 * Get the time difference between two timer clock readings.
 *
 * @return		currenTime - lastTime
 */
uint32_t OS_PeriodicTimer_TimeDifference(uint32_t lastTime, uint32_t currentTime)
{
	return Timer1_getJitter(lastTime, currentTime);
}

/** @}*/



