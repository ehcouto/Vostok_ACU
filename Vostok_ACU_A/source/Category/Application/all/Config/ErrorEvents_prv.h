/**
 *  @file
 *
 *  @brief  Private parameters for the ErrorEvents module.
 *
 *  $Header: ErrorEvents_prv.h 1.20 2014/10/21 20:30:55EDT Wei Miao Wu (WUW) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2014/10/21 20:30:55EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ERROREVENTS_PRV_H_
#define ERROREVENTS_PRV_H_

#include "Log.h"
#include "SettingFile.h"

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================


//----------------------------------Set Registry Size(s)---------------------------------------------------------------
/**
 *  Number of observers allowed to attach to ErrorEvents for Error_Report callback(s)
 *  Set to 0 if not used.
 */
#define ERROR_REPORT_CALLBACK_REGISTRY_SIZE (5)

/**
 *  Number of observers allowed to attach to ErrorEvents for Error_Store callback(s)
 *  Set to 0 if not used.
 */
#define ERROR_STORE_CALLBACK_REGISTRY_SIZE (5)

/**
 *  Number of observers allowed to attach to ErrorEvents for Cycle_Stop callback(s).
 *  Set to 0 if not used.
 */
#define CYCLE_STOP_CALLBACK_REGISTRY_SIZE (5)

/**
 *  Number of observers allowed to attach to ErrorEvents for Application Specific 1 callback(s).
 *  Set to 0 if not used.
 */
#define APP_SPECIFIC_1_CALLBACK_REGISTRY_SIZE (5)

/**
 *  Number of observers allowed to attach to ErrorEvents for Application Specific 2 callback(s).
 *  Set to 0 if not used.
 */
#define APP_SPECIFIC_2_CALLBACK_REGISTRY_SIZE (5)

/**
 *  Number of observers allowed to attach to ErrorEvents for Application Specific 3 callback(s).
 *  Set to 0 if not used.
 */
#define APP_SPECIFIC_3_CALLBACK_REGISTRY_SIZE (0)

/**
 *  Number of observers allowed to attach to ErrorEvents for Application Specific 4 callback(s).
 *  Set to 0 if not used.
 */
#define APP_SPECIFIC_4_CALLBACK_REGISTRY_SIZE (0)

/**
 *  Number of observers allowed to attach to ErrorEvents for Application Specific 5 callback(s).
 *  Set to 0 if not used.
 */
#define APP_SPECIFIC_5_CALLBACK_REGISTRY_SIZE (0)


//----------------------------------Set application specific masks for notification flags------------------------------

//! Mask for Application Specific 1 Notify Flags
#define MASK_APP_SPECIFIC_1 ((EVENT_MASK_TYPE)(1 << ERROR_EVENTS_OBSERVER_ALARM_REPORT))

//! Mask for Application Specific 2 Notify Flags
#define MASK_APP_SPECIFIC_2 ((EVENT_MASK_TYPE)(1 << ERROR_EVENTS_OBSERVER_ALARM_STORE))

//! Mask for Cycle Pause Callback Notify Flag
#define MASK_CYCLE_PAUSE    ((EVENT_MASK_TYPE)(1 << ERROR_EVENTS_OBSERVER_CYCLE_PAUSE))

//! Mask to set all notify Flags for project specific application
#define MASK_ALL_NAR (MASK_REPORT | MASK_STORE | MASK_CYCLE_STOP)


//----------------------------------Application specific debug macros.-------------------------------------------------

//! Define Log Messages to post when an error is discovered in ErrorEvents.
typedef enum MODULE_ERROR_EVENTS_LOG_MESSAGE_ID_ENUM
{
    REGISTER_INVALID_OBSERVER_FAILED,
    UNREGISTER_INVALID_OBSERVER_FAILED,
    NOTIFY_INVALID_ERROR_EVENT_FAILED,
    REQUEST_INVALID_ERROR_EVENT_DATA
} MODULE_ERROR_EVENTS_LOG_MESSAGE_ID_TYPE;

//! Debug macro called when an attempt is made to register a handler with an invalid observer type.
//! @param observer_type =  Invalid observer type.
#define ATTEMPT_TO_REGISTER_WITH_INVALID_OBSERVER_TYPE(observer_type) \
    Log__AddMessage(MODULE_ERROR_EVENTS, REGISTER_INVALID_OBSERVER_FAILED, LOG_EXCEPTION, observer_type);

//! Debug macro called when an attempt is made to unregister a handler with an invalid observer type.
//! @param observer_type =  Invalid observer type.
#define ATTEMPT_TO_UNREGISTER_WITH_INVALID_OBSERVER_TYPE(observer_type) \
    Log__AddMessage(MODULE_ERROR_EVENTS, UNREGISTER_INVALID_OBSERVER_FAILED, LOG_EXCEPTION, observer_type);

//! Debug macro called when an attempt is made to notify with an invalid error event.
//! @param error_event =  Invalid error_event.
#define ATTEMPT_TO_NOTIFY_WITH_INVALID_ERROR_EVENT(error_event) \
    Log__AddMessage(MODULE_ERROR_EVENTS, NOTIFY_INVALID_ERROR_EVENT_FAILED, LOG_EXCEPTION, error_event);

//! Debug macro called when an attempt is made to get data with an invalid error event.
//! @param error_event =  Invalid error_event.
#define ATTEMPT_TO_GET_DATA_WITH_INVALID_ERROR_EVENT(error_event) \
    Log__AddMessage(MODULE_ERROR_EVENTS, REQUEST_INVALID_ERROR_EVENT_DATA, LOG_EXCEPTION, error_event);


//----------------------------------Application specific ErrorEvents data.---------------------------------------------
/**
 * Table of error information for each service error code.
 * These table entries must correspond to the ERROR_EVENT_TYPE enumerated values in ErrorEvents_prm.h.
 *
 * This dynamic file in MKS GPL is currently set for typical NAR projects.
 * For typical EMEA project change to the commented out format of ERROR_EVENTS_DATA definition.
 *
 * F = F Code (nibble)
 * E = E Code (nibble)
 * EN = Engineering Number (nibble)
 * PH = Phase that is to be sent to Supervisor determining what phase to run under the fault cycle
 * PFTO = Pre Fault Time offset (used to extract pre-fault time from setting file)
 * FTO = Fault Time offset (used to extract fault time from setting file)
 * DSP = Timeout displacement data offset
 * EVENT_MASK = Callback Notification type
 * Compartment = compartment where fault exists
 *
 */
ERROR_EVENTS_DATA_TYPE const ERROR_EVENTS_DATA[] =
{
//    F   E   EN  PH    PFTO   FTO   DSP               (EVENT_MASK)             ERROR_EVENT_TYPE enumerated value
//   --- --- --- ---    ---------------------------------------------
    { 0,  0,  0,  0,      0,   0,    DSPL_MOTOR_FAULT, (0),                 0}, // ERROR_EVENT_NONE - Mandatory (Do Not Remove)
    { 1,  2,  0,  1,    000,   000,  DSPL_MOTOR_FAULT, (MASK_CYCLE_STOP),   0}, // ERROR_EVENT_TEST_FAULT
    { 3,  4,  0,  0,    000,   003,  DSPL_MOTOR_FAULT, (MASK_CYCLE_STOP),   0}, // ERROR_EVENT_DOOR_SWITCH_FAULT
    { 6,  1,  0,  0,    000,   003,  DSPL_MOTOR_FAULT, (MASK_CYCLE_STOP),   0}  // ERROR_EVENT_THERMISTOR_FAULT
};



#endif // ERROREVENTS_PRV_H_
