#include "_serial.h"


Serial *SerialCreate(){
    Serial *self;
    self = (Serial *)malloc(sizeof(Serial));
    self->tickClock = 0;
    self->SdTPC[0] = 0;
    self->clockTick = clockTick;
    self->sendGpsData = sendGpsData;
    self->sendTimeStamp = sendTimeStamp;
    return self;                                                                                                        
}

void SerialDelete(Serial *self){
    free(self);
}

    /************************通过串口3发送大车状态信息******************************/
static void clockTick(Serial *self){
    self->tickClock += 0.1;        //秒计时器
    self->sendGpsData(self);
	
}

void sendGpsData(Serial *self){
    sprintf(self->SdTPC, "(%d,%.2f,%.6f,%.4f,%.4f,1)\n", self->Head, GPS.position.x, GPS.position.y, GPS.radian, self->tickClock);
    Com_Puts(3, self->SdTPC);
    self->SdTPC[0] = 0;
}

void sendTimeStamp(Serial *self){
    sprintf(self->SdTPC, "(%.1f,2)\n", self->tickClock);
    Com_Puts(1, self->SdTPC);
    self->SdTPC[0] = 0;
}

void Com_Puts(u8 uartx, char * str)
{
    USART_TypeDef *_uarts[] = {USART1};
    USART_TypeDef *uart = _uarts[uartx-1];
    u32 count = 0;

    assert_param(uartx>=1 && uartx<=6);

    while (*str)
    {
        count = 0;
        USART_SendData(uart, *str++);
        while (USART_GetFlagStatus(uart, USART_FLAG_TXE)==RESET && count<0xffff)
        {
            count++;
        }
        if(!(count<0xfff))
            assert_param(0);
    }
}
