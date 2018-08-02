#include <stdint.h>
#include <stdio.h>
#include "hall.h"
#include "can_interface.h"

static uint8_t Hall_Scan( void );
static uint8_t is_hall_changed(void);
static void upload_hall_data(can_message_t *hall_msg, uint8_t hall_state);

can_message_t upload_msg;

void hall_init()
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	
	  	//初始化KEY0-->GPIOC.1,KEY1-->GPIOC.13  上拉输入
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC, ENABLE);//使能PORTA,PORTE时钟

		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC12

		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认下拉	  
		GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOA.0
	
		return;
}


void hall_task()
{
	  uint8_t hall_state;
	
		hall_state = Hall_Scan();
	
	  if (is_hall_changed())
		{
				upload_hall_data(&upload_msg, hall_state);
		}
	
		return;
}

static uint8_t hall_state_pre = 0x00;
static uint8_t hall_state = 0x00;
static uint8_t hall_state_changed = 0;


static uint8_t Hall_Scan( void )
{
	if ( HALL1 ) hall_state &= ~(uint8_t)(HALL1_ON);
	else hall_state |= HALL1_ON;
	
	if ( HALL2 ) hall_state &= ~(uint8_t)(HALL2_ON);
	else hall_state |= HALL2_ON;
	
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

static uint8_t is_hall_changed(void)
{
	return (hall_state_changed == 0x0) ? 0 : 1;
}

static void upload_hall_data(can_message_t *hall_msg, uint8_t hall_state)
{
	  if (!hall_msg) return;

		hall_msg->id = 0x1aa02185;
		hall_msg->data[0] = 0;
		if (hall_state & HALL1_ON)
		{
			hall_msg->data[1] = 1;
			printf("hall 1 is detected\r\n");
		}
		else 
		{
			hall_msg->data[1] = 0;
			printf("hall 1 is undetected\r\n");
		}
		
		if (hall_state & HALL2_ON)
		{
			hall_msg->data[2] = 1;
			printf("hall 2 is detected\r\n");
		}
		else
		{
			hall_msg->data[2] = 0;
			printf("hall 2 is undetected\r\n");
		}
		hall_msg->data_len = 3;
		can.can_send( hall_msg );
		
		return;
}

uint8_t is_hall_detected()
{
		return hall_state;
}

