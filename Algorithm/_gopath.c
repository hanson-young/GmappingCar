#include "_typedef.h"
#include "math.h"
#include "../Algorithm/_mecanum.h"
#include "../Algorithm/_math.h"
#include "../Algorithm/_pid.h"
#include "../Algorithm/_pathpoint.h"
#define Acc_Limit_Enable_new 0
struct GPS_State GPS;
float S_Factor = 0.9;
extern struct Path_Point* PointRoute;
int Point_NowNum;
int Point_StartNum;
int Point_EndNum;
int count = 0; 
extern u8 WalkGround;
extern u8 RouteFinish;//对其置零即可使路径停止
/*
 * 函数名: SetPointPath
 * 描  述: 初始化跑点阵路径
 * 输  入: -sta_num 路径开始的点
 *		   -end_num 路径结束的点
 * 输  出: 无
 * 调  用: 外部调用
 */
void SetPointPath(int sta_num,int end_num)
{
	Point_StartNum = sta_num;
	Point_EndNum = end_num;
	Point_NowNum = sta_num;
	PID_Clear();
	RouteFinish=1;//1表示路径未结束，0表示路径结束
}

void GoPointPath(struct PID Pid_ang,struct PID Pid_dis)
{
	int count_scope=9;				//搜索范围
	int count_delta=0;
	int i;
	
	float phi;									//
	float speed,speed_x,speed_y;				//世界坐标里的速度与分速度
	float speed_robot_x,speed_robot_y;			//转换到机器人坐标的分速度
	
	float error_angle,error_dis;				//角度误差与距离误差
	float v_err_x,v_err_y;						//PID调整速度的分量
	
#if Acc_Limit_Enable_new
	static struct GPS_State last_GPS;
	float delta_s;
	float delta_a;
	float robot_speed;
	float w;
	float max_acc = 200;
	float max_acc_w = 50;
#endif	

	//搜索下一个目标点
	for(i=0;i<count_scope;i++)
	{
		if((Point_NowNum+i) >= Point_EndNum) break;
		if(GetLength(PointRoute[Point_NowNum+i+1].position,GPS.position)
			<= GetLength(PointRoute[Point_NowNum+i].position,GPS.position))
			count_delta = i+1;
	}
	Point_NowNum += count_delta;
	
	//求速度在x,y方向上的分量
	speed = PointRoute[Point_NowNum].speed*S_Factor;//*Speed_Factor

	/****************************对加速度进行限制(以实际速度限制赋速度)******************************/
	
#if Acc_Limit_Enable_new
	
	delta_s = GetLength(last_GPS.position,GPS.position);
	robot_speed = delta_s/((TIM2->CNT)/1000000.0);
	robot_speed = robot_speed/1.3;
	
	delta_a = GPS.radian - last_GPS.radian;
	w = delta_a/((TIM2->CNT)/1000000.0);

	last_GPS = GPS;
	
	if(speed-robot_speed > max_acc)
	{
		speed = robot_speed + max_acc;
	}
	else if(speed-robot_speed < -max_acc)
	{
		speed = robot_speed - max_acc;
	}
#endif
	
	speed_x = speed*cos(PointRoute[Point_NowNum].speeddir);
	speed_y = speed*sin(PointRoute[Point_NowNum].speeddir);
	
	//角度误差和距离误差
	error_angle = (PointRoute[Point_NowNum].selfdir)-GPS.radian;// self_flag (PointRoute[Point_NowNum].selfdir)
	error_dis = (GPS.position.x-PointRoute[Point_NowNum].position.x)*sin(PointRoute[Point_NowNum].speeddir)
				-(GPS.position.y-PointRoute[Point_NowNum].position.y)*cos(PointRoute[Point_NowNum].speeddir);
	
	if(error_angle>pi)
		error_angle-=2*pi;
	if(error_angle<-pi)
		error_angle+=2*pi;
	
	Vout[0] = 1000*AnglePID(error_angle,Pid_ang);
	Vout[1] = DistancePID(error_dis,Pid_dis);
	
	if(Vout[0]>600)
		Vout[0]=600;
	if(Vout[0]<-600)
		Vout[0]=-600;
	
	//PID调整速度的分量
	v_err_x=-(Vout[1]*sin(PointRoute[Point_NowNum].speeddir));
	v_err_y= (Vout[1]*cos(PointRoute[Point_NowNum].speeddir));

	
	//加上调整增量
	speed_x += v_err_x;
	speed_y += v_err_y;
	
	//将世界坐标的速度转换为机器人局部坐标的速度
	phi = GPS.radian;
	speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
	speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
	
	//为四轮赋速度
	
	Speed_X = speed_robot_x;
	Speed_Y = speed_robot_y;
	Speed_Rotation = Vout[0];
}
