#include "sys.h"
#include "usart.h"


#if SYSTEM_SUPPORT_UCOS
#include "includes.h"               //ucos ʹ��
#endif


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

void _ttywrch(int ch)
{
    ch = ch;
}

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
#if EN_USART1_RX
    //while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
    //USART1->DR = (u8) ch;
#else
    //while((USART3->SR&0X40)==0);//ѭ������,ֱ���������
    //USART3->DR = (u8) ch;
#endif
    return ch;
}
#endif
/*ʹ��microLib�ķ���*/
/*
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t) ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
    return ch;
}
int GetKey (void)
{
    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/






