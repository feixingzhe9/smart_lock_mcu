/*
 *  Author: Kaka Xie
 *  brief: usart operation
 */

#include "sys.h"
#include "usart.h"
#include <string.h>
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"           //ucos 使用
#endif


#if 1
#pragma import(__use_no_semihosting)
//标准库函数需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义 _sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    //while((USART1->SR&0X40)==0);
    //USART1->DR = (u8) ch;
    return ch;
}
#endif


uint8_t rfid_uart_send_buf[RFID_UART_SEND_SIZE];
uint8_t sw_rfid_uart_rcv_buf[RFID_UART_RCV_SIZE];
void uart2_dma_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    GPIO_InitTypeDef	GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  //
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
    //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    //DMA configuration,  USART2 TX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&rfid_uart_send_buf[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = RFID_UART_SEND_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel7, DISABLE);

    //USART2 RX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&sw_rfid_uart_rcv_buf[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = RFID_UART_RCV_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    //DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel6, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


int uart2_send(uint8_t *data, uint16_t len)
{
    memcpy(rfid_uart_send_buf, data, len);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    DMA1_Channel7->CNDTR = len;
    DMA_Cmd(DMA1_Channel7, ENABLE);

    return 0;
}



//ԵʼۯIO Ԯࠚ1
//bound:Ҩ͘Ê
void uart_1_init(u32 bound)
{
    //GPIO׋ࠚʨ׃
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹŜUSART1ìGPIOAʱד
    USART_DeInit(USART1);  //شλԮࠚ1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//شԃΆάˤԶ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //ԵʼۯPA9

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//ءࠕˤɫ
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //ԵʼۯPA10

    //Usart1 NVIC Ƥ׃

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//ȀռԅЈܶ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//ؓԅЈܶ3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨրʹŜ
    NVIC_Init(&NVIC_InitStructure);	//ٹߝָ֨քӎ˽ԵʼۯVIC݄զǷ

    //USART Եʼۯʨ׃

    USART_InitStructure.USART_BaudRate = bound;//һѣʨ׃Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//ؖӤΪ8λ˽ߝٱʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һٶֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//ϞǦżУҩλ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ϞӲݾ˽ߝ·࠘׆
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//˕עģʽ

    USART_Init(USART1, &USART_InitStructure); //ԵʼۯԮࠚ
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ߪǴא׏
    USART_Cmd(USART1, ENABLE);                    //ʹŜԮࠚ
}




//ԵʼۯIO Ԯࠚ2
//bound:Ҩ͘Ê
void uart_2_init(u32 bound)
{
    //GPIO׋ࠚʨ׃
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹŜUSART2ìGPIOAʱד
    USART_DeInit(USART2);  //شλԮࠚ2
    //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//شԃΆάˤԶ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //ԵʼۯPA2

    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//ءࠕˤɫ
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //ԵʼۯPA3

    //USART2 NVIC Ƥ׃

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//ȀռԅЈܶ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//ؓԅЈܶ3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨրʹŜ
    NVIC_Init(&NVIC_InitStructure);	//ٹߝָ֨քӎ˽ԵʼۯVIC݄զǷ

    //USART Եʼۯʨ׃

    USART_InitStructure.USART_BaudRate = bound;//һѣʨ׃Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//ؖӤΪ8λ˽ߝٱʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һٶֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//ϞǦżУҩλ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ϞӲݾ˽ߝ·࠘׆
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//˕עģʽ

    USART_Init(USART2, &USART_InitStructure); //ԵʼۯԮࠚ
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ߪǴא׏
    USART_Cmd(USART2, ENABLE);                    //ʹŜԮࠚ
}



//ԵʼۯIO Ԯࠚ3
//bound:Ҩ͘Ê
void uart_3_init(u32 bound)
{
    //GPIO׋ࠚʨ׃
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹŜUSART3ìGPIOBʱד
    USART_DeInit(USART3);  //شλԮࠚ1
    //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//شԃΆάˤԶ
    GPIO_Init(GPIOB, &GPIO_InitStructure); //ԵʼۯPB10

    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//ءࠕˤɫ
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //ԵʼۯPB11

    //USART3 NVIC Ƥ׃

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//ȀռԅЈܶ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//ؓԅЈܶ3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨրʹŜ
    NVIC_Init(&NVIC_InitStructure);	//ٹߝָ֨քӎ˽ԵʼۯVIC݄զǷ

    //USART Եʼۯʨ׃

    USART_InitStructure.USART_BaudRate = bound;//һѣʨ׃Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//ؖӤΪ8λ˽ߝٱʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һٶֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//ϞǦżУҩλ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ϞӲݾ˽ߝ·࠘׆
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//˕עģʽ

    USART_Init(USART3, &USART_InitStructure); //ԵʼۯԮࠚ
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//ߪǴא׏
    USART_Cmd(USART3, ENABLE);                    //ʹŜԮࠚ
}

