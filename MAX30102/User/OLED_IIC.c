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

//���²���
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

void OLED_Printf_EN(uint8_t row,uint8_t column,const char *Str,...)//????��?????
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
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,column&0x0f);//?��????��	
			IIC_Write_Byte(OLED_Device_address,OLED_Device_Command,0x10+(column>>4));//?��????��
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
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
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
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//����Yλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			//����Xλ�õ�4λ
}
/**
  * @brief  OLED����
  * @param  ��
  * @retval ��
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
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
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
	IIC_Send_Byte(0x78);		//�ӻ���ַ
	IIC_Send_Byte(0x00);		//д����
	IIC_Send_Byte(Command); 
	IIC_Stop();
}
/**
  * @brief  OLEDд����
  * @param  Data Ҫд�������
  * @retval ��
  */
void OLED_WriteData(uint8_t Data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);		//�ӻ���ַ
	IIC_Send_Byte(0x40);		//д����
	IIC_Send_Byte(Data);
	IIC_Stop();
}

void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WriteCommand(0xb0+i);  //����ҳ��ַ��0~7��
		OLED_WriteCommand(0x00);    //������ʾλ�á��е͵�ַ
		OLED_WriteCommand(0x10);    //������ʾλ�á��иߵ�ַ	
		for(n=0;n<128;n++)
			OLED_WriteData(OLED_GRAM[n][i]); 
	}   
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
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


//����
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2)
{
	u8 i,k,k1,k2;
	for(int j=0;j<2;j++){
		if((x1<0)||(x2>128)||(y1<0)||(y2>64)||(x1>x2)||(y1>y2))  return ;
		if(x1==x2)    //������
		{
				for(i=0;i<(y2-y1);i++)
				{
					OLED_DrawPoint(x1,y1+i,1);
				}
		}
		else if(y1==y2)   //������
		{
				for(i=0;i<(x2-x1);i++)
				{
					OLED_DrawPoint(x1+i,y1,1);
				}
		}
		else      //��б��
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


