/* Copyright 2018 ImageCraft Creations Inc., All rights reserved.
 * Smart.IO Host Interface Layer
 * https://imagecraft.com/smartio/
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "main.h"

#include "smartio_interface.h"
#include "smartio_hardware_interface.h"

/* ST-Nucleo F411 Smart.IO pin assignments
 * SPI
 *  PORT  PIN AF
 *  porta, 5, 5,       // SCK
 *  porta, 7, 5,       // MOSI
 *  porta, 6, 5,       // MISO
 *  portb, 6,          // CS
 *
 * HOST interrupt
 *  PA9
 *
 * Smart.IO RESET
 *  PC7
 */

#define HOST_INTR_PIN       9

void (*smartio_IRQ_ISR)(void);

/* Hardware Smart.IO
 */
void SmartIO_HardReset(void)
    {
    GPIOC->BSRR = (1 << 7) << 16;   // Clear PORTC.7
    LL_mDelay(10);
    GPIOC->BSRR = (1 << 7);         // Set PORTC.7
    }

static uint8_t SPI_WriteByte(int n)
    {
    while ((SPI1->SR & SPI_SR_TXE) == 0)
        ;
    SPI1->DR = (uint8_t)n;
    while ((SPI1->SR & SPI_SR_RXNE) == 0)
        ;
    while ((SPI1->SR & SPI_SR_BSY) != 0)
        ;
    return SPI1->DR;
    }

/* send bytes out using the SPI port
 *  sendbuf: buffer containing content to send
 *  sendlen: number of bytes to send
 */
void SmartIO_SPI_SendBytes(unsigned char *sendbuf, int sendlen)
    {
    GPIOB->BSRR = (1 << 6) << 16;   // Clear PORTB.6    SPI nCS
    SPI_WriteByte(sendlen);
    SPI_WriteByte(sendlen >> 8);
    for (int i = 0; i < sendlen; i++)
        SPI_WriteByte(*sendbuf++);
    GPIOB->BSRR = (1 << 6);        // Set PORTB.6
    }

/* read bytes from the SPI port
 *  replybuf: buffer to fill the content with
 *  buflen: length of "replybuf"
 */
int SmartIO_SPI_ReadBytes(unsigned char *replybuf, int buflen)
    {
    GPIOB->BSRR = (1 << 6) << 16;   // Clear PORTB.6    SPI nCS
   // LL_mDelay(1);
    int low = SPI_WriteByte(0xFF);
    int replylen = (SPI_WriteByte(0xFF) << 8) | low;

    // printf("reading %d bytes from Smart.IO\n", replylen);
    if (replylen > buflen)
        replylen = -1;
    else
        {
        for (int i = 0; i < replylen; i++)
            *replybuf++ = SPI_WriteByte(0xFF);
        }
    GPIOB->BSRR = (1 << 6);          // Set PORTB.6
    return replylen;
    }

/* Initialize the hardware pins connected to Smart.IO
 */
void SmartIO_HardwareInit(void (*IRQ_ISR)(void))
    {
    smartio_IRQ_ISR = IRQ_ISR;
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    LL_SPI_Enable(SPI1);
    GPIOB->BSRR = (1 << 6);         // CS high
    SmartIO_HardReset();
    }

/* Wait until the Host INTR pin to go low as part of the read protocol
 */
void SmartIO_SPI_FinishRead(void)
    {
    while ((GPIOA->IDR & (1 << HOST_INTR_PIN)) != 0)
        ;
    }

void EXTI9_5_IRQHandler(void)
    {
    if (EXTI->PR & (1 << 9))
        {
        (*smartio_IRQ_ISR)();
        EXTI->PR = (1 << 9);
        }
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    }

/* Handle error conditions
 */
void SmartIO_Error(int n, ...)
    {
    va_list args;
    unsigned h;

    va_start(args, n);

    switch (n)
        {
    case SMARTIO_ERROR_BADRETURN:
        printf("SMARTIO_ERROR_BADRETURN\n");
        break;
    case SMARTIO_ERROR_BAD_RETURN_HANDLE:
        printf("SMARTIO_ERROR_BAD_RETURN_HANDLE\n");
        break;
    case SMARTIO_ERROR_BAD_ASYNC_HANDLE:
        printf("SMARTIO_ERROR_BAD_ASYNC_HANDLE\n");
        break;
    case SMARTIO_ERROR_CALLBACK_TABLE_TOO_SMALL:
        printf("SMARTIO_ERROR_CALLBACK_TABLE_TOO_SMALL\n");
        break;
    case SMARTIO_ERROR_CALLBACK_HANDLE_NOT_FOUND:
        h = va_arg(args, unsigned);
        printf("SMARTIO_ERROR_CALLBACK_HANDLE_NOT_FOUND handle %d\n", h);
        break;
    case SMARTIO_ERROR_BUFFER_TOOSMALL:
        printf("SMARTIO_ERROR_BUFFER_TOOSMALL\n");
        break;
    case SMARTIO_ERROR_INVALID_HANDLE2:
        printf("SMARTIO_ERROR_INVALID_HANDLE2\n");
        break;
    case SMARTIO_ERROR_COMMAND_BUFFER_OVERFLOW:
        printf("SMARTIO_ERROR_COMMAND_BUFFER_OVERFLOW\n");
        }
    }
