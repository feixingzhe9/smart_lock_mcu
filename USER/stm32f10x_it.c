/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "sys.h"
#include "usart.h"
#include "stm32f10x.h"
#include "param.h"
#include <stdio.h>


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

//void PendSV_Handler(void)
//{
//}

//extern uint32_t sys_tick_cnt;
///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
//void SysTick_Handler(void)
//{
//    sys_tick_cnt++;
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
void EXTI4_IRQHandler(void)
{
    //printf("exti at pc4\r\n");
    EXTI_ClearITPendingBit(EXTI_Line4);
    NVIC_SystemReset();
}

void EXTI9_5_IRQHandler(void)
{
#if 0
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line9);
        lock_status_change_start_tick = get_tick();
        is_lock_status_changed = 1;
    }
#endif
    //printf("exti at pc6\r\n");
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
        NVIC_SystemReset();
    }

}



#include <string.h>
#include "can_protocol_task.h"
extern CanRxMsg RxMessage;
void USB_LP_CAN1_RX0_IRQHandler(void)
{
#if 0
    OSIntEnter();
    can_pkg_t can_pkg_tmp;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    can_pkg_tmp.id.canx_id = RxMessage.ExtId;
    can_pkg_tmp.len = RxMessage.DLC;
    memcpy(can_pkg_tmp.data.can_data, RxMessage.Data, can_pkg_tmp.len);
    put_can_pkg_to_fifo(can_fifo, can_pkg_tmp);
    OSIntExit();
#else
    OSIntEnter();
    can_pkg_t *can_buf;
    uint8_t err = 0;
    while((CAN1->RF0R & 0x03) > 0)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        can_buf = (can_pkg_t *)OSMemGet(can_rcv_buf_mem_handle, &err);
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0 | CAN_IT_FOV0);   //ȥԽא׏

        if((can_buf != 0) && (err == OS_ERR_NONE))
        {
            can_buf->id.canx_id = RxMessage.ExtId;
            can_buf->len = RxMessage.DLC;
            memcpy(can_buf->data.can_data, RxMessage.Data, can_buf->len);
            OSQPost(can_rcv_buf_queue_handle, (void *)can_buf);
        }
//        else
//        {
//            can_rcv_buf_mem_handle = OSMemCreate((void *)&can_rcv_buf_mem[0][0], sizeof(can_rcv_buf_mem) / sizeof(can_rcv_buf_mem[0]), sizeof(can_pkg_t), &err);
//            if(can_rcv_buf_mem_handle == 0)
//            {
//                /*
//                todo: err process
//                */
//            }
//        }
    }

    OSIntExit();
#endif
}

