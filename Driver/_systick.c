/**
  ******************************************************************************
  * @file    _systick.c
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
#include "_systick.h"
static __IO uint32_t TimingDelay;

void SysTick_init(void)
{
	if (SysTick_Config(SystemCoreClock / 500))   //Setup SysTick Timer for 1 ms interrupts
	{ 
		/* Capture error */ 
		while (1);
	}
}

void delay(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

void System_Clock_Init(void)
{
	ErrorStatus HSEStartUpStatus;
	
	//初始化
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);		 //使能外部8Hz晶振
	
	HSEStartUpStatus=RCC_WaitForHSEStartUp();
	
	if(HSEStartUpStatus == SUCCESS)
	{
		//SYSCLK基础上设置HCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		//HCLK基础上设置PCLK1和PCLK2
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div1);
		
		//锁相环对 HSE 9倍频(72MHZ)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		//锁相环使能
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{}
		
    //SYSCLK为锁相环输出			
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
			
		while(RCC_GetSYSCLKSource()!=0x08)
		{}
	}
	else
	{
		//提示HSE初始化失败
	}
}
