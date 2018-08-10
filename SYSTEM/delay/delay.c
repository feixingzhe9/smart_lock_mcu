
#include "delay.h"
#include "sys.h"

void delay_ms(uint16_t xms)
{
    uint32_t tickstart = 0;
    tickstart = get_tick();
    while((get_tick() - tickstart) < xms)
    {
        
    }
}


