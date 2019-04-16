#ifndef _esp8266_drive_H
#define _esp8266_drive_H

#include "config.h"


//#define WIFI_SSID	  	  "HiWiFi_cekong"		//Ҫ�������ȵ������
//#define WIFI_PASSWORD  	  "cekong2015"      //Ҫ�������ȵ����Կ
//#define WIFI_SSID	  	  "MYTEST"		  //Ҫ�������ȵ������
//#define WIFI_PASSWORD  	  "pppp1234"      //Ҫ�������ȵ����Կ

#define USART2_MAX_RECV_LEN		1500				//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		600					//����ͻ����ֽ���

extern uint8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 	//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern uint8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern uint16 USART2_RX_STA;   						//��������״̬

#define ESP8266_RST_Pin          	GPIO_Pin_6		//��λ����
#define ESP8266_RST_Pin_Port     	GPIOE
#define ESP8266_RST_Pin_Periph_Clock  RCC_APB2Periph_GPIOE

#define ESP8266_CH_PD_Pin     		GPIO_Pin_7		//ʹ������,�ߵ�ƽ��Ч
#define ESP8266_CH_PD_Pin_Port      GPIOG
#define ESP8266_CH_PD_Pin_Periph_Clock  RCC_APB2Periph_GPIOG

#define ESP8266_RST_Pin_SetH     GPIO_SetBits(ESP8266_RST_Pin_Port,ESP8266_RST_Pin)
#define ESP8266_RST_Pin_SetL     GPIO_ResetBits(ESP8266_RST_Pin_Port,ESP8266_RST_Pin)

#define ESP8266_CH_PD_Pin_SetH     GPIO_SetBits(ESP8266_CH_PD_Pin_Port,ESP8266_CH_PD_Pin)
#define ESP8266_CH_PD_Pin_SetL     GPIO_ResetBits(ESP8266_CH_PD_Pin_Port,ESP8266_CH_PD_Pin)

//����Э�����Ͷ���
typedef enum
{
	enumTCP,
	enumUDP
}ENUM_NetPro_TypeDef;	

//����������
void ESP8266_USART2_Init(uint32 bound);
void ESP8266_Printf(char *fmt,...);
uint8 ESP8266_SendATCmd(uint8 *cmd, uint8 *ack, uint16 wait_time);
uint8 ESP8266_ExitTran(void);
uint8 ESP8266_LinkServer(ENUM_NetPro_TypeDef enumN,uint8 *ip, uint8 *port);
void ESP8266_Config(void);
uint8 ESP8266_ConnectAP(char *ssid, char *password);

#endif
