/*
 * @file 	OS.c
 * @author	Hassan Abid
 * @date	Feb 1, 2018
 *
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#include "inc\hw_ints.h"
#include "driverlib\interrupt.h"
#include "driverlib\sysctl.h"


#include "OS.h"
#include "OS_Task_Lists.h"
#include "OS_Logger.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value






#define sizeof_array(arr)		(sizeof(arr) / sizeof(arr[0]))


/**
 * @defgroup	OS_Private_Variables			OS Private Variables
 * @{
 */


uint32_t OS_sysTicks = 0;
uint32_t OS_cyclesPerTick = 0;

TCB_t*  TCBhistory[100];
int  TCBhistoryCounter;


TaskFunction_t SW1Task;
TaskFunction_t SW2Task;

TaskFunction_t periodicTask;


/** @} */


/**
 * @defgroup OS_Private_Functions	OS Private Functions
 * @{
 */
 



/**
 * Triggers context switch.
 */
void Yeild(void)
{
	IntPendSet(FAULT_PENDSV);
}

/**
 * Context switcher.
 *
 */
__asm void PendSV_Handler(void)
{
	extern currentTCB;
	extern Scheduler;
	
	PRESERVE8						//keil compiler warning
	
	CPSID			I					//Disable Interrupts
	
	PUSH			{R4-R11}	
	LDR				R0, =currentTCB			//Pointer to current task
	LDR				R1, [R0]				//Pointer to the stack pointer variable in TCB
	STR				SP, [R1]				//Save current stack pointer into the TCB
	PUSH			{R0, LR}				//C function does not preserve R0 and LR
														//so, the must be pushed onto stack before function call.
	BL				Scheduler			
	POP				{R0, LR}	
	LDR				R1, [R0]				//Pointer to new task
	LDR				SP,	[R1]				//Load stack for new task into the registers.
	POP				{R4-R11}
	
	CPSIE			I			//Enable Interrupts
	
	BX 				LR		
}


/**
 * Start executing first task in the OS.
 * Called only once on call to OS_Start()
 */
__asm void StartFirstTask(void)
{

	PRESERVE8

	LDR		R0, =currentTCB
	LDR		R1, [R0]		//Get stack pointer for first thread
	LDR		SP, [R1]		//Update stack pointer register
	POP		{R4-R12}		//Restore registers R0-R12
	POP		{R0-R3}
	POP		{LR}
	POP		{LR}		//Return to start of first thread.
	POP		{R2}		//Ignore PSR

	CPSIE	I			//Enable interrupts
	BX 		LR			//Go to start of first thread.
}

/**
 * Systick Interrupt handler.
 * The systick provides the OS base clock.
 * On systick interrupt, the OS triggers a context switch
 * if preemption is enabled.
 */
void SysTick_Handler(void)
{
	uint32_t sr;
	TCB_t* delayedTask;
	
	sr = StartCritical();
	OS_sysTicks++;
	#if OS_USE_PREEMPTION == 1
	Yeild();
	#endif
	
	while (List_getSize(&delayedTasksList))
	{
			
		delayedTask = (TCB_t*)List_getLastElement( &delayedTasksList );
		
		if (delayedTask->sleepTimer > OS_sysTicks)
			break;
		
		removeTaskFromDelayedList(delayedTask);
		addTaskToReadyList(delayedTask);

	}

	EndCritical(sr);
}

/**
 * SW1 Interrupt callback.
 * Executes the SW1 tasks.
 */
void SW1_IRQ_Callback(void)
{
	if (SW1Task != NULL)
		SW1Task();
}

void SW2_IRQ_Callback(void)
{
	if (SW2Task != NULL)
		SW2Task();
}

/**
 * Determines the next thread to be executed.
 */
void Scheduler(void)
{
	//Round robin scheduling
	if (currentTCB->stack[0] != 0xABABABAB)
	{
		DisableInterrupts();
		for(;;){}
	}
	
	currentTCB = getNextReadyTask();
	
	
}

/**
 * Called when a thread exits without calling OS_Kill()
 */
void TaskExitErrorCallback(void)
{
	DisableInterrupts();
	for(;;);
}

static void IdleTask(void)
{
	for (;;)
	{
		
	}
}

/**
 * Initialize the function stack.
 * This function sets the values of the R0-R11, PSR, LR and PC registers
 * so that when the OS starts, the stack of the function seems like it was already
 * running.
 *
 * @param tcb			Pointer to task function.
 * @param taskFunction
 */
void InitializeStack(TCB_t* tcb, TaskFunction_t taskFunction)
{
	tcb->sp = &tcb->stack[OS_STACK_SIZE] ;

	tcb->sp--;
	*(tcb->sp) = 0x01000000;		//PSR
	tcb->sp--;
	*(tcb->sp) = (uint32_t)taskFunction;		//PC
	tcb->sp--;
	*(tcb->sp) = (uint32_t)TaskExitErrorCallback;	//LR

	tcb->sp -= 13;		//R11 - R0

	tcb->stack[0] =0xABABABAB;
}

/** @}*/

/**
 * @defgroup OS_Public_Functions	OS Public Functions
 * @{
 */

/**
 *	Initialize the OS Base clock (systick timer) to a granularity of 1ms.
 */
int OS_Init(void)
{
	DisableInterrupts();

	
	//Switch_Init();
	
	IntPrioritySet(FAULT_PENDSV, 0xE0);
	
	for (int i = 0; i < OS_MAX_NUM_TASKS ; i++)
	{
		TCB[i].stateListItem.list = NULL;
	}
	return 0;
}

void OS_PeriodicTimerInit(uint32_t freq)
{
	//Timer1_Init(80e6/freq);
}

/**
 * Start the OS kernel and start executing the first task in queue.
 * @param baseFrequency		Base frequency of the OS system clock.
 */
void OS_Start(uint32_t baseFrequency)
{
	SysTick_Config(SysCtlClockGet()/baseFrequency);
	
	//The first task when starting the OS for the first time
	//should always be in TCB[0]
	
	#if OS_USE_PRIORITY_SCHEDULER == 1
	OS_AddThread(&IdleTask, "Idle Task", OS_TASK_PRIORITY_IDLE);
	#endif
	
	currentTCB = getNextReadyTask();

	StartFirstTask();
}


/**
 * Suspend the current task.
 */
void OS_Suspend(void)
{
	uint32_t sr;
	sr = StartCritical();
	Yeild();
	EndCritical(sr);
}


/**
 * Add a thread to the OS kernel for execution.
 * @param taskFunction		Pointer to task function
 * @param name				Name of the thread (for debugging purposes)
 * @param priority			Task priority
 * 							@see OS_Task_Priority_t
 * @return			1 if the thread was created successfully
 *   						0 if there were no more TCBs available to be alloted to the thread
 */
uint32_t OS_AddThread(TaskFunction_t taskFunction, char* name, uint32_t priority)
{
	for (int i = 0 ; i < OS_MAX_NUM_TASKS ; i++)
	{

		if (TCB[i].stateListItem.list == NULL)
		{
			InitializeStack(&TCB[i], taskFunction);
			strncpy(TCB[i].name, name, sizeof(TCB[i].name));
			
			#if OS_USE_PRIORITY_SCHEDULER == 1
			TCB[i].priority = priority;
			#else
			TCB[i].priority = OS_TASK_PRIORITY_MEDIUM;
			#endif
			
			addTaskToReadyList(&TCB[i]);
			TCB[i].stateListItem.parent = &TCB[i];
			
			OS_TaskCount++;
			
			return 1;
		}

	}
	
	return 0;
}
	




/**
 * Add a task to be executed when SW1 is pressed.
 * @param taskFunction	Pointer to task function
 */
int OS_AddSW1Task(TaskFunction_t taskFunction)
{
	SW1Task = taskFunction;
	return 1;
}

/**
 * Add a task to be executed when SW2 is pressed.
 * @param taskFunction		Pointer to task function
 */
int OS_AddSW2Task(TaskFunction_t taskFunction)
{
	SW2Task = taskFunction;

	return 1;

}

/**
 * Unlink the SW2 task. Once unlinked, 
 * the task function must be passed again through
 * OS_AddSW2Task() function.
 */
int OS_RemoveSW2Task(void)
{
	SW2Task = NULL;
	return 1;
}


/**
 * Get the current OS clock time.
 */
uint32_t OS_getTickCount(void)
{
	return OS_sysTicks;
}


/**
 * Clear the OS clock.
 * Resets the clock to 0.
 */
uint32_t OS_clearTickCount(void)
{
	OS_sysTicks = 0;
	return OS_sysTicks;
}

/**
 * Suspend the current task for the specified number of ticks.
 * The function does not start executing exactly after ticks.
 * It will be ready to run after ticks period.
 * @param ticks		Number of ticks (has units of OS base time period)
 */
void OS_Sleep(uint32_t ticks)
{
	uint32_t sr = StartCritical();
	
	currentTCB->sleepTimer = ticks + OS_sysTicks;
	removeTaskFromReadyList(currentTCB);
	addTaskToDelayedList(currentTCB);
	Yeild();
	
	EndCritical(sr);
}

/**
 * Stop executing the current task and remove from OS kernel.
 */
void OS_Kill(void)
{
	long sr = StartCritical();

	removeTaskFromReadyList(currentTCB);
	Yeild();

	OS_TaskCount--;
	EndCritical(sr);
}



/** @}*/
