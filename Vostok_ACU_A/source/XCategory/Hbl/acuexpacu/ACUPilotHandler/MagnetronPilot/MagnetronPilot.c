/**
 *  @file
 *
 *  @brief      Implementation for the Magnetron Pilot.
 *
 *  @details    Detailed description of the file contents
 *
     @startuml{MagnetronPilot_SM.png}
        title MagnetronPilot State Machine

        ' Use State = Magnetron_Pilot_Data[Pilot_Index].State
        [*] --> OFF

        ' OFF State - request pilot off on entry and wait for event in do
            OFF: do / OffDo()
            OFF --> ON: evOn
            OFF --> TURBO: evTurbo

        ' ON STATE - request pilot on on entry and wait for event in do
            ON: do / OnDo()
            ON --> OFF: evOff
            ON --> TURBO: evTurbo

        ' TURBO STATE - request pilot turbo on entry and wait for event in do
            TURBO: do / TurboDo()
            TURBO --> OFF: evOff
            TURBO --> ON: evOn

    @enduml
 *
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------

//lint -e766 Suppress Info: header not used in module 'VirtualGI.c'
#include "SystemConfig.h"

#if (HBL_PILOT_NUM_MAGNETRON > 0)
#include "MagnetronPilot.h"
#include "MagnetronPilotMainsSampler.h"
#include "MagnetronPilotRelaySync.h"
#include "Hbl.h"
#include "Log.h"
#include "Gpio.h"

#if (SYSTEM_ACU_EXPANSION_HANDLING  == ENABLED)
    #include "API012Exp.h"
#endif

#include <string.h>

//  --- Private Declarations ------------------------------------------------------------------------------------------

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Magnetron Pilot State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Magnetron Pilot state machine.
typedef enum MAGNETRON_PILOT_STATE_ENUM
{
    STATE_MAGNETRON_PILOT_NONE  = 0,

    STATE_MAGNETRON_PILOT_OFF   = 1,                //!< (initial_state)
    STATE_MAGNETRON_PILOT_ON    = 2,
    STATE_MAGNETRON_PILOT_TURBO = 3,

    STATE_MAGNETRON_PILOT_END   = 4
} MAGNETRON_PILOT_STATE_TYPE;

// -- Private Function Prototypes used by the state machine for customization functions --
static void OffDo(void);
static void OnDo(void);
static void TurboDo(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventOff(void);
static void EventOn(void);
static void EventTurbo(void);
static void MagnetronPilotExecute(void);
static void MagnetronPilotInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#define MAGNETRON_PILOT_PARAMETER_STRUCTURE_VERSION     0

//! A structure with all the setting file parameters that are used by the module.
typedef PACKED struct MAGNETRON_PILOT_PARAMETERS_STRUCT
{
    uint8   MW_FB_Active_High    : 1;   //!< Flag to indicate MW feedback signal active high or low: True: Active High / False: Active Low
    uint8   AC_Mains_Same_Phase  : 1;   //!< Flag to indicate AC and mains phase: True: Same Phase / False: Reversed Phaser
    uint8   Turbo_Enable         : 1;   //!< Flag to indicate Turbo enable or disable: True: Enable / False: Disable
    uint8   Unused               : 1;   //!< Reserved for future flag if needed.
    uint8   Structure_Version    : 4;   //!< A version byte that describes the data structure.
    uint8   Turbo_On_Off_Delay;         //!< Delay time between switching microwave and turbo relays.
    uint8   Mains_Feedback_Pin;         //!< Index into virtual pin table to find the Mains feedback signal.
    uint8   MW_Feedback_Pin;            //!< Index into virtual pin table to find the MW feedback signal.
    uint8   Door_GI_Index;              //!< Index of Door GI interacted with Magnetron Pilot.
} MAGNETRON_PILOT_PARAMETERS_TYPE;

//! Magnetron pilot data type declaration.
typedef PACKED struct MAGNETRON_PILOT_DATA_STRUCT
{
    HBL_LOAD_TYPE *Load;
    MAGNETRON_PILOT_REQUEST_TYPE Requested;
    MAGNETRON_PILOT_REQUEST_TYPE Processed;
    MAGNETRON_PILOT_STATE_TYPE State;
    MAGNETRON_PILOT_PARAMETERS_TYPE *Parameters;    //!< Pointer to parameters from HBL
    uint8 Pilot_Enable;                             //!< Flag to indicate Pilot enable or disable.
} MAGNETRON_PILOT_DATA_TYPE;

//  --- Private Definitions -------------------------------------------------------------------------------------------
//! Tracks the number of magnetron pilot allocations.
static uint8 Magnetron_Pilot_Allocation;

//! Pilot index for current running magnetron state machine.
static uint8 Pilot_Index;

//! Magnetron pilot data array. One entry per magnetron instance.
static MAGNETRON_PILOT_DATA_TYPE Magnetron_Pilot_Data[HBL_PILOT_NUM_MAGNETRON];

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_MAGNETRON_PILOT

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_MAGNETRON_PILOT_LOG_MESSAGE_ID_ENUM
{
    INVALID_REQUEST_VALUE                               = 1,
    INVALID_PILOT_PARAMETER_VERSION                     = 2,
    REQUEST_REJECT_DUE_TO_TURBO_FEATURE_DISABLE         = 3,
    NOT_ALLOW_TO_CHANGE_STATE_FROM_ON_TO_TURBO          = 4,
    DOOR_GI_IS_NOT_CONFIGURED_CORRECTLY                 = 5,
    INVALID_PILOT_PARAMETER_LENGTH                      = 6,
} MODULE_MAGNETRON_PILOT_LOG_MESSAGE_ID_TYPE;

//  --- Private Function Prototypes -----------------------------------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MagnetronPilot and its variables
 */
void MagnetronPilot__Initialize(void)
{
    Magnetron_Pilot_Allocation = 0;
    Pilot_Index = 0;
    memset(Magnetron_Pilot_Data, 0, sizeof(Magnetron_Pilot_Data));

    MagnetronPilotMainsSampler__Initialize();
    MagnetronPilotRelaySync__Initialize();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for magnetron.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 MagnetronPilot__Allocate(void)
{
    uint8 result = 0xFF;

    if (Magnetron_Pilot_Allocation < HBL_PILOT_NUM_MAGNETRON)
    {
        result = Magnetron_Pilot_Allocation;
        Magnetron_Pilot_Allocation++;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize magnetron pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated magnetron load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of magnetron pilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE MagnetronPilot__InitializeInstance(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load->Pilot_Data_Index < Magnetron_Pilot_Allocation)
    {
        Pilot_Index = load->Pilot_Data_Index;
        Magnetron_Pilot_Data[Pilot_Index].Load = load;
        Magnetron_Pilot_Data[Pilot_Index].Parameters = (void *)load->Parameter_Ptr.Data;

        Magnetron_Pilot_Data[Pilot_Index].Pilot_Enable = FALSE;

        //! Pilot parameter version validation
        if(Magnetron_Pilot_Data[Pilot_Index].Parameters->Structure_Version == MAGNETRON_PILOT_PARAMETER_STRUCTURE_VERSION)
        {
            //! Pilot parameter length validation
            if (load->Parameter_Ptr.Length >= 6)
            {
                HBL_GENERIC_INPUT_ENUM_TYPE door_id = Hbl__GetGIIDByIndex(Magnetron_Pilot_Data[Pilot_Index].Parameters->Door_GI_Index);

                if (door_id == HBL_GI_DOOR_1 || door_id == HBL_GI_DOOR_2)
                {
                    //! Initialize Mains Sampler
                    MagnetronPilotMainsSampler__InitializeInstance(Pilot_Index,
                                                                   Magnetron_Pilot_Data[Pilot_Index].Parameters->AC_Mains_Same_Phase,
                                                                   Magnetron_Pilot_Data[Pilot_Index].Parameters->Mains_Feedback_Pin);

                    //! Initialize Relay Sync
                    MagnetronPilotRelaySync__InitializeInstance(Pilot_Index,
                                                                Magnetron_Pilot_Data[Pilot_Index].Parameters->MW_Feedback_Pin,
                                                                Magnetron_Pilot_Data[Pilot_Index].Parameters->MW_FB_Active_High,
                                                                Magnetron_Pilot_Data[Pilot_Index].Load->Pin_Ptr[0],
                                                                Magnetron_Pilot_Data[Pilot_Index].Parameters->Turbo_Enable,
                                                                Magnetron_Pilot_Data[Pilot_Index].Load->Pin_Ptr[1],
                                                                Magnetron_Pilot_Data[Pilot_Index].Parameters->Turbo_On_Off_Delay,
                                                                Magnetron_Pilot_Data[Pilot_Index].Parameters->Door_GI_Index);

                    //! Enable Pilot
                    Magnetron_Pilot_Data[Pilot_Index].Pilot_Enable = TRUE;

                    //! Initialize Magnetron Pilot state machine
                    MagnetronPilotInitialize();

                    result = PASS;
                }
                else
                {
                    LOG_ADD_EXCEPTION(DOOR_GI_IS_NOT_CONFIGURED_CORRECTLY, door_id);
                }
            }
            else
            {
                LOG_ADD_EXCEPTION(INVALID_PILOT_PARAMETER_LENGTH, Pilot_Index);
            }
        }
        else
        {
            LOG_ADD_EXCEPTION(INVALID_PILOT_PARAMETER_VERSION, Pilot_Index);
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set magnetron pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated magnetron load instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as MAGNETRON_PILOT_REQUEST_TYPE *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of magnetron pilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE MagnetronPilot__Request(HBL_LOAD_TYPE *load, void *data)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load->Pilot_Data_Index < Magnetron_Pilot_Allocation)
    {
        Magnetron_Pilot_Data[load->Pilot_Data_Index].Requested = *((MAGNETRON_PILOT_REQUEST_TYPE *)data);

        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process magnetron pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to magnetron load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of magnetron pilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE MagnetronPilot__Process(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load->Pilot_Data_Index < Magnetron_Pilot_Allocation)
    {
        Pilot_Index = load->Pilot_Data_Index;

        if (Magnetron_Pilot_Data[Pilot_Index].Pilot_Enable == TRUE)
        {
            if (Magnetron_Pilot_Data[Pilot_Index].Processed != Magnetron_Pilot_Data[Pilot_Index].Requested)
            {
                if (Magnetron_Pilot_Data[Pilot_Index].Requested > MAGNETRON_PILOT_REQUEST_TURBO)
                {
                    LOG_ADD_EXCEPTION(INVALID_REQUEST_VALUE, Pilot_Index);
                }
                else
                {
                    if(Magnetron_Pilot_Data[Pilot_Index].Parameters->Turbo_Enable == TRUE)
                    {
                        if(Magnetron_Pilot_Data[Pilot_Index].Requested == MAGNETRON_PILOT_REQUEST_TURBO && Magnetron_Pilot_Data[Pilot_Index].State == STATE_MAGNETRON_PILOT_ON)
                        {
                            LOG_ADD_EXCEPTION(NOT_ALLOW_TO_CHANGE_STATE_FROM_ON_TO_TURBO, Pilot_Index);
                        }
                        else
                        {
                            Magnetron_Pilot_Data[Pilot_Index].Processed = Magnetron_Pilot_Data[Pilot_Index].Requested;
                        }
                    }
                    else
                    {
                        if(Magnetron_Pilot_Data[Pilot_Index].Requested == MAGNETRON_PILOT_REQUEST_TURBO)
                        {
                            LOG_ADD_EXCEPTION(REQUEST_REJECT_DUE_TO_TURBO_FEATURE_DISABLE, Pilot_Index);
                        }
                        else
                        {
                            Magnetron_Pilot_Data[Pilot_Index].Processed = Magnetron_Pilot_Data[Pilot_Index].Requested;
                        }
                    }
                }
            }

            MagnetronPilotExecute();

            result = PASS;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current magnetron pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated magnetron load instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((MAGNETRON_PILOT_REQUEST_TYPE *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * MagnetronPilot__GetRequested(HBL_LOAD_TYPE *load)
{
    void *requested = NULL;

    if (load->Load_Data_Index < Magnetron_Pilot_Allocation)
    {
        requested = &(Magnetron_Pilot_Data[load->Pilot_Data_Index].Requested);
    }

    return (requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get magnetron pilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated magnetron load instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((MAGNETRON_PILOT_REQUEST_TYPE *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * MagnetronPilot__GetProcessed(HBL_LOAD_TYPE *load)
{
    void *processed = NULL;

    if (load->Pilot_Data_Index < Magnetron_Pilot_Allocation)
    {
        processed = &(Magnetron_Pilot_Data[load->Pilot_Data_Index].Processed);
    }

    return (processed);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get magnetron pilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to magnetron load instance.
 *
 *  @return     void *status - pointer to status of pilot instance. Cast it using *((MAGNETRON_PILOT_REQUEST_TYPE *)ptr) to get value.
 *  @retval     COMPLETE - instance request is complete
 *  @retval     INCOMPLETE - instance request in process
 */
void * MagnetronPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status;

    status = &(Magnetron_Pilot_Data[load->Pilot_Data_Index].Processed);

    return (status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handler called at every 200us.
 */
void MagnetronPilot__200usHandler(void)
{
    //! Save Pilot_Index before entering interrupt handler
    uint8 Pilot_Index_Bak = Pilot_Index;

    for(Pilot_Index = 0; Pilot_Index < Magnetron_Pilot_Allocation; Pilot_Index++)
    {
        if(Magnetron_Pilot_Data[Pilot_Index].Pilot_Enable == TRUE)
        {
            MagnetronPilotMainsSampler__200usHandler(Pilot_Index);
            MagnetronPilotRelaySync__200usHandler(Pilot_Index);
        }
    }

    //! Restore Pilot_Index before exiting interrupt handler
    Pilot_Index = Pilot_Index_Bak;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Handler called at every 5ms.
 * @param      load
 * @return
 */
//---------------------------------------------------------------------------------------------------------------------
PASS_FAIL_TYPE MagnetronPilot__AsynchHandler(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < Magnetron_Pilot_Allocation)
    {
        Pilot_Index = load->Pilot_Data_Index;

        if(Magnetron_Pilot_Data[Pilot_Index].Pilot_Enable == TRUE)
        {
            MagnetronPilotMainsSampler__AsynchHandler(Pilot_Index);
            MagnetronPilotRelaySync__AsynchHandler(Pilot_Index, Magnetron_Pilot_Data[Pilot_Index].Processed);

            retval = PASS;
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OffDo(void)
{
    switch (Magnetron_Pilot_Data[Pilot_Index].Processed)
    {
        case MAGNETRON_PILOT_REQUEST_ON:
            EventOn();
            break;

        case MAGNETRON_PILOT_REQUEST_TURBO:
            EventTurbo();
            break;

        default:
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OnDo(void)
{
    switch (Magnetron_Pilot_Data[Pilot_Index].Processed)
    {
        case MAGNETRON_PILOT_REQUEST_OFF:
            EventOff();
            break;

        case MAGNETRON_PILOT_REQUEST_TURBO:
            EventTurbo();
            break;

        default:
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void TurboDo(void)
{
    switch (Magnetron_Pilot_Data[Pilot_Index].Processed)
    {
        case MAGNETRON_PILOT_REQUEST_OFF:
            EventOff();
            break;

        case MAGNETRON_PILOT_REQUEST_ON:
            EventOn();
            break;

        default:
            break;
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Magnetron Pilot State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Off Event for the Magnetron Pilot state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOff(void)
{
    switch (Magnetron_Pilot_Data[Pilot_Index].State)
    {
        case STATE_MAGNETRON_PILOT_ON:
        case STATE_MAGNETRON_PILOT_TURBO:
            Magnetron_Pilot_Data[Pilot_Index].State = STATE_MAGNETRON_PILOT_OFF;
            break;

        default:
            // Event is ignored for all other values of Magnetron_Pilot_Data[Pilot_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the On Event for the Magnetron Pilot state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOn(void)
{
    switch (Magnetron_Pilot_Data[Pilot_Index].State)
    {
        case STATE_MAGNETRON_PILOT_OFF:
        case STATE_MAGNETRON_PILOT_TURBO:
            Magnetron_Pilot_Data[Pilot_Index].State = STATE_MAGNETRON_PILOT_ON;
            break;

        default:
            // Event is ignored for all other values of Magnetron_Pilot_Data[Pilot_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Turbo Event for the Magnetron Pilot state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTurbo(void)
{
    switch (Magnetron_Pilot_Data[Pilot_Index].State)
    {
        case STATE_MAGNETRON_PILOT_OFF:
        case STATE_MAGNETRON_PILOT_ON:
            Magnetron_Pilot_Data[Pilot_Index].State = STATE_MAGNETRON_PILOT_TURBO;
            break;

        default:
            // Event is ignored for all other values of Magnetron_Pilot_Data[Pilot_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Magnetron Pilot state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void MagnetronPilotExecute(void)
{
    switch(Magnetron_Pilot_Data[Pilot_Index].State)
    {
        case STATE_MAGNETRON_PILOT_OFF:
            OffDo();                                // Do function.
            break;

        case STATE_MAGNETRON_PILOT_ON:
            OnDo();                                 // Do function.
            break;

        case STATE_MAGNETRON_PILOT_TURBO:
            TurboDo();                              // Do function.
            break;

        default:
            // There is nothing to do for all other values of Magnetron_Pilot_Data[Pilot_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Magnetron Pilot state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void MagnetronPilotInitialize(void)
{
    Magnetron_Pilot_Data[Pilot_Index].State = STATE_MAGNETRON_PILOT_OFF;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif //(HBL_PILOT_NUM_MAGNETRON > 0)

