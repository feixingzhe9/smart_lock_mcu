#ifndef __USER_LED_TASK_H__
#define __USER_LED_TASK_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INDICATOR_LED_STK_SIZE                   32
extern OS_STK indicator_led_task_stk[INDICATOR_LED_STK_SIZE];

void indicator_led_task(void *pdata);

#endif
#ifdef __cplusplus
  }
#endif
