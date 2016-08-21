/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "_mecanum.h"
#include "_delay.h"
#include "_ultrasonic.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */


void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
// void SysTick_Handler(void)
// {
// }

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
	



u8 startNum[2] = {0};
Gps_Data GpsData;
u8 cnt_flag = 0;
u8 gps_cnt = 0;
u8 rec_cnt = 0;
u8 tmp_data[4] = {0};

extern struct GPS_State GPS;

void USART3_IRQHandler(void)
{
	u8 array_cnt = 0;
	uint8_t rec_buf = 0;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
	  USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		rec_buf = USART_ReceiveData(USART3);
		USART_SendData(USART3,rec_buf);	
		if(rec_buf == 0x0D && startNum[0] == 0 && cnt_flag == 0)
		{
			startNum[0] = 1;
			startNum[1] = 1;
			tmp_data[0] = rec_buf;
		}
		if(rec_buf == 0x0A && startNum[0] == 1 && cnt_flag == 0)
		{
			if(rec_buf == 0x0A)
			{
				GpsData.rest[0] = 0x0D;
				GpsData.rest[1] = 0x0A;
				cnt_flag = 1;
				rec_cnt = 1;
			}
			startNum[1] = 0;
			startNum[0] = 0;				
		}
		if(cnt_flag == 1)
			GpsData.rest[rec_cnt++] = rec_buf;
		if(rec_cnt == 28 && cnt_flag == 1)
		{
			if(GpsData.rest[26] == 0x0A && GpsData.rest[27] == 0x0D)
			{
				gps_cnt++;
				for(array_cnt = 0; array_cnt < 4; array_cnt++)
				{
					GpsData.angleZ.data[array_cnt] = GpsData.rest[array_cnt + 2];
					GpsData.angleX.data[array_cnt] = GpsData.rest[array_cnt + 6];
					GpsData.angleY.data[array_cnt] = GpsData.rest[array_cnt + 10];
					GpsData.X.data[array_cnt] = GpsData.rest[array_cnt + 14];
					GpsData.Y.data[array_cnt] = GpsData.rest[array_cnt + 18];
					GpsData.W.data[array_cnt] = GpsData.rest[array_cnt + 22];
					GPS.radian = GpsData.angleZ.fl32 * 3.1416 / 180;
					GPS.position.x = GpsData.X.fl32;
					GPS.position.y = GpsData.Y.fl32;
				}
			}
			cnt_flag = 0;
		}
  }

}
void USART1_IRQHandler(void)
{
	uint8_t rec_buf = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		rec_buf = USART_ReceiveData(USART3);
	}
}


void TIM4_IRQHandler(void)
{
	u8 iCount = 0;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		for( ;iCount < ultrasonic_num; iCount++)
		{
			if(Ultrasonic[iCount].IsStart == 0x02)
			{
				if(Ultrasonic[iCount].GPIO_IN->IDR & Ultrasonic[iCount].Pin_In)
					Ultrasonic[iCount].trig_time++;
				else
				{
					Ultrasonic[iCount].IsStart = 0;
					//计算得到距离
					Ultrasonic[iCount].distance = Ultrasonic[iCount].trigfactor * Ultrasonic[iCount].trig_time;
					Ultrasonic[iCount].trig_time = 0;
					//如果距离小于阈值，则停车
					if(Ultrasonic[iCount].distance <= Ultrasonic[iCount].threthold)
					{
						SetSpeed(0,0,0);
						Ultrasonic[iCount].IsStop = 1;
					}
				}
			}
		}
	}
}

/************************0--4**************************/
void EXTI0_IRQHandler(void)
{
	EXTI->PR = EXTI_Line0;//PC0
	if(Ultrasonic[0].IsStart == 0x01)
	Ultrasonic[0].IsStart = 0x02;
}

void EXTI1_IRQHandler(void)
{
	EXTI->PR = EXTI_Line1;//PC1
	if(Ultrasonic[1].IsStart == 0x01)
	Ultrasonic[1].IsStart = 0x02;

}

void EXTI2_IRQHandler(void)
{
	EXTI->PR = EXTI_Line2;//PC2
	if(Ultrasonic[2].IsStart == 0x01)
	Ultrasonic[2].IsStart = 0x02;
}

void EXTI3_IRQHandler(void)
{
	 EXTI->PR = EXTI_Line3;////PC3
	 if(Ultrasonic[3].IsStart == 0x01)
	 Ultrasonic[3].IsStart = 0x02;
}
////////////////////////////////////////////////////////////////
void EXTI4_IRQHandler(void)//PB4
{
	EXTI->PR = EXTI_Line4;

}


/************************5--9**************************/
void EXTI9_5_IRQHandler(void)
{
	if((EXTI->PR & EXTI_Line5) != (uint32_t)RESET)//PB5
	{
		//up tongs
		EXTI->PR = EXTI_Line5;

	}
	
	if((EXTI->PR & EXTI_Line6) != (uint32_t)RESET)//PB6
	{
		EXTI->PR = EXTI_Line6;

	}

	if((EXTI->PR & EXTI_Line7) != (uint32_t)RESET)   
 	{
		EXTI->PR = EXTI_Line7;
	}

	if((EXTI->PR & EXTI_Line8) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line8;	
	}
	
	if((EXTI->PR & EXTI_Line9) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line9;
	}
}
/************************10--15**************************/
void EXTI15_10_IRQHandler(void)
{

	if((EXTI->PR & EXTI_Line10) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line10;//key3
	}
	
	if((EXTI->PR & EXTI_Line11) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line11;//key1
	}

	if((EXTI->PR & EXTI_Line12) != (uint32_t)RESET)   
 	{
		EXTI->PR = EXTI_Line12;//key2
	}

	if((EXTI->PR & EXTI_Line13) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line13; //key4
	}
	
	if((EXTI->PR & EXTI_Line14) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line14;//key5
	}
	
	if((EXTI->PR & EXTI_Line15) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line15;//key6
	}
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
