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


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

    //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB0

    //���ø�����Ϊ�����������,���TIM3 CH3��PWM���岨��	GPIOB.0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

    //��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM3 Channel3 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3


    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //�Ƚ����ʹ��
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

    TIM_SetCompare3(TIM3,50);

}

void beeper_on(u16 hz)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

    TIM_SetCompare3(TIM3,50);/////test code, frequecy is not right
}


void beeper_off(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //�Ƚ����ʹ��
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC3

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




