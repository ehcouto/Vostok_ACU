/**
 *  @file       OverfillMonitor.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "OverfillMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Micro.h"
#include "Selector.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
typedef enum OVERFILL_STATUS_ENUM
{
    OVERFILL_STATUS_OPEN = 0,
    OVERFILL_STATUS_CLOSED
}OVERFLOW_STATUS_TYPE;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static OVERFLOW_STATUS_TYPE Overfill_Input;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//static void OverfillFaultObserver(uint8 fault_monitor_id);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module OverfillMonitor and its variables
 *
 */
void OverfillMonitor__Initialize(void)
{
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
    	Overfill_Input = OVERFILL_STATUS_CLOSED;
    }
}

void OverfillMonitor__Execution(void)
{
    uint8 overfill_seq_id;

	// Need to evaluate Overflow GI Sequence id before evaluating its output
    overfill_seq_id = Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_OVERFLOW));
 	if (overfill_seq_id != (uint8)0)
 	{
 		Overfill_Input = (OVERFLOW_STATUS_TYPE)(*(uint8* )Hbl__GetGIDataByIndex(Hbl__GetGIIndexByGIID(HBL_GI_OVERFLOW)));

		if (OVERFILL_STATUS_OPEN == Overfill_Input)		// Fault condition
		{
		    CYCLE_PHASE_STATEMENT_TYPE cps;
		    CYCLE_STATE_TYPE cycle_state;
		    uint8 cycle;

			cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);

			cycle_state = Selector__GetCycleState(0, COMPARTMENT_DEFAULT, cps.Cycle);

			if ((cycle_state == CYCLE_STATE_DELAY) || (cycle_state == CYCLE_STATE_PAUSE))
			{
				cycle = cps.Phase;
			}
			else
			{
				cycle = cps.Cycle;
			}

		    if (Selector__GetCycleGroup(0, COMPARTMENT_DEFAULT, cycle) == CYCLE_GROUP_EOL_TEST)
			{
				Fault__ConditionDetected(FAULT_ID_OVERFILL_FINDLAY_FACTORY);
			}
			else
			{
				Fault__ConditionDetected(FAULT_ID_OVERFILL);
			}
		}
		else
		{
			Fault__ConditionRemoved(FAULT_ID_OVERFILL);

			if (Fault__IsInFaultState(FAULT_ID_OVERFILL_FINDLAY_FACTORY) == FALSE)
			{
				// Do not remove fault if it's already in FAULT state, so it is kept through all the test cycle.
				Fault__ConditionRemoved(FAULT_ID_OVERFILL_FINDLAY_FACTORY);
			}
		}
 	}
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

