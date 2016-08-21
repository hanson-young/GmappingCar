#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "math.h"
#include "_typedef.h"
#include "_math.h"
#include "../Driver/_pwm.h"
#include "_mecanum.h"
#define Acc_Limit_Enable 0

//下面的坐标系是机器人的局部坐标系
float Speed_X;				// mm/s
float Speed_Y;				// mm/s
float Speed_Rotation;		// (rad/s)/100

float coeff_x,coeff_y,coeff_r; //雅克比矩阵化简后的系数

float Max_Acc = 12.5;

// struct Mecanum_State Mec_FL={2, -1};		//四轮的状态参数
// struct Mecanum_State Mec_FR={3, -1};
// struct Mecanum_State Mec_BL={4, -1};
// struct Mecanum_State Mec_BR={1, -1};

struct Mecanum_State Mec_FL={1, 1};		//四轮的状态参数
struct Mecanum_State Mec_FR={2, 1};
struct Mecanum_State Mec_BL={3, 1};
struct Mecanum_State Mec_BR={4, 1};

static float robo_L = 636.6;//左右轮距
static float robo_W = 636.2;//前后轮距
static float wheel_r = 66.64;
static float wheel_rr = 9.56;

float motor_speed=0;

float Speed_Limit = 1000;

// double real_speed_x;
// double real_speed_y;
// double real_speed_rot;

void SetSpeed(float speed_x,float speed_y,float speed_rotation)
{
	/*角速度*/
	float fl_speed;
	float fr_speed;
	float bl_speed;
	float br_speed;
	
	float fl_delta;
	float fr_delta;
	float bl_delta;
	float br_delta;
	
// 	double real_set_fl_w;
// 	double real_set_fr_w;
// 	double real_set_bl_w;
// 	double real_set_br_w;
	
	static float fl_last=0;	//储存四轮的速度
	static float fr_last=0;
	static float bl_last=0;
	static float br_last=0;
	

	int coeff_arg[4][3] = 
	{	
		{ +1, -1, +1},
		{ +1, +1, +1},
		{ -1, +1, +1},
		{ -1, -1, +1},
	};
	float delta_max;			//速度变化量最大值
	float PWM_Max;				//生成的PWM后对应的速度最大值
	
	float PWM_FL,PWM_FR,PWM_BL,PWM_BR;
	float dec_rot = 1.0/100;//自转速度(rad/s)/100
	float cov = 60.0/(2*pi);//角速度和转速的转化系数

//	这两个参数不能随意设置 雅克比矩阵的各个正负号只是能确保正常运作，但是定量的分析上就会出问题
	coeff_x = coeff_y = 1 / (wheel_r + wheel_rr);
	coeff_r = (robo_L + robo_W) / 2 / (wheel_r + wheel_rr);

	
/****************************通过雅克比矩阵把速度分解到各个轮子******************************/
	fr_speed = (coeff_arg[0][0] * coeff_x * speed_x + coeff_arg[0][1] * coeff_y * speed_y + coeff_arg[0][2] * dec_rot * coeff_r * speed_rotation) * cov;
	fl_speed = (coeff_arg[1][0] * coeff_x * speed_x + coeff_arg[1][1] * coeff_y * speed_y + coeff_arg[1][2] * dec_rot * coeff_r * speed_rotation) * cov;
	bl_speed = (coeff_arg[2][0] * coeff_x * speed_x + coeff_arg[2][1] * coeff_y * speed_y + coeff_arg[2][2] * dec_rot * coeff_r * speed_rotation) * cov;
	br_speed = (coeff_arg[3][0] * coeff_x * speed_x + coeff_arg[3][1] * coeff_y * speed_y + coeff_arg[3][2] * dec_rot * coeff_r * speed_rotation) * cov;
/**************************对加速度进行限制(以理论速度限制赋速度)****************************/
	
	fl_delta = fl_speed - fl_last;
	fr_delta = fr_speed - fr_last;
	bl_delta = bl_speed - bl_last;
	br_delta = br_speed - br_last;
	
	delta_max=MaxFour(Abs(fl_delta),Abs(fr_delta),Abs(bl_delta),Abs(br_delta));
	
	if((delta_max >= Max_Acc) && (Acc_Limit_Enable==1))
	{
		fl_last += Max_Acc*(fl_delta/delta_max);
		fr_last += Max_Acc*(fr_delta/delta_max);
		bl_last += Max_Acc*(bl_delta/delta_max);
		br_last += Max_Acc*(br_delta/delta_max);
	} 
	else if((delta_max <= -Max_Acc) && (Acc_Limit_Enable==1))
	{
		fl_last -= Max_Acc*(fl_delta/delta_max);
		fr_last -= Max_Acc*(fr_delta/delta_max);
		bl_last -= Max_Acc*(bl_delta/delta_max);
		br_last -= Max_Acc*(br_delta/delta_max);
	}
	else
	{
		fl_last = fl_speed;
		fr_last = fr_speed;
		bl_last = bl_speed;
		br_last = br_speed;
	}

	fl_speed = fl_last * Mec_FL.arg;
	fr_speed = fr_last * Mec_FR.arg;
	bl_speed = bl_last * Mec_BL.arg;
	br_speed = br_last * Mec_BR.arg;	
	
	/**************************************************************************/	
 	PWM_FL = (50.0/6000)*14*fl_speed+50;
 	PWM_FR = (50.0/6000)*14*fr_speed+50;
 	PWM_BL = (50.0/6000)*14*bl_speed+50;
 	PWM_BR = (50.0/6000)*14*br_speed+50;

 	/************************对最终速度再次进行限制****************************/

 	PWM_Max=MaxFour(Abs(PWM_FL-50),Abs(PWM_FR-50),Abs(PWM_BL-50),Abs(PWM_BR-50));
 	if(PWM_Max >= 48)
 	{
 		PWM_FL = 48*(PWM_FL-50)/PWM_Max+50;
 		PWM_FR = 48*(PWM_FR-50)/PWM_Max+50;
 		PWM_BL = 48*(PWM_BL-50)/PWM_Max+50;
 		PWM_BR = 48*(PWM_BR-50)/PWM_Max+50;
 	}
	/*************************雅克比逆矩阵反解实际赋速度***********************/
	//根据赋给电机的PWM来反解实际转速
	// real_set_fl_w = (PWM_FL - 50)/(14*50.0)*6000 * Mec_FL.arg / cov;
	// real_set_fr_w = (PWM_FR - 50)/(14*50.0)*6000 * Mec_FR.arg / cov;	
	// real_set_bl_w = (PWM_BL - 50)/(14*50.0)*6000 * Mec_BL.arg / cov;
	// real_set_br_w = (PWM_BR - 50)/(14*50.0)*6000 * Mec_BR.arg / cov;
	// real_speed_x = (coeff_arg[0][0] * real_set_fl_w + coeff_arg[1][0] * real_set_fr_w + coeff_arg[2][0] * real_set_bl_w + coeff_arg[3][0] * real_set_br_w) * (wheel_r + wheel_rr) / 4;
	// real_speed_y = (coeff_arg[0][1] * real_set_fl_w + coeff_arg[1][1] * real_set_fr_w + coeff_arg[2][1] * real_set_bl_w + coeff_arg[3][1] * real_set_br_w) * (wheel_r + wheel_rr) / 4;
	// real_speed_rot = (coeff_arg[0][2] * real_set_fl_w + coeff_arg[1][2] * real_set_fr_w + coeff_arg[2][2] * real_set_bl_w + coeff_arg[3][2] * real_set_br_w) * (wheel_r + wheel_rr) / (4 * (robo_L + robo_W)/2);	
	/************************给相应的端口进行赋值******************************/
	
	PWM_SetDuty(Mec_FL.port,PWM_FL);
 	PWM_SetDuty(Mec_FR.port,PWM_FR);
 	PWM_SetDuty(Mec_BL.port,PWM_BL);
 	PWM_SetDuty(Mec_BR.port,PWM_BR);
}
/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
