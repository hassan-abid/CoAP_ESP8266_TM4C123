/*
 * @file 	HAL_Def.h
 * @author	Hassan Abid
 * @date	Mar 2, 2018
 *
 * 
 */

#ifndef INC_HAL_DEFS_H_
#define INC_HAL_DEFS_H_

#pragma anon_unions



typedef enum{

	HAL_OK,
	HAL_BUSY,
	HAL_ERROR,

}HAL_Return_t;

#define HAL_ASSERT_RETURN(cond, retValue)	\
	if (cond)\
		return retValue;

#ifdef __cplusplus
extern "C"{
#endif 
 
uint32_t HAL_getTicks(void);


#ifdef __cplusplus
}
#endif


#endif /* INC_HAL_DEFS_H_ */
