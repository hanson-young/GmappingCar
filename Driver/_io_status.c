#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "../System/_typedef.h"
#include "../Driver/_io_status.h"

sIO_t io_structs[] = 
{// io_type		gpiox			pinx 		trigger_type			nvic_channel 		PrePriority		SubPriority
//超声波外部中断IO配置信息（配合定时器完成高电平的测量）
		{INPUT_TRIG,			PC,			 0,					0x08,					EXTI0_IRQn,		  			2,					1			},
		{INPUT_TRIG,			PC,			 1,					0x08,					EXTI1_IRQn,		  			2,					2			},
		{INPUT_TRIG,			PC,			 2,					0x08,					EXTI2_IRQn,		  			2,					3			},
		{INPUT_TRIG,			PC,			 3,					0x08,					EXTI3_IRQn,		  			2,					4			},

//超声波触发信号配置信息

			{OUTPUT,		PD,			 12,				0xFF,					0xFF,		  	0xFF,					0xFF			},
			{OUTPUT,		PD,			 13,				0xFF,					0xFF,		  	0xFF,					0xFF		  },
			{OUTPUT,		PD,			 14,				0xFF,					0xFF,		  	0xFF,					0xFF			},
			{OUTPUT,		PD,			 15,				0xFF,					0xFF,		  	0xFF,					0xFF			},
//防撞条IO触发配置信息

// 		{INPUT,			PB,			 4,					0x0C,					EXTI4_IRQn,		  			4,					1			},
// 		{INPUT,			PB,			 5,					0x0C,					EXTI9_5_IRQn,		  		4,					2			},
// 		{INPUT,			PB,			 6,					0x0C,					EXTI9_5_IRQn,		  		4,					3			},

};

static const u8 io_total = sizeof(io_structs) / sizeof(io_structs[0]);

static void GPIO_Config(sIO_t *io_struct,u8 i)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_initstruct;
	NVIC_InitTypeDef NVIC_initstruct;	
	uint16_t pinx;
	GPIO_TypeDef *gpiox = (GPIO_TypeDef *)(GPIOA_BASE + (io_struct->gpiox << 10));//*0x400
	
	assert_param(IS_GPIO_ALL_PERIPH(gpiox));
	pinx = (uint16_t)(1 << io_struct->pinx);	
	RCC_APB2PeriphClockCmd((uint16_t)(1 << io_struct->gpiox << 2),ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin = pinx; 
	if(io_struct->io_type == INPUT_TRIG)
	{
		switch(io_struct->trigger_type)
		{
			case 0x0C:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
				EXTI_initstruct.EXTI_Trigger = EXTI_Trigger_Falling;
				break;
			case 0x08:
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
				EXTI_initstruct.EXTI_Trigger = EXTI_Trigger_Rising;
				break;
			default:
				break;
		}
		GPIO_Init(gpiox, &GPIO_InitStructure); 
		if(i == 0)
			EXTI_DeInit();
		EXTI->PR = ((uint32_t)1 << io_struct->pinx);
		GPIO_EXTILineConfig(io_struct->gpiox,io_struct->pinx);
		EXTI_initstruct.EXTI_Line = (uint32_t)1 << io_struct->pinx;
		switch(io_struct->trigger_type)
		{
			case 0x0C:
				EXTI_initstruct.EXTI_Trigger = EXTI_Trigger_Falling;
				break;
			case 0x08:
				EXTI_initstruct.EXTI_Trigger = EXTI_Trigger_Rising;
				break;
			default:
				break;
		}
		EXTI_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_initstruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_initstruct);
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_initstruct.NVIC_IRQChannel = io_struct->nvic_channel;
		NVIC_initstruct.NVIC_IRQChannelPreemptionPriority = io_struct->PrePriority;
		NVIC_initstruct.NVIC_IRQChannelSubPriority = io_struct->SubPriority;
		NVIC_initstruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_initstruct);
	}
	else if(io_struct->io_type == OUTPUT)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(gpiox, &GPIO_InitStructure); 
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
		GPIO_Init(gpiox, &GPIO_InitStructure); 
	}
}

void IO_Init(void)
{
	uint8_t i = 0;
	for(i = 0; i < io_total; i++)
	{
		GPIO_Config(&io_structs[i],i);
	}
}
/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
