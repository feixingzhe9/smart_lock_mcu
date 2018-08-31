#include "led.h"

LEDClass LED;

LEDClass::LEDClass(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED_RCC_APB2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(LED_PORT,LED_PIN);
}

void LEDClass::led_on(void)
{
    GPIO_SetBits(LED_PORT,LED_PIN);
}

void LEDClass::led_off(void)
{
    GPIO_ResetBits(LED_PORT,LED_PIN);
}


#define INDICATOR_LED_PERIOD        500/SYSTICK_PERIOD
#define INDICATOR_LED_PERIOD_ERR    200/SYSTICK_PERIOD

static uint32_t indicator_period = INDICATOR_LED_PERIOD;
void sys_indicator(void)
{
    static u16 cnt = 0;
    static u32 start_tick = 0;

    if(sys_err & SYS_CP2532_INIT_ERR)
    {
        indicator_period = INDICATOR_LED_PERIOD_ERR;
    }

    if(get_tick() - start_tick >= indicator_period)
    {
        cnt++;

        if(cnt % 2 == 1)
        {
            LED.led_on();
        }
        else
        {
            LED.led_off();
        }
        start_tick = get_tick();
    }
}
