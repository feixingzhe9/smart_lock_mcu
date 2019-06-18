#include "qr_code.h"
//#include "can_interface.h"
#include "usart.h"

QRCodeClass qr_code_1(1);
QRCodeClass qr_code_2(2);
QRCodeClass qr_code_3(3);

#ifdef __cplusplus
extern "C" {
#endif
void USART1_IRQHandler(void)                        //串口1中断服务程序
{
    u8 data;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        data =USART_ReceiveData(USART1);//(USART1->DR); //读取接收到的数据
        qr_code_1.put_one_data(data, get_tick());
    }
}

void USART3_IRQHandler(void)                        //串口3中断服务程序
{
    u8 data;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)   //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        data =USART_ReceiveData(USART3);//(USART3->DR); //读取接收到的数据
        qr_code_3.put_one_data(data, get_tick());
    }
}

void USART2_IRQHandler(void)                        //串口3中断服务程序
{
    u8 data;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        data =USART_ReceiveData(USART2);//(USART3->DR); //读取接收到的数据
        qr_code_2.put_one_data(data, get_tick());
    }
}

#ifdef __cplusplus
  }
#endif


u8 qr_test_data_1[QR_DATA_LENTH] = {0};
u8 qr_test_data_2[QR_DATA_LENTH] = {0};
u8 qr_test_data_3[QR_DATA_LENTH] = {0};
void QRCodeClass::upload_qr_data(void)  //upload data through CAN bus
{
//    can_id_union id;
//    switch(this->my_id)
//    {
//        case 1:
//            memcpy(qr_test_data_1, this->qr_data, sizeof(this->qr_data));
//            id.can_id_struct.source_id = CAN_SOURCE_ID_QR_CODE_UPLOAD_1;
//            break;
//        case 2:
//            memcpy(qr_test_data_2, this->qr_data, sizeof(this->qr_data));
//            id.can_id_struct.source_id = CAN_SOURCE_ID_QR_CODE_UPLOAD_2;
//            break;
//        case 3:
//            memcpy(qr_test_data_3, this->qr_data, sizeof(this->qr_data));
//            id.can_id_struct.source_id = CAN_SOURCE_ID_QR_CODE_UPLOAD_3;
//            break;
//        default : break;
//    }

//    id.can_id_struct.src_mac_id = LOCK_CAN_MAC_SRC_ID;

//    id.can_id_struct.res = 0;
//    id.can_id_struct.ack = 0;
//    id.can_id_struct.func_id = 0;

//    Can1_TX(id.can_id,this->qr_data, this->data_cnt);

}

void QRCodeClass::put_one_data(u8 data, u32 start_tick)
{
    if(this->data_cnt < QR_DATA_LENTH)
    {
        this->qr_data[this->data_cnt] = data;
        this->tick = start_tick;
        this->data_cnt++;
    }
}

void QRCodeClass::clear_data(void)
{
    this->data_cnt = 0;
    tick = 0;
    memset(qr_data, 0, sizeof(qr_data));
}

#define QR_CODE_UPLOAD_DELAY_TIME   100/SYSTICK_PERIOD
void QRCodeClass::task(void)
{
    if(this->data_cnt > 0)
    {
        if(get_tick() - this->tick > QR_CODE_UPLOAD_DELAY_TIME)
        {
            this->upload_qr_data();
            this->clear_data();
        }
    }
}


void qr_code_init(void)
{
    uart_1_init(9600);      //串口1初始化为9600
    uart_2_init(9600);      //串口2初始化为9600
    uart_3_init(9600);      //串口3初始化为9600
}

void all_qr_data_task(void)
{
    qr_code_1.task();
    qr_code_2.task();
    qr_code_3.task();
}

