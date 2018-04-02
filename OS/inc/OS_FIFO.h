/*
 * @file 	OS_FIFO.h
 * @author	Hassan Abid
 * @date	Feb 22, 2018
 *
 * 
 */


#ifndef INC_OS_FIFO_H_
#define INC_OS_FIFO_H_

#include "OS_Signal.h"


#define OS_FIFO_Def(name, dataType, _size)\
\
struct __##name\
{\
	const uint32_t size;\
	uint32_t count;\
	uint32_t front;\
	uint32_t back;\
	OS_Semaphore_t smphrRoomLeft;\
	OS_Semaphore_t smphrDataAvailable;\
	OS_Mutex_t	mutex;\
	dataType buffer[_size];\
\
} name = { .size = _size, .count = 0, .front = 0, .back = 0};


#ifdef __cplusplus
extern "C"{
#endif 

/**
 * Initialize the fifo OS components (semaphores and mutexes)
 * @param fifo	Pointer to the fifo object
 */
void OS_FIFO_Init(void* fifo);

/**
 * Get oldest value from fifo in blocking mode.
 * If there is no data present in the fifo, the
 * function blocks the present thread until data is available.
 *
 * @param fifo			Pointer to fifo object
 * @param data[out]		Pointer to output variable.
 */
void OS_FIFO_Get(void* fifo, void* data);

/**
 * Insert the given data into the back of the FIFO.
 * The function checks if room is available in the fifo and
 * if the fifo is full, blocks the current thread.
 *
 * @param fifo		Pointer to fifo object
 * @param data		Pointer to input variable.
 */
void OS_FIFO_Put_Blocking(void* fifo, void* data);

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
int  OS_FIFO_Put(void* fifo, void* data);
	

#ifdef __cplusplus
}
#endif


#endif /* INC_OS_FIFO_H_ */
