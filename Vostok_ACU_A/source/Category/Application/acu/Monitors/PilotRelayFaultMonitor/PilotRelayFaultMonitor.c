/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "PilotRelayFaultMonitor.h"
#include "PilotRelayStuckClosedMonitor.h"
#include "PilotRelayStuckOpenMonitor.h"

// -- Other Modules --
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"



//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define MODULE_NAME MODULE_PILOT_RELAY_FAULT_MONITOR

//! Time to wait after a change in pilot relay status, or in Zero Cross status or after resetting
// the fault, before evaluating again Pilot Relay consistency (unit 100 ms).
#define PILOT_RELAY_FAULT_STABILIZATION_TIME	10

typedef enum MODULE_PILOT_RELAY_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_COMPONENTS  = 1
} MODULE_PILOT_RELAY_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

//! The consistency of the Pilot Relay load
static uint8 Pilot_Relay_Consistency;
//! The consistency of the Pilot Relay load in the previous iteration
static uint8 Pilot_Relay_Prev_Consistency;

//! Counter for stabilization time.
static uint8 Pilot_Relay_Stabilization_Counter;

//! Boolean indicating if the Pilot Relay Fault Monitor is active or not
static BOOL_TYPE Pilot_Relay_Fault_Monitor_Is_Active;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      PilotRelayFaultMonitor__Initialize
 *  @details    Function used to initialize the Pilot Relay Motor Fault monitor.
 *
 */
void PilotRelayFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Pilot_Relay_Consistency = COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_INVALID, HBL_CONSISTENCY_INVALID_VALUE);
		Pilot_Relay_Prev_Consistency = COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_INVALID, HBL_CONSISTENCY_INVALID_VALUE);

		Pilot_Relay_Stabilization_Counter = PILOT_RELAY_FAULT_STABILIZATION_TIME;

		if ((Hbl__GetLoadIndexByLoadID(HBL_LOAD_PILOT_RELAY) != (uint8)HBL_INDEX_INVALID) ||
			(Hbl__GetLoadIndexByLoadID(HBL_LOAD_RELAY_ENABLE) != (uint8)HBL_INDEX_INVALID))
		{
			PilotRelayStuckClosedMonitor__Initialize();
			PilotRelayStuckOpenMonitor__Initialize();

			Pilot_Relay_Fault_Monitor_Is_Active = TRUE;
		}
		else
		{
			Pilot_Relay_Fault_Monitor_Is_Active = FALSE;
			LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
		}

	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      PilotRelayFaultMonitor__Execution
 *  @details    Function used to perform the Pilot Relay Fault monitor.
 *
 */
void PilotRelayFaultMonitor__Execution(void)
{
	uint8 pilot_relay_state;
	uint8 pilot_relay_load_index;
	static uint8 pilot_relay_prev_state = OFF;

	if ((uint8)Pilot_Relay_Fault_Monitor_Is_Active == (uint8)TRUE)
	{
		pilot_relay_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_PILOT_RELAY);
		if (pilot_relay_load_index == HBL_INDEX_INVALID)
		{
			pilot_relay_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_RELAY_ENABLE);
		}

		Pilot_Relay_Consistency = Hbl__GetLoadConsistency(pilot_relay_load_index);

		pilot_relay_state = ((uint8 *)Hbl__GetProcessedLoad(pilot_relay_load_index))[0];

		if ((pilot_relay_state != pilot_relay_prev_state) || (Pilot_Relay_Consistency != Pilot_Relay_Prev_Consistency))
		{
			Pilot_Relay_Stabilization_Counter = PILOT_RELAY_FAULT_STABILIZATION_TIME;
			pilot_relay_prev_state = pilot_relay_state;
			Pilot_Relay_Prev_Consistency = Pilot_Relay_Consistency;
		}
		else
		{
			if (Pilot_Relay_Stabilization_Counter > (uint8)0)
			{
				Pilot_Relay_Stabilization_Counter --;
			}
		}

		PilotRelayStuckClosedMonitor__Execution();
		PilotRelayStuckOpenMonitor__Execution();
	}

}


//----------------------------------------------------------------------------
/**
 *  @brief      PilotRelayFaultMonitor__CycleStarted
 *  @details    Function to be called when a cycle is started to remove the Pilot Relay Fault.
 *
 */
void PilotRelayFaultMonitor__CycleStarted(void)
{
	PilotRelayStuckClosedMonitor__CycleStarted();
	PilotRelayStuckOpenMonitor__CycleStarted();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
uint8 PilotRelayFaultMonitor__GetConsistency(void)
{
	return Pilot_Relay_Consistency;
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
uint8 PilotRelayFaultMonitor__GetStabilizationCounter(void)
{
	return Pilot_Relay_Stabilization_Counter;
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void PilotRelayFaultMonitor__RestartStabilizationCounter(void)
{
	Pilot_Relay_Stabilization_Counter = PILOT_RELAY_FAULT_STABILIZATION_TIME;
}



//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


