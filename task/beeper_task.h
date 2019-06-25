#ifndef __TASK_BEEPER_TASK_H__
#define __TASK_BEEPER_TASK_H__

#include "stm32f10x.h"
#include "ucos_ii.h"
#include "beeper.h"

#define BEEPER_TASK_STK_SIZE            32

extern OS_STK beeper_task_stk[BEEPER_TASK_STK_SIZE];

extern OS_EVENT *beeper_mailbox_handle;

void beeper_task(void *pdata);

#endif

