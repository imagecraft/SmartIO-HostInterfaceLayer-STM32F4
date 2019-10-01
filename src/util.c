#include <stdio.h>

#include "stm32f4xx_ll_usart.h"


int putchar(int ch)
    {
    while ((USART2->SR & USART_SR_TXE) == 0)
        ;
    USART2->DR = ch;
    return ch;
    }

int _write (int fd, char *ptr, int len)
{
    int byteCount = len;
    while(byteCount--)
    {
        if (*ptr == '\n')
        {
            putchar('\r');
        }
        putchar(*ptr++);
    }
    return len;
}

void _exit(int stat)
    {
    while (1);
    }
