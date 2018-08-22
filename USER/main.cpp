#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "rfid.h"
#include "can_interface.h"
#include "stmflash.h"
#include "myiic.h"
#include "cp2532.h"
#include "timer.h"
#include "beeper.h"
#include "lock.h"
#include "qr_code.h"

static void init()
{
    sys_tick_init();
    NVIC_Configuration();       //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    qr_code_init();
    rfid_init();

    i2c_init();
    beeper_init(200,50);
    tim2_int_init(499,7199);    // timer to control locks

#ifdef CP2532_INT_ENABLE
    cp2532_init();
#endif

    get_rfid_in_flash(rfid_in_flash);
    get_password_in_flash(psss_word_in_flash);

    return;
}

int main(void)
{
    init();

    while(1)
    {
        rfid_task();

#ifndef CP2532_INT_ENABLE
        touch_key_task();
#endif
        all_qr_data_task();
        can_protocol();
        beeper_task();
        sys_indicator();
    }
}

