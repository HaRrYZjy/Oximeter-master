//血液信息转换
void blood_data_translate(void)
{	
	float n_denom;
	uint16_t i;
	// 4 pt Moving Average
//	printf("****************red******************************************************************\r\n");
//	for(i = 0;i < FFT_N;i++) 
//	{
//		printf("%f\r\n",s1[i].real);	
//	}
//	printf("***************** ir*****************************************************************\r\n");
//	for(i = 0;i < FFT_N;i++) 
//	{
//		printf("%f\r\n",s2[i].real);	
//	}
	//直流滤波
	float dc_red =0; 
	float dc_ir =0;
	float ac_red =0; 
	float ac_ir =0;
	
	for (i=0 ; i<FFT_N ; i++ ) 
	{
		dc_red += s1[i].real ;
		dc_ir +=  s2[i].real ;
	}
		dc_red =dc_red/FFT_N ;
		dc_ir =dc_ir/FFT_N ;
	for (i=0 ; i<FFT_N ; i++ )  
	{
		s1[i].real =  s1[i].real - dc_red ; 
		s2[i].real =  s2[i].real - dc_ir ; 
	}
	
	//移动平均滤波
	printf("***********8 pt Moving Average red******************************************************\r\n");
	for(i = 1;i < FFT_N-1;i++) 
	{
			n_denom= ( s1[i-1].real + 2*s1[i].real + s1[i+1].real);
			s1[i].real=  n_denom/4.00; 
			
			n_denom= ( s2[i-1].real + 2*s2[i].real + s2[i+1].real);
			s2[i].real=  n_denom/4.00; 			
	}
	//八点平均滤波
	for(i = 0;i < FFT_N-8;i++) 
	{
			n_denom= ( s1[i].real+s1[i+1].real+ s1[i+2].real+ s1[i+3].real+ s1[i+4].real+ s1[i+5].real+ s1[i+6].real+ s1[i+7].real);
			s1[i].real=  n_denom/8.00; 
			
			n_denom= ( s2[i].real+s2[i+1].real+ s2[i+2].real+ s2[i+3].real+ s2[i+4].real+ s2[i+5].real+ s2[i+6].real+ s2[i+7].real);
			s2[i].real=  n_denom/8.00; 
		
			printf("%f\r\n",s1[i].real);		
	}
	printf("************8 pt Moving Average ir*************************************************************\r\n");
	for(i = 0;i < FFT_N;i++) 
	{
		printf("%f\r\n",s2[i].real);	
	}
	printf("**************************************************************************************************\r\n");
	//开始变换显示	
	g_fft_index = 0;	
	//快速傅里叶变换
	FFT(s1);
	FFT(s2);
	
	//解平方
	printf("开始FFT算法****************************************************************************************************\r\n");
	for(i = 0;i < FFT_N;i++) 
	{
		s1[i].real=sqrtf(s1[i].real*s1[i].real+s1[i].imag*s1[i].imag);
		s1[i].real=sqrtf(s2[i].real*s2[i].real+s2[i].imag*s2[i].imag);
	}
	//计算交流分量
	for (i=1 ; i<FFT_N ; i++ ) 
	{
		ac_red += s1[i].real ;
		ac_ir +=  s2[i].real ;
	}
	
	for(i = 0;i < FFT_N/2;i++) 
	{
		printf("%f\r\n",s1[i].real);
		
	}

	printf("****************************************************************************************\r\n");
	for(i = 0;i < FFT_N/2;i++) 
	{
		printf("%f\r\n",s2[i].real);

		
	}

	printf("结束FFT算法***************************************************************************************************************\r\n");
//	for(i = 0;i < 50;i++) 
//	{
//		if(s1[i].real<=10)
//			break;
//	}
	
	//printf("%d\r\n",(int)i);
	//读取峰值点的横坐标  结果的物理意义为 
	int s1_max_index = find_max_num_index(s1, 30);
	int s2_max_index = find_max_num_index(s2, 30);
	printf("%d\r\n",s1_max_index);
	printf("%d\r\n",s2_max_index);
	//检查HbO2和Hb的变化频率是否一致
//	if(i>=45)
//	{
		//心率计算
//		uint16_t Heart_Rate = 60.00 * SAMPLES_PER_SECOND * 
//													s1_max_index / FFT_N;
		
		float Heart_Rate = 60.00 * ((100.0 * s1_max_index )/ 512.00);
		
		g_blooddata.heart = Heart_Rate;
		
		//血氧含量计算
		//double R = (s2[average].real * s1[0].real)/(s1[s1_max_index].real * s2[0].real);		
		//sp02_num = (1 - sp02_num) * SAMPLES_PER_SECOND + CORRECTED_VALUE;
		
		//double R = (s1[s1_max_index].real/s1[0].real)/(s2[s2_max_index].real/s2[0].real);
		
		//double sp02_num =-16.666666*R*R+ 8.333333 *R + 100;
		//g_blooddata.SpO2 = sp02_num;
		//血氧含量计算
	//float sp02_num = (s2[s1_max_index].real * s1[0].real)/(s1[s1_max_index].real * s2[0].real);
			
			//sp02_num = (1 - sp02_num) * SAMPLES_PER_SECOND + CORRECTED_VALUE;
			//( n_y_ac *n_x_dc_max) / ( n_x_ac *n_y_dc_max)
			
			float R = (ac_ir*dc_red)/(ac_red*dc_ir);
			float sp02_num =-45.060*R*R+ 30.354 *R + 94.845;
			g_blooddata.SpO2 = sp02_num;
			
		//状态正常
//	}
//	else //数据发生异常
//	{
//		g_blooddata.heart = 0;
//		g_blooddata.SpO2 	= 0;
//	}
	//结束变换显示
}


//更新oled显示
void dis_DrawCurve(u32* data1,u8 x1,u32* data2,u8 x2)
{
	u16 i;
	u32 max1=0,max2=0,min1=262144,min2=262144;
	u32 temp1,temp2;
	u32 compress1,compress2;
	
	for(i=0;i<128*2;i++)
	{
		if(data1[i]>max1)
		{
			max1 = data1[i];
		}
		if(data1[i]<min1)
		{
			min1 = data1[i];
		}
	}
		for(i=0;i<128*2;i++)
	{
		if(data2[i]>max2)
		{
			max2 = data2[i];
		}
		if(data2[i]<min2)
		{
			min2 = data2[i];
		}
	}
	
	compress1 = (max1-min1)/20;
	compress2 = (max2-min2)/20;
	for(i=0;i<128;i++)
	{
		temp1 = data1[i*2] + data1[i*2+1];
		temp2 = data2[i*2] + data2[i*2+1];
		temp1/=2;
		temp2/=2;
		temp1 -= min1;
		temp2 -= min2;
		temp1/=compress1;
		temp2/=compress2;
		if(temp1>20)temp1=20;
		if(temp2>20)temp2=20;
		OLED_DrawPoint(i,63-x1-temp1,1);
		OLED_DrawPoint(i,63-x2-temp2,1);
		if(i%8==0){
			OLED_Refresh_Gram();
		}
			
		
	}
}