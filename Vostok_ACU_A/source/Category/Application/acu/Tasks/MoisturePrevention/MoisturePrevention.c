/**
 *  @file       MoisturePrevention.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MoisturePrevention.h" 
#include "MoisturePreventionMonitor.h"
#include "SettingFile.h"
#include "String.h"
#include "Log.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_MOISTURE_PREVENTION

typedef enum
{
    ERROR_LOADING_PARAMETERS       = 1,
// Log messages enumerated here...
} MODULE_MOISTURE_PREVENTION_LOG_MESSAGE_ID_TYPE;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

#define DISPL_MOISTURE_PREVENTION 2

static MoisturePreventionSettingFileStructureType MPSettingFileStructure;


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MoisturePrevention and its variables
 *
 */
ACTIVE_TYPE MoisturePrevention__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement)
{
    SETTINGFILE_LOADER_TYPE loader;
    MoisturePreventionSettingFileStructureType* parameter;
    ACTIVE_TYPE retval;


    if (FAIL == SettingFile__BasicLoader(SF_PTR_ACU_MOIST_PREV_PARAMETERS, displacement, &loader) )
    {
        LOG_ADD_EXCEPTION(0, DISPL_MOISTURE_PREVENTION);
        retval = INACTIVE;
    }
    else
    {
        parameter = (MoisturePreventionSettingFileStructureType*)((void*)(loader.Data));  // Read the data from the setting file
        MPSettingFileStructure.NumberOfSlots = parameter->NumberOfSlots;
        MPSettingFileStructure.SlotDurationSplitted = parameter->SlotDurationSplitted;
        MPSettingFileStructure.SlotDuration = parameter->SlotDuration;
        MPSettingFileStructure.FanONSpeed = parameter->FanONSpeed;
        MPSettingFileStructure.FanOFFSpeed = parameter->FanOFFSpeed;
        MPSettingFileStructure.VentStatusBitMap = ENDIANU16_STON(parameter->VentStatusBitMap);
        MPSettingFileStructure.FanStatusBitMap = ENDIANU16_STON(parameter->FanStatusBitMap);

        memcpy(&MPSettingFileStructure.SlotDurationBank[0], &parameter->SlotDurationBank[0], sizeof(MPSettingFileStructure.SlotDurationBank));

        MoisturePreventionMonitor__Activate(&MPSettingFileStructure);
        retval = ACTIVE;
    }

    return retval;
}

/**
 * @brief The function called at the initialise of the task after NVdata restore
 *        It Initialises the Moisture Prevention Module and its variables
 *
 * @param compartment that run the task (unused)
 *
 * @return  None
 */

void MoisturePrevention__NVDataRestore(uint8 compartment)
{

}

/**
 * The function called during the execution of the task
 * @param compartment that run the task (inside)
 * @return the status of the task (ACTIVE or INACTIVE)
 */
ACTIVE_TYPE MoisturePrevention__Task(uint8 compartment)
{
	ACTIVE_TYPE retval;

    if (MoisturePreventionMonitor__GetStatus() != MP_DEACTIVATED)
    {
        retval = ACTIVE;
    }
    else
    {
        retval = INACTIVE;
    }

    return retval;
}


/**
 * The function used when the task terminate. It reinitialise the Counter and corresponding variables of the task
 * @param compartment the compartment that uses the task (unused)
 *
 * @return : None
 */
void MoisturePrevention__Terminate(uint8 compartment)
{
    MoisturePreventionMonitor__Deactivate();
}

void MoisturePrevention__Pause(uint8 compartment)
{
    MoisturePreventionMonitor__Pause(MP_PAUSE_SUPERVISOR_EVENT);
}

void MoisturePrevention__Resume(uint8 compartment)
{
    MoisturePreventionMonitor__Resume(MP_PAUSE_SUPERVISOR_EVENT);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


