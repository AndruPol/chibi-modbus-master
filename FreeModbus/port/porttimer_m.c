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
 * File: $Id: porttimer.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <ch.h>
#include <hal.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "port.h"

/* ----------------------- Defines ------------------------------------------*/

#define GPTDRIVER		GPTD4
static void mtimerHandler(GPTDriver *gptp);

/* ----------------------- Static variables ---------------------------------*/
/*
 * GPT configuration: 10uS tick
 */
static const GPTConfig gptcfg = {
		100000, /* 100kHz timer clock.*/
		mtimerHandler, /* Timer callback.*/
		0,
		0 };

static systime_t timerout = 0;

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Start implementation -----------------------------*/
static void mtimerHandler(GPTDriver *gptp) {
	(void) gptp;

	chSysLockFromISR();
	vMBPortSetWithinException(TRUE);

	pxMBMasterPortCBTimerExpired();

	vMBPortSetWithinException(FALSE);
	chSysUnlockFromISR();
}

BOOL xMBMasterPortTimersInit(USHORT usTim1Timerout50us) {
	timerout = usTim1Timerout50us * ((500 * 1000) / gptcfg.frequency);
	gptStart(&GPTDRIVER, &gptcfg);
	return TRUE;
}

void vMBMasterPortTimersT35Enable() {
	/* Set current timer mode, don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_T35);

	if (bMBPortIsWithinException()) {
		gptStopTimerI(&GPTDRIVER);
		gptStartOneShotI(&GPTDRIVER, timerout);
	} else {
		gptStopTimer(&GPTDRIVER);
		gptStartOneShot(&GPTDRIVER, timerout);
	}
}

void vMBMasterPortTimersConvertDelayEnable() {
	systime_t timer_tick = MB_MASTER_DELAY_MS_CONVERT * gptcfg.frequency / 1000;

	/* Set current timer mode, don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);

	if (bMBPortIsWithinException()) {
		gptStopTimerI(&GPTDRIVER);
		gptStartOneShotI(&GPTDRIVER, timer_tick);
	} else {
		gptStopTimer(&GPTDRIVER);
		gptStartOneShot(&GPTDRIVER, timer_tick);
	}
}

void vMBMasterPortTimersRespondTimeoutEnable() {
	systime_t timer_tick = MB_MASTER_TIMEOUT_MS_RESPOND * gptcfg.frequency
			/ 1000;

	/* Set current timer mode, don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);

	if (bMBPortIsWithinException()) {
		gptStopTimerI(&GPTDRIVER);
		gptStartOneShotI(&GPTDRIVER, timer_tick);
	} else {
		gptStopTimer(&GPTDRIVER);
		gptStartOneShot(&GPTDRIVER, timer_tick);
	}
}

void vMBMasterPortTimersDisable() {
	if (bMBPortIsWithinException())
		gptStopTimerI(&GPTDRIVER);
	else
		gptStopTimer(&GPTDRIVER);
}
#endif

void vMBMasterPortTimerClose(void) {
	vMBMasterPortTimersDisable();
}
