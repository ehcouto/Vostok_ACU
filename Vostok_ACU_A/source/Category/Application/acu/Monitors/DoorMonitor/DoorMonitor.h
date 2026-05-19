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

#ifndef DOORMONITOR_H_
#define DOORMONITOR_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --

typedef enum DOOR_STATUS_ENUM
{
    DOOR_STATUS_OPEN = 0,
    DOOR_STATUS_CLOSE,
	DOOR_STATUS_UNKNOWN
}DOOR_STATUS_TYPE;

extern DOOR_STATUS_TYPE Door_Status;

//! Structure to hold the information if a request to advance step has been made, and to which step.
typedef struct STEP_ADV_REQUEST_STRUCT
{
	BOOL_TYPE Requested;
	uint8 Step_Value;
} STEP_ADV_REQUEST_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void DoorMonitor__Initialize(void);
void DoorMonitor__Execution(void);
void DoorMonitor__ResetStepAdvRequest(void);
STEP_ADV_REQUEST_TYPE DoorMonitor__GetStepAdvRequest(void);
DOOR_STATUS_TYPE DoorMonitor__GetDoorStatus(void);

#endif      /* _DOORMONITOR_H_ */
