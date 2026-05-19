/**
 * @file
 * @brief       Public interface to the API018Diagnostic module.
 *
 * @details     Refer to the API018Diagnostic.c source file for more detailed information.
 *
 * @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL.
 */

#ifndef API018DIAGNOSTIC_H_
#define API018DIAGNOSTIC_H_


#if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#if (API018_SERVER_FEATURE == ENABLED)
    #include "Fault.h"
#endif
#include <limits.h>
#include "Reveal.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! A constant used to identify the API implemented by this module.
#define API018DIAGNOSTIC_NUM        0x12    //18
#define API018DIAGNOSTIC_TYPE       0x5
#define API018DIAGNOSTIC_VERSION    0x0
#define API018DIAGNOSTIC_INSTANCES  0x1


// -- Public Constant Declarations --

//! A list of all cross-platform fault keys that can be generated through an API message.
//! These keys can each be associated with individual fault_id values that are platform-specific.
typedef enum API018_FAULT_KEY_ENUM
{
    //! Dummy value reserved for clarity.
    FAULT_KEY_NONE                                  = 0x0000,

    //! Setting file configuration does not match existing hardware.
    //! Setting file configuration does not match expected model.
    FAULT_KEY_CONFIGURATION_MISMATCH                = 0x0001,

    //! Measured operating temperature is too hot.
    FAULT_KEY_OVER_TEMPERATURE                      = 0x0002,

    //! Touch detects key is non-operational.
    //! Tactile switch pressed for too long.
    FAULT_KEY_STUCK_KEY                             = 0x0101,

    //! Not able to establish I2C communications with touch keypad.
    //! Not able to read diodes on tactile switches.
    FAULT_KEY_KEYPAD_DISCONNECTED                   = 0x0102,

    //! Not able to establish communications with LCD display.
    //! Not able to establish communication with LED device.
    FAULT_KEY_DISPLAY_DISCONNECTED                  = 0x0103,

    //! Not able to establish SPI communication with audio device.
    FAULT_KEY_AUDIO_DEVICE                          = 0x0104,

    //! Touch detects cancel key is non-operational.
    //! Tactile cancel key pressed for too long.
    FAULT_KEY_STUCK_CANCEL_KEY                      = 0x0105,

    //! The model number received from the ACU does not match the one on the HMI
    FAULT_KEY_MODEL_NUMBER_MISMATCH                 = 0x0106,

    //! The model number on the ACU is missing
    //! This can happen when an ACU is replaced in the field
    FAULT_KEY_MODEL_NUMBER_MISSING                  = 0x0107,

    //! Regulation(s) unexpectedly rejected by the ACU
    FAULT_KEY_REGULATION_REJECTED                   = 0x0108,

    //! The door switch has not opened , so no cycle should start.
    FAULT_KEY_DOOR_SWITCH_STUCK_CLOSED              = 0x0110,

    //! The door switch has not closed, so no cycle should start.
    FAULT_KEY_DOOR_SWITCH_STUCK_OPEN                = 0x0111,

    //! An accelerometer has detected a failure of an internal self-test.
    FAULT_KEY_ACCELEROMETER_SELF_TEST               = 0x0201,

    //! An accelerometer has detected improper orientation of the sensor.
    FAULT_KEY_ACCELEROMETER_ORIENTATION             = 0x0202,

    //! Not able to establish communication with an accelerometer
    FAULT_KEY_ACCELEROMETER_NOT_RESPONDING          = 0x0203,

    //! Accelerometer is not indicating any new data is available
    FAULT_KEY_ACCELEROMETER_OLD_DATA                = 0x0204,

    //! Accelerometer data being received faster than the application can process it (detected by firmware)
    FAULT_KEY_ACCELEROMETER_DATA_OVERFLOW           = 0x0205,

    //! New accelerometer data was written before the previous data was read (detected by accelerometer based on ODR and read command)
    FAULT_KEY_ACCELEROMETER_DATA_OVERWRITTEN        = 0x0206,

    //! Maximum is defined to force a uint16 data type.
    FAULT_KEY_MAX_VALUE                             = 0xFFFF
} API018_FAULT_KEY_TYPE;


// -- Public Type Declarations --

#if (API018_SERVER_FEATURE == ENABLED)
    //! A structure that links an external fault key to a platform-specific fault ID.
    // This structure is defined here for use in the API018Diagnostic_prv.h header file.
    typedef struct API018_FAULT_KEY_ID_PAIR_STRUCT
    {
        API018_FAULT_KEY_TYPE Key;                  //!< External cross-platform fault key.
        uint8 Node;                                 //!< Communication node that reported the fault.
        FAULT_ID_TYPE Fault_Id;                     //!< Platform-specific fault ID.
    } API018_FAULT_KEY_ID_PAIR_TYPE;
#else   /* if only API018_CLIENT_FEATURE is ENABLED */
    //! This is the payload of the API018_FBK_PUBLISH_LIFETIME_DATA message.
    //! Measurements related to the total life of the appliance and appliance components.
    typedef struct API018_COMMON_LIFETIME_STRUCT
    {
        uint32 Seconds;
        uint16 Cycle_Count;
        uint32 Seconds_Running_Cycles;
    } API018_COMMON_LIFETIME_TYPE;
#endif //#if ((API018_SERVER_FEATURE == DISABLED)

//! A structure that holds a fault error code.
typedef struct API018_ERROR_CODE_STRUCT
{
    uint8 F_Code;
    uint8 E_Code;
    uint8 Eng_Code;
} API018_ERROR_CODE_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void            API018Diagnostic__Initialize        (void);
void            API018Diagnostic__Handler25ms       (void);

REVEAL_RECIPE_STATUS_TYPE API018Diagnostic__CommandParser(REVEAL_MSG_TYPE* cmd);
void            API018Diagnostic__FeedbackParser    (REVEAL_MSG_TYPE* fbk);
void            API018Diagnostic__AckParser         (uint8 node, uint8 api, uint8 op_code, REVEAL_RECIPE_STATUS_TYPE reason_code);

#if (API018_SERVER_FEATURE == ENABLED)
    PASS_FAIL_TYPE API018Diagnostic__PublishFaultBasic(uint8 requested_fault_index);
#else   /* if only API018_CLIENT_FEATURE is ENABLED */
    PASS_FAIL_TYPE API018Diagnostic__RequestCommonLifeTimeData(uint8 node);
    API018_COMMON_LIFETIME_TYPE* API018Diagnostic__GetCommonLifeTimeData(void);
#endif   // #if ((API018_SERVER_FEATURE == ENABLED)
#if (API018_CLIENT_FEATURE == ENABLED)
    PASS_FAIL_TYPE API018Diagnostic__ClearFaults(uint8 node);
    PASS_FAIL_TYPE API018Diagnostic__GenerateFault(API018_FAULT_KEY_TYPE fault_key);
    API018_ERROR_CODE_TYPE Api018Diagnostic__GetErrorCode(uint8 fault_index);
    uint8          Api018Diagnostic__GetNumberOfErrorCodes(void);
    PASS_FAIL_TYPE API018Diagnostic__RemoveFault(API018_FAULT_KEY_TYPE fault_key);
    PASS_FAIL_TYPE API018Diagnostic__RequestFaultBasic(uint8 node, uint8 fault_index);
#endif


#endif      // #if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
#endif      // API018DIAGNOSTIC_API018DIAGNOSTIC_H_

