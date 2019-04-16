#include "timer.h"

uint16 Uart2CharRecTimer =0;

/********************************************************************************************************
** ����: TIM1_Init, ͨ�ö�ʱ��1��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
** ˵��: ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft
** ����: void	
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM1_Init(uint16 arr,uint16 psc)
{
	int i;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //	/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //	/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; /* ���ⲿʱ�ӽ��в�����ʱ�ӷ�Ƶ,����û���õ� */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //
 	TIM_ClearFlag(TIM1,TIM_FLAG_Update );
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //
 
	//�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel =  TIM1_UP_IRQn;;//ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ���Ĵ���
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMX		
    
	for(i=0;i< MAX_TIMER ;i++)
	 g_Timer1[i] = 0;   //ȫ�ֶ�ʱ����ʼ��
}


/********************************************************************************************************
** ����: TIM2_Init, ͨ�ö�ʱ��2��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
** ˵��: ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft
** ����: void	
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM2_Init(uint16 arr,uint16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//TIM2ʱ��ʹ��    
	
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
	
	TIM_Cmd(TIM2,ENABLE);	//������ʱ��2
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}

/********************************************************************************************************
** ����: TIM3_Init, ͨ�ö�ʱ��3��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
** ˵��: ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft
** ����: void	
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM3_Init(uint16 arr,uint16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //	/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //	/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; /* ���ⲿʱ�ӽ��в�����ʱ�ӷ�Ƶ,����û���õ� */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //
 	TIM_ClearFlag(TIM3,TIM_FLAG_Update );
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //
 
	//�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel =  TIM3_IRQn;;//ָ���ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ���Ĵ���
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMX			 
}

/********************************************************************************************************
** ����: TIM1_IRQHandler,  ��ʱ��1�жϷ������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��	
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM1_UP_IRQHandler(void)   //TIM1�ж�
{
	uint8 i;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //���TIM1�����жϷ������
	{
		//-------------------------------------------------------------------------------
		// ���ֶ�ʱ������ʱ
		for (i = 0; i < MAX_TIMER; i++)					// ��ʱʱ��ݼ�							
		if( g_Timer1[i] ) g_Timer1[i]-- ;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );   //���TIMx�����жϱ�־ 
	}
} 

/********************************************************************************************************
** ����: TIM2_IRQHandler,  ��ʱ��2�жϷ������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��		
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/	    
void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		if(Uart2CharRecTimer) Uart2CharRecTimer --;
		else if((USART2_RX_STA>0)&& (( USART2_RX_STA & (1<<15))==0))  
		{
			USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIM2�����жϱ�־    
	}	    
}

/********************************************************************************************************
** ����: TIM3_IRQHandler,  ��ʱ��3�жϷ������
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��	
** ����: 2017.12.23 by Hezhijie and Lizhengnian 
********************************************************************************************************/
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM1�����жϷ������
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
	}
} 
