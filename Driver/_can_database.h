#ifndef _CAN_DATABASE_H
#define _CAN_DATABASE_H
	#include "stm32f10x.h"
	
	#ifndef NULL
	#define NULL ((void *)0)
	#endif
	
	#define fp64 double
	#define fp32  float

	/*Data_Type*/
	#define READ_ONLY  0	//主控读，外设写
	#define WRITE_ONLY 1	//主控写，外设读
	
	union fi64_to_u8     //fp64型数据，或int64_t型数据转成u8型数据进行can总线发送
	{
		fp64	 f64_data;
		int64_t  i64_data;
		uint8_t  u8_data[8];
		uint16_t u16_data[4];
	};
	
		union fi32_to_u8     //fp32型数据，或int32_t型数据转成u8型数据进行can总线发送
	{
		fp32	f32_data;
		int32_t i32_data;
		uint8_t u8_data[4];
	};
/*所有命令对应的ID号列表*/
/*
ID位号	[	(GS:GPS Slave  MS:Motor Slave)							]
NO.7	[			0--GS			|		1--Others				]
NO.6	[	0--MOSI		|	1--MISO	|	0--MS MOSI	|	1--Others	]
NO.5~0	[	ID numbers				|	ID numbers  &  Others		]
*/
	typedef enum
	{
//B11-100-00x 主控与超声波协主控通信ID
// 		//MOSI B11-100-000  发送使能和失能某超声波命令
// 		W_SONIC_ID		= 0xE0,
// 		//MISO B11-100-001   向主控返回超声波数据
// 		S_SONIC_ID		= 0xE1,
				//MOSI B11-100-000  发送使能和失能某超声波命令
//		W_SONIC_ID		= 0xE2,
		//MISO B11-100-001   向主控返回超声波数据
		S_SONIC_ID		= 0xE3,
//		S_SWITCH_ID			= 0xc0,
//		W_PWM_ID			= 0xc4,
//		W_BACK_ID = 0xE1,
		W_infrared_ID = 0X18,
		W_inf_switch_ID = 0x19
	}ID_NUMDEF;	
	
	/*主控和GPS协主控之间的命令格式*/
	typedef enum
	{
		NO_COMMAND			= 0x00,
		//M_G_CMD_ID下的命令
		GPS_READ			= 0x01,		//索取GPS信息命令
		CHECK_FLOAT			= 0x02,		//通知GPS协主控校准陀螺仪零漂命令
		GYRO_INIT			= 0x03,		//通知GPS协主控标定陀螺仪命令
		ENC_L_INIT			= 0x04,		//通知GPS协主控标定码盘正反转系数命令
		ENC_R_INIT			= 0x05,		//通知GPS协主控标定码盘旋转半径命令
		//G_M_CMD_ID下的命令
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
		
		//在can总线接到数据后，调用的函数
		void (*MenuFunc)(void);			//入口函数		
		uint8_t  Channel;
		uint8_t  Fifo_num;			//在接收方将该ID配置的fifo号
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
