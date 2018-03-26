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
#include "mb_m.h"
#include "mbport.h"
#include "port.h"

/* ----------------------- Variables ----------------------------------------*/
static msg_t xMasterQueueBuf;
static mailbox_t xMasterQueueHdl;
static binary_semaphore_t xMasterRunRes;

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBMasterPortEventInit(void) {
	chMBObjectInit(&xMasterQueueHdl, &xMasterQueueBuf, 1);
	return true;
}

BOOL xMBMasterPortEventPost(eMBMasterEventType eEvent) {

	if (bMBPortIsWithinException() == TRUE) {
		if (chMBPostI(&xMasterQueueHdl, (msg_t) eEvent) != MSG_OK)
			return false;
	} else {
		if (chMBPost(&xMasterQueueHdl, (msg_t) eEvent, MS2ST(50)) != MSG_OK)
			return false;
	}

	return true;
}

BOOL xMBMasterPortEventGet(eMBMasterEventType * peEvent) {
	eMBMasterEventType recvedEvent;

	if (bMBPortIsWithinException() == TRUE) {
		if (chMBFetchI(&xMasterQueueHdl, (msg_t *) &recvedEvent) != MSG_OK)
			return false;
		if ((recvedEvent & EV_MASTER_READY) || (recvedEvent & EV_MASTER_FRAME_RECEIVED) ||
			(recvedEvent & EV_MASTER_EXECUTE) || (recvedEvent & EV_MASTER_FRAME_SENT) ||
			(recvedEvent & EV_MASTER_ERROR_PROCESS)) {
			*peEvent = recvedEvent;
			return true;
		} else {
			chMBPostI(&xMasterQueueHdl, (msg_t) recvedEvent);
		}
	} else {
		do {
			chMBFetch(&xMasterQueueHdl, (msg_t *) &recvedEvent, TIME_INFINITE);
			chSysLock();
			if ((recvedEvent & EV_MASTER_READY) || (recvedEvent & EV_MASTER_FRAME_RECEIVED) ||
					(recvedEvent & EV_MASTER_EXECUTE) || (recvedEvent & EV_MASTER_FRAME_SENT) ||
					(recvedEvent & EV_MASTER_ERROR_PROCESS)) {
				*peEvent = recvedEvent;
				chSysUnlock();
				return true;
			}
			chMBPostS(&xMasterQueueHdl, (msg_t) recvedEvent, TIME_IMMEDIATE);
			chSysUnlock();
			chThdSleepMilliseconds(10);
		} while (true);
	}

	return false;
}

/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit(void) {
	chBSemObjectInit(&xMasterRunRes, false);
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBMasterRunResTake(LONG lTimeOut) {
	/*If waiting time is -1 .It will wait forever */
	// TODO: lTimeOut -?
	return chBSemWaitTimeout(&xMasterRunRes, lTimeOut) == MSG_OK;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease(void) {
	/* release resource */
	chBSemSignal(&xMasterRunRes);
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress,
		const UCHAR* pucPDUData, USHORT ucPDULength) {
	(void) ucDestAddress;
	(void) pucPDUData;
	(void) ucPDULength;
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);

	/* You can add your code under here. */

}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData,
		USHORT ucPDULength) {
	(void) ucDestAddress;
	(void) pucPDUData;
	(void) ucPDULength;
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);

	/* You can add your code under here. */

}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress,
		const UCHAR* pucPDUData, USHORT ucPDULength) {
	(void) ucDestAddress;
	(void) pucPDUData;
	(void) ucPDULength;
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	xMBMasterPortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);

	/* You can add your code under here. */

}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestSuccess(void) {
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCESS);

	/* You can add your code under here. */

}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish(void) {
	eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
	eMBMasterEventType recvedEvent;

	/* waiting for OS event */
	do {
		chMBFetch(&xMasterQueueHdl, (msg_t *) &recvedEvent, TIME_INFINITE);
		chSysLock();
		if ((recvedEvent & EV_MASTER_PROCESS_SUCESS) || (recvedEvent & EV_MASTER_ERROR_RESPOND_TIMEOUT) ||
			(recvedEvent & EV_MASTER_ERROR_RECEIVE_DATA) || (recvedEvent & EV_MASTER_ERROR_EXECUTE_FUNCTION)) {
			chSysUnlock();
			break;
		}
		chMBPostS(&xMasterQueueHdl, (msg_t) recvedEvent, TIME_IMMEDIATE);
		chSysUnlock();
		chThdSleepMilliseconds(10);
	} while (true);

	switch (recvedEvent) {
	case EV_MASTER_PROCESS_SUCESS:
		break;
	case EV_MASTER_ERROR_RESPOND_TIMEOUT:
		eErrStatus = MB_MRE_TIMEDOUT;
		break;
	case EV_MASTER_ERROR_RECEIVE_DATA:
		eErrStatus = MB_MRE_REV_DATA;
		break;
	case EV_MASTER_ERROR_EXECUTE_FUNCTION:
		eErrStatus = MB_MRE_EXE_FUN;
		break;
	default:
		xMBMasterPortEventPost(recvedEvent);
		break;
	}

	return eErrStatus;
}

void vMBMasterPortEventClose(void) {
	chMBReset(&xMasterQueueHdl);
	chBSemReset(&xMasterRunRes, false);
}
