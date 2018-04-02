/*
 * @file 	OS_Signal.c
 * @author	Johnny Rojas
 * @date	Feb 8, 2018
 *
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include "OS_Signal.h"
#include "OS.h"
#include "OS_Task_Lists.h"



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value


#define blockTaskOnSignal( signal, task )		\
			removeTaskFromReadyList( task )	\
			addTaskToSignalList( signal, task )

#define unblockHighestPriorityTask( signal )			\
			{ TCB_t* task = (TCB_t*)List_getFirstElement( &(signal)->blockedTasksList );\
				removeTaskFromSignalList( signal, task );\
				addTaskToReadyList( task );\
			}\
	

/**
 * @defgroup OS_Signal_Private_Functions	OS_Signal Private Functions
 * @{
 */


/** @}*/

/**
 * @defgroup OS_Signal_Public_Functions	OS_Signal Public Functions
 * @{
 */

/**
 * Initialize mutex structure.
 *
 * @param mutex		Pointer to mutex object
 */
void OS_InitMutex(OS_Mutex_t* mutex)
{
	long sr;
	
	sr = StartCritical();
	
	if(mutex)
	{
		mutex->flag = 1;
	}
	EndCritical(sr);
}

/**
 * Take the mutex if available, otherwise block
 * the current thread while waiting for the mutex
 * @param mutex		Pointer to the mutex object
 */
void OS_MutexWait(OS_Mutex_t* mutex)
{
	DisableInterrupts();

	if((mutex->flag) == 0)
	{
		blockTaskOnSignal(mutex, currentTCB);
		EnableInterrupts();
		OS_Suspend();
		DisableInterrupts();
	} 
	
	(mutex->flag) = 0; 
	EnableInterrupts();
}

/**
 * Release the mutex.
 * @param mutex			Pointer to the mutex object.
 *
 * @warning		This function does not check if the current thread
 * 				owns the mutex and therefore should be called only
 * 				after the thread has acquired the mutex.
 */
void OS_MutexSignal(OS_Mutex_t* mutex)
{ 
	long sr;
	sr = StartCritical();
	mutex->flag = 1; 
	
	if (List_getSize(&mutex->blockedTasksList))
	{
		unblockHighestPriorityTask( mutex );
	}
	
	EndCritical(sr);
}

/**
 * Check if the mutex is available.
 * This function can be used for checking for the mutex,
 * before taking it, instead of blocking on it.
 * @param mutex		Pointer to the mutex object.
 * @return			0 if mutex is unavailable
 * 					1 if mutex is available
 */
int OS_MutexAvailable(OS_Mutex_t* mutex)
{
	return mutex->flag;
}

/**
 * Initialize the counting semaphore and set it to it's initial value.
 * @param smphr				Pointer to semaphore object
 * @param initialValue		Initial value of the semaphore
 */
void OS_InitSemaphore(OS_Semaphore_t* smphr, int32_t initValue)
{
	long sr;

	sr = StartCritical();
	
	if(smphr){
		smphr->value = initValue;
	
	}
	EndCritical(sr);
}

/**
 * Take the semaphore if available otherwise block
 * until it becomes available.
 * This function decrements the counting semaphore.
 *
 * @param smphr		Pointer to semaphore object
 */
void OS_Wait(OS_Semaphore_t* smphr)
{
	DisableInterrupts();
	if (smphr->value <= 0)
	{
		blockTaskOnSignal( smphr, currentTCB );
		EnableInterrupts();
		OS_Suspend();
		DisableInterrupts();
	}

	smphr->value--;
	EnableInterrupts();
}

/**
 * Signal the release of semaphore by the current thread.
 * This function increments the counting semaphore.
 *
 * @param smphr		Pointer to the semaphore object.
 */
void OS_Signal(OS_Semaphore_t* smphr)
{
	long sr;
	sr = StartCritical();
	
	if (List_getSize(&smphr->blockedTasksList))
	{
		unblockHighestPriorityTask(smphr);
	}
	
	smphr->value++;
	EndCritical(sr);
}

/** @}*/
