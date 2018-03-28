/*
 * FreeModbus Libary: Atmel AT91SAM3S Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: portserial.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <ch.h>
#include <hal.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"

/* ----------------------- Defines ------------------------------------------*/
#define USART_INVALID_PORT      ( 0xFF )
#define USART_NOT_RE_IDX        ( 3 )
#define USART_DE_IDX            ( 4 )
#define USART_IDX_LAST          ( 1 ) // only one usart

#define UARTDRIVER				UARTD1

#if defined (STM32F103xB)
#define BOARD_SERIAL_ALTERNATE	PAL_MODE_STM32_ALTERNATE_PUSHPULL
#define	BOARD_SERIAL_INPUT		PAL_MODE_INPUT
#define	BOARD_SERIAL_OUTPUT		PAL_MODE_OUTPUT_PUSHPULL
#define BOARD_SERIAL_PORT		GPIOA				// USART1 tx/rx port
#define BOARD_SERIAL_TX			GPIOA_MB_TX			// TX - PA9
#define BOARD_SERIAL_RX			GPIOA_MB_RX			// RX - PA10
#define BOARD_MAX485			0					// MAX485 TX enable control
#define BOARD_MAX485_PORT		GPIOA				// PA12
#define BOARD_MAX485_PIN		GPIOA_MB_TX_EN		// PA12
#elif defined (STM32L152xB)
#define BOARD_SERIAL_ALTERNATE	(PAL_MODE_ALTERNATE(7) | PAL_STM32_OSPEED_HIGHEST)
#define	BOARD_SERIAL_INPUT		(PAL_MODE_INPUT | PAL_STM32_OSPEED_HIGHEST)
#define	BOARD_SERIAL_OUTPUT		(PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST)
#define BOARD_SERIAL_PORT		GPIOA				// USART1 tx/rx port
#define BOARD_SERIAL_TX			GPIOA_MB_TX			// TX - PA9
#define BOARD_SERIAL_RX			GPIOA_MB_RX			// RX - PA10
#define BOARD_MAX485			1					// MAX485 TX enable control
#define BOARD_MAX485_PORT		GPIOA				// PA11
#define BOARD_MAX485_PIN		GPIOA_MB_TX_EN		// PA11
#endif

/* ----------------------- Static variables ---------------------------------*/
static void stxBufferEmpty(UARTDriver *uartp);
static void srxChar(UARTDriver *uartp, uint16_t c);

static UCHAR ucUsedPort = USART_INVALID_PORT;
static UCHAR oneByteAccum = 0; // should we use a circular buffer ?

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

static UARTConfig uartCfg = {
	NULL,
	stxBufferEmpty,
	NULL,
	srxChar,
	NULL,
	9600,
	0,
	USART_CR2_LINEN,
	0 // no rts cts
};

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity) //, UCHAR nbBitsStop
{

	BOOL bStatus = FALSE;

#if BOARD_MAX485
	palSetPadMode(BOARD_MAX485_PORT, BOARD_MAX485_PIN, BOARD_SERIAL_OUTPUT);
	palClearPad(BOARD_MAX485_PORT, BOARD_MAX485_PIN);
#endif

	palSetPadMode(BOARD_SERIAL_PORT, BOARD_SERIAL_TX, BOARD_SERIAL_ALTERNATE);
	palSetPadMode(BOARD_SERIAL_PORT, BOARD_SERIAL_RX, BOARD_SERIAL_INPUT);

	if (ucPORT <= USART_IDX_LAST) {
		bStatus = TRUE;
		uartCfg.speed = ulBaudRate;

		switch ( eParity ) {
			case MB_PAR_NONE:
			break;
			case MB_PAR_ODD:
			uartCfg.cr1 |= (USART_CR1_PCE|USART_CR1_PS); // parity odd
			break;
			case MB_PAR_EVEN:
			uartCfg.cr1 |= USART_CR1_PCE;// parity even
			break;
			default:
			bStatus = FALSE;
			break;
		}

		switch ( ucDataBits ) {
			case 8:
			if (eParity != MB_PAR_NONE) {
				uartCfg.cr1 |= USART_CR1_M; // 8 bit + parity : we should put usart in 9 bits mode
			}
			break;
			default:
			bStatus = FALSE;
		}

		if (bStatus == TRUE) {
			uartStart(&UARTDRIVER, &uartCfg);
		}
	}
	ucUsedPort = ucPORT;
	return bStatus;
}

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	USART_TypeDef *u = UARTDRIVER.usart;

#if BOARD_MAX485
	if( xRxEnable ) {
		palClearPad(BOARD_MAX485_PORT, BOARD_MAX485_PIN);
	} else {
		palSetPad(BOARD_MAX485_PORT, BOARD_MAX485_PIN);
	}
#endif

	if( xRxEnable ) {
		(u->CR1) |= USART_CR1_RXNEIE;
	} else {
		(u->CR1) &= ~USART_CR1_RXNEIE;
	}

	if( xTxEnable ) {
		(u->CR1) |= USART_CR1_TCIE;
		pxMBFrameCBTransmitterEmpty();
	} else {
		(u->CR1) &= ~USART_CR1_TCIE;
	}
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
	CHAR toSend = ucByte;

	if (bMBPortIsWithinException() == TRUE) {
		uartStartSendI (&UARTDRIVER, 1, &toSend);
	} else {
		uartStartSend (&UARTDRIVER, 1, &toSend);
	}
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
	*pucByte = oneByteAccum;
	return TRUE;
}

#endif

static void stxBufferEmpty(UARTDriver *uartp) {
	(void) uartp;

	chSysLockFromISR();
	vMBPortSetWithinException(TRUE);

	if (pxMBFrameCBTransmitterEmpty() == TRUE)
		rescheduleJbus485FromIsr();

	vMBPortSetWithinException(FALSE);
	chSysUnlockFromISR();

}

static void srxChar(UARTDriver *uartp, uint16_t c) {
	(void) uartp;
	(void) c;

	oneByteAccum = (UCHAR) c;

	chSysLockFromISR();
	vMBPortSetWithinException(TRUE);

	pxMBFrameCBByteReceived();
	rescheduleJbus485FromIsr();

	vMBPortSetWithinException(FALSE);
	chSysUnlockFromISR();
}

void vMBPortSerialClose(void) {
	if (ucUsedPort != USART_INVALID_PORT) {
#if BOARD_MAX485
		palClearPad(BOARD_MAX485_PORT, BOARD_MAX485_PIN);
#endif
		ucUsedPort = USART_INVALID_PORT;
	}
}
