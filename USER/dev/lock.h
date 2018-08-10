#ifndef __LOCK__H_
#define __LOCK__H_

#include "sys.h"
#include <stdio.h>

#define LOCK_NUM_MAX        3
struct lock_lock_ctrl_t
{
    volatile u8 lock_array[LOCK_NUM_MAX];
    volatile u8 lock_cnt;
    volatile u8 to_unlock_cnt;
    u32 start_tick;
};

class LockClass 
{
    public:
        LockClass(GPIO_TypeDef* port, uint16_t pin, u8 id, struct lock_lock_ctrl_t * lock_ctrl)
        {
            lock_port = port;
            lock_pin = pin;
            lock_lock_ctrl = lock_ctrl;
            
            GPIO_InitTypeDef  GPIO_InitStructure;
        
            if(lock_port == GPIOG)
            {
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //使能PG端口时钟
            }
            
            GPIO_InitStructure.GPIO_Pin = lock_pin;				
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
            GPIO_Init(lock_port, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.9
            GPIO_ResetBits(lock_port,lock_pin);	        // default value: reset
            
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

        void lock_task(u32 tick);
        void start_to_unlock(void);
        
        volatile bool lock_status;
        
    private:
        GPIO_TypeDef*  lock_port;
        uint16_t lock_pin;
    
        volatile bool is_need_to_unlock;
        volatile u32 lock_period_start_tick;
        volatile u32 self_lock_start_tick;
        volatile u32 between_lock_start_tick;
    
        volatile bool self_lock;
    
        struct lock_lock_ctrl_t *lock_lock_ctrl;
        
        u8 my_id;
    
    #define LOCK_MACHINE_STATE_WAIT_FOR_MY_TURN     0
    #define LOCK_MACHINE_STATE_GET_TURN             1
    #define LOCK_MACHINE_STATE_WAIT_SELF_LOCK       2
    #define LOCK_MACHINE_STATE_LOCK_ON              3
    #define LOCK_MACHINE_STATE_LOCK_OFF             4
    #define LOCK_MACHINE_STATE_BETWEEN_LOCK_DELAY   5
    
        u8 lock_machine_state;
    
        void lock_on(void);
        void lock_off(void);
        void remove_first_unlock(void);
        bool is_to_my_turn(void);
        bool search_unlock_array(u8 id);
};


void all_lock_task(u32 tick);
void start_to_unlock_all(void);


extern LockClass lock_1;
extern LockClass lock_2;
extern LockClass lock_3;



#endif
