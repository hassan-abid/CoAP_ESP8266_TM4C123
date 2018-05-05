
#ifndef __OS_PROCESS_H_
#define __OS_PROCESS_H_

typedef struct PCB_t {
	
	uint32_t PID;
	char ID[16];
	void* code;
	void* data;
	void* Heap;
	uint32_t priority;

	uint32_t threadCount;
	
}PCB_t;

#endif
