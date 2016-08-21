#ifndef _CAN_DATABASE_H
#define _CAN_DATABASE_H
	#include "stm32f10x.h"
	
	#ifndef NULL
	#define NULL ((void *)0)
	#endif
	
	#define fp64 double
	#define fp32  float

	/*Data_Type*/
	#define READ_ONLY  0	//���ض�������д
	#define WRITE_ONLY 1	//����д�������
	
	union fi64_to_u8     //fp64�����ݣ���int64_t������ת��u8�����ݽ���can���߷���
	{
		fp64	 f64_data;
		int64_t  i64_data;
		uint8_t  u8_data[8];
		uint16_t u16_data[4];
	};
	
		union fi32_to_u8     //fp32�����ݣ���int32_t������ת��u8�����ݽ���can���߷���
	{
		fp32	f32_data;
		int32_t i32_data;
		uint8_t u8_data[4];
	};
/*���������Ӧ��ID���б�*/
/*
IDλ��	[	(GS:GPS Slave  MS:Motor Slave)							]
NO.7	[			0--GS			|		1--Others				]
NO.6	[	0--MOSI		|	1--MISO	|	0--MS MOSI	|	1--Others	]
NO.5~0	[	ID numbers				|	ID numbers  &  Others		]
*/
	typedef enum
	{
//B11-100-00x �����볬����Э����ͨ��ID
// 		//MOSI B11-100-000  ����ʹ�ܺ�ʧ��ĳ����������
// 		W_SONIC_ID		= 0xE0,
// 		//MISO B11-100-001   �����ط��س���������
// 		S_SONIC_ID		= 0xE1,
				//MOSI B11-100-000  ����ʹ�ܺ�ʧ��ĳ����������
//		W_SONIC_ID		= 0xE2,
		//MISO B11-100-001   �����ط��س���������
		S_SONIC_ID		= 0xE3,
//		S_SWITCH_ID			= 0xc0,
//		W_PWM_ID			= 0xc4,
//		W_BACK_ID = 0xE1,
		W_infrared_ID = 0X18,
		W_inf_switch_ID = 0x19
	}ID_NUMDEF;	
	
	/*���غ�GPSЭ����֮��������ʽ*/
	typedef enum
	{
		NO_COMMAND			= 0x00,
		//M_G_CMD_ID�µ�����
		GPS_READ			= 0x01,		//��ȡGPS��Ϣ����
		CHECK_FLOAT			= 0x02,		//֪ͨGPSЭ����У׼��������Ư����
		GYRO_INIT			= 0x03,		//֪ͨGPSЭ���ر궨����������
		ENC_L_INIT			= 0x04,		//֪ͨGPSЭ���ر궨��������תϵ������
		ENC_R_INIT			= 0x05,		//֪ͨGPSЭ���ر궨������ת�뾶����
		//G_M_CMD_ID�µ�����
		CF_NORMAL			= 0x06,
		CF_CHANGED			= 0x07,
		CF_ERROR			= 0x08
	}COM_TYPE;
	
	
	typedef struct
	{
		uint8_t  Data_type;
		ID_NUMDEF  Data_ID;
		uint8_t* Data_ptr;
		uint8_t  Data_length;
		
		//��can���߽ӵ����ݺ󣬵��õĺ���
		void (*MenuFunc)(void);			//��ں���		
		uint8_t  Channel;
		uint8_t  Fifo_num;			//�ڽ��շ�����ID���õ�fifo��
	}Can_Data;
	
	extern uint8_t Can_Data_Num;
	extern Can_Data Can_Database[];
	extern uint8_t HASH_TABLE[256];
	
	extern uint8_t MOSI_SONIC_CMD;
	extern uint8_t MISO_SONIC_DAT[3];
	extern uint8_t MISO_SWITCH[2];
	extern union fi64_to_u8 MOSI_PWM;
  extern union fi32_to_u8 MISO_INFRARED;
	void Hash_Table_init(void);
	void Write_Database(ID_NUMDEF ID_NUM);

#endif
