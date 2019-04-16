/********************************************************************************************************
**
** SYN6288 ���������ϳ�ģ��
**
**
**
********************************************************************************************************/

#include "syn6288.h"


u8 UART4_RX_BUF[UART4_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 UART4_RX_STA=0;       //����״̬���	


static void UART4_SendStr(uint8_t *data, uint8_t len);


/********************************************************************************************************
** ����: SYN6288_UART4_Init,  ��������ʼ��
**------------------------------------------------------------------------------------------------------
** ����: bound:������ 
** ˵��: SYN6288ģ��Ĭ�ϲ�����9600bit/s
** ����: ��		
** ����: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void SYN6288_UART4_Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TX			   //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOC,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//RX			 //��������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOC,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //UART4 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������4
	
	USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���4 
	
	USART_ClearFlag(UART4, USART_FLAG_TC);
		
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�

	//UART4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

/*******************************************************************************************************
** ����: TTSPlay���ı��ϳ���������
**------------------------------------------------------------------------------------------------------
** ����: Music(��������ѡ��):0�ޱ������֡�1-15����ر�������   *Text:�ı�ָ����� 
** ����: ��
** ˵������1��SYN6288Ĭ�ϲ�����Ϊ9600bit/s
		��2�����ݰ���ʽ��5�ֽ�֡ͷ+�ı�+1�ֽ�У�飬�ı��ֽ���С�ڵ���200�ֽ�
** ����: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TTSPlay(uint8_t Music,uint8_t *Text)
{
	/****************��Ҫ���͵��ı�**********************************/ 
	uint8_t DataPacket[50];		//
	uint8_t Text_Len;  
	uint8_t ecc  = 0;  			//����У���ֽ�
	uint8_t i=0;  
	Text_Len =strlen((const char*)Text); 			//��Ҫ�����ı��ĳ���

	/*****************֡�̶�������Ϣ**************************************/           
	DataPacket[0] = 0xFD ; 				//����֡ͷFD
	DataPacket[1] = 0x00 ; 				//�������������ȵĸ��ֽ�
	DataPacket[2] = Text_Len + 3; 		//�������������ȵĵ��ֽ�
	DataPacket[3] = 0x01 ; 				//���������֣��ϳɲ�������		 		 
	DataPacket[4] = 0x01 | Music<<4 ;   //����������������������趨

	/*******************У�������***************************************/		 
	for(i = 0; i<5; i++)   				//���η��͹���õ�5��֡ͷ�ֽ�
	{  
		ecc=ecc^(DataPacket[i]);		//�Է��͵��ֽڽ������У��	
	}

	for(i= 0; i<Text_Len; i++)   		//���η��ʹ��ϳɵ��ı�����
	{  
		ecc=ecc^(Text[i]); 				//�Է��͵��ֽڽ������У��		
	}		 
	/*******************����֡��Ϣ***************************************/		  
	memcpy(&DataPacket[5], Text, Text_Len);
	DataPacket[5+Text_Len]=ecc;
	UART4_SendStr((char*)DataPacket,5+Text_Len+1);
} 	 

/*******************************************************************************************************
** ����: UART4_SendStr��UART4�����ַ�������
**------------------------------------------------------------------------------------------------------
** ����: data��Ҫ���͵��ַ���   len���ַ�������
** ����: ��
** ����: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void UART4_SendStr(uint8_t *data, uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART_SendData(UART4, data[i]);         //�򴮿�1��������
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}

/*******************************************************************************************************
** ����: USART3_IRQHandler��USART3�жϺ���
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2018.4.24 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void UART4_IRQHandler(void)                	//����3�жϷ������
{
	u8 r;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
	{
		r =USART_ReceiveData(UART4);//(USART3->DR);	//��ȡ���յ�������
		if((UART4_RX_STA&0x8000)==0)//����δ���
		{
			if(UART4_RX_STA&0x4000)//���յ���0x0d
			{
				if(r!=0x0a)UART4_RX_STA=0;//���մ���,���¿�ʼ
				else UART4_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(r==0x0d)UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=r ;
					UART4_RX_STA++;
					if(UART4_RX_STA>(UART4_REC_LEN-1))UART4_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		
	} 
	
}
