/*
 * @file 	OS_Task_Lists.c
 * @author	Hassan Abid
 * @date	Mar 1, 2018
 *
 * 
 */

#include <stdint.h>
#include "OS_Task_Lists.h"

 List_t	readyTasksList[OS_NUM_TASK_PRIORITIES];
 List_t	delayedTasksList;

TCB_t TCB[OS_MAX_NUM_TASKS];
TCB_t* currentTCB;

uint32_t OS_TaskCount = 0;
uint32_t OS_ReadyPriorityFlags = 0;
