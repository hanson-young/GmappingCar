/**
  ******************************************************************************
  * @file    _pwm.c
  * @author  flysnow
  * @version V1.0
  * @date    2016-8-21
  * @brief   This file contains all the configuration of pwm
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "../System/_typedef.h"
#include "../Driver/_pwm.h"

typedef struct 
{
	u32 clk_freq;//配置分频后时钟频率
	u32 pwm_freq;//配置pwm波频率
	u8 timx;//pwm波口对应的定时器
	u8 chx;//定时器通道
	u8 gpiox;//pwm波IO口
	u8 pinx;//pwm波管脚
}sPWM_t;

/*配置PWM波结构体数据库*/
sPWM_t pwm_structs[] = 
{// clk_freq		pwm_freq	timx 	chx		gpiox 	pinx
  /*底盘电机*/
	{1000000, 		200, 		2, 		1, 		PA, 	0	},
	{1000000, 		200, 		2, 		2, 		PA, 	1	},
	{1000000, 		200, 		2, 		3, 		PA, 	2	},
	{1000000, 		200, 		2, 		4, 		PA, 	3	},
	/*上层电机*/
// 	{1000000, 		100, 		3, 		1, 		PA, 	6	},
// 	{1000000, 		100, 		3, 		2, 		PA, 	7	},
// 	{1000000, 		100, 		3, 		3, 		PB, 	0	},
// 	{1000000, 		100, 		3, 		4, 		PB, 	1	},
	/*其他PWM*/

};

/*得到结构体数据库信息条数*/
static const u8 pwm_total = sizeof(pwm_structs) / sizeof(pwm_structs[0]);

/*获取挂载总线APB1或APB2*/
static u8 Get_APB_TIMx(u8 timx)
{
    if(timx>14) 
        return 0xff;
    else if((timx>=2 && timx<=7)||(timx>=12 && timx<=14))
        return 1;
    else 
        return 2;
}

/*获取RCC外设时钟*/
static u32 Get_Tim_RCC(u8 timx)
{
    if(timx <= 1)
        return ((uint32_t)0x00000800);
    else if(timx <= 7)
        return (uint32_t)(1 << (timx - 2));
    else if(timx <= 8)
        return ((uint32_t)0x00002000);
    else if(timx <= 11)
        return (u32)(1 << (timx + 10));
    else if(timx <= 14)
        return (u32)(1 << (timx - 6));
    else
        return 0xff;
}

/*配置pwm波口*/
static void PWM_GPIO_Init(sPWM_t *pwm_struct)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t pin_bit;
	GPIO_TypeDef *gpiox = (GPIO_TypeDef *)(GPIOA_BASE + (pwm_struct->gpiox << 10));//*0x400
	assert_param(IS_GPIO_ALL_PERIPH(gpiox));
	pin_bit = (uint16_t)(1 << pwm_struct->pinx);
	assert_param(IS_GPIO_PIN(pin_bit));  
	// 开启时钟
	RCC_APB2PeriphClockCmd((uint16_t)(1 << pwm_structs->gpiox << 2)|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = pin_bit;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(gpiox, &GPIO_InitStructure);
}

/*获取TIM基地址*/
static TIM_TypeDef *Get_TIMx(u8 timx)
{
    switch (timx)
    {
        case 1:  return TIM1;
        case 2:  return TIM2;
        case 3:  return TIM3;
        case 4:  return TIM4;
        case 5:  return TIM5;
        case 6:  return TIM6;
        case 7:  return TIM7;
        case 8:  return TIM8;
        case 9:  return TIM9;
        case 10: return TIM10;
        case 11: return TIM11;
        case 12: return TIM12;
        case 13: return TIM13;
        case 14: return TIM14;
        default: break;
    }
    return (TIM_TypeDef *)0;
}


/*初始化定时器*/
static void PWM_Clk_Init(sPWM_t *pwm_struct)
{   
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    u16 presc;
    u16 cycle;
    u32 p_freq;
    u8  APBx;

    APBx = Get_APB_TIMx(pwm_struct->timx);
    p_freq = (APBx == 1) ? SystemCoreClock : SystemCoreClock;
    presc  = p_freq / pwm_struct->clk_freq - 1;
    cycle  = pwm_struct->clk_freq / pwm_struct->pwm_freq;
    if(APBx==1)
        RCC_APB1PeriphClockCmd(Get_Tim_RCC(pwm_struct->timx), ENABLE); 
    else
        RCC_APB2PeriphClockCmd(Get_Tim_RCC(pwm_struct->timx), ENABLE); 
    
    TIM_TimeBaseStructure.TIM_Period = cycle;
    TIM_TimeBaseStructure.TIM_Prescaler = presc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(Get_TIMx(pwm_struct->timx), &TIM_TimeBaseStructure);
    
}

/*初始化PWM*/
static void PWM_OCInit(sPWM_t *pwm_struct)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TypeDef *timx = Get_TIMx(pwm_struct->timx);
    assert_param(IS_TIM_ALL_PERIPH(timx)); 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = pwm_struct->clk_freq / pwm_struct->pwm_freq / 2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    switch (pwm_struct->chx)
    {
        case 1:
            TIM_OC1Init(timx, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(timx, TIM_OCPreload_Enable);
            break;
        case 2:
            TIM_OC2Init(timx, &TIM_OCInitStructure);
            TIM_OC2PreloadConfig(timx, TIM_OCPreload_Enable);
            break;
        case 3:
            TIM_OC3Init(timx, &TIM_OCInitStructure);
            TIM_OC3PreloadConfig(timx, TIM_OCPreload_Enable);            
            break;
        case 4:
            TIM_OC4Init(timx, &TIM_OCInitStructure);
            TIM_OC4PreloadConfig(timx, TIM_OCPreload_Enable);            
            break;
        default:
            break;
    }
    TIM_Cmd(timx, ENABLE);
}

/*初始化所有结构体包含的pwm*/
void PWM_Init()
{
    u8 i;
    for(i = 0; i < pwm_total; i++)
    {
        PWM_GPIO_Init(&pwm_structs[i]);
        PWM_Clk_Init(&pwm_structs[i]);
        PWM_OCInit(&pwm_structs[i]);
    }
}

/*获取pwm波周期*/
static u32 Get_Cycle(u8 pwmx)
{
    assert_param(pwmx>=1 && pwmx<=4);
    return pwm_structs[pwmx-1].clk_freq / pwm_structs[pwmx-1].pwm_freq;
}

/*获取pwm波占空比*/
int PWM_SetDuty(u8 pwmx, float duty)
{
    TIM_TypeDef * timx = Get_TIMx(pwm_structs[pwmx-1].timx);
    u32 ccrx = duty *Get_Cycle(pwmx) / 100;
    
    assert_param(pwmx>=1 && pwmx<=pwm_total);
    assert_param(duty<=100);
    switch (pwm_structs[pwmx-1].chx)
    {
        case 1: timx->CCR1 = ccrx; break;
        case 2: timx->CCR2 = ccrx; break;
        case 3: timx->CCR3 = ccrx; break;
        case 4: timx->CCR4 = ccrx; break;
        default:    break;
    }   
    
    return 1;
}

/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
