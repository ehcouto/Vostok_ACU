/**
 *  @file
 *
 *  @brief  Public parameters for the ErrorEvents module.
 *
 *  @copyright  Copyright 2013-2015. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

#ifndef ERROREVENTS_PRM_H_
#define ERROREVENTS_PRM_H_


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


// -- Public Type Declarations --

/**
 * Enumerated service error events.
 * These enumerated values must correspond to the ERROR_EVENTS_DATA table entries in ErrorEvents_prv.h.
 */
typedef enum
{
    ERROR_EVENT_NONE = 0,                           //!< Mandatory (Do Not Remove)
    ERROR_EVENT_TEST_FAULT,                         //!< Describe project specific error
    ERROR_EVENT_DOOR_SWITCH_FAULT,                  //!< Door switch fault
    ERROR_EVENT_THERMISTOR_FAULT,                   //!< Thermistor probe fault

    NUMBER_OF_ERROR_EVENTS                          //!< Mandatory (Do Not Remove)
} ERROR_EVENT_TYPE;

/**
 *  Project/Region service fault code type.
 *
 *  This dynamic file in MKS GPL is currently set for typical NAR projects.
 *  For typical EMEA project change to typedef uint8 ERROR_EVENT_INFO_TYPE;
 */
typedef struct
{
    uint8 F_Code            :4;             //!< Indicates which component category has a failure or malfunction
    uint8 E_Code            :4;             //!< Indicates component system that has an error
    uint8 Eng_Number        :4;             //!< Differentiates multiple sources of the same FxEx service error code.
} ERROR_EVENT_INFO_TYPE;

/**
 *  Enumerated values for ErrorEvents Observer Types.
 *  Application specific observer types can be uncommented and renamed appropriately.
 */
typedef enum
{
    ERROR_EVENTS_OBSERVER_REPORT = 0,               //!< Mandatory (Do Not Remove)
    ERROR_EVENTS_OBSERVER_STORE,                    //!< Mandatory (Do Not Remove)
    ERROR_EVENTS_OBSERVER_CYCLE_STOP,               //!< Mandatory (Do Not Remove)

    ERROR_EVENTS_OBSERVER_PRE_FAULT,

    NUMBER_OF_ERROR_EVENTS_OBSERVER_TYPES           //!< Mandatory (Do Not Remove - Must Be Last and less then 9)
} ERROR_EVENTS_OBSERVER_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#endif // ERROREVENTS_PRM_H_
