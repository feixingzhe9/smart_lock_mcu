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


//    RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

//    //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB0

//    //���ø�����Ϊ�����������,���TIM1 CH3��PWM���岨��	GPIOB.15
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //TIM1_CH3
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
//    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

//    //��ʼ��TIM1
//    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

//    //��ʼ��TIM1 Channel3 PWMģʽ
//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3

//    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR3�ϵ�Ԥװ�ؼĴ���

//    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1


//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //�Ƚ����ʹ��
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3

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

//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3

//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3

//    TIM_SetCompare3(TIM1,50);/////test code, frequecy is not right
    
    GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

void MotorClass::stop(void)
{
//    TIM_OCInitTypeDef  TIM_OCInitStructure;

//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3

//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //�Ƚ����ʹ��
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3

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
