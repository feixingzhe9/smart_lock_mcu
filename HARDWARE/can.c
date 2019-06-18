/**
 ******************************************************************************
 * @file    can.c
 * @author  Kaka.Xie
 * @brief   This file provides CAN driver.
 ******************************************************************************

 ******************************************************************************
 */

#include "can.h"

void deinit_can1(void)
{
    CAN_DeInit(CAN1);
}

uint8_t init_can1(void)
{
    //////-- CAN GPIO Init --//////
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟

    GPIO_InitStructure.GPIO_Pin = CAN1_GPIO_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(CAN1_GPIO_TX_PORT, &GPIO_InitStructure);		//初始化IO

    GPIO_InitStructure.GPIO_Pin = CAN1_GPIO_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(CAN1_GPIO_RX_PORT, &GPIO_InitStructure);//初始化IO

    //////-- CAN Init --//////
    CAN_InitTypeDef  can_init_struct;

    CAN_StructInit(&can_init_struct);           // Fills each CAN_InitStruct member with its default value.

    can_init_struct.CAN_ABOM = ENABLE;          // The Bus-Off state is left automatically by hardware once 128 occurrences of 11 recessive bits have been monitored.
    can_init_struct.CAN_NART = DISABLE;         // The CAN hardware will automatically retransmit the message until it has been successfully transmitted according to the CAN standard.
    can_init_struct.CAN_AWUM = ENABLE;          // The Sleep mode is left automatically by hardware on CAN message detection
    can_init_struct.CAN_RFLM = DISABLE;         // Receive FIFO not locked on overrun. Once a receive FIFO is full the next incoming message will overwrite the previous one.
    can_init_struct.CAN_TTCM = DISABLE;         // Time Triggered Communication mode disabled
    can_init_struct.CAN_TXFP = DISABLE;         // Priority driven by the identifier of the message

    can_init_struct.CAN_Mode = CAN_Mode_Normal; // normal mode

    can_init_struct.CAN_Prescaler = 9;

    can_init_struct.CAN_SJW = CAN_SJW_1tq;
    can_init_struct.CAN_BS1 = CAN_BS1_4tq;
    can_init_struct.CAN_BS2 = CAN_BS2_3tq;

    CAN_Init(CAN1, &can_init_struct);

    //////-- Filter Init --//////
    CAN_FilterInitTypeDef can_filter_init_struct;
    can_filter_init_struct.CAN_FilterMode = CAN_FilterMode_IdMask;
    can_filter_init_struct.CAN_FilterNumber = 0;
    can_filter_init_struct.CAN_FilterScale = CAN_FilterScale_32bit;

    can_filter_init_struct.CAN_FilterIdHigh = ((CAN_FILTER_ID << 3) >> 16) & 0xffff;//0x0000;
    can_filter_init_struct.CAN_FilterIdLow = (uint16_t)(CAN_FILTER_ID << 3) | CAN_ID_EXT;//0x0000;
    can_filter_init_struct.CAN_FilterMaskIdHigh = (CAN_FILTER_MASK << 3) >> 16;//0x0000;
    can_filter_init_struct.CAN_FilterMaskIdLow = ((CAN_FILTER_MASK << 3) & 0xffff) | 0x06;//0x0000;

    can_filter_init_struct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    can_filter_init_struct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&can_filter_init_struct);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);


    //////-- CAN1 RX0 Interrupts Config --//////
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
extern void can_long_buf_init(void);
    can_long_buf_init();
    return 0;
}

