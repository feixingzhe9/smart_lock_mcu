#include "lock.h"

struct lock_lock_ctrl_t lock_lock_ctrl;


LockClass lock_1(GPIOG, GPIO_Pin_5, 1, &lock_lock_ctrl);
LockClass lock_2(GPIOG, GPIO_Pin_4, 2, &lock_lock_ctrl);
LockClass lock_3(GPIOG, GPIO_Pin_3, 3, &lock_lock_ctrl);

//LockClass *lock_1 = new LockClass(GPIOG, GPIO_Pin_5);

void LockClass::lock_on(void)
{   
   GPIO_SetBits(this->lock_port, this->lock_pin);
}

void LockClass::lock_off(void)
{   
   GPIO_ResetBits(this->lock_port, this->lock_pin);
}


bool LockClass::search_unlock_array(u8 id)
{
    for(u8 i = 0; i < this->lock_lock_ctrl->to_unlock_cnt; i++)
    {
        if(this->lock_lock_ctrl->lock_array[i] == id)
        {
            return true;
        }
    }
    return false;
}

void LockClass::start_to_unlock(void)
{
    if(this->search_unlock_array(this->my_id) == false)
    {
        this->lock_lock_ctrl->lock_array[this->lock_lock_ctrl->to_unlock_cnt] = this->my_id;
        this->lock_lock_ctrl->to_unlock_cnt++;
    }    
    this->is_need_to_unlock = true;
}

void LockClass::remove_first_unlock(void)
{
    if(this->lock_lock_ctrl->to_unlock_cnt > 0)
    {      
        this->lock_lock_ctrl->lock_array[0] = 0;
        for(u8 i = 1; i < this->lock_lock_ctrl->to_unlock_cnt; i++)
        {
            this->lock_lock_ctrl->lock_array[i - 1] = this->lock_lock_ctrl->lock_array[i];
        }
        this->lock_lock_ctrl->to_unlock_cnt--;
        this->lock_lock_ctrl->lock_array[this->lock_lock_ctrl->to_unlock_cnt] = 0;
        
    }
}

bool LockClass::is_to_my_turn(void)
{
    if(this->lock_lock_ctrl->to_unlock_cnt > 0)
    {
        if(this->lock_lock_ctrl->lock_array[0] == this->my_id)
        {
            return true;
        }
    }
    return false;
}


#define LOCK_CTRL_PERIOD        100/50
#define SELF_LOCK_TIME          1500/50
#define BETWEEN_LOCK_TIME       300/50
void LockClass::lock_task(u32 tick)
{
    if(true == this->is_to_my_turn())
    {
        if(false == this->self_lock)
        {
            if(true == this->is_need_to_unlock)
            {
                if(this->lock_period_start_tick == 0)
                {
                    this->lock_period_start_tick = tick;
                    this->lock_on();
                    this->lock_status = true;
                }
                       
                if(tick - this->lock_period_start_tick >= LOCK_CTRL_PERIOD)
                {
                    
                    this->lock_off();
                    
                    if(0 == this->between_lock_start_tick)
                    {
                        this->between_lock_start_tick = tick;
                    }
                    if(tick - this->between_lock_start_tick >= BETWEEN_LOCK_TIME)
                    {
                        this->remove_first_unlock();
                        this->lock_period_start_tick = 0;
                        this->is_need_to_unlock = false;
                        this->self_lock = true;
                        this->between_lock_start_tick = 0;
                    }
                    
                }
            }
        }            
    }
    
    if(true == this->self_lock)
    {
        if(0 == this->self_lock_start_tick)
        {
            this->self_lock_start_tick = tick;
        }
        if(tick - this->self_lock_start_tick >= SELF_LOCK_TIME)
        {
            this->self_lock = false;
            this->self_lock_start_tick = 0;
        }
    }       
}


void all_lock_task(u32 tick)
{
    lock_1.lock_task(tick);
    lock_2.lock_task(tick);
    lock_3.lock_task(tick);
}


void start_to_unlock_all(void)
{
    lock_1.start_to_unlock();   
    lock_2.start_to_unlock();   
    lock_3.start_to_unlock();
}
