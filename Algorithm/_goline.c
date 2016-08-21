#include "_typedef.h"
#include "../Algorithm/_mecanum.h"
#include "../Algorithm/_math.h"
#include "../Algorithm/_pid.h"
struct Point Sta_Point;
struct Point End_Point;
float Aim_Radian;
float Speed_Sta;
float Speed_Max;
float Line_Angle;
float S;
float Acc_Dis,Dec_Dis;
float Acc,Dec;
float Acc_Rate=0.3;
float Dec_Rate=0.3;

/*
 * 函数名: SetLine
 * 描  述: 初始化走直线功能
 * 输  入: -end_point  目标位置
 *		   -aim_radian 目标角度
 *		   -speed_sta  初始速度
 *		   -speed_max  最大速度
 *		   -speed_end  结束速度
 * 输  出: 无
 * 调  用: 外部调用
 */
void SetLine(struct Point end_point,float aim_radian,float speed_sta,float speed_max,float speed_end)
{
	Sta_Point = GPS.position;		//记录初始位置
	End_Point = end_point;
	Aim_Radian = aim_radian;
	
	Speed_Sta = speed_sta;
	Speed_Max = speed_max;
	
	Line_Angle = GetLineAngle(Sta_Point,end_point);	//求出该路径线段的倾角
	
	S = GetLength(Sta_Point,end_point);				//求出该路径线段的长度
	
	Acc_Dis = Acc_Rate*S;//加速距离和减速距离
	Dec_Dis = Dec_Rate*S;
	
	Acc = (speed_max - speed_sta)/Acc_Dis;//加速度与减速度（与距离相关）
	Dec = (speed_end - speed_max)/Dec_Dis;
	
	PID_Clear();
}

void GoLine(void)
{
	struct Point pos;							//理论坐标

	float k;
	float speed_x;						//世界坐标里的速度与分速度
	float speed=0;
	float speed_y;
	float speed_robot_x;
	float phi;
	float speed_robot_y;			//转换到机器人坐标的分速度
	
	float error_angle,error_dis;				//角度误差与距离误差
	float Vout_A=0;							//PID调整后角度输出值
	float Vout_D=0;							//PID调整后距离输出值
	float v_err_x,v_err_y;						//PID调整速度的分量
	
	pos = GetFoot_P2L_PP(GPS.position,Sta_Point,End_Point);	//得到理论坐标
	
	//根据理论坐标赋速度
	if(GetLength(Sta_Point,pos) < Acc_Dis)
	{
		if(speed < Speed_Max)
			speed = Speed_Sta+  Acc*GetLength(Sta_Point,pos); //speed_sta
		else
			speed = Speed_Max;
	}
	else 
		if(GetLength(Sta_Point,pos) > (S-Dec_Dis))
	{
		speed = Speed_Max + Dec*(GetLength(Sta_Point,pos)-(S-Dec_Dis));
	}
		else
		speed = Speed_Max;
	
	//求速度在x,y方向上的分量
	speed_x = speed*cos(Line_Angle);
	speed_y = speed*sin(Line_Angle);
	
	//角度误差和距离误差
	error_angle = Aim_Radian-GPS.radian;
	error_dis = ( GPS.position.x - Sta_Point.x ) * sin( Line_Angle ) -( GPS.position.y - Sta_Point.y ) * cos( Line_Angle );
		//error_dis为垂直路径方向上偏离路径的距离，无误！
	if(error_angle>pi)
		error_angle-=2*pi;
	if(error_angle<-pi)
		error_angle+=2*pi;
	
	//PID调整
	//if(GetLength(GPS.position,Sta_Point) >= Acc_Dis)
	Vout_A = 1000*AnglePID(error_angle,Pid_List[2]);
	Vout_D = DistancePID(error_dis,Pid_List[3]);
	
	//PID调整速度的分量
	v_err_x = -Vout_D*sin(Line_Angle);
	v_err_y =  Vout_D*cos(Line_Angle);
	
	//加上调整增量
	speed_x += v_err_x;
	speed_y += v_err_y;
	
	k=sqrt(speed_x*speed_x+speed_y*speed_y);
	
		if(speed_x>Speed_Max||speed_y>Speed_Max)
		{speed_x=Speed_Max*speed_x/k;
	
		speed_y=Speed_Max*speed_y/k;}
	
	
	if(speed_x<-Speed_Max||speed_y<-Speed_Max)
	{speed_x=-Speed_Max*speed_x/k;

		speed_y=-Speed_Max*speed_y/k;}
		
		phi = GPS.radian;
	
		speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
		speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
		//将世界坐标的速度转换为机器人局部坐标的速度
	
	
	//为四轮赋速度
	Speed_X = speed_robot_x;
	Speed_Y = speed_robot_y;
	Speed_Rotation = Vout_A;
		if(Speed_Rotation>=100)
			Speed_Rotation=100;
		if(Speed_Rotation<=-100)
			Speed_Rotation=-100;
}
