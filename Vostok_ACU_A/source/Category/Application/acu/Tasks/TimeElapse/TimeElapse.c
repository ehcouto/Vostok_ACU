/**
 *  @file       Time Elapse
 *
 *  @brief      The Task Time Elapse deals with counting the time
 *
 *  @details    The algorithm time elapse deals with counting the time
 *              (the value of the time elapsed is available for the execution of the jumpif)
 *              When this algorithm is activated, the counter is reset and restarted
 *              (it counts in seconds starting from 0). When deactivated,
 *              the counter if reset and stopped.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "TimeElapse.h" 
#include "NVData.h"
#include "Micro.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#pragma location = "_appliance_segment"     // Need to save data on a black out event

// Need to tie up this variable to be used in the variable module
uint16 TimeElapse_TimeCount;    // Counter to count the time elapse resolution is set to 1 sec

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define COUNTS_PER_SEC              10    //Calling rate is 100msec so 10 counts is 1 sec

typedef union
{
    uint8 Timer_Bytedata;
    struct
    {
        uint8 Pause_Flag:1;
        uint8 Time_Prescaler:7;
    }bits;
}TIMER_STATUS_TYPE;

#pragma location = "_appliance_segment"     // Need to save data on a black out event
static TIMER_STATUS_TYPE Timer_Data;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
/*@startuml
title TimeElapse state Machine

@enduml
*/

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initialises the module TimeElapse and its variables
 *
 */
ACTIVE_TYPE TimeElapse__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement)
{
    ACTIVE_TYPE revalue = ACTIVE;
    TimeElapse_TimeCount = 0; // Initialise the variable
    Timer_Data.Timer_Bytedata =0; // Initialise the flag and the counter
    return(revalue);
}


/**
 * @brief The function called at the initialise of the task after NVdata restore
 *        It Initialises the Time Elapse Module and its variables
 *
 * @param compartment that run the task (unused)
 *
 * @return  None
 */

void TimeElapse__NVDataRestore(uint8 compartment)
{
    Timer_Data.bits.Time_Prescaler = 0; // Initialise the flag and the counter
}

/**
 * The function called during the execution of the task
 * @param compartment that run the task (inside)
 * @return the status of the task (ACTIVE or INACTIVE)
 */
ACTIVE_TYPE TimeElapse__Task(uint8 compartment)
{
    ACTIVE_TYPE   revalue = ACTIVE; // Update the Variable as active

    // Check if we are not in the pause state as the falg would be set in the pause state.
    if((uint8)FALSE == (uint8)Timer_Data.bits.Pause_Flag)
    {
        Timer_Data.bits.Time_Prescaler++;

        if(Timer_Data.bits.Time_Prescaler >= (uint8)COUNTS_PER_SEC) // check for the 1 sec elapse time
        {
            TimeElapse_TimeCount++;   // Increment the Time as 1 sec has elapsed
            Timer_Data.bits.Time_Prescaler = 0;       // Reinitialise the variable so that it can we used in next iteration
        }
    }
    //else do nothing as we need not count down
    return(revalue);
}



/**
 * The function used when the task terminate. It reinitialise the Counter and corresponding variables of the task
 * @param compartment the compartment that uses the task (unused)
 *
 * @return : None
 */


void TimeElapse__Terminate(uint8 compartment)
{
    TimeElapse_TimeCount = 0; // Initialise the variable
    Timer_Data.Timer_Bytedata = 0;
}

void TimeElapse__Pause(void)
{
    Timer_Data.bits.Pause_Flag = TRUE;
}

void TimeElapse__Resume(void)
{
    Timer_Data.bits.Pause_Flag = FALSE;
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


