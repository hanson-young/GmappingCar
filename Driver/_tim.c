/**
  ******************************************************************************
  * @file    _tim.c
  * @author  flysnow
  * @version V1.0
  * @date    2016-8-21
  * @brief   tim4 is used to counting
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "../Driver/_tim.h"

static void TIM4_Configuration(void)
{
	//4ms
	TIM_TimeBaseInitTypeDef TIM_Instruction;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_Instruction.TIM_Period=399;
	TIM_Instruction.TIM_Prescaler=719;
	TIM_Instruction.TIM_ClockDivision=0;
	TIM_Instruction.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_Instruction);
}

static void TIM4_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_Initstruct.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Initstruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_Initstruct);
}

void TIM_Config(void)
{
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	TIM4_Configuration();
	TIM4_NVIC_Configuration();
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
}

/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
