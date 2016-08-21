#ifndef __KEEP_H
#define __KEEP_H
#include "stm32f10x.h"
#include "_typedef.h"
void SetKeep(struct Point end_point,float aim_radian,float speed_max);
void KeepPoint(void);
void KeepTest(void);
#endif