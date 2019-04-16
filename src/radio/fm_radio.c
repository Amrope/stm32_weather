/*
*********************************************************************************************************
*
*	ģ������ : FM������������ģ�顣
*	�ļ����� : fm_radio.c
*	��    �� : V2.0
*	˵    �� : ��ģ��ʵ�ֵ�Ƶ���������ܡ�
*
*		���������������õ�FM������оƬΪTEA5767����оƬ����I2C�����ϡ�
*		I2C����ͨ��ģ��IO��ʽʵ�֡�TEA5767����Ƶ������ӵ�CODECоƬWM8978��LINE����ˣ����
*		��Ҫͬʱ����WM8978���ܴӶ������߰��������������Ƶ�źš�
*
*		������ʾ��
*			[TAMPER]��     = ������/�����л�
*			[WAKEUP]��     = ɨ���̨
*			[USER]��       = ������/�����ر�
*			ҡ����/�¼�    = ��������
*			ҡ����/�Ҽ�    = ������̨/������̨
*			ҡ��OK��       = �Զ���̨/�ֶ���̨
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-08-27 armfly  ST�̼���V3.5.0�汾��
*		v2.0    2011-10-16 armfly  �Ż����̽ṹ��ȡ��TEA5767��Ӳ���Զ��������ܣ�������������жϡ�
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "fm_radio.h"

#define LINE_CAP	18		/* �����м�� */

#define STR_Title	"FM��Ƶ������(TEA5767 + WM8978)"
#define STR_Help1	"[TAMPER]��     = ������/����"
#define STR_Help3	"[USER]��       = ������/�����ر�"
#define STR_Help4	"ҡ����/�¼�    = ��������"
#define STR_Help5	"ҡ����/�Ҽ�    = ������̨/������̨"
#define STR_Help6	"ҡ��OK��       = �Զ���̨/�ֶ���̨"

#define STR_HelpScan "����϶���, Ȼ��WAKEUP���Զ�������̨"

RADIO_T g_tRadio;
TEA5767_T g_tTEA;

static void DispStatus(void);
static void AutoScan(void);
static void DispSaveFreq(void);

/*
*********************************************************************************************************
*	�� �� ��: RadioInit
*	����˵��: ��������ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RadioInit(void)
{
	if (i2c_CheckDevice(TEA5767_SLAVE_ADDRESS) != 0)
	{
		printf("Don't Find TEA5767\n");
	}

	/* ��ʼ��ȫ�ֱ��� */
	g_tRadio.ucSpkEn = 0;		/* ��������� */
	g_tRadio.ucMuteOn = 0;		/* �����ر� */
	g_tRadio.ucAutoMode = 1;	/* �Զ���̨ģʽʹ�� */
	g_tRadio.ucVolume = 30;		/* ȱʡ���� */
	g_tRadio.ulFreq = 100100000;	/* ����/ŷ��(87.5-108M), �ձ�(76-91MHz) */
	g_tRadio.ucAdcLevelSet = ADC_LEVEL_7;
	tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, ADC_LEVEL_7);
	PrintfToHMI("Radio Init..." ,"page_Init", "InitRadioTxt", "txt");
}

/*
*********************************************************************************************************
*	�� �� ��: AutoScan
*	����˵��: �Զ�ɨ���̨���յ��ĵ�̨���浽ȫ�ֱ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
/*
	�人�����ĳ���������ĵ�̨�� 15��
	89.6	90.7	91.2	92.7	93.6	95.6	96.0	97.8
	99.8	101.8	102.6	103.8	104.6	105.8	107.8
*/
static void AutoScan(void)
{
	uint8_t ucLevelBuf[FREQ_COUNT];	/* �洢ÿ��Ƶ���ADCֵ��ÿ��20msɨ��һ�� */
	uint8_t ucPos = 0;
	uint16_t i;
	uint8_t ucFirst, ucSecond;
	uint8_t k;
	char buf[100];

	memset(buf, ' ', 400 / 8);
	buf[400 / 8] = 0;

	printf("����������̨...            \n");

   	g_tRadio.ulFreq	=  FM_FREQ_MIN;

	for (i = 0; i < FREQ_COUNT; i++)
	{
		tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, g_tRadio.ucAdcLevelSet);

		delay_ms(30);
		for (k = 0; k < 3; k++)
		{
			delay_ms(5);
			tea5767_ReadStatus(&g_tTEA);	/* ��ȡTEA5767��״̬ */
			ucFirst = g_tTEA.ucAdcLevel; 

			if (ucFirst < 6)
			{
				break;
			}

			delay_ms(5);
			ucSecond = g_tTEA.ucAdcLevel;
			
			if (ucFirst == ucSecond)
			{
				break;
			} 
		}
		ucLevelBuf[i] = ucSecond;
		
		g_tRadio.ulFreq += FREQ_STEP;
	}

	/* ��ౣ��32����̨ */
	for (i = 0; i < FREQ_COUNT; i++)
	{
		if ((i == 0) || (i == FREQ_COUNT - 1))
		{
			/* 2ͷ2��Ƶ�㶪��*/;			
		}
		else
		{
			if ((ucLevelBuf[i] > 5) &&
				(ucLevelBuf[i] > ucLevelBuf[i - 1]) && (ucLevelBuf[i] > ucLevelBuf[i + 1]))
			{
				g_tRadio.ulSaveFreq[ucPos++] = FM_FREQ_MIN + i * FREQ_STEP;
			}
		}
		if (ucPos >= FM_SAVE_COUNT)
		{
			break;
		}
	}
	g_tRadio.ucSaveCount = ucPos; 	/* ��̨���� */

   	/* ȱʡ���ŵ�һ����̨ */
	g_tRadio.ucSavePos = 0;
	g_tRadio.ulFreq = g_tRadio.ulSaveFreq[g_tRadio.ucSavePos]; 	
	tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, g_tRadio.ucAdcLevelSet);
}

/*
*********************************************************************************************************
*	�� �� ��: DispSaveFreq
*	����˵��: ��ʾ�Զ��������ĵ�̨Ƶ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/							
static void DispSaveFreq(void)
{
	char buf[100];
	uint16_t pos;
	uint8_t i;

	memset(buf, ' ', 400 / 8);
	buf[400 / 8] = 0;

	/* ��ʾ�������ĵ�̨���� */
	sprintf(buf, "���������ĵ�̨����: %d  ",g_tRadio.ucSaveCount);

	pos = 0;
	memset(buf, 0, sizeof(buf));
   	for (i = 0; i < 8; i++)
	{
		sprintf(&buf[pos], "%3d.%d ",
			g_tRadio.ulSaveFreq[i] / 1000000, (g_tRadio.ulSaveFreq[i] % 1000000) / 100000);

		pos = strlen(buf);
	}

	pos = 0;
	memset(buf, 0, sizeof(buf));
   	for (i = 8; i < 16; i++)
	{
		if (i == g_tRadio.ucSaveCount)
		{
			break;
		}
		sprintf(&buf[pos], "%d.%d ",
			g_tRadio.ulSaveFreq[i] / 1000000, (g_tRadio.ulSaveFreq[i] % 1000000) / 100000);

		pos = strlen(buf);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DispStatus
*	����˵��: ��ʾ��ǰ״̬��Ƶ�ʣ�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispStatus(void)
{
	uint16_t y, x1, x2;
	char buf[80];
	

	x1 = 20;	/* ״̬����1��X���� */
	x2 = 200;	/* ״̬����2��X���� */
	y = LINE_CAP * 11;

	if (g_tRadio.ucSearch == 1)
	{
		if (g_tRadio.ucDirection == SEARCH_UP)
		{
			sprintf((char *)buf, "Ƶ�� = >>>>>>>>    ");
		}
		else
		{
			sprintf((char *)buf, "Ƶ�� = <<<<<<<<    ");
		}
	}
	else
	{
		#if 1
		sprintf((char *)buf, "Ƶ�� = %d.%d�׺�  ",
			g_tRadio.ulFreq / 1000000, (g_tRadio.ulFreq % 1000000) / 100000);
		#else
		sprintf((char *)buf, "Ƶ�� = %dHz    ",	g_tRadio.ulFreq);
		#endif
	}

	sprintf((char *)buf, "���� = %d ", g_tRadio.ucVolume);

	y += LINE_CAP;

	if (g_tRadio.ucAutoMode == 1)
	{
		printf("��̨ = �Զ�\n");
	}
	else
	{
		printf("��̨ = �ֶ�\n");
	}

	sprintf((char *)buf, "�ź�ǿ��  = %d ",g_tTEA.ucAdcLevel);
}
