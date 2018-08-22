#include "motor.h"
#include "timer.h"
#include "cp2532.h"

MotorClass motor;

MotorClass::MotorClass(void)
{
//    u16 arr = 200;
//    u16 psc = 50;
//    GPIO_InitTypeDef GPIO_InitStructure;
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    TIM_OCInitTypeDef  TIM_OCInitStructure;


//    RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//使能定时器1时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

//    //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB0

//    //设置该引脚为复用输出功能,输出TIM1 CH3的PWM脉冲波形	GPIOB.15
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //TIM1_CH3
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
//    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

//    //初始化TIM1
//    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

//    //初始化TIM1 Channel3 PWM模式
//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3

//    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR3上的预装载寄存器

//    TIM_Cmd(TIM1, ENABLE);  //使能TIM1


//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //比较输出使能
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3

//    TIM_SetCompare3(TIM1,50);


}

void MotorClass::init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void MotorClass::start(void)
{
//    TIM_OCInitTypeDef  TIM_OCInitStructure;

//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3

//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3

//    TIM_SetCompare3(TIM1,50);/////test code, frequecy is not right
    
    GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

void MotorClass::stop(void)
{
//    TIM_OCInitTypeDef  TIM_OCInitStructure;

//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3

//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //比较输出使能
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3

//    TIM_SetCompare3(TIM1,50);/////test code, frequecy is not right
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}

void motor_init(void)
{
    motor.init();
}

#define TOUCH_KEY_MOTOR_TIME   50/SYSTICK_PERIOD
void motor_key_task(uint16_t key_value)
{
    static u16 key_value_last = 0;
    static u32 start_tick = 0;
    static bool motor_status = false;

    if(get_tick() - start_tick >= TOUCH_KEY_MOTOR_TIME)
    {
        if(motor_status == true)
        {
            motor.stop();
            motor_status = false;
        }
    }

    if(key_value_last != key_value)
    {
        key_value_last = key_value;
        start_tick = get_tick();
        if(key_value != 0)
        {
            motor.start();
            motor_status = true;
        }
    }

}

void motor_task(void)
{
    motor_key_task( get_key_value() );
}
