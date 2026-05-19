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
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "CycleResume.h"

// -- Other Modules --
#include "DoorMonitor.h"
#include "Interpreter.h"
#include "Regulations.h"
#include "SettingFile.h"
#include "Selector.h"
#include "SubCycle.h"
#include "Supervisor.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef PACKED struct CYCLE_RESUME_SUBCYCLE_PRIORITY_STRUCT
{
    uint8 Cycle_Resume_Subcycle_Priority;
} CYCLE_RESUME_SUBCYCLE_PRIORITY_TYPE;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief		It starts the Cycle Resume subcycle when the Supervisor goes from PAUSE to RUN
 *
 */
void CycleResume__StartSubcycle(void)
{
    SETTINGFILE_LOADER_TYPE loader;
    uint8 priority;
    uint8 rap_adv_position;
    CYCLE_NAME_TYPE cycle_name;

    cycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
    											COMPARTMENT_DEFAULT,
												Interpreter__GetStatus().Main_Cycle[COMPARTMENT_DEFAULT]);


    rap_adv_position = Regulations__GetPositionByFunctionID(FUNC_ID_RAPID_ADVANCE_MODE, COMPARTMENT_DEFAULT);

    if ((cycle_name < CYCLE_NAME_EOL_0) &&
		((rap_adv_position == (uint8)REGULATION_INVALID) || (Regulations__GetMainValueByPosition(rap_adv_position) == 0)))
    {
		// Get the Cycle Resume subcycle priority from the setting file.
		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA,	DSPL_CYCLE_RESUME_SUBCYCLE_PRIORITY, &loader) == PASS)
		{
			priority = ((const CYCLE_RESUME_SUBCYCLE_PRIORITY_TYPE *)loader.Data)->Cycle_Resume_Subcycle_Priority;	//lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

			if (priority != 0U)
			{
				if (Interpreter__IsSubCycleRunning(COMPARTMENT_DEFAULT) != FALSE)
				{
				    CYCLE_NAME_TYPE subcycle_name;
				    subcycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
				    											COMPARTMENT_DEFAULT,
																Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);
				    if (subcycle_name != CYCLE_NAME_DEMO_0)
				    {
					    if (subcycle_name == CYCLE_NAME_SPECIFIC_CYCLE_RESUME)
						{
							(void)Interpreter__TerminateSubCycle(0);
						}

						(void)SubCycle__RequestStart(0, priority);
				    }
				    else
				    {
				    	// Do not execute Cycle Resume if Demo subcycle is running.
				    }
				}
				else
				{
					(void)SubCycle__RequestStart(0, priority);
				}
			}
		}
    }
    else
    {
    	// Do not execute Cycle Resume if:
    	// 	Factory OR Service OR Demo cycle is running.
    	//	OR
    	// 	Rapid Advance Mode is active.
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief		It checks if Rapid Advance Option is active and if a request to advance step has been made.
 *  			It is called in RUN_ENTRY but it has no effect if RUN_ENTRY is called inside EventStart(),
 *  			because it is preceded by the macro START in which DoorMonitor__ResetStepAdvRequest() is called,
 *  			that resets any pending request of step advancing. So a step advance request can be executed only
 *  			if RUN_ENTRY is called inside EventResume() (in other words, if the previous state is STATE_SUPERVISOR_PAUSE).
 *  			Note that calling this routine inside macro RESUME wouldn't work, because when RESUME is called
 *  			Supervisor_Data[Current_Compartment].State is still STATE_SUPERVISOR_PAUSE, so Supervisor__SetStep()
 *  			has no effect.
 *  @Warning	Based on the considerations above, working of this routine strongly depends on the architecture
 *  			of the calls of the Supervisor's macros.
 *
 */
void CycleResume__CheckStepAdvance(void)
{
    uint8 rap_adv_position;
    STEP_ADV_REQUEST_TYPE step_adv_request;

    rap_adv_position = Regulations__GetPositionByFunctionID(FUNC_ID_RAPID_ADVANCE_MODE, COMPARTMENT_DEFAULT);

    if ((rap_adv_position != (uint8)REGULATION_INVALID) && (Regulations__GetMainValueByPosition(rap_adv_position) > 0))
    {
    	// Rapid Advance Mode active, a check has to be made if a Step Advance has been requested during PAUSE state.

    	step_adv_request = DoorMonitor__GetStepAdvRequest();
    	if ((uint8)step_adv_request.Requested == (uint8)TRUE)
    	{
    		(void)Supervisor__SetStep(COMPARTMENT_DEFAULT, step_adv_request.Step_Value);
    	}

    }

    // Once processed, request has to be reset.
    DoorMonitor__ResetStepAdvRequest();
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



