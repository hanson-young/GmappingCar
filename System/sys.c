#include "../SYSTEM/sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//系统中断分组设置化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/10
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************  
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

void SysTick_init(void)
{
	if (SysTick_Config(SystemCoreClock / 500))   //Setup SysTick Timer for 1 ms interrupts
	{ 
		/* Capture error */ 
		while (1);
	}
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
