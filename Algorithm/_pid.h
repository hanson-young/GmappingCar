/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PID_H
#define __PID_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "_typedef.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#define X_DIR 1
#define Y_DIR 2
extern struct PID Pid_List[];

extern fp64 Vout[5]; 

/* Exported function prototype -----------------------------------------------*/

void PID_Clear(void);
float PID(float Error,struct PID pid);
float AnglePID(float Error,struct PID pid);
float DistancePID(float Error,struct PID pid);

#endif 

/**********************************END OF FILE*********************************/
