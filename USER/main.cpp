#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hall.h"
#include "rfid.h"
#include "can_interface.h"
#include "stmflash.h"
#include "myiic.h"
#include "cp2532.h"
#include "timer.h"
#include "beeper.h"
#include "lock.h"

static void init_exti(void);
static void sys_indicator(void);



#if 1   //test code

#define FLASH_SAVE_ADDR  0X08070000 				//设置FLASH 保存地址(必须为偶数)
const u8 TEXT_Buffer[]={"STM32 FLASH TEST"};
#define SIZE sizeof(TEXT_Buffer)	 			  	//数组长度

#endif
static void init()
{
    sys_tick_init();
    NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(115200);	 	//串口初始化为115200
    LED.led_init();			     //LED端口初始化
    rfid_init();
    printf("RFID Driver version:%s\r\n", SW_VERSION);
    init_exti();
    IIC_Init();
    //TIM3_PWM_Init(100,100);	 //不分频。PWM频率=72000/900=8Khz
    //TIM_SetCompare3(TIM3,50);
    
    beeper_init(200,50);

    return;
}

int main(void)
{
    init();

#if 0 //test code   of flash  
      
    STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);
#endif
#if 0
    u8 datatemp[SIZE];  
    STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
#endif
    
    
#if 0
    
    cp2532_test = read_byte(0x31);
#endif  
   
    while(1)
    {
        rfid_task();
        touch_key_task();
        all_lock_task();
        can_protocol();
        beeper_task();
        sys_indicator();
        
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


#define INDICATOR_LED_PERIOD    500/SYSTICK_PERIOD

extern void set_beeper_low(void);
static void sys_indicator(void)
{
    static u16 cnt = 0;
    static u32 start_tick = 0;
    if(get_tick() - start_tick >= INDICATOR_LED_PERIOD)
    {
        cnt++;
        //cp2532_test = read_byte(0x31);
        //cp2532_test = quick_read();
        
        if(cnt % 2 == 1)
        {
            LED.led_on();//SDA_IN();
        }
        else
        {
            LED.led_off();
            //TIM_Cmd(TIM3, DISABLE);  //使能TIM3
            //set_beeper_low();
            
        }
        start_tick = get_tick();//SDA_OUT();IIC_SDA = 1;
    }
    
}
