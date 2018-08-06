#ifndef __LOCK__H_
#define __LOCK__H_

#include "sys.h"

void lock_task(void);

class LockClass 
{
    public:
        LockClass(GPIO_TypeDef* port, uint16_t pin)
        {
            lock_port = port;
            lock_pin = pin;
            
            GPIO_InitTypeDef  GPIO_InitStructure;
        
            if(lock_port == GPIOG)
            {
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PG�˿�ʱ��
            }
            
            GPIO_InitStructure.GPIO_Pin = lock_pin;				
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
            GPIO_Init(lock_port, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.9
            GPIO_ResetBits(lock_port,lock_pin);	        // default value: reset
            
            
            is_need_to_unlock = false;
            lock_status = false;
            start_tick = 0;
        }

        void lock_on(void);
        void lock_off(void);
        void lock_task(void);
            
        GPIO_TypeDef*  lock_port;
        uint16_t lock_pin;
        u32 start_tick;
        bool is_need_to_unlock;
        bool lock_status;
};


void all_lock_task(void);

extern LockClass lock_1;
extern LockClass lock_2;
extern LockClass lock_3;
#endif
