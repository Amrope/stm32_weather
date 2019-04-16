/*************�˿���Ϣ********************
 * ����˵��
 * P2/SDCK  PB3
 * P1/SDO   PB4
 * P0/SDI   PB5
 * RST      PB1
 * A0		PB2
 * CS   	PB8
 * WR/SPIS  PB9
 * IRQ      PF6
 * RD       PA0
*****************************************/

#include "LDchip.h"
#include "Reg_RW.h"	
#include "LD3320_main.h"

char g_WeatherText[200] = {0};


static void LD3320_EXTI_Cfg(void);
static void LD3320_Spi_cfg(void);
static void LD3320_GPIO_Cfg(void);
static void CheckReadWrite(void);

extern int SwithToUtf_8(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

/***********************************************************
* ��    �ƣ�LD3320_Init(void)
* ��    �ܣ�ģ�������˿ڳ�ʼ����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_Init(void)
{
	LD3320_GPIO_Cfg();	
	LD3320_EXTI_Cfg();
	LD3320_Spi_cfg();
//	CheckReadWrite();	//����LD3320��д�ĺ�����������
	LD_reset();
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	PrintfToHMI("ASR Init..." ,"page_Init", "InitASRTxt", "txt");
}

/*******************************************************************************************************
** ����: CheckReadWrite������ͨ���Ƿ�����
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ����: 2018.1.10 by Hezhijie and Lizhengnian 
********************************************************************************************************/
static void CheckReadWrite(void)
{
	uint8 RegValue;
	
	LD_reset();
	/* ���ͨ���Ƿ����� */
	printf("���ͨ���Ƿ�������\n");
	RegValue = LD_ReadReg(0x06);	// 0x06�Ĵ�������FIFO״̬
	LD_WriteReg(0x35,0x33);			// 0x35�Ĵ�����ADC����
	LD_WriteReg(0x1b,0x55);			// 0x1b�Ĵ�����ʱ��Ƶ������
	LD_WriteReg(0xb3,0xaa);			// 0xb3�Ĵ����������ϵ���
	RegValue = LD_ReadReg(0x35);
	printf("RegValue(0x35) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0x1b);
	printf("RegValue(0x1b) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0xb3);
	printf("RegValue(0xb3) = %#x\n",RegValue);
	LD_reset();
	/* ���Ĵ�����ֵ */
	printf("�Ĵ�����ֵΪ��\n");
	RegValue = LD_ReadReg(0x06);
	printf("RegValue(0x06) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0x35);
	printf("RegValue(0x35) = %#x\n",RegValue);
	RegValue = LD_ReadReg(0xb3);
	printf("RegValue(0xb3) = %#x\n",RegValue);
}

/***********************************************************
* ��    �ƣ� void Delay_( int i)
* ��    �ܣ� ����ʱ
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void Delay_( int i)
{     
	while( i--)
	{	

	}
}	
/***********************************************************
* ��    �ƣ�	LD3320_delay(unsigned long uldata)
* ��    �ܣ�	����ʱ����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void  LD3320_delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<5;j++)
	{
		for (g=0;g<uldata;g++)
		{
			Delay_(120);
		}
	}
}

/***********************************************************
* ��    �ƣ�	RunASR(void)
* ��    �ܣ�	����ASR
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;
	for (i=0; i<5; i++)			//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_AsrStart();			//(1)��ʼ��ASR
		LD3320_delay(100);
		if (LD_AsrAddFixed()==0)//(2)��ӹؼ����ﵽLD3320оƬ��
		{
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			LD3320_delay(50);	//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}

		LD3320_delay(10);

		if (LD_AsrRun() == 0)	//(3)����ASR����
		{
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			LD3320_delay(50);	//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}

		asrflag=1;	//�����ɹ�
		break;					//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}

	return asrflag;	//����ʧ��
}

/***********************************************************
* ��    �ƣ�LD3320_GPIO_Cfg(void)
* ��    �ܣ���ʼ����Ҫ�õ���IO��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
static void LD3320_GPIO_Cfg(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	//io������
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ,ENABLE);
	//LD_CS/A0/RSET
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_2|GPIO_Pin_1;//;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//LD_RD
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_2);	/*A0Ĭ������*/
	GPIO_SetBits(GPIOA,GPIO_Pin_0);	/*RDĬ������*/
}
/***********************************************************
* ��    �ƣ�LD3320_Spi_cfg(void)
* ��    �ܣ�����SPI���ܺͶ˿ڳ�ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
static void LD3320_Spi_cfg(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    //spi�˿�����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3 | RCC_APB2Periph_GPIOB,ENABLE);	   //ʹ��SPI3����ʱ��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

	//P0/P1/P2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//WR ��CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
	LD_CS_H();
	
	//spi��������
	SPI_Cmd(SPI3, DISABLE);
	/* SPI3 ���� */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   //ʱ�Ӽ��� ����״̬ʱ��SCK���ֵ͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //�������NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;   //�����ʿ��� SYSCLK/128
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //���ݸ�λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC����ʽ�Ĵ�����ʼֵΪ7
	SPI_Init(SPI3, &SPI_InitStructure);
	/* ʹ��SPI3 */
	SPI_Cmd(SPI3, ENABLE);

}
/***********************************************************
* ��    �ƣ� LD3320_EXTI_Cfg(void) 
* ��    �ܣ� �ⲿ�жϹ������ú���ض˿�����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
static void LD3320_EXTI_Cfg(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//�ж���������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	//�ⲿ�ж�������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line6);
		
	GPIO_SetBits(GPIOF,GPIO_Pin_6);	 //Ĭ�������ж�����

	EXTI_ClearFlag(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line6);
	//�ж�Ƕ������
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/***********************************************************
* ��    �ƣ�  EXTI9_5_IRQHandler(void)
* ��    �ܣ� �ⲿ�жϺ���
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6)!= RESET ) 
	{
 		ProcessInt0(); 	//	ʶ������
		printf("�����ж�\r\n");	
		EXTI_ClearFlag(EXTI_Line6);
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}

/*******************************************************************************************************
** ����: ASRSuccess_Handle��ʶ��ɹ���ִ����Ӧ����
**------------------------------------------------------------------------------------------------------
** ����: asr_code��ʶ����
** ����: ��
** ����: 2018.1.10 by Hezhijie and Lizhengnian 
********************************************************************************************************/
uint8 RunFlag = FALSE;
void ASRSuccess_Handle(uint8 asr_code)
{
	printf("\r\nʶ����:%d\n",asr_code);		
	if(0 == asr_code)
	{
		printf("���ڣ���Ҫ�ҵİ�����\n");
		TTSPlay(0, "[t3][2]����,[2]��Ҫ[2]�ҵ�[3]������");
		RunFlag = TRUE;
	}
	else if(RunFlag)
	{
		RunFlag = FALSE;
		/* ʶ����0-10Ϊ��������ʶ���� */
		if(asr_code>=0&&asr_code<=10)
		{
			switch(asr_code)		  
			{
				case CODE01:			
					printf("�����ݡ�����ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");
					memcpy(g_city,"fujianfuzhou",sizeof(g_place));
					break;
				case CODE02:	 
					printf("���Ϻ�������ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ�������Ϻ�����");			
					memcpy(g_city,"shanghai",sizeof(g_place));
					break;
				case CODE03:		
					printf("�����ڡ�����ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");	
					memcpy(g_city,"shenzhen",sizeof(g_place));
					break;
				case CODE04:		
					printf("������������ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");	
					memcpy(g_city,"beijing",sizeof(g_place));
					break;
				case CODE05:		
					printf("�����ݡ�����ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");	
					memcpy(g_city,"guangzhou",sizeof(g_place));
					break;
				case CODE06:		
					printf("������������ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");	
					memcpy(g_city,"nanning",sizeof(g_place));
					break;
				case CODE07:		
					printf("�����š�����ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");	
					memcpy(g_city,"xiamen",sizeof(g_place));
					break;
				case CODE08:		
					printf("��Ȫ�ݡ�����ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ������Ȫ������");	
					memcpy(g_city,"quanzhou",sizeof(g_place));
					break;
				case CODE09:		
					printf("���������ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������������");	
					memcpy(g_city,"putian",sizeof(g_place));
					break;
				case CODE10:	
					printf("����ƽ������ʶ��ɹ�\r\n");
					TTSPlay(0, "[t3][2]С������Ϊ��������ƽ����");	
					memcpy(g_city,"nanping",sizeof(g_place));
					break;
			}
			memset(&weather_data, 0, sizeof(weather_data));
			GET_NowWeather();
			GET_DailyWeather();
			GetWeatherTimer = TIMER1_HOUR;	
			DisplayWeather(weather_data);
			DisplayWeatherIcon(weather_data);
		}
		else
		{
			switch(asr_code)	
			{
				case CODE11:
					printf("��������������������ʶ��ɹ�\r\n");
					printf("%s\n",g_WeatherText);
//					TTSPlay(0, (uint8_t*)g_WeatherText);
					break;
				case CODE12:
					printf("������������Ƕ��١�����ʶ��ɹ�\r\n");
					break;
				default:
					TTSPlay(0, "����ʶ��ʧ�ܣ����׼��˷�˵����");
					break;
			}
		}
		
	}
}