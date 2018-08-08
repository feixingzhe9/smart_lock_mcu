#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include "sys.h"
#include <stdio.h>	

#define CAN_USED    CAN1

#define LOCK_CAN_MAC_SRC_ID     0xd6
#define CAN_FILTER_ID           (0x00d6 << 13)
#define CAN_FILTER_MASK         (0x00ff << 13)



//////  function id define  //////
#define CAN_FUN_ID_RESET        0x06
#define CAN_FUN_ID_WRITE        0x01
#define CAN_FUN_ID_READ         0x02
#define CAN_FUN_ID_TRIGGER      0x03


//////  source id define  //////
#define CAN_SOURCE_ID_READ_VERSION      0x01    

#define CAN_SOURCE_ID_CAN_TEST              0x03


#define CAN_SOURCE_ID_UNLOCK            0x80
#define CAN_SOURCE_ID_LOCK_STATUS       0x81
#define CAN_SOURCE_ID_PW_UPLOAD         0x82
#define CAN_SOURCE_ID_RFID_UPLOAD       0x83

#define CAN_SOURCE_ID_SET_SUPER_PW      0x84
#define CAN_SOURCE_ID_SET_SUPER_RFID    0x85

#define CAN_SOURCE_ID_QR_CODE_UPLOAD_1  0x90
#define CAN_SOURCE_ID_QR_CODE_UPLOAD_2  0x91
#define CAN_SOURCE_ID_QR_CODE_UPLOAD_3  0x92



#define CAN_LONG_FRAME_TIME_OUT     5000/SYSTICK_PERIOD
    
#define CAN_ONE_FRAME_DATA_LENTH    7
#define CAN_SEG_NUM_MAX             64
#define CAN_LONG_FRAME_LENTH_MAX    (CAN_ONE_FRAME_DATA_LENTH*CAN_SEG_NUM_MAX)


#if 0
typedef union
{
	struct
	{
		uint32_t SourceID  : 8;
		uint32_t FUNC_ID   : 4;
		uint32_t ACK       : 1;
		uint32_t DestMACID : 8;
		uint32_t SrcMACID  : 8;
		uint32_t res       : 3;
	} __attribute__ ((packed)) CanID_Struct;
	uint32_t  CANx_ID;
} __attribute__ ((packed)) CAN_ID_UNION;


typedef union
{
	struct
	{
				uint8_t SegNum  : 6;
				uint8_t SegPolo : 2;
		uint8_t Data[7];
	} __attribute__ ((packed)) CanData_Struct;
	uint8_t CanData[8];
} __attribute__ ((packed)) CAN_DATA_UNION;

#else

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
	}__attribute__ ((packed)) can_id_struct;
	uint32_t  can_id;
} __attribute__ ((packed)) can_id_union;

//typedef union
//{
//	struct
//	{
//		uint32_t SourceID  : 8;
//		uint32_t FUNC_ID   : 4;
//		uint32_t ACK       : 1;
//		uint32_t DestMACID : 8;
//		uint32_t SrcMACID  : 8;
//		uint32_t res       : 3;
//	} __attribute__ ((packed)) CanID_Struct;
//	uint32_t  CANx_ID;
//} __attribute__ ((packed)) CAN_ID_UNION;


typedef union
{
	struct
	{
        uint8_t SegNum  : 6;
		uint8_t SegPolo : 2;
		
        uint8_t Data[7];
	} __attribute__ ((packed)) CanData_Struct;
	uint8_t CanData[8];
} __attribute__ ((packed)) CAN_DATA_UNION;
#endif


typedef struct
{
    can_id_union can_id;
    
    uint32_t start_time; 
    uint16_t used_len;
    uint8_t rcv_buf[CAN_LONG_FRAME_LENTH_MAX];   
}CAN_RCV_BUFFER_T;

typedef uint8_t (*GetOneFreeBufFn)(void);
typedef uint8_t (*GetTheBufByIdFn)(uint32_t);
typedef void (*FreeBufFn)(uint8_t);

#define CAN_LONG_BUF_NUM    2
typedef struct
{
    CAN_RCV_BUFFER_T can_rcv_buf[CAN_LONG_BUF_NUM];
    GetOneFreeBufFn GetOneFreeBuf; 
    GetTheBufByIdFn GetTheBufById;
    FreeBufFn FreeBuf;
}CAN_LONG_BUF_T;


#define ONLYONCE       0x00
#define BEGIAN         0x01
#define TRANSING       0x02
#define END            0x03




struct can_timing_t {
	const char *baud;
	uint16_t brp; // brp[0:9]
	uint16_t ts; // res[15] lbkm[14] res[13:10] swj[9:8] res[7] ts2[6:4] ts1[3:0]
} __attribute__ ((packed));

#define CAN_MSG_SIZE  0x0F // DLC[0:3]
#define CAN_MSG_RTR   0x10 // RTR[4]
#define CAN_MSG_EID   0x20 // EID[5]
#define CAN_MSG_INV   0x40 // is message in-valid

struct can_message_t {
	uint32_t id;
	uint8_t data_len;
	uint8_t data[8];
} __attribute__ ((packed));

class loop_buffer {
public:
	#define RBUF_SIZE 10
	struct can_message_t rbuf[RBUF_SIZE];
  uint8_t  rbuf_head;
	uint8_t  rbuf_tail;
  inline void rbuf_enqueue(struct can_message_t *msg);
  inline struct can_message_t rbuf_dequeue(void);
  inline bool is_rbuf_has_data(void);
  inline void rbuf_clear(void);
  loop_buffer();
};

class can_interface {
private:
	struct can_timing_t *CAN_Timing;
  uint8_t 						changeBaudRate(const char* baud);
  NVIC_InitTypeDef 		CAN_Int;
public:
  loop_buffer 				rx_buf;
	can_interface(const uint8_t id, const char* baud);
  //~can_interface();
  void canHWReinit();
  void can_filter_apply();
  void can_filter_addmask(uint16_t cobid, uint16_t cobid_mask, uint8_t prio);
	uint8_t can_send(struct can_message_t *m);
  bool is_can_has_data(void);
  struct can_message_t can_read(void);
  void canAckBack(uint32_t CANx_ID, const uint8_t * const pdata, uint16_t len);
};

uint8_t can_receive();

extern can_interface can;
extern void can_protocol(void);

extern void Can1_TX(uint32_t CANx_ID,uint8_t* pdata,uint16_t len);
//extern can_interface can(0x66, "500K");
#endif // CAN_INTERFACE_H

