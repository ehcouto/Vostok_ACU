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
// -- This Module --
#include "SystemConfig.h"
#include "PORTimeMonitor.h"

// -- Other Modules --
#include "DoorOpeningSystemMonitor.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Micro.h"
#include "Regulations.h"
#include "Selector.h"
#include "SensedDrainProcess.h"
#include "Supervisor.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef enum POR_TIME_STATUS_ENUM
{
	POR_TIME_STATUS_SHORT = 0,
	POR_TIME_STATUS_LONG
}POR_TIME_STATUS_TYPE;

static BOOL_TYPE Test_POR_Time;
volatile uint16 POR_LLI_Data; // Used for diagnostic

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
void PORTimeMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Test_POR_Time = TRUE;
		POR_LLI_Data = 0xFFFF;
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      DoorMonitor__Execution
 *  @details    Function used to perform the door monitor.
 *
 */
void PORTimeMonitor__Execution(void)
{
	if ((uint8)Test_POR_Time == (uint8)TRUE)
	{
		uint8 por_input_gi_index;
		uint8 por_input_sequence_id;
		BOOL_TYPE wait_por_input;
		POR_TIME_STATUS_TYPE last_por_time_status;

		last_por_time_status = POR_TIME_STATUS_LONG;

		por_input_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_ENTRAPMENT_INPUT_POR);
		if (por_input_gi_index != (uint8)HBL_INDEX_INVALID)
		{
			por_input_sequence_id = Hbl__GetGISequenceIDByIndex(por_input_gi_index);

			if (por_input_sequence_id > (uint8)0)
			{
				wait_por_input = FALSE;
				last_por_time_status = (POR_TIME_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex(por_input_gi_index);
				POR_LLI_Data = (*(uint16*)Hbl__GetGIDataByID(HBL_GI_ENTRAPMENT_INPUT_POR));
			}
			else
			{
				wait_por_input = TRUE;
			}
		}
		else
		{
			wait_por_input = FALSE;
		}

		if (wait_por_input == FALSE)
		{
			if (last_por_time_status == POR_TIME_STATUS_LONG)
			{
				uint8 position_life_test_mode = Regulations__GetPositionByFunctionID(FUNC_ID_LIFE_TEST_MODE, 0);
				uint8 position_rapid_advance_mode = Regulations__GetPositionByFunctionID(FUNC_ID_RAPID_ADVANCE_MODE, 0);
				uint8 cancel_drain_cycle_pos = Selector__GetCycleByName(0, COMPARTMENT_DEFAULT, CYCLE_NAME_SPECIFIC_CANCEL_DRAIN);
				CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);
				CYCLE_STATE_TYPE label = (Selector__GetCycleState(0, COMPARTMENT_DEFAULT, cps.Cycle));

				if ( ((position_life_test_mode == (uint8)REGULATION_INVALID) || (Regulations__GetMainValueByPosition(position_life_test_mode) == (sint32)OFF)) &&
					 ((position_rapid_advance_mode == (uint8)REGULATION_INVALID) || (Regulations__GetMainValueByPosition(position_rapid_advance_mode) == (sint32)OFF)) &&
					 ((cancel_drain_cycle_pos == (uint8)0xFF) || (cps.Cycle != cancel_drain_cycle_pos)) &&
					 ((label == CYCLE_STATE_RUN) || (label == CYCLE_STATE_DELAY))
				)
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

				// Long Power Loss: reset Drain Confirmed flag
				SensedDrain__SetDrainConfirmed(FALSE);
			}

			Test_POR_Time = FALSE;
		}
	}
	else
	{
		uint8 por_pin_state;
		por_pin_state = (uint8)ON;

		(void)Hbl__SetLoadRequestByLoadID(HBL_POR_TIME_CAPTURE_CIRCUIT, HBL_PRIORITY_POR_TIME_MONITOR, &por_pin_state);
	}

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

