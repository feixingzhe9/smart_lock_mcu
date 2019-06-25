#ifndef __BEEPER__H_
#define __BEEPER__H_
#include "stdio.h"
#include "sys.h"

struct beeper_times_t
{
    uint32_t start_tick;
    uint32_t times;
    uint32_t frequency;
    uint32_t durantion;
    uint32_t period;
    bool state;
};


typedef union
{
    struct
    {
        uint32_t duration       : 8;
        uint32_t interval_time  : 8;
        uint32_t times          : 8;
        uint32_t cmd            : 8;
    }ctrl_t;
    uint32_t ctrl;
}beeper_ctrl_t;

extern struct beeper_times_t beeper_times;

void beeper_task(void);

void beeper_init(u16 arr,u16 psc);
void beeper_on(uint16_t);
void beeper_off(void);

#endif

