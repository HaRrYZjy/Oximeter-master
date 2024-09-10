#include "stm32f10x.h"
#include "systick.h"
#include "usart.h"
#include "iic.h"
#include "Font.h"
#include "OLED_IIC.h"
#include "MAX30102.h"
#include "algorithm.h"
#include "blood.h"
int main(void)
{	
	int i=0;
	USART_CONFIG();
	printf("这是串口调试助手\r\n");
	
	IIC_GPIO_INIT();
	
  OLED_Init();
	
	fill_picture(0x00);
	OLED_ShowString(0,64,"good",16);
	OLED_Refresh_Gram();
	//Ds18b20_Init();
	//DS18B20_Read_Temperature();
	
	MAX30102_GPIO();
	
	Max30102_reset();
	
	MAX30102_Config();
	Delay_ms(1000);	
	
	
	for(i = 0;i < 128;i++) 
	{
		while(MAX30102_INTPin_Read()==0)
		{
			//读取FIFO
			max30102_read_fifo();
		}
	}
	fill_picture(0x00);
	while(1)
	{	
		//deal_Temp();
		blood_Loop();
		//fill_picture(0x00);
	}
}
