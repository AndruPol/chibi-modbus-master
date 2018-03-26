/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "main.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"

#define MB_PORT			1				// NU
#define MB_MODE			MB_RTU			// RTU mode only
#define MB_BITRATE		115200			// bitrate
#define MB_PARITY		MB_PAR_EVEN		// parity

#define WAIT_FOREVER	(-1)

static THD_WORKING_AREA(waThdModbus, 384);
#define MB_PRIO		(NORMALPRIO+1)	// Modbus process priority

USHORT  usModbusUserData[MB_PDU_SIZE_MAX];
UCHAR   ucModbusUserData[MB_PDU_SIZE_MAX];

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/

/*
* The MODBUS master main thread
*/
static THD_WORKING_AREA(waThdTest, 384);
static THD_FUNCTION(thdTest, arg) {
  (void)arg;

  eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
  uint16_t errorCount = 0;

  chRegSetThreadName("MBTest");

  while (true)
  {
	  //Test Modbus Master
	  usModbusUserData[0] = (USHORT) (chVTGetSystemTimeX() / 10);
	  usModbusUserData[1] = (USHORT) (chVTGetSystemTimeX() % 10);

	  errorCode = eMBMasterReqReadDiscreteInputs(1,0,8,WAIT_FOREVER);
	  (void) ucMDiscInBuf[0];

	  errorCode = eMBMasterReqWriteCoil(1,0,0xFF00,WAIT_FOREVER);
	  errorCode = eMBMasterReqReadCoils(1,0,1,WAIT_FOREVER);
	  (void) ucMCoilBuf[0];

	  errorCode = eMBMasterReqReadInputRegister(1,0,2,WAIT_FOREVER);
	  (void) usMRegInBuf[0];

	  errorCode = eMBMasterReqWriteHoldingRegister(1,0,usModbusUserData[0],WAIT_FOREVER);
	  errorCode = eMBMasterReqWriteHoldingRegister(1,1,usModbusUserData[1],WAIT_FOREVER);

	  errorCode = eMBMasterReqReadHoldingRegister(1,0,2,WAIT_FOREVER);
	  (void) usMRegHoldBuf[0];

//      errorCode = eMBMasterReqWriteMultipleCoils(1,3,5,ucModbusUserData,WAIT_FOREVER);
//		errorCode = eMBMasterReqWriteMultipleHoldingRegister(1,3,2,usModbusUserData,WAIT_FOREVER);
//		errorCode = eMBMasterReqReadWriteMultipleHoldingRegister(1,3,2,usModbusUserData,5,2,WAIT_FOREVER);
	  if (errorCode != MB_MRE_NO_ERR) {
		  errorCount++;
	  }
	  chThdSleepMilliseconds(1000);
  }
}


bool initModbus(void) {
  eMBErrorCode eStatus;
  eStatus = eMBMasterInit(MB_MODE, MB_PORT, MB_BITRATE, MB_PARITY);
  if (eStatus != MB_ENOERR) {
	  return false;
  }

  eStatus = eMBMasterEnable();
  if (eStatus != MB_ENOERR) {
	  return false;
  }

  return true;
}


/*
* The MODBUS master main thread
*/
static THD_FUNCTION(thdModbus, arg) {
  (void)arg;

  chRegSetThreadName("MODBUS");

  while (initModbus() != TRUE) {
	chThdSleepMilliseconds(1000);

	if (chThdShouldTerminateX())
	  goto cleanAndExit;
  }

  do {
    eMBMasterPoll();
  } while (!chThdShouldTerminateX());

cleanAndExit:
  eMBMasterDisable();
  eMBMasterClose();
}

void modbus_init(void) {
  chThdCreateStatic(waThdModbus, sizeof(waThdModbus), MB_PRIO, thdModbus, NULL);
}

// called on kernel panic
void halt(void){
	port_disable();
	while(TRUE)
	{
	}
}

/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  modbus_init();
  chThdCreateStatic(waThdTest, sizeof(waThdTest), NORMALPRIO, thdTest, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and listen for events.
  */
  while (TRUE) {

    uint16_t free = chCoreGetStatusX();

    if (free)
    	chThdSleepMilliseconds(1000);

  }
}
