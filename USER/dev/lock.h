#ifndef __LOCK__H_
#define __LOCK__H_

#include "sys.h"
#include <stdio.h>
#include "lock_param.h"


//struct gpio_t
//{
//    GPIO_TypeDef* port;
//    uint16_t pin;
//};


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
        void lock_task(u32 tick);
        void start_to_unlock(void);
        void init(void);
        uint8_t get_lock_status(void);

        volatile bool lock_status;

//        LockClass(gpio_t gpio_out, gpio_t gpio_in, u8 id, struct lock_lock_ctrl_t * lock_ctrl);
        LockClass(uint8_t id, struct lock_lock_ctrl_t * lock_ctrl);

    private:
//        GPIO_TypeDef*  lock_out_port;
//        uint16_t lock_out_pin;

//        GPIO_TypeDef*  lock_in_port;
//        uint16_t lock_in_pin;

//        gpio_t lock_out_gpio;
//        gpio_t lock_in_gpio;

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
void lock_init(void);
void lock_in_status_task(void);

extern LockClass lock_1;
extern LockClass lock_2;
extern LockClass lock_3;





#endif
