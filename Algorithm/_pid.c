
/* Includes ------------------------------------------------------------------*/
#include "math.h"
#include "_math.h"
#include "_typedef.h"

struct PID Pid_List[]=
{
	{2.0,1000000,0},	//0  Keep�Ƕ�PID
	{6,1000000,0},		//1  Keep����PID
	
	{4.0,1000000,0},	//2  Keep�Ƕ�PID
	{6,1000000,0},		//3  Keep����PID

	{0.60,1000000,0},	//4  Keep�Ƕ�PID
	{2.5,1000000,2.5},		//5  Keep����PID
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
 * ������: PID_Clear
 * ��  ��: ��PID�洢�����ֵ���
 * ��  ��: ��
 * ��  ��: ��
 * ��  ��: �ⲿ����
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
 * ������: PID
 * ��  ��: λ��ʽPID����������������ʽ�ĵ���
 * ��  ��: -Error �����
 *		   -pid pid����
 * ��  ��: �������
 * ��  ��: �ⲿ����
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
 * ������: AnglePID
 * ��  ��: λ��ʽ�Ƕ�PID����������������ʽ�ĵ���
 * ��  ��: -Error �����
 *		   -pid pid����
 * ��  ��: ���ٶ�
 * ��  ��: �ⲿ����
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
 * ������: DistancePID
 * ��  ��: λ��ʽ����PID����������������ʽ�ĵ���
 * ��  ��: -Error �����
 *		   -pid pid����
 * ��  ��: �ٶ�
 * ��  ��: �ⲿ����
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
