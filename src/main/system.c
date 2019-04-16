/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�: system.c 
** �汾: v1.0
**------------------------------------------------------------------------------------------------------
** ����:
**      ϵͳ�ĳ�ʼ��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "system.h"

char Init_buf[32] = {0};

void  LD3320_main(void);
extern int SwithToUtf_8(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);
//�����ļ����õĺ���
static void Task_GetWeather(void);		// ��ȡ��������
static void Task_RecHMIDate(void);		// ���մ��������·�������
static void Task_DisplayWeather(void);	// ��������������������
static void Task_UpdateIcon(void);		// ������������ͼ����������
static void Task_TimeCalibration(void);	// ʱ��У׼
static void Task_DHT11Collect(void);	// �ɼ���ʪ������
static void Task_ASR(void);				// ����ʶ��
static void InitSuccess(void);			// ��ʼ���ɹ�

/*******************************************************************************************************
** ����: CpuInit,����ϵͳ��Ϣ
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��
** ˵��: system_stm32f10x.c�ڵ�SystemInit�����Ѿ��ж�ϵͳһЩ�������ã����ɻ������������ڵ���
		 ������һЩ����,�ĺ�����������������֮ǰ����
********************************************************************************************************/
void CpuInit( void )
{
  	//-------------------------------------------------------------------------------
	// �رյ��Զ˿�����ӳ�� �����ж����ȼ�
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  		// �رյ��� �˿�����ӳ��
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);					// ѡ���3����ж����ȼ�
}

/*******************************************************************************************************
** ����: PrintfInfo����ӡ���̱���
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
********************************************************************************************************/
void PrintfInfo(void)
{
	printf("----------------------| ���Ʊ�ҵ��� |----------------------\n");
	printf("\n\n");
	printf("                     ��������Ԥ��ϵͳ                        \n");
	printf("\n\n");
	printf("------------------------------------------------------------\n");
}

/*******************************************************************************************************
** ����: SysInit���ϵ��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void SysInit(void)
{
	SysTick_Init(72);									// ��ʼ��ϵͳʱ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);		// ѡ���3����ж����ȼ�
	SYN6288_UART4_Init(9600);							// ��ʼ��TTSģ��
	delay_ms(1000);										// ��ʼ������ʱ1S���ң������������ܲ�������
	TTSPlay(0,"����һ����������Ԥ��ϵͳ��[2]�ҽ�[2]С��");	// ����
	HMI_USART3_init(9600);								// ��������ʼ��,Ĭ�ϲ�����Ϊ9600!!
	my_mem_init(SRAMIN);								// ��ʼ���ڲ��ڴ��
	USART1_Init(9600);      							// ����1��ʼ��
	PrintfInfo();										// ��ӡ���̱���
	RTC_Init();											// RTC��ʼ��
	ESP8266_USART2_Init(115200);						// ESP8266 WIFI��ʼ��,Ĭ�ϲ�����Ϊ115200!!
	TIM1_Init(2000-1, 36000-1);							// 1s���һ��
	RadioInit();										// ��������ʼ��
	DHT11_Init();										// DHT11��ʪ�ȴ�����GPIO��ʼ��
	LD3320_Init();										// ��ʼ������ʶ��ģ��
	InitSuccess();										// ��ʼ������
}

/*******************************************************************************************************
** ����: BackTask����̨����
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void BackTask(void)
{
	Task_RecHMIDate();		// ���մ������·�������
	Task_GetWeather();		// ��ȡ����Ԥ������
	Task_DisplayWeather();  // ��������������������
	Task_UpdateIcon();		// ��������ͼ��
	Task_TimeCalibration(); // ʱ��У׼
	Task_DHT11Collect();	// �ɼ���ʪ������
	Task_ASR();				// ����ʶ��
}

/*******************************************************************************************************
** ����: TaskGetWeather����̨����--��ȡ��������,ÿһ��Сʱץһ������Ԥ�����ݰ�
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: 1:�ɹ� 0:ʧ��
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_GetWeather(void)
{
	if( GetWeatherTimer ) return;
	GetWeatherTimer = TIMER1_HOUR;
	memset(&weather_data, 0, sizeof(weather_data));
	GET_NowWeather();
	GET_DailyWeather();
}

/*******************************************************************************************************
** ����: Task_RecHMIDate�����մ������·�������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char *p = NULL;
static void Task_RecHMIDate(void)
{
//	if(GetHMIDataTimer) return;
//	GetHMIDataTimer = 1;
	if(USART3_RX_STA&0x8000)
	{
		/* ҳ���ʶ */
		if((p=strstr((char*)USART3_RX_BUF,"page:"))!=NULL)
		{
			if(sscanf(p,"page:%s",g_page)==1)
			{
				printf("page:%s\n",g_page);
				DisplayWeather(weather_data);
//				DisplayWeatherIcon(weather_data);
				if((g_connect_sta==1)&&(strcmp(g_page,"Desk")==0))
				{
					DisplayWifiIcon(g_connect_sta);
				}
			}
		}
		/* wifi���� */
		else if((p=strstr((char*)USART3_RX_BUF,"ssid:"))!=NULL)
		{
			if(sscanf(p,"ssid:%s",g_wifi_ssid)==1)
			{
				printf("ssid:%s\n",g_wifi_ssid);
			}
		}
		/* wifi���� */
		else if((p=strstr((char*)USART3_RX_BUF,"passwd:"))!=NULL)
		{
			if(sscanf(p,"passwd:%s",g_wifi_password)==1)
			{
				printf("passwd:%s\n",g_wifi_password);
				g_connect_sta = ESP8266_ConnectAP(g_wifi_ssid, g_wifi_password);
				printf("g_connect_sta = %d\n",g_connect_sta);
				while(!g_connect_sta);
				DisplayWifiIcon(g_connect_sta);
				GET_BeiJingTime();								// ��ȡ����ʱ��
				printf("wifi connect success!\n");
				TTSPlay(0, "[t3][2]�������ӳɹ���");
			}
		}
		/* ��������:China or English */
		else if((p=strstr((char*)USART3_RX_BUF,"SerchMethod:"))!=NULL)
		{
			memset(g_serch_method, 0, 32);
			if(sscanf(p,"SerchMethod:%s",g_serch_method)==1)
			{
				printf("SerchMethod:%s\n",g_serch_method);
			}
		}
		/* ���� */
		else if((p=strstr((char*)USART3_RX_BUF,"place:"))!=NULL)
		{
			if(sscanf(p,"place:%s",g_place)==1)
			{
				if(strcmp(g_serch_method,"China")==0)
				{
					SwithToUtf_8((const unsigned char*)g_place, strlen((const char*)g_place),(unsigned char*) g_place_utf8, &utf8_len);
					printf("place:%s\n",g_place_utf8);
					printf("place:%X%X%X %X%X%X\n",g_place_utf8[0],g_place_utf8[1],g_place_utf8[2],
					g_place_utf8[3],g_place_utf8[4],g_place_utf8[5]);
					sprintf(g_city,"%%%X%%%X%%%X%%%X%%%X%%%X",g_place_utf8[0],g_place_utf8[1],
					g_place_utf8[2],g_place_utf8[3],g_place_utf8[4],g_place_utf8[5]);
				}
				else if(strcmp(g_serch_method,"English")==0)
				{
					printf("place:%s\n",g_place);
					memcpy(g_city,g_place,sizeof(g_place));
				}
				memset(&weather_data, 0, sizeof(weather_data));
				GET_NowWeather();
				GET_DailyWeather();
				GetWeatherTimer = TIMER1_HOUR;	
				TTSPlay(0, "[t3][2]����Ϊ����ʾ������Ϣ��");
				DisplayWeather(weather_data);
				DisplayWeatherIcon(weather_data);
				
			}
		}
		/* Ƶ������ */
		else if((p=strstr((char*)USART3_RX_BUF,"freq_int:"))!=NULL)
		{
			if(sscanf(p,"freq_int:%s",g_freq_int)==1)
			{
				printf("freq_int:%s(%d)\n",g_freq_int,g_freq_int[0]);
			}
		}
		/* Ƶ��С�� */
		else if((p=strstr((char*)USART3_RX_BUF,"freq_deci:"))!=NULL)
		{
			if(sscanf(p,"freq_deci:%s",g_freq_deci)==1)
			{
				printf("freq_deci:%s(%d)\n",g_freq_deci,g_freq_deci[0]);
				g_tRadio.ulFreq = g_freq_int[0]*1000000 + g_freq_deci[0]*100000;
				printf("g_tRadio.ulFreq=%d\n",g_tRadio.ulFreq);
				tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, ADC_LEVEL_7);
			}
		}
		memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
		p = NULL;
		USART3_RX_STA = 0;
	}
}

/*******************************************************************************************************
** ����: Task_DisplayWeather����ʱ��������������������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: 1:�ɹ� 0:ʧ��
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_DisplayWeather(void)
{
	if(SendWeatherTimer) return;
	SendWeatherTimer = 10*TIMER1_SEC;
	DisplayWeather(weather_data);
}

/*******************************************************************************************************
** ����: Task_UpdateIcon���ض�ʱ���������ͼ����������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ˵��:�жϵ�ǰʱ��Ϊ�賿3����4��䲢�Ҵ��������棬����3�㵽4��ĸ��±�־ΪFALSE��
		��ִ�и���ͼ�������������֮�� ��3�㵽4��ĸ��±�־��ΪTRUE����ʹ�������ʱ��
		��Ҳ������¡�������������賿0������ �޸�3����4��ĸ��±�־ΪFLASE����������
		��һ�θ��£�Ҫ�����Ӧ��ʱ��Σ�����������
** ����: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_UpdateIcon(void)
{
	if( 0==calendar_t.hour )
	{
		WeatherIconFlag = FALSE;
	}
	else if( (3==calendar_t.hour) && (FALSE==WeatherIconFlag) )
	{
		DisplayWeatherIcon(weather_data);
		WeatherIconFlag = TRUE;
	}
}

/*******************************************************************************************************
** ����: Task_TimeCalibration��ʱ��У׼��һ��У׼һ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: 1:�ɹ� 0:ʧ��
** ����: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void Task_TimeCalibration(void)
{
	if(TimeCalibraTimer) return;
	TimeCalibraTimer = TIMER1_DAY;
	GET_BeiJingTime();
}

/*******************************************************************************************************
** ����: Task_DHT11Collect��DHT11��ʪ�����ݲɼ���20s�ɼ�һ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: 1:�ɹ� 0:ʧ��
** ����: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 temperature=0,humidity=0;
static void Task_DHT11Collect(void)
{
	if(DHT11CollectTimer) return;
	DHT11CollectTimer = 20*TIMER1_SEC;
	DHT11_Read_Data(&temperature, &humidity);
//	printf("temperatur:%d;humidity:%d\n",temperature,humidity);
}

/*******************************************************************************************************
** ����: Task_ASR������ʶ������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: 1:�ɹ� 0:ʧ��
** ˵��:	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
			LD_ASR_NONE:		��ʾû������ASRʶ��
			LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
			LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
			LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
			LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
** ����: 2018.1.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 nAsrStatus = 0;
uint8 nAsrRes=0;
static void Task_ASR(void)
{
	switch(nAsrStatus)
	{
		case LD_ASR_RUNING:		
		case LD_ASR_ERROR:		
			break;
		case LD_ASR_NONE:
			nAsrStatus=LD_ASR_RUNING; 
			if (RunASR()==0)	//	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
			{		
				nAsrStatus = LD_ASR_ERROR;
			}
			break;

		case LD_ASR_FOUNDOK:
			nAsrRes = LD_GetResult( );	//	һ��ASRʶ�����̽�����ȥȡASRʶ����	
			ASRSuccess_Handle(nAsrRes);
			nAsrStatus = LD_ASR_NONE;	
		break;
		
		case LD_ASR_FOUNDZERO:
		default:
			nAsrStatus = LD_ASR_NONE;	
			break;
	}
}

/*******************************************************************************************************
** ����: InitSuccess��ϵͳ��ʼ������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2018.1.11 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void InitSuccess(void)
{
	PrintfToHMI("Init Success!" ,"page_Init", "InitSccessTxt", "txt");
	delay_ms(1000);
	HMISendStr("page Desk");
	HMISendByte(0xff);
}
/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
