#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置
//#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}

#define SDA_IN()    do{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL|= (uint32_t)8 << 28;}\
                    while(0)

#define SDA_OUT()   do{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL|= (uint32_t)3 << 28;}\
                    while(0)

//IO操作函数
//#define IIC_SCL    PBout(10) //SCL
//#define IIC_SDA    PBout(11) //SDA
//#define READ_SDA   PBin(11)  //输入SDA
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA
#define READ_SDA   PBin(7)  //输入SDA



#define I2C_SDA_PIN         GPIO_Pin_7
#define I2C_SDA_PORT        GPIOB

#define I2C_SCL_PIN         GPIO_Pin_6
#define I2C_SCL_PORT        GPIOB

#define RCC_APB2Periph_I2C_PORT     RCC_APB2Periph_GPIOB

//IIC所有操作函数
void i2c_init(void);                //初始化IIC的IO口
void IIC_Start(void);               //发送IIC开始信号
void IIC_Stop(void);                //发送IIC停止信号
void IIC_Send_Byte(u8 txd);         //IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void);              //IIC等待ACK信号
void IIC_Ack(void);                 //IIC发送ACK信号
void IIC_NAck(void);                //IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);



#endif



