#include "tftlcd.h"
#include "font.h"

/* LCD�Ļ�����ɫ�ͱ���ɫ */	   
uint16 FRONT_COLOR = WHITE;	//������ɫ
uint16 BACK_COLOR = BLACK;  //����ɫ 

TFTLCD_DATA_t tftlcd_data; 

/********************************************************************************************************
** ����: TFTLCD_GPIO_Init,  TFTLCD_GPIO��ʼ��,�ڲ���������TFTLCD_Init��������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
static void TFTLCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* ʹ��PORTD,E,Gʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE);
	
 	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);			  //PD0��1��4��5��8��9��10��14��15 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);			  //PE7��8��9��10��11��12��13��14��15    	    	 											 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);  		  //PG0��12
}

/********************************************************************************************************
** ����: TFTLCD_FSMC_Init,  FSMC��ʼ��,�ڲ���������TFTLCD_Init��������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
static void TFTLCD_FSMC_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  FSMC_ReadTimingInitStructure; 
	FSMC_NORSRAMTimingInitTypeDef  FSMC_WriteTimingInitStructure;
	/* ʹ��FSMCʱ�� */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	
	
	/* ��ʱ������ */ 
	FSMC_ReadTimingInitStructure.FSMC_AddressSetupTime = 0x01;	 //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns
  	FSMC_ReadTimingInitStructure.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
  	FSMC_ReadTimingInitStructure.FSMC_DataSetupTime = 0x0f;		 //���ݱ���ʱ��Ϊ16��HCLK,��ΪҺ������IC�Ķ����ݵ�ʱ���ٶȲ���̫�죬�����1289���IC��
  	FSMC_ReadTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
    
	/* дʱ������ */ 
	FSMC_WriteTimingInitStructure.FSMC_AddressSetupTime = 0x15;	 //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK  
  	FSMC_WriteTimingInitStructure.FSMC_AddressHoldTime = 0x15;	 //��ַ����ʱ��		
  	FSMC_WriteTimingInitStructure.FSMC_DataSetupTime = 0x05;	 //���ݱ���ʱ��Ϊ6��HCLK	
  	FSMC_WriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 	
 
	/* ���÷����ⲿ�洢����ʱ����� */
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;					 //��������ʹ��NE4 ��Ҳ�Ͷ�ӦBTCR[6],[7]��
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //���������ݵ�ַ
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;			 //NOR/PSRAM����TFTLCD����SRAM��ʹ��   
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;   //�洢�����ݿ��Ϊ16bit   
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;//FSMC_BurstAccessMode_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;			  //�洢��дʹ��
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 			  //��дʹ�ò�ͬ��ʱ��
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadTimingInitStructure; //��дʱ��
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_WriteTimingInitStructure;    //дʱ��
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  										  //��ʼ��FSMC����

	/* ʹ�ܴ洢��1��4������ʼ��ַΪ0X6C000000 */
 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}

/********************************************************************************************************
** ����: LCD_WriteCmd, д�Ĵ�������
**------------------------------------------------------------------------------------------------------
** ����: cmd
** ����: ��											  
********************************************************************************************************/
void LCD_WriteCmd(uint16 cmd)
{
	TFTLCD->LCD_CMD=cmd;//д��Ҫд�ļĴ������
}

/********************************************************************************************************
** ����: LCD_WriteData, д���ݺ���
**------------------------------------------------------------------------------------------------------
** ����: data
** ����: ��											  
********************************************************************************************************/
void LCD_WriteData(uint16 data)
{
	TFTLCD->LCD_DATA=data;//д��Ҫд�ļĴ�����ֵ
}

/********************************************************************************************************
** ����: LCD_WriteCmdData, д������ݺ���
**------------------------------------------------------------------------------------------------------
** ����: cmd, data 
** ����: ��											  
********************************************************************************************************/
void LCD_WriteCmdData(uint16 cmd,uint16 data)
{
	LCD_WriteCmd(cmd);
	LCD_WriteData(data);
}

/********************************************************************************************************
** ����: LCD_WriteData_Color, д��ɫ����
**------------------------------------------------------------------------------------------------------
** ����: color
** ����: ��											  
********************************************************************************************************/
void LCD_WriteData_Color(uint16 color)
{
	TFTLCD->LCD_DATA=color;
}

/********************************************************************************************************
** ����: LCD_ReadData, �����ݺ���
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ����������											  
********************************************************************************************************/
uint16 LCD_ReadData(void)
{
	return TFTLCD->LCD_DATA;
}

/********************************************************************************************************
** ����: LCD_Display_Dir, ����LCD����ʾ����
**------------------------------------------------------------------------------------------------------
** ����: 0����������1��������
** ����: ��										  
********************************************************************************************************/
void LCD_Display_Dir(uint8 dir)
{
	if(dir==0)  //Ĭ����������
	{				
		LCD_WriteCmd(0x0001);   
		LCD_WriteData(0x0100);
		LCD_WriteCmd(0x0003);
		LCD_WriteData(0x1030);
		tftlcd_data.height=400;
		tftlcd_data.width=240;
		tftlcd_data.dir=0;
		
	}
	else
	{		
		LCD_WriteCmd(0x0001);   
		LCD_WriteData(0x0000);
		LCD_WriteCmd(0x0003);
		LCD_WriteData(0x1038);
		tftlcd_data.height=240;
		tftlcd_data.width=400;
		tftlcd_data.dir=1;
	}	
}

/********************************************************************************************************
** ����: LCD_Set_Window, ���ô���,���Զ����û������굽�������Ͻ�(sx,sy),�����С:width*height. 
**------------------------------------------------------------------------------------------------------
** ����: sx,sy:������ʼ����(���Ͻ�) width,height:���ڿ�Ⱥ͸߶�,�������0!!
** ����: ��										  
********************************************************************************************************/
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height)
{    
	if(tftlcd_data.dir==0)
	{
		LCD_WriteCmd(0x0210);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0211);  
	    LCD_WriteData(width);
		LCD_WriteCmd(0x0212);   
	    LCD_WriteData(sy);
		LCD_WriteCmd(0x0213);   
	    LCD_WriteData(height);
	
		LCD_WriteCmd(0x0200);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0201);   
	    LCD_WriteData(sy);	
	}   	
	else
	{
		LCD_WriteCmd(0x0212);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0213);  
	    LCD_WriteData(width);
		LCD_WriteCmd(0x0210);   
	    LCD_WriteData(sy);
		LCD_WriteCmd(0x0211);   
	    LCD_WriteData(height);
	
		LCD_WriteCmd(0x0201);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0200);   
	    LCD_WriteData(sy);	
	}
	LCD_WriteCmd(0x0202);
}

/********************************************************************************************************
** ����: LCD_Clear, ��������
**------------------------------------------------------------------------------------------------------
** ����: color:Ҫ���������ɫ
** ����: ��										  
********************************************************************************************************/
void LCD_Clear(uint16 color)
{
	uint16_t i, j ;

	LCD_Set_Window(0, 0, tftlcd_data.width-1, tftlcd_data.height-1);	 //��������
  	for(i=0; i<tftlcd_data.width; i++)
	{
		for (j=0; j<tftlcd_data.height; j++)
		{
			LCD_WriteData_Color(color);
		}
	} 
}

/********************************************************************************************************
** ����: TFTLCD_Init, TFTLCD��ʼ������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��										  
********************************************************************************************************/
void TFTLCD_Init(void)
{
	TFTLCD_GPIO_Init();
	TFTLCD_FSMC_Init();	
	delay_ms(50); 

	LCD_WriteCmd(0X0000);				   	
	tftlcd_data.id=LCD_ReadData();    
 	printf(" LCD ID:%x\r\n",tftlcd_data.id); //��ӡLCD ID
	/* --R61509V_CPT3.0 --- */

	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	delay_ms(10);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	delay_ms(10);
	
	LCD_WriteCmd(0x0400); LCD_WriteData(0x6200);
	LCD_WriteCmd(0x0008); LCD_WriteData(0x0808);
	
	LCD_WriteCmd(0x0300); LCD_WriteData(0x0005);//gamma
	LCD_WriteCmd(0x0301); LCD_WriteData(0x4C06);
	LCD_WriteCmd(0x0302); LCD_WriteData(0x0602);
	LCD_WriteCmd(0x0303); LCD_WriteData(0x050C);
	LCD_WriteCmd(0x0304); LCD_WriteData(0x3300);
	LCD_WriteCmd(0x0305); LCD_WriteData(0x0C05);
	LCD_WriteCmd(0x0306); LCD_WriteData(0x4206);
	LCD_WriteCmd(0x0307); LCD_WriteData(0x060C);
	LCD_WriteCmd(0x0308); LCD_WriteData(0x0500);
	LCD_WriteCmd(0x0309); LCD_WriteData(0x0033);
	
	LCD_WriteCmd(0x0010); LCD_WriteData(0x0014);//
	LCD_WriteCmd(0x0011); LCD_WriteData(0x0101);//
	LCD_WriteCmd(0x0012); LCD_WriteData(0x0000);//
	LCD_WriteCmd(0x0013); LCD_WriteData(0x0001);//
	
	LCD_WriteCmd(0x0100); LCD_WriteData(0x0330);//BT AP
	LCD_WriteCmd(0x0101); LCD_WriteData(0x0247);//DC0,DC1,VC
	LCD_WriteCmd(0x0103); LCD_WriteData(0x1000);//VDV
	LCD_WriteCmd(0x0280); LCD_WriteData(0xDE00);//VCM
	LCD_WriteCmd(0x0102); LCD_WriteData(0xD1B0);//VR,VCMR,PSON,PON
	delay_ms(10);
	
	LCD_WriteCmd(0x0001); LCD_WriteData(0x0100);
	LCD_WriteCmd(0x0002); LCD_WriteData(0x0100);
	LCD_WriteCmd(0x0003); LCD_WriteData(0x1030);
	LCD_WriteCmd(0x0009); LCD_WriteData(0x0001);
	LCD_WriteCmd(0x000C); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0090); LCD_WriteData(0x8000);
	LCD_WriteCmd(0x000F); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0210); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0211); LCD_WriteData(0x00EF);
	LCD_WriteCmd(0x0212); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0213); LCD_WriteData(0x018F);//432=1AF, 400=18F
	LCD_WriteCmd(0x0500); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0501); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0502); LCD_WriteData(0x005F);
	LCD_WriteCmd(0x0401); LCD_WriteData(0x0001);
	LCD_WriteCmd(0x0404); LCD_WriteData(0x0000);
	delay_ms(10);
							 
	LCD_WriteCmd(0x0007); LCD_WriteData(0x0100);//BASEE
	delay_ms(10);
	
	LCD_WriteCmd(0x0200); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0201); LCD_WriteData(0x0000);
	delay_ms(10);
	LCD_WriteCmd(0x0202);

	LCD_Display_Dir(0);		//0������  1������  Ĭ������
	LCD_Clear(WHITE);
}

//����
//x,y:����
//FRONT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_Set_Window(x, y, x, y);  //���õ��λ��
	LCD_WriteData_Color(FRONT_COLOR);	
}


/********************************************************************************************************
** ����: LCD_DrawFRONT_COLOR, ���ٻ���
**------------------------------------------------------------------------------------------------------
** ����: x,y:���� color:��ɫ
** ����: ��										  
********************************************************************************************************/
void LCD_DrawFRONT_COLOR(uint16 x,uint16 y,uint16 color)
{	   
	LCD_Set_Window(x, y, x, y);
	LCD_WriteData_Color(color);	
} 

//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

/********************************************************************************************************
** ����: LCD_ShowChar, ��ָ��λ����ʾһ���ַ�
**------------------------------------------------------------------------------------------------------
** ����: x,y:��ʼ���� color:��ɫ num:Ҫ��ʾ���ַ�:" "--->"~" size:�����С 12/16/24 
**        mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
** ����: ��										  
********************************************************************************************************/
void LCD_ShowChar(uint16 x,uint16 y,uint8 num,uint8 size,uint8 mode)
{  							  
    uint8 temp,t1,t;
	uint16 y0=y;
	uint8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=ascii_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=ascii_1608[num][t];	//����1608����
		else if(size==24)temp=ascii_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
			else if(mode==0)LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=tftlcd_data.height)return;	//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=tftlcd_data.width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}   

/********************************************************************************************************
** ����: LCD_Pow, m^n����
**------------------------------------------------------------------------------------------------------
** ����: m:���� n:ָ��
** ����: m^n�η�.										  
********************************************************************************************************/
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		

/********************************************************************************************************
** ����: LCD_ShowNum, ��ʾ����
**------------------------------------------------------------------------------------------------------
** ����: x,y:������� len :���ֵ�λ�� size:�����С num:��ֵ(0~4294967295);
** ����: ��										  
********************************************************************************************************/	
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 

/********************************************************************************************************
** ����: LCD_ShowString, ��ʾ�ַ���
**------------------------------------------------------------------------------------------------------
** ����: x,y:������� color:��ɫ width,height:�����С size:�����С *p:�ַ�����ʼ��ַ
** ����: ��										  
********************************************************************************************************/	
void LCD_ShowString(uint16 x,uint16 y,uint16 width,uint16 height,uint8 size,uint8 *p)
{
	uint8 x0 = x;
	width += x;
	height += y;
    while( (*p<='~')&&(*p>=' ') )//�ж��ǲ��ǷǷ��ַ�!
    {       
        if( x>=width )
		{
			x = x0;
			y += size;
		}
        if( y>=height )break;//�˳�
        LCD_ShowChar(x, y, *p, size, 0);
        x += size/2;
        p++;
    }  
}

/********************************************************************************************************
** ����: LCD_ShowFontHZ, ��ʾ����
**------------------------------------------------------------------------------------------------------
** ����: x,y:������� cn:�����ַ���
** ����: ��										  
********************************************************************************************************/	
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNumɨ���ֿ������
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSK��λ��
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
						} 						
						else
						{
							LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
						} 
						color<<=1;
						x++;
						if((x-x0)==32)
						{
							x=x0;
							y++;
							if((y-y0)==29)
							{
								y=y0;
							}
						}
					}//for(j=0;j<8;j++)����
				}	
			}
			
		} //for (wordNum=0; wordNum<20; wordNum++)���� 	
		cn += 2;
		x += 32;
		x0=x;
	}
}	
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

