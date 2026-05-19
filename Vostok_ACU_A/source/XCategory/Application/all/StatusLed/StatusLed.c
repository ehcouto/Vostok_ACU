/**
 *  @file       
 *
 *  @brief      Source file for the StatusLed module.
 *
 *  @details    Handles blinking the status led to indicate the health of the micro.
 *              The Fast Flash profile will always run for a defined amount of time at micro power up.
 *              Afterwards, the system inputs are used to determine the profile that will be used to drive the led.
 *              STATUS_LED_FEATURE                      - ENABLED to enable StatusLed feature
 *              STATUS_LED_VIRTUAL_PIN_INDEX            - StatusLed Pin index from VirtualPin table, starting from index 0 (instead of 1)
 *                                                        Mandatory if StatusLed feature is ENABLED
 *              STATUS_LED_ACTIVE_LOW                   - if ENABLED, Led will set Low to turn it ON. Default is DISABLED
 *              STATUS_LED_VIRTUAL_PIN_INDEX_INVALID()  - Macro to define function to call if Virtual Pin index is not valid
 *              STATUS_LED_RESUME_TIME_MS               - Time (in ms) to automatically resume Led blink after a StatusLed__Suspend()
 *                                                          Default is 0 - Disabled; Led will be resumed when StatusLed__Resume() is called
 *                                                          Maximum allowed is 2550.
 *              STATUS_LED_DEFAULT_COMM_NODE            - Overrides default node to check for Communication Failure event
 *                                                          Default values if not defined:
 *                                                          - WIN_UI_ADDRESS for ACU P3 and HMI Expansions only
 *                                                          - WIN_ACU_ADDRESS for MCU or ACU Expansions only
 *                                                          - Feature Disabled (REVEAL_BROADCAST_NODE, 15) for all other nodes
 *
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "StatusLed.h"

#if (STATUS_LED_FEATURE == ENABLED)

#include "StatusLed_prv.h"

#include "Gpio.h"
#include "VirtualPin.h"
#include "Log.h"
#include "Mode.h"
#include "Reveal.h"
#include "SettingFile.h"
#include <string.h>


//====================================== Module State Machine =========================================================
/*
    @startuml{StatusLed_state_machine.jpg}
        title StatusLed State Machine
        ' Suppress transition checks

        [*] --> INITIALIZE

        INITIALIZE : entry /\t InitializeModule()
        INITIALIZE : do / RunInitialize()
        INITIALIZE --> NEXT_PROFILE_STEP            : evEnable

        PROFILE_LED_OFF : entry /\t SetProfileOffTime()
        PROFILE_LED_OFF : do /\t DecrementMicroPowerUpTimer() \n\t DecrementLedTimer()
        PROFILE_LED_OFF -->NEXT_PROFILE_STEP        : evLedTimerExpired

        PROFILE_LED_ON : entry /\t SetProfileOnTime()
        PROFILE_LED_ON : do /\t DecrementMicroPowerUpTimer() \n\t DecrementLedTimer()
        PROFILE_LED_ON -->PROFILE_LED_OFF           : evLedTimerExpired

        NEXT_PROFILE_STEP : entry /\t IncrementProfileLedStep()
        NEXT_PROFILE_STEP --> PROFILE_LED_ON        : evProfileFinished \n / DefineProfileToRun()
        NEXT_PROFILE_STEP --> PROFILE_LED_ON        : evMoreStepsInProfile

    @enduml
 */
//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Status Led State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Status Led state machine.
typedef enum STATUS_LED_STATE_ENUM
{
    STATE_STATUS_LED_NONE              = 0,

    STATE_STATUS_LED_INITIALIZE        = 1,         //!< (initial_state)
    STATE_STATUS_LED_NEXT_PROFILE_STEP = 2,
    STATE_STATUS_LED_PROFILE_LED_OFF   = 3,
    STATE_STATUS_LED_PROFILE_LED_ON    = 4,

    STATE_STATUS_LED_END               = 5
} STATUS_LED_STATE_TYPE;

//! The state variable for the Status Led state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static STATUS_LED_STATE_TYPE Status_Led_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void DecrementLedTimer(void);
static void DecrementMicroPowerUpTimer(void);
static void DefineProfileToRun(void);
static void IncrementProfileLedStep(void);
static void InitializeModule(void);
static void RunInitialize(void);
static void SetProfileOffTime(void);
static void SetProfileOnTime(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventEnable(void);
static void EventLedTimerExpired(void);
static void EventMoreStepsInProfile(void);
static void EventProfileFinished(void);
static void StatusLedExecute(void);
static void StatusLedInitialize(void);
static void StatusLedTerminate(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

// post compiler message to user notifying them the STATUS_LED_VIRTUAL_PIN_INDEX macro must be defined.
#ifndef STATUS_LED_VIRTUAL_PIN_INDEX
    #error "STATUS_LED_VIRTUAL_PIN_INDEX in StatusLed_prv.h must be defined."
#endif

// Function to call when Status Led Pin index is not valid
#ifndef STATUS_LED_VIRTUAL_PIN_INDEX_INVALID
    #define STATUS_LED_VIRTUAL_PIN_INDEX_INVALID()
#endif

// Default behaviour for StatusLed is to not resume after StatusLed__Suspend() call
#ifndef STATUS_LED_RESUME_TIME_MS
    #define STATUS_LED_RESUME_TIME_MS           0
#endif

#if(STATUS_LED_RESUME_TIME_MS > 2550)
#error  "STATUS_LED_RESUME_TIME_MS cannot be higher than 2550"
#endif

// Communication fault Led notification is disabled if COMMUNICATION_FAULT_PRESENT() is not defined
#ifndef COMMUNICATION_FAULT_PRESENT
    #define COMMUNICATION_FAULT_PRESENT()       FALSE
#endif

// Flag on ACU Setting File to use Solid ON profile instead of Slow Blink
#define STATUS_LED_SETTING_FILE_KEEP_SOLID_ON				0x01

// Default behaviour for StatusLed is to set pin High to turn Led ON
#ifndef STATUS_LED_ACTIVE_LOW
    #define STATUS_LED_ACTIVE_LOW               DISABLED
#endif

// Reveal broadcast node - duplicate only to limit dependency from Reveal.h
#define STATUS_LED_BROADCAST_NODE						15

// Check if there is a Node to check for Communication Failure profile
// Default is:
// - UI node (4) for ACU P3 and HMI Expansion
// - ACU node (1) for HMI, MCU or ACU Expansions only
// - Disabled (REVEAL_BROADCAST_NODE) for all other nodes
#ifndef STATUS_LED_DEFAULT_COMM_NODE
    #if((SYSTEM_ACU_HANDLING == ENABLED) && (SYSTEM_HMI_HANDLING == DISABLED))
        #define STATUS_LED_DEFAULT_COMM_NODE            WIN_UI_ADDRESS
    #elif((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
        #define STATUS_LED_DEFAULT_COMM_NODE            WIN_UI_ADDRESS
    #elif((SYSTEM_HMI_HANDLING == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
        #define STATUS_LED_DEFAULT_COMM_NODE            WIN_ACU_ADDRESS
    #elif((SYSTEM_MCU_HANDLING == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
        #define STATUS_LED_DEFAULT_COMM_NODE            WIN_ACU_ADDRESS
    #elif((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) && (SYSTEM_HMI_HANDLING == DISABLED))
        #define STATUS_LED_DEFAULT_COMM_NODE            WIN_ACU_ADDRESS
    #else
        #define STATUS_LED_DEFAULT_COMM_NODE            STATUS_LED_BROADCAST_NODE
    #endif
#endif

#if (STATUS_LED_DEFAULT_COMM_NODE > STATUS_LED_BROADCAST_NODE)
	#error "STATUS_LED_DEFAULT_COMM_NODE must be in range [0..15]"
#endif

#if (STATUS_LED_DEFAULT_COMM_NODE < STATUS_LED_BROADCAST_NODE)
	#include "Reveal.h"
#endif

//! list of module Log messages
typedef enum
{
    INVALID_VIRTUAL_PIN_INDEX = 1,
    INVALID_PROFILE
} MODULE_STATUSLED_LOG_MESSAGE_ID_TYPE;
#define MODULE_NAME MODULE_STATUSLED


/*
 *  Definition of status led module time
 *  Range: 0 to 255
 *  Resolution: 1 = 10 millisecond
 */
typedef uint8 STATUS_LED_TIMER_TYPE;


// a step is composed of an off and on time
typedef struct
{
    STATUS_LED_TIMER_TYPE   On_Time;
    STATUS_LED_TIMER_TYPE   Off_Time;
} STATUS_LED_STEP_TYPE;

// the total time the led will perform the power up behavior
// resolution is the same as STATUS_LED_TIMER_TYPE
#define STATUS_LED_MICRO_POWER_UP_TIME     500   // 5 seconds

#define STATUS_LED_MAX_NUM_OF_STEPS_IN_PROFILE 3

// structure defining the elements in a StatusLed Profile
typedef struct
{
    uint8                   Num_Of_Steps_In_Profile;                        // the number of steps in the profile, range : 1 to 255, resolution: 1 = 1 step
    STATUS_LED_STEP_TYPE    Led[STATUS_LED_MAX_NUM_OF_STEPS_IN_PROFILE];    // a profile is composed of multiple steps
} STATUS_LED_PROFILE_TABLE_TYPE;


// list of status led profiles that can be run
typedef enum
{
    STATUS_LED_PROFILE_OFF = 0,
    STATUS_LED_PROFILE_SLOW_FLASH,
    STATUS_LED_PROFILE_FAST_FLASH,
    STATUS_LED_PROFILE_DOUBLE_BLINK,
    STATUS_LED_PROFILE_ON,
    STATUS_LED_PROFILE_TRIPLE_BLINK,
    NUM_OF_STATUS_LED_PROFILES
} STATUS_LED_PROFILE_TYPE;

// define the led behavior for each profile defined in STATUS_LED_PROFILE_TYPE.
static const STATUS_LED_PROFILE_TABLE_TYPE STATUS_LED_PROFILE_TABLE[] =
{
                    //                                       Step 0          Step 1          Step 3
                    //  Num_Of_Steps_In_Profil             On     Off       On     Off        On    Off
    /* STATUS_LED_PROFILE_OFF               */   { 1    ,{{ 0    ,100 }   ,{ 0    ,0   }   ,{ 0    ,0   }}  }
    /* STATUS_LED_PROFILE_SLOW_FLASH        */  ,{ 1    ,{{ 50   ,50  }   ,{ 0    ,0   }   ,{ 0    ,0   }}  }
    /* STATUS_LED_PROFILE_FAST_FLASH        */  ,{ 1    ,{{ 10   ,10  }   ,{ 0    ,0   }   ,{ 0    ,0   }}  }
    /* STATUS_LED_PROFILE_DOUBLE_BLINK      */  ,{ 2    ,{{ 10   ,10  }   ,{ 10   ,170 }   ,{ 0    ,0   }}  }
    /* STATUS_LED_PROFILE_ON                */  ,{ 1    ,{{ 100  ,0   }   ,{ 0    ,0   }   ,{ 0    ,0   }}  }
    /* STATUS_LED_PROFILE_TRIPLE_FAST_BLINK */  ,{ 3    ,{{ 20   ,20  }   ,{ 20   ,20  }   ,{ 20   ,200 }}  }
};
CT_ASSERT(ELEMENTS_IN_ARRAY(STATUS_LED_PROFILE_TABLE) == NUM_OF_STATUS_LED_PROFILES);


//!< module static variables
static struct
{
    struct
    {
        STATUS_LED_TIMER_TYPE   Led;            // Holds the remaining time to perform the LED profile behavior
        uint16                  Micro_Power_Up; // Holds the remaining time to perform the LED behavior after the micro resets
    } Timer;                                    // Intentionally not using STATUS_LED_TIMER_TYPE to avoid increasing size of STATUS_LED_PROFILE_TABLE
    struct
    {
        STATUS_LED_PROFILE_TYPE Index:7;        // Index into the table that executes the steps of a profile
        BOOL_TYPE               Off_Override;	// Set profile to OFF
        uint8                   Step;           // 0 based index
    } Profile;
} StatusLed;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE IsMicroPowerUpTimerExpired(void);
static void SetLed(ON_OFF_TYPE value);

#endif // STATUS_LED_FEATURE
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It initializes the module StatusLed.
 *
 */
void StatusLed__Initialize(void)
{
    #if (STATUS_LED_FEATURE == ENABLED)
        memset(&StatusLed, 0, sizeof(StatusLed));
        if ( STATUS_LED_VIRTUAL_PIN_INDEX > VirtualPin__GetNumOf() )
        {
            LOG_ADD_EXCEPTION(INVALID_VIRTUAL_PIN_INDEX, STATUS_LED_VIRTUAL_PIN_INDEX);
            STATUS_LED_VIRTUAL_PIN_INDEX_INVALID();
            StatusLedTerminate();
        }
        else
        {
            StatusLed.Timer.Micro_Power_Up = STATUS_LED_MICRO_POWER_UP_TIME;
            StatusLedInitialize();
        }
    #endif // STATUS_LED_FEATURE
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It executes the module StatusLed.  Module is designed to be executed at a 10 millisecond call rate.
 *
 */
void StatusLed__Execute(void)
{
    #if (STATUS_LED_FEATURE == ENABLED)
        StatusLedExecute();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It's called to stop/suspend  execution of StatusLed.
 *
 */
void StatusLed__Suspend(void)
{
    #if (STATUS_LED_FEATURE == ENABLED)
        if(Status_Led_State != STATE_STATUS_LED_NONE)
        {
            // Override current execution forcing Led OFF
            SetLed(OFF);
            #if(STATUS_LED_RESUME_TIME_MS > 0)
                // Resume execution after STATUS_LED_RESUME_TIME_MS time, roundup to 10ms
                StatusLed.Timer.Led = (STATUS_LED_RESUME_TIME_MS+9)/10;
                StatusLed.Timer.Micro_Power_Up = 0;
            #else
                // Stop Status Led execution at all
                StatusLedTerminate();
            #endif
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It's called to resume execution of StatusLed, if Resume Time is not 0.
 *
 */
void StatusLed__Resume(void)
{
    #if ((STATUS_LED_FEATURE == ENABLED) && (STATUS_LED_RESUME_TIME_MS == 0))
        StatusLedInitialize();
        StatusLed.Timer.Micro_Power_Up = 0;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function can be called to set OFF Profile by an external monitor module
 *  			to expose a Board failure condition
 *
 */
void StatusLed__GoToOffProfile(BOOL_TYPE go_to_off)
{
	#if (STATUS_LED_FEATURE == ENABLED)
        StatusLed.Profile.Off_Override = go_to_off;
	#endif
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (STATUS_LED_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Write the value to the status led
 *
 *  @param  ON = led on; OFF = led off
 */
static void SetLed(ON_OFF_TYPE value)
{
    // Update the resulting pin state based on active high/low output
    BOOL_TYPE pin_state = (BOOL_TYPE)(value ^ STATUS_LED_ACTIVE_LOW);
    VIRTUALPIN_TYPE *pin_ptr = (void *)&VIRTUALPIN_TABLE[STATUS_LED_VIRTUAL_PIN_INDEX];

    Gpio__PinWrite(pin_ptr->port, pin_ptr->pin, pin_state);
    Gpio__PinConfig(pin_ptr->port, pin_ptr->pin, pin_ptr->pin_config);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  State machine guard function to check if the power up LED behavior is done
 */
static BOOL_TYPE IsMicroPowerUpTimerExpired(void)
{
    return ( (StatusLed.Timer.Micro_Power_Up == 0) ? TRUE : FALSE );
}


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  State machine do function to decrement the timer while an LED profile is running (separate from power up time)
 */
static void DecrementLedTimer(void)
{
	if(StatusLed.Timer.Led > 0)
	{
	    StatusLed.Timer.Led--;
	    if ( StatusLed.Timer.Led == 0 )
	    {
	        EventLedTimerExpired();
	    }
	    // else StatusLed.Timer.Led has not expired, do nothing
	}
    // else StatusLed.Timer.Led was already 0, no events

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  State machine do function to decrement the timer while the power up behavior is occuring
 */
static void DecrementMicroPowerUpTimer(void)
{
    StatusLed.Timer.Micro_Power_Up = (StatusLed.Timer.Micro_Power_Up > 0) ? StatusLed.Timer.Micro_Power_Up - 1 : 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Sets the next profile based on various conditions of the board
 */
static void DefineProfileToRun(void)
{
	MODE_TYPE current_mode = Mode__GetMode();
	STATUS_LED_PROFILE_TYPE new_index = STATUS_LED_PROFILE_FAST_FLASH;

	if(SettingFile__IsValid() == FALSE)
	{
		new_index = STATUS_LED_PROFILE_ON;
	}
	else if((current_mode == MODE_NORMAL) &&
    	(Mode__GetSubMode() == SUBMODE_NORMAL_ERROR))
    {
    	new_index = STATUS_LED_PROFILE_TRIPLE_BLINK;
    }
    else if((IsMicroPowerUpTimerExpired() != FALSE) &&
	    (current_mode != MODE_PROGRAMMING))
	{
		BOOL_TYPE keep_solid_on = FALSE;
		uint8 comm_node = STATUS_LED_DEFAULT_COMM_NODE;

		#if(SYSTEM_ACU_HANDLING == ENABLED)
            SETTINGFILE_LOADER_TYPE sf_ptr;
            
            if(SettingFile__BasicLoader(SF_PTR_ACU_CROSS_PRODUCT_CONFIG, SF_DISPL_FVT_SETTING_FILE, &sf_ptr) == PASS)
            {
                keep_solid_on = (BOOL_TYPE) ((sf_ptr.Data[0] & STATUS_LED_SETTING_FILE_KEEP_SOLID_ON) != 0);
            }
            #if(SYSTEM_HMI_HANDLING == DISABLED)
                // Setting file parameter shall be used on ACU only boards (P3)
                if(SettingFile__BasicLoader(SF_PTR_ACU_CROSS_PRODUCT_CONFIG, SF_DISPL_MAIN_HMI_NODE, &sf_ptr) == PASS)
                {
                    comm_node = sf_ptr.Data[1];
                }
            #endif
        #endif

        new_index = STATUS_LED_PROFILE_SLOW_FLASH;

        if (StatusLed.Profile.Off_Override == TRUE)
        {
            new_index = STATUS_LED_PROFILE_OFF;
        }
        #if((SYSTEM_ACU_HANDLING == ENABLED) || (STATUS_LED_DEFAULT_COMM_NODE < STATUS_LED_BROADCAST_NODE))
            else if((comm_node != STATUS_LED_BROADCAST_NODE) &&
                ((Reveal__GetNodestatus() & (1 << comm_node)) == 0))
            {
                new_index = STATUS_LED_PROFILE_DOUBLE_BLINK;
            }
        #endif
        else if(keep_solid_on == TRUE)
        {
            new_index = STATUS_LED_PROFILE_ON;
        }

    }

    StatusLed.Profile.Index = new_index;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  State machine entry function to move the LED profile to the next step or go back to the first step.
 */
static void IncrementProfileLedStep(void)
{
	if(StatusLed.Profile.Index < NUM_OF_STATUS_LED_PROFILES)
	{
	    // increment the Led Step.  Set the Led Step to zero if all steps in the profile have run
	    StatusLed.Profile.Step++;
	    if ( StatusLed.Profile.Step >= STATUS_LED_PROFILE_TABLE[StatusLed.Profile.Index].Num_Of_Steps_In_Profile )
	    {
	        StatusLed.Profile.Step = 0;
	    }
	}
	else
	{
        StatusLed.Profile.Step = 0;
	}

    if ( StatusLed.Profile.Step == 0 )
    {
        // all steps in the profile have run
        EventProfileFinished();
    }
    else // StatusLed.Profile.Step > 0
    {
        // there are more steps in the profile
        EventMoreStepsInProfile();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  State machine do function to begin the status LED behavior
 */
static void InitializeModule(void)
{
	StatusLed.Profile.Index = NUM_OF_STATUS_LED_PROFILES;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Sets the off time for the step of the profile to execute
 */
static void SetProfileOffTime(void)
{
    StatusLed.Timer.Led = STATUS_LED_PROFILE_TABLE[StatusLed.Profile.Index].Led[StatusLed.Profile.Step].Off_Time;
    if ( StatusLed.Timer.Led == 0 )
    {
        EventLedTimerExpired();
    }
    else
    {
    	SetLed(OFF);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Sets the on time for the step of the profile to execute
 */
static void SetProfileOnTime(void)
{
    StatusLed.Timer.Led = STATUS_LED_PROFILE_TABLE[StatusLed.Profile.Index].Led[StatusLed.Profile.Step].On_Time;
    if ( StatusLed.Timer.Led == 0 )
    {
        EventLedTimerExpired();
    }
    else
    {
    	SetLed(ON);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Start module sending Enable Event to start the LED profile
 */
static void RunInitialize(void)
{
	EventEnable();
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Status Led State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Enable Event for the Status Led state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventEnable(void)
{
    switch (Status_Led_State)
    {
        case STATE_STATUS_LED_INITIALIZE:
            Status_Led_State = STATE_STATUS_LED_NEXT_PROFILE_STEP;
            IncrementProfileLedStep();              // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Status_Led_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Led Timer Expired Event for the Status Led state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventLedTimerExpired(void)
{
    switch (Status_Led_State)
    {
        case STATE_STATUS_LED_PROFILE_LED_OFF:
            Status_Led_State = STATE_STATUS_LED_NEXT_PROFILE_STEP;
            IncrementProfileLedStep();              // Entry Function.
            break;

        case STATE_STATUS_LED_PROFILE_LED_ON:
            Status_Led_State = STATE_STATUS_LED_PROFILE_LED_OFF;
            SetProfileOffTime();                    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Status_Led_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the More Steps In Profile Event for the Status Led state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventMoreStepsInProfile(void)
{
    switch (Status_Led_State)
    {
        case STATE_STATUS_LED_NEXT_PROFILE_STEP:
            Status_Led_State = STATE_STATUS_LED_PROFILE_LED_ON;
            SetProfileOnTime();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Status_Led_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Profile Finished Event for the Status Led state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventProfileFinished(void)
{
    switch (Status_Led_State)
    {
        case STATE_STATUS_LED_NEXT_PROFILE_STEP:
            DefineProfileToRun();                   // Transition action function.
            Status_Led_State = STATE_STATUS_LED_PROFILE_LED_ON;
            SetProfileOnTime();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Status_Led_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Status Led state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void StatusLedExecute(void)
{
    switch(Status_Led_State)
    {
        case STATE_STATUS_LED_INITIALIZE:
            RunInitialize();                        // Do function.
            break;

        case STATE_STATUS_LED_PROFILE_LED_OFF:
        case STATE_STATUS_LED_PROFILE_LED_ON:
            DecrementMicroPowerUpTimer();           // Do function.
            DecrementLedTimer();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of Status_Led_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Status Led state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void StatusLedInitialize(void)
{
    Status_Led_State = STATE_STATUS_LED_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The termination function for the Status Led state machine.
 * This function disables the state machine. Calls to StateMachineExecute() will have no effect.
 * The StateMachineInitialize() function must be called again to restart the state machine.
 * If the current state has an exit function, it will be called.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void StatusLedTerminate(void)
{
    // None of the states have an Exit function.
    Status_Led_State = STATE_STATUS_LED_NONE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif // STATUS_LED_FEATURE
