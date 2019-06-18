#ifndef __USER_PLATFORM_H__
#define __USER_PLATFORM_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PLATFORM_GPIO_SYS_LED = 0,

    PLATFORM_GPIO_PHO_SWITCH_1,
    PLATFORM_GPIO_PHO_SWITCH_2,
    PLATFORM_GPIO_PHO_SWITCH_3,
    PLATFORM_GPIO_MOTOR_EN,
    PLATFORM_GPIO_MOTOR_PWR_EN,
    PLATFORM_GPIO_MOTOR_DIR,
    PLATFORM_GPIO_LOCK_CTRL,

    PLATFORM_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    PLATFORM_GPIO_NONE,
} platform_gpio_e;


typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;

}platform_gpio_t;


typedef struct
{
    uint16_t sys_status;
}sys_status_t;

extern sys_status_t *sys_status;


uint32_t get_tick(void);
void mcu_restart(void);

void hardware_init(void);
void user_param_init(void);

uint8_t forward_conveyor_belt(void);
uint8_t reverse_conveyor_belt(void);
uint8_t stop_conveyor_belt(void);
uint8_t get_pho_switch_1_state(void);
uint8_t get_pho_switch_2_state(void);
uint8_t get_pho_switch_3_state(void);
void lock_ctrl_unlock(void);
void lock_ctrl_lock(void);

#ifdef __cplusplus
  }
#endif
#endif
