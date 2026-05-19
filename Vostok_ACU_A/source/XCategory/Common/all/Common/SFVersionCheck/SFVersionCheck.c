/**
 *  @file       
 *
 *  @brief      Source file for the SFVersionCheck module.
 *
 *  @details    Verifies the Settings file version is equal to or greater than the version required by this module.
 *              If the Version is less than the required, the machine is bricked.
 *
 *  @copyright  Copyright 2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#include "SFVersionCheck.h" 
#include "SFVersionCheck_prv.h"

#include "Log.h"
#include "Mode.h"
#include "SettingFile.h"

#include <string.h>
#include <Utilities.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! list of module Log messages
typedef enum
{
    INVALID_SETTING_FILE = 1,
    SETTING_FILE_VERSION_OLD_FW_MAJOR_MINOR,    // MSB Firmware Settings File required Major; LSB Firmware Settings File required Minor
    SETTING_FILE_VERSION_OLD_FW_BUILD_REVISION  // MSB Firmware Settings File required Build; LSB Firmware Settings File required Revision
} MODULE_SFVERSIONCHECK_LOG_MESSAGE_ID_TYPE;
#define MODULE_NAME MODULE_SFVERSIONCHECK

// structure defining the GESE Editor version.
typedef enum
{
    MAJOR = 0,
    MINOR,
    BUILD,
    REVISION,
    NUM_OF_FIELDS
} GESE_EDITOR_VERSION_TYPE;

// Definition of the minimum GESE Editor version required to be compatible with the ACU firmware.
static const uint8 GESE_EDITOR_VERSION_REQUIRED[] =
{
    PROJECT_SPECIFIC_GESE_EDITOR_VERSION
};
CT_ASSERT(ELEMENTS_IN_ARRAY(GESE_EDITOR_VERSION_REQUIRED) == NUM_OF_FIELDS);

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Verifies the Settings file version is greater than or equal to the Firmware's minimum requried version.
 *
 */
void SFVersionCheck__Verify(void)
{
    SETTINGFILE_LOADER_TYPE loader;

    //GESE Project_Release_Version
    if (( SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_GESE_VERSION, &loader) == FAIL ) ||
        ( loader.Length != NUM_OF_FIELDS ))
    {
        LOG_ADD_EXCEPTION(INVALID_SETTING_FILE, 0);
        Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
    }
    else
    {
        uint32 firmware_version = Utilities__ConvertArrayTo32bits((uint8 *)(void *)GESE_EDITOR_VERSION_REQUIRED);
        uint32 gese_version = Utilities__ConvertArrayTo32bits(loader.Data);

        if ( gese_version < firmware_version )
        {
            // the firmware requires a newer version of the settings file
            LOG_ADD_EXCEPTION(SETTING_FILE_VERSION_OLD_FW_MAJOR_MINOR, (GESE_EDITOR_VERSION_REQUIRED[MAJOR] << 8) + GESE_EDITOR_VERSION_REQUIRED[MINOR]);
            LOG_ADD_EXCEPTION(SETTING_FILE_VERSION_OLD_FW_BUILD_REVISION, (GESE_EDITOR_VERSION_REQUIRED[BUILD] << 8) + GESE_EDITOR_VERSION_REQUIRED[REVISION]);
            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
        }
        // else GESE setting file version is greater than or equal to the firmware required version, do nothing
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


