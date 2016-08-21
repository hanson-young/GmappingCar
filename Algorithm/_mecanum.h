/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MECANUM_H
#define __MECANUM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "_typedef.h"
/* Exported macro ------------------------------------------------------------*/

#define SPEED_STOP Speed_X = Speed_Y = Speed_Rotation = 0

/* Exported variables --------------------------------------------------------*/

extern float Speed_X;
extern float Speed_Y;
extern float Speed_Rotation;

extern struct Mecanum_State Mec_FL;		//四轮的状态参数
extern struct Mecanum_State Mec_FR;
extern struct Mecanum_State Mec_BL;
extern struct Mecanum_State Mec_BR;
extern float Max_Acc;
extern float real_speed_x;
extern float real_speed_y;
extern float real_speed_rot;

/* Exported function prototype -----------------------------------------------*/
void SetSpeed(float speed_x,float speed_y,float speed_rotation);

#endif 

/**********************************END OF FILE*********************************/
