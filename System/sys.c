#include "../SYSTEM/sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
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
