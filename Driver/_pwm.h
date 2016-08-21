/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/

void PWM_Init(void);
u32  PWM_GetDuty(u8 PWMx);
int  PWM_SetDuty(u8 PWMx, float Duty);

#endif 

/**********************************END OF FILE*********************************/
