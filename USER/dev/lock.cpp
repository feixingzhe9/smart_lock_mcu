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

void LockClass::start_to_unlock(void)
{
    this->is_need_to_unlock = true;
}


//#define LOCK_CTRL_PERIOD    100/SYSTICK_PERIOD
//void LockClass::lock_task(u32 tick)
//{
//    if(this->is_need_to_unlock == true)
//    {
//        if(this->start_tick == 0)
//        {
//            this->start_tick = get_tick();
//            this->lock_on();
//            this->lock_status = true;
//        }
//               
//        if(get_tick() - this->start_tick >= LOCK_CTRL_PERIOD)
//        {
//            this->start_tick = 0;
//            this->is_need_to_unlock = false;
//            this->lock_off();
//        }
//    }
//}



#define LOCK_CTRL_PERIOD    100/50
#define SELF_LOCK_TIME      3000/50
void LockClass::lock_task(u32 tick)
{
    if(false == this->self_lock)
    {
        if(true == this->is_need_to_unlock)
        {
            if(this->start_tick == 0)
            {
                this->start_tick = tick;
                this->lock_on();
                this->lock_status = true;
            }
                   
            if(tick - this->start_tick >= LOCK_CTRL_PERIOD)
            {
                this->start_tick = 0;
                this->is_need_to_unlock = false;
                this->lock_off();
                this->self_lock = true;
            }
        }
    }
    
    else
    {
        if(0 == this->start_tick)
        {
            this->start_tick = tick;
        }
        if(tick - this->start_tick >= SELF_LOCK_TIME)
        {
            this->self_lock = false;
            this->start_tick = 0;
        }
    }   
}


void all_lock_task(u32 tick)
{
    lock_1.lock_task(tick);
    lock_2.lock_task(tick);
    lock_3.lock_task(tick);
}
