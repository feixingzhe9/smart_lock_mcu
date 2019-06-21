/*
 *  Author: Kaka Xie
 *  brief: lock task
 */

#include "lock_task.h"
#include "delay.h"
#include "lock.h"

OS_STK lock_task_stk[LOCK_TASK_STK_SIZE];


void lock_task(void *pdata)
{
    uint32_t tick = 0;
    delay_ms(1000);
    for(;;)
    {
        tick++;
        all_lock_task(tick);
        delay_ms(50);
    }
}
