#include "OLED_IIC.h"
#include "oledfont.h"



u8 OLED_GRAM[128][8];
/********************************************
// fill_Picture
********************************************/
void fill_picture(uint8_t fill_Data)
{
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xb0+m);		//rowe0-rowe1
		IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x00);		//low column start address
		IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x10);		//high column start address
		for(n=0;n<128;n++)
		{
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Data,fill_Data);
			OLED_GRAM[n][m]=0x00;
		}
	}
}

//更新波形
void fill_picture1(uint8_t fill_Data)
{
	uint8_t m,n;
	for(m=0;m<6;m++)
	{
		IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xb0+m);		//rowe0-rowe1
		IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x00);		//low column start address
		IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x10);		//high column start address
		for(n=0;n<128;n++)
		{
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Data,fill_Data);
			OLED_GRAM[n][m]=0x00;
		}
	}
}

void OLED_Printf_EN(uint8_t row,uint8_t column,const char *Str,...)//????С?????
{
	uint8_t r,n,ChaNum=0;
	
	char string[100];
	va_list ap;
  va_start(ap,Str);
	vsprintf(string,Str,ap);
	va_end(ap); 
	char *ptr=string;	
	while(*ptr!='\0')
	{		
		for(r=row;r<row+2;r++)//?????????8??
		{
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xb0+r);//??
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,column&0x0f);//?е????λ	
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x10+(column>>4));//?е????λ
			if(r==row)
			{
				for(n=0;n<8;n++)
					IIC_Write_Byte(OLED_Device_address,OLED_Device_Data,ASCII8x16_Table[*ptr-' '][n]);		
			}
			if(r==row+1)
			{
				for(n=8;n<16;n++)
					IIC_Write_Byte(OLED_Device_address,OLED_Device_Data,ASCII8x16_Table[*ptr-' '][n]);	
			}
		}
		ChaNum++;
		column+=8;	
		if(ChaNum%16==0)
			row+=2;
		if(ChaNum==64)
			break;
		ptr++;
	}
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}
/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	

void OLED_WriteCommand(uint8_t Command)
{
	IIC_Start();
	IIC_Send_Byte(0x78);		//从机地址
	IIC_Send_Byte(0x00);		//写命令
	IIC_Send_Byte(Command); 
	IIC_Stop();
}
/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);		//从机地址
	IIC_Send_Byte(0x40);		//写数据
	IIC_Send_Byte(Data);
	IIC_Stop();
}

void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WriteCommand(0xb0+i);  //设置页地址（0~7）
		OLED_WriteCommand(0x00);    //设置显示位置—列低地址
		OLED_WriteCommand(0x10);    //设置显示位置—列高地址	
		for(n=0;n<128;n++)
			OLED_WriteData(OLED_GRAM[n][i]); 
	}   
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

/******************************************
// picture????????????
******************************************/
void OLED_Picture(uint8_t *image)
{
  uint8_t x,y;
  for(y=0;y<8;y++)
    {
      IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xb0+y);
      IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x0);
      IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x10);
      for(x=0;x<128;x++)
        {
          IIC_Write_Byte(OLED_Device_address,OLED_Device_Data,*image++);
        }
    }
}


//画线
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2)
{
	u8 i,k,k1,k2;
	for(int j=0;j<2;j++){
		if((x1<0)||(x2>128)||(y1<0)||(y2>64)||(x1>x2)||(y1>y2))  return ;
		if(x1==x2)    //画竖线
		{
				for(i=0;i<(y2-y1);i++)
				{
					OLED_DrawPoint(x1,y1+i,1);
				}
		}
		else if(y1==y2)   //画横线
		{
				for(i=0;i<(x2-x1);i++)
				{
					OLED_DrawPoint(x1+i,y1,1);
				}
		}
		else      //画斜线
		{
			k1=y2-y1;
			k2=x2-x1;
			k=k1*10/k2;
			for(i=0;i<(x2-x1);i++)
			{
			OLED_DrawPoint(x1+i,y1+i*k/10,1);
			}
			x1=x2;
		}
	}
	
}

void OLED_Init(void)
{
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xAE);   //display off
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x20);	//Set Memory Addressing Mode	
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x00);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,rowe Addressing Mode (RESET);11,Invalid
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xb0);	//Set rowe Start Address for rowe Addressing Mode,0-7
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xc0);	//Set COM Output Scan Direction
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x00);//---set low column address
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x10);//---set high column address
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x40);//--set start line address
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x81);//--set contrast control register
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xdf);
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xa1);//--set segment re-map 0 to 127
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xa6);//--set normal display
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xa8);//--set multiplex ratio(1 to 64)
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x3F);//
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xa4);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xd3);//-set display offset
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x00);//-not offset
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xd5);//--set display clock divide ratio/oscillator frequency
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xf0);//--set divide ratio
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xd9);//--set pre-charge period
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x22); //
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xda);//--set com pins hardware configuration
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x12);
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xdb);//--set vcomh
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x20);//0x20,0.77xVcc
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x8d);//--set DC-DC enable
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x14);//
	IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0xaf);//--turn on oled panel 
}


