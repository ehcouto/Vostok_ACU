/**
 * @file
 * @brief       A Fault Debounce module that counts error detection events to debounce error conditions.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "FdmCounter.h"

// -- Other Modules --


//  --- Private Properties --------------------------------------------------------------------------------------------


//  --- Private Function Prototypes -----------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup the fault debounce module's data based on the parameters provided.
 *
    @startuml
        title Setup (Event-Based Fault Debounce Method)
        hide footbox
        autonumber

        participant Fault as flt
        participant DebounceMethod as fdm

        flt -> fdm : FdmCounter__Setup()
        alt Event counter is 0 or 1
            flt <-- fdm : FAULT_DEBOUNCE_EVENT_NEXT
        else else
            flt <-- fdm : FAULT_DEBOUNCE_EVENT_NONE
        end
    @enduml
 *
 * @param config = Pointer to setting file configuration data for the Fault Debounce module.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__Setup(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    uint8 counter;

    // Initialize the debounce event counter.
    counter = config->Delay;
    if (counter > 0)
    {
        counter--;
    }
    data->Counter = counter;

    // If the counter is already 0, then trigger a pre-fault or a fault.
    if (counter == 0)
    {
        return FAULT_DEBOUNCE_EVENT_NEXT;
    }

    return FAULT_DEBOUNCE_EVENT_NONE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Manage the new detection of an error condition.
 *
    @startuml
        title Condition Detected (Event-Based Fault Debounce Method)
        hide footbox
        autonumber

        participant Fault as flt
        participant DebounceMethod as fdm

        flt -> fdm : FdmCounter__ConditionDetected()
        alt Event counter reached limit
            flt <-- fdm : FAULT_DEBOUNCE_EVENT_NEXT
        else else
            flt <-- fdm : FAULT_DEBOUNCE_EVENT_NONE
        end
    @enduml
 *
 * @param config = Pointer to setting file configuration data for the Fault Debounce module.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__ConditionDetected(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    if (data->Counter > 1)
    {
        data->Counter--;
    }
    else
    {
        data->Counter = 0;
        return FAULT_DEBOUNCE_EVENT_NEXT;
    }

    return FAULT_DEBOUNCE_EVENT_REPEAT;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Manage the discovered absence of an error condition.
 *
    @startuml
        title Condition Removed (Event-Based Fault Debounce Method)
        hide footbox
        autonumber

        participant Fault as flt
        participant DebounceMethod as fdm

        flt -> fdm : FdmCounter__ConditionRemoved()
        flt <-- fdm : FAULT_DEBOUNCE_EVENT_CANCEL
    @enduml
 *
 * @param config = Pointer to setting file configuration data for the Fault Debounce module.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__ConditionRemoved(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    return FAULT_DEBOUNCE_EVENT_CANCEL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Manage the passing of time while debouncing a fault.
 *
    @startuml
        title Time Passes (Event-Based Fault Debounce Method)
        hide footbox
        autonumber

        participant Fault as flt
        participant DebounceMethod as fdm

        flt -> fdm : FdmCounter__TimePasses()
        flt <-- fdm : FAULT_DEBOUNCE_EVENT_NONE
    @enduml
 *
 * @param config = Pointer to setting file configuration data for the Fault Debounce module.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__TimePassed(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    return FAULT_DEBOUNCE_EVENT_NONE;
}


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


