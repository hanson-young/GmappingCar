#ifndef _COMMAND_H
#define _COMMAND_H
	#include "stm32f10x.h"
	
	void W_SONIC_FUNC(void);
	void S_SONIC_FUNC(u16 data,u8 i);
	void S_SWITCH_FUNC(void);
	void W_PWM_FUNC(void);
	void INF_DISTAN(void);
	
#endif
