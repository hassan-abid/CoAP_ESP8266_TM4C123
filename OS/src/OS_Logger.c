/*
 * @file 	OS_Logger.c
 * @author	Hassan Abid
 * @date	Mar 6, 2018
 *
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include "OS_Logger.h"

#define __OS_LOG_EVENT_STRING(event, ...)	\
	#event ,

static const char* logEventStrLookup[] =
{
		OS_LOG_EVENT_LIST(__OS_LOG_EVENT_STRING)
};


OS_Log_t OS_log = {.count = 0};




/**
 * @defgroup OS_Logger_Private_Functions	OS_Logger Private Functions
 * @{
 */


/** @}*/

/**
 * @defgroup OS_Logger_Public_Functions	OS_Logger Public Functions
 * @{
 */

void OS_printLog(void)
{
	for (int i = 0 ; i < OS_log.count ; i++)
	{
		printf("%u.%u, \t%s,\t %s,\t %u\r\n", OS_log.logEntry[i].timestamp.milliseconds,
															(uint32_t)(OS_log.logEntry[i].timestamp.subticks * 12.5),
															OS_log.logEntry[i].name, 
															logEventStrLookup[OS_log.logEntry[i].event],
															OS_log.logEntry[i].info);
	}
}

void OS_clearLog(void)
{
	OS_log.count = 0;
}

/** @}*/
