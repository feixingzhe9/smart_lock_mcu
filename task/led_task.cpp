/*
 *  Author: Kaka Xie
 *  brief: led task
 */

#include "led_task.h"
#include "delay.h"
#include "led.h"

LEDClass LED;

OS_STK indicator_led_task_stk[INDICATOR_LED_STK_SIZE];

void indicator_led_task(void *pdata)
{
    delay_ms(500);
    LED.init();
    while(1)
    {
        LED.led_on();
        delay_ms(500);
        LED.led_off();
        delay_ms(500);
    };
}


