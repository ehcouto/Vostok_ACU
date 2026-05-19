/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Public interface to the SRDigitalOutputPilot module.
 *
 * @details     Refer to the SRDigitalOutputPilot.c source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SRDIGITALOUTPUTPILOT_H_
#define SRDIGITALOUTPUTPILOT_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SRSystemConfig.h"
#ifndef HBL_PILOT_NUM_SRDIGITALOUTPUT
#define HBL_PILOT_NUM_SRDIGITALOUTPUT 0
#endif
#if (HBL_PILOT_NUM_SRDIGITALOUTPUT > 0)

// -- Other Modules --
#include "HblDefs.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Macro Declarations --

//! A list that describes the functions and data size for the SRDigitalOutputPilot.
#define SRDIGITALOUTPUTPILOT_REGISTER           \
    SRDigitalOutputPilot__Initialize,           \
    SRDigitalOutputPilot__Allocate,             \
    SRDigitalOutputPilot__InitializeInstance,   \
    SRDigitalOutputPilot__Request,              \
    SRDigitalOutputPilot__Process,              \
    SRDigitalOutputPilot__GetRequested,         \
    SRDigitalOutputPilot__GetProcessed,         \
    NULL,                                       \
    SRDigitalOutputPilot__AsynchProcess,        \
    NULL,                                       \
    NULL,                                       \
    SRDigitalOutputPilot__GetProcessed,         \
    1


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void            SRDigitalOutputPilot__Initialize            (void);
uint8           SRDigitalOutputPilot__Allocate              (void);
PASS_FAIL_TYPE  SRDigitalOutputPilot__InitializeInstance    (HBL_LOAD_TYPE* load);
PASS_FAIL_TYPE  SRDigitalOutputPilot__Request               (HBL_LOAD_TYPE* load, void* data);
PASS_FAIL_TYPE  SRDigitalOutputPilot__Process               (HBL_LOAD_TYPE* load);
void*           SRDigitalOutputPilot__GetRequested          (HBL_LOAD_TYPE* load);
void*           SRDigitalOutputPilot__GetProcessed          (HBL_LOAD_TYPE* load);
PASS_FAIL_TYPE  SRDigitalOutputPilot__AsynchProcess         (HBL_LOAD_TYPE* load);

void            SRDigitalOutputPilot__CheckDataIntegrity    (void);


#endif      // #if (HBL_PILOT_NUM_SRDIGITALOUTPUT > 0)
#endif      // SRDIGITALOUTPUTPILOT_H_
