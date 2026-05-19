/**
 *  @file
 *
 *  @brief      Public interface for the SettingFile module.
 *
 *  @details    Refer to the SettingFile.c source file for more detailed information.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2015-2023 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SETTINGFILE_H_
#define SETTINGFILE_H_

#include "SRSystemConfig.h"
#include "SettingFile_defs.h"
#if (SETTINGFILE_SR_CAT_DATA_FEATURE == ENABLED)    // This should be ENABLED/DISABLED in SRSystemConfig_prm.h
#include "SettingFile_SRCat.h"
#endif
#include "SettingFile_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Index for primary setting file.
#define PRIMARY_SETTING_FILE        (0)

#ifndef SF_MAX_SECTIONS
//! The maximum number of setting file sections that this module supports
#define SF_MAX_SECTIONS 10
#endif

//! Definition of the data structure for a displacement in the setting file
PACKED typedef struct
{
    uint8 * Data;                                   // Pointer to the displacement data.
    uint16 Length;                                  // Length of displacement data in bytes.
} SETTINGFILE_LOADER_TYPE;

//! List of possible Setting File section IDs
//! Lower nibble is the type of board (ACU/HMI/MCU).
//! Upper nibble is an index to distinguish between multiple boards of the same type.
typedef enum SF_ID_ENUM
{
    SF_ID_ACU       = 0x00,
    SF_ID_HMI       = 0x01,
    SF_ID_MCU       = 0x02,
    SF_ID_ACU_EXP_1 = 0x10,
    SF_ID_ACU_EXP_2 = 0x20,
    SF_ID_ACU_EXP_3 = 0x30,
    SF_ID_ACU_EXP_4 = 0x40,
    SF_ID_ACU_EXP_5 = 0x50,
    SF_ID_HMI_EXP_1 = 0x11,
    SF_ID_HMI_EXP_2 = 0x21,
    SF_ID_HMI_EXP_3 = 0x31,
    SF_ID_HMI_EXP_4 = 0x41,
    SF_ID_HMI_EXP_5 = 0x51,
    SF_ID_END = 0xFF
} SF_ID_TYPE;

//! Main platform setting file pointers.
typedef enum SETTINGFILE_OFFSET_ENUM
{
    SF_PTR_ACU_END_OF_SECTION                   = 0x0000,
    SF_PTR_ACU_CRC                              = 0x0001,
    SF_PTR_ACU_SERIAL_NUMBER                    = 0x0002,
    SF_PTR_ACU_MAIN_EXTERNAL_PROCESS            = 0x0003,
    SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA       = 0x0004,
    SF_PTR_ACU_CROSS_PRODUCT_CONFIG             = 0x0005,
    SF_PTR_ACU_HI_STATEMENTS                    = 0x0006,
    SF_PTR_ACU_LO_STATEMENTS                    = 0x0007,
    SF_PTR_ACU_SUB_CYCLES                       = 0x0008,
    SF_PTR_ACU_TIME_ESTIMATION                  = 0x0009,
    SF_PTR_ACU_CONDITION_TABLE                  = 0x000A,
    SF_PTR_ACU_MODIFIERS                        = 0x000B,
    SF_PTR_ACU_MONITORS_DATA                    = 0x000C,       // Platform specific monitors
    SF_PTR_ACU_FAULT_CONFIGURATION              = 0x000D,

    SF_PTR_ACU_PROCESS_SET_STATE                = 0x0010,
    SF_PTR_ACU_PROCESS_SET_VARIABLE             = 0x0011,
    SF_PTR_ACU_PROCESS_MAINTAIN                 = 0x0012,
    SF_PTR_ACU_PROCESS_LOADS_CONTROL            = 0x0013,
    SF_PTR_ACU_PROCESS_JUMP_IF                  = 0x0014,
    SF_PTR_ACU_PROCESS_ACTIVATE_TASK            = 0x0015,

    //  0x0016 to 0x001F is platform specific

    SF_PTR_ACU_IO_CONFIG                        = 0x0020,
    SF_PTR_ACU_IO_DEVICE                        = 0x0021,
    SF_PTR_ACU_LOADS_PROFILE                    = 0x0022,       // Setting file pointer Loads's Pilot Profiles
    SF_PTR_ACU_REG_PARAM_TABLE                  = 0x0023,       // Main table Regulation values
    SF_PTR_ACU_FAULT_DATA                       = 0x0024,
    SF_PTR_ACU_CLASSB_PARAMETERS_DATA           = 0x0025,
    SF_PTR_ACU_STATISTICS                       = 0x0026,
    SF_PTR_ACU_ALGORITHM_PARAMETERS             = 0x0027,

    //  0x0028 to 0x002F is platform specific

    SF_PTR_ACU_SELECTOR_TARGET_0                = 0x0030,
    SF_PTR_ACU_SELECTOR_TARGET_1                = 0x0031,
    SF_PTR_ACU_SELECTOR_TARGET_2                = 0x0032,
    SF_PTR_ACU_SELECTOR_TARGET_3                = 0x0033,
    SF_PTR_ACU_SELECTOR_TARGET_4                = 0x0034,
    SF_PTR_ACU_SELECTOR_TARGET_5                = 0x0035,
    SF_PTR_ACU_SELECTOR_TARGET_6                = 0x0036,
    SF_PTR_ACU_SELECTOR_TARGET_7                = 0x0037,
    SF_PTR_ACU_SELECTOR_TARGET_8                = 0x0038,
    SF_PTR_ACU_SELECTOR_TARGET_9                = 0x0039,
    SF_PTR_ACU_SELECTOR_TARGET_10               = 0x003A,

    ////////////////////////////////////////////////////////////

    SF_PTR_UI_END_OF_SECTION                    = 0x0100,
    SF_PTR_UI_CRC                               = 0x0101,
    SF_PTR_UI_PRODUCT_CONFIG                    = 0x0102,
    SF_PTR_UI_HI_STATEMENTS                     = 0x0103,
    SF_PTR_UI_LO_STATEMENTS                     = 0x0104,
    SF_PTR_UI_PROCESS_SET_AUDIO                 = 0x0105,
    SF_PTR_UI_PROCESS_SET_VISUAL_OBJECT         = 0x0106,
    SF_PTR_UI_PROCESS_SET_FUNCTION_IF           = 0x0107,
    SF_PTR_UI_PROCESS_SET_FUNCTION              = 0x0108,
    SF_PTR_UI_PROCESS_UI_JUMP_IF                = 0x0109,
    SF_PTR_UI_PROCESS_SET_ANIMATION             = 0x010A,
    SF_PTR_UI_PROCESS_SET_HMI_VARIABLE          = 0x010B,
    SF_PTR_UI_PROCESS_SET_HMI_TIMER_VARIABLE    = 0x010C,

    // Pointers 0x010D - 0x00F reserved for future processes

    SF_PTR_UI_IO_CONFIG                         = 0x0110,       // Setting file pointer to UI I/O Map
    SF_PTR_UI_IO_DEVICE                         = 0x0111,       // Setting file pointer to UI I/O configuration parameters
    SF_PTR_UI_FUNCTION_HEADING                  = 0x0112,
    SF_PTR_UI_DISPLAY_REGULATION_TABLE          = 0x0113,
    SF_PTR_UI_CUSTOM_PROGRAM_HEADING            = 0x0114,       // Custom Program Heading
    SF_PTR_UI_PROGRAM_HEADING_TARGET_0          = 0x0115,       // Compartment 0 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_1          = 0x0116,       // Compartment 1 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_2          = 0x0117,       // Compartment 2 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_3          = 0x0118,       // Compartment 3 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_4          = 0x0119,       // Compartment 4 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_5          = 0x011A,       // Compartment 5 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_6          = 0x011B,       // Compartment 6 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_7          = 0x011C,       // Compartment 7 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_8          = 0x011D,       // Compartment 8 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_9          = 0x011E,       // Compartment 9 cycle headers
    SF_PTR_UI_PROGRAM_HEADING_TARGET_10         = 0x011F,       // Compartment 10 cycle headers

    SF_PTR_UI_GLOBAL_SELECTOR_DATA              = 0x0120,       // UI Global Macro
    SF_PTR_UI_SEQUENCER_DATA                    = 0x0121,       // Table of Sequences
    SF_PTR_UI_SEQUENCER_STEPS                   = 0x0122,
    SF_PTR_UI_SELECTOR_CONDITIONS               = 0x0123,
    SF_PTR_UI_REGULATION_PARAMS                 = 0x0124,
    SF_PTR_UI_LED_GROUP_CONFIGURATION           = 0x0125,
    SF_PTR_UI_AUDIO_CONFIGURATION               = 0x0126,
    SF_PTR_UI_LED_ANIMATION                     = 0x0127,
    SF_PTR_UI_CONDITION_TABLE                   = 0x0128,
    SF_PTR_UI_TIME_TO_END                       = 0x0129,
    SF_PTR_UI_VIEW_ENGINE_CONTROL_STATE_LAYER   = 0x012A,
    SF_PTR_UI_VIEW_ENGINE_VIEWS                 = 0x012B,

    SF_PTR_UI_SR_EVENT_DATA                     = 0x012E,       // Additional data for SR Events
    SF_PTR_UI_SR_EVENTS                         = 0x012F,       // SR Event data for the HMI

    SF_PTR_UI_SELECTOR_TARGET_0                 = 0x0130,
    SF_PTR_UI_SELECTOR_TARGET_1                 = 0x0131,
    SF_PTR_UI_SELECTOR_TARGET_2                 = 0x0132,
    SF_PTR_UI_SELECTOR_TARGET_3                 = 0x0133,
    SF_PTR_UI_SELECTOR_TARGET_4                 = 0x0134,
    SF_PTR_UI_SELECTOR_TARGET_5                 = 0x0135,
    SF_PTR_UI_SELECTOR_TARGET_6                 = 0x0136,
    SF_PTR_UI_SELECTOR_TARGET_7                 = 0x0137,
    SF_PTR_UI_SELECTOR_TARGET_8                 = 0x0138,
    SF_PTR_UI_SELECTOR_TARGET_9                 = 0x0139,
    SF_PTR_UI_SELECTOR_TARGET_10                = 0x013A,

    SF_PTR_UI_TOUCH_PARAMETERS                  = 0x013C,

    // Pointers 0x0140 - 0x014F and Pointers 0x0160 - 0x017F are reserved for Platform specific HMI pointers
    // They shall be enumerated through the SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR definition

    ////////////////////////////////////////////////////////////

    SF_PTR_MCU_END_OF_SECTION                   = 0x0200,
    SF_PTR_MCU_CRC                              = 0x0201,
    SF_PTR_MCU_MACHINE_CONFIG                   = 0x0202,
    SF_PTR_MCU_PARAMETER_ESTIMATION             = 0x0203,
    // Pointers 0x0204-0x020F reserved for Platform specific MCU pointers
    // Non safety related Platform specific pointers shall be extended through the SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR definition
    // Safety related Platform specific pointers shall be extended through the SETTINGFILE_APPLICATION_SPECIFIC_SR_SF_PTR definition
    SF_PTR_MCU_CLASS_A_MCI                      = 0x0210,
    SF_PTR_MCU_CLASS_B_MCI                      = 0x0211,

    ////////////////////////////////////////////////////////////

    SF_PTR_INVALID_VALUE                        = 0xFFFF,

    // Cross Category list of pointers with no safety relevant data; defined in SettingFile_defs.h
    SETTINGFILE_CROSS_CATEGORY_SF_PTR

    // Category specific list of pointers with no safety relevant data; defined in SettingFile_prm.h (as needed)
    #ifdef SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR
    SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR
    #endif

    // Category specific list of pointers with safety relevant data; defined in SettingFile_SRCat.h (as needed)
    #ifdef SETTINGFILE_APPLICATION_SPECIFIC_SR_SF_PTR
    SETTINGFILE_APPLICATION_SPECIFIC_SR_SF_PTR
    #endif
} SETTINGFILE_OFFSET_TYPE;

//! Displacements for cross-product pointers.
// New cross category safety relevant displacements must be added here, else cross category displacements must be added in SettingFile_defs.h.
// New category specific safety relevant displacements must be in SettingFile_SRCat.h, else they can be added in SettingFile_prm.h
typedef enum SETTINGFILE_CROSS_PRODUCT_DISPLACEMENT_ENUM
{
    // Displacement numbers for SF_PTR_ACU_CROSS_PRODUCT_CONFIG
    SF_DISPL_PRODUCT_CFG_BMP                    = 0x02,
    SF_DISPL_BASE_UNITS                         = 0x03,

     // Displacement numbers for SF_PTR_ACU_IO_CONFIG and SF_PTR_UI_IO_CONFIG
    SF_DISPL_LOW_LEVEL_INPUT_MAP                = 0x00,
    SF_DISPL_GENERIC_INPUT_MAP                  = 0x01,
    SF_DISPL_LED_CONFIGURATION                  = 0x02,
    SF_DISPL_OUTPUTS_MAP                        = 0x03,
    SF_DISPL_MODULE_PIN_MAP                     = 0x04,
    SF_DISPL_STATUS_VARIABLES                   = 0x05,
    SF_DISPL_CROSS_LOADS                        = 0x06,

    // Displacement numbers for SF_PTR_UI_PRODUCT_CONFIG
    SF_DISPL_EXPANSION_BOARD_CONFIG             = 0x08,
    SF_DISPL_CYCLES_NUMBER                      = 0x09,
    SF_DISPL_FUNCTIONS_NUMBER                   = 0x0A,
    SF_DISPL_BRAND_ID                           = 0x0B,
    SF_DISPL_TEMPERATURE_SCALE                  = 0x0C,
    SF_DISPL_SAFETY_RELEVANT                    = 0x0D,

    // Displacement numbers for SF_PTR_ACU_SERIAL_NUMBER
   SF_DISPL_PART_NUMBER                        = 0x00,
   SF_DISPL_PROJECT_STATUS                     = 0x01,
   SF_DISPL_DESCRIPTION                        = 0x02,
   SF_DISPL_GESE_VERSION                       = 0x03,
   SF_DISPL_PLATFORM_PRODUCT_TYPE              = 0x04,
   SF_DISPL_PROJECT_TIMESTAMP                  = 0x05,
   SF_DISPL_CLASSB_CRC                         = 0x06,

   // Displacement numbers for SF_PTR_MCU_CLASS_B_MCI
   SF_DISPL_MOTOR_CLASS_B_VERSION               = 0x00,
   SF_DISPL_MOTOR_SR_PARAMS                     = 0x01,

    ////////////////////////////////////////////////////////////

    SF_DISPL_INVALID_VALUE                      = 0xFF
} SETTINGFILE_CROSS_PRODUCT_DISPLACEMENT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SettingFile__Initialize(void);
BOOL_TYPE SettingFile__IsPlatformPointerUsed(uint8 sf_index, SETTINGFILE_OFFSET_TYPE platform_pointer);
BOOL_TYPE SettingFile__IsValid(void);
void SettingFile__CheckDataIntegrity(void);

uint8 SettingFile__GetNumExtraSF(void);

PASS_FAIL_TYPE SettingFile__BasicLoader(SETTINGFILE_OFFSET_TYPE pointer_id, uint16 displacement, SETTINGFILE_LOADER_TYPE * pointer_data_out);
PASS_FAIL_TYPE SettingFile__MSettingBasicLoader( uint8 sf_index,  SETTINGFILE_OFFSET_TYPE  pointer_id, uint16 displacement,  SETTINGFILE_LOADER_TYPE * data_pointer);

uint16 SettingFile__GetNumDisplacements(uint8 sf_index, SETTINGFILE_OFFSET_TYPE pointer_id);
BOOL_TYPE SettingFile__IsExtendedDisplacement(uint8 sf_index,SETTINGFILE_OFFSET_TYPE pointer_id);

uint16 SettingFile__GetSettingFileCRC(SF_ID_TYPE sf_id);
void * SettingFile__GetSettingFileStart(SF_ID_TYPE sf_id);
uint8 SettingFile__GetSectionIDs(SF_ID_TYPE* section_ids);
uint32 SettingFile__GetSettingFileSize(SF_ID_TYPE sf_id);
void * SettingFile__GetMemorySectionStart(void);
uint32 SettingFile__GetMemorySectionSize(void);

void * SettingFile__GetSettingFilePointerByIndex(SF_ID_TYPE sf_id,uint32 index);

uint16 SettingFile__CalculateCRC(SF_ID_TYPE sf_id);

#endif // SETTINGFILE_H_
