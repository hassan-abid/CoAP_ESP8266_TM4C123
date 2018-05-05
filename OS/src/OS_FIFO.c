/*
 * @file 	OS_FIFO.c
 * @author	Hassan Abid
 * @date	Feb 22, 2018
 *
 * 
 */

#include <stdint.h>
#include <string.h>
#include "OS_FIFO.h"

OS_FIFO_Def(Default_FIFO, uint8_t, 1);
typedef struct __Default_FIFO OS_FIFO_t;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value


/**
 * @defgroup OS_FIFO_Private_Functions	OS_FIFO Private Functions
 * @{
 */


/** @}*/

/**
 * @defgroup OS_FIFO_Public_Functions	OS_FIFO Public Functions
 * @{
 */


/**
 * Initialize the fifo OS components (semaphores and mutexes)
 * @param fifo	Pointer to the fifo object
 */
void OS_FIFO_Init(void* fifo)
{
	OS_FIFO_t* _fifo = fifo;
	OS_InitMutex(&_fifo->mutex);
	OS_InitSemaphore(&_fifo->smphrRoomLeft, _fifo->size);
	OS_InitSemaphore(&_fifo->smphrDataAvailable, 0);
}

/**
 * Insert the given data into the back of the FIFO.
 * The function checks if room is available in the fifo and
 * if the fifo is full, blocks the current thread.
 *
 * @param fifo		Pointer to fifo object
 * @param data		Pointer to input variable.
 */
void OS_FIFO_Put_Blocking(void* fifo, void* data)
{
	long sr;
	OS_FIFO_t* _fifo = fifo;
	
	OS_Wait(&_fifo->smphrRoomLeft);
	OS_MutexWait(&_fifo->mutex);
	
	sr = StartCritical();
	memcpy(_fifo->buffer + (_fifo->back *_fifo->elementSize), data, _fifo->elementSize);
	
	_fifo->back++;
	_fifo->count++;
	if (_fifo->back == _fifo->size)
		_fifo->back = 0;
	
	EndCritical(sr);

	OS_Signal(&_fifo->smphrDataAvailable);
	OS_MutexSignal(&_fifo->mutex);
}

/**
 * Insert the given data into the back of the FIFO in non-blocking
 * mode. If there is no room available, the function returns an error
 * value immediately.
 *
 * @param fifo		pointer to fifo object
 * @param data		Pointer to input variable
 * @return			0 if no space available in fifo
 * 					1 if data added to fifo.
 */
int OS_FIFO_Put(void* fifo, void* data)
{
	long sr = StartCritical();
	uint32_t ret = 0;
	
	OS_FIFO_t* _fifo = fifo;
	if (_fifo->count == _fifo->size )
	{
		goto end;
	}
	OS_Wait(&_fifo->smphrRoomLeft);
	
	memcpy(_fifo->buffer + (_fifo->back*_fifo->elementSize), data, _fifo->elementSize);
	_fifo->back++;
	_fifo->count++;
	if (_fifo->back == _fifo->size)
		_fifo->back = 0;
	
	ret = 1;
	OS_Signal(&_fifo->smphrDataAvailable);
	end:
	EndCritical(sr);
	
	return ret;
}

/**
 * Get oldest value from fifo in blocking mode.
 * If there is no data present in the fifo, the
 * function blocks the present thread until data is available.
 *
 * @param fifo			Pointer to fifo object
 * @param data[out]		Pointer to output variable.
 */
void OS_FIFO_Get(void* fifo, void* data)
{
	long sr;
	OS_FIFO_t* _fifo = fifo;

	OS_Wait(&_fifo->smphrDataAvailable);
	OS_MutexWait(&_fifo->mutex);

	sr = StartCritical();

	memcpy(data, _fifo->buffer + (_fifo->front*_fifo->elementSize), _fifo->elementSize);
	
	_fifo->front++;
	_fifo->count--;
	if (_fifo->front == _fifo->size)
		_fifo->front = 0;

	OS_Signal(&_fifo->smphrRoomLeft);
	OS_MutexSignal(&_fifo->mutex);	
	EndCritical(sr);



}
/** @}*/
