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
#include "motor.h"

//#define CAN_LOAD_TEST

#ifdef CAN_LOAD_TEST
void can_bus_load_test(void);
#endif
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

    motor_init();

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
        motor_task();
        sys_indicator();

#ifdef CAN_LOAD_TEST
        can_bus_load_test();
#endif
    }
}



#ifdef CAN_LOAD_TEST
void can_bus_load_test(void)
{
    static uint32_t test_cnt = 0;
    uint8_t data_1[] = "0000abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890! test 2";
    uint8_t data_2[] = "00001234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz!test 1";

    can_id_union id;
    id.can_id_struct.src_mac_id = LOCK_CAN_MAC_SRC_ID;
    id.can_id_struct.source_id = CAN_SOURCE_ID_CAN_LOAD_TEST;
    id.can_id_struct.res = 0;
    id.can_id_struct.ack = 0;
    id.can_id_struct.func_id = 0;

    while(1)
    {
        if(test_cnt % 2)
        {
            memset(&data_1[0], 0, 4);
            *(uint32_t *)&(data_1[0]) = test_cnt;
            Can1_TX( id.can_id, data_1, sizeof(data_1));
        }
        else
        {
            memset(&data_2[0], 0, 4);
            *(uint32_t *)&(data_2[0]) = test_cnt;
            Can1_TX( id.can_id, data_2, sizeof(data_2));
        }
//        delay_ms(2);
        test_cnt++;
    }
}
#endif
