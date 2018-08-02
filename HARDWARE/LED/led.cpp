#include "led.h"

LEDClass LED;

void LEDClass::led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED_RCC_APB2, ENABLE);	 //ʹ��PA,PE�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = LED_PIN;				 //LED0-->PC.11 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.11
    GPIO_SetBits(LED_PORT,LED_PIN);						 //PA.11 �����
}

void LEDClass::led_on(void)
{
    GPIO_SetBits(LED_PORT,LED_PIN);
}
 
void LEDClass::led_off(void)
{
    GPIO_ResetBits(LED_PORT,LED_PIN);
}
