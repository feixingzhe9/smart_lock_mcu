#include "sys.h"


uint32_t sys_err = 0;

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

void sys_tick_init(void)
{
    uint32_t ticks = SystemCoreClock/8000;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      // set reload register
    //NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  // set Priority for Cortex-M3 System Interrupts
    SysTick->VAL   = 0;                                          // Load the SysTick Counter Value
    SysTick->CTRL  = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;   // Enable SysTick IRQ and SysTick Timer
}

volatile uint32_t sys_tick_cnt = 0;

//uint32_t get_tick(void)
//{
//    return sys_tick_cnt;
//}

void platform_mcu_reset(void)
{
    NVIC_SystemReset();
}


