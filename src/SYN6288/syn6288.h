#ifndef __SYN6288_H
#define __SYN6288_H

#include "config.h"

#define UART4_REC_LEN  		200  	  //�����������ֽ��� 200
	  	
extern u8  UART4_RX_BUF[UART4_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 UART4_RX_STA;         	  //����״̬���	

void SYN6288_UART4_Init(u32 bound);
void TTSPlay(uint8_t Music,uint8_t *Text);

#endif

