/**
 * @file		DoorOpeningSystemMonitor.h
 * @brief       Public interface to the Door Opening System Monitor module.
 *
 */

#ifndef DOOROPENINGSYSTEMMONITOR_H_
#define DOOROPENINGSYSTEMMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "C_Extensions.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Setting file data structure for pointer SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, displacement DSPL_DOOR_OPENING_SYSTEM
typedef PACKED struct DOOR_OPENING_SYSTEM_SF_PARAMS_STRUCT
{
	uint8 Pull_Off_Time;
	uint8 Pull_On_Time;
	uint8 Push_1st_On_Time;
	uint8 Push_Off_Time;
	uint8 Push_2nd_On_Time;
	uint8 Push_Pre_On_Time;
} DOOR_OPENING_SYSTEM_SF_PARAMS_TYPE;

//! States of the Dos Push procedure
typedef enum DOS_PUSH_STATE_ENUM
{
	/* Don't change this order */
	DOS_PUSH_PRE_ON_STEP	 = 0,
	DOS_PUSH_FST_ON_STEP,
	DOS_PUSH_OFF_STEP,
	DOS_PUSH_SND_ON_STEP,
	DOS_PUSH_COMPLETE
} DOS_PUSH_STATE_TYPE;

//! States of the Dos Pull procedure
typedef enum DOS_PULL_STATE_ENUM
{
	/* Don't change this order */
	DOS_PULL_REQUIRED	= 0,
	DOS_PULL_OFF_STEP,
	DOS_PULL_ON_STEP,
	DOS_PULL_COMPLETE
} DOS_PULL_STATE_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DoorOpeningSystemMonitor__Initialize(void);
void DoorOpeningSystemMonitor__Execution(void);
void DoorOpeningSystemMonitor_CheckIfPullNeeded(void);
void DoorOpeningSystemMonitor__StartPull(uint8 time);
BOOL_TYPE DoorOpeningSystemMonitor__IsDoorOpenedByDOS(void);
void DoorOpeningSystemMonitor__SetDoorOpenedByDOS(BOOL_TYPE value);
DOS_PUSH_STATE_TYPE DoorOpeningSystemMonitor__GetDOSPushState(void);
void DoorOpeningSystemMonitor__SetDOSPushState(DOS_PUSH_STATE_TYPE value);
DOS_PULL_STATE_TYPE DoorOpeningSystemMonitor__GetDOSPullState(void);
void DoorOpeningSystemMonitor__SetDOSPullState(DOS_PULL_STATE_TYPE value);
BOOL_TYPE DoorOpeningSystemMonitor__IsDOSPinFullyRetracted(void);


#endif /* DOOROPENINGSYSTEMMONITOR_H_ */
