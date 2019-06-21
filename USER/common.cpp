/*
 *  Author: Kaka Xie
 *  brief: user configuration of ucosii
 */

#include "common.h"

static void task_create(void)
{
    OSTaskCreate(can_protocol_task,                 (void *)0,  (OS_STK*)&can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE - 1],                    CAN_RPOTOCOL_TASK_PRIO);
    OSTaskCreate(can_send_task,                     (void *)0,  (OS_STK*)&can_send_task_stk[CAN_SEND_TASK_STK_SIZE - 1],                            CAN_SEND_TASK_PRIO);
    OSTaskCreate(lock_task,                         (void *)0,  (OS_STK*)&lock_task_stk[LOCK_TASK_STK_SIZE - 1],                                    LOCK_TASK_PRIO);
    OSTaskCreate(indicator_led_task,                (void *)0,  (OS_STK*)&indicator_led_task_stk[INDICATOR_LED_STK_SIZE - 1],                       INDICATOR_LED_TASK_PRIO);
}

static void sem_create(void)
{

}

static int mailbox_create(void)
{
    return 0;
}

static int mem_create(void)
{
    uint8_t err = 0;
    can_send_buf_mem_handle = OSMemCreate((void *)&can_send_buf_mem[0][0], sizeof(can_send_buf_mem) / sizeof(can_send_buf_mem[0]), sizeof(can_buf_t), &err);
    if(can_send_buf_mem_handle == 0)
    {
        /*
        todo: err process
        */
//        return -1;
    }

    can_rcv_buf_mem_handle = OSMemCreate((void *)&can_rcv_buf_mem[0][0], sizeof(can_rcv_buf_mem) / sizeof(can_rcv_buf_mem[0]), sizeof(can_pkg_t), &err);
    if(can_rcv_buf_mem_handle == 0)
    {
        /*
        todo: err process
        */
//        return -1;
    }

    can_upload_ack_mem_handle = OSMemCreate((void *)&can_upload_ack_mem[0][0], sizeof(can_upload_ack_mem) / sizeof(can_upload_ack_mem[0]), sizeof(can_upload_ack_t), &err);
    if(can_upload_ack_mem_handle == 0)
    {
        /*
        todo: err process
        */
//        return -1;
    }

    return 0;
}

static int queue_create(void)
{
    can_send_buf_queue_handle = OSQCreate(&can_send_buf_queue_p[0], CAN_SEND_BUF_QUEUE_NUM);
    if(can_send_buf_queue_handle == 0)
    {
        /*
        todo: err process
        */
//        return -1;
    }

    can_rcv_buf_queue_handle = OSQCreate(&can_rcv_buf_queue_p[0], CAN_RCV_BUF_QUEUE_NUM);
    if(can_rcv_buf_queue_handle == 0)
    {
        /*
        todo: err process
        */
//        return -1;
    }

    can_upload_ack_queue_handle = OSQCreate(&can_upload_ack_queue_p[0], CAN_UPLOAD_ACK_QUEUE_NUM);
    if(can_rcv_buf_queue_handle == 0)
    {
        /*
        todo: err process
        */
//        return -1;
    }

    return 0;
}

static void os_user_config(void)
{
    sem_create();
    mailbox_create();
    mem_create();
    queue_create();
    task_create();
}

static void user_init_depend_on_os_config(void)
{

}

void user_init(void)
{
    os_user_config();
    user_init_depend_on_os_config();
}

