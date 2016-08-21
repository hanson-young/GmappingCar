/**
  ******************************************************************************
  * @file    _uart.c
  * @author  flysnow
  * @version V1.0
  * @date    2016-8-21
  * @brief   This file contains all the configuration of uarts
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "../System/_typedef.h"
#include "../Driver/_uart.h"

typedef struct 
{
	u8 usartx;//usarts口
	uint32_t baudrate;//配置波特率
	u8 gpiox;//配置IO
	u8 tx;//配置发送管脚
	u8 rx;//配置接收管脚
	uint8_t nvic_channel;//中断通道
	uint8_t PrePriority;//抢占优先级
	uint8_t SubPriority; //响应优先级
	uint8_t IsRemp;//是否开启重映射
	uint32_t PartialRemap;//重映射时钟
}sUSART_t;

/*配置UART结构体数据库*/
sUSART_t usart_structs[] = 
{// uartx		baudrate	gpiox 	tx			rx 		nvic_channel		PrePriority		SubPriority    IsRemap					PartialRemap

			{1,			115200,			PB,		6,			7,		USART1_IRQn,			1,							0,					1,			GPIO_Remap_USART1					},
//			{2,			115200,			PD,		5,			6,		USART2_IRQn,			1,							1,					1,			GPIO_Remap_USART2					},
			{3,			115200,			PC,		10,			11,		USART3_IRQn,			1,							2,					1,			GPIO_PartialRemap_USART3	},
//			{2,			115200,			PA,		2,		3,		USART2_IRQn,			1,							2,					0,			GPIO_PartialRemap_USART3	},
};

/*得到结构体数据库信息条数*/
static const u8 usart_total = sizeof(usart_structs) / sizeof(usart_structs[0]);

/*获取挂载总线APB1或APB2*/
static u8 Get_APB_USARTx(u8 usartx)
{

    if(usartx <= 1)
        return 2;
    else if(usartx <= 5) 
        return 1;
    else 
        return 0xff;
}

/*获取RCC外设时钟*/
static u32 Get_Usart_RCC(u8 usartx)
{
    if(usartx <= 1)
        return RCC_APB2Periph_USART1;
    else if(usartx <= 2)
        return RCC_APB1Periph_USART2;
    else if(usartx <= 3)
        return RCC_APB1Periph_USART3;
    else if(usartx <= 4)
				return RCC_APB1Periph_UART4;
    else if(usartx <= 5)
				return RCC_APB1Periph_UART5;
		else
        return 0xff;
}

/*获取UART基地址*/
static USART_TypeDef *Get_USARTx(u8 usartx)
{
    switch (usartx)
    {
        case 1:  return USART1;
        case 2:  return USART2;
        case 3:  return USART3;
				case 4:  return UART4;
				case 5:  return UART5;
        default: break;
    }
    return (USART_TypeDef *)0;
}

/*单个uart初始化函数*/
static void Uart_Init(sUSART_t *usart_struct)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	uint16_t pin_rx;
	uint16_t pin_tx;
	u8  APBx;
	USART_TypeDef *usartx = Get_USARTx(usart_struct->usartx);
	GPIO_TypeDef *gpiox = (GPIO_TypeDef *)(GPIOA_BASE + (usart_struct->gpiox << 10));//*0x400
	assert_param(IS_GPIO_ALL_PERIPH(gpiox));
	pin_rx = (uint16_t)(1 << usart_struct->rx);	
	pin_tx = (uint16_t)(1 << usart_struct->tx);

	APBx = Get_APB_USARTx(usart_struct->usartx);
	if(APBx==1)
			RCC_APB1PeriphClockCmd(Get_Usart_RCC(usart_struct->usartx), ENABLE); 
	else
			RCC_APB2PeriphClockCmd(Get_Usart_RCC(usart_struct->usartx), ENABLE); 
	if(usart_struct->IsRemp)
	{
		RCC_APB2PeriphClockCmd((uint16_t)(1 << usart_struct->gpiox << 2)|RCC_APB2Periph_AFIO,ENABLE);
		GPIO_PinRemapConfig(usart_struct->PartialRemap, ENABLE);
	}
 //USARTX_TX
	GPIO_InitStructure.GPIO_Pin = pin_tx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(gpiox, &GPIO_InitStructure);
	//USARTX_RX
	GPIO_InitStructure.GPIO_Pin = pin_rx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(gpiox, &GPIO_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = usart_struct->nvic_channel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=usart_struct->PrePriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = usart_struct->SubPriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;	
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;	
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;	
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;	
  USART_ClockInit(usartx, &USART_ClockInitStructure);

	USART_InitStructure.USART_BaudRate = usart_struct->baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(usartx, &USART_InitStructure);
	USART_ITConfig(usartx,USART_IT_RXNE,ENABLE);  
	USART_Cmd(usartx, ENABLE);
}

/*所有uart初始化*/
void Usart_Init(void)
{
	u8 i;
	for(i = 0; i < usart_total; i++)
	{
			Uart_Init(&usart_structs[i]);
	}
}

/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
