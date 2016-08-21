#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "../Driver/_can_database.h"
#include "../Driver/_can.h"
extern u8 sa_status,st_status;
extern uint8_t infrared_status;
extern union fi32_to_u8 MISO_INFRARED;
extern uint8_t MOSI_INFRA_SWITCH[4];
extern double volt_mv;
extern double predistance;
void W_SONIC_FUNC(void)
{

}

void S_SONIC_FUNC(u16 data,u8 i)
{
	union u8_to_u16
	{
		uint8_t u8_data[2];
		uint16_t u16_data;
	}temp;
	
	temp.u16_data = data;
	
	MISO_SONIC_DAT[0] = temp.u8_data[0];
	MISO_SONIC_DAT[1] = temp.u8_data[1];
	MISO_SONIC_DAT[2] = i;
	
   Write_Database(S_SONIC_ID);
}

void S_SWITCH_FUNC(void)
{
// 	MISO_SWITCH[0] = sa_status;
// 	MISO_SWITCH[1] = st_status;
// 	Write_Database(S_SWITCH_ID);
}

void W_PWM_FUNC(void)
{

}

void INF_DISTAN(void)
{

}

/******************* (C) COPYRIGHT 2016 Heils *****END OF FILE****/
