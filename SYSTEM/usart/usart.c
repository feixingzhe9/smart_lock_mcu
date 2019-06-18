#include "sys.h"
#include "usart.h"


#if SYSTEM_SUPPORT_UCOS
#include "includes.h"               //ucos 使用
#endif


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

void _ttywrch(int ch)
{
    ch = ch;
}

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
#if EN_USART1_RX
    //while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
    //USART1->DR = (u8) ch;
#else
    //while((USART3->SR&0X40)==0);//循环发送,直到发送完毕
    //USART3->DR = (u8) ch;
#endif
    return ch;
}
#endif
/*使用microLib的方法*/
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






