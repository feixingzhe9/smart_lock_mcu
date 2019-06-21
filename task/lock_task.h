#ifndef __TASK_LOCK_TASK_H__
#define __TASK_LOCK_TASK_H__

#include "stm32f10x.h"
#include "ucos_ii.h"

#define LOCK_TASK_STK_SIZE      64

extern OS_STK lock_task_stk[LOCK_TASK_STK_SIZE];

void lock_task(void *pdata);
#endif
