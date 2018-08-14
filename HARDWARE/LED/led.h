#ifndef __LED_H
#define __LED_H	 
	 
#include "sys.h"

class LEDClass {
public:
#define LED_PIN         GPIO_Pin_9
#define LED_PORT        GPIOC
#define LED_RCC_APB2    RCC_APB2Periph_GPIOC
    void led_init(void);//≥ı ºªØ
    void led_on(void);
    void led_off(void);
};

extern LEDClass LED;

void sys_indicator(void);

#endif
