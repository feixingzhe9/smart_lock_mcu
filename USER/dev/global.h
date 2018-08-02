#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "sys.h"

typedef struct
{
    GPIO_TypeDef* 				port; 
    uint16_t              pin_number;
} gpio_t;

#endif
