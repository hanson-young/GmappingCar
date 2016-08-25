/**
  ******************************************************************************
  * @file    __task.c
  * @author  flysnow
  * @version V1.0
  * @date    2016-8-7
  * @brief   This file contains all the configuration of pwm
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "includes.h"
#include "../System/sys.h" 	
#include "../Driver/_delay.h"	
#include "../Task/_task.h" 
#include "../Driver/_delay.h"
#include "../Driver/_can.h"
#include "../Algorithm/_mecanum.h"
#include "../Driver/_pwm.h"
#include "../Driver/_uart.h"
#include "../Driver/_io_status.h"

#include "../Driver/_tim.h"
#include "../Driver/_ps2.h"
#include "../Driver/_ultrasonic.h"
#include "../Algorithm/_pid.h"

#include "_ctiic.h"
#include "_touch.h"
#include "_24cxx.h"
#include "_myiic.h"
#include "_ott2001a.h"
#include "_lcd.h"
#include "_lcdio.h"
#include "_keep.h"
#include "stm32f10x_it.h"
#include "../Algorithm/_math.h"
#include "_serial.h"


void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=RED;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  POINT_COLOR=RED;//设置画笔蓝色 
}

/////////////////////////UCOSII任务设置///////////////////////////////////

void TaskStart(void *para);	
void ManualTask(void *para);
void AutoTask(void *para);
void LcdTask(void *para);

sUcosTask_t TaskStructs[] =
{
	{.task_prio = 0x10,		.task_status = 0,		.FunTask = TaskStart   },
	{.task_prio = 0x08,		.task_status = 0,		.FunTask = LcdTask    },
	{.task_prio = 0x07,		.task_status = 0,		.FunTask = ManualTask  },
	{.task_prio = 0x06,		.task_status = 0,		.FunTask = AutoTask    },
};
	  
const u8 taskNum = sizeof(TaskStructs)/sizeof(TaskStructs[0]);
//开始任务
void TaskStart(void *para)
{
  OS_CPU_SR cpu_sr=0;
  OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断) 
	u8 iCount = 1;
	for(; iCount < taskNum; iCount++)
		OSTaskCreate(TaskStructs[iCount].FunTask,
								(void *)0,
								(OS_STK*)&TaskStructs[iCount].task_stack[STK_SIZE - 1],
								TaskStructs[iCount].task_prio);		
								
	OSTaskSuspend(TaskStructs[0].task_prio);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

u8 testflag1 = 0;
u8 testflag2 = 0;
u8 testflag3 = 0;
	u8 PS2_key;
char SdTPC[64] = {0};
extern PC_Data PcData;
void ManualTask(void *para)
{	 	
	Ultrasonic[0].ClcUtralData(0);
 	Ultrasonic[1].ClcUtralData(1);
 	Ultrasonic[2].ClcUtralData(2);
 	Ultrasonic[3].ClcUtralData(3);
	while(1)
	{
// 		USART_SendData(USART1, 0x55);
// 		USART_SendData(USART2, 0x55);
// 		USART_SendData(USART3, 0x55);
  	PS2_key=PS2_DataKey();
    sprintf(SdTPC, "(%d,%d,%d,%d,1)\n\t", (int)GPS.position.x, (int)GPS.position.y, (int)GPS.radian, 100);
    Com_Puts(1, SdTPC);
    SdTPC[0] = 0;
//		Speed_X = (Data[5] - 0x80)* 20;
//		Speed_Y = (0x80 - Data[6])* 20;
//		SetSpeed(1000,Speed_Y,Speed_Rotation);
 		SetSpeed(PcData.speed_x.fl32*10,PcData.speed_y.fl32*10,PcData.speed_rot.fl32*10);
 		Ultrasonic[0].UtralMea(0);
		Ultrasonic[0].Ultrafilter(0);
 		Ultrasonic[1].UtralMea(1);
		Ultrasonic[1].Ultrafilter(1);
 		Ultrasonic[2].UtralMea(2);
		Ultrasonic[2].Ultrafilter(2);
 		Ultrasonic[3].UtralMea(3);
		Ultrasonic[3].Ultrafilter(3);
		delay_ms(10);
	};
}

void AutoTask(void *para)
{	  
//	u8 iCount = 0;
//	Load_Drow_Dialog();	 	
	while(1)
	{
//		if(!iCount)
//			KeepTest();
//		iCount++;
		delay_ms(5);
	}

}

void LcdTask(void *para)
{
//	while(1)
//	{
		//LCD_Clear(WHITE);
//		SetCursor(0,40);
//		LCD_WriteString("GPS.y:");
//		LCD_WriteFloat(GPS.position.x);
//		SetCursor(0,70);
//		LCD_WriteString("GPS.x:");
//		LCD_WriteFloat(GPS.position.y);
//		SetCursor(0,100);
//		LCD_WriteString("GPS.a:");
//		LCD_WriteFloat(GPS.radian);
//		delay_ms(300);
//	}
		static struct Point end_point={500,0};
	static double aim_angle=0;
  static double speed_max=500;

	LCD_Clear(WHITE);
	PID_Clear();

	SetKeep(end_point,aim_angle*pi/180,speed_max);
	 
	while(1)
	{
		
		KeepPoint();
		SetCursor(0,130);
		LCD_WriteString("now_distance:");
		LCD_WriteFloat(GetLength(GPS.position,end_point));//输出实时距离
		if((GetLength(GPS.position,end_point) < 50)&&((GPS.radian-aim_angle*pi/180<0.02)&&(GPS.radian-aim_angle*pi/180>-0.02)))
		{
			SPEED_STOP;
//			SetSpeed(0,0,0);
			break;
		}  
		delay_ms(15);
	}
}