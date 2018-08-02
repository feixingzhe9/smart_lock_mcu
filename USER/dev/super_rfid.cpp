#include "super_rfid.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hall.h"
#include "can_interface.h"

const static uint8_t rfid_info[] = {0x04,0xff,0x21,0x19,0x95};

const static uint8_t turnoff_antenna[] = {0x05,0x00,0x66,0xf3,0x5c};

const static uint8_t set_antenna1[] = {0x05,0x00,0x3f,0x01,0x95,0x49};
//const static uint8_t set_antenna2[] = {0x05,0x00,0x3f,0x02,0x0e,0x7b};
//const static uint8_t set_antenna3[] = {0x05,0x00,0x3f,0x03,0x38,0x1e};
//const static uint8_t set_antenna4[] = {0x05,0x00,0x3f,0x04,0x54,0xd4};
//const static uint8_t set_antenna_all[] = {0x05,0x00,0x3f,0x0f,0xeb,0xa0};

const static uint8_t loop_check[] = {0x11,0x00,0x19,0x04,0x00,0x02,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x80,0x14,0xd3,0x72};

static void uart3_init();
static void serial_clear_buffer(void);
static inline bool serial_has_data(void);
static uint8_t serial_recv(void);
static void serial_send(const uint8_t *bytes, uint32_t length);
static void protocol_startup(void);
static unsigned int uiCrc16Cal(unsigned char const* pucY, unsigned char ucX);
static uint8_t read_one_frame(uint8_t cmd, uint8_t *read_out);
static char* DataToHexStringWithSpaces( const uint8_t *inBuf, uint32_t inBufLen );
static void upload();
static void upload_can(can_message_t *rfid_msg, const uint8_t *tid);
static void reset_state();

static can_message_t upload_msg;

#define TID_NONE	0x00
#define TID_OLD		0x01
#define TID_STAY	0x02
#define TID_NEW		0x03

#define TID_NO_MAX	32
#define STABLE_COUNT_MAX 64
#define STABLE_COUNT_MIN 10
static uint8_t TID_NO[TID_NO_MAX][TID_LEN+1] = {0};


static int get_index(uint8_t *no)
{
		int index = -1;
	
	  if (!no) return -1;
	
		for (int i = 0; i < TID_NO_MAX; i++)
		{
				if (TID_NO[i][0] == TID_NONE) continue;
				
				if (memcmp(&TID_NO[i][1], no, TID_LEN) == 0)
				{
						index = i;
						break;
				}
		}
		
		return index;
}


static int get_effect_cnt()
{
	  int cnt = 0;
	  
	  for (int i = 0; i < TID_NO_MAX; i++)
	  {
			  if (TID_NO[i][0] == TID_STAY || TID_NO[i][0] == TID_NEW) cnt++;
		}
		
		return cnt;
}


static uint8_t get_state(int index)
{
		return TID_NO[index][0];
}

static const uint8_t *get_tid(int index)
{
		return (const uint8_t *)&TID_NO[index][1];
}

static void set_state(int index, uint8_t state)
{
		TID_NO[index][0] = state;
		
		return;
}

static void insert_no(uint8_t *no)
{
		int i;
	
		if (!no) return;
		
		for (i = 0; i < TID_NO_MAX; i++)
		{
				if (TID_NO[i][0] == TID_NONE)
				{
						memcpy(&TID_NO[i][1], no, TID_NO_MAX);
						TID_NO[i][0] = TID_NEW;
						break;
				}
		}
		
		if (i >= TID_NO_MAX)
		{
				printf("TID_NO_MAX is too small\r\n");
		}
		
		return;
}

static int insert(uint8_t *no)
{
		int index;
		int is_stable = 0;
	
	  if (!no) return is_stable;
	
	  index = get_index(no);
	
		if (index >= 0)
		{
				if (get_state(index) == TID_OLD)
				{
						set_state(index, TID_STAY);
				}
				else 
				{
						is_stable = 1;
				}
		}
		else
		{
				insert_no(no);
		}
		
		return is_stable;
}

static void process(uint8_t* no)
{
		if (!no) return;
	
		static uint32_t stable_count = 0;

		int is_stable = insert(no);
		
		if (is_stable)
		{
			  int cnt = get_effect_cnt()*3;
			
				if (cnt < STABLE_COUNT_MIN) cnt = STABLE_COUNT_MIN;
			
				if (++stable_count > cnt)
				{
						upload();
					  reset_state();
				}
		}
		else
		{
				stable_count = 0;
		}
		
		return;
}

void super_rfid_init()
{
	  uart3_init();
	  protocol_startup();
	
		return;
}

void super_rfid_task()
{	
	  uint8_t status;
	  uint8_t tid_num[TID_LEN];
	
		if (!is_hall_detected()) return;
			
	  do {
			  status = read_one_frame(0x19, tid_num);
				if (status != 0)
				{
					process(tid_num);
				}
		} while (serial_has_data());

		serial_send(loop_check, sizeof(loop_check));
	
		return;
}

static void upload_can(can_message_t *rfid_msg, const uint8_t *tid)
{
		if (!rfid_msg || !tid ) return;
	
		rfid_msg->id = 0x0de02186;
		rfid_msg->data[0] = 0x40;
		memcpy( (void *)&rfid_msg->data[1], tid, 7 );
		rfid_msg->data_len = 8;
		can.can_send( rfid_msg );
		delay_ms(10);
		rfid_msg->id = 0x0de02186;
		rfid_msg->data[0] = 0x81;
		memcpy( (void *)&rfid_msg->data[1], tid + 7, 7 );
		rfid_msg->data_len = 8;
		can.can_send( rfid_msg );
		delay_ms(10);
		rfid_msg->id = 0x0de02186;
		rfid_msg->data[0] = 0xC2;
		memcpy( (void *)&rfid_msg->data[1], tid + 14, 1 );
		rfid_msg->data_len = 2;
		can.can_send( rfid_msg );
}

static void upload()
{
		int count = 0;
	
		for (int i = 0; i < TID_NO_MAX; i++ )
		{
				uint8_t state = get_state(i);
				if (state == TID_STAY || state == TID_NEW)
				{
						upload_can(&upload_msg, get_tid(i));
						char *debug_str;
						debug_str = DataToHexStringWithSpaces(get_tid(i), TID_LEN);
						printf("\r\nTID_NO.%d:%s\r\n", ++count, debug_str);
						free( debug_str );
				}
		}
		printf("-----------------------------------------------------\r\n");
		
		return;
}


static void reset_state()
{
		for (int i = 0; i < TID_NO_MAX; i++ )
		{
				uint8_t state = get_state(i);
			
				if (state == TID_STAY || state == TID_NEW)
				{
						set_state(i, TID_OLD);
				}
				else if (state == TID_OLD)
				{
						set_state(i, TID_NONE);
				}
		}
		
		return;
}

static char* DataToHexStringWithSpaces( const uint8_t *inBuf, uint32_t inBufLen )
{
    char* buf_str = NULL;
    char* buf_ptr = NULL;
    if (!inBuf) return NULL;
    if (!inBufLen) return NULL;

    buf_str = (char*) malloc (3*inBufLen + 1);
    if (!buf_str) goto error;
    buf_ptr = buf_str;
    uint32_t i;
    for (i = 0; i < inBufLen; i++) buf_ptr += sprintf(buf_ptr, "%02X ", inBuf[i]);
    *buf_ptr = '\0';
    return buf_str;

error:
    if ( buf_str ) free( buf_str );
    return NULL;
}

#define FRAME_LEN_MAX			0xFF
static uint8_t read_one_frame(uint8_t cmd, uint8_t *read_out)
{
		int i;
		uint8_t len;
		uint8_t addr;
	  uint8_t reCmd;
	  uint8_t status = 0;
		uint8_t data[FRAME_LEN_MAX];
		uint8_t need_len;
	
	  switch (cmd)
		{
			case 0x19:
				need_len = 0x1A;
				break;
			default:
				need_len = 0xFF;
				break;
		}
		
		do {
				len = data[0] = serial_recv();
				//printf("%02x ", len);
		} while (len != need_len && serial_has_data());
		if (!serial_has_data()) return status;

		addr = data[1] = serial_recv();
		reCmd = data[2] = serial_recv();
		
		for (i = 3; i <= len; i++)
		{
				data[i] = serial_recv();
		}
		
		uint16_t data_check = (uint16_t)data[len] << 8 | data[len - 1];
		if ( data_check == uiCrc16Cal(data, len - 1))
		{
			 	status = data[3];
				if (read_out)
				{
						switch (cmd)
						{
							case 0x19:
								memcpy(read_out, &data[8], TID_LEN);
							break;
						}
				}
				#if 0
				char *debug_str;
				debug_str = DataToHexStringWithSpaces(data, len + 1);
				printf("\r\nresponse:%s\r\n", debug_str);
				free( debug_str );
				#endif
		}
		(void)addr;
		(void)reCmd;
		
		return status;	
}

static void protocol_startup(void)
{
		serial_send(rfid_info, sizeof(rfid_info));
		delay_ms(100);
	  read_one_frame(0x21, NULL);
	  serial_clear_buffer();
		serial_send(turnoff_antenna, sizeof(turnoff_antenna));
	  delay_ms(100);
	  read_one_frame(0x66, NULL);
	  serial_clear_buffer();
	  serial_send(set_antenna1, sizeof(set_antenna1));
	  delay_ms(100);
	  read_one_frame(0x3F, NULL);
	  serial_clear_buffer();
	
		return;
}

static void serial_send(const uint8_t *bytes, uint32_t length)
{
		uint8_t *p_data = (uint8_t *)bytes;
	
		if (!p_data) return;
	
	  for (int i = 0; i < length; i++)
		{
				while ((USART3->SR&0X40) == 0);//循环发送,直到发送完毕   
				USART3->DR = (uint8_t)*p_data++;    
		}
		
		return;
}

#define PRESET_VALUE 0xFFFF
#define POLYNOMIAL  0x8408
static unsigned int uiCrc16Cal(unsigned char const* pucY, unsigned char ucX)
{
		unsigned char ucI,ucJ;
		unsigned short int  uiCrcValue = PRESET_VALUE;
		for(ucI = 0; ucI < ucX; ucI++)
		{
				uiCrcValue = uiCrcValue ^ *(pucY + ucI);
				for(ucJ = 0; ucJ < 8; ucJ++)
				{
						if(uiCrcValue & 0x0001)
						{
								uiCrcValue = (uiCrcValue >> 1) ^ POLYNOMIAL;
						}
						else
						{
								uiCrcValue = (uiCrcValue >> 1);
						}
				}
		}
		return uiCrcValue;
}

static void uart3_init()
{
	  //GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	
		USART_DeInit(USART3);  //复位串口1
	
		//USART3_TX   PB.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PA9

		//USART3_RX	  PB.11
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PA10

		//Usart1 NVIC 配置

		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

		//USART 初始化设置

		USART_InitStructure.USART_BaudRate = 57600;//一般设置为115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Init(USART3, &USART_InitStructure); //初始化串口
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
		USART_Cmd(USART3, ENABLE);                    //使能串口
		
		return;
}

#define UART_RBUF_SIZE		300
static uint8_t rbuf[UART_RBUF_SIZE];
static uint8_t rbuf_head = 0;
static uint8_t rbuf_tail = 0;

#ifdef  __cplusplus
extern "C" {
#endif
	
//void USART3_IRQHandler(void)
//{
//		uint8_t next = (rbuf_head + 1) % UART_RBUF_SIZE;

//		if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
//		{
//				if (next != rbuf_tail)
//				{
//						rbuf[rbuf_head] = USART_ReceiveData(USART3);//(USART3->DR);
//						rbuf_head = next;
//				}
//				else
//				{
//						USART_ReceiveData(USART3);
//				}
//    }
//		
//		return;
//}

#ifdef  __cplusplus
}
#endif

static uint8_t serial_recv(void)
{
		uint8_t data = 0;
	
		if (rbuf_head != rbuf_tail) {
				data = rbuf[rbuf_tail];
				rbuf_tail = (rbuf_tail + 1) % UART_RBUF_SIZE;
		}
		
		return data;
}

static void serial_clear_buffer(void)
{
		rbuf_head = rbuf_tail = 0;
}

static inline bool serial_has_data(void)
{
		return rbuf_head != rbuf_tail;
}

