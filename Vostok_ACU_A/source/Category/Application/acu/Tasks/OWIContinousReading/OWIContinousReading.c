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
#include "C_Extensions.h"
#include "DishAuxVar.h"
#include "DoorMonitor.h"
#include "Hbl.h"
#include "OWIContinousReading.h" 
#include "Variable.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Sanitization and its variables
 *
 */
ACTIVE_TYPE OWIContinousReading__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement)
{
	ACTIVE_TYPE revalue = ACTIVE;

	return(revalue);
}

/**
 * @brief The function called at the initialise of the task after NVdata restore
 *        It Initialises the Time Elapse Module and its variables
 *
 * @param compartment that run the task (unused)
 *
 * @return  None
 */

void OWIContinousReading__NVDataRestore(uint8 compartment)
{

}

/**
 * The function called during the execution of the task
 * @param compartment that run the task (inside)
 * @return the status of the task (ACTIVE or INACTIVE)
 */
ACTIVE_TYPE OWIContinousReading__Task(uint8 compartment)
{
	sint32 duty_cycle;
	DOOR_STATUS_TYPE door_status;

	door_status = DoorMonitor__GetDoorStatus();

	if (door_status == DOOR_STATUS_CLOSE)
	{
		(void)Variable__GetValue(VARIABLE_OWI_CALIBRATION_DRIVE_DUTY_CYCLE,0,(sint32*)((void*)(&duty_cycle)));

		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_OWI, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &duty_cycle);

		// OWI_Analog
		OWI_Analog = *(uint16 PACKED *)Hbl__GetGIDataByIndex(Hbl__GetGIIndexByGIID(HBL_GI_TURBIDITY_SENSOR_OWI));
	}

    return(ACTIVE);
}

/**
 * The function used when the task terminate. It reinitialise the Counter and corresponding variables of the task
 * @param compartment the compartment that uses the task (unused)
 *
 * @return : None
 */
void OWIContinousReading__Terminate(uint8 compartment)
{

}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


