/*
 *  Author: Kaka Xie
 *  brief: rfid task
 */

#include "rfid_task.h"
#include "delay.h"
#include "rfid.h"

OS_STK rfid_task_stk[RFID_STK_SIZE];

void rfid_task(void *pdata)
{
    delay_ms(1000);
    for(;;)
    {
        rfid_proc();
        delay_ms(50);
    }
}

