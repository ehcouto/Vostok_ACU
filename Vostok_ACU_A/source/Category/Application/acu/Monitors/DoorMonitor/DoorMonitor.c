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
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "DoorMonitor.h"

#include "BrownOut.h"
#include "DoorOpeningSystemMonitor.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Micro.h"
#include "Regulations.h"
#include "Selector.h"
#include "SensedDrainProcess.h"
#include "SettingFile.h"
#include "StatisticsDish.h"
#include "Supervisor.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

DOOR_STATUS_TYPE  Door_Status;

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#pragma location = "_appliance_segment"
static DOOR_STATUS_TYPE Previous_Door_Status;

//! Structure to hold the information if a request to advance step has been made, and to which step.
static STEP_ADV_REQUEST_TYPE Step_Advance_Request;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      DoorMonitor__Initialize
 *  @details    Function used to initialize the door monitor.
 *
 */
void DoorMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Step_Advance_Request.Requested = FALSE;
		Step_Advance_Request.Step_Value = PHASE_STEP_NONE;

		// Init Door_Status
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			Previous_Door_Status = DOOR_STATUS_CLOSE;
			Door_Status = DOOR_STATUS_OPEN; // This value is used by other modules only before the first
											// valid reading after a cold reset is completed.
		}
		else
		{
			Door_Status = Previous_Door_Status; // The value saved in NV data is used by other modules only before
												// the first valid reading after a cold reset is completed.
		}
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      DoorMonitor__Execution
 *  @details    Function used to perform the door monitor.
 *
 */
void DoorMonitor__Execution(void)
{
	CYCLE_STATE_TYPE label = Selector__GetCycleState(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
													 COMPARTMENT_DEFAULT,
													 Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

	uint8 door_input_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_DOOR);
	uint8 door_input_sequence_id;

	uint8 position_life_test_mode = Regulations__GetPositionByFunctionID(FUNC_ID_LIFE_TEST_MODE, 0);
	uint8 position_rapid_advance_mode = Regulations__GetPositionByFunctionID(FUNC_ID_RAPID_ADVANCE_MODE, 0);

	if (door_input_gi_index != (uint8)HBL_INDEX_INVALID)
	{
		door_input_sequence_id = Hbl__GetGISequenceIDByIndex(door_input_gi_index);

		if ((door_input_sequence_id > 0U) && (BrownOut__IsDetected() == FALSE))
		{
			// Door status is updated only if:
			//   1. At least a valid reading has been completed after last reset.
			//   AND
			//   2. The voltage is not under the Brown Out threshold, otherwise a false OPEN status can be read.
			Door_Status = (DOOR_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex(door_input_gi_index);

			if ((Previous_Door_Status == DOOR_STATUS_OPEN) && (Door_Status == DOOR_STATUS_CLOSE))
			{
				if ((((position_life_test_mode != (uint8)REGULATION_INVALID) && (Regulations__GetMainValueByPosition(position_life_test_mode) == (sint32)ON))) ||
					(((position_rapid_advance_mode != (uint8)REGULATION_INVALID) && (Regulations__GetMainValueByPosition(position_rapid_advance_mode) == (sint32)ON))))
				{
					if (label == CYCLE_STATE_PAUSE)
					{
						(void)Supervisor__SetStart(0, FS_START_CONDITION);
					}
				}
			}
			else if ((Previous_Door_Status == DOOR_STATUS_CLOSE) && (Door_Status == DOOR_STATUS_OPEN))
			{
				StatisticsDish__IncreaseDoorOpenings();

				if((label == CYCLE_STATE_RUN) || (label == CYCLE_STATE_DELAY))
				{
					if (DoorOpeningSystemMonitor__IsDoorOpenedByDOS() == FALSE)
					{
						(void)Supervisor__SetStart(0, FS_PAUSE_CONDITION);
					}
					else
					{
						// Door opened by Door Opening System: cycle has to continue
					}
				}
			}
			else
			{
				// Nothing to do
			}

			// Align previous state
			Previous_Door_Status = Door_Status;

			if (Door_Status == DOOR_STATUS_OPEN)
			{
				// Reset Drain Confirmed flag
				SensedDrain__SetDrainConfirmed(FALSE);

				if (label == CYCLE_STATE_PAUSE)
				{
					if ((position_rapid_advance_mode != (uint8)REGULATION_INVALID) && (Regulations__GetMainValueByPosition(position_rapid_advance_mode) > (sint32)OFF))
					{
						// Rapid Advance active

						uint8 position;

						position = Regulations__GetPositionByFunctionID(FUNC_ID_ADVANCE_STEP, 0/*COMPARTMENT_DEFAULT*/);

						if ((position !=(uint8)REGULATION_INVALID) && ((uint8)Regulations__IsRegulationChanged(position) == (uint8)TRUE))
						{
							// Advance Step regulation has changed

							Step_Advance_Request.Requested = TRUE;
							Step_Advance_Request.Step_Value = (uint8)Regulations__GetRequestedMainValueByPosition(position);

							Regulations__AcceptRequestByPosition(position);
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief		It resets any pending request of step advancing. It should be called any time the STATE_SUPERVISOR_RUN
 *  			state is entered from a state different from STATE_SUPERVISOR_PAUSE. Also, it should be called in RUN_ENTRY
 *  			after the request has been processed.
 */
void DoorMonitor__ResetStepAdvRequest(void)
{
	Step_Advance_Request.Requested = FALSE;
	Step_Advance_Request.Step_Value = PHASE_STEP_NONE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief		It returns a structure containing information if a request to advance step has been made, and
 *  			to which step.
 *
 */
STEP_ADV_REQUEST_TYPE DoorMonitor__GetStepAdvRequest(void)
{
	return Step_Advance_Request;
}


/*
 * Function to return the Door_Status
 */
DOOR_STATUS_TYPE DoorMonitor__GetDoorStatus(void)
{
	return Door_Status;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

