#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "_io_status.h"

struct Ultrasonic
{
	unsigned char Id;
	
	unsigned int trig_time;
	GPIO_TypeDef * GPIO_IN;
	uint16_t Pin_In;
	GPIO_TypeDef * GPIO_OUT;
	uint16_t Pin_Out;

  float trigfactor;
	float distance;
	unsigned char IsStop;
	float threthold;
  unsigned char   IsStart;
	float filterdistance;

  void  (*ClcUtralData)(unsigned char parg);
  void  (*UtralMea)(unsigned char parg);
	void  (*Ultrafilter)(unsigned char parg);
};
void bubble_sort(float a[], u8 n);
extern struct Ultrasonic Ultrasonic[];
extern unsigned char ultrasonic_num;
#define TOTAL 11
#define MEAN 3
#define SIDE 1
#define Threshold 21
#endif

