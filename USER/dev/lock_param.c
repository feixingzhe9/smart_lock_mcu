#include "lock_param.h"

#ifdef __cplusplus
extern "C" {
#endif

enum LOCK
{
    LOCK_1 = 0,
    LOCK_2,
    LOCK_3
};

const struct gpio_in_int_param_t gpio_in_int_param[LOCK_NUM_MAX] =
{

    [LOCK_1] =
    {
        .out_port = LOCK_1_OUT_PORT,
        .out_pin = LOCK_1_OUT_PIN,
        .in_port = LOCK_1_IN_PORT,
        .in_pin = LOCK_1_IN_PIN,

        .nvic_irq_channel = EXTI9_5_IRQn,
        .exit_line = EXTI_Line9,
        .gpio_port_int_source = GPIO_PortSourceGPIOD,
        .gpio_pin_int_source = GPIO_PinSource9,
    },

    [LOCK_2] =
    {
        .out_port = LOCK_2_OUT_PORT,
        .out_pin = LOCK_2_OUT_PIN,
        .in_port = LOCK_2_IN_PORT,
        .in_pin = LOCK_2_IN_PIN,

        .nvic_irq_channel = EXTI15_10_IRQn,
        .exit_line = EXTI_Line10,
        .gpio_port_int_source = GPIO_PortSourceGPIOD,
        .gpio_pin_int_source = GPIO_PinSource10,
    },

    [LOCK_3] =
    {
        .out_port = LOCK_3_OUT_PORT,
        .out_pin = LOCK_3_OUT_PIN,
        .in_port = LOCK_3_IN_PORT,
        .in_pin = LOCK_3_IN_PIN,

        .nvic_irq_channel = EXTI15_10_IRQn,
        .exit_line = EXTI_Line11,
        .gpio_port_int_source = GPIO_PortSourceGPIOD,
        .gpio_pin_int_source = GPIO_PinSource11,
    },

};

#ifdef __cplusplus
  }
#endif
