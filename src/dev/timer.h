#ifndef _TIMER_H
#define _TIMER_H

#include "config.h"

#define  MAX_TIMER  		5						// ���ʱ������
EXT volatile unsigned long  g_Timer1[MAX_TIMER]; 
#define  GetWeatherTimer	g_Timer1[0]				// ��ʱGET�������ݰ�
#define  SendWeatherTimer  	g_Timer1[1]	 			// ��ʱ��������������������
#define  GetHMIDataTimer  	g_Timer1[2]	 			// ��ʱ��ȡ�������·�������
#define  TimeCalibraTimer  	g_Timer1[3]	 			// ��ʱУ׼ʱ��
#define  DHT11CollectTimer  g_Timer1[4]	 			// �ɼ���ʪ������

#define  TIMER1_SEC  	(1)							// ��
#define  TIMER1_MIN  	(TIMER1_SEC*60)				// ��
#define  TIMER1_HOUR  	(TIMER1_MIN*60)			 	// ʱ
#define  TIMER1_DAY  	(TIMER1_HOUR*24)			// ��

extern uint16 Uart2CharRecTimer;

void TIM1_Init(uint16 arr,uint16 psc);
void TIM2_Init(uint16 arr,uint16 psc);
void TIM3_Init(uint16 arr,uint16 psc);

#endif
