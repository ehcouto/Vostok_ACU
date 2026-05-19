/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2018-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "StandbyManager.h" 

#include "DishAuxVar.h"
#include "DoorOpeningSystemMonitor.h"
#include "FillValveStuckOn.h"
#include "Hbl.h"
#include "LITTMonitor.h"
#include "Micro.h"
#include "Mode.h"
#include "MoisturePreventionMonitor.h"
#include "NVData.h"
#include "SystemTimers.h"
#include "SRPowerMgr.h"


typedef enum STANDBY_MANAGER_STATE_ENUM
{
    STANDBY_MANAGER_IDLE                        = 0,
    STANDBY_MANAGER_INITIALIZE                  = 1, //!< (initial_state)
    STANDBY_MANAGER_LOAD_INVALID                = 2,
    STANDBY_MANAGER_RUNNING                     = 3,
    STANDBY_MANAGER_REATTEMPTS_RUNNING          = 4,
    STANDBY_MANAGER_REATTEMPTS_FAILED           = 5,
    STANDBY_MANAGER_SETLOAD_AUTOKILL_FAILED     = 6


} STANDBY_MANAGER_STATE_TYPE;

typedef enum OVERFILL_STATUS_ENUM
{
    OVERFILL_STATUS_OPEN = 0,
    OVERFILL_STATUS_CLOSED
}OVERFLOW_STATUS_TYPE;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

static uint8 StandbyManager_Autokill_Index = HBL_LOAD_INDEX_INVALID;
static BOOL_TYPE StandbyManager_Activate_Standby;
static SYSTEMTIMER_NV_TIMER_TYPE StandbyManager_Timer_Handler;
static uint8 StandbyManager_Attempt_Counter;
#if (SRPOWERMGR_LOWPOWER_MODE == 2) // DEA703, FINN
static BOOL_TYPE StandbyManager_Standby_Rejected_Attempts_Failed;
#endif
static STANDBY_MANAGER_STATE_TYPE StandbyManager_State;
static BOOL_TYPE StandbyManager_Start_Timer = FALSE;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
BOOL_TYPE IsOverfillOpen(void);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module StandbyManager and its variables
 *
 */
void StandbyManager__Initialize(void)
{

    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        StandbyManager_Autokill_Index =  Hbl__GetLoadIndexByLoadID(HBL_LOAD_AUTO_KILL);
#if (SRPOWERMGR_LOWPOWER_MODE == 2) // DEA703, FINN
        StandbyManager_Standby_Rejected_Attempts_Failed = FALSE;
#endif
        StandbyManager_Activate_Standby = FALSE;
        StandbyManager_State = STANDBY_MANAGER_INITIALIZE;
        if(StandbyManager_Autokill_Index == (uint8)HBL_LOAD_INDEX_INVALID)
        {
        	StandbyManager_State = STANDBY_MANAGER_LOAD_INVALID;
        }
    }

}



void StandbyManager__Execute(void)
{

    uint8 data_on = ON;
    PASS_FAIL_TYPE result = FAIL;

    if (StandbyManager_Autokill_Index != (uint8)HBL_LOAD_INDEX_INVALID)
    {
    	if ((Micro__GetResetMode() == MICRO_RESET_MODE_WARM) &&  (Mode__GetMode() == MODE_LOWPOWER) )
		{
			StandbyManager_Activate_Standby = TRUE;
			StandbyManager_State = STANDBY_MANAGER_RUNNING;
			Mode__SetMode(MODE_NORMAL);
			Micro__ForceReset(MICRO_RESET_MODE_WARM);
		}

		if(((uint8)StandbyManager_Activate_Standby == (uint8)TRUE) &&  (Mode__GetMode() == MODE_NORMAL) )
		{
			if( (StandbyManager_State == STANDBY_MANAGER_REATTEMPTS_RUNNING) || (StandbyManager_State ==STANDBY_MANAGER_SETLOAD_AUTOKILL_FAILED) )
			{
				if( (uint8)StandbyManager_Start_Timer == (uint8)TRUE)
				{
					(void)SystemTimers__NVSetTimerMS(&StandbyManager_Timer_Handler, STANDBY_ATTEMPT_TIMEOUT, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
					StandbyManager_Start_Timer = FALSE;
				}

				/*waiting for hardware shutdown*/
				if (SystemTimers__NVCheckTimer(&StandbyManager_Timer_Handler) == SYSTEMTIMERS_STATE_DONE)
				{
					if (StandbyManager_Attempt_Counter < (uint8)STANDBY_ATTEMPT_DEFAULT_VALUE)
					{
						result = Hbl__SetLoadRequestByLoadID(HBL_LOAD_AUTO_KILL, HBL_PRIORITY_STANDBY_MANAGER_MONITOR, &data_on);
						(void)SystemTimers__NVSetTimerMS(&StandbyManager_Timer_Handler, STANDBY_ATTEMPT_TIMEOUT, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
						StandbyManager_Attempt_Counter++;
					}
					else
					{
#if (SRPOWERMGR_LOWPOWER_MODE == 2) // DEA703, FINN
						/* hardware shutdown failed*/
						StandbyManager_Standby_Rejected_Attempts_Failed = TRUE;
#endif
						StandbyManager_Activate_Standby = FALSE;
						StandbyManager_State = STANDBY_MANAGER_REATTEMPTS_FAILED;
					}
				}
			}

			if(StandbyManager_State == STANDBY_MANAGER_RUNNING)
			{
				result = Hbl__SetLoadRequestByLoadID(HBL_LOAD_AUTO_KILL, HBL_PRIORITY_STANDBY_MANAGER_MONITOR, &data_on);
				StandbyManager_State = STANDBY_MANAGER_REATTEMPTS_RUNNING ;
				StandbyManager_Start_Timer = TRUE;
				StandbyManager_Attempt_Counter = 0;
			}

			if (result == FAIL)
			{
				StandbyManager_State = STANDBY_MANAGER_SETLOAD_AUTOKILL_FAILED;
			}

			// Check if need stop procedure
			if ((uint8)StandbyManager__StopProcedure() == (uint8)TRUE)
			{
				StandbyManager_Activate_Standby = FALSE;
			}

		}
    }
}

BOOL_TYPE StandbyManager__GetPermission(void)
{
	BOOL_TYPE retval;

#if (SRPOWERMGR_LOWPOWER_MODE == 3) // VICTORIA, LUKE
	if (((MoisturePreventionMonitor__GetStatus() == MP_DEACTIVATED) &&
	    (LITTMonitor__GetLITTStatus() == OFF) &&
		((uint8)DoorOpeningSystemMonitor__IsDOSPinFullyRetracted() == (uint8)TRUE) &&
		(IsOverfillOpen() == FALSE) &&
		(DishAuxVar__AreFlowMeterPulsesPresent() == FALSE) &&
		(FillValveStuckOn__UnexpectedPulsesDetected() == FALSE) ) ||
		(Mode__IsPCControlEngaged() != FALSE))	// When PC Control is engaged Low Power is always allowed
	{
		retval = TRUE;
	}
	else
	{
		retval = FALSE;
	}
#elif (SRPOWERMGR_LOWPOWER_MODE == 2) // DEA703, FINN
    if ((((MoisturePreventionMonitor__GetStatus() == MP_DEACTIVATED) &&
    	  (LITTMonitor__GetLITTStatus() == OFF) &&
		  ((uint8)DoorOpeningSystemMonitor__IsDOSPinFullyRetracted() == (uint8)TRUE) &&
		  (IsOverfillOpen() == FALSE) &&
		  (DishAuxVar__AreFlowMeterPulsesPresent() == FALSE) &&
		  (FillValveStuckOn__UnexpectedPulsesDetected() == FALSE)) ||
    	  (Mode__IsPCControlEngaged() != FALSE))	// When PC Control is engaged Low Power is always allowed
		 &&
		 (StandbyManager_Autokill_Index<HBL_LOAD_INDEX_INVALID) &&
         (StandbyManager_Standby_Rejected_Attempts_Failed == FALSE))
    {
    	retval = TRUE;
    }
    else
    {
       if(StandbyManager_Autokill_Index == HBL_LOAD_INDEX_INVALID)
       {
           StandbyManager_State = STANDBY_MANAGER_LOAD_INVALID;
       }
       retval = FALSE;
    }
#endif

    return retval;
}


uint8 StandbyManager__GetStatus(void)
{
    return (uint8)StandbyManager_State;
}

BOOL_TYPE StandbyManager__GetActivateStandby(void)
{
    return StandbyManager_Activate_Standby;
}

void StandbyManager__ResetActivateStandby(void)
{
    StandbyManager_Activate_Standby = FALSE;
}

BOOL_TYPE StandbyManager__StopProcedure(void)
{
	BOOL_TYPE retval = FALSE;

	if (((uint8)IsOverfillOpen() == (uint8)TRUE) || ((uint8)DishAuxVar__AreFlowMeterPulsesPresent() == (uint8)TRUE) || ((uint8)FillValveStuckOn__UnexpectedPulsesDetected() == (uint8)TRUE))
	{
		retval = TRUE;
	}

	return retval;
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

BOOL_TYPE IsOverfillOpen(void)
{
	BOOL_TYPE retval = FALSE;

	uint8 overfill_seq_id;
	OVERFLOW_STATUS_TYPE overfill_input;

	// Need to evaluate Overflow GI Sequence id before evaluating its output
	overfill_seq_id = Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_OVERFLOW));
	if (overfill_seq_id != (uint8)0)
	{
		overfill_input = (OVERFLOW_STATUS_TYPE)(*(uint8* )Hbl__GetGIDataByIndex(Hbl__GetGIIndexByGIID(HBL_GI_OVERFLOW)));

		if (OVERFILL_STATUS_OPEN == overfill_input)
		{
			retval = TRUE;
		}
	}

	return retval;
}

