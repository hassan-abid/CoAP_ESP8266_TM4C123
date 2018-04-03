/**
 * @file   	mQueue.h
 * @author 	Hassan Abid
 * @version	1.0.0
 * @date	Mar 16, 2016
 *
 * @brief
 */

#ifndef MQUEUE_H_
#define MQUEUE_H_

#include <string.h>
#include <stdint.h>



/**
 * @defgroup mQueue_Public_Macros		mQueue Public Macro Definitions
 * @{
 */

#define mQueue_isFull(q)			((q)->count == (q)->size)
#define mQueue_isEmpty(q)			((q)->count == 0)
#define mQueue_size(q)              ((q)->size)
#define mQueue_count(q)             ((q)->count)



#define mQueue_Def( name, dataType, bufferSize )	\
		dataType __mQueue_##name##_buff[bufferSize];\
		mQueueStruct_t name = \
		{\
			.size = bufferSize,\
			.buff = __mQueue_##name##_buff,\
			.elementSize = sizeof(dataType),\
		}\


/** @}*/

/**
 * @defgroup mQueue_Public_Enums		mQueue Public Enum Definitions
 * @{
 */

/**
 * @brief enum for return values from the mQueue functions
 *
 */
typedef enum{

	mQueue_OK = 0,	/*!< function executed successfully*/
	mQueue_ERROR	/*!< there was an error while executing the function*/

}mQueueReturn_t;

/** @}*/

/**
 * @defgroup mQueue_Public_Structs		mQueue Public Struct Definitions
 * @{
 */

/**
 * @brief	structure to implement and manipulate
 * 			a character buffer in FIFO fashion (Queue).
 *
 * @note	The Queue is circular and oldest entries
 * 			are overwritten on overflow.
 *
 * @note   	The struct members {count, front, back, size} must be of the
 * 			same size.
 */
typedef struct{

	const uint8_t elementSize;	/*!< size of the elements in queue in bytes.*/
  uint32_t count;			/*!< number of elements currently stored in queue*/
  uint32_t front;			/*!< index at which the next element will be pushed*/
	uint32_t back;			/*!< index from which the next element is to be popped */
	const uint32_t size;			/*!< maximum number of elements queue can hold*/
	uint8_t* const buff;			/*!< pointer to the character buffer for storing elements.*/

}mQueueStruct_t;

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif


mQueueReturn_t mQueue_init		 (mQueueStruct_t* q);
mQueueReturn_t mQueue_push		 (mQueueStruct_t* q, void* element);
mQueueReturn_t mQueue_peekAt     (mQueueStruct_t* q, uint16_t index, void* pRetMemAddr);
mQueueReturn_t mQueue_peek		 (mQueueStruct_t* q, void* pRetMemAddr);
mQueueReturn_t mQueue_pop 		 (mQueueStruct_t* q);
mQueueReturn_t mQueue_popMultiple(mQueueStruct_t* q, uint16_t count);

#ifdef __cplusplus
}
#endif

#endif /* MQUEUE_H_ */
