#ifndef __CONFIG_H 
#define __CONFIG_H

//----------------------------------------------------------------
//�޷������ͺ궨��
#define uchar 	unsigned char													 
#define uint 	unsigned int

#define uint8 	unsigned char 
#define uint16 	unsigned short int
#define uint32 	unsigned int

//----------------------------------------------------------------
// MAIN_CONFIG��	main.c ����#define  MAIN_CONFIG
#ifdef 	 MAIN_CONFIG										// ͷ�ļ������C�ļ�����,���������ͻ������ 
	#define  EXT											// ���ļ��ж���ı���
#else
	#define  EXT	extern									// ȫ���ļ�����������
#endif
	
//----------------------------------------------------------------
//��������ͷ�ļ�  
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include "system.h"
#include "SysTick.h"
	
//---------------------------------------------------------------
//��������ͷ�ļ�
#include "usart1.h"
#include "serial_screen.h"
#include "rtc.h"
#include "dht11.h"
#include "esp8266_drive.h"
#include "http_client.h"
#include "iic.h"
#include "tea5767.h"
#include "fm_radio.h"
#include "timer.h"
#include "cJSON.h"
#include "malloc.h"
#include "LDchip.h"
#include "Reg_RW.h"
#include "LD3320_main.h"
#include "spi.h"
#include "syn6288.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "sd.h"

	
//---------------------------------------------------------------
//����ȫ�ֱ���

//---------------------------------------------------------------
//����ȫ�ֺ궨��
#define CPU_INT_DISABLE()   { __set_PRIMASK(1); }		// �ر����ж� 
#define CPU_INT_ENABLE()    { __set_PRIMASK(0);	} 		// �������ж�

//---------------------------------------------------------------
//����ȫ�����Ͷ���
typedef enum{FALSE=0,TRUE=!FALSE,}BOOL;					// �ɹ�orʧ��

////---------------------------------------------------------------
////����ȫ��ϵͳ��ʱ��
//#define  MAX_TIMER  		2							// ���ʱ������
//EXT volatile unsigned long  g_dwSysTimer[MAX_TIMER];	// ����ϵͳ��ʱ�� 
//#define  Uart2CharRecTimer		g_dwSysTimer[0]			// ����2���ݽ���
//#define  Uart3CharRecTimer  	g_dwSysTimer[1]	 		// ����3���ݽ���


#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/


