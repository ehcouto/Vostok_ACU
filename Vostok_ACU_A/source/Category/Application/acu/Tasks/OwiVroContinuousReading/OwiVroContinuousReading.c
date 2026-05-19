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
#include "OwiVroContinuousReading.h"

// -- Other Modules --
#include "OWIVoltageRegulatedOutput.h"
#include "Variable.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Minimum amount of water (cl) in the tub to allow the Owi Vro Continuous Reading task to activate the OWI sensor.
#define OWI_VRO_CNT_READING_MINIMUM_WATER_AMOUNT	100

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


/**
 * The function called during the execution of the task
 * @param compartment that run the task (inside)
 * @return the status of the task (ACTIVE or INACTIVE)
 */
ACTIVE_TYPE OwiVroContinuousReading__Task(uint8 compartment)
{
	sint32 liters_in_tub;

	if (Variable__GetValue(VARIABLE_LITERS_IN_TUB, 0, &liters_in_tub) == FALSE)
	{
		liters_in_tub = 0;
	}

	if (liters_in_tub >= OWI_VRO_CNT_READING_MINIMUM_WATER_AMOUNT)
	{
		OWIVoltageRegulatedOutput__RequestResetIfDisabled();
	}
	else
	{
		// Do nothing, the task shall not request the monitor to run if there insn't enough water in the tub
	}

    return (ACTIVE);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
