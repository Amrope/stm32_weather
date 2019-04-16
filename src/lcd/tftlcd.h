#ifndef __TFTLCD_H
#define __TFTLCD_H

#include "config.h"

#define TFTLCD_R61509V3

//��ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ

typedef struct
{
	uint16 LCD_CMD;
	uint16 LCD_DATA;
}TFTLCD_TypeDef;

//ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A10��Ϊ�������������� 
//ע������16λ����ʱSTM32�ڲ�������һλ����!			    
#define TFTLCD_BASE        ((u32)(0x6C000000 | 0x000007FE))
#define TFTLCD             ((TFTLCD_TypeDef *) TFTLCD_BASE)

//TFTLCD��Ҫ������
typedef struct  
{										    
	uint16 width;			//LCD ���
	uint16 height;			//LCD �߶�
	uint16 id;				//LCD ID
	uint8  dir;           	//LCD ����
}TFTLCD_DATA_t;

/* ����LCD��Ҫ���� */
extern TFTLCD_DATA_t tftlcd_data;	
/* LCD��ǰ����ɫ�ͱ���ɫ	*/   
extern uint16  FRONT_COLOR;//ǰ����ɫ Ĭ�Ϻ�ɫ    
extern uint16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

void LCD_WriteCmd(uint16 cmd);  //д�Ĵ�������
void LCD_WriteData(uint16 data);//д���ݺ���
void LCD_WriteCmdData(uint16 cmd,uint16 data);//д�Ĵ��������ݺ���
void LCD_WriteData_Color(uint16 color);//�����ݼĴ�����д����ɫֵ
uint16 LCD_ReadData(void);	 //������
void LCD_Display_Dir(u8 dir);//����LCD��ʾ����
void TFTLCD_Init(void);      //��ʼ��
void LCD_Set_Window(uint16 sx,uint16 sy,uint16 width,uint16 height);//���ô���
void LCD_Clear(uint16 Color);	 									  //����
void LCD_Fill(uint16 xState,uint16 yState,uint16 xEnd,uint16 yEnd,uint16 color); //������
void LCD_DrawPoint(uint16 x,uint16 y);							  //����
void LCD_DrawFRONT_COLOR(uint16 x,uint16 y,uint16 color);		  //���ٻ���
uint16 LCD_ReadPoint(uint16 x,uint16 y);							  //��ȡ��Ļ��ĳ�������ɫֵ
void LCD_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2);    //����
void LCD_DrawLine_Color(uint16 x1, uint16 y1, uint16 x2, uint16 y2,uint16 color);//������ɫ����
void LCD_DrowSign(uint16 x, uint16 y, uint16 color);	//��ʮ�ֱ��
void LCD_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2);		//������
void LCD_Draw_Circle(uint16 x0,uint16 y0,uint8 r);					//��Բ��
void LCD_ShowChar(uint16 x,uint16 y,uint8 num,uint8 size,uint8 mode);		//��ָ��λ����ʾһ���ַ�
void LCD_ShowNum(uint16 x,uint16 y,u32 num,uint8 len,uint8 size);	 	//��ʾ����
void LCD_ShowxNum(uint16 x,uint16 y,u32 num,uint8 len,uint8 size,uint8 mode);//��ʾ����
void LCD_ShowString(uint16 x,uint16 y,uint16 width,uint16 height,uint8 size,uint8 *p);//��ʾ�ַ���
void LCD_ShowFontHZ(uint16 x, uint16 y, uint8 *cn); 							//��ʾ����			   						   																			
void LCD_ShowPicture(uint16 x, uint16 y, uint16 wide, uint16 high,uint8 *pic);		//��ʾͼƬ
#endif
