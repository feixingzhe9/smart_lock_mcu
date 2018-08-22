#ifndef __USART_H
#define __USART_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "sys.h"

void uart_1_init(u32 bound);
void uart_3_init(u32 bound);
void uart_2_init(u32 bound);

#ifdef  __cplusplus
}
#endif

#endif


