/*
 *  Author: Kaka Xie
 *  brief: beeper task
 */

#include "beeper_task.h"
#include "delay.h"


OS_STK beeper_task_stk[BEEPER_TASK_STK_SIZE];

OS_EVENT *beeper_mailbox_handle = (OS_EVENT *)0;

beeper_ctrl_t g_beeper_ctrl = {0, 0};
void beeper_task(void *pdata)
{
    uint8_t err = 0;
    beeper_ctrl_t beeper_ctrl;
    beeper_ctrl.ctrl = 0;
    beeper_init(200, 50);
    delay_ms(1000);
    for(;;)
    {
        beeper_ctrl.ctrl = *(uint32_t *)OSMboxPend(beeper_mailbox_handle, 0, &err);
        if(err == OS_ERR_NONE)
        {
            if(beeper_ctrl.ctrl != 0)
            {
                beeper_off();
                uint8_t times = beeper_ctrl.ctrl_t.times;
                uint8_t duration = beeper_ctrl.ctrl_t.duration;
                uint8_t interval_time = beeper_ctrl.ctrl_t.interval_time;
                for(uint8_t i = 0; i < times; i++)
                {
                    beeper_on(0);
                    if(duration > 0) delay_ms(duration * 20);
                    beeper_off();
                    if(interval_time > 0) delay_ms(interval_time * 20);
                }
                beeper_off();
                beeper_ctrl.ctrl = 0;
            }
        }
    }
}
