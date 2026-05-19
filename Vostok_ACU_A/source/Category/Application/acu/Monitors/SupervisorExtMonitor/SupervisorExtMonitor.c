/**
 *  @file		SupervisorExtMonitor.c
 *
 *  @brief      Loads monitor module implementation
 *
 *  @details    This module checks the Loads that must be pilot from supervisor state.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *
 *  @copyright  Copyright 2016. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "C_Extensions.h"
#include "SupervisorExtMonitor.h"

// -- Other Modules --
#include "Hbl.h"
#include "Interpreter.h"
#include "Selector.h"
#include "SettingFile.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8* Ptr_Sup_Ext_Mon_Data;
static uint16 Num_Loads_Supervisor_Monitored;

#define NO_LOADS_TO_MANAGES		(uint16)0
#define OFFSET_HEAD_DATA		2
#define NO_SETTING_LOAD			0xFF

typedef enum
{
	SUP_EXT_MON_STATE_NULL      		= 0,
	SUP_EXT_MON_STATE_PROGRAMMING   	= 1,
	SUP_EXT_MON_STATE_DELAY	   			= 2,
	SUP_EXT_MON_STATE_RUN          		= 3,
	SUP_EXT_MON_STATE_PAUSE				= 4,
	SUP_EXT_MON_STATE_END				= 5,
	SUP_EXT_MON_STATE_FAULT				= 6,
	SUP_EXT_MON_STATE_OFF 				= 7,

	SUP_EXT_MON_NUM_STATES 				= 8
}SUPERVISOR_EXT_MON_TYPE;

#define OFF_CYCLE	0
#define OFF_PHASE	0

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
SUPERVISOR_EXT_MON_TYPE SupervisorExtMonitorGetState(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


void SupervisorExtMonitor__Initialize(void)
{
	SETTINGFILE_LOADER_TYPE loader;

	if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_LOADS_STATES_MNG, &loader) == PASS)
	{
		Num_Loads_Supervisor_Monitored = (loader.Length - (uint16)OFFSET_HEAD_DATA) / (uint8)(*loader.Data);
		Ptr_Sup_Ext_Mon_Data = (loader.Data + OFFSET_HEAD_DATA);
	}
	else
	{
		// error
		Num_Loads_Supervisor_Monitored = NO_LOADS_TO_MANAGES;
		Ptr_Sup_Ext_Mon_Data = NULL;
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief		Monitor Loads
 *
 */
void SupervisorExtMonitor__Execution(void)
{
    if ((Ptr_Sup_Ext_Mon_Data != (uint8*)NULL) && (Num_Loads_Supervisor_Monitored != NO_LOADS_TO_MANAGES))
    {
    	SUPERVISOR_EXT_MON_TYPE state_supervisor_monitor;
        uint8 numberbytes;
        uint16 index;

        state_supervisor_monitor = SupervisorExtMonitorGetState();
        numberbytes = (uint8)*(Ptr_Sup_Ext_Mon_Data - OFFSET_HEAD_DATA);

		for (index = 0; index < Num_Loads_Supervisor_Monitored; index++)
		{
			if ((state_supervisor_monitor > SUP_EXT_MON_STATE_NULL) && (state_supervisor_monitor < SUP_EXT_MON_NUM_STATES))
			{
			    uint8 loadcmd;
			    uint8 *pntlocal;

			    // Ptr_Sup_Ext_Mon_Data 	= pointer at first data = LoadID.
				// state_supervisor_monitor = index of states value.
				pntlocal = Ptr_Sup_Ext_Mon_Data + (index * numberbytes);
				loadcmd = *(pntlocal + (uint8)state_supervisor_monitor);

				// Control load
				if (loadcmd < (uint8)NO_SETTING_LOAD)
				{   // else we don't set nothing.
					// HBL_PRIORITY_APPLICATION_LOW in this way the LoadsControl by supervisor can take control of load.
					(void)(Hbl__SetLoadRequestByLoadID((HBL_LOAD_ENUM_TYPE)(*pntlocal), HBL_PRIORITY_SUPERVISOR_EXT_MONITOR, &loadcmd));
				}
			}
		}
    }


	(void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_DIVERTER, HBL_PRIORITY_APPLICATION_LOW, Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER))));
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief		Set Loads Request.
 *
 */
SUPERVISOR_EXT_MON_TYPE SupervisorExtMonitorGetState(void)
{
	SUPERVISOR_EXT_MON_TYPE ret_value;
	CYCLE_PHASE_STATEMENT_TYPE cps;
	CYCLE_STATE_TYPE cycle_state;

	ret_value = SUP_EXT_MON_STATE_NULL;
	cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);
	cycle_state = Selector__GetCycleState(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT), COMPARTMENT_DEFAULT, cps.Cycle);

	switch (cycle_state)
	{
		case CYCLE_STATE_PROGRAMMING:
			if ((cps.Cycle == (uint8)OFF_CYCLE) && (cps.Phase == (uint8)OFF_PHASE))
			{
				ret_value = SUP_EXT_MON_STATE_OFF;
			}
			else
			{
				ret_value = SUP_EXT_MON_STATE_PROGRAMMING;
			}
			break;

		case CYCLE_STATE_RUN:
			ret_value = SUP_EXT_MON_STATE_RUN;
			break;

		case CYCLE_STATE_DELAY:
			ret_value = SUP_EXT_MON_STATE_DELAY;
			break;

		case CYCLE_STATE_END:
			ret_value = SUP_EXT_MON_STATE_END;
			break;

		case CYCLE_STATE_PAUSE:
			ret_value = SUP_EXT_MON_STATE_PAUSE;
			break;

		case CYCLE_STATE_FAULT:
			ret_value = SUP_EXT_MON_STATE_FAULT;
			break;

		default:
			//do nothing
			break;
	}

	return ret_value;
}

