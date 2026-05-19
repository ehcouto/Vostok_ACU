/**
 *  @file
 *
 *  @brief      Cross category Class A definitions for the SettingFile module.
 *
 *  @copyright  Copyright 2015-2023. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SETTINGFILE_DEFS_H_
#define SETTINGFILE_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Cross category pointers that do not have any safety relevant data
#define SETTINGFILE_CROSS_CATEGORY_SF_PTR       \
    /* ACU */                                   \
    SF_PTR_ACU_MODIFIERS_EXTENDED    = 0x000E,  \
    /* 0x000F is RESERVED */                    \
    SF_PTR_ACU_DEBUG_DATA            = 0x0042,  \
                                                \
    /* HMI */                                   \
    SF_PTR_UI_DATA_MODEL_TRANSLATION = 0x012C,  \
    SF_PTR_UI_DATA_MODEL_KEY_MAPPING = 0x012D,  \
    SF_PTR_UI_MODIFIERS              = 0x013D,  \
    SF_PTR_UI_CYCLE_MAPPING          = 0x013E,  \
    SF_PTR_UI_SERIAL_NUMBER          = 0x013F,  \
    SR_PTR_UI_EXPANSION_BOARD        = 0x0150,

//! Standard displacements for the SF_PTR_MCU_CLASS_A_MCI setting file pointer
typedef enum SETTINGFILE_MCU_CLASS_A_MCI_DISPLACEMENT_ENUM
{
    SF_DISPL_MOTOR_CLASS_A_VERSION      = 0x00,
    SF_DISPL_MOTOR_PARAMS_DTC           = 0x01,
    SF_DISPL_MOTOR_PARAMS_CIM           = 0x02,
    SF_DISPL_MOTOR_PARAMS_LUT           = 0x03,
    SF_DISPL_MOTOR_SPEED_CTRL           = 0x04,
    SF_DISPL_MOTOR_PWM_MODULATOR        = 0x05,
    SF_DISPL_MOTOR_RS                   = 0x06,
    SF_DISPL_MOTOR_OTE                  = 0x07,
} SETTINGFILE_MCU_CLASS_A_MCI_DISPLACEMENT_TYPE;

//! Displacements for cross-product pointers (Non-safety relevant)
// New non-safety relevant cross category displacements must be added here
// New category specific safety relevant displacements must be in SettingFile_SRCat.h, else they can be added in SettingFile_prm.h
typedef enum SETTINGFILE_NON_SR_CROSS_PRODUCT_DISPLACEMENT_ENUM
{
    // Displacement numbers for SF_PTR_ACU_CROSS_PRODUCT_CONFIG
    SF_DISPL_ACU_EXPANSION_CONFIGURATION        = 0x04,
    SF_DISPL_LO_STATEMENT_DISPLACEMENT_SIZE     = 0x06,
    SF_DISPL_FVT_SETTING_FILE                   = 0x07,
    SF_DISPL_MAIN_HMI_NODE                      = 0x08,

    // Displacement numbers for SF_PTR_UI_PRODUCT_CONFIG
    SF_DISPL_FAULT_CODES                        = 0x05,
    SF_DISPL_VISUAL_BOARD_TYPE                  = 0x07,
    // Displacements 0x08 - 0x0D are defined in SettingFile.h
    SF_DISPL_FUNCTIONAL_TIMERS                  = 0x0E,
    SF_DISPL_NODE_ID                            = 0x0F,

    // Displacement numbers for SF_PTR_UI_IO_CONFIG
    SF_DISPL_INPUT_EVENT_MAP                    = 0x03,
    SF_DISPL_LED_FUNCTION_MAP                   = 0x06,
    SF_DISPL_AUDIO_FUNCTION_MAP                 = 0x08,
    SF_DISPL_ANIMATION_FUNCTION_MAP             = 0x09,

    //Displacement number for product config data
    SF_DISPL_PRODUCT_CONFIGURATION_DATA         = 0X09,

    //Displacement number for SR_PTR_UI_EXPANSION_BOARD
    SF_DISPL_EXP_BOARD_INFO                     = 0X00,
    SF_DISPL_EXP_LED_FUNC_ID_INFO               = 0x01,
    SF_DISPL_EXP_SOUND_FUNC_ID_INFO             = 0x02,
    SF_DISPL_EXP_ANIMATION_FUNC_ID_INFO         = 0x03,

} SETTINGFILE_NON_SR_CROSS_PRODUCT_DISPLACEMENT_TYPE;

#endif // SETTINGFILE_DEFS_H_
