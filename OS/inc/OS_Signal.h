/*
 * @file 	OS_Signal.h
 * @author	Johnny Rojas
 * @date	Feb 8, 2018
 *
 * 
 */

#ifndef OS_SIGNAL_H_
#define OS_SIGNAL_H_

#include "List.h"

typedef struct OS_Semaphore_t{

	int32_t value;
	int32_t max;
	
	List_t blockedTasksList;

}OS_Semaphore_t;

typedef struct OS_Mutex_t{

	uint8_t flag;
	List_t blockedTasksList;
	
}OS_Mutex_t;


#ifdef __cplusplus
extern "C"{
#endif 


/**
 * Initialize the counting semaphore and set it to it's initial value.
 * @param smphr				Pointer to semaphore object
 * @param initialValue		Initial value of the semaphore
 */
void OS_InitSemaphore(OS_Semaphore_t* smphr, int32_t initialValue);

/**
 * Signal the release of semaphore by the current thread.
 * This function increments the counting semaphore.
 *
 * @param smphr		Pointer to the semaphore object.
 */
void OS_Signal(OS_Semaphore_t* smphr);

/**
 * Take the semaphore if available otherwise block
 * until it becomes available.
 * This function decrements the counting semaphore.
 *
 * @param smphr		Pointer to semaphore object
 */
void OS_Wait(OS_Semaphore_t* smphr);

/**
 * Initialize mutex structure.
 *
 * @param mutex		Pointer to mutex object
 */
void OS_InitMutex(OS_Mutex_t* mutex);

/**
 * Take the mutex if available, otherwise block
 * the current thread while waiting for the mutex
 * @param mutex		Pointer to the mutex object
 */
void OS_MutexWait(OS_Mutex_t* mutex);

/**
 * Release the mutex.
 * @param mutex			Pointer to the mutex object.
 *
 * @warning		This function does not check if the current thread
 * 				owns the mutex and therefore should be called only
 * 				after the thread has acquired the mutex.
 */
void OS_MutexSignal(OS_Mutex_t* mutex);

/**
 * Check if the mutex is available.
 * This function can be used for checking for the mutex,
 * before taking it, instead of blocking on it.
 * @param mutex		Pointer to the mutex object.
 * @return			0 if mutex is unavailable
 * 					1 if mutex is available
 */
int  OS_MutexAvailable(OS_Mutex_t* mutex);

#ifdef __cplusplus
}
#endif


#endif /* OS_SIGNAL_H_ */
