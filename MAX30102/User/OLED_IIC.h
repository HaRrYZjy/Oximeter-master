#ifndef _OLED_IIC_H
#define _OLED_IIC_H
#include "stm32f10x.h"                  // Device header
#include "iic.h"
#include "Font.h"
#include <stdarg.h>
#include <stdio.h>
#define OLED_Device_address 0x78
#define OLED_Device_Command 0x00
#define OLED_Device_Data 		0x40

void fill_picture(uint8_t fill_Data);
void OLED_Printf_EN(uint8_t row,uint8_t column,const char *Str,...);
void OLED_Picture(uint8_t *image);
void OLED_Init(void);

void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2);
void  OLED_Refresh_Gram();
void  OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);
void OLED_Clear(void);
void OLED_WriteData(uint8_t Data);
void OLED_WriteCommand(uint8_t Command);
void fill_picture1(uint8_t fill_Data);
	
#endif
