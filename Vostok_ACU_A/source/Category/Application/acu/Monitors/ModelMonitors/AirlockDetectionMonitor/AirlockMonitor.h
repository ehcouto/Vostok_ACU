/**
 * @file
 * @brief       Public interface to the code generated Airlock_Model module.
 *
 * @details     Refer to the AirlockMonitor.c source file for more detailed information.
 *
 * $Header: $
 *
 * @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef AIRLOCKMONITOR_H_
#define AIRLOCKMONITOR_H_

#include "Airlock_Model.h"


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --



//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void AirlockMonitor__Initialize(void);
void AirlockMonitor__Execution(void);
void AirlockMonitor__Start(void);
void AirlockMonitor__Stop(void);
void AirlockMonitor__Reset(void);
AIRLOCK_STATUS_TYPE AirlockMonitor__GetStatus(void);

#endif      /* AIRLOCKMONITOR_H_ */
