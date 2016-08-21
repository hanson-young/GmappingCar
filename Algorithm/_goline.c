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
 * ������: SetLine
 * ��  ��: ��ʼ����ֱ�߹���
 * ��  ��: -end_point  Ŀ��λ��
 *		   -aim_radian Ŀ��Ƕ�
 *		   -speed_sta  ��ʼ�ٶ�
 *		   -speed_max  ����ٶ�
 *		   -speed_end  �����ٶ�
 * ��  ��: ��
 * ��  ��: �ⲿ����
 */
void SetLine(struct Point end_point,float aim_radian,float speed_sta,float speed_max,float speed_end)
{
	Sta_Point = GPS.position;		//��¼��ʼλ��
	End_Point = end_point;
	Aim_Radian = aim_radian;
	
	Speed_Sta = speed_sta;
	Speed_Max = speed_max;
	
	Line_Angle = GetLineAngle(Sta_Point,end_point);	//�����·���߶ε����
	
	S = GetLength(Sta_Point,end_point);				//�����·���߶εĳ���
	
	Acc_Dis = Acc_Rate*S;//���پ���ͼ��پ���
	Dec_Dis = Dec_Rate*S;
	
	Acc = (speed_max - speed_sta)/Acc_Dis;//���ٶ�����ٶȣ��������أ�
	Dec = (speed_end - speed_max)/Dec_Dis;
	
	PID_Clear();
}

void GoLine(void)
{
	struct Point pos;							//��������

	float k;
	float speed_x;						//������������ٶ�����ٶ�
	float speed=0;
	float speed_y;
	float speed_robot_x;
	float phi;
	float speed_robot_y;			//ת��������������ķ��ٶ�
	
	float error_angle,error_dis;				//�Ƕ������������
	float Vout_A=0;							//PID������Ƕ����ֵ
	float Vout_D=0;							//PID������������ֵ
	float v_err_x,v_err_y;						//PID�����ٶȵķ���
	
	pos = GetFoot_P2L_PP(GPS.position,Sta_Point,End_Point);	//�õ���������
	
	//�����������긳�ٶ�
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
	
	//���ٶ���x,y�����ϵķ���
	speed_x = speed*cos(Line_Angle);
	speed_y = speed*sin(Line_Angle);
	
	//�Ƕ����;������
	error_angle = Aim_Radian-GPS.radian;
	error_dis = ( GPS.position.x - Sta_Point.x ) * sin( Line_Angle ) -( GPS.position.y - Sta_Point.y ) * cos( Line_Angle );
		//error_disΪ��ֱ·��������ƫ��·���ľ��룬����
	if(error_angle>pi)
		error_angle-=2*pi;
	if(error_angle<-pi)
		error_angle+=2*pi;
	
	//PID����
	//if(GetLength(GPS.position,Sta_Point) >= Acc_Dis)
	Vout_A = 1000*AnglePID(error_angle,Pid_List[2]);
	Vout_D = DistancePID(error_dis,Pid_List[3]);
	
	//PID�����ٶȵķ���
	v_err_x = -Vout_D*sin(Line_Angle);
	v_err_y =  Vout_D*cos(Line_Angle);
	
	//���ϵ�������
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
		//������������ٶ�ת��Ϊ�����˾ֲ�������ٶ�
	
	
	//Ϊ���ָ��ٶ�
	Speed_X = speed_robot_x;
	Speed_Y = speed_robot_y;
	Speed_Rotation = Vout_A;
		if(Speed_Rotation>=100)
			Speed_Rotation=100;
		if(Speed_Rotation<=-100)
			Speed_Rotation=-100;
}
