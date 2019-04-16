#include "usart1.h"
	 
/********************************************************************************************************
** ����: fput,  �ض���,��ʹ��printf����ʱ�Զ�����
**------------------------------------------------------------------------------------------------------
** ����: int ch, FILE *p
** ����: ch											  
********************************************************************************************************/
int fputc(int ch, FILE *p)
{
	USART_SendData(USART1,(uint8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/********************************************************************************************************
** ����: USART1_Init,  ����1��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
void USART1_Init(uint32 bound)
{
	/* ������ؽṹ����� */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* ���ʱ��ʹ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;		       // �������PA9(TX)
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;    // IO��Ƶ��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	   // �����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  			   // ��ʼ��
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;		   // ��������PA10(RX)
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;// ��������
	GPIO_Init(GPIOA,&GPIO_InitStructure); 			   // ��ʼ�� 
	
	/* USART1 ��ʼ������ */
	USART_InitStructure.USART_BaudRate   = bound;				// ������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8λ���ݳ��� 
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    // 1λֹͣλ
	USART_InitStructure.USART_Parity	 = USART_Parity_No;		// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ����
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);// ��ʼ��
	
	/* ʹ�ܴ���1 */
	USART_Cmd(USART1, ENABLE);
	
	/* ������ͱ�־λ */
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	/* ���������ж�ʹ�� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* NVIC���� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	   // ����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;// ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;	   // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		   // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);						   // ��ʼ��
	PrintfToHMI("USART1 Init..." ,"page_Init", "InitUsart1Txt", "txt");
}

/********************************************************************************************************
** ����: USART1_IRQHandler,  ����1�жϷ�����
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
void USART1_IRQHandler(void)
{
	uint8 rec;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		rec = USART_ReceiveData(USART1);
		USART_SendData(USART1, rec);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
	}
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
