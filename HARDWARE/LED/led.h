#ifndef __LED_H
#define __LED_H

#include "sys.h"
#define LED_PIN         GPIO_Pin_11
#define LED_PORT        GPIOC
#define LED_RCC_APB2    RCC_APB2Periph_GPIOC

class LEDClass
{
    public:
        void init(void);
        void led_on(void);
        void led_off(void);
        LEDClass();
};

extern LEDClass LED;

void sys_indicator(void);

#endif
