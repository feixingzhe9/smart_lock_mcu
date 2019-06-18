#include "lock.h"
#include "lock_param.h"
//#include "can_interface.h"

struct lock_lock_ctrl_t lock_lock_ctrl;


LockClass lock_1(1, &lock_lock_ctrl);
LockClass lock_2(2, &lock_lock_ctrl);
LockClass lock_3(3, &lock_lock_ctrl);
LockClass lock_4(4, &lock_lock_ctrl);


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

    debounce_cnt = 0;
    pre_lock_status = 0;
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


    //---- input GPIO config ----//
    if(gpio_in_int_param[my_id - 1].in_port == GPIOD)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = gpio_in_int_param[my_id - 1].in_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(gpio_in_int_param[my_id - 1].in_port, &GPIO_InitStructure);

#if 0
    //---- lock input interrupt config ----//
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if(gpio_in_int_param[my_id - 1].in_port == GPIOD)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);   //使能时钟
    }

    GPIO_InitStructure.GPIO_Pin = gpio_in_int_param[my_id - 1].in_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//
    GPIO_Init(gpio_in_int_param[my_id - 1].in_port, &GPIO_InitStructure);  //

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(gpio_in_int_param[my_id - 1].gpio_port_int_source,gpio_in_int_param[my_id - 1].gpio_pin_int_source);

    exit_init_structure.EXTI_Line=gpio_in_int_param[my_id - 1].exit_line;
    exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exit_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exit_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exit_init_structure);        //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = gpio_in_int_param[my_id - 1].nvic_irq_channel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;    //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;           //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif


    is_need_to_unlock = false;
    current_lock_input_status = 0;
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

uint8_t LockClass::get_lock_status_debounce(void)
{
    if(previous_state != this->get_lock_status())
    {
        this->debounce_cnt = 0;
    }
    else
    {
        debounce_cnt++;
    }

    previous_state = this->get_lock_status();

    if(debounce_cnt >= LOCK_IN_STATE_DEBOUNCE_CNT)
    {
        debounce_cnt = LOCK_IN_STATE_DEBOUNCE_CNT;
        return this->get_lock_status();
    }

    return 0xff;        //unstable state
}

uint8_t LockClass::is_lock_input_status_changed(void)
{
    uint8_t lock_status = 0;
    uint8_t lock_status_tmp = 0;
    uint8_t ret = 0xff;

//    if(lock_lock_ctrl->to_unlock_cnt == 0)
    {
        lock_status_tmp = this->get_lock_status_debounce();

        if(lock_status_tmp != 0xff)  //normal status
        {
            lock_status = lock_status_tmp;
            if(lock_status != pre_lock_status)
            {
                ret = lock_status;
                current_lock_input_status = lock_status;
            }
            pre_lock_status = lock_status;
        }
    }
    return ret;
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
                //this->current_lock_input_status = true;
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


static void upload_lock_status(uint8_t *lock_status)
{
//    can_id_union id;

//    id.can_id_struct.source_id = CAN_SOURCE_ID_LOCK_STATUS_UPLOAD;

//    id.can_id_struct.src_mac_id = LOCK_CAN_MAC_SRC_ID;

//    id.can_id_struct.res = 0;
//    id.can_id_struct.ack = 0;
//    id.can_id_struct.func_id = 0;

//    Can1_TX(id.can_id, lock_status, LOCK_NUM_MAX);
}

//uint32_t lock_status_change_start_tick = 0;
//uint8_t is_lock_status_changed = 0;
//#define LOCK_IN_STATUS_STABLE_DELAY_TICK    100/SYSTICK_PERIOD

#define LOCK_INPUT_DETECTION_PERIOD     10/SYSTICK_PERIOD
void lock_input_status_task(void)
{
    uint8_t lock_status[LOCK_NUM_MAX] = {0};
    uint8_t lock_status_tmp[LOCK_NUM_MAX] = {0};
    bool lock_status_changed_flag = false;
    static uint32_t start_tick = 0;

    if(get_tick() - start_tick >= LOCK_INPUT_DETECTION_PERIOD)
    {
        start_tick = get_tick();
        lock_status_tmp[LOCK_1] = lock_1.is_lock_input_status_changed();
        lock_status_tmp[LOCK_2] = lock_2.is_lock_input_status_changed();
        lock_status_tmp[LOCK_3] = lock_3.is_lock_input_status_changed();
        lock_status_tmp[LOCK_4] = lock_3.is_lock_input_status_changed();

        for(uint8_t i = LOCK_1; i < LOCK_NUM_MAX; i++)
        {
            if(lock_status_tmp[i] != 0xff)
            {
                lock_status_changed_flag = true;
                break;
            }
        }

        if(true == lock_status_changed_flag)
        {
            lock_status[LOCK_1] = lock_1.current_lock_input_status;
            lock_status[LOCK_2] = lock_2.current_lock_input_status;
            lock_status[LOCK_3] = lock_3.current_lock_input_status;
            lock_status[LOCK_4] = lock_3.current_lock_input_status;
            upload_lock_status(lock_status);
        }
    }
}


//Execution in timer interruption
void all_lock_task(u32 tick)
{
    lock_1.lock_task(tick);
    lock_2.lock_task(tick);
    lock_3.lock_task(tick);
    lock_4.lock_task(tick);
}


uint32_t unlock_start_tick = 0;
void start_to_unlock(uint32_t lock)
{
    if(0 == lock)
    {
        return ;
    }

//    if(lock > (1 << LOCK_NUM_MAX) - 1)
//    {
//        return ;
//    }

    unlock_start_tick = get_tick();
    if(lock & (1 << 0))
    {
        lock_1.start_to_unlock();
    }
    if(lock & (1 << 1))
    {
        lock_2.start_to_unlock();
    }
    if(lock & (1 << 2))
    {
        lock_3.start_to_unlock();
    }
    if(lock & (1 << 3))
    {
        lock_4.start_to_unlock();
    }

}


void lock_init(void)
{
    lock_1.init();
    lock_2.init();
    lock_3.init();
    lock_4.init();
}
