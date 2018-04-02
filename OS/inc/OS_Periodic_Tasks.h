/*
 * @file 	OS_Periodic_Tasks.h
 * @author	Hassan Abid
 * @date	Feb 22, 2018
 *
 * 
 */

#ifndef INC_OS_PERIODIC_TASKS_H_
#define INC_OS_PERIODIC_TASKS_H_


typedef struct PeriodicTCB_t{

	void (*task)(void);
	uint32_t period;
	uint32_t priority;
	uint32_t start_time;
	ListItem_t listItem;
	char name[16];

}PeriodicTCB_t;


#ifdef __cplusplus
extern "C"{
#endif 
 

#ifdef __cplusplus
}
#endif


#endif /* INC_OS_PERIODIC_TASKS_H_ */
