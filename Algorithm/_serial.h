#ifndef __USERTIMER_H_
#define __USERTIMER_H_
#include "stm32f10x_usart.h"
#include "_uart.h"
#include "stdio.h"
#include "stdlib.h"
#include "_typedef.h"
typedef struct Serial{
    char SdTPC[64];
		char Head;
    double tickClock;
    void (*clockTick)(struct Serial *self);
		void (*sendGpsData)(struct Serial *self);
    void (*sendTimeStamp)(struct Serial *self);
    int (*init)(struct Serial *self);
}Serial;

extern struct GPS_State GPS;
static void clockTick(Serial *self);
void sendGpsData(Serial *self);
void sendTimeStamp(Serial *self);
void SerialDelete(Serial *self);
Serial *SerialCreate();
void Com_Puts(u8 uartx, char * str);
#endif