#ifndef __SERIAL_SCREEN_H
#define __SERIAL_SCREEN_H 

#include "config.h"

#define USART3_REC_LEN  			200  	//�����������ֽ��� 200


extern char g_place[32];
extern char g_place_utf8[32];
extern char g_city[32];
extern int utf8_len;
extern char g_wifi_ssid[32];
extern char g_wifi_password[32];
extern char g_freq_int[32];
extern char g_freq_deci[32];
extern char g_serch_method[32];
extern char g_high_suggestion[64];
extern char g_low_suggestion[64];

extern u8  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;         		//����״̬���	

void HMI_USART3_init(u32 bound);
void HMISendByte(u8 k);
void HMISendStr(char *buf);
void PrintfToHMI(char *str, char *page, char *objname, char *property);

#endif
