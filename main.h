/*
 * main.h
 *
 *  Created on: 16.08.2013
 *      Author: A.Polyakov
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "port.h"
#include "mbconfig.h"
#include "user_mb_app.h"

extern USHORT   usMDiscInStart;
#if      M_DISCRETE_INPUT_NDISCRETES%8
extern UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
extern UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif
extern USHORT   usMCoilStart;
#if      M_COIL_NCOILS%8
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif
extern USHORT   usMRegInStart;
extern USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
extern USHORT   usMRegHoldStart;
extern USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];

#endif /* MAIN_H_ */
