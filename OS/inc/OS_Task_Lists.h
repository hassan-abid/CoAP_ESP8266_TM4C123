/*
 * @file 	OS_Task_Lists.h
 * @author	Hassan Abid
 * @date	Mar 1, 2018
 *
 * 
 */

#ifndef INC_OS_TASK_LISTS_H_
#define INC_OS_TASK_LISTS_H_

#include "OS_Config.h"
#include "OS_Defs.h"
#include "List.h"

//Helper Macros

/**
 * Set the corresponding flag for the priority in the ready list register.
 * The register determines the highest priority list which is not empty.
 */
#define SET_READY_PRIORITY_FLAG(priority)			(OS_ReadyPriorityFlags |= 1<<priority)

/**
 * Clear the corresponding flag for the priority in the ready list register.
 * The flag is cleared once there are no more tasks ready with this priority.
 */
#define RESET_READY_PRIORITY_FLAG(priority)		(OS_ReadyPriorityFlags &= ~(1<<priority))

/**
 * Get the highest priority of all the ready tasks
 */
#define getHighestReadyPriority()			((uint32_t)(31 - __clz(OS_ReadyPriorityFlags)))

/**
 * Get the highest priority task to be run by the scheduler.
 * @note Updates the corresponding list iterator.
 */
#define getNextReadyTask()				((TCB_t*)List_getElement(List_Iterate(&readyTasksList[getHighestReadyPriority()])))

/**
 * Add a new task to the end of the specified list.
 */
#define addTaskToList(list, task)			List_InsertEnd( (list), &(task)->stateListItem);

#define addTaskToOrderedList(list, task)	List_InsertSorted( (list), &(task)->stateListItem);

/**
 * Removes a specified task from the list.
 */
#define removeTaskFromList(list, task)	List_Remove( (list), &(task)->stateListItem);


/**
 * Adds a task to the ready list.
 * The task is added to the ready list corresponding to the priority of the task.
 * Also updates the ready list priorities register.
 */
#define addTaskToReadyList(task)	\
				addTaskToList(&readyTasksList[(task)->priority], (task))\
				SET_READY_PRIORITY_FLAG((task)->priority);\


/**
 * Removes a task from the ready list.
 * Determines the priority ready list and removes it from that list.
 * If it was the last task in the list for that priority, clear the corresponding
 * flag in the ready list priorities register
 */
#define removeTaskFromReadyList(task)	\
					removeTaskFromList(&readyTasksList[(task)->priority], (task));\
					if(List_getSize(&readyTasksList[(task)->priority]) == 0)\
						RESET_READY_PRIORITY_FLAG((task)->priority);\


/**
 * Add a task to the delayed list
 */
#define addTaskToDelayedList(task) \
					(task)->stateListItem.value = (task)->sleepTimer;\
					addTaskToOrderedList(&delayedTasksList, (task))

/**
 * Remove a task from the delayed list.
 */
#define removeTaskFromDelayedList(task)		\
					removeTaskFromList(&delayedTasksList, (task))


#define addTaskToSignalList(event, task)	\
			(task)->stateListItem.value = (task)->priority;\
			addTaskToOrderedList( &(event)->blockedTasksList, task )


#define removeTaskFromSignalList(event, task)  \
			removeTaskFromList( &(event)->blockedTasksList, task )


/**
 * Task Control Block
 */
typedef struct TCB_t{

	uint32_t* sp;						///stack pointer
	char name[16];						///Task name
	uint32_t priority;					///Task priority when ready.
	uint32_t sleepTimer;				///Countdown timer for when the task is delayed for a particular time interval.
	uint32_t stack[OS_STACK_SIZE];
	ListItem_t stateListItem;		///Determines the list of the current state of thread (Ready list, Blocked list, etc)

}TCB_t;


extern TCB_t 	TCB[OS_MAX_NUM_TASKS];		//Pool of TCBs

/// OS State List Definitions
extern List_t	readyTasksList[OS_NUM_TASK_PRIORITIES];
extern List_t	delayedTasksList;

extern TCB_t* currentTCB;

extern uint32_t OS_TaskCount;
extern uint32_t OS_ReadyPriorityFlags;


#endif /* INC_OS_TASK_LISTS_H_ */
