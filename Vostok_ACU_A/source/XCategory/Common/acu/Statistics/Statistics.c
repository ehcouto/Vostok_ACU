/**
 * @file
 * @brief       Manages all statistics data.
 *
 * @details
 *
 * The Statistics module stores information that may be useful to a diagnostic tool.
 *
 *      - Lifetime measurements
 *          - Lifetime data that measures the total life of the appliance components
 *      - Cycle measurements [ last C cycles run ]
 *          - Measurements that describe the behavior of completed and interrupted cycles
 *          - Optionally, more detailed data for the most recent last cycles
 *      - Fault information [ last F faults detected ]
 *          - Fault ID
 *          - Number of occurrences
 *          - Black box data at the time the fault occurred
 *          - Cycle data for the most recent cycle
 *          - Optionally, more detailed data for the most recent faults
 *      - Pre-fault information [ last P pre-faults detected ]
 *          - Pre-fault ID
 *          - Number of occurrences
 *          - Black box data at the time the pre-fault occurred
 *          - Cycle data for the most recent cycle
 *          - Optionally, more detailed data for the most recent pre-faults
 *
 * @copyright  Copyright 2015-2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "Statistics.h"
#if (((STATISTICS_SAVED_CYCLES > 0) && (STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED)) || \
     ((STATISTICS_SAVED_CYCLES > 0) && (STATISTICS_SAVED_CYCLE_REGULATIONS > 0)) || \
     (STATISTICS_SAVED_LAST_CYCLES > 0) || \
     (STATISTICS_SAVED_FAULT_BLACK_BOX > 0) || \
     (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0) || \
     (STATISTICS_CUSTOM_LIFETIME_ENABLE == ENABLED))
    #include "Statistics_prv.h"
#endif

// -- Other Modules --
#include "Estimator.h"
#include "Interpreter.h"
#include <limits.h>
#include "Log.h"
#include "Micro.h"
#include "Millis.h"
#include "NVData.h"
#if ((STATISTICS_SAVED_CYCLES > 0) && (STATISTICS_SAVED_CYCLE_REGULATIONS > 0))
    #include "Regulations.h"
#endif
#if (STATISTICS_SAVED_CYCLES > 0)
    #include "Selector.h"
#endif
#include <string.h>
#if (STATISTICS_SAVED_CYCLES > 0)
    #include "Supervisor.h"
#endif


//  --- Compile-Time Assertions ---------------------------------------------------------------------------------------


// If cycle statistics are saved, there must be at least one saved cycle for every appliance compartment.
CT_ASSERT((STATISTICS_SAVED_CYCLES == 0) || (STATISTICS_SAVED_CYCLES >= APPLIANCE_MAX_COMPARTMENTS));

// The number of saved last cycles must be less than or equal to the number of saved cycles.
CT_ASSERT((0 <= STATISTICS_SAVED_LAST_CYCLES) && (STATISTICS_SAVED_LAST_CYCLES <= STATISTICS_SAVED_CYCLES));

// The statistics data structure must store at least 5 faults.
CT_ASSERT(STATISTICS_SAVED_FAULTS >= 5);

// The number of saved fault black box data must be less than or equal to the number of saved faults.
CT_ASSERT((0 <= STATISTICS_SAVED_FAULT_BLACK_BOX) && (STATISTICS_SAVED_FAULT_BLACK_BOX <= STATISTICS_SAVED_FAULTS));

// The statistics data structure must store at least 5 pre-faults.
CT_ASSERT(STATISTICS_SAVED_PRE_FAULTS >= 5);

// The number of saved pre-fault black box data must be less than or equal to the number of saved pre-faults.
CT_ASSERT((0 <= STATISTICS_SAVED_PRE_FAULT_BLACK_BOX) && (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX <= STATISTICS_SAVED_PRE_FAULTS));

#if (STATISTICS_SAVED_CYCLE_REGULATIONS > 0)
    // The number of regulations saved must match the number of function IDs defined.
    CT_ASSERT(ELEMENTS_IN_ARRAY(STATISTICS_CYCLE_REGULATION_FUNCTION_IDS) == STATISTICS_SAVED_CYCLE_REGULATIONS);
#endif

// Set default configuration for how fault age should be measured.
#ifndef STATISTICS_FAULT_AGE_BY_CYCLES
    #if (STATISTICS_SAVED_CYCLES > 0)
        #define STATISTICS_FAULT_AGE_BY_CYCLES ENABLED
    #else
        #define STATISTICS_FAULT_AGE_BY_CYCLES DISABLED
    #endif
#endif

#ifndef STATISTICS_FAULT_MAX_AGE_DIFF
    #if (STATISTICS_FAULT_AGE_BY_CYCLES == ENABLED)
        // Default age cutoff is 10 cycles
        #define STATISTICS_FAULT_MAX_AGE_DIFF 10
    #else
        // Default age cutoff is 30 days
        #define STATISTICS_FAULT_MAX_AGE_DIFF 300
    #endif
#endif


//  --- Private Properties --------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The maximum value for a counter that counts the number of resets during a cycle.
#define MAX_CYCLE_RESETS 15

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_STATISTICS

//! Number of seconds in a day. Used for tracking appliance age on some appliances.
#define SECONDS_IN_ONE_TENTH_DAY 8640L


// -- Private Type Declarations --

//! A list of reference values used by several functions to manage either
//!     1) The Statistics.Fault array and the Statistics.Fault_Black_Box array, or
//!     2) The Statistics.Pre_Fault array and the Statistics.Pre_Fault_Black_Box array
typedef struct FAULT_REFERENCE_STRUCT
{
    STATISTICS_FAULT_TYPE* Faults;
    #if ((STATISTICS_SAVED_FAULT_BLACK_BOX > 0) || (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0))
        PLATFORM_BLACK_BOX_TYPE* Black_Box;
    #else
        void* Unused;
    #endif
    uint8 Fault_Count;
    uint8 Black_Box_Count;
} FAULT_REFERENCE_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_STATISTICS_LOG_MESSAGE_ID_ENUM
{
    REQUESTED_CYCLE_OUT_OF_RANGE = 1,
    REQUESTED_FAULT_OUT_OF_RANGE,
    REQUESTED_FAULT_BLACK_BOX_OUT_OF_RANGE,
    REQUESTED_LAST_CYCLE_OUT_OF_RANGE,
    REQUESTED_PRE_FAULT_OUT_OF_RANGE,
    REQUESTED_PRE_FAULT_BLACK_BOX_OUT_OF_RANGE,
    DATA_LOST_DUE_TO_CORRUPTION
} MODULE_STATISTICS_LOG_MESSAGE_ID_TYPE;


// -- Private Variable Definitions --

//! A time reference used by Statistics to know when a second has passed.
static uint16 Statistics_Milliseconds;

//! The number of seconds since the last warm or cold reset.
uint32 Statistics_Seconds_Since_Reset;

#if (STATISTICS_SAVED_CYCLES > 0)
    // Index of current running cycle storage Set to STATISTICS_SAVED_CYCLES if data does not indicate a running cycle
    static uint8 Cycles_Index[APPLIANCE_MAX_COMPARTMENTS];
#endif

#pragma location = "_statistics_segment"
STATISTICS_TYPE Statistics;


// -- Private Flash-Based Constant Definitions --

static const FAULT_REFERENCE_TYPE FAULT_REF =
{
    Statistics.Fault,
    #if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
        Statistics.Fault_Black_Box,
    #else
        NULL,
    #endif
    STATISTICS_SAVED_FAULTS,
    STATISTICS_SAVED_FAULT_BLACK_BOX
};

static const FAULT_REFERENCE_TYPE PRE_FAULT_REF =
{
    Statistics.Pre_Fault,
    #if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
        Statistics.Pre_Fault_Black_Box,
    #else
        NULL,
    #endif
    STATISTICS_SAVED_PRE_FAULTS,
    STATISTICS_SAVED_PRE_FAULT_BLACK_BOX
};


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static void SaveErrorData(FAULT_REFERENCE_TYPE ref, uint8 compartment, uint8 fault_id, uint8 code, uint8 sub_code, uint8 eng_code);
static void ShiftArray(void* array, void* swap_space, uint16 element_size, sint8 index);
static BOOL_TYPE SortFaultsArray(FAULT_REFERENCE_TYPE ref, uint8 code, uint8 sub_code, uint8 eng_code);
#if (STATISTICS_SAVED_CYCLES > 0)
    static void UpdateCustomCycleData(uint8 index, STATISTICS_CYCLE_STATE_TYPE state);
#endif


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the Statistics module variables.
 */
void Statistics__Initialize(void)
{
    // Restore the Cycles_Index[] array based on the Interpreter cycles.
    #if (STATISTICS_SAVED_CYCLES > 0)
        memset(Cycles_Index, STATISTICS_SAVED_CYCLES, sizeof(Cycles_Index));
        for (uint8 compartment = 0; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
        {
            CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(compartment);
            CYCLE_STATE_TYPE state = Selector__GetCycleState(0, compartment, cps.Cycle);
            if ((state == CYCLE_STATE_RUN) || (state == CYCLE_STATE_PAUSE))
            {
                #if (APPLIANCE_MAX_COMPARTMENTS > 1)
                    for (uint8 index = 0; index < STATISTICS_SAVED_CYCLES; index++)
                    {
                        if (Statistics.Cycle[index].Compartment == compartment)
                        {
                            Cycles_Index[compartment] = index;
                            break;                      // Break out of the inner for('index') loop.
                        }
                    }
                #else
                    Cycles_Index[0] = 0;
                #endif
            }
        }
    #endif

    MICRO_RESET_MODE_TYPE reset_mode = Micro__GetResetMode();
    if (reset_mode == MICRO_RESET_MODE_COLD)
    {
        if (NVData__GetLastRestoreStatus()->Bit.Statistics_Data_Recovered == NVDATA_RESTORE_CORRUPTED)
        {
            // Non-volatile memory was corrupted -- reset all statistics.
            memset(&Statistics, 0, sizeof(Statistics));
            LOG_ADD_EXCEPTION(DATA_LOST_DUE_TO_CORRUPTION, 0);
        }
        Statistics_Milliseconds = (uint16)Millis__GetFreeRunningCounter();
        if (Statistics.Lifetime.Cold_Reset_Count < USHRT_MAX)
        {
            Statistics.Lifetime.Cold_Reset_Count++;
        }
    }
    else
    {
        if (Statistics.Lifetime.Warm_Reset_Count < USHRT_MAX)
        {
            Statistics.Lifetime.Warm_Reset_Count++;
        }
    }
    Statistics_Seconds_Since_Reset = 0;

    // Count the cold/warm reset in any running cycle's data.
    #if (STATISTICS_SAVED_CYCLES > 0)
        for (uint8 compartment = 0; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
        {
            uint8 index = Cycles_Index[compartment];
            if (index < STATISTICS_SAVED_CYCLES)
            {
                if (reset_mode == MICRO_RESET_MODE_COLD)
                {
                    if (Statistics.Cycle[index].Cold_Reset_Count < MAX_CYCLE_RESETS)
                    {
                        Statistics.Cycle[index].Cold_Reset_Count++;
                    }
                }
                else
                {
                    if (Statistics.Cycle[index].Warm_Reset_Count < MAX_CYCLE_RESETS)
                    {
                        Statistics.Cycle[index].Warm_Reset_Count++;
                    }
                }
            }
        }
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Background function for the BlackBox module.
 *      Lifetime data updates (once per second):
 *          - Update the number of seconds that the board has ever been powered.
 *          - Update the number of seconds that the board has been powered since reset.
 *          - Update custom lifetime data (if appropriate).
 */
void Statistics__Background(void)
{
    // Keep track of seconds that the board has been powered.
    if ((uint16)(Millis__GetFreeRunningCounter() - Statistics_Milliseconds) >= 1000)
    {
        Statistics_Milliseconds += 1000;
        if (Statistics.Lifetime.Seconds < ULONG_MAX)
        {
            Statistics.Lifetime.Seconds++;
        }
        Statistics_Seconds_Since_Reset++;

        #if (STATISTICS_CUSTOM_LIFETIME_ENABLE == ENABLED)
        	UPDATE_CUSTOM_LIFETIME(&Statistics.Lifetime.Platform);
        #endif
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Apply the age cutoff to the faults.
 * @return The number of faults that should be displayed.
 */
uint8 Statistics__ApplyFaultAgeCutoff(void)
{
    uint8 fault_count = 0;

    // Determine the age cutoff for faults that are too old to display.
    uint16 age_cutoff;
    if (STATISTICS_FAULT_AGE_BY_CYCLES == ENABLED)
    {
        age_cutoff = Statistics.Lifetime.Cycles_Completed;
    }
    else
    {
        uint32 age = Statistics.Lifetime.Seconds / SECONDS_IN_ONE_TENTH_DAY;
        age_cutoff = ((age < USHRT_MAX) ? age : USHRT_MAX);
    }
    if (age_cutoff < STATISTICS_FAULT_MAX_AGE_DIFF - 1)
    {
        age_cutoff = 0;
    }
    else
    {
        age_cutoff -= STATISTICS_FAULT_MAX_AGE_DIFF - 1;
    }

    // Apply the age cutoff.
    BOOL_TYPE hide_remaining_faults = FALSE;
    for (uint8 fault_index = 0; fault_index < STATISTICS_SAVED_FAULTS; fault_index++)
    {
        STATISTICS_FAULT_TYPE* fault = &Statistics.Fault[fault_index];
        if ((hide_remaining_faults == TRUE) ||
            (fault->Fault_Id == 0) ||
            (fault->Hide_From_Hmi == TRUE) ||
            (fault->Age < age_cutoff))
        {
            // Hide the remaining faults.
            fault->Hide_From_Hmi = TRUE;
            hide_remaining_faults = TRUE;
        }
        else
        {
            // Count the fault.
            fault_count++;
        }
    }

    return fault_count;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Mark all faults as hidden from the HMI.
 */
void Statistics__ClearFaultData(void)
{
    for (uint8 fault_index = 0; fault_index < STATISTICS_SAVED_FAULTS; fault_index++)
    {
        Statistics.Fault[fault_index].Hide_From_Hmi = TRUE;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Record data related to the start of a new cycle.
 *      Lifetime data updates:
 *          - Increment the number of times that the specific cycle has started (if appropriate).
 *      Cycle data updates:
 *          - Create a new Cycle slot and fill with zeroes.
 *          - Set the Cycle to the selected cycle.
 *          - Set the estimated time (if appropriate).
 *          - Set the compartment (if there is more than one compartment).
 *          - Save the specified user-selected regulations (if appropriate).
 *          - Update custom cycle data (if appropriate).
 *          - Update custom last cycle data (if appropriate).
 * @param compartment = The compartment in which the cycle started.
 * @param cycle_id = The selector of the cycle that was started.
 */
void Statistics__CycleStarted(uint8 compartment, uint8 cycle_id)
{
	if (compartment < APPLIANCE_MAX_COMPARTMENTS)
	{
        #if (STATISTICS_SAVED_CYCLES > 0)
	        if (Statistics.Lifetime.Cycles_Started < USHRT_MAX)
	        {
	            Statistics.Lifetime.Cycles_Started++;
	        }
            #if (STATISTICS_NUMBER_OF_LIFETIME_CYCLES > 0)
                // Increment the cycle start counter for this cycle.
                if ((0 < cycle_id) && (cycle_id <= STATISTICS_NUMBER_OF_LIFETIME_CYCLES))
                {
                    if (Statistics.Lifetime.Cycle_Started_Count[cycle_id - 1] < USHRT_MAX)
                    {
                        Statistics.Lifetime.Cycle_Started_Count[cycle_id - 1]++;
                    }
                }
            #endif

            // Shift the saved cycles up one -- drop the oldest cycle statistics.
            ShiftArray(Statistics.Cycle, NULL, sizeof(STATISTICS_CYCLE_TYPE), STATISTICS_SAVED_CYCLES - 1);

            // Shift the saved last cycles up one -- drop the oldest last cycle statistics.
            #if (STATISTICS_SAVED_LAST_CYCLES > 0)
                ShiftArray(Statistics.Last_Cycle, NULL, sizeof(PLATFORM_LAST_CYCLE_TYPE), STATISTICS_SAVED_LAST_CYCLES - 1);
            #endif

            // Cycle indices must be incremented for each compartment since all the cycle statistics are shifting up.
            for (uint8 compartment = 0; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
            {
                if (Cycles_Index[compartment] < STATISTICS_SAVED_CYCLES)
                {
                    Cycles_Index[compartment]++;
                }
            }

            Cycles_Index[compartment] = 0;
            Statistics.Cycle[0].Cps.Cycle = cycle_id;
            #if (TIME_ESTIMATION_FEATURE == ENABLED)
                Statistics.Cycle[0].Estimated_Seconds = Estimator__GetRemainingSeconds(compartment);
            #endif
            #if (APPLIANCE_MAX_COMPARTMENTS > 1)
                Statistics.Cycle[0].Compartment = compartment;
            #endif
            #if (STATISTICS_SAVED_CYCLE_REGULATIONS > 0)
                for (uint8 index = 0; index < STATISTICS_SAVED_CYCLE_REGULATIONS; index++)
                {
                    uint8 pos = Regulations__GetPositionByFunctionID(STATISTICS_CYCLE_REGULATION_FUNCTION_IDS[index], compartment);
                    if (pos != REGULATION_INVALID)
                    {
                        Statistics.Cycle[0].Regulations[index] = Regulations__GetIndexValueByPosition(pos);
                    }
                }
            #endif

            // Call category-specific function to update cycle data at the start of the cycle.
            UpdateCustomCycleData(0, STATS_CYCLE_STARTED);
        #endif      // #if (STATISTICS_SAVED_CYCLES > 0)
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Record data related to the end of a new cycle.
 *      Lifetime data updates:
 *          - Increment the number of cycles completed (if appropriate).
 *          - Update the time spent running cycles.
 *      Cycle data updates:
 *          - Update the time spent in the cycle.
 *          - Update the Cycle, Phase, and Statement where the cycle ended.
 *          - Mark the cycle as completed (if appropriate).
 *          - Update custom cycle data (if appropriate).
 *          - Update custom last cycle data (if appropriate).
 *
 * @param cycle_completed_normally = TRUE if the cycle completed normally. FALSE if the cycle was cancelled.
 * @param compartment = The compartment in which the cycle ended.
 * @param cps = Cycle, Phase, and Statement where the cycle execution ended.
 * @param cycle_seconds = Number of seconds that the cycle was running.
 */
void Statistics__CycleStopped(BOOL_TYPE cycle_completed_normally, uint8 compartment, CYCLE_PHASE_STATEMENT_TYPE cps, uint16 cycle_seconds)
{
    if (cycle_completed_normally == TRUE)
    {
        if (Statistics.Lifetime.Cycles_Completed < USHRT_MAX)
        {
            Statistics.Lifetime.Cycles_Completed++;
        }
    }
    #if (STATISTICS_SAVED_CYCLES > 0)
        if (compartment < APPLIANCE_MAX_COMPARTMENTS)
        {
            uint8 index = Cycles_Index[compartment];
            if (index < STATISTICS_SAVED_CYCLES)
            {
                // Update the lifetime statistics.
                uint32 seconds = Statistics.Lifetime.Seconds_Running_Cycles + cycle_seconds;
                Statistics.Lifetime.Seconds_Running_Cycles = (seconds >= Statistics.Lifetime.Seconds_Running_Cycles) ? seconds : ULONG_MAX;

                // Update the running cycle statistics.
                STATISTICS_CYCLE_TYPE* cycle = &Statistics.Cycle[index];
                cycle->Run_Seconds = cycle_seconds;
                cycle->Cps = cps;
                cycle->Cycle_Completed = cycle_completed_normally;

                // Update custom cycle statistics.
                if (cycle_completed_normally == TRUE)
                {
                    UpdateCustomCycleData(index, STATS_CYCLE_ENDED);
                }
                else
                {
                    UpdateCustomCycleData(index, STATS_CYCLE_CANCELLED);
                }
            }

            // Remember that the cycle has completed.
            Cycles_Index[compartment] = STATISTICS_SAVED_CYCLES;
        }
    #endif
}


#if (STATISTICS_SAVED_CYCLES > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the data for one of the cycle entries.
 *
 * @param index = Which cycle data slot to get. (0 to STATISTICS_SAVED_CYCLES-1)
 *              0 = Most recently started cycle's data
 *              1 = Previously started cycle's data
 *              etc.
 *
 * @return A pointer to the PLATFORM_CYCLE_TYPE with the cycle data.
 */
STATISTICS_CYCLE_TYPE* Statistics__GetCycleData(uint8 index)
{
	STATISTICS_CYCLE_TYPE *cycle = NULL;
    if (index < STATISTICS_SAVED_CYCLES)
    {
        cycle = &Statistics.Cycle[index];
        #if (APPLIANCE_MAX_COMPARTMENTS > 1)
            uint8 compartment = cycle->Compartment;
        #else
            uint8 compartment = 0;
        #endif
        if (Cycles_Index[compartment] == index)
        {
            cycle->Run_Seconds = Supervisor__GetCycleTime(compartment);
            cycle->Cps = Interpreter__GetCurrentStatement(compartment);
            UpdateCustomCycleData(index, STATS_CYCLE_ACTIVE);
        }
    }
	else
	{
		LOG_ADD_EXCEPTION(REQUESTED_CYCLE_OUT_OF_RANGE, index);
	}
    return(cycle);
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of recorded faults in the FAULT data.
 * @return The number of recorded faults in the FAULT data.
 */
uint8 Statistics__GetFaultCount(void)
{
    uint8 count;
    for (count = 0; count < STATISTICS_SAVED_FAULTS; count++)
    {
        if ((Statistics.Fault[count].Fault_Id == 0) ||
            (Statistics.Fault[count].Hide_From_Hmi == TRUE))
        {
            break;                                  // Break out of the for() loop.
        }
    }
    return count;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the data for one of the fault entries.
 *
 * @param index = Which fault data slot to get. (0 to STATISTICS_SAVED_FAULTS-1)
 *
 * @return A pointer to the STATISTICS_FAULT_TYPE with the fault data.
 */
STATISTICS_FAULT_TYPE* Statistics__GetFaultData(uint8 index)
{
    if (index < STATISTICS_SAVED_FAULTS)
    {
        return (&Statistics.Fault[index]);
    }
    else
    {
        LOG_ADD_EXCEPTION(REQUESTED_FAULT_OUT_OF_RANGE, index);
        return (NULL);
    }
}


#if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the black box data for one of the fault entries.
 *
 * @param index = Which fault black box data slot to get. (0 to STATISTICS_SAVED_FAULT_BLACK_BOX-1)
 *
 * @return A pointer to the PLATFORM_BLACK_BOX_TYPE with the fault black box data.
 */
PLATFORM_BLACK_BOX_TYPE* Statistics__GetFaultBlackBoxData(uint8 index)
{
    if (index < STATISTICS_SAVED_FAULT_BLACK_BOX)
    {
        return (&Statistics.Fault_Black_Box[index]);
    }
    else
    {
        LOG_ADD_EXCEPTION(REQUESTED_FAULT_BLACK_BOX_OUT_OF_RANGE, index);
        return (NULL);
    }
}
#endif


#if (STATISTICS_SAVED_LAST_CYCLES > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the data for one of the last cycle entries.
 *
 * @param index = Which last cycle data slot to get. (0 to STATISTICS_SAVED_LAST_CYCLES-1)
 *
 * @return A pointer to the PLATFORM_LAST_CYCLE_TYPE with the last cycle data.
 */
PLATFORM_LAST_CYCLE_TYPE* Statistics__GetLastCycleData(uint8 index)
{
    PLATFORM_LAST_CYCLE_TYPE* last_cycle = NULL;
    if (index < STATISTICS_SAVED_LAST_CYCLES)
    {
        #if (APPLIANCE_MAX_COMPARTMENTS > 1)
            uint8 compartment = Statistics.Cycle[index].Compartment;
        #else
            uint8 compartment = 0;
        #endif
        if (Cycles_Index[compartment] == index)
        {
            UpdateCustomCycleData(index, STATS_CYCLE_ACTIVE);
        }
        last_cycle = &Statistics.Last_Cycle[index];
    }
    else
    {
        LOG_ADD_EXCEPTION(REQUESTED_LAST_CYCLE_OUT_OF_RANGE, index);
        return (NULL);
    }
    return(last_cycle);
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the appliance lifetime data.
 *
 * @return A pointer to the STATISTICS_LIFETIME_TYPE with the appliance lifetime data.
 */
STATISTICS_LIFETIME_TYPE* Statistics__GetLifetimeData(void)
{
    return (&Statistics.Lifetime);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the data for one of the pre-fault entries.
 *
 * @param index = Which pre-fault data slot to get. (0 to STATISTICS_SAVED_PRE_FAULTS-1)
 *
 * @return A pointer to the STATISTICS_FAULT_TYPE with the pre-fault data.
 */
STATISTICS_FAULT_TYPE* Statistics__GetPreFaultData(uint8 index)
{
    if (index < STATISTICS_SAVED_PRE_FAULTS)
    {
        return (&Statistics.Pre_Fault[index]);
    }
    else
    {
        LOG_ADD_EXCEPTION(REQUESTED_PRE_FAULT_OUT_OF_RANGE, index);
        return (NULL);
    }
}


#if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the black box data for one of the pre-fault entries.
 *
 * @param index = Which pre-fault black box data slots to get. (0 to STATISTICS_SAVED_PRE_FAULT_BLACK_BOX-1)
 *
 * @return A pointer to the PLATFORM_BLACK_BOX_TYPE with the pre-fault black box data.
 */
PLATFORM_BLACK_BOX_TYPE* Statistics__GetPreFaultBlackBoxData(uint8 index)
{
    if (index < STATISTICS_SAVED_PRE_FAULT_BLACK_BOX)
    {
        return (&Statistics.Pre_Fault_Black_Box[index]);
    }
    else
    {
        LOG_ADD_EXCEPTION(REQUESTED_PRE_FAULT_BLACK_BOX_OUT_OF_RANGE, index);
        return (NULL);
    }
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 * Save fault data for the new fault to the statistics data.
 *
 * If the fault ID is already recorded, the error data is updated.
 * If a fault slot is empty, it is used.
 * Otherwise, the fault replaces the oldest fault data.
 *
 * @param compartment = The compartment associated with the fault data
 * @param fault_id = The Fault ID for the generated fault.
 * @param code = The fault code for the generated fault.
 * @param sub_code = The error code for the generated fault.
 * @param eng_code = The engineering code for the generated fault.
 */
void Statistics__SaveFaultData(uint8 compartment, uint8 fault_id, uint8 code, uint8 sub_code, uint8 eng_code)
{
    SaveErrorData(FAULT_REF, compartment, fault_id, code, sub_code, eng_code);
    #if (STATISTICS_SAVED_CYCLES > 0)
        // Save the fault code in the running cycle's statistics data (if any).
        uint8 index = Cycles_Index[compartment];
        if (index < STATISTICS_SAVED_CYCLES)
        {
            STATISTICS_CYCLE_TYPE* cycle = &Statistics.Cycle[index];
            cycle->Last_Fault_Code = code;
            cycle->Last_Fault_Sub_Code = sub_code;
            cycle->Last_Fault_Eng_Code = eng_code;
        }
    #endif

    #if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
        // Update the category-specific black box data related to this fault.
        UPDATE_CUSTOM_FAULT(Statistics.Fault_Black_Box, fault_id);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Save pre-fault data for the new pre-fault to the statistics data.
 *
 * If the pre-fault ID is already recorded, the error data is updated.
 * If a pre-fault slot is empty, it is used.
 * Otherwise, the pre-fault replaces the oldest pre-fault data.
 *
 * @param compartment = The compartment associated with the fault data
 * @param fault_id = The Fault ID for the generated fault.
 * @param code = The fault code for the generated fault.
 * @param sub_code = The error code for the generated fault.
 * @param eng_code = The engineering code for the generated fault.
 */
void Statistics__SavePreFaultData(uint8 compartment, uint8 fault_id, uint8 code, uint8 sub_code, uint8 eng_code)
{
    SaveErrorData(PRE_FAULT_REF, compartment, fault_id, code, sub_code, eng_code);
    #if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
        // Update the category-specific black box data related to this pre-fault.
        UPDATE_CUSTOM_PRE_FAULT(Statistics.Pre_Fault_Black_Box, fault_id);
    #endif
}


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Save the fault data in the Statistics data structure.
 *
 * @param ref = Reference information for fault and related black box data.
 * @param compartment = The compartment associated with the fault data
 * @param fault_id = The Fault ID for the generated fault.
 * @param code = The fault code for the generated fault.
 * @param sub_code = The error code for the generated fault.
 * @param eng_code = The engineering code for the generated fault.
 */
static void SaveErrorData(FAULT_REFERENCE_TYPE ref, uint8 compartment, uint8 fault_id, uint8 code, uint8 sub_code, uint8 eng_code)
{
    BOOL_TYPE already_in_history = SortFaultsArray(ref, code, sub_code, eng_code);

    // Set the fault ID and update the occurrences.
    if (already_in_history == TRUE)
    {
        if (ref.Faults->Occurrences < UCHAR_MAX)
        {
            ref.Faults->Occurrences++;
        }
    }
    else
    {
        ref.Faults->Fault_Code = code;
        ref.Faults->Fault_Sub_Code = sub_code;
        ref.Faults->Fault_Eng_Code = eng_code;
        ref.Faults->Unused = 0;
        ref.Faults->Occurrences = 1;
    }
    ref.Faults->Fault_Id = fault_id;
    ref.Faults->Hide_From_Hmi = FALSE;

    if (STATISTICS_FAULT_AGE_BY_CYCLES == ENABLED)
    {
        // Set the age of the fault based on the current number of completed cycles.
        ref.Faults->Age = Statistics.Lifetime.Cycles_Completed;
    }
    else
    {
        // Set the age of the fault based on the amount of time that the appliance has run.
        // Supports a maximum age of approximately 18 years.
        uint32 age = Statistics.Lifetime.Seconds / SECONDS_IN_ONE_TENTH_DAY;
        ref.Faults->Age = (age < USHRT_MAX) ? age : USHRT_MAX;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Shift an element in an array to position 0.
 * @param array = Pointer to array data.
 * @param swap_space = Pointer to additional memory to use as swap space for an element.
 *                     NULL indicates that the new element 0 should be erased.
 * @param element_size = The size of the swap_space and of each element in the array.
 * @param index = The index of the element to move to position 0.
 */
static void ShiftArray(void* array, void* swap_space, uint16 element_size, sint8 index)
{
    uint8* buffer = array;
    if (index > 0)
    {
        uint16 offset = index * element_size;
        if (swap_space != NULL)
        {
            memcpy(swap_space, &buffer[offset], element_size);
        }
        memmove(&buffer[element_size], buffer, offset);
        if (swap_space != NULL)
        {
            memcpy(buffer, swap_space, element_size);
        }
    }
    if (swap_space == NULL)
    {
        memset(buffer, 0, element_size);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine which slot of the statistics array must be updated or replaced with the new fault's data.
 * Sort the statistics array such that the newly detected fault is in the first position.
 * @param fault_statistics = Pointer to an array of saved data for faults or pre-faults.
 * @param fault_id = The FAULT_ID_TYPE of the newly detected fault or pre-fault.
 *
 * @param fault_codes = The codes for the generated fault.
 * @param is_pre_fault = TRUE for pre-faults, FALSE for faults.
 *
 * @return TRUE if the fault already existed; otherwise FALSE.
 */
static BOOL_TYPE SortFaultsArray(FAULT_REFERENCE_TYPE ref, uint8 code, uint8 sub_code, uint8 eng_code)
{
    // Search the faults (or pre-faults) array for a matching and/or unused slot.
    uint8 fault_index = ref.Fault_Count - 1;
    BOOL_TYPE fault_exists = FALSE;
    for (uint8 index = 0; index < ref.Fault_Count; index++)
    {
        if ((ref.Faults[index].Fault_Code == code) &&
            (ref.Faults[index].Fault_Sub_Code == sub_code) &&
            (ref.Faults[index].Fault_Eng_Code == eng_code))
        {
            fault_index = index;
            fault_exists = TRUE;
            break;                                  // Break out of the for() loop.
        }
    }

    // Select which slot in the statistics array should be updated.
    union SWAP_UNION
    {
        STATISTICS_FAULT_TYPE Swap_Fault;
        #if ((STATISTICS_SAVED_FAULT_BLACK_BOX > 0) || (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0))
            PLATFORM_BLACK_BOX_TYPE Swap_Black_Box;
        #endif
    } swap_space;
    void* no_swap_space = NULL;
    ShiftArray(ref.Faults, fault_exists ? &swap_space : no_swap_space, sizeof(STATISTICS_FAULT_TYPE), fault_index);

    #if ((STATISTICS_SAVED_FAULT_BLACK_BOX > 0) || (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0))
        // Shift the corresponding black-box data.
        if (ref.Black_Box_Count > 0)
        {
            uint8 black_box_index = (fault_index < ref.Black_Box_Count) ? fault_index : ref.Black_Box_Count - 1;
            BOOL_TYPE black_box_exists = (BOOL_TYPE)(fault_exists && (black_box_index >= 0));
            ShiftArray(ref.Black_Box, black_box_exists ? &swap_space : no_swap_space, sizeof(PLATFORM_BLACK_BOX_TYPE), black_box_index);
        }
    #endif

    return fault_exists;
}


#if (STATISTICS_SAVED_CYCLES > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the part of the cycle data that is custom for the category.
 * @param index = Position of the data in the array.
 * @param state = State of the cycle when the update is being made.
 */
static void UpdateCustomCycleData(uint8 index, STATISTICS_CYCLE_STATE_TYPE state)
{
    #if ((STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED) || (STATISTICS_SAVED_LAST_CYCLES > 0))
        #if (APPLIANCE_MAX_COMPARTMENTS > 1)
            uint8 compartment = Statistics.Cycle[index].Compartment;
        #else
            uint8 compartment = 0;
        #endif

        #if (STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED)
            PLATFORM_CYCLE_TYPE* custom_cycle = &Statistics.Cycle[index].Platform;
        #else
            void* custom_cycle = NULL;
        #endif

        #if (STATISTICS_SAVED_LAST_CYCLES > 0)
            PLATFORM_LAST_CYCLE_TYPE* last_cycle = NULL;
            if (index < STATISTICS_SAVED_LAST_CYCLES)
            {
                last_cycle = &Statistics.Last_Cycle[index];
            }
        #else
            void* last_cycle = NULL;
        #endif

        UPDATE_CUSTOM_CYCLE(compartment, state, custom_cycle, last_cycle);
    #endif
}
#endif

