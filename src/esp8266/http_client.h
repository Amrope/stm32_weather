#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H

#include "config.h"

extern uint8 g_connect_sta;

/* ���ط�����--���ڲ���TCP Client�Ƿ������ݳɹ� */
#define My_Computer 	"10.48.90.54"//"192.168.199.237"  
#define My_Port 		"8080"

/* ��֪������api.seniverse.com��--�������ݵ���Դ */
#define WEATHER_IP 		"116.62.81.138"
#define WEATHER_PORT 	"80"

/* ʱ�������--����ͬ������ʱ�� */
#define TIME_SERVERIP	"www.beijing-time.org"
#define TIME_PORTNUM	"80"

/* ��������ͼ������꼰ͼƬID */
#define Today_Icon_X		140
#define Today_Icon_Y		170
#define Today_Icon_ID		(atoi(weather.code_day[0])+12) //����12����Ϊ��λ���������������ͼƬID�Ǵ�12��ʼ�ģ��ɸ��ģ�

/* ��������ͼ������꼰ͼƬID */
#define Tomorrow_Icon_X		250
#define Tomorrow_Icon_Y		170
#define Tomorrow_Icon_ID	(atoi(weather.code_day[1])+12) //����12����Ϊ��λ���������������ͼƬID�Ǵ�12��ʼ�ģ��ɸ��ģ�

/* ��������ͼ������꼰ͼƬID */
#define AfterTomor_Icon_X	250
#define AfterTomor_Icon_Y	170
#define AfterTomor_Icon_ID	(atoi(weather.code_day[2])+12) //����12����Ϊ��λ���������������ͼƬID�Ǵ�12��ʼ�ģ��ɸ��ģ�

/* wifiͼ������꼰ͼƬID */
#define Wifi_Icon_X			4
#define Wifi_Icon_Y			11
#define Wifi_ConnectIcon_ID	51
#define Wifi_DisconnIcon_ID	52

/* ���������������ı� */
// ������֪������Ϊ���������������������н�����쵽�����ƣ������25���϶ȣ������18���϶ȣ����Ϸ�2������������Ϊ�����Ⱦ
#define WeatherText1 "֪������Ϊ����������������"
#define WeatherText2 "�н�����쵽���"
#define WeatherText3 ",[3]�����"
#define WeatherText4 ",[3]�����"

/* �������ݽṹ�� */
typedef struct
{
	/* ʵ���������� */
	char name[32];				//����
	char text[32];				//����Ԥ������
	char code[32];				//����Ԥ������
	char temperature[32];   	//����
	
	/* ���졢���졢������������ */
	char date[3][32];			//����
	char text_day[3][64];	    //����������������
	char code_day[3][32];		//���������������
	char code_night[3][64]; 	//��������������
	char high[3][32];			//�����
	char low[3][32];			//�����
	char wind_direction[3][64]; //����
	char wind_speed[3][32];  	//���٣���λkm/h����unit=cʱ��
	char wind_scale[3][32];  	//�����ȼ�
	
	char life_suggestion[128][2];	//�����
}Result_T;

/* ���ڽ����������ݽṹ���е�date��Ա */
typedef struct
{
	int date_year;
	int date_month;
	int date_day;
}date_T;

extern Result_T weather_data;
extern date_T g_date[3];
extern int g_week[3];
extern char g_page[32];

//����������,���ⲿ�ļ�����
void ESP8266_TCPClient_Test(void);
uint8 GET_NowWeather(void);
uint8 GET_DailyWeather(void);
//uint8 GET_LifeSuggestion(void);
uint8 GET_BeiJingTime(void);
void DisplayWeather(Result_T weather);
void DisplayWeatherIcon(Result_T weather);
void DisplayWifiIcon(uint8 connect_sta);
void DisplayTest(Result_T result);


//SwitchToGbk������utf8togbk.c��
extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

#endif
