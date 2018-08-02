#ifndef __LED_H
#define __LED_H	 
	 
#include "sys.h"

class LEDClass {
public:
#if 0
#define LED0 PAout(8)// PA8
#define LED1 PDout(2)// PD2
#else
#define LED0 PCout(9)// PC11
#define LED1 PCout(9)// PC11
#endif
	void LED_Init(void);//≥ı ºªØ
};

extern LEDClass LED;

#endif
