#ifndef __LCDIO_H
#define __LCDIO_H
#include "stm32f10x.h"
#define LCD_LEN 100
void LCD_Clear(u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r);										//��Բ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);							//����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   				//������
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//���ָ����ɫ
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//��ʾһ���ַ���,12/16����

void LCD_SetXY(u8 x_index,u8 y_index);
void LCD_WriteChar(char ch);
void LCD_WriteString(char *str);
void LCD_Printf(const char *fmt,...);
void LCD_WriteInt(int val);
void LCD_WriteFloat(float val);
char *Int2Str(int IntData);
char *Float2Str(float FloatData, int PointNum);
void SetCursor(u8 x, u8 y);

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
#endif