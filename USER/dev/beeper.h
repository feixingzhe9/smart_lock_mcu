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


extern struct beeper_times_t beeper_times;

void beeper_task(void);

void beeper_init(u16 arr,u16 psc);

#endif

