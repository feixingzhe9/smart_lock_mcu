#ifndef __LOCK_PARAM_H__
#define __LOCK_PARAM_H__

#include "sys.h"
#include <stdio.h>


#define LOCK_NUM_MAX        3

#define LOCK_1_OUT_PORT     GPIOG
#define LOCK_1_OUT_PIN      GPIO_Pin_5

#define LOCK_2_OUT_PORT     GPIOG
#define LOCK_2_OUT_PIN      GPIO_Pin_4

#define LOCK_3_OUT_PORT     GPIOG
#define LOCK_3_OUT_PIN      GPIO_Pin_3


#define LOCK_1_IN_PORT     GPIOD
#define LOCK_1_IN_PIN      GPIO_Pin_9

#define LOCK_2_IN_PORT     GPIOD
#define LOCK_2_IN_PIN      GPIO_Pin_10

#define LOCK_3_IN_PORT     GPIOD
#define LOCK_3_IN_PIN      GPIO_Pin_11


struct gpio_in_int_param_t
{
    GPIO_TypeDef*  out_port;
    uint16_t out_pin;

    GPIO_TypeDef*  in_port;
    uint16_t in_pin;

    uint8_t nvic_irq_channel;//NVIC_IRQChannel
    uint32_t exit_line;//EXTI_Line
    uint8_t gpio_port_int_source;
    uint8_t gpio_pin_int_source;
};


extern const struct gpio_in_int_param_t gpio_in_int_param[LOCK_NUM_MAX];
extern uint32_t lock_status_change_start_tick;
extern uint8_t is_lock_status_changed;


#endif
