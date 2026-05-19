/**
 *  @file		OWIVoltageRegulatedOutput.c
 *
 *  @brief      Wrapper for the code generated OWI Voltage Regulated Output model
 *
 *  @details
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "OWIVoltageRegulatedOutput.h"
#include "OWIVoltageRegulatedOutputModel.h"

#include "Micro.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! List of possible requests for the OWI VRO Monitor, sorted in ascending order of priority. Do not change the order.
typedef enum OWI_VRO_PRIORITIZED_REQUEST_ENUM
{
	OWI_VRO_REQUEST_NONE = 0,
	OWI_VRO_REQUEST_RESET_IF_DISABLED,
	OWI_VRO_REQUEST_KEEP_CURRENT_STATUS,
	OWI_VRO_REQUEST_STOP,
	OWI_VRO_REQUEST_RESET
} OWI_VRO_PRIORITIZED_REQUEST_TYPE;

static RT_MODELOWIVOLTAGEREGULATE_TYPE OWI_VRO_RTModelData;

//! Model Inputs

//! Model Outputs

//! Local Variables
static OWI_VRO_PRIORITIZED_REQUEST_TYPE OWI_VRO_Request;
static BOOL_TYPE OWI_VRO_Model_Enabled;
static uint8 OWI_VRO_Setpoint_Index;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void OWIVoltageRegulatedOutputReset(void);
static void OWIVoltageRegulatedOutputStop(void);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      OWIVoltageRegulatedOutput__Initialize
 *  @details    Function used to initialize the monitor.
 *
 */
void OWIVoltageRegulatedOutput__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		OWIVoltageRegulatedOutputModel__Initialize(&OWI_VRO_RTModelData);
		Initialize(&OWI_VRO_RTModelData);

		OWI_VRO_Model_Enabled = FALSE;
		OWI_VRO_Request = OWI_VRO_REQUEST_NONE;
		OWI_VRO_Setpoint_Index = 0;
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      OWIVoltageRegulatedOutput__Execution
 *  @details    Function used to perform the monitor.
 *
 */
void OWIVoltageRegulatedOutput__Execution(void)
{
	// Process requests by Task and Process
	switch (OWI_VRO_Request)
	{
		case OWI_VRO_REQUEST_RESET:
			OWIVoltageRegulatedOutputReset();
			break;

		case OWI_VRO_REQUEST_KEEP_CURRENT_STATUS:
			// Nothing to do
			break;

		case OWI_VRO_REQUEST_RESET_IF_DISABLED:
			if (OWI_VRO_Model_Enabled == FALSE)
			{
				OWIVoltageRegulatedOutputReset();
			}
			break;

		default: // Includes OWI_VRO_REQUEST_STOP
			OWIVoltageRegulatedOutputStop();
			break;
	}


	// Reset all the requests for next iteration
	OWI_VRO_Request = OWI_VRO_REQUEST_NONE;

	// Run Model
	Monitor(&OWI_VRO_RTModelData);
}

uint8 OWIVoltageRegulatedOutput__GetSequenceId(void)
{
	uint8 sequence_id;

	GetSequenceId(&OWI_VRO_RTModelData, &sequence_id);
	return sequence_id;
}

void OWIVoltageRegulatedOutput__RequestReset(uint8 setpoint_index)
{
	if (OWI_VRO_Request < OWI_VRO_REQUEST_RESET)
	{
		OWI_VRO_Request = OWI_VRO_REQUEST_RESET;
		OWI_VRO_Setpoint_Index = setpoint_index;
	}
}

void OWIVoltageRegulatedOutput__RequestStop(void)
{
	if (OWI_VRO_Request < OWI_VRO_REQUEST_STOP)
	{
		OWI_VRO_Request = OWI_VRO_REQUEST_STOP;
	}
}

void OWIVoltageRegulatedOutput__RequestKeepRunning(void)
{
	if (OWI_VRO_Request < OWI_VRO_REQUEST_KEEP_CURRENT_STATUS)
	{
		OWI_VRO_Request = OWI_VRO_REQUEST_KEEP_CURRENT_STATUS;
	}
}

void OWIVoltageRegulatedOutput__RequestResetIfDisabled(void)
{
	if (OWI_VRO_Request < OWI_VRO_REQUEST_RESET_IF_DISABLED)
	{
		OWI_VRO_Request = OWI_VRO_REQUEST_RESET_IF_DISABLED;
	}
}


void OWIVoltageRegulatedOutput__ResetSetpoint(void)
{
	OWI_VRO_Setpoint_Index = 0;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

static void OWIVoltageRegulatedOutputReset(void)
{
	// It is supposed to reset the model and start it.
	// It is supposed to be called as a consequence of the following events:
	// - The first time (after a reset) the model needs to be activated, being requested by the task or by a process.
	// - When the model is already running due to the task, but a process is called and has to take over (at the end of the Initial Delay).
	// - During the execution of a process, when the Delay Between Samples has expired and the acquisition of a new sample has to start.

	OWI_VRO_Model_Enabled = TRUE;
	Reset(&OWI_VRO_RTModelData, OWI_VRO_Setpoint_Index);
}

static void OWIVoltageRegulatedOutputStop(void)
{
	// It is supposed to stop the model.
	// It is supposed to be called as a consequence of the following events:
	// - when neither a process nor the task is requesting it to run.
	// - during a process, as soon as a sample has been acquired.

	OWI_VRO_Model_Enabled = FALSE;
	Stop(&OWI_VRO_RTModelData);
}
