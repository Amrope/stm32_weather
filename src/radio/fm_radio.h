#ifndef __FM_RADIO_H
#define __FM_RADIO_H

#include "config.h"

#define FM_SAVE_COUNT	32			/* ��ౣ�漸����̨ */

typedef struct
{
	uint8_t ucSearch;			/* 1 ��ʾ���������� */
	uint8_t ucSpkEn;			/* 0 : ��������� 1: ��������� */
	uint8_t ucMuteOn;			/* 0 : ������ 1: ���� */
	uint8_t ucAutoMode;			/* 0 : ���Ҽ������޸�Ƶ�� 1: ���Ҽ������Զ�������һ����̨ */
	uint8_t ucVolume;			/* ��ǰ���� */
	uint8_t ucAdcLevelSet;		/* �Զ�����ֹͣ��ADC��ƽ */
	uint8_t ucAdcLevelNow;		/* ��ǰƵ����ADC��ƽ */
	uint32_t ulFreq;			/* ��ǰƵ�� */	
	uint8_t ucDirection;		/* �Զ��������� */
	uint32_t ulSaveFreq[FM_SAVE_COUNT];	/* �����Ƶ���б� */
	uint8_t ucSaveCount;			/* �����Ƶ����� */
	uint8_t ucSavePos;
}RADIO_T;
extern RADIO_T g_tRadio;

void RadioInit(void);

#endif
