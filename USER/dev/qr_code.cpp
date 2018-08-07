#include "qr_code.h"


QRCodeClass qr_code_1(1);
QRCodeClass qr_code_2(2);
QRCodeClass qr_code_3(3);


#ifdef __cplusplus
extern "C" {
#endif
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    u8 data;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        data =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
        qr_code_1.put_one_data(data, get_tick());
    } 
}

#ifdef __cplusplus
  }
#endif

u8 qr_test_data[QR_DATA_LENTH] = {0};
void QRCodeClass::upload_qr_data(void)  //upload data through CAN bus
{
    //----  test code  ----//

    memcpy(qr_test_data, this->qr_data, sizeof(this->qr_data));
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
//            upload_qr_data();
//            clear_data();             
        }
    }   
}

void all_qr_data_task(void)
{
    qr_code_1.task();
    qr_code_2.task();
    qr_code_3.task();
}

