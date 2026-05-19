/**
 * @file
 * @brief       Public interface to the DoorMonitor module.
 *
 * @details     Refer to the DoorMonitor.c source file for more detailed information.
 *
 * $Header: DoorMonitor.h 1.1 2016/06/30 14:17:35EDT Antonio D'Agostino (DAGOSA) Exp  $
 *
 * @copyright  Copyright 2015 - $Date: 2016/06/30 14:17:35EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef LITTMONITOR_H
#define LITTMONITOR_H


//  --- Include Files -------------------------------------------------------------------------------------------------
#include "C_Extensions.h"

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --

typedef enum
{
	LITT_ON = 2,
	LITT_OFF = 1
}LITT_STATUS_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void LITTMonitor__Initialize(void);
void LITTMonitor__Execution(void);
void LITTMonitor__ResetStepAdvRequest(void);
ON_OFF_TYPE LITTMonitor__GetLITTStatus(void);
void LITTMonitor__ResetLITTTimer(void);
void LITTMonitor__RestartLITTTimer(void);


#endif      /* LITTMONITOR_H */
