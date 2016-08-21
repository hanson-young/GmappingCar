/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPEDEF_H
#define __TYPEDEF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "../Driver/_can_database.h"
#include "math.h"
typedef enum
{
	PA = 0,	PB,PC,PD,PE,PF,PG
}eGPIO_t;


//点结构体
struct Point
{
	float x;
	float y;
};

//路径信息结构体
struct Path_Point
{
	struct Point position;	//位置
	float speeddir;		//速度方向
	float selfdir;			//车体方向
	float speed;			//速度大小
	
};

//陀螺仪信息结构体
struct Gyro_State
{
	float angle;			//角度
	float convert1;		//陀螺仪正转系数
	float convert2;		//陀螺仪反转系数
};
//码盘信息结构体
struct Encoder_State
{
	float dis;				//距离
	float convert1;		//正转系数
	float convert2;		//反转系数
	float r;				//车身自转时码盘的旋转半径
	float radian;			//码盘与车身坐标系Y轴正方向的夹角
};

//全场定位信息结构体
struct GPS_State
{
	struct Point position;	
	float radian;			//弧度
};

//PID结构体
struct PID
{
	float p;
	float i;
	float d;
	float mend;
	float ep1;
	float ep2;
};

//每个轮子对应的状态
struct Mecanum_State
{
	int port;				//对应的PWM端口
	int arg;				//方向系数
};

//按键值
enum M_KeyNumTpye
{
    key1 = 1,
    key2,
    key3,
    key4,
    keyback,//5
    emergency,//6
    
    key5,//7
    key6,//8
    key7,//9
    key8,//10
    empty1,//11
    empty2,//12
    
    key9,//13
    key0,
    point,
    sign,
    keydelete,
    keyOK
};


extern struct GPS_State GPS;
#endif 

/**********************************END OF FILE*********************************/
