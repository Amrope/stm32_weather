#include "http_client.h"

/* ����һЩȫ�ֱ��� */
//------------------------------------------------------------------------------------------
//����Ҫ����������
Result_T weather_data;	

//�洢��ȡ�õ������ڣ����ڽ�����������2017-12-8������2017 12 8��Ϊ��RTC_GetWeek����ʵ���Լ�������ڹ���������ʾ��
date_T g_date[3] = {0};

//�洢���ڶ�Ӧ������
int g_week[3] = {0};

//��������ҳ���ʶ
char g_page[32] = {0};

//WiFi���ӵ�״̬ 1:δ����   0������
uint8 g_connect_sta = 0;

//���ļ�����������ֻ�ܹ����ļ�����
static int cJSON_NowWeatherParse(char *JSON, Result_T *result);
static int cJSON_DailyWeatherParse(char *JSON, Result_T *result);
//static int cJSON_LifeSuggestionParse(char *JSON, Result_T *result);
static uint8 ParseBeijingTime(char* time_data);
static uint8 ParseDate(char *buf, date_T *date);

/********************************************************************************************************
** ����: ESP8266_TCPClient_Test,����TCP Client�Ƿ������ݳɹ�
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void	
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void ESP8266_TCPClient_Test(void)
{
	char *str="hello world!\r\n";
	
	printf ( "\r\nESP8266 TCP Client Test\r\n" );
	ESP8266_CH_PD_Pin_SetH;
	ESP8266_LinkServer(enumTCP, (uint8*)My_Computer, (uint8*)My_Port);
	ESP8266_ExitTran();//�˳�͸��
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //��ʼ͸��	
	ESP8266_Printf( "%s", str );
}

/********************************************************************************************************
** ����: GET_NowWeather, GET ����ʵ�����ݰ�now.json
**------------------------------------------------------------------------------------------------------
** ����: void
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:��ȡ�ɹ� ��������ȡʧ��	
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_NowWeather(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)WEATHER_IP, (uint8*)WEATHER_PORT);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //��ʼ͸��
	ESP8266_Printf("GET https://api.seniverse.com/v3/weather/now.json?"
	"key=2owqvhhd2dd9o9f9&location=%s&language=zh-Hans&unit=c\r\n\r\n",g_city); //��Ҫ����\r\n\r\n,����language��uintΪ��ѡ����	
	delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
	USART2_RX_STA=0;	//���㴮��2����
	delay_ms(1000);
	
	if(USART2_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
	} 
//	printf("Receive now data:%s\n",USART2_RX_BUF);
//	printf("---------Now len = %d\n",strlen((char*)USART2_RX_BUF));  //266Byte
	
	cJSON_NowWeatherParse((char*)USART2_RX_BUF, &weather_data);	

	ESP8266_ExitTran();//�˳�͸��
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //�ر�����
	
	return 0;
}

/********************************************************************************************************
** ����: GET_DailyWeather, GET δ�����������Ԥ��daily.json
**------------------------------------------------------------------------------------------------------
** ����: void
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:��ȡ�ɹ� ��������ȡʧ��	
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_DailyWeather(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)WEATHER_IP, (uint8*)WEATHER_PORT);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //��ʼ͸��
	ESP8266_Printf("GET https://api.seniverse.com/v3/weather/daily.json?"
	"key=2owqvhhd2dd9o9f9&location=%s&language=zh-Hans&unit=c&start=0&days=5\r\n\r\n",g_city); //��Ҫ����\n\n,����language��uintΪ��ѡ����	
	delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
	USART2_RX_STA=0;	//���㴮��3����
	delay_ms(1000);
	
	if(USART2_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
	} 
//	printf("Receive daily data:%s\n",USART2_RX_BUF);
//	printf("---------Daily len = %d\n",strlen((char*)USART2_RX_BUF));  //904Byte
	
	cJSON_DailyWeatherParse((char*)USART2_RX_BUF, &weather_data);
	
	ESP8266_ExitTran();//�˳�͸��
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //�ر�����
	
	return 0;
}
#if 0
/********************************************************************************************************
** ����: GET_LifeSuggestion, GET һЩ����
**------------------------------------------------------------------------------------------------------
** ����: void
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:��ȡ�ɹ� ��������ȡʧ��	
** ����: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_LifeSuggestion(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)WEATHER_IP, (uint8*)WEATHER_PORT);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //��ʼ͸��
	ESP8266_Printf("GET https://api.seniverse.com/v3/life/suggestion.json?"
	"key=2owqvhhd2dd9o9f9&location=%s&language=zh-Hans\r\n\r\n",g_city); //��Ҫ����\n\n,����language��uintΪ��ѡ����	
	delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
	USART2_RX_STA=0;	//���㴮��2����
	delay_ms(1000);
	
	if(USART2_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
	} 
//	printf("Receive daily data:%s\n",USART2_RX_BUF);
//	printf("---------Daily len = %d\n",strlen((char*)USART2_RX_BUF));  //
	
	cJSON_LifeSuggestionParse((char*)USART2_RX_BUF, &weather_data);
	
	ESP8266_ExitTran();//�˳�͸��
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //�ر�����
	
	return 0;
}
#endif

/********************************************************************************************************
** ����: GET_BeiJingTime, ��ȡ����ʱ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:��ȡ�ɹ� ��������ȡʧ��	
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 GET_BeiJingTime(void)
{
	ESP8266_LinkServer(enumTCP, (uint8*)TIME_SERVERIP, (uint8*)TIME_PORTNUM);
	delay_ms(300);
	ESP8266_SendATCmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	
	USART2_RX_STA=0;
	ESP8266_SendATCmd("AT+CIPSEND","OK",100);         //��ʼ͸��
	ESP8266_Printf("GET http://www.beijing-time.org/time15.asp\r\n\r\n");
	delay_ms(20);
	USART2_RX_STA=0;	
	delay_ms(1000);
	printf("\r\n\r\n");
	printf("Receive BeiJing Time:%s\n",USART2_RX_BUF);
	
	ParseBeijingTime((char*)USART2_RX_BUF);
	
	ESP8266_ExitTran();//�˳�͸��
	ESP8266_SendATCmd("AT+CIPCLOSE","OK",50);         //�ر�����
	return 0;
}

/********************************************************************************************************
** ����: cJSON_NowWeatherParse, ��������ʵ�����ݰ�now.json
**------------------------------------------------------------------------------------------------------
** ����: JSON:����ʵ�����ݰ�  Result_T:�洢�����õ������õ�����
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:�����ɹ� ����������ʧ��		
** ����: 2017.12.6 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static int cJSON_NowWeatherParse(char *JSON, Result_T *result)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	
	utf8str = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	
	memset(utf8str, 0, 32);
	memset(gbkstr, 0, 32);
	
//	printf("Receive now data:%s\n",JSON);
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //ƥ���ַ���"results",��ȡ��������
		{
			int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
//			printf("cJSON_GetArraySize: size=%d\n",size); 
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//��ȡ����������
			{
				/* ƥ���Ӷ���1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("--------------------------------Now Weather Data------------------------------\n");
					printf("---------------------------------subobject1-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //����--��Ҫ�õ�������
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->name, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
				}
				/* ƥ���Ӷ���2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					printf("---------------------------------subobject2-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//����Ԥ������--��Ҫ�õ�������
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->text, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//����Ԥ������--��Ҫ�õ�������
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->code, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //�¶�--��Ҫ�õ�������
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->temperature, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
				}
				/* ƥ���Ӷ���3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					printf("---------------------------------subobject3-------------------------------\n");
					printf("cJSON_GetObjectItem: %s:%s\n\n",subobject->string,subobject->valuestring);
				}
			} 
		}
	}
	
	cJSON_Delete(json); //�ͷ�cJSON_Parse()����������ڴ�ռ�
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	
	return 0;

}

/********************************************************************************************************
** ����: cJSON_NowWeatherParse, ����δ�����������Ԥ��daily.json
**------------------------------------------------------------------------------------------------------
** ����: JSON:δ�����������Ԥ�����ݰ�  Result_T:�洢�����õ������õ�����
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:�����ɹ� ����������ʧ��			
** ����: 2017.12.6 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static int cJSON_DailyWeatherParse(char *JSON, Result_T *result)
{
	cJSON *json,*arrayItem,*object,*subobject,*item,*sub_child_object,*child_Item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	int i;
	
	utf8str = (char*)mymalloc(SRAMIN, 64*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 64*sizeof(char));
	
	memset(utf8str, 0, 64);
	memset(gbkstr, 0, 64);
	
	
//	printf("Receive daily data:%s\n",JSON);
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //ƥ���ַ���"results",��ȡ��������
		{
			int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
			printf("Get Array Size: size=%d\n",size); 
			
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//��ȡ����������
			{
				/* ƥ���Ӷ���1------�ṹ��location */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("---------------------------Daily Weather Data-------------------------------\n");
					printf("---------------------------------subobject1-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //ƥ���Ӷ���1��Ա"name"
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
						printf("cJSON_GetObjectItem: %s:%s\n", item->string, gbkstr);
					}
				}
				/* ƥ���Ӷ���2------����daily */
				if((subobject = cJSON_GetObjectItem(object,"daily")) != NULL)
				{
					int sub_array_size = cJSON_GetArraySize(subobject);
					printf("---------------------------------subobject2-------------------------------\n");
					printf("Get Sub Array Size: sub_array_size=%d\n",sub_array_size);
					for(i=0;i<sub_array_size;i++)
					{
						if((sub_child_object = cJSON_GetArrayItem(subobject,i))!=NULL)
						{
							memset(utf8str, 0, 64);
							memset(gbkstr, 0, 64);
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"date")) != NULL)//����
							{
								memcpy(result->date[i], child_Item->valuestring, strlen(child_Item->valuestring));
								ParseDate(result->date[i], &g_date[i]);			//�������ڵõ�������
								g_week[i] = RTC_GetWeek(g_date[i].date_year, g_date[i].date_month, g_date[i].date_day); //�������
								printf("daliy[%d]--%s:%s(After parse:%d %d %d %s)\n",i ,child_Item->string, child_Item->valuestring,
								g_date[i].date_year, g_date[i].date_month, g_date[i].date_day,wday[g_week[i]]);
							}
							memset(utf8str, 0, 64);
							memset(gbkstr, 0, 64);
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"text_day")) != NULL)//����������������
							{
								utf8str = child_Item->valuestring;
								SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, gbkstr);
								memcpy(result->text_day[i], gbkstr, strlen(gbkstr));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"code_day")) != NULL)//���������������
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->code_day[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"code_night")) != NULL)//ҹ�������������
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->code_night[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"high")) != NULL)//����¶�
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->high[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"low")) != NULL)//����¶�
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->low[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							memset(utf8str, 0, 64);
							memset(gbkstr, 0, 64);
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"wind_direction")) != NULL)//����
							{
								utf8str = child_Item->valuestring;
								SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, gbkstr);
								memcpy(result->wind_direction[i], gbkstr, strlen(gbkstr));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"wind_speed")) != NULL)//���٣���λkm/h����unit=cʱ��
							{
								printf("daliy[%d]--%s:%s\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->wind_speed[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
							if((child_Item = cJSON_GetObjectItem(sub_child_object,"wind_scale")) != NULL)//�����ȼ�
							{
								printf("daliy[%d]--%s:%s\n\n",i ,child_Item->string, child_Item->valuestring);
								memcpy(result->wind_scale[i], child_Item->valuestring, strlen(child_Item->valuestring));
							}
						}
					}
				}
			} 
		}
		/* �����ı���� */
		sprintf(g_WeatherText,"֪������Ϊ����������������%s�н�����쵽���[2]%s��[3]�����%s[3]���϶ȣ�[3]�����%s[3]���϶ȣ�[3]%s[2]%s��",
		result->name, result->text[0], result->high[0], result->low[0], result->wind_direction[0], result->wind_scale[0]);
	}
	
	cJSON_Delete(json); //�ͷ�cJSON_Parse()����������ڴ�ռ�
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	
	return 0;
}

#if 0
/********************************************************************************************************
** ����: cJSON_LifeSuggestionParse, ��������ָ���������
**------------------------------------------------------------------------------------------------------
** ����: JSON:δ�����������Ԥ�����ݰ�  Result_T:�洢�����õ������õ�����
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:�����ɹ� ����������ʧ��			
** ����: 2018.1.1 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static int cJSON_LifeSuggestionParse(char *JSON, Result_T *result)
{
	cJSON *json,*arrayItem,*object,*subobject,*childobject,*item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	
	utf8str = (char*)mymalloc(SRAMIN, 200*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 200*sizeof(char));
	
	memset(utf8str, 0, 200);
	memset(gbkstr, 0, 200);
	
//	printf("Receive LifeSuggestion data:%s\n",JSON);
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //ƥ���ַ���"results",��ȡ��������
		{
			int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
			printf("cJSON_GetArraySize: size=%d\n",size); 
			if((object = cJSON_GetObjectItem(arrayItem,0)) != NULL)//��ȡ����������
			{
				if((subobject = cJSON_GetObjectItem(object,"suggestion")) != NULL)
				{
					if((childobject = cJSON_GetObjectItem(subobject,"flu"))!= NULL)
					{
						if((item=cJSON_GetObjectItem(childobject,"brief"))!=NULL)
						{
							utf8str = item->valuestring;
							SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
							printf("cJSON_GetObjectItem: %s:%s\n\n",item->string,gbkstr);
							memcpy(result->life_suggestion, gbkstr, strlen(gbkstr));
						}
					}
				}
			}
		}
	}
}
#endif

/********************************************************************************************************
** ����: ParseBeijingTime, ������ȡ�õ��ı���ʱ��
**------------------------------------------------------------------------------------------------------
** ����: time_data:����ʱ��
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:�����ɹ� ����������ʧ��	
** ����: 2017.12.10 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static uint8 ParseBeijingTime(char* time_data)
{
	uint8 res = 0;
	
	if(sscanf((char*)time_data,"t0=new Date().getTime();\r\nnyear=%d;\r\nnmonth=%d;"
		"\r\nnday=%d;\r\nnwday=%d;\r\nnhrs=%d;\r\nnmin=%d;\r\nnsec=%d;",
		(int*)&calendar_t.w_year, (int*)&calendar_t.w_month, (int*)&calendar_t.w_date,
			(int*)&calendar_t.week,(int*)&calendar_t.hour,(int*)&calendar_t.min,(int*)&calendar_t.sec)!=7)
	{
		return -1; //����ʧ��
	}
	RTC_Set(calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,calendar_t.hour,calendar_t.min,calendar_t.sec); 
	//�������
	printf("Time after update:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
	calendar_t.w_year,calendar_t.w_month,calendar_t.w_date,calendar_t.hour,calendar_t.min,calendar_t.sec);
	
	return res;		//�����ɹ�
}

/********************************************************************************************************
** ����: ParseDate, ����date,���"2017-12-14"�н�����������,Ŀ����Ҫ��������ڹ���������ʾ
**------------------------------------------------------------------------------------------------------
** ����: buf:���������ݽ�������������date, date_T date:��������õ��Ľ��
** ����: 0:�����ɹ� ����������ʧ��		
** ����: 2017.12.13 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static uint8 ParseDate(char *buf, date_T *date)
{
	uint8 res = 0;
	
//	printf("-----------------------%s %s\n", __FUNCTION__, buf);
	if(sscanf((char*)buf,"%d-%d-%d",&date->date_year,&date->date_month,&date->date_day)!=3)
	{
		printf("%s:Parse Date Error!\n",__FUNCTION__);
		return -1;
	}
	//�������
//	printf("-----------------------%d %d %d\n",date->date_year,date->date_month,date->date_day);
	
	return res;
}

/********************************************************************************************************
** ����: DisplayWeather, ��������������Һ������������
**------------------------------------------------------------------------------------------------------
** ����: weather����ȡ������������
** ����: ��		
** ˵��: ����˵������Desk.DeskPlaceTxt.txt����DeskΪҳ�����ƣ�DeskPlaceTxtΪ�ı��ؼ����ƣ�txtΪ�ؼ���txt���ԡ�
**		����ҳ��������ؼ������ǿɸ��ĵģ����������ǲ��ɸ��ĵġ�		
** ����: 2017.12.13 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char pbuf[64] = {0};
void DisplayWeather(Result_T weather)
{
	//------------------------------------------------------------------------------------------
	//��ʾ������������
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskPlaceTxt.txt=\"%s\"", weather.name);	
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNowTempTxt.txt=\"%s��\"", weather.temperature);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskHumiTxt.txt=\"%d%%\"", humidity);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskWindDirect.txt=\"%s\"", weather.wind_direction[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskWindScale.txt=\"%s��\"", weather.wind_scale[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayTxt.txt=\"%s\"", wday[g_week[0]]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayTempH.txt=\"%s��\"", weather.high[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayTempL.txt=\"%s��\"", weather.low[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTodayWeath.txt=\"%s\"", weather.text_day[0]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	//------------------------------------------------------------------------------------------
	//��ʾ������������
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorTxt.txt=\"%s\"", wday[g_week[1]]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorTempH.txt=\"%s��\"", weather.high[1]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorTempL.txt=\"%s��\"", weather.low[1]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskTomorWeath.txt=\"%s\"", weather.text_day[1]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	//------------------------------------------------------------------------------------------
	//��ʾ������������
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextTxt.txt=\"%s\"", wday[g_week[2]]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextTempH.txt=\"%s��\"", weather.high[2]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextTempL.txt=\"%s��\"", weather.low[2]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "Desk.DeskNextWeath.txt=\"%s\"", weather.text_day[2]);
	HMISendStr(pbuf);
	HMISendByte(0xff);
	//------------------------------------------------------------------------------------------
	//��ʾ��ܰ��ʾ
	if(atoi(weather.temperature)>=20)
	{
		memset(pbuf, 0, sizeof(pbuf));
		sprintf(pbuf, "Desk.DeskRemindTxt.txt=\"%s\"", g_high_suggestion);
		HMISendStr(pbuf);
		HMISendByte(0xff);
	}
	else
	{
		memset(pbuf, 0, sizeof(pbuf));
		sprintf(pbuf, "Desk.DeskRemindTxt.txt=\"%s\"", g_low_suggestion);
		HMISendStr(pbuf);
		HMISendByte(0xff);
	}
}

/********************************************************************************************************
** ����: DisplayWeatherIcon, ��������ͼ����Һ������������
**------------------------------------------------------------------------------------------------------
** ����: weather����ȡ������������
** ����: ��		
** ˵��: ˢͼָ�pic x,y,picid  (x:��ʼ��x���꣬y:��ʼ��y���꣬picid:ͼƬID)
** ����: 2017.12.18 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char icon_buf[32] = {0};
void DisplayWeatherIcon(Result_T weather)
{
	if(strcmp(g_page,"page_Desk")==0)
	{
		printf("page Desk!\n");
		memset(icon_buf, 0, sizeof(icon_buf));
		sprintf(icon_buf, "pic %d,%d,%d", Today_Icon_X, Today_Icon_Y, Today_Icon_ID);
		HMISendStr(icon_buf);
		HMISendByte(0xff);
		memset(icon_buf, 0, sizeof(icon_buf));
		sprintf(icon_buf, "pic %d,%d,%d", Tomorrow_Icon_X, Tomorrow_Icon_Y, Tomorrow_Icon_ID);
		HMISendStr(icon_buf);
		HMISendByte(0xff);
		memset(icon_buf, 0, sizeof(icon_buf));
		sprintf(icon_buf, "pic %d,%d,%d", AfterTomor_Icon_X, AfterTomor_Icon_Y, AfterTomor_Icon_ID);
		HMISendStr(icon_buf);
		HMISendByte(0xff);
	}
}

/********************************************************************************************************
** ����: DisplayWifiIcon, ����wifiͼ����Һ������������
**------------------------------------------------------------------------------------------------------
** ����: weather����ȡ������������
** ����: ��		
** ˵��: ˢͼָ�pic x,y,picid  (x:��ʼ��x���꣬y:��ʼ��y���꣬picid:ͼƬID)
** ����: 2018.1.7 by Hezhijie and Lizhengnian 
********************************************************************************************************/
char wifi_icon_buf[32] = {0};
void DisplayWifiIcon(uint8 connect_sta)
{
	memset(wifi_icon_buf, 0, sizeof(wifi_icon_buf));
	if(connect_sta)
	{
		sprintf(wifi_icon_buf, "pic %d,%d,%d", Wifi_Icon_X, Wifi_Icon_Y, Wifi_ConnectIcon_ID);
	}
	else
	{
		sprintf(wifi_icon_buf, "pic %d,%d,%d", Wifi_Icon_X, Wifi_Icon_Y, Wifi_DisconnIcon_ID);
	}
	HMISendStr(wifi_icon_buf);
	HMISendByte(0xff);
}

/********************************************************************************************************
** ����: Display, ������--���ڼ�����������Ƿ񱣴�ɹ�
**------------------------------------------------------------------------------------------------------
** ����: result:�������������������
** ����: ��	
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void DisplayTest(Result_T result)
{
	printf("---------------------------------Display Test-------------------------------\n");
	printf("name:%s\n",weather_data.name);
	printf("text:%s\n",weather_data.text);
	printf("code:%s\n",weather_data.code);
	printf("temperature:%s\n",weather_data.temperature);
	
	printf("date[0]:%s\n",weather_data.date[0]);
	printf("text_day[0]:%s\n",weather_data.text_day[0]);
	printf("code_day[0]:%s\n",weather_data.code_day[0]);
	printf("code_night[0]:%s\n",weather_data.code_night[0]);
	printf("high[0]:%s\n",weather_data.high[0]);
	printf("low[0]:%s\n",weather_data.low[0]);
	printf("wind_speed[0]:%s\n",weather_data.wind_speed[0]);
	printf("wind_scale[0]:%s\n",weather_data.wind_scale[0]);
	
	printf("date[1]:%s\n",weather_data.date[1]);
	printf("text_day[1]:%s\n",weather_data.text_day[1]);
	printf("code_day[1]:%s\n",weather_data.code_day[1]);
	printf("code_night[1]:%s\n",weather_data.code_night[1]);
	printf("high[1]:%s\n",weather_data.high[1]);
	printf("low[1]:%s\n",weather_data.low[1]);
	printf("wind_speed[1]:%s\n",weather_data.wind_speed[1]);
	printf("wind_scale[1]:%s\n",weather_data.wind_scale[1]);
	
	printf("date[2]:%s\n",weather_data.date[2]);
	printf("text_day[2]:%s\n",weather_data.text_day[2]);
	printf("code_day[2]:%s\n",weather_data.code_day[2]);
	printf("code_night[2]:%s\n",weather_data.code_night[2]);
	printf("high[2]:%s\n",weather_data.high[2]);
	printf("low[2]:%s\n",weather_data.low[2]);
	printf("wind_speed[2]:%s\n",weather_data.wind_speed[2]);
	printf("wind_scale[2]:%s\n",weather_data.wind_scale[2]);
}
