#ifndef __QR_CODE__H_
#define __QR_CODE__H_

#include "sys.h"
#include "string.h"

class QRCodeClass 
{
#define QR_DATA_LENTH   200
    public:
        QRCodeClass(u8 id)
        {
            my_id = id;
            tick = 0;
            data_cnt = 0;
            memset(qr_data, 0, sizeof(qr_data));
        }   

        void upload_qr_data(void);
        void put_one_data(u8 data, u32 start_tick);
        void clear_data(void);
        void task(void);
            
        u32 tick;
        u8 qr_data[QR_DATA_LENTH];
        u16 data_cnt;
        u8 my_id;
        
};

extern QRCodeClass qr_code_1;
extern QRCodeClass qr_code_2;
extern QRCodeClass qr_code_3;

extern void all_qr_data_task(void);

#endif
