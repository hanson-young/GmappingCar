#ifndef _CAN_H
#define _CAN_H
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void Can1_Init(void);
void Can_SendData(CanTxMsg* TxM);
	
#endif
