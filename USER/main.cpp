#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hall.h"
#include "rfid.h"
//#include "super_rfid.h"
#include "can_interface.h"


static void init_exti(void);

static void init()
{
		delay_init();	    	 //延时函数初始化	  
		NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		uart_init(115200);	 	//串口初始化为115200
		LED.LED_Init();			     //LED端口初始化
		hall_init();
		rfid_init();
//		super_rfid_init();
		printf("RFID Driver version:%s\r\n", SW_VERSION);
	  init_exti();
	
		return;
}

int main(void)
{
	u16 i=0;

	init();
	
	while(1)
	{
		rfid_task();
		hall_task();
//		super_rfid_task();
        can_protocol();
		
		delay_ms(10);
		if (++i >= 10)
		{
			LED0 =! LED0;//提示系统正在运行	
			i = 0;
		}		   
	}
}


static void init_exti(void)
{
	  EXTI_InitTypeDef exit_init_structure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
	  exit_init_structure.EXTI_Line = EXTI_Line4;
	  exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
	  exit_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  exit_init_structure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&exit_init_structure);
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
	  exit_init_structure.EXTI_Line = EXTI_Line6;
	  EXTI_Init(&exit_init_structure);
	
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
}


