#include "lock.h"


LockClass lock_1(GPIOG, GPIO_Pin_5);
LockClass lock_2(GPIOG, GPIO_Pin_4);
LockClass lock_3(GPIOG, GPIO_Pin_3);

//LockClass *lock_1 = new LockClass(GPIOG, GPIO_Pin_5);

void LockClass::lock_on(void)
{   
   GPIO_SetBits(this->lock_port, this->lock_pin);
}

void LockClass::lock_off(void)
{   
   GPIO_ResetBits(this->lock_port, this->lock_pin);
}


void lock_task(void)
{
    
}
