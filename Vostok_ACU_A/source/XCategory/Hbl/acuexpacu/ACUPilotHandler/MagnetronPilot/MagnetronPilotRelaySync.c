/**
 *  @file
 *
 *  @brief      Implementation for the Magnetron Pilot Relay Sync.
 *
 *  @details    Detailed description of the file contents
 *
     @startuml{MagnetronPilotRelaySync_SM.png}
        title MagnetronPilotRelaySync State Machine

        ' Use State = Relay_Sync_Data[Pilot_Index].MW_Relay_State
        [*] --> OFF

        ' OFF State - request relay sync off on entry and wait for event in do
            OFF: entry / OffEntry()
            OFF --> ON_PENDING: evOnPending

        ' ON State - request relay sync on on entry and wait for event in do
            ON: entry / OnEntry
            ON --> OFF: evOff
            ON --> OFF_PENDING: evOffPending

        ' OFF_PENDING State - request relay sync off pending on entry and wait for event in do
            OFF_PENDING: do / OffPendingDo()
            OFF_PENDING --> OFF: evOff
            OFF_PENDING --> ON_PENDING: evOnPending

        ' ON_PENDING State - request relay sync on pending on entry and wait for event in do
            ON_PENDING: do / OnPendingDo()
            ON_PENDING --> OFF: evOff
            ON_PENDING --> ON: evOn
            ON_PENDING --> OFF_PENDING: evOffPending

    @enduml
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (HBL_PILOT_NUM_MAGNETRON > 0)
#include "MagnetronPilotRelaySync.h"
#include "MagnetronPilotRelaySync_prv.h"
#include "Hbl.h"
#include "Log.h"
#include "Gpio.h"
#include "VirtualPin.h"
#include "Fault.h"

#include <string.h>

//  --- Private Declarations ------------------------------------------------------------------------------------------

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Magnetron Pilot Relay Sync State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Magnetron Pilot Relay Sync state machine.
typedef enum MAGNETRON_PILOT_RELAY_SYNC_STATE_ENUM
{
    STATE_MAGNETRON_PILOT_RELAY_SYNC_NONE        = 0,

    STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF         = 1, //!< (initial_state)
    STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING  = 2,
    STATE_MAGNETRON_PILOT_RELAY_SYNC_ON          = 3,
    STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING = 4,

    STATE_MAGNETRON_PILOT_RELAY_SYNC_END         = 5
} MAGNETRON_PILOT_RELAY_SYNC_STATE_TYPE;

// -- Private Function Prototypes used by the state machine for customization functions --
static void OffEntry(void);
static void OffPendingDo(void);
static void OnEntry(void);
static void OnPendingDo(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventOff(void);
static void EventOffPending(void);
static void EventOn(void);
static void EventOnPending(void);
static void MagnetronPilotRelaySyncExecute(void);
static void MagnetronPilotRelaySyncInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//! Enumeration for MW relay synchronization type.
typedef enum
{
    SYNC_ERROR,         //!< Sync state error. Relay delay off limits.
    SYNC_NOT_FOUND,     //!< Unsynchronized state
    SYNC_FOUND          //!< Synchronized state
}MW_RELAY_SYNC_TYPE;

//! Relay Sync data type declaration.
typedef PACKED struct RELAY_SYNC_DATA_STRUCT
{
    uint8 MW_Feedback_Pin;
    uint8 MW_Relay_Pin;
    uint8 Turbo_Relay_Pin;
    MAGNETRON_PILOT_RELAY_SYNC_STATE_TYPE MW_Relay_State    : 3;    //!< MW Relay state.
    ON_OFF_TYPE MW_Relay_Pin_State                          : 1;    //!< MW Relay pin state.
    ON_OFF_TYPE Turbo_Relay_Pin_State                       : 1;    //!< Turbo Relay pin state.
    uint8 MW_FB_Active_High                                 : 1;    //!< Flag to indicate MW feedback signal active high or low: True: Active High / False: Active Low
    uint8 Turbo_Enable                                      : 1;    //!< Flag to indicate Turbo enable or disable: True: Enable / False: Disable
    uint8 Zero_Cross_Start_Found                            : 1;    //!< Flag to indicate zero cross start when changing relay state.
    uint8 Peak_Start_Found                                  : 1;    //!< Flag to indicate peak start when changing relay state.
    MW_RELAY_SYNC_TYPE MW_Relay_On_Sync                     : 2;    //!< MW Relay on synchronization state.
    MW_RELAY_SYNC_TYPE MW_Relay_Off_Sync                    : 2;    //!< MW Relay off synchronization state.
    uint8 Error_Present                                     : 1;
    uint8 Error_Changed                                     : 1;
    uint8 Verifying_Delay_Time                              : 1;
    uint8 Door_Open_Detected                                : 1;    //!< Flag to indicate if door open is detected or not.
    uint8 Dynamic_Relay_On_Delay;                                   //!< Current calculated delay for relay on (in samples).
    uint8 Relay_On_Counter;                                         //!< Counter to determine when to turn on relay.
    uint8 Dynamic_Relay_Off_Delay;                                  //!< Current calculated delay for relay off (in samples).
    uint8 Relay_Off_Counter;                                        //!< Counter to determine when to turn off relay.
    uint8 Previous_Feedback_State;                                  //!< Previous feedback state.
    uint8 Majority_Feedback_State;                                  //!< Majority feedback state.
    uint8 Intend_Relay_Switch_Point;                                //!< Mains signal Counter used to determine error in frequency.
    uint8 Error_Counter;
    uint8 Relay_Stable_Delay;
    uint8 Turbo_On_Off_Delay;                                       //!< Delay time between switching microwave and turbo relays.
    uint8 Turbo_On_Off_Counter;
    uint8 Door_GI_Index;                                            //!< Index of Door GI interacted with Magnetron Pilot.

    /** Local variables used by state machine during off with found delay.
    *  The delay during off can not be detected in some cases and need to be verified
    *  at regular intervals specified by VERIFICATION_COUNTER in the parameter file.
    */
    uint8 Ok_Off_Counter;
    uint8 One_MS_Handler_Counter;
} RELAY_SYNC_DATA_TYPE;

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_MAGNETRON_PILOT_RELAY_SYNC

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_MAGNETRON_PILOT_RELAY_SYNC_LOG_MESSAGE_ID_ENUM
{
    MW_RELAY_ON_DELAY_EXCEED_LIMITATION                 = 1,
    MW_RELAY_OFF_DELAY_EXCEED_LIMITATION                = 2,
    MW_RELAY_ERROR_PRESENT                              = 3,
    MW_RELAY_ERROR_CANCEL                               = 4,
} MODULE_MAGNETRON_PILOT_RELAY_SYNC_LOG_MESSAGE_ID_TYPE;

//  --- Private Definitions -------------------------------------------------------------------------------------------
//! Pilot index for current running magnetron state machine.
static uint8 Pilot_Index;

//! Relay sync data array.
static RELAY_SYNC_DATA_TYPE Relay_Sync_Data[HBL_PILOT_NUM_MAGNETRON];

//  --- Private Function Prototypes -----------------------------------------------------------------------------------
static void MwRelayInitialize(void);
static uint8 MwRelayReadFeedbackSignal(void);
static void MwRelayOnSyncStart(void);
static void MwRelayOffSyncStart(void);
static void MWRelayOn(void);
static void MWRelayOff(void);
static BOOL_TYPE IsMWRelayOn(void);
static void TurboRelayOn(void);
static void TurboRelayOff(void);
static BOOL_TYPE IsTurboRelayOn(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MagnetronPilotRelaySync and its variables
 */
void MagnetronPilotRelaySync__Initialize(void)
{
    Pilot_Index = 0;
    memset(Relay_Sync_Data, 0, sizeof(Relay_Sync_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize magnetron pilot Relay Sync instance.
 *
 *  @param      uint8 Parent_Pilot_Index - Index to associated magnetron Pilot instance.
 *              uint8 MW_Feedback_Pin - Index into virtual pin table to find the MW feedback signal.
 *              uint8 MW_FB_Active_High - Flag to indicate MW feedback signal active high or low: True: Active High / False: Active Low
 *              uint8 MW_Relay_Pin - Index into virtual pin table to find the MW relay signal.
 *              uint8 Turbo_Enable - Flag to indicate Turbo enable or disable: True: Enable / False: Disable
 *              uint8 Turbo_Relay_Pin - Index into virtual pin table to find the Turbo relay signal.
 *              uint8 Turbo_On_Off_Delay - Delay time between switching microwave and turbo relays.
 *
 *  @return     void
 */
void MagnetronPilotRelaySync__InitializeInstance(uint8 Parent_Pilot_Index, uint8 MW_Feedback_Pin, uint8 MW_FB_Active_High, uint8 MW_Relay_Pin, uint8 Turbo_Enable, uint8 Turbo_Relay_Pin, uint8 Turbo_On_Off_Delay, uint8 Door_GI_Index)
{
    uint8 table_index;

    Pilot_Index = Parent_Pilot_Index;

    Relay_Sync_Data[Pilot_Index].MW_Feedback_Pin = MW_Feedback_Pin;
    Relay_Sync_Data[Pilot_Index].MW_FB_Active_High = MW_FB_Active_High;
    Relay_Sync_Data[Pilot_Index].MW_Relay_Pin = MW_Relay_Pin;
    Relay_Sync_Data[Pilot_Index].Turbo_Enable = Turbo_Enable;
    Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin = Turbo_Relay_Pin;
    Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Delay = Turbo_On_Off_Delay;
    Relay_Sync_Data[Pilot_Index].Door_GI_Index = Door_GI_Index;

    //! Initialize MW feedback pin
    table_index = Relay_Sync_Data[Pilot_Index].MW_Feedback_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);
    }

    //! Initialize MW relay pin
    table_index = Relay_Sync_Data[Pilot_Index].MW_Relay_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);
    }

    //! Initialize MW relay parameter
    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay = COLD_START_ON_DELAY;
    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay = COLD_START_OFF_DELAY;
    Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point = COUNTER_TWO_PERIODS;
    Relay_Sync_Data[Pilot_Index].Zero_Cross_Start_Found = FALSE;
    Relay_Sync_Data[Pilot_Index].Peak_Start_Found = FALSE;
    Relay_Sync_Data[Pilot_Index].Previous_Feedback_State = 0;
    Relay_Sync_Data[Pilot_Index].Majority_Feedback_State = 0;
    Relay_Sync_Data[Pilot_Index].Door_Open_Detected = FALSE;
    Relay_Sync_Data[Pilot_Index].One_MS_Handler_Counter = 0;
    MwRelayInitialize();
    Fault__ConditionRemoved(XCATEGORY_MAGNETRON_RELAY_FAILURE);
    Fault__ConditionRemoved(XCATEGORY_MAGNETRON_RELAY_SYNCHRONIZATION_FAILURE);

    if(Relay_Sync_Data[Pilot_Index].Turbo_Enable == TRUE)
    {
        //! Initialize turbo relay pin
        table_index = Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin;
        if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
        {
            Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);
        }

        //! Initialize turbo relay parameter
        Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter = 0;
        TurboRelayOff();
    }

    //! Initialize Magnetron Pilot Relay Sync state machine
    MagnetronPilotRelaySyncInitialize();
}

//----------------------------------------------------------------------------------------------
/**
*    @brief    Execute function called at the same rate as main sampler.
*              Shall be called regularly at the same execution rate as mains sampler.
*              Main state machine for relay on and relay off handling. Waits for
*              a mains frequency peak and zero-cross to occur and executes the sample and
*              calculate functions when the time is deemed to be within the allowed window.
*/
void MagnetronPilotRelaySync__200usHandler(uint8 Parent_Pilot_Index)
{
    //! Save Pilot_Index before entering interrupt handler
    uint8 Pilot_Index_Bak = Pilot_Index;

    Pilot_Index = Parent_Pilot_Index;

    MagnetronPilotRelaySyncExecute();

    //! Restore Pilot_Index before exiting interrupt handler
    Pilot_Index = Pilot_Index_Bak;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Handler called at every 5ms.
 * @param      Parent_Pilot_Index, Pilot_Request
 * @return
 */
//---------------------------------------------------------------------------------------------------------------------
void MagnetronPilotRelaySync__AsynchHandler(uint8 Parent_Pilot_Index, MAGNETRON_PILOT_REQUEST_TYPE Pilot_Request)
{
    Pilot_Index = Parent_Pilot_Index;

    //! Turbo_On_Off_Counter count down every 5ms
    if(Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter > 0)
    {
        Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter--;
    }

    //! Turn on/off relay every 25ms
    Relay_Sync_Data[Pilot_Index].One_MS_Handler_Counter++;
    if((Relay_Sync_Data[Pilot_Index].One_MS_Handler_Counter >= 5) ||
       ((Pilot_Request == MAGNETRON_PILOT_REQUEST_OFF) && (MagnetronPilotMainsSampler__GetMainsFrequencyStatus(Pilot_Index) == MAINS_FREQ_ERROR)))
    {
        Relay_Sync_Data[Pilot_Index].One_MS_Handler_Counter = 0;

        if(Relay_Sync_Data[Pilot_Index].Turbo_Enable == TRUE)
        {
            switch(Pilot_Request)
            {
                case MAGNETRON_PILOT_REQUEST_OFF:
                    MwRelayOffSyncStart();
                    if(IsTurboRelayOn())
                    {
                        if(Relay_Sync_Data[Pilot_Index].MW_Relay_State == STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF && !IsMWRelayOn())
                        {
                            TurboRelayOff();
                        }
                    }

                    Fault__ConditionRemoved(XCATEGORY_MAGNETRON_RELAY_SYNCHRONIZATION_FAILURE);
                    break;

                case MAGNETRON_PILOT_REQUEST_ON:
                    if(IsTurboRelayOn())
                    {
                        MwRelayOffSyncStart();
                        if(Relay_Sync_Data[Pilot_Index].MW_Relay_State == STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF && !IsMWRelayOn())
                        {
                            TurboRelayOff();
                            Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter = (Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Delay) / 5;
                        }
                    }
                    else
                    {
                        if(Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter == 0)
                        {
                            MwRelayOnSyncStart();
                        }
                    }
                    break;

                case MAGNETRON_PILOT_REQUEST_TURBO:
                    if(!IsTurboRelayOn())
                    {
                        TurboRelayOn();
                        Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter = (Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Delay) / 5;
                    }
                    else
                    {
                        if(Relay_Sync_Data[Pilot_Index].Turbo_On_Off_Counter == 0)
                        {
                            MwRelayOnSyncStart();
                        }
                    }
                    break;
            }
        }
        else
        {
            if(Pilot_Request == MAGNETRON_PILOT_REQUEST_ON)
            {
                MwRelayOnSyncStart();
            }
            else
            {
                MwRelayOffSyncStart();

                Fault__ConditionRemoved(XCATEGORY_MAGNETRON_RELAY_SYNCHRONIZATION_FAILURE);
            }
        }
    }

    //! Read the new state of the door from door GI every 5ms.
    Relay_Sync_Data[Pilot_Index].Door_Open_Detected = (uint8)(Hbl__GetGIDataConvertedByIndex(Relay_Sync_Data[Pilot_Index].Door_GI_Index) == 0);

    //! Check if there is relay error msg needed to be published
    if (Relay_Sync_Data[Pilot_Index].Error_Present == TRUE)
    {
        Fault__ConditionDetected(XCATEGORY_MAGNETRON_RELAY_FAILURE);
        if (Relay_Sync_Data[Pilot_Index].Error_Changed == TRUE)
        {
            LOG_ADD_EXCEPTION(MW_RELAY_ERROR_PRESENT, Pilot_Index);
        }
    }
    else
    {
        Fault__ConditionRemoved(XCATEGORY_MAGNETRON_RELAY_FAILURE);
        if (Relay_Sync_Data[Pilot_Index].Error_Changed == TRUE)
        {
            LOG_ADD_EXCEPTION(MW_RELAY_ERROR_CANCEL, Pilot_Index);
        }
    }
    Relay_Sync_Data[Pilot_Index].Error_Changed = FALSE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Handler to sync the mains period
 * @param      Parent_Pilot_Index, Period
 * @return
 */
//---------------------------------------------------------------------------------------------------------------------
void MagnetronPilotRelaySync__MainsPeriodSync(uint8 Parent_Pilot_Index, uint8 Period)
{
    if ((Relay_Sync_Data[Parent_Pilot_Index].MW_Relay_State != STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING) && (Relay_Sync_Data[Parent_Pilot_Index].MW_Relay_State != STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING))
    {
        Relay_Sync_Data[Parent_Pilot_Index].Intend_Relay_Switch_Point = (Period * 2);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Handler to handle the mains angle 0,90,180,270 event
 * @param      Parent_Pilot_Index, Event_Type
 * @return
 */
//---------------------------------------------------------------------------------------------------------------------
void MagnetronPilotRelaySync__MainsEventHandler(uint8 Parent_Pilot_Index, MAINS_EVENT_TYPE Event_Type)
{
    //! Save Pilot_Index before entering interrupt handler
    uint8 Pilot_Index_Bak = Pilot_Index;

    Pilot_Index = Parent_Pilot_Index;

    switch(Event_Type)
    {
        case MAINS_ANGLE_0_EVENT:
            if (Relay_Sync_Data[Pilot_Index].MW_Relay_State == STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING)
                Relay_Sync_Data[Pilot_Index].Zero_Cross_Start_Found = TRUE;
            else
                Relay_Sync_Data[Pilot_Index].Zero_Cross_Start_Found = FALSE;
            break;

        case MAINS_ANGLE_90_EVENT:
            if (Relay_Sync_Data[Pilot_Index].MW_Relay_State == STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING)
                Relay_Sync_Data[Pilot_Index].Peak_Start_Found = TRUE;
            else
                Relay_Sync_Data[Pilot_Index].Peak_Start_Found = FALSE;

            //! Relay error detection
            if (Relay_Sync_Data[Pilot_Index].Relay_Stable_Delay > 0)
            {
                Relay_Sync_Data[Pilot_Index].Relay_Stable_Delay--;
            }
            else
            {
                if (Relay_Sync_Data[Pilot_Index].Door_Open_Detected == FALSE && !IsMWRelayOn())
                {
                    if(MwRelayReadFeedbackSignal())
                    {
                        if (Relay_Sync_Data[Pilot_Index].Error_Present == FALSE)
                        {
                            Relay_Sync_Data[Pilot_Index].Error_Counter++;
                            if(Relay_Sync_Data[Pilot_Index].Error_Counter > 16)
                            {
                                Relay_Sync_Data[Pilot_Index].Error_Counter = 16;
                                Relay_Sync_Data[Pilot_Index].Error_Present = TRUE;
                                Relay_Sync_Data[Pilot_Index].Error_Changed = TRUE;
                                EventOff();
                            }
                        }
                    }
                    else
                    {
                        if (Relay_Sync_Data[Pilot_Index].Error_Present == TRUE)
                        {
                            Relay_Sync_Data[Pilot_Index].Error_Counter--;
                            if (Relay_Sync_Data[Pilot_Index].Error_Counter == 0)
                            {
                                Relay_Sync_Data[Pilot_Index].Error_Present = FALSE;
                                Relay_Sync_Data[Pilot_Index].Error_Changed = TRUE;
                            }
                        }
                    }
                }
                else
                {
                    if (Relay_Sync_Data[Pilot_Index].Error_Present == TRUE)
                    {
                        Relay_Sync_Data[Pilot_Index].Error_Present = FALSE;
                        Relay_Sync_Data[Pilot_Index].Error_Changed = TRUE;
                    }
                    Relay_Sync_Data[Pilot_Index].Error_Counter = 0;
                }
            }
            break;

        case MAINS_ANGLE_180_EVENT:
            break;

        case MAINS_ANGLE_270_EVENT:
            break;

        case MAINS_NO_EVENT:
            break;
    }

    //! Restore Pilot_Index before exiting interrupt handler
    Pilot_Index = Pilot_Index_Bak;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OffEntry(void)
{
    MWRelayOff();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OffPendingDo(void)
{
    if (Relay_Sync_Data[Pilot_Index].Zero_Cross_Start_Found == TRUE)
    {
        Relay_Sync_Data[Pilot_Index].Relay_Off_Counter++;

        if (Relay_Sync_Data[Pilot_Index].Relay_Off_Counter == (Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point - Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay))
        {
            MWRelayOff();
        }

        if ((Relay_Sync_Data[Pilot_Index].Relay_Off_Counter <= (Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point + WINDOW_END_OFF)) && (Relay_Sync_Data[Pilot_Index].Relay_Off_Counter >= (Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point + WINDOW_START_OFF)))
        {
            Relay_Sync_Data[Pilot_Index].Majority_Feedback_State += MwRelayReadFeedbackSignal();

            /** Relay delay time during off is only evaluated at last sample in window. */
            if (Relay_Sync_Data[Pilot_Index].Relay_Off_Counter == Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point + WINDOW_END_OFF)
            {
                /** Regardless of relay delay size, relay stays off. */
                EventOff();

                uint8 off_state_feedback;

                if (Relay_Sync_Data[Pilot_Index].Majority_Feedback_State >= FEEDBACK_SAMPLE_SIZE)
                {
                    off_state_feedback = TRUE;
                }
                else
                {
                    off_state_feedback = FALSE;
                }

                /** Relay delay time determined */
                if (Relay_Sync_Data[Pilot_Index].MW_Relay_Off_Sync == SYNC_FOUND)
                {
                    if (off_state_feedback == TRUE)
                    {
                        /** Relay off delay too short, not synchronized anymore? */
                        Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay += ADJUST_SHORT_OFF_DELAY;

                        /** If verifying delay time, this is normal otherwise something is wrong */
                        if (Relay_Sync_Data[Pilot_Index].Verifying_Delay_Time == TRUE)
                        {
                            Relay_Sync_Data[Pilot_Index].Ok_Off_Counter = 0;
                            Relay_Sync_Data[Pilot_Index].Verifying_Delay_Time = FALSE;
                        }
                        else
                        {
                            Relay_Sync_Data[Pilot_Index].MW_Relay_Off_Sync = SYNC_NOT_FOUND;
                        }
                    }
                    else
                    {
                        Relay_Sync_Data[Pilot_Index].Ok_Off_Counter++;
                        if (Relay_Sync_Data[Pilot_Index].Ok_Off_Counter > VERIFICATION_COUNTER)
                        {
                            /** Shorten relay delay and flag that we are verifying the time */
                            Relay_Sync_Data[Pilot_Index].Verifying_Delay_Time = TRUE;
                            Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay -= ADJUST_LONG_OFF_DELAY;
                        }
                    }

                }

                /** Relay delay time not yet determined */
                if (Relay_Sync_Data[Pilot_Index].MW_Relay_Off_Sync == SYNC_NOT_FOUND)
                {
                    /** Feedback on means too short relay delay */
                    if (off_state_feedback == FALSE)
                    {
                        /** Relay off delay too long */
                        Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay -= ADJUST_LONG_OFF_DELAY;

                        if (Relay_Sync_Data[Pilot_Index].Previous_Feedback_State != 0)
                        {
                            /** In this case, we have found our delay time. */
                            Relay_Sync_Data[Pilot_Index].MW_Relay_Off_Sync = SYNC_FOUND;
                        }
                    }
                    /**  Feedback is off in window. Delay too short */
                    else
                    {
                        Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay += ADJUST_SHORT_OFF_DELAY;
                    }
                    Relay_Sync_Data[Pilot_Index].Previous_Feedback_State = off_state_feedback;
                }

                Relay_Sync_Data[Pilot_Index].Majority_Feedback_State = 0;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OnEntry(void)
{
    MWRelayOn();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OnPendingDo(void)
{
    if (Relay_Sync_Data[Pilot_Index].Peak_Start_Found == TRUE)
    {
        Relay_Sync_Data[Pilot_Index].Relay_On_Counter++;

        if (Relay_Sync_Data[Pilot_Index].Relay_On_Counter == (Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point - Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay))
        {
            MWRelayOn();
        }

        if ((Relay_Sync_Data[Pilot_Index].Relay_On_Counter <= (Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point + WINDOW_END_ON)) && (Relay_Sync_Data[Pilot_Index].Relay_On_Counter >= (Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point - WINDOW_START_ON)))
        {
            if (Relay_Sync_Data[Pilot_Index].MW_Relay_On_Sync == SYNC_NOT_FOUND)
            {
                MWRelayOff();
            }

            if (MwRelayReadFeedbackSignal())
            {
                /** Feedback ok at first sample. Leave on and adjust */
                if (Relay_Sync_Data[Pilot_Index].Relay_On_Counter == Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point - WINDOW_START_ON)
                {
                    EventOn();
                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay = Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay - ADJUST_LONG_ON_DELAY;
                    Relay_Sync_Data[Pilot_Index].MW_Relay_On_Sync = SYNC_NOT_FOUND;

                }
                else
                {
                    EventOn();
                    /** Adjust for feedback on time relative to where in the allowed window the feedback is found */
                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay = (Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay + (Relay_Sync_Data[Pilot_Index].Relay_On_Counter - Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point));
                    Relay_Sync_Data[Pilot_Index].MW_Relay_On_Sync = SYNC_FOUND;
                }
            }
            else
            {
                if (Relay_Sync_Data[Pilot_Index].Relay_On_Counter == Relay_Sync_Data[Pilot_Index].Intend_Relay_Switch_Point + WINDOW_END_ON)
                {
                    EventOff();
                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay = Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay + ADJUST_SHORT_ON_DELAY;
                    Relay_Sync_Data[Pilot_Index].MW_Relay_On_Sync = SYNC_NOT_FOUND;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the MW relay and it's variales
 *
 */
static void MwRelayInitialize(void)
{
    MWRelayOff();

    /**
    * @brief Cold start assumption.
    *        Relay delay not synchronized.
    *        Mains signal frequency not known.
    */
    Relay_Sync_Data[Pilot_Index].MW_Relay_On_Sync = SYNC_NOT_FOUND;
    Relay_Sync_Data[Pilot_Index].MW_Relay_Off_Sync = SYNC_NOT_FOUND;
    Relay_Sync_Data[Pilot_Index].Relay_On_Counter = 0;
    Relay_Sync_Data[Pilot_Index].Relay_Off_Counter = 0;
    Relay_Sync_Data[Pilot_Index].Error_Present = FALSE;
    Relay_Sync_Data[Pilot_Index].Error_Changed = FALSE;
    Relay_Sync_Data[Pilot_Index].Error_Counter = 0;
    Relay_Sync_Data[Pilot_Index].Relay_Stable_Delay = 60;  //delay 1s for error detection after power on.
    Relay_Sync_Data[Pilot_Index].Ok_Off_Counter = 0;
    Relay_Sync_Data[Pilot_Index].Verifying_Delay_Time = FALSE;
}

//----------------------------------------------------------------------------------------------
/**
*    @brief    Samples the feedback signal at a specified time decided by the
*              package MwRelaySync.
*    @details  Performs sampling of the MW Relay feedback signal in relation to the mains
*              signal. Decision of the feedback state is taken with majority that 2 of 3
*              samples is determined low state.
*    @param    void
*
*    @return   State of feedback signal.
*    @retval   0   Feedback signal not present
*    @retval   1   Feedback signal present
*/
static uint8 MwRelayReadFeedbackSignal(void)
{
    uint8 mw_fb_pinState = 0;

    uint8 table_index = Relay_Sync_Data[Pilot_Index].MW_Feedback_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        mw_fb_pinState = Gpio__PinRead(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin);
    }

    if (Relay_Sync_Data[Pilot_Index].MW_FB_Active_High == FALSE)
    {
        mw_fb_pinState  ^= (BOOL_TYPE) 1;
    }

    return (mw_fb_pinState);
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It try to turn on MW Relay at proper timing.
 */
static void MwRelayOnSyncStart(void)
{
    if (Relay_Sync_Data[Pilot_Index].Error_Present == FALSE)
    {
        if((Relay_Sync_Data[Pilot_Index].MW_Relay_State != STATE_MAGNETRON_PILOT_RELAY_SYNC_ON) && (Relay_Sync_Data[Pilot_Index].MW_Relay_State != STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING))
        {
            if (MagnetronPilotMainsSampler__GetMainsFrequencyStatus(Pilot_Index) == MAINS_FREQ_OK)
            {
                if (Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay < RELAY_ON_LOW_LIMIT)
                {
                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay = RELAY_ON_LOW_LIMIT;
                    MwRelayInitialize();
                }

                if (Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay > RELAY_ON_HIGH_LIMIT)
                {
                    LOG_ADD_EXCEPTION(MW_RELAY_ON_DELAY_EXCEED_LIMITATION, Pilot_Index);
                    Fault__ConditionDetected(XCATEGORY_MAGNETRON_RELAY_SYNCHRONIZATION_FAILURE);

                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_On_Delay = RELAY_ON_LOW_LIMIT;
                    MwRelayInitialize();
                }

                Relay_Sync_Data[Pilot_Index].Relay_On_Counter = 0;
                Relay_Sync_Data[Pilot_Index].Peak_Start_Found = FALSE;
                EventOnPending();
            }
        }
    }
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It try to turn off MW Relay at proper timing.
 */
static void MwRelayOffSyncStart(void)
{
    if(IsMWRelayOn())
    {
        if(Relay_Sync_Data[Pilot_Index].Door_Open_Detected == TRUE)
        {
            EventOff();
        }
        else if (MagnetronPilotMainsSampler__GetMainsFrequencyStatus(Pilot_Index) == MAINS_FREQ_ERROR)
        {
            EventOff();
        }
        else
        {
            if((Relay_Sync_Data[Pilot_Index].MW_Relay_State != STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF) && (Relay_Sync_Data[Pilot_Index].MW_Relay_State != STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING))
            {
                if (Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay < RELAY_OFF_LOW_LIMIT)
                {
                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay = RELAY_OFF_LOW_LIMIT;
                    MwRelayInitialize();
                }

                if (Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay > RELAY_OFF_HIGH_LIMIT)
                {
                    LOG_ADD_EXCEPTION(MW_RELAY_OFF_DELAY_EXCEED_LIMITATION, Pilot_Index);

                    Relay_Sync_Data[Pilot_Index].Dynamic_Relay_Off_Delay = RELAY_OFF_LOW_LIMIT;
                    MwRelayInitialize();
                }

                Relay_Sync_Data[Pilot_Index].Relay_Off_Counter = 0;
                Relay_Sync_Data[Pilot_Index].Zero_Cross_Start_Found = FALSE;
                EventOffPending();
            }
        }
    }
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It Turn on actual MW Relay
 */
static void MWRelayOn(void)
{
    Relay_Sync_Data[Pilot_Index].MW_Relay_Pin_State = ON;

    uint8 table_index = Relay_Sync_Data[Pilot_Index].MW_Relay_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, TRUE);
    }
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It Turn off actual MW Relay
 */
static void MWRelayOff(void)
{
    Relay_Sync_Data[Pilot_Index].MW_Relay_Pin_State = OFF;

    uint8 table_index = Relay_Sync_Data[Pilot_Index].MW_Relay_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, FALSE);
    }
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It return MW Relay state
 */
static BOOL_TYPE IsMWRelayOn(void)
{
    return ((Relay_Sync_Data[Pilot_Index].MW_Relay_Pin_State == ON) ? TRUE : FALSE);
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It Turn on actual Turbo Relay
 */
static void TurboRelayOn(void)
{
    Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin_State = ON;

    uint8 table_index = Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, TRUE);
    }
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It Turn off actual Turbo Relay
 */
static void TurboRelayOff(void)
{
    Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin_State = OFF;

    uint8 table_index = Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin;
    if (VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
    {
        Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, FALSE);
    }
}

//----------------------------------------------------------------------------------------------
/**
 *  @brief      It return Turbo Relay state
 */
static BOOL_TYPE IsTurboRelayOn(void)
{
    return ((Relay_Sync_Data[Pilot_Index].Turbo_Relay_Pin_State == ON) ? TRUE : FALSE);
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Magnetron Pilot Relay Sync State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Off Event for the Magnetron Pilot Relay Sync state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOff(void)
{
    switch (Relay_Sync_Data[Pilot_Index].MW_Relay_State)
    {
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING:
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_ON:
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING:
            Relay_Sync_Data[Pilot_Index].MW_Relay_State = STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF;
            OffEntry();                             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Relay_Sync_Data[Pilot_Index].MW_Relay_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Off Pending Event for the Magnetron Pilot Relay Sync state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOffPending(void)
{
    switch (Relay_Sync_Data[Pilot_Index].MW_Relay_State)
    {
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING:
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_ON:
            Relay_Sync_Data[Pilot_Index].MW_Relay_State = STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING;
            break;

        default:
            // Event is ignored for all other values of Relay_Sync_Data[Pilot_Index].MW_Relay_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the On Event for the Magnetron Pilot Relay Sync state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOn(void)
{
    switch (Relay_Sync_Data[Pilot_Index].MW_Relay_State)
    {
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING:
            Relay_Sync_Data[Pilot_Index].MW_Relay_State = STATE_MAGNETRON_PILOT_RELAY_SYNC_ON;
            OnEntry();                              // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Relay_Sync_Data[Pilot_Index].MW_Relay_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the On Pending Event for the Magnetron Pilot Relay Sync state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOnPending(void)
{
    switch (Relay_Sync_Data[Pilot_Index].MW_Relay_State)
    {
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF:
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING:
            Relay_Sync_Data[Pilot_Index].MW_Relay_State = STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING;
            break;

        default:
            // Event is ignored for all other values of Relay_Sync_Data[Pilot_Index].MW_Relay_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Magnetron Pilot Relay Sync state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void MagnetronPilotRelaySyncExecute(void)
{
    switch(Relay_Sync_Data[Pilot_Index].MW_Relay_State)
    {
        case STATE_MAGNETRON_PILOT_RELAY_SYNC_ON_PENDING:
            OnPendingDo();                          // Do function.
            break;

        case STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF_PENDING:
            OffPendingDo();                         // Do function.
            break;

        default:
            // There is nothing to do for all other values of Relay_Sync_Data[Pilot_Index].MW_Relay_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Magnetron Pilot Relay Sync state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void MagnetronPilotRelaySyncInitialize(void)
{
    Relay_Sync_Data[Pilot_Index].MW_Relay_State = STATE_MAGNETRON_PILOT_RELAY_SYNC_OFF;
    OffEntry();                                     // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif //(HBL_PILOT_NUM_MAGNETRON > 0)

