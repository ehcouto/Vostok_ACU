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
#include "C_Extensions.h"
#include "DiverterLeakFaultMonitor.h"

// -- Other Modules --
#include "DishAuxVar.h"
#include "Fault.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Selector.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef enum DIVERTER_FEEDBACK_ENUM
{
	DIVERTER_FEEDBACK_LOW_RANGE			= 0,
	DIVERTER_FEEDBACK_LEAK_RANGE		= 1,
	DIVERTER_FEEDBACK_HIGH_RANGE  		= 2
}DIVERTER_FEEDBACK_TYPE;

#define DIVERTER_LEAK_RANGE_WAIT 		(uint8)2 /* hundredths of a second */

BOOL_TYPE DLFM_Diverter_Leak_Happened;  /* Trigger for diverter leak range detected */
uint8 DLFM_Diverter_Leak_Range_Counter; /* Counter to solve the side effect that during transition High-Low the monitor catch the leak range*/
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      DiverterLeakFaultMonitor__Initialize
 *  @details    Function used to initialize the DiverterLeakFaultMonitor.
 *
 */
void DiverterLeakFaultMonitor__Initialize(void)
{
	/*if ((Micro__GetResetMode() == MICRO_RESET_MODE_COLD) &&
		(NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY))
	{

	}*/
	DLFM_Diverter_Leak_Happened = FALSE;

	DLFM_Diverter_Leak_Range_Counter = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief      DiverterLeakFaultMonitor__Execution
 *  @details    Function used to perform the DiverterLeakFaultMonitor.
 *
 */
void DiverterLeakFaultMonitor__Execution(void)
{
	if (DishAuxVar__GetDiverterPosition().Is_Stable == FALSE)
	{
		DIVERTER_FEEDBACK_TYPE temp = (DIVERTER_FEEDBACK_TYPE)Hbl__GetGIDataConvertedByIndex(Hbl__GetGIIndexByGIID(HBL_GI_DIVERTER_FEEDBACK));

		if (temp == DIVERTER_FEEDBACK_LEAK_RANGE)
		{
			DLFM_Diverter_Leak_Range_Counter++;
			if (DLFM_Diverter_Leak_Range_Counter >= DIVERTER_LEAK_RANGE_WAIT)
			{
				if (DLFM_Diverter_Leak_Happened == FALSE)
				{
					DLFM_Diverter_Leak_Happened = TRUE;
				}
			}
		}
		else if (temp == DIVERTER_FEEDBACK_LOW_RANGE)
		{
			DLFM_Diverter_Leak_Range_Counter = 0;

			if ((uint8)DLFM_Diverter_Leak_Happened == (uint8)TRUE)
			{
				CYCLE_GROUP_TYPE cycle_group = Selector__GetCycleGroup(0, COMPARTMENT_DEFAULT, Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

				DLFM_Diverter_Leak_Happened = FALSE;

				if (cycle_group == CYCLE_GROUP_EOL_TEST)
				{
					Fault__ConditionDetected(FAULT_ID_DIVERTER_LEAK_FACTORY);
				}
				else
				{
					Fault__ConditionDetected(FAULT_ID_DIVERTER_LEAK);
				}
			}
		}
		else if (temp == DIVERTER_FEEDBACK_HIGH_RANGE)
		{
			DLFM_Diverter_Leak_Range_Counter = 0;

			DiverterLeakFaultMonitor__ResetFaultObserver();
		}
		else
		{
			//do nothing
		}
	}
	else
	{
		DLFM_Diverter_Leak_Happened = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      DiverterLeakFaultMonitor__ResetFaultObserver
 *  @details    Function used to reset the FaultObserver for Diverter Leak fault.
 *
 */
void DiverterLeakFaultMonitor__ResetFaultObserver(void)
{
	DLFM_Diverter_Leak_Happened = FALSE;

	if (FALSE != Fault__IsObserverActive(FAULT_ID_DIVERTER_LEAK))
	{
		Fault__ConditionRemoved(FAULT_ID_DIVERTER_LEAK);
	}

	if (FALSE != Fault__IsObserverActive(FAULT_ID_DIVERTER_LEAK_FACTORY))
	{
		Fault__ConditionRemoved(FAULT_ID_DIVERTER_LEAK_FACTORY);
	}
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

