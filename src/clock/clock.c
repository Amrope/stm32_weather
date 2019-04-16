#include "clock.h"

/********************************************************************************************************
** ����: ��Բ
**------------------------------------------------------------------------------------------------------
** ����: x,y:����  color:��ɫ
** ����: ��											  
********************************************************************************************************/
static void GetCircle(int x,int y,int r,int color)
{
	int xc=0;
	int yc,p;
	yc=r;
	p=3-(r<<1);	
	while(xc <= yc)
	{
		LCD_DrawFRONT_COLOR(x+xc,y+yc,color);
		LCD_DrawFRONT_COLOR(x+xc,y-yc,color);	
		LCD_DrawFRONT_COLOR(x-xc,y+yc,color);
		LCD_DrawFRONT_COLOR(x-xc,y-yc,color);
		LCD_DrawFRONT_COLOR(x+yc,y+xc,color);	
		LCD_DrawFRONT_COLOR(x+yc,y-xc,color);
		LCD_DrawFRONT_COLOR(x-yc,y+xc,color);
		LCD_DrawFRONT_COLOR(x-yc,y-xc,color);
		if(p<0)
		{
			p += (xc++ << 2) + 6;	
		}
		else
			p += ((xc++ - yc--)<<2) + 10;
	}
}

/********************************************************************************************************
** ����: DrawCircle, ��ָ��ʱ�ӵı߽�Բ������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
static void DrawCircle(void)	 
{
	GetCircle(120,250,100,YELLOW);
	GetCircle(120,250,99,YELLOW);
	GetCircle(120,250,98,YELLOW);
	GetCircle(120,250,97,YELLOW);
	GetCircle(120,250,5,YELLOW);			
}

/********************************************************************************************************
** ����: DrawGrid, ��ָ��ʱ�ӵĸ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
static void DrawGrid(void) 
{
	u8 i;
	u8 rome[][3]={"12","1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11" } ; //�������� 
	int x1,y1,x2,y2,x3,y3;
	for(i=0;i<60;i++)
	{
		x1 = (int)(120 + (sin(i * PI / 30) * 92)); 
		y1 = (int)(120 - (cos(i * PI / 30) * 92)); 
		x2 = (int)(120 + (sin(i * PI / 30) * 97)); 
		y2 = (int)(120 - (cos(i * PI / 30) * 97)); 
		FRONT_COLOR=RED;
		LCD_DrawLine(x1,y1+130,x2,y2+130);

		if(i%5==0)
		{
			x1 = (int)(120 + (sin(i * PI / 30) * 85)); 
			y1 = (int)(120 - (cos(i * PI / 30) * 85)); 
			x2 = (int)(120 + (sin(i * PI / 30) * 97)); 
			y2 = (int)(120 - (cos(i * PI / 30) * 97)); 
			LCD_DrawLine(x1,y1+130,x2,y2+130);	

			x3 = (int)(92 + (sin((i ) * PI / 30) * 80)); 
			y3 = (int)(92 - (cos((i ) * PI / 30) * 80));
			FRONT_COLOR=YELLOW;
			LCD_ShowString(x3+20,y3+150,tftlcd_data.width,tftlcd_data.height,16,rome[i/5]);
		}	
	}		
}

/********************************************************************************************************
** ����: DrawGrid, ��ָ��ʱ�ӵ�ʱ�롢���롢����
**------------------------------------------------------------------------------------------------------
** ����: hhourʱ��mmin�֣���ssec
** ����: ��											  
********************************************************************************************************/
static void DrawPointer(int hhour,int mmin,int ssec)  
{
	int xhour, yhour, xminute, yminute, xsecond, ysecond; //��������ϵָ������
	xhour = (int)(60 * sin( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	yhour = (int)(60 * cos( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	xminute = (int)(90 * sin( mmin * PI / 30 + ssec * PI / 1800)); 
	yminute = (int)(90 * cos( mmin * PI / 30 + ssec * PI / 1800)); 
	xsecond = (int)(100 * sin( ssec * PI / 30)); 
	ysecond = (int)(100 * cos( ssec * PI / 30)); 

	FRONT_COLOR=RED;
	LCD_DrawLine(120 + xhour, 250 - yhour, 120 -xhour / 6, 250 + yhour / 6);
	FRONT_COLOR=BLUE;
	LCD_DrawLine(120 + xminute, 250 - yminute, 120 -xminute / 4, 250 + yminute / 4);
	FRONT_COLOR=GREEN;
	LCD_DrawLine(120 + xsecond, 250 - ysecond, 120 -xsecond / 3, 250 + ysecond / 3);
	
}

/********************************************************************************************************
** ����: PointerWipe, ����ָ�루��ָ�����ɫ���ɫ��ͬ���ﵽ����ָ���Ч����
**------------------------------------------------------------------------------------------------------
** ����: hhourʱ��mmin�֣���ssec
** ����: ��											  
********************************************************************************************************/
static void PointerWipe(int hhour,int mmin,int ssec)  
{
	int xhour, yhour, xminute, yminute, xsecond, ysecond; //��������ϵָ������
	xhour = (int)(60 * sin( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	yhour = (int)(60 * cos( hhour * PI / 6 + mmin * PI / 360 + ssec * PI / 1800)); 
	xminute = (int)(90 * sin( mmin * PI / 30 + ssec * PI / 1800)); 
	yminute = (int)(90 * cos( mmin * PI / 30 + ssec * PI / 1800)); 
	xsecond = (int)(100 * sin( ssec * PI / 30)); 
	ysecond = (int)(100 * cos( ssec * PI / 30)); 

	FRONT_COLOR=BLACK;
	LCD_DrawLine(120 + xhour, 250 - yhour, 120 -xhour / 6, 250 + yhour / 6);
	LCD_DrawLine(120 + xminute, 250 - yminute, 120 -xminute / 4, 250 + yminute / 4);
	LCD_DrawLine(120 + xsecond, 250 - ysecond, 120 -xsecond / 3, 250 + ysecond / 3);
}

/********************************************************************************************************
** ����: ShowClock, ��ʾʱ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
void ShowClock(void)
{
	char date_buf[20],time_buf[10];
	
	DrawCircle(); 	//����
	DrawGrid();		//��ָ��
	if( g_time_flag )
	{
		PointerWipe(calendar_t.hour,calendar_t.min,calendar_t.sec);
		g_time_flag = 0;
		RTC_Get();
		//------------------------------------------------------------------------------------------
		//������ʾ
		printf("RTC Time:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\t%s\n",
		calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,
		calendar_t.hour,calendar_t.min,calendar_t.sec,wday[calendar_t.week]);
		//------------------------------------------------------------------------------------------
		//Һ����ʾ	
		sprintf(date_buf,"%.4d/%.2d/%.2d   %s",calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,wday[calendar_t.week]);
		sprintf(time_buf,"%.2d:%.2d:%.2d",calendar_t.hour,calendar_t.min,calendar_t.sec);
		DrawPointer(calendar_t.hour, calendar_t.min, calendar_t.sec);	//��ָ��
	}
	DrawCircle();
	DrawGrid();
	LCD_ShowString(24,80,tftlcd_data.width,tftlcd_data.height,24,(uint8 *)date_buf);
	LCD_ShowString(77,110,tftlcd_data.width,tftlcd_data.height,24,(uint8 *)time_buf);
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/