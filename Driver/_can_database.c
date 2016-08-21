/*
每加一个CAN总线设备
	1、更改Can_Data_Num定义值
	2、要在Can_Database列表中添加总线信息，特别注意不要搞错
	3、void Hash_Table_init(void)中，添加新加设备在Can_Database列表中的序号
	4、添加ID_NUMDEF枚举定义中加入新添ID代表的命令意义
*/

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "../Driver/_command.h"
#include "../Driver/_can_database.h"
#include "../Driver/_can.h"
#include "../Driver/_io_status.h"

uint8_t Can_Data_Num=0;
union fi32_to_u8 MISO_INFRARED;
uint8_t MOSI_INFRA_SWITCH[4] = {0X0,0X0,0X0,0X0};
uint8_t infrared_status;
uint8_t MOSI_SONIC_CMD;
uint8_t MISO_SONIC_DAT[3];
uint8_t MISO_SWITCH[2]={1};
union fi64_to_u8 MOSI_PWM;

//新加ID时，在ID_NUMDEF中定义相应ID的意义
Can_Data Can_Database[]=
{
	//Data_type		    Data_ID				        *Data_ptr				         Data_length		  *MenuFunc		Channel		  Fifo_num
//主控&&SONIC协主控通信ID*/
//	{WRITE_ONLY,	W_SONIC_ID,			(uint8_t *)(&MOSI_SONIC_CMD)		,1,		W_SONIC_FUNC,		1,			CAN_Filter_FIFO0},
//	{READ_ONLY,		S_SONIC_ID,			(uint8_t *)(MISO_SONIC_DAT)			,3,		NULL,				1,			CAN_Filter_FIFO1},
//	{READ_ONLY,		S_SWITCH_ID,		(uint8_t *)(MISO_SWITCH)			,2,		NULL,				1,			CAN_Filter_FIFO1},
//  {WRITE_ONLY,	W_PWM_ID,			(uint8_t *)(MOSI_PWM.u8_data)		,8,		W_PWM_FUNC,			1,			CAN_Filter_FIFO0},
//小车GPS板和pwm板通信
	//        WRITE_ONLY   GPS板写，pwm板读       READ_ONLY  GPS板读，pwm板写 
	{WRITE_ONLY,	W_infrared_ID,					  (uint8_t *)(MISO_INFRARED.u8_data)			    ,8,		NULL,					1,			CAN_Filter_FIFO0} ,
	{READ_ONLY,	  W_inf_switch_ID,				  (uint8_t *)(MOSI_INFRA_SWITCH)				   	  ,4,		INF_DISTAN,		1,			CAN_Filter_FIFO1} ,
};

//HASH_TABLE初始值为255，认为Can_Data_Num值小于256，即设备数少于256个
uint8_t HASH_TABLE[256]={255};

void Hash_Table_init(void)
{
	int i;
	
	Can_Data_Num = sizeof(Can_Database) / sizeof(Can_Database[0]);
	for(i=0;i<256;i++)
	{
		HASH_TABLE[i] = 255;
	}
	for(i=0;i<Can_Data_Num;i++)
	{
		HASH_TABLE[Can_Database[i].Data_ID] = i;
	}
}

void Write_Database(ID_NUMDEF ID_NUM)
{
	uint8_t j;
	CanTxMsg TxMessage;
	/* Check the parameters */
	if((HASH_TABLE[ID_NUM] >= Can_Data_Num)&&(Can_Database[HASH_TABLE[ID_NUM]].Data_type!=WRITE_ONLY))
	{
		return;
	}
	
	TxMessage.StdId=Can_Database[HASH_TABLE[ID_NUM]].Data_ID;
	TxMessage.ExtId=0;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.DLC=Can_Database[HASH_TABLE[ID_NUM]].Data_length;
	
	for(j=0;j<Can_Database[HASH_TABLE[ID_NUM]].Data_length;j++)
	{
		TxMessage.Data[j]=(*(Can_Database[HASH_TABLE[ID_NUM]].Data_ptr+j));
	}
	
	Can_SendData(&TxMessage);
	
}

/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/

