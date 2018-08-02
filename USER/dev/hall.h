#ifndef __HALL_H
#define	__HALL_H

#include "sys.h"	

#define HALL1  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)
#define HALL2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)

#define HALL1_ON			0x01
#define HALL2_ON			0x02

void hall_init();
void hall_task();
uint8_t is_hall_detected();

#endif

