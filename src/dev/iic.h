#ifndef _iic_H
#define _iic_H

#include "config.h"

/* ����I2C�������ӵ�GPIO�˿�, ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define GPIO_PORT_I2C	GPIOB					// GPIO�˿� 
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB	// GPIO�˿�ʱ�� 
#define I2C_SCL_PIN		GPIO_Pin_6				// ���ӵ�SCLʱ���ߵ�GPIO 
#define I2C_SDA_PIN		GPIO_Pin_7				// ���ӵ�SDA�����ߵ�GPIO 
#define I2C_SCL_1()     GPIO_SetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		// SCL = 1 
#define I2C_SCL_0()     GPIO_ResetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		// SCL = 0 

#define I2C_SDA_1()     GPIO_SetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		// SDA = 1 
#define I2C_SDA_0()     GPIO_ResetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		// SDA = 0 

#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	// ��SDA����״̬ 

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */

//IIC���в�������			
void i2c_Start(void);				//����IIC��ʼ�ź�
void i2c_Stop(void);				//����IICֹͣ�ź�
void i2c_SendByte(uint8_t _ucByte); //IIC����һ���ֽ�
uint8_t i2c_ReadByte(void);			//IIC��ȡһ���ֽ�
uint8_t i2c_WaitAck(void);			//IIC�ȴ�ACK�ź�
void i2c_Ack(void);					//IIC����ACK�ź�
void i2c_NAck(void);				//IIC������ACK�ź�
uint8_t i2c_CheckDevice(uint8_t _Address);
#endif
