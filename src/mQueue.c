/**
 * @file   	mQueue.c
 * @author 	Hassan Abid
 * @version	1.0.0
 * @date	Mar 16, 2016
 *
 * @brief
 */



#include "mQueue.h"

/**
 * @defgroup mQueue_private_Function mQueue Private Functions
 * @{
 */

/**
 * @brief calculate the corresponding index in the character buffer
 * 		  for the given element index in queue.
 * @param queueIndex 	index of element in queue.
 * @param elementSize   size of elements in queue.
 * @return the char buffer index for the given element index.
 */
static __inline uint32_t getCharBufferIndex(uint16_t queueIndex, uint16_t elementSize)
{
	return (queueIndex*elementSize);
}

/**
 * @brief			 	Increment and wrap the input queue index.
 * @param queueIndex	current value of index
 * @param queueSize		max number of elements in Queue.
 * @return				new queue index after increment.
 */
static __inline uint16_t incrementQueueIndex(uint16_t queueIndex, uint16_t queueSize)
{
	return ((queueIndex + 1) % queueSize);
}

/**
 * @brief 			increment the count of number of elements currently in Queue
 * @param count		number of elements in queue.
 * @param queueSize max number of elements in queue.
 * @return			New count of elements. Same as before if queue full or
 * 					incremented by one.
 */
static inline uint16_t incrementCount(uint16_t count, uint16_t queueSize){

	return (count >= queueSize ? count : count+1);
}

/** @}*/


/**
 * @defgroup mQueue_public_functions mQueue Public Functions
 * @{
 */

/**
 * @brief 		Initialize the input queue struct.
 * @param q		pointer to the Queue struct to be initialized.
 * @return 		mQueue_OK
 *
 * @note 		The struct members {buff, elementSize and size} must
 * 				be initialize at compile time.
 */
 mQueueReturn_t mQueue_init(mQueueStruct_t* q)
{
	q->front = 0;	/* Set the input cursor of queue at 0*/
	q->back  = 0;	/* Set the output cursor of the queue at 0*/
	q->count = 0;	/* Initialize the queue as empty*/

	return mQueue_OK;
}


/**
 * @brief 			Add a new element to the back of the Queue.
 * @param q			pointer to queue
 * @param element	pointer to element which is to be added to queue.
 * @return			mQueue_OK
 *
 * @note 			This function does not check for overflow condition
 * 					and the oldest entries are overwritten incase of overflow.
 */
mQueueReturn_t mQueue_push(mQueueStruct_t* q, void* element)
{
	//copy the element into the buffer at the back of queue.
	memcpy( q->buff + getCharBufferIndex(q->back, q->elementSize),
			element,
			q->elementSize);

	//increment element count
	q->count = incrementCount(q->count, q->size);
	//increment back index
	q->back = incrementQueueIndex(q->back, q->size);
    
    //if the queue has overflowed, update the front of the 
    //queue as well.
    if (mQueue_isFull(q))
    {
       q->front = incrementQueueIndex(q->front, q->size); 
    }

	return mQueue_OK;
}

/**
 * @brief 			  read the element at the specified index in the queue, with reference
 * 		  			  from the front of the queue.(i.e 0 would return front of queue)
 * @param q			  pointer to queue structure.
 * @param index		  index offset of the element to read from the front of the queue.
 * @param pRetMemAddr pointer to the element buffer for output to be stored.
 * @return
 * 		@arg mQueue_OK 		if the index is with in bounds or
 * 		@arg mQueue_ERROR	if the index is out of bounds
 */
mQueueReturn_t mQueue_peekAt(mQueueStruct_t* q, uint16_t index, void* pRetMemAddr)
{
	if (index >= q->count)
		return mQueue_ERROR;

	//translate offset into queue index.
	index = (q->front + index)%q->size;
	//set the pointer to absolute memory address of element in buffer.
	memcpy( pRetMemAddr,
			q->buff + getCharBufferIndex(index, q->elementSize),
			q->elementSize );

	return mQueue_OK;
}

/**
 * @brief 		 	  read the element at the front of the queue.
 * @param q			  pointer to queue struct.
 * @param pRetMemAddr pointer to return value, the memory address of the element.
 * @return
 * 		@arg mQueue_OK 		if there is an element in the queue.
 * 		@arg mQueue_ERROR	if the queue is empty.
 */
mQueueReturn_t mQueue_peek(mQueueStruct_t* q, void* pRetMemAddr)
{
	return mQueue_peekAt(q, 0, pRetMemAddr);
}

/**
 * @brief 	Remove and return the oldest element from the queue.
 * @return
 * 		@arg mQueue_OK 		if there is an element in the queue.
 * 		@arg mQueue_ERROR	if the queue is empty.
*/
mQueueReturn_t mQueue_pop(mQueueStruct_t* q)
{
	return mQueue_popMultiple(q, 1);
}


/**
 * @brief 		Remove multiple elements from the queue.
 * @param q		pointer to queue struct
 * @param count number of elements to be removed from the queue
 * @return
 * 		@arg mQueue_OK		if elements were successfully removed
 * 		@arg mQueue_ERROR	if the number of elements to be removed is greater than
 * 							number of elements in queue
 */
mQueueReturn_t mQueue_popMultiple(mQueueStruct_t* q, uint16_t count)
{
	if (count > q->count)
		return mQueue_ERROR;

	q->count = q->count - count;
	q->front = (q->front + count)%q->size;

	return mQueue_OK;
}

/** @}*/
