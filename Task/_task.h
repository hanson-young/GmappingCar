#ifndef __TASK_H
#define __TASK_H
#include "includes.h"
#include "stm32f10x.h"
#define STK_SIZE 64
typedef struct
{
	unsigned char task_prio;
	unsigned char task_status;
	OS_STK task_stack[STK_SIZE];
	void (*FunTask)(void *para);
}sUcosTask_t;

extern sUcosTask_t TaskStructs[];
#endif

