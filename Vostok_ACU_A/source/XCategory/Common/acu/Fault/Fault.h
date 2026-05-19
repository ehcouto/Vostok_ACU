/**
 * @file
 * @brief       Public interface to the Fault module.
 *
 * @details     Refer to the Fault.c source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef FAULT_H_
#define FAULT_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "Fault_prm.h"

// -- Other Modules --
#include <limits.h>


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --


// -- Public Type Declarations --

//! A function pointer prototype for a function that handles faults.
typedef void (*FAULT_EVENT_HANDLER_TYPE)(uint8 fault_observer_id);

//! A function pointer prototype for a function that handles faults.
typedef void (*PRE_FAULT_EVENT_HANDLER_TYPE)(uint8 fault_observer_id);

//! Information that is stored in the setting file and relevant to a pre-fault or a fault event.
typedef struct FAULT_CONFIG_STRUCT
{
    //! The unique fault ID.
    FAULT_ID_TYPE Id;

    //! The displacement for the debounce method used to detect a pre-fault and a fault.
    uint8 Debounce_Method_Displacement;

    //! The compartment associated with the fault.
    //! The fault can only occur in this compartment.
    uint8 Compartment       : 4;

    uint8 Reserved          : 4;

    //! The fault code used when the fault must be displacement and/or transmitted on WIN.
    uint8 Code              : 4;

    //! The fault sub-code used when the fault must be displacement and/or transmitted on WIN.
    uint8 Sub_Code          : 4;

    //! The fault engineering code used when the fault is transmitted on WIN.
    uint8 Engineering_Code  : 4;

    // Flags that describe additional behaviors:

    //! Configure whether the state machine should transition from Alarm directly to Fault.
    //!    FALSE = Transition from Alarm to Pre-Fault (normal behavior).
    //!     TRUE = Transition directly from Alarm to Fault.
    BOOL_TYPE Direct_To_Fault               : 1;

    //! Configure whether the Supervisor should transfer to the pause state on a pre-fault.
    //!    FALSE = Continue running the cycle (normal behavior).
    //!     TRUE = Pause the cycle if a pre-fault condition occurs.
    BOOL_TYPE Pause_Cycle                   : 1;

    //! Configure whether the fault observer should remember the fault across a cold reset.
    //!    FALSE = Remember the fault if a cold reset occurs (normal behavior).
    //!     TRUE = Forget the fault if a cold reset occurs.
    BOOL_TYPE Clear_On_Cold_Reset           : 1;

    //! Configure whether the fault observer will allow a fault to be reset without removing the
    //! error condition.
    //!    FALSE = The error condition must be removed before the fault can be reset.
    //!     TRUE = The fault can be reset even if the error condition was not removed.
    BOOL_TYPE Allow_Clear_Without_Remove    : 1;

    //! The sub-cycle (if any) that is triggered by the pre-fault.
    uint8 Sub_Cycle;

    //! The fault phase (if any) executed by the Supervisor that is triggered by the fault.
    uint8 Phase;

    //! The displacement for the fault specific data.
    uint8 Fault_Specific_Displacement;
} FAULT_CONFIG_TYPE;

//! A list of all the fault IDs that are configured in the setting file.
typedef struct FAULT_LIST_STRUCT
{
    //! The number of fault IDs that are configured in the setting file.
    uint8 Number_Of_Faults;

    //! A list of the configured fault IDs sorted by priority (highest to lowest).
    //! Although the array declaration contains many elements,
    //! the array definition only contains Number_Of_Faults elements.
    FAULT_ID_TYPE Fault_Ids[UCHAR_MAX];
} FAULT_LIST_TYPE;

//! A list of all the possible states of a fault
typedef enum FAULT_STATE_ENUM
{
    FAULT_STATE_NONE = 0,
    FAULT_STATE_ALARM,
    FAULT_STATE_PRE_FAULT,
    FAULT_STATE_FAULT
}FAULT_STATE_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void Fault__Initialize(void);
void Fault__Background(void);
void Fault__CompartmentManaged(uint8 compartment);
BOOL_TYPE Fault__CompartmentReset(uint8 compartment);
void Fault__ConditionDetected(FAULT_ID_TYPE fault_id);
void Fault__ConditionRemoved(FAULT_ID_TYPE fault_id);
FAULT_LIST_TYPE* Fault__GetConfiguredFaults(void);
void Fault__GetEventInfo(uint8 observer, FAULT_CONFIG_TYPE** info);
FAULT_CONFIG_TYPE* Fault__GetParameters(FAULT_ID_TYPE fault_id);
uint32 Fault__GetRemainingTime(FAULT_ID_TYPE fault_id);
void* Fault__GetSpecificData(FAULT_ID_TYPE fault_id);
FAULT_STATE_TYPE Fault__GetState(FAULT_ID_TYPE fault_id);
BOOL_TYPE Fault__IsInFaultState(FAULT_ID_TYPE fault_id);
BOOL_TYPE Fault__IsObserverActive(FAULT_ID_TYPE fault_id);
BOOL_TYPE Fault__IsOkToExitFaultState(uint8 compartment);
void Fault__RegisterFaultEvents(FAULT_EVENT_HANDLER_TYPE fault_handler_function);
void Fault__RegisterPreFaultEvents(PRE_FAULT_EVENT_HANDLER_TYPE pre_fault_handler_function);
void Fault__ResetAllObservers(void);
void Fault__SetTestEngaged(BOOL_TYPE engaged);
void Fault__UnregisterFaultEvents(FAULT_EVENT_HANDLER_TYPE fault_handler_function);
void Fault__UnregisterPreFaultEvents(PRE_FAULT_EVENT_HANDLER_TYPE pre_fault_handler_function);


#endif      /* FAULT_H_ */
