#ifndef __USER_OS_COMMON_H__
#define __USER_OS_COMMON_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#include "led_task.h"
#include "can_protocol_task.h"
#include "rfid_task.h"

typedef enum
{
    CAN_SEND_TASK_PRIO = 5,
    CAN_RPOTOCOL_TASK_PRIO,
    RFID_TASK_PRIO,
    INDICATOR_LED_TASK_PRIO
}task_prio_e;

void os_user_config(void);
void user_init_depend_on_os_config(void);

uint32_t get_tick(void);
void user_init(void);

#endif

