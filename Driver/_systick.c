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
	
	//��ʼ��
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);		 //ʹ���ⲿ8Hz����
	
	HSEStartUpStatus=RCC_WaitForHSEStartUp();
	
	if(HSEStartUpStatus == SUCCESS)
	{
		//SYSCLK����������HCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		//HCLK����������PCLK1��PCLK2
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div1);
		
		//���໷�� HSE 9��Ƶ(72MHZ)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		//���໷ʹ��
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{}
		
    //SYSCLKΪ���໷���			
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
			
		while(RCC_GetSYSCLKSource()!=0x08)
		{}
	}
	else
	{
		//��ʾHSE��ʼ��ʧ��
	}
}
