/**
 * @file
 * @brief       A Fault Debounce module that uses time to debounce error conditions.
 *
 * @copyright   Copyright 2016,2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "FdmTimer.h"

// -- Other Modules --


//  --- Private Properties --------------------------------------------------------------------------------------------


// -- Private Variable Definitions --

//! The maximum number of debounce data elements found in a single setting file entry (so far).
//! If this value is 2, then the old setting file format is being used.
static uint8 Fdm_Timer_Max_Debounce_Array_Size;


// -- Private Flash-Based Constants --

//! Multipliers used to convert the delay to deciseconds.
const uint16 PRESCALAR_MULTIPLIER[] =
{
    1,                                              //!< 1 delay unit = 100 milliseconds
    10,                                             //!< 1 delay unit = 1 second
    100,                                            //!< 1 delay unit = 10 seconds
    600,                                            //!< 1 delay unit = 1 minute
    6000                                            //!< 1 delay unit = 10 minutes
};


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static BOOL_TYPE SetupTimer(uint8 delay, uint8 prescalar, struct FDM_TIMER_DATA_STRUCT* data);


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup the fault debounce module's data based on the parameters provided.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @param params = Pointer to setting file configuration data for the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__Setup(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    data->Timer.Detecting = TRUE;
    if (SetupTimer(config[0].Delay, config[0].Prescalar, &data->Timer) == FALSE)
    {
        return FAULT_DEBOUNCE_EVENT_NEXT;
    }

    return FAULT_DEBOUNCE_EVENT_NONE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Manage the new detection of an error condition.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__ConditionDetected(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    if (data->Timer.Detecting == FALSE)
    {
        return FdmTimer__Setup(config, data);
    }
    return FAULT_DEBOUNCE_EVENT_NONE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Manage the discovered absence of an error condition.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__ConditionRemoved(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    if (data->Timer.Detecting == TRUE)
    {
        data->Timer.Detecting = FALSE;
        uint8 index = (Fdm_Timer_Max_Debounce_Array_Size <= 2) ? 0 : 2;
        if (SetupTimer(config[index].Delay, config[index].Prescalar, &data->Timer) == FALSE)
        {
            return FAULT_DEBOUNCE_EVENT_CANCEL;
        }
    }
    return FAULT_DEBOUNCE_EVENT_NONE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Manage the passing of time while debouncing a fault.
 * @param data = Pointer to the data used by the Fault Debounce module.
 * @return The event that should be triggered on the parent state machine.
 */
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__TimePassed(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data)
{
    if (data->Timer.Time > 1)
    {
        data->Timer.Time--;
    }
    else if (data->Timer.Detecting == TRUE)
    {
        return FAULT_DEBOUNCE_EVENT_NEXT;
    }
    else
    {
        return FAULT_DEBOUNCE_EVENT_CANCEL;
    }

    return FAULT_DEBOUNCE_EVENT_NONE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Let the FdmTimer module know the number of bytes found in debounce data.
 * This function is used to detect the difference between old and new setting files.
 * For older setting files, this value will always be 2 words (4 bytes).
 * Newer setting files may have 2, 3, or 4 words (4 to 8 bytes).
 * @param debounce_data_size = Number of bytes in a single displacement of debounce data.
 */
void FdmTimer__DebounceDataSize(uint8 debounce_data_size)
{
    uint8 array_size = debounce_data_size / sizeof(FAULT_DEBOUNCE_TYPE);
    if (Fdm_Timer_Max_Debounce_Array_Size < array_size)
    {
        Fdm_Timer_Max_Debounce_Array_Size = array_size;
    }
}


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup the timer to debounce the error condition.
 * Used to debounce both the detection and removal of the error condition.
 * @param delay = The delay set in the setting file data.
 * @param prescalar = The unit of measure for the delay set in the setting file.
 * @param data = Pointer to the data structure used by the timer.
 * @return TRUE if the timer was setup with a non-zero value.
 *          FALSE if the timer value should be zero.
 */
static BOOL_TYPE SetupTimer(uint8 delay, uint8 prescalar, struct FDM_TIMER_DATA_STRUCT* data)
{
    if ((delay > 0) &&
        (prescalar < ELEMENTS_IN_ARRAY(PRESCALAR_MULTIPLIER)))
    {
        data->Time = delay * PRESCALAR_MULTIPLIER[prescalar];
    }
    else
    {
        data->Time = 0;
    }

    return (BOOL_TYPE)(data->Time > 0);
}


