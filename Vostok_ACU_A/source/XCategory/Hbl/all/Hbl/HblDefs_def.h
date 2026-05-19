/**
 *  @file
 *
 *  @brief      Non Safety Relevant Hbl standard definitions
 *
 *  @copyright  Copyright 2015 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef HBLDEFS_DEF_H_
#define HBLDEFS_DEF_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/**
 * @brief HBL PILOT ENUM LIST MACRO
 *
 * @details Non Safety relevant Pilot ID must be listed here
 * NOTE: Range to use is 0-127
 *
 */
#define HBL_PILOT_LIST \
    HBL_PILOT_DIGITAL                       = 0, \
    HBL_PILOT_ANALOG                        = 1, \
    HBL_PILOT_STEPPER                       = 2, \
    HBL_PILOT_TRIAC_SWITCH                  = 3, \
    HBL_PILOT_TRIAC                         = 4, \
    HBL_PILOT_ZEROCROSSING_RELAY            = 5, \
    HBL_PILOT_PWM                           = 10, \
    HBL_PILOT_PWM_HI_RES                    = 11, \
    HBL_PILOT_IMPULSES_DOOR_LOCK            = 13, \
    HBL_PILOT_WAX_DOOR_LOCK                 = 15, \
    HBL_PILOT_PMSM                          = 18, \
    HBL_PILOT_EXTERNAL_MOTOR                = 21, \
    HBL_PILOT_UNIVERSAL_MOTOR               = 22, \
    HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN    = 29, \
    HBL_PILOT_FIXED_SPEED_MOTOR             = 32, \
    HBL_PILOT_STEADY_PWM_PROFILE            = 37, \
    HBL_PILOT_CHARGEPUMP                    = 52, \
    HBL_PILOT_MULTIDRIVER_2                 = 53, \
    HBL_PILOT_MULTIDRIVER_3                 = 54, \
    HBL_PILOT_MULTIDRIVER_4                 = 55, \
    HBL_PILOT_MULTISEQUENCE_1               = 56, \
    HBL_PILOT_MULTISEQUENCE_2               = 57, \
    HBL_PILOT_MULTISEQUENCE_3               = 58, \
    HBL_PILOT_MULTISEQUENCE_4               = 59, \
    HBL_PILOT_EXPANSION_DIGITAL             = 60, \
    HBL_PILOT_NONFUNCTIONAL                 = 61, \
    HBL_PILOT_MAGNETRON                     = 62

/**
 * @brief HBL LLI (Low Level Input) ENUM LIST MACRO
 *
 * @details Non Safety relevant LLI ID must be listed here
 * NOTE: Range to use is 0-127; do not use LLI ID already allocated.
 */
#define HBL_LLI_LIST \
    HBL_LLI_DIGITAL                     =  0, \
    HBL_LLI_ANALOG                      =  1, \
    HBL_LLI_FEEDBACK                    =  2, \
    HBL_LLI_TOUCH_KEY                   =  3, \
    HBL_LLI_BUFFERED_INPUT_CAPTURE      =  4, \
    HBL_LLI_AC_INPUT                    =  5, \
    HBL_LLI_ENCODER                     =  6, \
    HBL_LLI_TRIAC_FEEDBACK              =  7, \
    HBL_LLI_I2C_PRESSURE_SENSOR         =  8, \
    HBL_LLI_INPUTCAPTURE                =  9, \
    HBL_LLI_ZEROCROSS                   = 10, \
    HBL_LLI_PULSE_COUNTER               = 11, \
    HBL_LLI_PEAK_ANALOG                 = 13, \
    HBL_LLI_EXPANSION                   = 16, \
    HBL_LLI_MULTIINPUT_2                = 18, \
    HBL_LLI_MULTIINPUT_3                = 19, \
    HBL_LLI_MULTIINPUT_4                = 20, \
    HBL_LLI_TOUCH_SLIDER                = 21, \
    HBL_LLI_TOUCH_WHEEL                 = 22, \
    HBL_LLI_VIRTUALDIGITAL              = 23, \
    HBL_LLI_VIRTUALDIGITAL_4            = 24, \
    HBL_LLI_CHARGEPUMP_FEEDBACK         = 25, \
    /* HBL_LLI_SRTOUCH_KEY              = 26 */ \
    /* HBL_LLI_SRDIGITAL                = 27 */ \
    HBL_LLI_I2C_INFRARED_SENSOR         = 28, \
    HBL_LLI_ABSOLUTE_HUMIDITY           = 29, \
    HBL_LLI_NONFUNCTIONAL               = 30, \
    HBL_LLI_I2C_RHT_HUMIDITY_SENSOR     = 31, \
    HBL_LLI_I2C_RHT_TEMEPERATURE_SENSOR = 32


/**
 * @brief HBLHMI LLI Enum
 *
 * @details Do not use these LLI ID already allocated:
 */
#define HBLHMI_GI_LIST \
    HBLHMI_GI_TOUCH_KEY                     = 0x00, \
    HBLHMI_GI_DIGITAL_KEY                   = 0x01, \
    HBLHMI_GI_ABSOLUTE_ENCODER              = 0x02, \
    HBLHMI_GI_INCREMENTAL_ENCODER           = 0x03, \
    HBLHMI_GI_ANALOG_POTENTIOMETER          = 0x04, \
    HBLHMI_GI_DISCRETE_POTENTIOMETER        = 0x05, \
    HBLHMI_GI_TOUCH_SLIDER                  = 0x06, \
    HBLHMI_GI_TOUCH_WHEEL                   = 0x07, \
    HBLHMI_GI_EXPANSION_KEY                 = 0x08, \
    /* HBLHMI_GI_SRKEY                      = 0x09, */ \
    /* HBLHMI_GI_ANALOG_SRPOTENTIOMETER     = 0x0A, */ \
    /* HBLHMI_GI_DISCRETE_SRPOTENTIOMETER   = 0x0B, */ \


/**
 * @brief Visual object types supported by the HblHmi
 */
typedef enum
{
    VISUAL_OBJECT_LED,   //!< VISUAL_OBJECT_LED
    VISUAL_OBJECT_BAR,   //!< VISUAL_OBJECT_BAR
    VISUAL_OBJECT_DIGIT, //!< VISUAL_OBJECT_DIGIT
    VISUAL_OBJECT_STRING,//!< VISUAL_OBJECT_STRING
    VISUAL_OBJECT_ICON   //!< VISUAL_OBJECT_ICON
}VISUAL_OBJECT_TYPE;


/**
 * @brief PWM Index number used to select a pwm to control the intensity of one or more visual objects
 */
typedef enum
{
    VIEWMGR_PWM_0 = 0,     //!< VIEWMGR_PWM_0
    VIEWMGR_PWM_1,         //!< VIEWMGR_PWM_1
    VIEWMGR_PWM_2,         //!< VIEWMGR_PWM_2
    VIEWMGR_PWM_3,         //!< VIEWMGR_PWM_3
    VIEWMGR_PWM_MAX,       //!< VIEWMGR_PWM_MAX
    VIEWMGR_PWM_NONE = 0xFF//!< VIEWMGR_PWM_NONE
}VIEWMGR_PWM_TYPE;


#endif // HBLDEFS_H_


