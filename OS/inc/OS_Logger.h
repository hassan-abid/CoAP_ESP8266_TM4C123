/*
 * @file 	OS_Logger.h
 * @author	Hassan Abid
 * @date	Mar 6, 2018
 *
 * 
 */

#ifndef INC_OS_LOGGER_H_
#define INC_OS_LOGGER_H_

#define OS_LOG_EVENT_LIST(OS_LOG_EVENT, ...) \
\
	OS_LOG_EVENT(FOREGROUND_TASK, 		__VA_ARGS__)\
	OS_LOG_EVENT(PERIODIC_THREAD_START, __VA_ARGS__)\
	OS_LOG_EVENT(PERIODIC_THREAD_END, 	__VA_ARGS__)\
	OS_LOG_EVENT(INTERRUPTS_DISABLED,   __VA_ARGS__)\
	OS_LOG_EVENT(INTERRUPTS_ENABLED, 		__VA_ARGS__)\
	OS_LOG_EVENT(START_CRITICAL,				__VA_ARGS__)\
	OS_LOG_EVENT(END_CRITICAL, 					__VA_ARGS__)

#define __OS_LOG_EVENT_DEF(name, ...)	\
	OS_LOG_EVENT_##name,

typedef enum{

	OS_LOG_EVENT_LIST(__OS_LOG_EVENT_DEF)

}OS_LogEvent_t;


typedef struct{

	uint32_t milliseconds;
	uint32_t subticks;

}OS_LogEvent_Timestamp_t;

typedef struct
{
	OS_LogEvent_Timestamp_t timestamp;
	char name[16];
	OS_LogEvent_t event;
	uint32_t info;

}OS_LogEntry_t;


typedef struct{

	OS_LogEntry_t logEntry[100];
	uint32_t count;

}OS_Log_t;


extern OS_Log_t OS_log;

#if !defined(sizeof_array)
#define		sizeof_array(arr)		(sizeof(arr) / sizeof(arr[0]))
#endif


#define OS_LogEvent( task, _event)	\
\
if (OS_log.count < sizeof_array(OS_log.logEntry))\
{\
	OS_log.logEntry[OS_log.count].timestamp.subticks = NVIC_ST_RELOAD_R - NVIC_ST_CURRENT_R;\
	OS_log.logEntry[OS_log.count].timestamp.milliseconds = OS_getTickCount();\
	OS_log.logEntry[OS_log.count].event = _event;\
	strncpy(OS_log.logEntry[OS_log.count].name, (task)->name, sizeof(OS_log.logEntry[OS_log.count].name));\
	OS_log.count++;\
}\

#define OS_LogEventInfo( task, event, _info ) \
	if (OS_log.count < sizeof_array(OS_log.logEntry))\
	{\
		OS_LogEvent( task, event );\
		OS_log.logEntry[OS_log.count].info = _info;\
	}\

#ifdef __cplusplus
extern "C"{
#endif 
 


#ifdef __cplusplus
}
#endif


#endif /* INC_OS_LOGGER_H_ */
