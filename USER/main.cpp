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
    NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_1_init(9600);	 	//����1��ʼ��Ϊ9600
    uart_2_init(9600);      //����2��ʼ��Ϊ9600
    uart_3_init(9600);      //����3��ʼ��Ϊ9600   
    LED.led_init();			     //LED�˿ڳ�ʼ��
    rfid_init();
    i2c_init();    
    beeper_init(200,50);    
    tim2_int_init(499,7199);    // timer to control locks
    
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
        touch_key_task();
        all_qr_data_task();
        can_protocol();
        beeper_task();
        sys_indicator();               
    }
}

