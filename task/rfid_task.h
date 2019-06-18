#ifndef __TASK_RFID_TASK_H__
#define __TASK_RFID_TASK_H__

#include "stm32f10x.h"
#include "ucos_ii.h"

#define RFID_STK_SIZE       256

extern OS_STK rfid_task_stk[RFID_STK_SIZE];

void rfid_task(void *pdata);
#endif

