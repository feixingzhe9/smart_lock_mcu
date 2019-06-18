#include "param.h"


#ifdef __cplusplus
extern "C" {
#endif


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
    [LOCK_4] =
    {
        .out_port = LOCK_4_OUT_PORT,
        .out_pin = LOCK_4_OUT_PIN,
        .in_port = LOCK_4_IN_PORT,
        .in_pin = LOCK_4_IN_PIN,

        .nvic_irq_channel = EXTI15_10_IRQn,
        .exit_line = EXTI_Line12,
        .gpio_port_int_source = GPIO_PortSourceGPIOD,
        .gpio_pin_int_source = GPIO_PinSource12,
    },

};


const platform_gpio_t platform_gpio_pins[] =
{
    [PLATFORM_GPIO_SYS_LED]                     = { GPIOB,  GPIO_Pin_3},

    [PLATFORM_GPIO_PHO_SWITCH_3]                = {GPIOB, GPIO_Pin_4},
    [PLATFORM_GPIO_PHO_SWITCH_2]                = {GPIOB, GPIO_Pin_5},
    [PLATFORM_GPIO_PHO_SWITCH_1]                = {GPIOB, GPIO_Pin_6},
    [PLATFORM_GPIO_MOTOR_DIR]                   = {GPIOA, GPIO_Pin_6},
    [PLATFORM_GPIO_MOTOR_EN]                    = {GPIOA, GPIO_Pin_5},
    [PLATFORM_GPIO_MOTOR_PWR_EN]                = {GPIOA, GPIO_Pin_4},
    [PLATFORM_GPIO_LOCK_CTRL]                   = {GPIOB, GPIO_Pin_7},
};

#ifdef __cplusplus
  }
#endif
