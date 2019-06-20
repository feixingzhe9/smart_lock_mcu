/**
******************************************************************************
* @file    can.c
* @author  Kaka.Xie
* @brief   This file provides CAN protocol functions.
******************************************************************************

******************************************************************************
*/
#include "can_protocol_task.h"
#include "delay.h"
#include "platform.h"
#include <string.h>
#include <stdio.h>
#include "can.h"
#include "can_fifo.h"
#include "rfid.h"



CanRxMsg RxMessage;
CanTxMsg TxMessage;
uint8_t CanTxdataBuff[CAN_LONG_FRAME_LENTH_MAX] = {0};


can_buf_t can_send_buf_mem[CAN_SEND_BUF_SIZE][1] = {{0}};
OS_MEM *can_send_buf_mem_handle;

OS_EVENT *can_send_buf_queue_handle;
void* can_send_buf_queue_p[CAN_SEND_BUF_QUEUE_NUM] = {0};


can_pkg_t can_rcv_buf_mem[CAN_RCV_BUF_SIZE][1] = {{0}};
OS_MEM *can_rcv_buf_mem_handle;

OS_EVENT *can_rcv_buf_queue_handle;
void* can_rcv_buf_queue_p[CAN_RCV_BUF_QUEUE_NUM] = {0};


can_upload_ack_t can_upload_ack_mem[CAN_UPLOAD_ACK_SIZE][1] = {{0}};
OS_MEM *can_upload_ack_mem_handle;

OS_EVENT *can_upload_ack_queue_handle;
void* can_upload_ack_queue_p[CAN_UPLOAD_ACK_QUEUE_NUM] = {0};

#define ONLYONCE       0x00
#define BEGIN         0x01
#define TRANSING       0x02
#define END            0x03


void Can1_TX(uint32_t canx_id,uint8_t* pdata,uint16_t len)
{
    //return ;
    uint16_t t_len;
    uint16_t roundCount;
    uint8_t modCount;
    can_data_union TxMsg = {0};
    //CanTxMsgTypeDef *TxMessage = platform_can_drivers[can_type].handle->pTxMsg;

    t_len = len;
    roundCount = t_len/7;
    modCount = t_len%7;

    TxMessage.ExtId = canx_id;
    TxMessage.IDE   = CAN_ID_EXT;
    TxMessage.RTR   = CAN_RTR_DATA;
    //if(roundCount <= 1)
    if(t_len <= 7)
    {
        TxMsg.can_data_t.seg_polo = ONLYONCE;
        TxMessage.DLC = t_len+1;

        memcpy(&TxMessage.Data[1],pdata,t_len);
        TxMessage.Data[0] = TxMsg.can_data[0];

        if((CAN_USED->TSR&0x1C000000))
        {
            CAN_Transmit(CAN1, &TxMessage);//
            delay_ms(10);
        }
        else
        {
            printf("TX busy ! \r\n");
        }
        return ;
    }

    {
        int num;
        {
            for(num = 0; num < roundCount; num++)
            {
                //SET SEGPOLO
                if( num == 0)
                {
                    TxMsg.can_data_t.seg_polo = BEGIN;
                }
                else
                {
                    TxMsg.can_data_t.seg_polo = TRANSING;
                }

                if( modCount == 0 && num == roundCount-1)
                {
                    TxMsg.can_data_t.seg_polo = END;
                }

                TxMsg.can_data_t.seg_num = num;
                memcpy(TxMsg.can_data_t.data, &pdata[num*7], 7);
                memcpy(TxMessage.Data, TxMsg.can_data, 8);
                TxMessage.DLC = 8;
                if((CAN_USED->TSR&0x1C000000))
                {
                    CAN_Transmit(CAN1, &TxMessage);//
                    delay_ms(10);
                }
                else
                {
                    printf("TX busy ! \r\n");
                }

                //TRANSMIT LAST MSG
                if( modCount !=0 && num == roundCount-1 )
                {
                    num++;
                    TxMsg.can_data_t.seg_polo = END;
                    TxMsg.can_data_t.seg_num = num;
                    memcpy(TxMsg.can_data_t.data,&pdata[num*7],modCount);
                    memcpy(TxMessage.Data,TxMsg.can_data,modCount+1);
                    TxMessage.DLC = modCount+1;
                    if((CAN_USED->TSR&0x1C000000))
                    {
                        CAN_Transmit(CAN1, &TxMessage);//
                        delay_ms(10);
                    }
                    else
                    {
                        printf("TX busy ! \r\n");
                    }
                }
            }
        }
    }
}


extern uint16_t rfid_src_mac_id;
void upload_rfid_data(const uint8_t *buffer_key)
{
    can_id_union id;
    can_buf_t can_buf;
    id.can_id_t.ack = 0;
    id.can_id_t.dest_mac_id = 0;////
    id.can_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.can_id_t.source_id = CAN_SOURCE_ID_RFID_UPLOAD;
    id.can_id_t.src_mac_id = rfid_src_mac_id;////
    can_buf.id = id.canx_id;
    can_buf.data_len = 4;
    memcpy(&can_buf.data[0], buffer_key, 4);
    send_can_msg(&can_buf);
}




uint16_t CmdProcessing(can_id_union *id, uint8_t *data_in, uint16_t data_in_len, uint8_t *data_out)
{
    id->can_id_t.ack = 1;
    id->can_id_t.dest_mac_id = id->can_id_t.src_mac_id;
    id->can_id_t.src_mac_id = rfid_src_mac_id;
    id->can_id_t.res = 0;
    switch(id->can_id_t.func_id)
    {
        case CAN_FUN_ID_RESET:
            mcu_restart();
            break;
        case CAN_FUN_ID_WRITE:
        case CAN_FUN_ID_READ:
            switch(id->can_id_t.source_id)
            {
                case CAN_SOURCE_ID_READ_VERSION:
                    data_out[0] = data_in[0];
                    data_out[1] = data_in[1];
                    if(data_in[1] == 1)//read software version
                    {
                        memcpy(&data_out[3], SW_VERSION, sizeof(SW_VERSION));
                        //return strlen(SW_VERSION) + 1;
                        data_out[2] = strlen(SW_VERSION);
                        return sizeof(SW_VERSION) + 3;
                    }
                    else if(data_in[1] == 2)//protocol version
                    {
                        memcpy(&data_out[3], PROTOCOL_VERSION, sizeof(PROTOCOL_VERSION));
                        data_out[2] = strlen(PROTOCOL_VERSION);
                        return sizeof(PROTOCOL_VERSION) + 3;

                    }
                    else if(data_in[1] == 3)//hardware version
                    {
                        memcpy(&data_out[3], HW_VERSION, strlen(HW_VERSION));
                        data_out[2] = strlen(HW_VERSION);
                        return strlen(HW_VERSION) + 3;
                    }
                    return CMD_NOT_FOUND;


                default :
                    break;
            }

        default:
            break;
    }
    return 0;
}

can_long_buf_t can_long_frame_buf_ram;
can_long_buf_t *can_long_frame_buf = &can_long_frame_buf_ram;

#define CAN_LONG_BUF_FULL   0xff
static uint8_t get_one_free_buf(void)
{
    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
    {
        if(can_long_frame_buf->can_rcv_buf[i].used_len == 0)
        {
            return i;
        }
    }
    return CAN_LONG_BUF_FULL;
}
static void free_buf(uint8_t index)
{
    can_long_frame_buf->can_rcv_buf[index].can_id = 0;
    can_long_frame_buf->can_rcv_buf[index].used_len = 0;
}
#define CAN_BUF_NO_THIS_ID      0xfe
static uint8_t get_the_buf_by_id(uint32_t id)
{
    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
    {
        if(id == can_long_frame_buf->can_rcv_buf[i].can_id)
        {
            return i;
        }
    }
    return CAN_BUF_NO_THIS_ID;
}


void can_long_buf_init(void)
{
    can_long_frame_buf->get_one_free_buf = get_one_free_buf;
    can_long_frame_buf->get_the_buf_by_id = get_the_buf_by_id;
    can_long_frame_buf->free_buf = free_buf;
}


#define CAN_LONG_FRAME_TIME_OUT     5 * OS_TICKS_PER_SEC
#define CAN_COMM_TIME_OUT           5 * OS_TICKS_PER_SEC
uint32_t can_comm_start_time;

#define CAN_COM_TIME_OUT    5 * OS_TICKS_PER_SEC



OS_STK can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE];

void can_protocol_task(void *pdata)
{
    can_id_union id = {0};
    can_data_union rx_buf = {0};
    can_buf_t can_buf;
    can_pkg_t *can_rcv_buf;
    can_upload_ack_t *can_upload_ack;
    uint8_t err = 0;
    delay_ms(500);
    while(1)
    {
        uint8_t buf_index = 0;
        uint8_t seg_polo = 0;
        uint8_t seg_num = 0;
        uint16_t tx_len = 0;
        uint8_t rx_len = 0;

        can_rcv_buf = (can_pkg_t *)OSQPend(can_rcv_buf_queue_handle, 5 * OS_TICKS_PER_SEC, &err);
        if(err == OS_ERR_NONE)
        {
            memcpy(rx_buf.can_data,  can_rcv_buf->data.can_data, can_rcv_buf->len);
            id.canx_id = can_rcv_buf->id.canx_id;
            rx_len = can_rcv_buf->len;
            seg_polo = rx_buf.can_data_t.seg_polo;
            seg_num = rx_buf.can_data_t.seg_num;
            OSMemPut(can_rcv_buf_mem_handle, can_rcv_buf);
            if(id.can_id_t.dest_mac_id == rfid_src_mac_id)
            {
                if(id.can_id_t.ack == 1)
                {
                    can_upload_ack = (can_upload_ack_t *)OSMemGet(can_upload_ack_mem_handle, &err);
                    if(err == OS_ERR_NONE)
                    {
                        can_upload_ack->id.canx_id = id.canx_id;
                        if(rx_len >= 2)
                        {
                            can_upload_ack->serial_num = rx_buf.can_data[rx_len - 1];
                            OSQPost(can_upload_ack_queue_handle, (void *)can_upload_ack);
                        }
                    }

                }
                else
                {
                    if(rx_buf.can_data_t.seg_polo == ONLYONCE)
                    {
                        //if((id.canx_id_t.source_id < SOURCE_ID_PREPARE_UPDATE) && (id.canx_id_t.source_id > SOURCE_ID_CHECK_TRANSMIT))
                        {
                            //process the data here//
                            tx_len = CmdProcessing(&id, rx_buf.can_data_t.data, rx_len - 1, CanTxdataBuff);
                            //process the data here//
                            if(tx_len > 0)
                            {
                                //Can1_TX(id.canx_id, CanTxdataBuff, tx_len);
                                can_buf.data_len = tx_len;
                                can_buf.id = id.canx_id;
                                memcpy(can_buf.data, CanTxdataBuff, tx_len);
                                send_can_msg(&can_buf);
                            }

                            //CanTX(MICO_CAN1, id.canx_id, test_data, sizeof(test_data));
                        }
                    }
                    else //long frame
                    {
                        for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
                        {
                            if(can_long_frame_buf->can_rcv_buf[i].used_len > 0)
                            {
                                if(get_tick() - can_long_frame_buf->can_rcv_buf[i].start_time > CAN_LONG_FRAME_TIME_OUT)
                                {
                                    can_long_frame_buf->free_buf(i);
                                    printf("LONG FRAME RCV TIMEOUT! ! ! !\r\n");
                                }
                            }
                        }

                        if(seg_polo == BEGIN)
                        {
                            buf_index = can_long_frame_buf->get_the_buf_by_id(id.canx_id);
                            if(buf_index == CAN_BUF_NO_THIS_ID)
                            {
                                buf_index = can_long_frame_buf->get_one_free_buf();
                            }
                            else
                            {
                                //
                            }

                            if((buf_index == CAN_LONG_BUF_FULL) || (buf_index >= CAN_LONG_BUF_NUM))
                            {
                                printf("LONG FRAME RCV BUF IS FULL! ! ! !\r\n");
                                goto exit;
                            }
                            memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[0], rx_buf.can_data_t.data, CAN_ONE_FRAME_DATA_LENTH);
                            can_long_frame_buf->can_rcv_buf[buf_index].used_len = CAN_ONE_FRAME_DATA_LENTH;
                            can_long_frame_buf->can_rcv_buf[buf_index].can_id = id.canx_id;
                            can_long_frame_buf->can_rcv_buf[buf_index].start_time = get_tick();
                        }
                        else if((seg_polo == TRANSING) || (seg_polo == END))
                        {
                            buf_index = can_long_frame_buf->get_the_buf_by_id(id.canx_id);
                            if((buf_index == CAN_BUF_NO_THIS_ID) || (buf_index >= CAN_LONG_BUF_NUM))
                            {
                                printf("ERROR ! !\r\n");
                                goto exit;
                            }
                            can_long_frame_buf->can_rcv_buf[buf_index].start_time = get_tick();
                            if(seg_polo == TRANSING)
                            {
                                memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num * CAN_ONE_FRAME_DATA_LENTH], rx_buf.can_data_t.data, CAN_ONE_FRAME_DATA_LENTH);
                                can_long_frame_buf->can_rcv_buf[buf_index].used_len += CAN_ONE_FRAME_DATA_LENTH;
                            }
                            if(seg_polo == END)
                            {
                                memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num * CAN_ONE_FRAME_DATA_LENTH], rx_buf.can_data_t.data, rx_len - 1);
                                can_long_frame_buf->can_rcv_buf[buf_index].used_len += rx_len - 1;

                                printf("long frame receive complete\r\n");
                                for(uint8_t j = 0; j < can_long_frame_buf->can_rcv_buf[buf_index].used_len; j++)
                                {
                                    printf("data[%d]: %d\r\n", j, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[j]);
                                }
                                //process the data here//
                                /**********************/
                                //process the data here//

                                Can1_TX(id.canx_id, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf, can_long_frame_buf->can_rcv_buf[buf_index].used_len);  // test :send the data back;
                                can_long_frame_buf->free_buf(buf_index);
                            }
                        }
                    }
                }

            }
        }
        else
        {
            deinit_can1();
            init_can1();
        }
    }

exit:
    return;
}

int send_can_msg(can_buf_t *can_msg)
{
    can_buf_t *can_buf;
    uint8_t err = 0;
    can_buf = (can_buf_t *)OSMemGet(can_send_buf_mem_handle, &err);
    if(can_buf)
    {
        can_buf->id = can_msg->id;
        can_buf->data_len = can_msg->data_len;
        memcpy(can_buf->data, can_msg->data, can_msg->data_len);
        OSQPost(can_send_buf_queue_handle, (void *)can_buf);
    }
    else
    {
        /*
        error: TODO
        */
        return -1;
    }

    return 0;
}


OS_STK can_send_task_stk[CAN_SEND_TASK_STK_SIZE]  = {0};
#define UPLOAD_CAN_MSG_RESEND_CNT               3
#define UPLOAD_CAN_MSG_WAIT_TICK                (5 * OS_TICKS_PER_SEC / 10)
void can_send_task(void *pdata)
{
    can_buf_t *can_send_buf;
    can_upload_ack_t *can_upload_ack;
    can_id_union id = {0};
    uint8_t err = 0;
    uint8_t serial_num = 0;
    uint8_t resend_cnt = 0;
    while(1)
    {
        can_send_buf = (can_buf_t *)OSQPend(can_send_buf_queue_handle, 0, &err);
        if(err == OS_ERR_NONE)
        {
            id.canx_id = can_send_buf->id;
            if(id.can_id_t.ack == 1)
            {
                Can1_TX(can_send_buf->id, can_send_buf->data, can_send_buf->data_len);
            }
            else if(id.can_id_t.ack == 0)
            {
                serial_num++;
                can_send_buf->data_len++;
                if(can_send_buf->data_len <= CAN_BUF_INSIDE_DATA_SIZE)
                {
                    can_send_buf->data[can_send_buf->data_len - 1] = serial_num;
                }
                Can1_TX(can_send_buf->id, can_send_buf->data, can_send_buf->data_len);
                resend_cnt = UPLOAD_CAN_MSG_RESEND_CNT - 1;
                while(resend_cnt--)
                {
                    can_upload_ack = (can_upload_ack_t *)OSQPend(can_upload_ack_queue_handle, UPLOAD_CAN_MSG_WAIT_TICK, &err);

                    if(err == OS_ERR_NONE)
                    {
                        uint8_t get_right_ack_flag = 0;
                        if((can_upload_ack->serial_num == serial_num) && (can_upload_ack->id.can_id_t.source_id == id.can_id_t.source_id))
                        {
                            get_right_ack_flag = 1;
                        }
                        OSMemPut(can_upload_ack_mem_handle, can_upload_ack);
                        if(get_right_ack_flag == 1)
                        {
                            break;
                        }
                    }
                    Can1_TX(can_send_buf->id, can_send_buf->data, can_send_buf->data_len);
                }
            }

            OSMemPut(can_send_buf_mem_handle, can_send_buf);

        }
        else if(err == OS_ERR_TIMEOUT)
        {
            /*
            TODO
            */
        }
//        delay_ms(10);

    }
}


