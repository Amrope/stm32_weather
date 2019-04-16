#include "serial_screen.h"

//���ڽ��ջ����� 
u8 USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART3_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	

//����һЩȫ�ֱ��������ڽ��մ������·�������
char g_place[32] = {0};
char g_place_utf8[32] = {0};
char g_city[32] = {0};
int utf8_len = 0;
char g_wifi_ssid[32] = "fjnu";
char g_wifi_password[32] = "happy110th";
char g_freq_int[32] = {0};
char g_freq_deci[32] = {0};
char g_serch_method[32] = "English";
char g_high_suggestion[64] = "�������ȣ�����ע�����ˮ��ע����";
char g_low_suggestion[64] = "��������������ע���������������ů��";

static void HMISendStart(void);

/********************************************************************************************************
** ����: HMI_USART3_init,  ��������ʼ��
**------------------------------------------------------------------------------------------------------
** ����: bound:������ 
** ˵��:  ������Ĭ�ϲ�����9600
** ����: ��											  
********************************************************************************************************/
void HMI_USART3_init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB 11  
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	// ѡ���3����ж����ȼ�
	//Usart3 NVIC ����
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

   	//USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  	USART_Init(USART3, &USART_InitStructure); //��ʼ������3

  	USART_ITConfig(USART3, USART_IT_RXNE|USART_IT_IDLE, ENABLE);//�������ڽ��ܺ����߿����ж�
  	
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 	
	
	HMISendStart();		//ȷ������HMI����ͨ��
	memset(Init_buf, 0, sizeof(Init_buf));
	HMISendStr("page page_Init");
	HMISendByte(0xff);
	PrintfToHMI("HMI Init..." ,"page_Init", "InitHMITxt", "txt");
}

/*******************************************************************************************************
** ����: HMISendByte�������ڷ���һ��3���ֽ����ݣ���������ʹ�ã�
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void HMISendByte(u8 byte)
{
	u8 i;
	
	for(i=0;i<3;i++)
	{
		if(byte!=0)
		{
			USART_SendData(USART3, byte);                                 //��������
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){} //�ȴ����ͽ���   
		}
		else
			return;
	}
}

/*******************************************************************************************************
** ����: HMISendStart��Ϊȷ������HMI����ͨ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void HMISendStart(void)
{
	delay_ms(200);
	HMISendByte(0xff);
	delay_ms(200);
}

/*******************************************************************************************************
** ����: HMISendStr���򴮿��������ַ��� 
**------------------------------------------------------------------------------------------------------
** ����: buf
** ����: ��
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void HMISendStr(char *buf)
{
	u8 i=0;
	
	while(1)
	{
		if(buf[i]!=0)
		{	
			USART_SendData(USART3, buf[i]);   
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){} 
			i++;
		}
		else
			return;
	}
}

/*******************************************************************************************************
** ����: PrintfToHMI����ӡ��ʼ����ʾ��Ϣ��������
**------------------------------------------------------------------------------------------------------
** ����: str:�ַ���  page��ҳ��  objname���ؼ�  property������
** ����: ��
** ����: 2018.1.11 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void PrintfToHMI(char *str, char *page, char *objname, char *property)
{
	memset(Init_buf, 0, sizeof(Init_buf));
	sprintf(Init_buf, "%s.%s.%s=\"%s\"", page, objname, property, str);	
	HMISendStr(Init_buf);
	HMISendByte(0xff);
}

/*******************************************************************************************************
** ����: USART3_IRQHandler��USART3�жϺ���
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2017.12.8 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void USART3_IRQHandler(void)                	
{
	u8 rec;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		rec = USART_ReceiveData(USART3);//(USART3->DR)����ȡ���յ�������
		if((USART3_RX_STA&0x8000)==0)//����δ���
		{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
			{
				if(rec!=0x0a)USART3_RX_STA = 0;  //���մ���,���¿�ʼ
				else USART3_RX_STA |= 0x8000;	 //��������� 
			}
			else //��û�յ�0X0d
			{	
				if(rec==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF] = rec;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		
	} 
} 	 
