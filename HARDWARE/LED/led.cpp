#include "led.h"

//��ʼ��PA8��PD2Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��

LEDClass LED;

void LEDClass::led_init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED0-->PC.11 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.11
 GPIO_SetBits(GPIOC,GPIO_Pin_9);						 //PA.11 �����
}

void LEDClass::led_on(void)
{
    GPIO_SetBits(GPIOC,GPIO_Pin_9);
}
 
void LEDClass::led_off(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_9);
}
