#ifndef __BEEPER__H_
#define __BEEPER__H_
#include "stdio.h"	
#include "sys.h"

void beeper_task(u16 key_value);

void beeper_init(u16 arr,u16 psc);

#endif

