#include "LDchip.h"

uint8 nLD_Mode = LD_MODE_IDLE;		//	������¼��ǰ���ڽ���ASRʶ�����ڲ���MP3

uint8 ucRegVal;
u8 ucHighInt;
u8 ucLowInt;
u8 bMp3Play=0;							//	������¼����MP3��״̬
u32 nMp3Size=0;
u32 nMp3Pos=0;
static FIL fsrc;
extern uint8  nAsrStatus;


/***********************************************************
* ��    �ƣ�void LD_reset(void)
* ��    �ܣ�LDоƬӲ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_reset(void)
{
	LD_RST_H();
	LD3320_delay(100);
	LD_RST_L();
	LD3320_delay(100);
	LD_RST_H();
	LD3320_delay(100);
	LD_CS_L();
	LD3320_delay(100);
	LD_CS_H();		
	LD3320_delay(100);
}

/***********************************************************
* ��    �ƣ� void LD_Init_Common(void)
* ��    �ܣ� ��ʼ������
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_Init_Common(void)
{

	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	LD3320_delay(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);   
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c); 
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

/***********************************************************
* ��    �ƣ�void LD_Init_ASR(void)
* ��    �ܣ���ʼ��ASR
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 	
void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	LD3320_delay( 5 );
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	LD3320_delay( 1 );
}
#if 0
//==================================================
//���ų�ʼ��
void LD_Init_MP3(void)	
{
	nLD_Mode = LD_MODE_MP3;	   //��ǰ����MP3����
	LD_Init_Common();		  		 //ͨ�ó�ʼ��

	LD_WriteReg(0xBD,0x02);	 		 	//�ڲ�������� ��ʼ��ʱд��FFH
	LD_WriteReg(0x17, 0x48);				//д48H���Լ���DSP
	LD3320_delay(10);
	LD_WriteReg(0x85, 0x52); 	//�ڲ��������� ��ʼ��ʱд��52H
	LD_WriteReg(0x8F, 0x00);  	//LineOut(��·���)ѡ�� ��ʼ��ʱд��00H
	LD_WriteReg(0x81, 0x00);		//���������� ����Ϊ00HΪ�������
	LD_WriteReg(0x83, 0x00);		//���������� ����Ϊ00HΪ�������
	LD_WriteReg(0x8E, 0xff);		//�����������  ���Ĵ�������Ϊ00HΪ�������	�˴������ر�
	LD_WriteReg(0x8D, 0xff);		//�ڲ�������� ��ʼ��ʱд��FFH
    LD3320_delay(1);
	LD_WriteReg(0x87, 0xff);	   //ģ���·���� MP3���ų�ʼ��ʱд FFH
	LD_WriteReg(0x89, 0xff);    //ģ���·���� MP3����ʱд FFH
	LD3320_delay(1);
	LD_WriteReg(0x22, 0x00);    //FIFO_DATA���޵�8λ
	LD_WriteReg(0x23, 0x00);		 //FIFO_DATA���޸�8λ
	LD_WriteReg(0x20, 0xef);    //FIFO_DATA���޵�8λ
	LD_WriteReg(0x21, 0x07);		 //FIFO_DATA���޸�8λ
	LD_WriteReg(0x24, 0x77);        
	LD_WriteReg(0x25, 0x03);	
	LD_WriteReg(0x26, 0xbb);    
	LD_WriteReg(0x27, 0x01); 	
}

//======================================================
//����fifo�Ӷ�����fifo�ж�,Ϊ����mp3��׼��
void fill_the_fifo(void)
{
  u8 ucStatus;
	int i = 0;
	ucStatus = LD_ReadReg(0x06);
	//fifo�Ƿ�����
	while ( !(ucStatus&MASK_FIFO_STATUS_AFULL))
	{

		LD_WriteReg(0x01,0xff);
		i++;
		ucStatus = LD_ReadReg(0x06);
	}
}
//==================================================
//���ź���
void LD_play()	  
{
	nMp3Pos=0;
	bMp3Play=1;

	if (nMp3Pos >=  nMp3Size)	  //���������ѷ������
		return ; 

	fill_the_fifo();
	LD_WriteReg(0xBA, 0x00);	  //�жϸ�����Ϣ����������Ϊ00
	LD_WriteReg(0x17, 0x48);	  //д48H���Լ���DSP
	LD_WriteReg(0x33, 0x01);	  //MP3���������� ��ʼ����ʱд��01H, ������д��00H
	LD_WriteReg(0x29, 0x04);	  //�ж����� ��2λ��FIFO �ж����� ��4λ��ͬ���ж����� 1����0������
	
	LD_WriteReg(0x02, 0x01); 	  //FIFO�ж����� ��0λ������FIFO_DATA�жϣ� ��2λ������FIFO_EXT�жϣ�
	LD_WriteReg(0x85, 0x5A);	  //�ڲ��������� ��ʼ��ʱд��52H ����MP3ʱд��5AH (�ı��ڲ�����)

	//EX0=1;		//���ⲿ�ж�0�ж�
//	EXTI_ClearITPendingBit(EXTI_Line6);
}

//======================================================
//��������
void LD_AdjustMIX2SPVolume(u8 val)	  
{
	u8 volume_val;
	
	volume_val = ((15-val)&0x0f) << 2;	
	LD_WriteReg(0x8E, val | 0xc3); 
	LD_WriteReg(0x87, 0x78); 
}

//============================================================
//����mp3 //����
char PlayDemoSound_mp3(char *filename,u8 Volume)
{	
	memset(&fsrc,0,sizeof(FIL));
	while(!(f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ) == FR_OK));
	bMp3Play = 1;
	nMp3Size = fsrc.fsize-1;
	LD_Init_MP3();									//��LD3320��ʼ��Ϊ����MP3ģʽ
	LD_AdjustMIX2SPVolume(Volume);	//���ö�������  
	LD_play();											//��ʼ����
	return 0;
}

/*  ������ȡmp3�ļ����ݵ�fifo,ֱ��fifo��
 *	��дmp3�ļ����ݵ�fifoʱ,LD3320�����벥��
 *	��Ȼдmp3�ļ����ݵ�fifo��ʱ���̹�������ʱ��
 *	�������첥����ϵ�ʱ������ProcessInt0����
 *	ProcessInt0�����ֻ���ô˺���,����������������
 */
void LD_ReloadMp3Data_Again(void)
{
	u8 val;
	u8 ucStatus;
	UINT br;
	
	ucStatus = LD_ReadReg(0x06);
	//fifo�Ƿ�����
	while (!(ucStatus&MASK_FIFO_STATUS_AFULL) && nMp3Pos<=nMp3Size)
	{

		nMp3Pos++;
		f_read(&fsrc,&val,1,&br);
		LD_WriteReg(0x01,val);

		ucStatus = LD_ReadReg(0x06);
	}

	if(nMp3Pos>=nMp3Size)
	{
	    LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);

		//�ȴ�MP3�������
		while(!(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END));

		LD_WriteReg(0x2B,  0);
      	LD_WriteReg(0xBA, 0);	
		LD_WriteReg(0xBC,0x0);	
		bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
		LD_WriteReg(0x08,1);
		LD_WriteReg(0x08,0);
		LD_WriteReg(0x33, 0);

		f_close(&fsrc);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
	}
		
}

#endif
/***********************************************************
* ��    �ƣ�void ProcessInt0(void)
* ��    �ܣ�ʶ������
* ��ڲ�����  
* ���ڲ�����
* ˵    �����������ò�ѯ��ʽ�����жϷ�ʽ���е���
* ���÷����� 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);//�ж�������

//	if(nLD_Mode == LD_MODE_ASR_RUN)		//��ǰ��������ʶ��
//	{
		// ����ʶ��������ж�
		// �����������룬����ʶ��ɹ���ʧ�ܶ����жϣ�
		LD_WriteReg(0x29,0) ;//�ж�����д0��ʾ������
		LD_WriteReg(0x02,0) ;//FIFO�ж�����

		if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
		{	 
			nAsrResCount = LD_ReadReg(0xba);

			if(nAsrResCount>0 && nAsrResCount<=4) 
			{
				nAsrStatus=LD_ASR_FOUNDOK; 				
			}
			else
			{
				nAsrStatus=LD_ASR_FOUNDZERO;
			}	
		}
		else
		{
			nAsrStatus=LD_ASR_FOUNDZERO;	//ִ��û��ʶ��
		}

		LD_WriteReg(0x2b, 0);
		LD_WriteReg(0x1C,0);/*д0:ADC������*/
//		return;
//		
//	}
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
	LD_WriteReg(0x2B,  0);
	LD_WriteReg(0xBA, 0);	
	LD_WriteReg(0xBC,0);	
	LD_WriteReg(0x08,1);	 /*���FIFO_DATA*/
	LD_WriteReg(0x08,0);	/*���FIFO_DATA�� �ٴ�д0*/

	//LD3320_delay(1);
#if 0	
	//=============================================================================================
	// �������Ų������жϣ������֣�
	// A. ����������ȫ�������ꡣ
	// B. ���������ѷ�����ϡ�
	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	ucHighInt = LD_ReadReg(0x29); 
	ucLowInt=LD_ReadReg(0x02); 
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
	// A. ����������ȫ�������ꡣ

			LD_WriteReg(0x2B,  0);
			LD_WriteReg(0xBA, 0);	
			LD_WriteReg(0xBC,0x0);	
			bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
			LD_WriteReg(0x08,1);
			LD_WriteReg(0x08,0);
			LD_WriteReg(0x33, 0);
			f_close(&fsrc);
			nLD_Mode = LD_MODE_ASR_RUN;
			nAsrStatus = LD_ASR_NONE;
			return ;
    }

	if(nMp3Pos>=nMp3Size)
	{
	// B. ���������ѷ�����ϡ�

		LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);
		f_close(&fsrc);
		nLD_Mode = LD_MODE_ASR_RUN;
		nAsrStatus = LD_ASR_NONE;
		return;	
	}

	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	

	LD_ReloadMp3Data_Again();
			
	LD_WriteReg(0x29,ucHighInt); 
	LD_WriteReg(0x02,ucLowInt) ;
#endif
}


/***********************************************************
* ��    �ƣ�uint8 LD_Check_ASRBusyFlag_b2(void)
* ��    �ܣ���� ASR �Ƿ�æ״̬
* ��ڲ�����flag ---1������״̬  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_Check_ASRBusyFlag_b2(void)
{
	uint8 j;
	uint8 flag = 0;
	for (j=0; j<10; j++)
	{
		//���ASRæ��״̬����⵽0x21��ʾ�У���ѯ��Ϊ��״̬���Խ�����һ�� ASR ����
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		LD3320_delay(10);		
	}
	return flag;
}
/***********************************************************
* ��    �ƣ� void LD_AsrStart(void)
* ��    �ܣ�
* ��ڲ�����  ASR��ʼ��
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();
}
/***********************************************************
* ��    �ƣ� uint8 LD_AsrRun(void)
* ��    �ܣ� ASRִ�к���
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);
	LD_WriteReg(0x37, 0x06);
	LD3320_delay( 5);
	LD_WriteReg(0x1C, 0x0b);
	LD_WriteReg(0x29, 0x10);
	
	LD_WriteReg(0xBD, 0x00);   
	return 1;
}

/***********************************************************
* ��    �ƣ�uint8 LD_AsrAddFixed(void)
* ��    �ܣ����ʶ��ؼ�����
* ��ڲ�����  
* ���ڲ����� flag-- 1��success
* ˵    ���������߿���ѧϰ"����ʶ��оƬLD3320�߽��ؼ�.pdf"��
						���������������մ�����÷������ʶ���ʡ�
* ���÷����� 
**********************************************************/ 
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 12    	/*�����ά��ֵ*/
	#define DATE_B 50		/*����һά��ֵ*/
	uint8  sRecog[DATE_A][DATE_B] = 
	{
		STR00,\
		STR01,\
		STR02,\
		STR03,\
		STR04,\
		STR05,\
		STR06,\
		STR07,\
		STR08,\
		STR09,\
		STR10,\
		STR11\
	};	/*��ӹؼ��ʣ��û��޸�*/
	uint8  pCode[DATE_A] = 
	{
		CODE00,\
		CODE01,\
		CODE02,\
		CODE03,\
		CODE04,\
		CODE05,\
		CODE06,\
		CODE07,\
		CODE08,\
		CODE09,\
		CODE10,\
		CODE11\
	};	/*���ʶ���룬�û��޸�*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{		
		//��� ASR �Ƿ�æ״̬��0Ϊæ״̬
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;	//ʧ��
			break;
		}

		LD_WriteReg(0xc1, pCode[k] ); //ASR��ʶ���� Index��00H��FFH��
		LD_WriteReg(0xc3, 0 );		  //ASR��ʶ�������ʱд�� 00
		LD_WriteReg(0x08, 0x04);	  //���FIFO_EXT
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);      //���ָ��FIFO��д��00
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
			break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]); //��ȡFIFO���ݿ�
		}
		LD_WriteReg(0xb9, nAsrAddLength);//ÿ���һ��ʶ������Ҫ�趨һ��ʶ����䳤��
		LD_WriteReg(0xb2, 0xff);//ASR:DSPæ��״̬
		LD_WriteReg(0x37, 0x04);//����ʶ������·��Ĵ�����д04H֪ͨDSPҪ���һ��ʶ�����
	}	 

	return flag;
}

/***********************************************************
* ��    �ƣ� uint8 LD_GetResult(void)
* ��    �ܣ� ��ȡʶ����
* ��ڲ�����  
* ���ڲ����� ����ʶ���� LD_ReadReg(0xc5 );
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5 );
}



