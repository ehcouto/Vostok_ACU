/**
 * @file
 * @brief       Public interface to the JumpIf module.
 *
 * @details     Refer to the JumpIf.c source file for more detailed information.
 *
 * $Header: JumpIf.h 1.2 2015/08/05 17:04:22EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 * @copyright  Copyright 2015 - $Date: 2015/08/05 17:04:22EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef JUMPIF_H_
#define JUMPIF_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

// -- Other Modules --
#include "Process.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --

//! A structure to hold a cycle, phase, and statement.
typedef struct CYCLE_PHASE_STATEMENT_STRUCT
{
    uint8 Cycle;
    uint8 Phase;
    uint8 Statement;
} CYCLE_PHASE_STATEMENT_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

#if (TIME_ESTIMATION_FEATURE == ENABLED)
BOOL_TYPE JumpIf__Estimate(uint8 sf_index, uint16 displacement, CYCLE_PHASE_STATEMENT_TYPE* cps);
#endif
BOOL_TYPE JumpIf__GetTarget(CYCLE_PHASE_STATEMENT_TYPE* cps);
PROCESS_RESULT_TYPE JumpIf__Process(PROCESS_CONTROL_TYPE* control);


#endif      /* JUMPIF_H_ */
