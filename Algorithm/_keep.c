#include "_typedef.h"
#include "../Algorithm/_mecanum.h"
#include "../Algorithm/_math.h"
#include "../Algorithm/_pid.h"
#include "_lcdio.h"
#include "_delay.h"
#include "_lcd.h"
#include "_keep.h"
struct Point Aim_Keep_Point;
float Aim_Keep_Radian;
float Speed_Keep_Max;
float Speed_rotation_max=150;

/*
 * ������: SetKeep
 * ��  ��: ���ٵ���ָ��λ����Ƕ�
 * ��  ��: -end_point  Ŀ��λ��
 *		   -aim_radian Ŀ��Ƕ�
 * ��  ��: ��
 * ��  ��: �ⲿ����
 */
void SetKeep(struct Point end_point,float aim_radian,float speed_max)
{
	Aim_Keep_Point = end_point;
	Aim_Keep_Radian = aim_radian;
	Speed_Keep_Max = speed_max;
}

void KeepPoint(void)
{
		float lineangle;
	
		float speed = 0;
		float speed_x,speed_y;						//������������ٶ�����ٶ�
		float speed_robot_x,speed_robot_y;			//ת��������������ķ��ٶ�
		float phi;
	
		float error_angle,error_dis;				//�Ƕ������������
		float Vout_A = 0;							//PID������Ƕ����ֵ
		float Vout_D = 0;							//PID������������ֵ
		lineangle = GetLineAngle(GPS.position,Aim_Keep_Point);	//�����·���߶ε����
		
		//��Ƕ������������
		error_angle = Aim_Keep_Radian-GPS.radian;
		error_dis = GetLength(GPS.position,Aim_Keep_Point);
		
		while(error_angle>pi)
			error_angle-=2*pi;
		while(error_angle<-pi)
			error_angle+=2*pi;
		
		//PID����

		Vout_A = 1000*AnglePID(error_angle,Pid_List[4]);
		Vout_D = DistancePID(error_dis,Pid_List[5]);
		

		//�����ת��
		if(Vout_A > Speed_rotation_max) Vout_A = Speed_rotation_max;
		else if(Vout_A < -Speed_rotation_max) Vout_A = -Speed_rotation_max;
		
		speed = Vout_D;
		SetCursor(0,0);
		LCD_WriteString("Vout_A:");		
		LCD_WriteFloat(Vout_A);
		SetCursor(0,20);
		LCD_WriteString("Vout_D:");		
		LCD_WriteFloat(Vout_D);
		SetCursor(0,40);
		LCD_WriteString("GPS.radian:");		
		LCD_WriteFloat(GPS.radian);
		SetCursor(0,60);
		LCD_WriteString("error_angle:");		
		LCD_WriteFloat(error_angle);			
		//�������
		if(speed > Speed_Keep_Max) speed = Speed_Keep_Max;
		else if(speed < -Speed_Keep_Max) speed = -Speed_Keep_Max;
		
		//���ٶ���x,y�����ϵķ���
		speed_x = speed*cos(lineangle);
		speed_y = speed*sin(lineangle);
		
		//������������ٶ�ת��Ϊ�����˾ֲ�������ٶ�
		phi = GPS.radian;
		speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
		speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
		
		//Ϊ���ָ��ٶ�
	
//		Speed_X = speed_robot_x;
//		Speed_Y = speed_robot_y;
//		Speed_Rotation = Vout_A;
}

void KeepTest(void)
{
	static struct Point end_point={500,0};
	static double aim_angle=0;
  static double speed_max=500;

	LCD_Clear(WHITE);
	PID_Clear();

	SetKeep(end_point,aim_angle*pi/180,speed_max);
	 
	while(1)
	{
		
		KeepPoint();
		SetCursor(0,130);
		LCD_WriteString("now_distance:");
		LCD_WriteFloat(GetLength(GPS.position,end_point));//���ʵʱ����
		if((GetLength(GPS.position,end_point) < 50)&&((GPS.radian-aim_angle*pi/180<0.02)&&(GPS.radian-aim_angle*pi/180>-0.02)))
		{
			SPEED_STOP;
//			SetSpeed(0,0,0);
			break;
		}  
		delay_ms(15);
	}
}