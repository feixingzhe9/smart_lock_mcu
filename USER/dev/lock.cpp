#include "lock.h"
#include "lock_param.h"

struct lock_lock_ctrl_t lock_lock_ctrl;


LockClass lock_1(1, &lock_lock_ctrl);
LockClass lock_2(2, &lock_lock_ctrl);
LockClass lock_3(3, &lock_lock_ctrl);


LockClass::LockClass(uint8_t id, struct lock_lock_ctrl_t * lock_ctrl)
{

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
    if(gpio_in_int_param[my_id - 1].out_port == GPIOG)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = gpio_in_int_param[my_id - 1].out_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gpio_in_int_param[my_id - 1].out_port, &GPIO_InitStructure);
    GPIO_ResetBits(gpio_in_int_param[my_id - 1].out_port,gpio_in_int_param[my_id].out_pin);     // default value: reset

    //---- input GPIO init ----//
    if(gpio_in_int_param[my_id - 1].in_port == GPIOD)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = gpio_in_int_param[my_id - 1].in_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(gpio_in_int_param[my_id - 1].in_port, &GPIO_InitStructure);

    //---- lock input interrupt config ----//
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;


    if(gpio_in_int_param[my_id - 1].in_port == GPIOD)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能时钟
    }

    GPIO_InitStructure.GPIO_Pin = gpio_in_int_param[my_id - 1].in_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
    GPIO_Init(gpio_in_int_param[my_id - 1].in_port, &GPIO_InitStructure);  //

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(gpio_in_int_param[my_id - 1].gpio_port_int_source,gpio_in_int_param[my_id - 1].gpio_pin_int_source);

    exit_init_structure.EXTI_Line=gpio_in_int_param[my_id - 1].exit_line;
    exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exit_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exit_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exit_init_structure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = gpio_in_int_param[my_id - 1].nvic_irq_channel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


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
   GPIO_SetBits(gpio_in_int_param[my_id - 1].out_port, gpio_in_int_param[my_id - 1].out_pin);
}

void LockClass::lock_off(void)
{
   GPIO_ResetBits(gpio_in_int_param[my_id - 1].out_port, gpio_in_int_param[my_id - 1].out_pin);
}

uint8_t LockClass::get_lock_status(void)
{
    return GPIO_ReadInputDataBit(gpio_in_int_param[my_id - 1].in_port, gpio_in_int_param[my_id - 1].in_pin);
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
