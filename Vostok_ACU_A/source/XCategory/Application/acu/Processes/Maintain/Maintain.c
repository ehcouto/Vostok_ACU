/**
 * @file
 * @brief       A cross-category process for measuring time in a cycle.
 *
 * @details
 * The Maintain Process measures elapsed time as the number of seconds that the current step has
 * been running. It compares the elapsed time to the delay time specified in the parameters.
 *
 * If the process parameters include a modifier, then the delay time is the time with the modifier
 * applied.
 *
 * If the process parameters include a condition, then the countdown only runs while the condition
 * is TRUE.
 *
 * If the ADD_UNUSED_TIME option of Maintain is used, then any time that was not used up in the
 * previous Maintain statement is added to the time for the current Maintain statement.

    @startuml{Maintain_sm.jpg}
        title Maintain State Machine
        ' Use state = Maintain.State

        [*] --> Initialize

        Initialize --> Paused : evCountDownDisabled
        Initialize --> CountDown : evCountDownEnabled
        Initialize --> Complete : evTimeout

        Paused : do / WaitForEnable()
        Paused -u-> Initialize : evInitialize
        Paused --> CountDown : evCountDownEnabled\n/ DecrementCounter()

        CountDown : entry / CheckForTimeout()
        CountDown : do / CheckIfDisabled()\n\t DecrementCounter()\n\t CheckForTimeout()
        CountDown -u-> Initialize : evInitialize
        CountDown -u-> Paused : evCountDownDisabled
        CountDown -r-> Complete : evTimeout

        Complete -u-> Initialize : evInitialize
    @enduml

 * @copyright  Copyright 2014-2019 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "Maintain.h"

// -- Other Modules --
#include "Condition.h"
#include <limits.h>
#include "Log.h"
#include "Micro.h"
#include "Modifier.h"
#include "NVData.h"
#include "Selector.h"
#include "SettingFile.h"
#include <string.h>
#include "Variable.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Maintain State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Maintain state machine.
typedef enum MAINTAIN_STATE_ENUM
{
    STATE_MAINTAIN_NONE       = 0,

    STATE_MAINTAIN_INITIALIZE = 1,                  //!< (initial_state)
    STATE_MAINTAIN_PAUSED     = 2,
    STATE_MAINTAIN_COUNT_DOWN = 3,
    STATE_MAINTAIN_COMPLETE   = 4,

    STATE_MAINTAIN_END        = 5
} MAINTAIN_STATE_TYPE;

//! The transition variable for the Maintain state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Maintain_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForTimeout(void);
static void CheckIfDisabled(void);
static void DecrementCounter(void);
static void WaitForEnable(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventCountDownDisabled(void);
static void EventCountDownEnabled(void);
static void EventInitialize(void);
static void EventTimeout(void);
static void MaintainExecute(void);
static void MaintainInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------

// -- Private Constant Declarations --

//! The number of times that the Maintain__Process function will be called every second while running.
#define MAINTAIN_CALLS_PER_SECOND 10

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_MAINTAIN

// -- Private Enumerated Constants Declarations --

//! The format of the Index field in the Maintain parameter structure.
typedef enum INDEX_FORMAT_ENUM
{
    INDEX_IS_MODIFIER           = 0,
    INDEX_IS_CONDITION          = 1,
    INDEX_IS_MODIFIER_EXTENDED  = 2,    // modifier displacement will be 2 bytes; condition may or may not be present
    INDICES_ALL_PRESENT         = 3
} INDEX_FORMAT_TYPE;

//! Index used to access the Timer[] and State[] arrays.
typedef enum MAINTAIN_CYCLE_ENUM
{
    MAINTAIN_MAIN_CYCLE = 0,
    MAINTAIN_SUB_CYCLE  = 1,
    MAINTAIN_PAUSE_CYCLE= 2
} MAINTAIN_CYCLE_TYPE;

//! List of values to compare with the F1_F0_Flags.
typedef enum MAINTAIN_OPTION_ENUM
{
    MAINTAIN_OPTION_DEFAULT,                        //!< F1=0, F0=0
    MAINTAIN_OPTION_ADD_UNUSED,                     //!< F1=0, F0=1
    MAINTAIN_OPTION_COUNT
} MAINTAIN_OPTION_TYPE;

//! Prescalars for the time in the Maintain process.
typedef enum MAINTAIN_PRESCALAR_ENUM
{
    PS_1_SECOND         = 0,
    PS_10_SECOND        = 1,
    PS_1_MINUTE         = 2,
    PS_10_MINUTE        = 3
} MAINTAIN_PRESCALAR_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_MAINTAIN_LOG_MESSAGE_ID_ENUM
{
    INVALID_PROCESS_CONTROL = 1,
    INVALID_PROCESS_PARAMETERS,
    INVALID_NON_VOLATILE_VALUES_RECOVERED
} MODULE_MAINTAIN_LOG_MESSAGE_ID_TYPE;


// -- Private Type Declarations --

//! A structure to hold volatile data for the Maintain process.
typedef struct MAINTAIN_DATA_STRUCT
{
    //! The conditions associated with each of the compartments.
    uint8 Condition[APPLIANCE_MAX_COMPARTMENTS];

    //! The compartment associated with the currently running Maintain routines.
    uint8 Compartment;

    //! Index for Timer[] and State[] arrays.
    MAINTAIN_CYCLE_TYPE Cycle_Type;

    //! The state associated with the currently running Maintain state machine.
    MAINTAIN_STATE_TYPE State;
} MAINTAIN_DATA_TYPE;

//! A structure to hold nonvolatile data for the Maintain process.
typedef struct MAINTAIN_NV_DATA_STRUCT
{
    //! Maintain timers for each of the possible cycles (Main, Sub-Cycle, Pause).
    uint32 Timer[3];

    //! Maintain states for each of the possible cycles (Main, Sub-Cycle, Pause).
    MAINTAIN_STATE_TYPE State[3];
} MAINTAIN_NV_DATA_TYPE;

//! A structure of the Maintain process parameters as they are found in the setting file.
typedef PACKED struct MAINTAIN_SF_DATA_STRUCT
{
    uint16 Time                     : 12;           //!< Time (before prescalar) for the Maintain process.
    uint16 Prescalar                : 2;            //!< Prescalar that converts Time to seconds.
    uint16 Unused1                  : 2;
    INDEX_FORMAT_TYPE Indices;                      //!< Specifies contents of Displacement and Condition_Displacement.
    uint8 Displacement;                             //!< Value of modifier displacement or only displacement.
    union
    {
        uint16 Extended_Modifier_Displacement;      //!< Value of displacement for modifier when it is 2 bytes
        struct
        {
            uint8 Condition_Displacement;           //!< Value of condition displacement when modifier with one byte displacement is also present
            uint8 Unused;
        };
    };

} MAINTAIN_SF_DATA_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

//! Multipliers to convert the time to seconds using the prescalar.
static const uint16 Multiplier[] =
{
    1,                                              //!< Convert from 1-second counts to seconds.
    10,                                             //!< Convert from 10-second counts to seconds.
    60,                                             //!< Convert from 1-minute counts to seconds.
    600                                             //!< Convert from 10-minute counts to seconds.
};


// -- Private Variable Definitions --

#pragma location = "_appliance_segment"
//! The non-volatile data required by the Maintain module.
static MAINTAIN_NV_DATA_TYPE Maintain_Nv[APPLIANCE_MAX_COMPARTMENTS];

//! The volatile data required by the Maintain module.
static MAINTAIN_DATA_TYPE Maintain;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static void GetMaintainSfParameters(PROCESS_CONTROL_TYPE* control);


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the Maintain modules non-volatile data.
 * Called after reset.
 */
void Maintain__Initialize(void)
{
    if ((Micro__GetResetMode() == MICRO_RESET_MODE_COLD) &&
        (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY))
    {
        memset(Maintain_Nv, 0, sizeof(Maintain_Nv));
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Inform the Maintain module that a cycle has paused.
 * Allows the Maintain module to support the use of Maintain in a pause macro.
 * @param compartment = The compartment in which the cycle was paused.
 */
void Maintain__PauseEntry(uint8 compartment)
{
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Maintain process.
 *
 * The Maintain Process measures elapsed time as the number of seconds that the current step has
 * been running. It compares the elapsed time to the delay time specified in the parameters.
 *
 * @param control = The control parameters provided by the Interpreter.
 * @retval PROCESS_COMPLETE = The process is ready for the Interpreter to proceed to the next step.
 * @retval PROCESS_BUSY = The process is not ready for the Interpreter to proceed to the next step.
 */
PROCESS_RESULT_TYPE Maintain__Process(PROCESS_CONTROL_TYPE* control)
{
    PROCESS_RESULT_TYPE result = PROCESS_COMPLETE;
    if ((control == NULL) ||
        (control->F1_F0_Flags >= (uint8)MAINTAIN_OPTION_COUNT) ||
        (control->Compartment >= APPLIANCE_MAX_COMPARTMENTS) ||
        (control->Process_Instance > 0))
    {
        LOG_ADD_EXCEPTION(INVALID_PROCESS_CONTROL, (control != NULL));
    }
    else
    {
        // Setup the cycle type based on the PROCESS_CONTROL_TYPE and the current cycle.
        Maintain.Compartment = control->Compartment;
        sint32 cycle_id;
        CYCLE_STATE_TYPE cycle_state;
        if (Variable__GetValue(VARIABLE_CYCLE, Maintain.Compartment, &cycle_id) == TRUE)
        {
            cycle_state = Selector__GetCycleState(0, Maintain.Compartment, (uint8)cycle_id);
        }
        else
        {
            cycle_state = CYCLE_STATE_RUN;
        }
        if (cycle_state == CYCLE_STATE_RUN)
        {
            if (control->Sub_Cycle_Running == TRUE)
            {
                Maintain.Cycle_Type = MAINTAIN_SUB_CYCLE;
            }
            else
            {
                Maintain.Cycle_Type = MAINTAIN_MAIN_CYCLE;
            }
        }
        else
        {
            Maintain.Cycle_Type = MAINTAIN_PAUSE_CYCLE;
        }

        // Setup variables for the current compartment.
        Maintain.State = Maintain_Nv[Maintain.Compartment].State[Maintain.Cycle_Type];
        if (Maintain.State == STATE_MAINTAIN_NONE)
        {
            // The Maintain Process state machine has never been initialized -- initialize it.
            MaintainInitialize();
        }

        // If this is a new step, then raise the initialize event.
        if (control->Step_Initializing == TRUE)
        {
            EventInitialize();
            GetMaintainSfParameters(control);
        }
        else
        {
            // Restore setting file parameters if necessary.
            if ((control->Resume_From_Pause == TRUE) ||
                (control->Resume_From_Reset == TRUE))
            {
                GetMaintainSfParameters(control);
            }
            // Execute the Maintain Process state machine.
            MaintainExecute();
        }

        // Save changes before returning.
        Maintain_Nv[Maintain.Compartment].State[Maintain.Cycle_Type] = Maintain.State;
        result = ((Maintain.State == STATE_MAINTAIN_COMPLETE) ? PROCESS_COMPLETE : PROCESS_BUSY);
    }

    return result;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the Maintain process parameters from the setting file and setup the Maintain process.
 * Determine the delay seconds and setup the condition displacement based on the referenced
 * setting file parameters. If a modifier exists, it is applied to the delay seconds.
 * @param control = Pointer to the process control parameters from the Interpreter.
 */
static void GetMaintainSfParameters(PROCESS_CONTROL_TYPE* control)
{
    // Get the Maintain process parameters from the setting file.
    SETTINGFILE_LOADER_TYPE loader;
    if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
                                         SF_PTR_ACU_PROCESS_MAINTAIN,
                                         control->Data_Field_Index,
                                         &loader) == FAIL)
    {
        LOG_ADD_EXCEPTION(INVALID_PROCESS_PARAMETERS, control->Data_Field_Index);
        EventTimeout();
    }
    else
    {
        // Map the setting file data to a structure for convenience.
        MAINTAIN_SF_DATA_TYPE* parameter = (MAINTAIN_SF_DATA_TYPE*)(void*)loader.Data;

        // Check for modifier and/or condition features.
        uint16 modifier_displacement = SF_DISPL_INVALID_VALUE;
        uint8 condition_displacement = SF_DISPL_INVALID_VALUE;
        if (loader.Length > 2)
        {
            switch (parameter->Indices)
            {
                case INDEX_IS_MODIFIER:
                    modifier_displacement = parameter->Displacement;
                    break;
                case INDEX_IS_CONDITION:
                    condition_displacement = parameter->Displacement;
                    break;
                case INDEX_IS_MODIFIER_EXTENDED:
                    modifier_displacement = parameter->Extended_Modifier_Displacement;
                    condition_displacement = parameter->Displacement;
                    break;
                case INDICES_ALL_PRESENT:
                    modifier_displacement = parameter->Displacement;
                    condition_displacement = parameter->Condition_Displacement;
                    break;
                default:
                    break;
            }
        }
        Maintain.Condition[Maintain.Compartment] = condition_displacement;

        if (control->Step_Initializing == TRUE)
        {
            // Get the maintain time in seconds.
            sint32 seconds = parameter->Time * Multiplier[parameter->Prescalar];

            // Apply the modifier feature (if present).
            if (modifier_displacement != SF_DISPL_INVALID_VALUE)
            {
                if (parameter->Indices == INDEX_IS_MODIFIER_EXTENDED)
                {
                    seconds = (sint32)Modifier__ModifyValue(seconds, SF_PTR_ACU_MODIFIERS_EXTENDED, modifier_displacement);
                }
                else
                {
                    seconds = (sint32)Modifier__ModifyValue(seconds, SF_PTR_ACU_MODIFIERS, modifier_displacement);
                }
            }

            // Set the Calls_Remaining based on the current time and any previously loaned time.
            sint32 timer = MAINTAIN_CALLS_PER_SECOND * seconds;
            if ((MAINTAIN_OPTION_TYPE)(control->F1_F0_Flags) == MAINTAIN_OPTION_ADD_UNUSED)
            {
                timer += Maintain_Nv[Maintain.Compartment].Timer[Maintain.Cycle_Type];
            }
            if (timer < 0)
            {
                timer = 0;
            }
            Maintain_Nv[Maintain.Compartment].Timer[Maintain.Cycle_Type] = timer;

            // Raise the appropriate event for the next state.
            if ((condition_displacement == SF_DISPL_INVALID_VALUE) ||
                (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, condition_displacement) != CONDITION_RESULT_FALSE))
            {
                EventCountDownEnabled();                // The count down is enabled.
            }
            else
            {
                EventCountDownDisabled();               // The count down is disabled.
            }
        }
    }
}


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if there is any time remaining.
 *
 * Generate the evTimeout event if no time is remaining.
 */
static void CheckForTimeout(void)
{
    if (Maintain_Nv[Maintain.Compartment].Timer[Maintain.Cycle_Type] == 0)
    {
        EventTimeout();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if a condition is specified and it has become FALSE.
 *
 * Generate the evCountDownDisabled event if the condition is specified and FALSE.
 */
static void CheckIfDisabled(void)
{
    // Check if the count down is still enabled.
    uint8 condition_displacement = Maintain.Condition[Maintain.Compartment];
    if ((condition_displacement != SF_DISPL_INVALID_VALUE) &&
        (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, condition_displacement) == CONDITION_RESULT_FALSE))
    {
        EventCountDownDisabled();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Decrement the countdown timer.
 */
static void DecrementCounter(void)
{
    uint32 calls = Maintain_Nv[Maintain.Compartment].Timer[Maintain.Cycle_Type];
    if (calls > 0)
    {
        calls--;
        Maintain_Nv[Maintain.Compartment].Timer[Maintain.Cycle_Type] = calls;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the maintain timer to be re-enabled.
 */
static void WaitForEnable(void)
{
    if (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, Maintain.Condition[Maintain.Compartment]) != CONDITION_RESULT_FALSE)
    {
        EventCountDownEnabled();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Maintain State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Count Down Disabled Event for the Maintain state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCountDownDisabled(void)
{
    switch (Maintain.State)
    {
        case STATE_MAINTAIN_INITIALIZE:
        case STATE_MAINTAIN_COUNT_DOWN:
            Maintain.State = STATE_MAINTAIN_PAUSED;
            Maintain_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Maintain.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Count Down Enabled Event for the Maintain state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCountDownEnabled(void)
{
    switch (Maintain.State)
    {
        case STATE_MAINTAIN_INITIALIZE:
            Maintain.State = STATE_MAINTAIN_COUNT_DOWN;
            CheckForTimeout();                      // Entry Function.
            Maintain_Transitioned = TRUE;
            break;

        case STATE_MAINTAIN_PAUSED:
            DecrementCounter();                     // Transition action function.
            Maintain.State = STATE_MAINTAIN_COUNT_DOWN;
            CheckForTimeout();                      // Entry Function.
            Maintain_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Maintain.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Initialize Event for the Maintain state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventInitialize(void)
{
    switch (Maintain.State)
    {
        case STATE_MAINTAIN_PAUSED:
        case STATE_MAINTAIN_COUNT_DOWN:
        case STATE_MAINTAIN_COMPLETE:
            Maintain.State = STATE_MAINTAIN_INITIALIZE;
            Maintain_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Maintain.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timeout Event for the Maintain state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimeout(void)
{
    switch (Maintain.State)
    {
        case STATE_MAINTAIN_INITIALIZE:
        case STATE_MAINTAIN_COUNT_DOWN:
            Maintain.State = STATE_MAINTAIN_COMPLETE;
            Maintain_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Maintain.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Maintain state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void MaintainExecute(void)
{
    Maintain_Transitioned = FALSE;
    switch(Maintain.State)
    {
        case STATE_MAINTAIN_PAUSED:
            WaitForEnable();                        // Do function.
            break;

        case STATE_MAINTAIN_COUNT_DOWN:
            CheckIfDisabled();                      // Do function.
            if (Maintain_Transitioned == FALSE)
            {
                DecrementCounter();                 // Do function.
                if (Maintain_Transitioned == FALSE)
                {
                    CheckForTimeout();              // Do function.
                }
            }
            break;

        default:
            // There is nothing to do for all other values of Maintain.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Maintain state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void MaintainInitialize(void)
{
    Maintain.State = STATE_MAINTAIN_INITIALIZE;
    Maintain_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.
