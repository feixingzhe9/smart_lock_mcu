#include "beeper.h"
#include "timer.h"
#include "cp2532.h"
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
void touch_key_beeper_task(u16 key_value)
{
    static u16 key_value_last = 0;
    static u32 start_tick = 0;
    static bool beeper_status = false;

    if(get_tick() - start_tick >= TOUCH_KEY_BEEPER_TIME)
    {
        if(beeper_status == true)
        {
            beeper_off();
            beeper_status = false;
        }
    }

    if(key_value_last != key_value)
    {
        key_value_last = key_value;
        start_tick = get_tick();
        if(key_value != 0)
        {
            beeper_on(0);
            beeper_status = true;
        }
    }
}


#define RFID_BEEPER_TIME    120/SYSTICK_PERIOD
void rfid_beeper_task(u32 rfid_start_tick)
{
    static bool rfid_beeper_status = false;
    if(get_tick() - rfid_start_tick <= RFID_BEEPER_TIME)
    {
        if(rfid_beeper_status == false)
        {
            beeper_on(0);
            rfid_beeper_status = true;
        }
    }
    else
    {
        if(rfid_beeper_status == true)
        {
            beeper_off();
            rfid_beeper_status = false;
        }
    }
}

void beeper_task(void)
{
    touch_key_beeper_task( get_key_value() );
    rfid_beeper_task(rfid_start_tick);
}




