#include "lock.h"

struct lock_lock_ctrl_t lock_lock_ctrl;

//gpio_t lock_1_gpio_out;
//gpio_t lock_1_gpio_in;

//gpio_t lock_2_gpio_out;
//gpio_t lock_2_gpio_in;

//gpio_t lock_3_gpio_out;
//gpio_t lock_3_gpio_in;

//void lock_gpio_parameter_init(void)
//{
//    lock_1_gpio_out.port = LOCK_1_OUT_PORT;
//    lock_1_gpio_out.pin = LOCK_1_OUT_PIN;

//    lock_2_gpio_out.port = LOCK_2_OUT_PORT;
//    lock_2_gpio_out.pin = LOCK_2_OUT_PIN;

//    lock_3_gpio_out.port = LOCK_3_OUT_PORT;
//    lock_3_gpio_out.pin = LOCK_3_OUT_PIN;

//    lock_1_gpio_in.port = LOCK_1_IN_PORT;
//    lock_1_gpio_in.pin = LOCK_1_IN_PIN;

//    lock_2_gpio_in.port = LOCK_2_IN_PORT;
//    lock_2_gpio_in.pin = LOCK_2_IN_PIN;

//    lock_3_gpio_in.port = LOCK_3_IN_PORT;
//    lock_3_gpio_in.pin = LOCK_3_IN_PIN;

//}


LockClass lock_1(LOCK_1_OUT_PORT, LOCK_1_OUT_PIN, LOCK_1_IN_PORT, LOCK_1_IN_PIN, 1, &lock_lock_ctrl);
LockClass lock_2(LOCK_2_OUT_PORT, LOCK_2_OUT_PIN, LOCK_2_IN_PORT, LOCK_2_IN_PIN, 2, &lock_lock_ctrl);
LockClass lock_3(LOCK_3_OUT_PORT, LOCK_3_OUT_PIN, LOCK_3_IN_PORT, LOCK_3_IN_PIN, 3, &lock_lock_ctrl);


LockClass::LockClass(GPIO_TypeDef*  out_port, uint16_t out_pin, GPIO_TypeDef* in_port, uint16_t in_pin, u8 id, struct lock_lock_ctrl_t * lock_ctrl)
{
    lock_out_port = out_port;
    lock_out_pin = out_pin;

    lock_in_port = in_port;
    lock_in_pin = in_pin;

    lock_lock_ctrl = lock_ctrl;

    if(id > 0)
    {
        my_id = id;
    }
    else
    {
        printf("fatal: lock id CAN NOT be 0 ! ! !");
    }
}

void LockClass::init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    //---- output GPIO init ----//
    if(lock_out_port == GPIOG)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = lock_out_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(lock_out_port, &GPIO_InitStructure);
    GPIO_ResetBits(lock_out_port,lock_out_pin);     // default value: reset

    //---- input GPIO init ----//
    if(lock_out_port == GPIOD)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = lock_in_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(lock_in_port, &GPIO_InitStructure);

    is_need_to_unlock = false;
    lock_status = false;
    lock_machine_state = 0;
    lock_period_start_tick = 0;
    self_lock_start_tick = 0;
    between_lock_start_tick = 0;

    self_lock = false;


    lock_lock_ctrl->lock_cnt++;
    lock_lock_ctrl->to_unlock_cnt = 0;
}

void LockClass::lock_on(void)
{
   GPIO_SetBits(this->lock_out_port, this->lock_out_pin);
}

void LockClass::lock_off(void)
{
   GPIO_ResetBits(this->lock_out_port, this->lock_out_pin);
}

uint8_t LockClass::get_lock_status(void)
{
    return GPIO_ReadInputDataBit(lock_in_port, lock_in_pin);
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

void lock_init(void)
{
    lock_1.init();
    lock_2.init();
    lock_3.init();
}
