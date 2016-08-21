#ifndef __SYSTICK_H
#define __SYSTICK_H
#include "stm32f10x.h"
void SysTick_init(void);
void delay(__IO uint32_t nTime);
void System_Clock_Init(void);
#endif
