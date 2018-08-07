#ifndef __USART_H
#define __USART_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdio.h"	
#include "sys.h" 

//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug

//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART3_RX 			0		//ʹ�ܣ�1��/��ֹ��0������3����
#if EN_USART3_RX	  	
extern u8  USART_RX_BUF_3[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
#endif
#if EN_USART1_RX	  	
extern u8  USART_RX_BUF13[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
#endif

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_1_init(u32 bound);
void uart_3_init(u32 bound);
void uart_2_init(u32 bound);

#ifdef  __cplusplus
}  
#endif

#endif


