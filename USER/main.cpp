#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "beeper.h"
#include "lock.h"
#include <ucos_ii.h>
#include "common.h"
#include "platform.h"

int main(void)
{
    OSInit();
    delay_init();
    NVIC_Configuration();
    __disable_irq();
    hardware_init();
    user_param_init();
    user_init();
    __enable_irq();
    OSStart();
}

