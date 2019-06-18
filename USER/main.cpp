#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "rfid.h"
//#include "can_interface.h"
#include "stmflash.h"
#include "myiic.h"
#include "cp2532.h"
#include "timer.h"
#include "beeper.h"
#include "lock.h"
#include "qr_code.h"
#include "motor.h"
#include <ucos_ii.h>
#include "common.h"
#include "platform.h"


//static void init()
//{
//    sys_tick_init();
//    NVIC_Configuration();       //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
////    qr_code_init();
//    rfid_init();

//    beeper_init(200,50);
//    lock_init();
//    tim2_int_init(499,7199);    // timer to control locks

//#ifdef CP2532_INT_ENABLE
////    cp2532_init();
//#else
////    i2c_init();
//#endif

////    motor_init();

//    get_rfid_in_flash(rfid_in_flash);
////    get_password_in_flash(psss_word_in_flash);

//    return;
//}

int main(void)
{
//    OSInit();
//    init();

//    while(1)
//    {
//        rfid_task();

//#ifndef CP2532_INT_ENABLE
////        touch_key_task();
//#endif
////        all_qr_data_task();
//        //can_protocol();
//        beeper_task();
////        motor_task();
//        lock_input_status_task();
//        //sys_indicator();
//        OSStart();
//    }

    OSInit();
    delay_init();
    NVIC_Configuration();
    __disable_irq();
    hardware_init();
    user_param_init();
    user_init();
    __enable_irq();
    OSStart();
}

