#include "esp8266_drive.h"


//���ڽ��ջ����� 	
uint8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 			//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
uint8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
uint16 USART2_RX_STA=0; 

//�ڲ���������
static void USART2_Init(uint32 bound);
static uint8 *ESP8266_CheckAck(uint8 *ack);


/********************************************************************************************************
** ����: ESP8266_Init,  ESP8266��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
void ESP8266_USART2_Init(uint32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(ESP8266_RST_Pin_Periph_Clock|ESP8266_CH_PD_Pin_Periph_Clock, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_Pin;			   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
	GPIO_Init(ESP8266_RST_Pin_Port, &GPIO_InitStructure);

   	GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_Pin;			   
	GPIO_Init(ESP8266_CH_PD_Pin_Port, &GPIO_InitStructure);
	
	USART2_Init(bound);			//����2��ʼ��
	
	ESP8266_RST_Pin_SetH;
	ESP8266_CH_PD_Pin_SetL;	    //ʹ�����ţ���ʼ��Ϊ�͵�ƽ������ATʱ����Ϊ��
	
	ESP8266_Config();			//ATָ������
	PrintfToHMI("WiFi Init..." ,"page_Init", "InitWiFiTxt", "txt");
}

/********************************************************************************************************
** ����: USART2_Init,  ����3��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
static void USART2_Init(uint32 bound)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2  TXD
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
   

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3  RXD
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA3  
  
	//Usart2 NVIC ����
  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

   	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  	USART_Init(USART2, &USART_InitStructure); //��ʼ������2

  	USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_IDLE, ENABLE);//�������ڽ��ܺ����߿����ж�
  	
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 	
	
	TIM2_Init(1000-1, 72-1);
	USART2_RX_STA=0;		//����
}

/********************************************************************************************************
** ����: ESP8266_CheckAck, ESP8266���������,�����յ���Ӧ��
**------------------------------------------------------------------------------------------------------
** ����: ack���ڴ���Ӧ����
**		  
** ����: 0��ʧ�� ������	�ڴ�Ӧ������λ��(ack��λ��)										  
********************************************************************************************************/
static uint8 *ESP8266_CheckAck(uint8 *ack)
{
	char *res_ack = NULL;
	
	if(USART2_RX_STA&0x8000)  //���յ��س������������ݽ���
	{
		USART2_RX_BUF[USART2_RX_STA&0x7fff] = 0;
		res_ack = strstr((const char*)USART2_RX_BUF, (const char*)ack);
	}
	
	return (uint8*)res_ack;   //�ɹ�
}

/********************************************************************************************************
** ����: ESP8266_Printf, ���ô�ӡ��ʽ��������
**------------------------------------------------------------------------------------------------------
** ����: fmt, ...
**		  
** ����: void 								  
********************************************************************************************************/
void ESP8266_Printf(char *fmt,...)
{
	uint16 i,len;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF, fmt, ap);
	va_end(ap);
	len = strlen((const char*)USART2_TX_BUF);//�˴η������ݵĳ���
	for(i=0;i<len;i++)						 //ѭ����������
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2, USART2_TX_BUF[i]);
	}
}

/********************************************************************************************************
** ����: ESP8266_SendATCmd, ��ESP8266ģ�鷢��ATָ��
**------------------------------------------------------------------------------------------------------
** ����: cmd�������͵�ָ�� ack���ڴ�����Ӧ��ΪNULL������Ӧ
**		  wait_time���ȴ���Ӧ��ʱ�䣨��λ10ms��
** ����: 0�����ͳɹ� 1��ʧ��											  
********************************************************************************************************/
uint8 ESP8266_SendATCmd(uint8 *cmd, uint8 *ack, uint16 wait_time)
{
	uint8 res = 0;
	USART2_RX_STA = 0;
	
	ESP8266_Printf("%s\r\n", cmd);
	if(ack&&wait_time)
	{
		while(--wait_time)
		{
			delay_ms(10);
			if(USART2_RX_STA&0x8000) //���յ��س������������ݽ���
			{
				if(ESP8266_CheckAck(ack))
				{
					printf("%s ack=%s\n",cmd, ack);
					USART2_RX_STA = 0;
					break; //�õ���Ч���ݣ��˳�whileѭ��
				}
				USART2_RX_STA = 0;
			}
		}
		if(wait_time==0)  //��ʱ
		{
			res = 1;	  //ʧ��
		}
	}
	
	return res;
}

/********************************************************************************************************
** ����: ESP8266_ExitTran,  �˳�͸��ģʽ
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: 0:�˳�͸���ɹ�, 1:ʧ��											  
********************************************************************************************************/
uint8 ESP8266_ExitTran(void)
{
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	return ESP8266_SendATCmd("AT","OK",20);//�˳�͸���ж�.
}

/********************************************************************************************************
** ����: ESP8266_Config,  ESP8266ģ��ĳ�������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��										  
********************************************************************************************************/
void ESP8266_Config(void)
{
	printf("ESP8266 WIFI Init...\n"); 
	ESP8266_CH_PD_Pin_SetH;   //ʹ�����ţ��ߵ�ƽ��Ч
	while(ESP8266_SendATCmd("AT","OK",20))	//���WIFIģ���Ƿ�����
	{
		ESP8266_ExitTran();//�˳�͸��
		ESP8266_SendATCmd("AT+CIPMODE=0","OK",200);  //�ر�͸��ģʽ	
		delay_ms(800);
	} 
	printf("ESP8266 WIFI Linking...\n"); 
	while(ESP8266_SendATCmd("ATE0","OK",20));	//�رջ���
	ESP8266_SendATCmd("AT+CWMODE=3","OK",50);	//����WIFI 1ΪSTAģʽ,2ΪAPģʽ,3ΪSTA��APģʽ����
	ESP8266_SendATCmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	ESP8266_SendATCmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
}

/********************************************************************************************************
** ����: ESP8266_ConnectAP,  ESP8266ģ������·��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: 0:����, 1:δ����									  
********************************************************************************************************/
uint8 ESP8266_ConnectAP(char *ssid, char *password)
{
	char cmd[100];
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
	return ESP8266_SendATCmd((char*)cmd, "WIFI GOT IP", 300);
}

/********************************************************************************************************
** ����: ESP8266_LinkServer, ����Զ�̷�����
**------------------------------------------------------------------------------------------------------
** ����: enumN:����Э�� ip:��������ip   port:�˿�
** ����: 0�����ӳɹ� 1������ʧ��						  
********************************************************************************************************/
uint8 ESP8266_LinkServer(ENUM_NetPro_TypeDef enumN, uint8 *ip, uint8 *port)
{
	uint8 ATcmd[120];
	switch( enumN )
	{
		case enumTCP:
			sprintf((char*)ATcmd, "AT+CIPSTART=\"%s\",\"%s\",%s", "TCP", ip, port);
			
			break;
		case enumUDP:
			sprintf((char*)ATcmd, "AT+CIPSTART=\"%s\",\"%s\",%s", "UDP", ip, port);
			break;
	}
	return ESP8266_SendATCmd(ATcmd, "OK", 200);
}

/********************************************************************************************************
** ����: USART2_IRQHandler,  ����2�жϷ�����
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��											  
********************************************************************************************************/
void USART2_IRQHandler( void )
{	
	u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	
		Uart2CharRecTimer = 10;
		res =USART_ReceiveData(USART2);		 
		if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//�����Խ�������
			{
				USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ	 
			}
		}
	}  				 						
}

