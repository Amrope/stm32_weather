#include "rtc.h"


//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31

Calendar_T calendar_t;	// �����ṹ��

/* �·�x�������ݱ� */											 
uint8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 
/* ƽ����·����ڱ� */
const uint8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
/* �������� */
char *wday[] = {"������", "����һ", "���ڶ�", "������", "������", "������", "������"};

/********************************************************************************************************
** ����: RTC_NVIC_Config,  RTC�ж����ã���RTC_Init��������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
static void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;				//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);							
}

/********************************************************************************************************
** ����: RTC_Init,  RTC��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: 0���ɹ�����������ʧ�ܣ�											  
********************************************************************************************************/
uint8 RTC_Init(void)
{
	//����ǲ��ǵ�һ������ʱ��
	uint8 temp = 0;
	
	/* ʹ��PWR��BKP����ʱ��  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	
	
	/* ʹ��RTC�ͺ󱸼Ĵ�������(ȡ����������д����) */
	PWR_BackupAccessCmd(ENABLE);	
	
	/* ��ָ���ĺ󱸼Ĵ����ж�������:������д���ָ�������Ƿ���� */
	if (BKP_ReadBackupRegister(BKP_DR1) != 0XA0A0)		
	{	 			
		BKP_DeInit();				// ��λ�������� 	
		RCC_LSEConfig(RCC_LSE_ON);	// �����ⲿ���پ���(LSE),ʹ��������پ���
		/* ���ָ����Rtc_Flag�������,�ȴ����پ������ */
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=250)return 1;					 // ��ʼ��ʱ��ʧ��,����������	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	 // ����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);					 // ʹ��RTCʱ��  
		RTC_WaitForLastTask();					 // �ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();					 // �ȴ�RTC�Ĵ�����RTC��APBʱ��ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		 // ʹ��RTC���ж�
		RTC_WaitForLastTask();					 // �ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();  				 // ��������	
		RTC_SetPrescaler(32767);				 // ����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();					 // �ȴ����һ�ζ�RTC�Ĵ�����д�������
//		RTC_Set(2017,10,20,21,51,40);    		 // ����ʱ��
		RTC_Set(calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,calendar_t.hour,calendar_t.min,calendar_t.sec);    		 // ����ʱ��
		RTC_ExitConfigMode();   				 // �˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0XA0A0);// ��ָ���ĺ󱸼Ĵ�����д���û���������
	}
	/* ϵͳ������ʱ */
	else
	{
		RTC_WaitForSynchro();					 // �ȴ�RTC�Ĵ�����RTC��APBʱ��ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		 // ʹ��RTC���ж�
		RTC_WaitForLastTask();				     // �ȴ����һ�ζ�RTC�Ĵ�����д�������
	}
	RTC_NVIC_Config();							 // RTC�жϷ�������		    				     
	RTC_Get();									 // ����ʱ��	
	PrintfToHMI("RTC Init" ,"page_Init", "InitRTCTxt", "txt");
	return 0; 									 // ��ʼ���ɹ�
}

/********************************************************************************************************
** ����: IsLeapYear,  �ж����ꡢƽ��
**------------------------------------------------------------------------------------------------------
** ����: year
** ����: 0��ƽ�꣩��1�����꣩		
** ע��: �����ж�����:1.�ܱ�4���������ܱ�100������2.�ܱ�4���������ܱ�400����
********************************************************************************************************/
uint8 IsLeapYear(uint16 year)
{
	if( year%4==0 )
	{
		if( year%100==0 )
		{
			if( year%400==0 )
			{
				return 1;//����
			}
			else
			{
				return 0;//ƽ��
			}
		}
		else
		{
			return 1;	 //����
		}
	}
	else
	{
		return 0;        //ƽ��
	}
}

/********************************************************************************************************
** ����: RTC_Get,  �õ���ǰ��ʱ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: 0���ɹ���������������											  
********************************************************************************************************/
uint8 RTC_Get(void)
{
	static uint16 daycnt = 0;
	uint32 rtc_count = 0;    		// ���ڽ���RTC������ֵ
	uint32 day_num = 0;		 		// 1970��1��1��������˶�����
	uint16 year = 0;				// ����
	
    rtc_count = RTC_GetCounter();	// ��ȡRTC��������ֵ
 	day_num = rtc_count/86400;      // �õ�����(��������Ӧ��)
	if( daycnt!=day_num )			// ����һ����
	{	  
		daycnt = day_num;
		year = 1970;				// ��1970�꿪ʼ
		while( day_num >= 365 )		// ����������1��ͼ����
		{				 
			if( IsLeapYear(year) )  // 1�����꣬0��ƽ��
			{
				if( day_num >= 366 )
				{
					day_num -= 366; //��������ȥ����366��֮��������1
				}
				else 
				{
					year++;
					break;
				}  
			}
			else day_num -= 365;    // ��������ȥƽ��365��֮��������1
			year++;  
		}   
		calendar_t.w_year = year;	// �õ����
		year = 0;
		while(day_num>=28)			// ������һ���¾ͼ��·�
		{
			if( IsLeapYear(calendar_t.w_year)&&year==1 )//�����ǲ�������2�·�
			{
				if( day_num >= 29 )
					day_num -= 29;	//��ȥ��ݵ�����֮�����������ȥ����2��29��֮���·ݼ�1
				else 
					break; 
			}
			else 
			{
				if( day_num >= mon_table[year] )
					day_num -= mon_table[year];//ƽ��
				else break;
			}
			year++;  
		}
		calendar_t.w_month = year+1;	//�õ��·�
		calendar_t.w_date  = day_num+1; //�õ����� 
	}
	day_num=rtc_count%86400;     		//һ����֮�ڵ�������   	   
	calendar_t.hour=day_num/3600;     	//Сʱ
	calendar_t.min=(day_num%3600)/60; 	//����	
	calendar_t.sec=(day_num%3600)%60; 	//����
	calendar_t.week=RTC_GetWeek(calendar_t.w_year,calendar_t.w_month,calendar_t.w_date);//��ȡ����   
	return 0;
}

/********************************************************************************************************
** ����: RTC_GetWeek,  ������������ڼ�
**------------------------------------------------------------------------------------------------------
** ����: uint16 year,uint8 month,uint8 day
** ����: ���ں�											  
********************************************************************************************************/
uint8 RTC_GetWeek(uint16 year,uint8 month,uint8 day)
{
	uint16 week;
	uint8 yearH,yearL;
	
	yearH = year/100;	
	yearL = year%100; 
	// ���Ϊ21����,�������100  
	if ( yearH>19 )
		yearL += 100;
	// ����������ֻ��1900��֮���  
	week = yearL+yearL/4;
	week = week%7; 
	week = week+day+table_week[month-1];
	if( yearL%4==0&&month<3 )
		week--;
	return (week%7);
}

/********************************************************************************************************
** ����: RTC_Set,  RTC��������ʱ�亯������1970��1��1��Ϊ��׼���������ʱ��ת��Ϊ���ӣ�
**------------------------------------------------------------------------------------------------------
** ����: uint16 year,uint8 mon,uint8 day,uint8 hour,uint8 min,uint8 sec
** ����: 0���ɹ�����1��ʧ�ܣ�											  
********************************************************************************************************/
uint8 RTC_Set(uint16 year,uint8 mon,uint8 day,uint8 hour,uint8 min,uint8 sec)
{
	uint16 t;
	uint32 seccount=0;
	
	/* ��1970������������������������ */
	if( year<1970||year>2099 )	
		return 1;	 
	for(t=1970;t<year;t++)	//��������ݵ��������
	{
		if(IsLeapYear(t))
		{
			seccount += 31622400;    //�����������
		}
		else 
		{
			seccount += 31536000;  //ƽ���������
		}
	}
	
	/* ��ĳ��1�������µ������������������·� */
	mon -= 1; //mon_table���������
	for( t=0; t<mon; t++)	   //��ǰ���·ݵ����������
	{
		seccount += (u32)mon_table[t]*86400;
		if(IsLeapYear(year)&&t==1)
		{
			seccount += 86400;//����2�·�����һ���������	
		}			
	}
	
	/* ��ĳ��1������������������������ú��� */
	seccount += (u32)(day-1)*86400;
	
	/* ��ĳ��0ʱ����ǰСʱ������������������Сʱ */
	seccount += (u32)hour*3600;
	
	/* ��ĳСʱ0������ǰ���ӵ����������������÷��� */
    seccount += (u32)min*60;	 
	
	/* ��ĳ����0������ǰ����������������������� */
	seccount += sec;
	
	/* ʹ��PWR��BKP����ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	 
	PWR_BackupAccessCmd(ENABLE);// ʹ��RTC�ͺ󱸼Ĵ������� 
	RTC_SetCounter(seccount);	// ����RTC��������ֵ
	RTC_WaitForLastTask();	    // �ȴ����һ�ζ�RTC�Ĵ�����д�������  	
	
	return 0;	    
}

/********************************************************************************************************
** ����: DisplayTime,  Һ������ʾʱ��
**------------------------------------------------------------------------------------------------------
** ����: time:�����ṹ�����
** ����: ��		
** ����: 2017.12.13 by Lizn
********************************************************************************************************/
char time_buf[32] = {0};
void DisplayTime(Calendar_T time)
{
	//------------------------------------------------------------------------------------------
	//��ӡ������ʱ����������������
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskYearTxt.txt=\"%.4d\"", time.w_year);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskMonthTxt.txt=\"%.2d\"", time.w_month);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskDayTxt.txt=\"%.2d\"", time.w_date);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskHourTxt.txt=\"%.2d\"", time.hour);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskMinTxt.txt=\"%.2d\"", time.min);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskSecTxt.txt=\"%.2d\"", time.sec);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
	sprintf(time_buf, "Desk.DeskWeekTxt.txt=\"%s\"", wday[time.week]);	
	HMISendStr(time_buf);
	HMISendByte(0xff);
	memset(time_buf, 0, sizeof(time_buf));
}

u8 g_time_flag = 0;       //���жϱ�־λ
u8 WeatherIconFlag = TRUE;
/********************************************************************************************************
** ����: RTC_IRQHandler,  RTC�жϴ�����
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��								  
********************************************************************************************************/
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//�����ж�
	{	
		RTC_Get();		 //����ʱ��  
		//------------------------------------------------------------------------------------------
		//������ʾ
		printf("RTC Time:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\t%s\r\n",
		calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,
		calendar_t.hour,calendar_t.min,calendar_t.sec,wday[calendar_t.week]);
		//------------------------------------------------------------------------------------------
		//Һ����ʾ
		DisplayTime(calendar_t);
 	}			  								 
	RTC_ClearITPendingBit(RTC_IT_SEC);// �����ж�
	RTC_WaitForLastTask();	  	    			// �ȴ����һ�ζ�RTC�Ĵ�����д�������  				 	   	 
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
