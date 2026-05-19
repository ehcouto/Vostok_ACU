/*
 * FlowMeterFaultMonitor.c
 *
 *  Created on: 05 giu 2018
 *      Author: CERINS
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "FlowMeterFaultMonitor.h"

// -- Other Modules --
#include "C_Extensions.h"
#include "Fault.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Micro.h"
#include "Selector.h"
#include "SystemConfig.h"
#include "SystemTimers.h"

typedef enum FLOWMETER_DIGITAL_STATUS_ENUM
{
	FLOWMETER_DIGITAL_STATUS_LOW = 0,
	FLOWMETER_DIGITAL_STATUS_HIGH
}FLOWMETER_DIGITAL_STATUS_TYPE;

typedef PACKED struct FLOW_METER_FAULT_PARAMS_STRUCT
{
	uint8 Unused;
	uint8 Factory_Fault_Debounce_Time;
} FLOW_METER_FAULT_PARAMS_TYPE;


static ON_OFF_TYPE Fill_Valve_State;
static uint8 Water_Counter_Input_Sequence_Id_Old;
static SYSTEMTIMER_NV_TIMER_TYPE FlowMeterFault_Timer_Handle;
static FLOW_METER_FAULT_PARAMS_TYPE *Flow_Meter_Fault_Params;
//----------------------------------------------------------------------------
/**
 *  @brief      FlowMeterFaultMonitor__Initialize
 *  @details    Function used to initialize the Flow Meter Fault monitor.
 *
 */
void FlowMeterFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Water_Counter_Input_Sequence_Id_Old = 0;

		Flow_Meter_Fault_Params = (FLOW_METER_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_FLOW_METER_FINDLAY_FACTORY);
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      FlowMeterFaultMonitor__Execution
 *  @details    Function used to perform the Flow Meter Fault monitor.
 *
 */
void FlowMeterFaultMonitor__Execution(void)
{
	if (Flow_Meter_Fault_Params != NULL)
	{
		INTERPRETER_STATUS_TYPE status = Interpreter__GetStatus();

		CYCLE_NAME_TYPE cycle_name = Selector__GetCycleNameId(0,0,status.Main_Cycle[0]);

		if ((CYCLE_NAME_EOL_0 == cycle_name) ||
			(CYCLE_NAME_EOL_1 == cycle_name) ||
			(CYCLE_NAME_EOL_2 == cycle_name))
		{
		    uint8 *load_state_ptr;

		    load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_FILL_VALVE));

		    if (load_state_ptr != NULL)
		    {
		    	Fill_Valve_State = (load_state_ptr[0] == 1U) ? ON:OFF;
		    }
		    else
		    {
		    	Fill_Valve_State = OFF;
		    }

			if ((Fill_Valve_State == ON) && (SystemTimers__NVCheckTimer(&FlowMeterFault_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING))
			{
				(void)SystemTimers__NVSetTimerMS(&FlowMeterFault_Timer_Handle, ((uint32)Flow_Meter_Fault_Params->Factory_Fault_Debounce_Time*1000u), SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				Water_Counter_Input_Sequence_Id_Old = Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER));
			}

			if (SystemTimers__NVCheckTimer(&FlowMeterFault_Timer_Handle) == SYSTEMTIMERS_STATE_RUNNING)
			{
				uint8 water_counter_input_sequence_id = Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER));

				if (water_counter_input_sequence_id == Water_Counter_Input_Sequence_Id_Old)
				{
					uint8 flow_meter_input_sequence_id = Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_FLOW_METER_DIGITAL));

					uint8 flow_meter_input_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_FLOW_METER_DIGITAL);

					FLOWMETER_DIGITAL_STATUS_TYPE flow_meter_digital_Status = (FLOWMETER_DIGITAL_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex(flow_meter_input_gi_index);

					if ((flow_meter_digital_Status == FLOWMETER_DIGITAL_STATUS_LOW) &&
						(flow_meter_input_sequence_id > 0U))
					{
						if (Fault__IsObserverActive(FAULT_ID_FLOW_METER_FINDLAY_FACTORY) == FALSE)
						{
							Fault__ConditionDetected(FAULT_ID_FLOW_METER_FINDLAY_FACTORY);
						}
					}
				}
				else
				{
					Water_Counter_Input_Sequence_Id_Old = water_counter_input_sequence_id;

					if (Fault__IsObserverActive(FAULT_ID_FLOW_METER_FINDLAY_FACTORY) == FALSE)
					{
						Fault__ConditionDetected(FAULT_ID_FLOW_METER_FINDLAY_FACTORY);
					}
				}
			}
		}
	}
}
