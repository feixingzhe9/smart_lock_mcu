#ifndef HARDWARE_CAN__H
#define HARDWARE_CAN__H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
//#define RFID_CAN_MAC_SRC_ID             0x00d6
#define RFID_CAN_MAC_SRC_ID_BASE        0x0080
//#define CAN_FILTER_ID                   (RFID_CAN_MAC_SRC_ID << 13)
#define CAN_FILTER_MASK                 (0x00ff << 13)

//CAN1
#define CAN1_GPIO_RX_PORT       GPIOA
#define CAN1_GPIO_RX_PIN        GPIO_Pin_11
#define CAN1_GPIO_TX_PORT       GPIOA
#define CAN1_GPIO_TX_PIN        GPIO_Pin_12

typedef union
{
    struct
    {
        uint32_t source_id  : 8;
        uint32_t func_id   : 4;
        uint32_t ack       : 1;
        uint32_t dest_mac_id : 8;
        uint32_t src_mac_id  : 8;
        uint32_t res       : 3;
    }can_id_t;
    uint32_t  canx_id;
}can_id_union;

typedef union
{
    struct
    {
        uint8_t seg_num  : 6;
        uint8_t seg_polo : 2;
        uint8_t data[7];
    }can_data_t;
    uint8_t can_data[8];
}can_data_union;


extern uint8_t init_can1(void);
void deinit_can1(void);
#ifdef __cplusplus
  }
#endif
#endif
