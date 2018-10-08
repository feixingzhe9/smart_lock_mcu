#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//IO��������
//#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}

#define SDA_IN()    do{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL|= (uint32_t)8 << 28;}\
                    while(0)

#define SDA_OUT()   do{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL|= (uint32_t)3 << 28;}\
                    while(0)

//IO��������
//#define IIC_SCL    PBout(10) //SCL
//#define IIC_SDA    PBout(11) //SDA
//#define READ_SDA   PBin(11)  //����SDA
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA
#define READ_SDA   PBin(7)  //����SDA



#define I2C_SDA_PIN         GPIO_Pin_7
#define I2C_SDA_PORT        GPIOB

#define I2C_SCL_PIN         GPIO_Pin_6
#define I2C_SCL_PORT        GPIOB

#define RCC_APB2Periph_I2C_PORT     RCC_APB2Periph_GPIOB

//IIC���в�������
void i2c_init(void);                //��ʼ��IIC��IO��
void IIC_Start(void);               //����IIC��ʼ�ź�
void IIC_Stop(void);                //����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);         //IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void);              //IIC�ȴ�ACK�ź�
void IIC_Ack(void);                 //IIC����ACK�ź�
void IIC_NAck(void);                //IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);



#endif



