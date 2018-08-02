#include "led.h"

//初始化PA8和PD2为输出口.并使能这两个口的时钟		    
//LED IO初始化

LEDClass LED;

void LEDClass::LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED0-->PC.11 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.11
 GPIO_SetBits(GPIOC,GPIO_Pin_9);						 //PA.11 输出高
}
 
