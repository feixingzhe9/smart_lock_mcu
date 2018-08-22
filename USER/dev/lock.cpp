#include "lock.h"

struct lock_lock_ctrl_t lock_lock_ctrl;


LockClass lock_1(GPIOG, GPIO_Pin_5, 1, &lock_lock_ctrl);
LockClass lock_2(GPIOG, GPIO_Pin_4, 2, &lock_lock_ctrl);
LockClass lock_3(GPIOG, GPIO_Pin_3, 3, &lock_lock_ctrl);

//LockClass *lock_1 = new LockClass(GPIOG, GPIO_Pin_5);


LockClass::LockClass(GPIO_TypeDef* port, uint16_t pin, u8 id, struct lock_lock_ctrl_t * lock_ctrl)
{
    lock_port = port;
    lock_pin = pin;
    lock_lock_ctrl = lock_ctrl;

    GPIO_InitTypeDef  GPIO_InitStructure;

    if(lock_port == GPIOG)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = lock_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(lock_port, &GPIO_InitStructure);
    GPIO_ResetBits(lock_port,lock_pin);     // default value: reset

    is_need_to_unlock = false;
    lock_status = false;
    lock_machine_state = 0;
    lock_period_start_tick = 0;
    self_lock_start_tick = 0;
    between_lock_start_tick = 0;

    self_lock = false;

    if(id > 0)
    {
        my_id = id;
    }
    else
    {
        printf("fatal: lock id CAN NOT be 0 ! ! !");
    }

    lock_lock_ctrl->lock_cnt++;
    lock_lock_ctrl->to_unlock_cnt = 0;
}

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

#define TIMER_TICK_PERIOD       50
#define LOCK_CTRL_PERIOD        100/TIMER_TICK_PERIOD
#define SELF_LOCK_TIME          1000/TIMER_TICK_PERIOD
#define BETWEEN_LOCK_TIME       300/TIMER_TICK_PERIOD
void LockClass::lock_task(u32 tick)
{
//    if(true == this->is_to_my_turn())
//    {
//        if(false == this->self_lock)
//        {
//            if(true == this->is_need_to_unlock)
//            {
//                if(this->lock_period_start_tick == 0)
//                {
//                    this->lock_period_start_tick = tick;
//                    this->lock_on();
//                    this->lock_status = true;
//                }
//
//                if(tick - this->lock_period_start_tick >= LOCK_CTRL_PERIOD)
//                {
//                    this->lock_off();
//
//                    if(0 == this->between_lock_start_tick)
//                    {
//                        this->between_lock_start_tick = tick;
//                    }
//                    if(tick - this->between_lock_start_tick >= BETWEEN_LOCK_TIME)
//                    {
//                        this->remove_first_unlock();
//                        this->lock_period_start_tick = 0;
//                        this->is_need_to_unlock = false;
//                        this->self_lock = true;
//                        this->between_lock_start_tick = 0;
//                    }
//                }
//            }
//        }
//    }
//
//    if(true == this->self_lock)
//    {
//        if(0 == this->self_lock_start_tick)
//        {
//            this->self_lock_start_tick = tick;
//        }
//        if(tick - this->self_lock_start_tick >= SELF_LOCK_TIME)
//        {
//            this->self_lock = false;
//            this->self_lock_start_tick = 0;
//        }
//    }


    switch(this->lock_machine_state)
    {
        case LOCK_MACHINE_STATE_WAIT_FOR_MY_TURN:
            {
                if( true == this->is_to_my_turn() )
                {
                    this->lock_machine_state = LOCK_MACHINE_STATE_WAIT_SELF_LOCK;
                }
                else
                {
                    break;
                }
            }

        case LOCK_MACHINE_STATE_WAIT_SELF_LOCK:
            {
                if(false == this->self_lock)
                {
                    this->lock_machine_state = LOCK_MACHINE_STATE_LOCK_ON;
                }
                else
                {
                    break;
                }
            }

        case LOCK_MACHINE_STATE_LOCK_ON:
            {
                this->lock_on();
                this->lock_period_start_tick = tick;
                this->lock_status = true;
                if(0 == this->between_lock_start_tick)
                {
                    this->between_lock_start_tick = tick;
                }
                this->lock_machine_state = LOCK_MACHINE_STATE_LOCK_OFF;
                //break;
            }

        case LOCK_MACHINE_STATE_LOCK_OFF:
            {
                if(tick - this->lock_period_start_tick >= LOCK_CTRL_PERIOD)
                {
                    this->lock_off();
                    this->self_lock = true;
                    this->lock_machine_state = LOCK_MACHINE_STATE_BETWEEN_LOCK_DELAY;
                    if(0 == this->between_lock_start_tick)
                    {
                        this->between_lock_start_tick = tick;
                    }
                }
                else
                {
//                    break;
                }
                break;
            }

        case LOCK_MACHINE_STATE_BETWEEN_LOCK_DELAY:
            {
                if(tick - this->between_lock_start_tick >= BETWEEN_LOCK_TIME)
                {
                    this->remove_first_unlock();
                    this->lock_period_start_tick = 0;
                    this->is_need_to_unlock = false;

                    this->between_lock_start_tick = 0;
                    this->lock_machine_state = LOCK_MACHINE_STATE_WAIT_FOR_MY_TURN;
                }
                else
                {
                    break;
                }
            }

        default :
            break;

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
