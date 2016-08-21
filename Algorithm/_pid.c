
/* Includes ------------------------------------------------------------------*/
#include "math.h"
#include "_math.h"
#include "_typedef.h"

struct PID Pid_List[]=
{
	{2.0,1000000,0},	//0  Keep角度PID
	{6,1000000,0},		//1  Keep距离PID
	
	{4.0,1000000,0},	//2  Keep角度PID
	{6,1000000,0},		//3  Keep距离PID

	{0.60,1000000,0},	//4  Keep角度PID
	{2.5,1000000,2.5},		//5  Keep距离PID
}; 
																								
static float A_ErrorPast1=0;
static float A_ErrorPast2=0;
static float A_Mend=0;
static float D_ErrorPast1=0;
static float D_ErrorPast2=0;
static float D_Mend=0;

fp64 Vout[5]={0.0};

u8 pid_total = sizeof(Pid_List)/sizeof(Pid_List[0]);
/*
 * 函数名: PID_Clear
 * 描  述: 将PID存储的误差值清除
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void PID_Clear()
{
	int i;
	
	for(i = 0; i < pid_total; i++)
	{
		Pid_List[i].mend = 0;
		Pid_List[i].ep1 = 0;
		Pid_List[i].ep2 = 0;
	}
	
	A_ErrorPast1 = 0.0;
	A_ErrorPast2 = 0.0;
	A_Mend = 0.0;
	D_ErrorPast1 = 0.0;
	D_ErrorPast2 = 0.0;
	D_Mend = 0.0;
	
	for(i=0;i<5;i++)
	   Vout[i]=0;
}

/*
 * 函数名: PID
 * 描  述: 位置式PID调整，利用了增量式的递推
 * 输  入: -Error 误差量
 *		   -pid pid参数
 * 输  出: 反馈结果
 * 调  用: 外部调用
 */
float PID(float Error,struct PID pid)
{
	float A=0;
	float B=0;
	float C=0;
	float delta=0;
	
	A = (pid.p*(1+1/pid.i+pid.d));
	B = -pid.p*(1+2*pid.d);
	C = pid.p*pid.d;
	
	delta = A*Error + B*pid.ep1 + C*pid.ep2;
	pid.mend += delta;
	
	pid.ep2 = pid.ep1;
	pid.ep1 = Error;
	
	return pid.mend;
}


/*
 * 函数名: AnglePID
 * 描  述: 位置式角度PID调整，利用了增量式的递推
 * 输  入: -Error 误差量
 *		   -pid pid参数
 * 输  出: 角速度
 * 调  用: 外部调用
 */
float AnglePID(float Error,struct PID pid)
{
	float A=0;
	float B=0;
	float C=0;
	float delta=0;

	
	A = (pid.p*(1+1/pid.i+pid.d));
	B = -pid.p*(1+2*pid.d);
	C = pid.p*pid.d;
	
	delta = A*Error + B*A_ErrorPast1 + C*A_ErrorPast2;
	A_Mend += delta;
	
	A_ErrorPast2=A_ErrorPast1;
	A_ErrorPast1=Error;
	
	return A_Mend;
}

/*
 * 函数名: DistancePID
 * 描  述: 位置式距离PID调整，利用了增量式的递推
 * 输  入: -Error 误差量
 *		   -pid pid参数
 * 输  出: 速度
 * 调  用: 外部调用
 */
float DistancePID(float Error,struct PID pid)
{
	float A=0;
	float B=0;
	float C=0;
	float delta=0;
	
	A = (pid.p*(1+1/pid.i+pid.d));
	B = -pid.p*(1+2*pid.d);
	C = pid.p*pid.d;
	
	delta = A*Error + B*D_ErrorPast1 + C*D_ErrorPast2;
	D_Mend+=delta;
	
	D_ErrorPast2=D_ErrorPast1;
	D_ErrorPast1=Error;
	
	return D_Mend;
}

/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
