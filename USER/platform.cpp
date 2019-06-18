/*
 *  Author: Kaka Xie
 *  brief: platform
 */

#include "platform.h"
#include "can.h"
#include "sys.h"
#include "delay.h"
#include "param.h"
#include "rfid.h"


sys_status_t sys_status_ram = {0};
sys_status_t *sys_status = &sys_status_ram;

uint16_t rfid_src_mac_id = 0;

uint32_t get_tick(void)
{
    return OSTimeGet();
}

void mcu_restart(void)
{
    __set_FAULTMASK(1);//close all interrupt
    NVIC_SystemReset();//reset all
}

static uint16_t get_id(void)
{
    return (GPIO_ReadInputData(GPIOB) >> 3) & 0x003f;;
}

static void id_key_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void platform_gpio_init(void)
{
    id_key_init();
}

uint32_t test_hardware_version = 0;

void hardware_init(void)
{
    platform_gpio_init();
    init_can1();
    rfid_init();
}

void user_param_init(void)
{
    uint16_t id = get_id();
    rfid_src_mac_id = id + RFID_CAN_MAC_SRC_ID_BASE;
//    rfid_src_mac_id = get_id() + RFID_CAN_MAC_SRC_ID_BASE;
    rfid_src_mac_id = 0xd6;
}
