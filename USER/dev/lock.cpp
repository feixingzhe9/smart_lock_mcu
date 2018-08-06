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


#define LOCK_CTRL_PERIOD    100/SYSTICK_PERIOD
void LockClass::lock_task(void)
{
    if(this->is_need_to_unlock == true)
    {
        if(this->start_tick == 0)
        {
            this->start_tick = get_tick();
            this->lock_on();
            this->lock_status = true;
        }
               
        if(get_tick() - this->start_tick >= LOCK_CTRL_PERIOD)
        {
            this->start_tick = 0;
            this->is_need_to_unlock = false;
            this->lock_off();
        }
    }
}

void all_lock_task(void)
{
    lock_1.lock_task();
    lock_2.lock_task();
    lock_3.lock_task();
}
