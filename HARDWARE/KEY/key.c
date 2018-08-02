#include "key.h"
#include "sys.h" 
#include "delay.h" 
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 

 	GPIO_InitTypeDef GPIO_InitStructure;

	//��ʼ��KEY0-->GPIOC.1,KEY1-->GPIOC.13  ��������
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC, ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC12

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOA.0
}

static uint8_t hall_state_pre = 0x00;
static uint8_t hall_state = 0x00;
static uint8_t hall_state_changed = 0;
uint8_t Hall_Scan( void )
{
	if ( HALL1 )
		hall_state &= ~(uint8_t)(HALL1_ON);
	else
		hall_state |= HALL1_ON;
	
	if ( HALL2 )
		hall_state &= ~(uint8_t)(HALL2_ON);
	else
		hall_state |= HALL2_ON;
	
	if (hall_state_pre != hall_state)
	{
		hall_state_changed = 0xff;
	}
	else
	{
		hall_state_changed = 0;
	}
	
	hall_state_pre = hall_state;
	return hall_state;
}

uint8_t is_hall_changed(void)
{
	return (hall_state_changed == 0x0) ? 0 : 1;
}
