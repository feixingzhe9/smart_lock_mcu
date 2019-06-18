#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"

#define RFID_UART_SEND_SIZE      32
#define RFID_UART_RCV_SIZE       32



extern uint8_t rfid_uart_send_buf[RFID_UART_SEND_SIZE];
extern uint8_t sw_rfid_uart_rcv_buf[RFID_UART_RCV_SIZE];


void uart2_dma_init(u32 bound);
int uart2_send(uint8_t *data, uint16_t len);

#endif

