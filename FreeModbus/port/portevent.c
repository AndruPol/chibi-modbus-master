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
 * File: $Id: portevent.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <ch.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"

/* ----------------------- Variables ----------------------------------------*/
static msg_t xSlaveQueueBuf;
static mailbox_t xSlaveQueueHdl;

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBPortEventInit(void) {
	chMBObjectInit(&xSlaveQueueHdl, &xSlaveQueueBuf, 1);
	return TRUE;
}

BOOL xMBPortEventPost(eMBEventType eEvent) {

	if (bMBPortIsWithinException() == TRUE) {
		if (chMBPostI(&xSlaveQueueHdl, (msg_t) eEvent) != MSG_OK)
			return false;
	} else {
		if (chMBPost(&xSlaveQueueHdl, (msg_t) eEvent, TIME_INFINITE) != MSG_OK)
			return false;
	}

	return true;
}

BOOL xMBPortEventGet(eMBEventType * peEvent) {
	eMBMasterEventType recvedEvent;

	if (chMBFetch(&xSlaveQueueHdl, (msg_t *) &recvedEvent, TIME_INFINITE) != MSG_OK)
		return false;

    switch (recvedEvent) {
    case EV_READY:
        *peEvent = EV_READY;
        break;
    case EV_FRAME_RECEIVED:
        *peEvent = EV_FRAME_RECEIVED;
        break;
    case EV_EXECUTE:
        *peEvent = EV_EXECUTE;
        break;
    case EV_FRAME_SENT:
        *peEvent = EV_FRAME_SENT;
        break;
    }

	return true;
}

void vMBPortEventClose(void) {
	chMBReset(&xSlaveQueueHdl);
}
