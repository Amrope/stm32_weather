// Reg_RW.h  ��дLD3320оƬ�ļĴ������������ַ�����
#ifndef REG_RW_H
#define REG_RW_H

#include "config.h"


//#define LD_RST_H() GPIO_SetBits(GPIOB, GPIO_Pin_6)
//#define LD_RST_L() GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define LD_RST_H() GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define LD_RST_L() GPIO_ResetBits(GPIOB, GPIO_Pin_1)

#define LD_CS_H()	GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define LD_CS_L()	GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#define LD_SPIS_H()  GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define LD_SPIS_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_9)



/****************************************************************
�������� LD_WriteReg
���ܣ�дLD3320оƬ�ļĴ���
������  address, 8λ�޷�����������ַ
		dataout��8λ�޷���������Ҫд�������
����ֵ����
****************************************************************/ 
void LD_WriteReg( unsigned char address, unsigned char dataout );

/****************************************************************
�������� LD_ReadReg
���ܣ���LD3320оƬ�ļĴ���
������  address, 8λ�޷�����������ַ
����ֵ��8λ�޷�����������ȡ�Ľ��
****************************************************************/ 
unsigned char LD_ReadReg( unsigned char address );



#endif
