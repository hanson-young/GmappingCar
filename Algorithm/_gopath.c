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
extern u8 RouteFinish;//�������㼴��ʹ·��ֹͣ
/*
 * ������: SetPointPath
 * ��  ��: ��ʼ���ܵ���·��
 * ��  ��: -sta_num ·����ʼ�ĵ�
 *		   -end_num ·�������ĵ�
 * ��  ��: ��
 * ��  ��: �ⲿ����
 */
void SetPointPath(int sta_num,int end_num)
{
	Point_StartNum = sta_num;
	Point_EndNum = end_num;
	Point_NowNum = sta_num;
	PID_Clear();
	RouteFinish=1;//1��ʾ·��δ������0��ʾ·������
}

void GoPointPath(struct PID Pid_ang,struct PID Pid_dis)
{
	int count_scope=9;				//������Χ
	int count_delta=0;
	int i;
	
	float phi;									//
	float speed,speed_x,speed_y;				//������������ٶ�����ٶ�
	float speed_robot_x,speed_robot_y;			//ת��������������ķ��ٶ�
	
	float error_angle,error_dis;				//�Ƕ������������
	float v_err_x,v_err_y;						//PID�����ٶȵķ���
	
#if Acc_Limit_Enable_new
	static struct GPS_State last_GPS;
	float delta_s;
	float delta_a;
	float robot_speed;
	float w;
	float max_acc = 200;
	float max_acc_w = 50;
#endif	

	//������һ��Ŀ���
	for(i=0;i<count_scope;i++)
	{
		if((Point_NowNum+i) >= Point_EndNum) break;
		if(GetLength(PointRoute[Point_NowNum+i+1].position,GPS.position)
			<= GetLength(PointRoute[Point_NowNum+i].position,GPS.position))
			count_delta = i+1;
	}
	Point_NowNum += count_delta;
	
	//���ٶ���x,y�����ϵķ���
	speed = PointRoute[Point_NowNum].speed*S_Factor;//*Speed_Factor

	/****************************�Լ��ٶȽ�������(��ʵ���ٶ����Ƹ��ٶ�)******************************/
	
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
	
	//�Ƕ����;������
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
	
	//PID�����ٶȵķ���
	v_err_x=-(Vout[1]*sin(PointRoute[Point_NowNum].speeddir));
	v_err_y= (Vout[1]*cos(PointRoute[Point_NowNum].speeddir));

	
	//���ϵ�������
	speed_x += v_err_x;
	speed_y += v_err_y;
	
	//������������ٶ�ת��Ϊ�����˾ֲ�������ٶ�
	phi = GPS.radian;
	speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
	speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
	
	//Ϊ���ָ��ٶ�
	
	Speed_X = speed_robot_x;
	Speed_Y = speed_robot_y;
	Speed_Rotation = Vout[0];
}
