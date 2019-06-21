/*
 *  Author: Kaka Xie
 *  brief: platform
 */

#include "platform.h"
#include "can.h"
#include "sys.h"
#include "delay.h"
#include "param.h"

sys_status_t sys_status_ram = {0};
sys_status_t *sys_status = &sys_status_ram;


uint32_t get_tick(void)
{
    return OSTimeGet();
}

void mcu_restart(void)
{
    __set_FAULTMASK(1);//close all interrupt
    NVIC_SystemReset();//reset all
}

static void lock_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*GPIO_B*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}



static void platform_gpio_init(void)
{
    lock_gpio_init();
}


uint32_t test_hardware_version = 0;

void hardware_init(void)
{
    platform_gpio_init();
    init_can1();
}

void user_param_init(void)
{

}
