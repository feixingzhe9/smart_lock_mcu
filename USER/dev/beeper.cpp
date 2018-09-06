#include "beeper.h"
#include "timer.h"
#include "cp2532.h"
#include "lock.h"
extern u32 rfid_start_tick;


void beeper_init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

    //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB0

    //设置该引脚为复用输出功能,输出TIM3 CH3的PWM脉冲波形	GPIOB.0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel3 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR3上的预装载寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIM3


    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //比较输出使能
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

    TIM_SetCompare3(TIM3,50);

}

void beeper_on(u16 hz)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

    TIM_SetCompare3(TIM3,50);/////test code, frequecy is not right
}


void beeper_off(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //比较输出使能
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

    TIM_SetCompare3(TIM3,50);/////test code, frequecy is not right
}


#define TOUCH_KEY_BEEPER_TIME   30/SYSTICK_PERIOD
#define RFID_BEEPER_TIME    120/SYSTICK_PERIOD
#define UNLOCK_BEEPER_TIME  500/SYSTICK_PERIOD
#define BEEPER_ON_DELAY_PERIOD      50/SYSTICK_PERIOD

void beeper_ctrl(void)
{
    static bool beeper_status = false;
    static uint16_t key_value_last = 0;
    static uint8_t beeper_machine = 0;
    static uint32_t beeper_start_tick = 0;
    static uint32_t beeper_on_period = 0;
    static uint32_t beeper_delay_on_start_tick = 0;
    uint16_t key_value = get_key_value();

    do
    {
        if(key_value != key_value_last)
        {
            key_value_last = key_value;
            if(key_value > 0)
            {
                beeper_on_period = TOUCH_KEY_BEEPER_TIME;
                beeper_machine = 1;
                beeper_start_tick = get_tick();
                break;
            }
        }

        if(rfid_start_tick != 0)
        {
            beeper_on_period = RFID_BEEPER_TIME;
            beeper_machine = 1;
            rfid_start_tick = 0;
            beeper_start_tick = get_tick();
            break;
        }

        if(unlock_start_tick != 0)
        {
            beeper_on_period = UNLOCK_BEEPER_TIME;
            beeper_machine = 1;
            unlock_start_tick = 0;
            beeper_start_tick = get_tick();
            break;
        }
    }while(0);

    switch(beeper_machine)
    {
        case 0:
            break;

        case 1:     //beeper on
            if(beeper_start_tick != 0)
            {
                if(beeper_status == false)
                {
                    beeper_on(0);
                    beeper_status = true;
                    beeper_machine = 2;
                }
                else
                {
                    beeper_machine = 3;
                    beeper_delay_on_start_tick = get_tick();
                }
            }
            break;

        case 2:
            if(get_tick() - beeper_start_tick >= beeper_on_period)
            {
                beeper_off();
                beeper_status = false;
                beeper_machine = 0;
            }
            break;

        case 3:
            if(get_tick() - beeper_delay_on_start_tick <= BEEPER_ON_DELAY_PERIOD)
            {
                beeper_off();
                beeper_status = false;
            }
            else
            {
                beeper_machine = 1;
                beeper_start_tick = get_tick();
            }
            break;

        default:
            break;
    }

}

void beeper_task(void)
{
    beeper_ctrl();
}




