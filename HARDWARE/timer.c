#include "timer.h"


//void timer_1_ch1_pwm_init(u16 arr, u16 psc, uint16_t pulse)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    TIM_OCInitTypeDef  TIM_OCInitStructure;
////    TIM_Cmd(TIM1, DISABLE);
////    TIM_CtrlPWMOutputs(TIM1, DISABLE);
////    TIM_DeInit(TIM1);
////    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);
////    TIM_DeInit(TIM1);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    TIM_TimeBaseStructure.TIM_Period = arr;//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//    TIM_TimeBaseStructure.TIM_Prescaler = psc;//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�:TDTS = Tck_tim
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
////    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

//    TIM_ARRPreloadConfig(TIM1, ENABLE);//ʹ��ARRԤװ�أ���ֹ���ϼ���ʱ�����¼��쳣�ӳ�

//    /*************����ռ�ձȣ���TIM1_CH1���ó�pwm���***************/
//    //��ʼ��TIM1_CH1 PWM���
//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�������:TIM����Ƚϼ��Ե�
////    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//�����������:TIM����Ƚϻ������Ը�
////    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//ʹ�ܻ��������
//    TIM_OCInitStructure.TIM_Pulse = pulse;
//    TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
//    TIM_OC1Init(TIM1, &TIM_OCInitStructure);//�����ò�����ʼ������TIM1 OC1

//    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);//ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���

//    TIM_Cmd(TIM1, ENABLE);//ʹ��TIM1
//    TIM_CtrlPWMOutputs(TIM1, ENABLE);

////    TIM_SetCompare1(TIM1, 50);

//}

