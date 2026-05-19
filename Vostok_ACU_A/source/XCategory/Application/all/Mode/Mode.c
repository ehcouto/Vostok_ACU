/**
 *  @file       
 *
 *  @brief      This module controls the overall mode (state) of the appliance
 *
 *  @details

    @startuml {SubMode_state_machine.jpg}
        title SubMode State Machine

        State Initialize
        State SubModeNormalSupervisor
        State SubModeError
        State SubModeNormalExtControlEntering
        State SubModeNormalExtControlEngaged


        [*] --> Initialize

        Initialize : do /\t HandleRequestedSubMode()
        Initialize --> SubModeNormalSupervisor : evReqSubModeSupervisor \n [IsReqSubModeInitialize()]
        Initialize --> SubModeError : evReqSubModeError \n [IsReqSubModeError()]

        SubModeNormalSupervisor : enter /\t SetReqSubModeNone() \n\t\t PublishMode()
        SubModeNormalSupervisor : do /\t HandleRequestedSubMode()
        SubModeNormalSupervisor --> SubModeError                    : evReqSubModeError
        SubModeNormalSupervisor --> SubModeNormalExtControlEntering : evReqSubModeExtControlEntering \n/ StartTimer()
        SubModeNormalSupervisor --> SubModeNormalSupervisor         : evReqSubModeSupervisor

        SubModeNormalExtControlEntering : do /\t VerifyEnteringTimerIsRunning() \n\t HandleRequestedSubMode()
        SubModeNormalExtControlEntering --> SubModeNormalExtControlEngaged  : evReqSubModeExtControlEngaged
        SubModeNormalExtControlEntering --> SubModeNormalSupervisor         : evReqSubModeSupervisor
        SubModeNormalExtControlEntering --> SubModeNormalSupervisor         : evTimerExpired

        SubModeNormalExtControlEngaged : enter /\t StartTimer() \n\t\t PublishMode()
        SubModeNormalExtControlEngaged : do /\t VerifyEngagedTimerIsRunning() \n\t HandleRequestedSubMode()
        SubModeNormalExtControlEngaged : exit
        SubModeNormalExtControlEngaged --> SubModeNormalSupervisor  : evReqSubModeSupervisor
        SubModeNormalExtControlEngaged --> SubModeNormalSupervisor  : evTimerExpired

    @enduml

 *
 *  @copyright  Copyright 2015-$Date: 2015/08/06 16:33:50EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#include "API011AppCtrl.h"
#include "Millis.h"
#include "Mode.h"
#include "SRMicro.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Sub Mode State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Sub Mode state machine.
typedef enum SUB_MODE_STATE_ENUM
{
    STATE_SUB_MODE_NONE                        = 0,

    STATE_SUB_MODE_INITIALIZE                  = 1, //!< (initial_state)
    STATE_SUB_MODE_NORMAL_SUPERVISOR           = 2,
    STATE_SUB_MODE_ERROR                       = 3,
    STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING = 4,
    STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENGAGED  = 5,

    STATE_SUB_MODE_END                         = 6
} SUB_MODE_STATE_TYPE;

//! The state variable for the Sub Mode state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static SUB_MODE_STATE_TYPE Sub_Mode_State;

//! The transition variable for the Sub Mode state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Sub_Mode_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void HandleRequestedSubMode(void);
static BOOL_TYPE IsReqSubModeError(void);
static BOOL_TYPE IsReqSubModeInitialize(void);
static void PublishMode(void);
static void SetReqSubModeNone(void);
static void StartTimer(void);
static void VerifyEngagedTimerIsRunning(void);
static void VerifyEnteringTimerIsRunning(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventReqSubModeError(void);
static void EventReqSubModeExtControlEngaged(void);
static void EventReqSubModeExtControlEntering(void);
static void EventReqSubModeSupervisor(void);
static void EventTimerExpired(void);
static void SubModeExecute(void);
static void SubModeInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! SubMode that is used only in the core.  Not defined in SUBMODE_TYPE because it is not needed in the API.
#define SUBMODE_NONE    ((SUBMODE_TYPE)(SUBMODE_NORMAL_EXT_CONTROL_ENGAGED + 1))

//! time out for keeping the SUBMODE_NORMAL_EXT_CONTROL_ENTERING active
#define SUB_MODE_EXT_ENTERING_TIMEOUT   (5000)

//! time out for keeping the SUBMODE_NORMAL_EXT_CONTROL_ENGAGED active
#define SUB_MODE_EXT_ENGAGED_TIMEOUT    (30000)

static MODE_TYPE Mode;
static MODE_TYPE Remote_Mode;
static uint8 N_Mode;
static SUBMODE_TYPE Requested_Sub_Mode;
static MILLIS_TIMER_TYPE Timer_PC_Control_Handle;
static SUBMODE_TYPE Remote_SubMode;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Mode and its variables
 *
 */
void Mode__Initialize(void)
{
    Remote_Mode = MODE_NORMAL;

    if ((Mode > MODE_LOWPOWER) ||
        (N_Mode != ((uint8)(~Mode))) ||
        (SRMicro__GetResetMode() == MICRO_RESET_MODE_COLD) ||
        (Sub_Mode_State == STATE_SUB_MODE_NONE))
    {
        Mode = MODE_NORMAL;
        N_Mode = (uint8)(~MODE_NORMAL);
        Requested_Sub_Mode = SUBMODE_NORMAL_INITIALIZE;
        SubModeInitialize();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Sets the mode of the machine and causes a warm reset (except for MODE_SETTINGFILE_NOT_CONSISTENT)
 * @param   mode: See MODE_TYPE
 */
void Mode__SetMode(MODE_TYPE mode)
{
    if (Mode != mode)
    {
        Mode = mode;
        N_Mode = (uint8)(~mode);

        if (mode != MODE_SETTINGFILE_NOT_CONSISTENT)
        {
            SRMicro__ForceReset(MICRO_RESET_MODE_WARM);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param mode
 * @param sub_mode
 */
void Mode__SetModeExtended(MODE_TYPE mode, SUBMODE_TYPE sub_mode)
{
    if (Mode != mode)
    {
        Mode = mode;
        N_Mode = (uint8)(~mode);
        Requested_Sub_Mode = sub_mode;
        if (mode != MODE_SETTINGFILE_NOT_CONSISTENT)
        {
            SRMicro__ForceReset(MICRO_RESET_MODE_WARM);
        }
    }
    else
    {
        Requested_Sub_Mode = sub_mode;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get the current mode of the machine.
 * @return  See MODE_TYPE.
 */
MODE_TYPE Mode__GetMode(void)
{
    return (Mode);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get the current sub-mode of the machine.
 * @return  See SUBMODE_TYPE.
 */
SUBMODE_TYPE Mode__GetSubMode(void)
{
    static const SUBMODE_TYPE SUBMODE_STATE_MAP[] =
    {
        SUBMODE_NORMAL_SUPERVISOR,              // STATE_SUB_MODE_NONE
        SUBMODE_NORMAL_SUPERVISOR,              // STATE_SUB_MODE_INITIALIZE
        SUBMODE_NORMAL_SUPERVISOR,              // STATE_SUB_MODE_NORMAL_SUPERVISOR
        SUBMODE_NORMAL_ERROR,                   // STATE_SUB_MODE_ERROR
        SUBMODE_NORMAL_EXT_CONTROL_ENTERING,    // STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING
        SUBMODE_NORMAL_EXT_CONTROL_ENGAGED      // STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENGAGED
    };
    CT_ASSERT((sizeof(SUBMODE_STATE_MAP) / sizeof(SUBMODE_TYPE)) == STATE_SUB_MODE_END);
    return (SUBMODE_STATE_MAP[Sub_Mode_State]);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Background tasks for the module.
 */
void Mode__Handler(void)
{
    switch (Mode)
    {
        case MODE_NORMAL:
            SubModeExecute();
            break;
        case MODE_PROGRAMMING:
        case MODE_SETTINGFILE_NOT_CONSISTENT:
        case MODE_LOWPOWER:
            #if (SYSTEM_ACU_HANDLING == DISABLED) && (SYSTEM_HMI_HANDLING == DISABLED)
                // Modes not handled on a pure expansion board
                SubModeInitialize();
            #endif
            break;
        default:
            SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param heartbeat_code
 */
void Mode__SpecialHeartBeat(MODE_SPECIAL_HEARTBEAT_TYPE heartbeat_code)
{
    if (Mode__IsPCControlEngaged() == TRUE)
    {
        StartTimer();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Determines if the Mode and Sub-Mode have the values that indicate the product is engaged in PC control
 *  @return True if PC Control is engaged, else FALSE
 */
BOOL_TYPE Mode__IsPCControlEngaged(void)
{
    return (((Mode == MODE_NORMAL) && (Mode__GetSubMode() == SUBMODE_NORMAL_EXT_CONTROL_ENGAGED)) ? TRUE : FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes if the board is in programming allowed mode
 * @return BOOL_TYPE
 *    @retval TRUE - Programming allowed .
 *    @retval FALSE - Programming not allowed .
 */
BOOL_TYPE Mode__IsProgrammingEngaged(void)
{
    return (((Mode == MODE_PROGRAMMING) || (Mode == MODE_SETTINGFILE_NOT_CONSISTENT)) ? TRUE : FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Saves the remote mode
 *  @param  mode: the remote mode to save
 */
void Mode__SaveRemoteMode(MODE_TYPE mode)
{
    Remote_Mode = mode;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Get the current remote mode
 *  @return Returns the current remote mode
 */
MODE_TYPE Mode__GetRemoteMode(void)
{
    return (Remote_Mode);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Saves the remote Sub Mode
 *  @param  submode: the Sub Mode to save
 */
void Mode__SaveRemoteSubMode(SUBMODE_TYPE submode)
{
    Remote_SubMode = submode;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Get the current remote Sub Mode
 *  @return Returns the current remote Sub Mode
 */
SUBMODE_TYPE Mode__GetRemoteSubMode(void)
{
    return (Remote_SubMode);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  Handles the static variable Requested_Sub_Mode.
 */
static void HandleRequestedSubMode(void)
{
    switch ( Requested_Sub_Mode )
    {
        case SUBMODE_NORMAL_SUPERVISOR:
            EventReqSubModeSupervisor();
            break;
        case SUBMODE_NORMAL_ERROR:
            EventReqSubModeError();
            break;
        case SUBMODE_NORMAL_EXT_CONTROL_ENTERING:
            EventReqSubModeExtControlEntering();
            break;
        case SUBMODE_NORMAL_EXT_CONTROL_ENGAGED:
            EventReqSubModeExtControlEngaged();
            break;
        case SUBMODE_NORMAL_INITIALIZE:
            EventReqSubModeSupervisor();
            break;
        default:
            // do nothing
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Publishes the current Mode to Reveal.
 */
static void PublishMode(void)
{
    API011AppCtrl__PubMode();   // Main ACU/HMI publishes mode change
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetReqSubModeNone(void)
{
    Requested_Sub_Mode = SUBMODE_NONE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines if the requested sub mode is ERROR or not
 * @return  TRUE if the requested sub mode is ERROR, else FALSE
 */
static BOOL_TYPE IsReqSubModeError(void)
{
    return ((BOOL_TYPE)(Requested_Sub_Mode == SUBMODE_NORMAL_ERROR));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines if the requested sub mode is INITIALIZE or not
 * @return  TRUE if the requested sub mode is INITIALIZE, else FALSE
 */
static BOOL_TYPE IsReqSubModeInitialize(void)
{
    return ((BOOL_TYPE)(Requested_Sub_Mode == SUBMODE_NORMAL_INITIALIZE));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Starts Millis timer.
 */
static void StartTimer(void)
{
    Millis__Start(&Timer_PC_Control_Handle);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Verifies the millis timer is reached timeout or not
 */
static void VerifyEngagedTimerIsRunning(void)
{
    if (Millis__ReachedTarget(&Timer_PC_Control_Handle, SUB_MODE_EXT_ENGAGED_TIMEOUT) != FALSE)
    {
        EventTimerExpired();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Verifies the millis timer is reached timeout or not
 */
static void VerifyEnteringTimerIsRunning(void)
{
    if (Millis__ReachedTarget(&Timer_PC_Control_Handle, SUB_MODE_EXT_ENTERING_TIMEOUT) != FALSE)
    {
        EventTimerExpired();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Sub Mode State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Req Sub Mode Error Event for the Sub Mode state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReqSubModeError(void)
{
    switch (Sub_Mode_State)
    {
        case STATE_SUB_MODE_INITIALIZE:
            if (IsReqSubModeError() == TRUE)
            {
                Sub_Mode_State = STATE_SUB_MODE_ERROR;
                Sub_Mode_Transitioned = TRUE;
            }
            break;

        case STATE_SUB_MODE_NORMAL_SUPERVISOR:
            Sub_Mode_State = STATE_SUB_MODE_ERROR;
            Sub_Mode_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Sub_Mode_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Req Sub Mode Ext Control Engaged Event for the Sub Mode state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReqSubModeExtControlEngaged(void)
{
    switch (Sub_Mode_State)
    {
        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING:
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENGAGED;
            Sub_Mode_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                PublishMode();                      // Entry Function.
            }
            Sub_Mode_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Sub_Mode_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Req Sub Mode Ext Control Entering Event for the Sub Mode state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReqSubModeExtControlEntering(void)
{
    switch (Sub_Mode_State)
    {
        case STATE_SUB_MODE_NORMAL_SUPERVISOR:
            StartTimer();                           // Transition action function.
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING;
            Sub_Mode_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Sub_Mode_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Req Sub Mode Supervisor Event for the Sub Mode state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReqSubModeSupervisor(void)
{
    switch (Sub_Mode_State)
    {
        case STATE_SUB_MODE_INITIALIZE:
            if (IsReqSubModeInitialize() == TRUE)
            {
                Sub_Mode_State = STATE_SUB_MODE_NORMAL_SUPERVISOR;
                Sub_Mode_Transitioned = FALSE;
                SetReqSubModeNone();                // Entry Function.
                if (Sub_Mode_Transitioned == FALSE)
                {
                    PublishMode();                  // Entry Function.
                }
                Sub_Mode_Transitioned = TRUE;
            }
            break;

        case STATE_SUB_MODE_NORMAL_SUPERVISOR:
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_SUPERVISOR;
            Sub_Mode_Transitioned = FALSE;
            SetReqSubModeNone();                    // Entry Function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                PublishMode();                      // Entry Function.
            }
            Sub_Mode_Transitioned = TRUE;
            break;

        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING:
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_SUPERVISOR;
            Sub_Mode_Transitioned = FALSE;
            SetReqSubModeNone();                    // Entry Function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                PublishMode();                      // Entry Function.
            }
            Sub_Mode_Transitioned = TRUE;
            break;

        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENGAGED:
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_SUPERVISOR;
            Sub_Mode_Transitioned = FALSE;
            SetReqSubModeNone();                    // Entry Function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                PublishMode();                      // Entry Function.
            }
            Sub_Mode_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Sub_Mode_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timer Expired Event for the Sub Mode state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimerExpired(void)
{
    switch (Sub_Mode_State)
    {
        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING:
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_SUPERVISOR;
            Sub_Mode_Transitioned = FALSE;
            SetReqSubModeNone();                    // Entry Function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                PublishMode();                      // Entry Function.
            }
            Sub_Mode_Transitioned = TRUE;
            break;

        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENGAGED:
            Sub_Mode_State = STATE_SUB_MODE_NORMAL_SUPERVISOR;
            Sub_Mode_Transitioned = FALSE;
            SetReqSubModeNone();                    // Entry Function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                PublishMode();                      // Entry Function.
            }
            Sub_Mode_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Sub_Mode_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Sub Mode state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void SubModeExecute(void)
{
    Sub_Mode_Transitioned = FALSE;
    switch(Sub_Mode_State)
    {
        case STATE_SUB_MODE_INITIALIZE:
        case STATE_SUB_MODE_NORMAL_SUPERVISOR:
            HandleRequestedSubMode();               // Do function.
            break;

        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENTERING:
            VerifyEnteringTimerIsRunning();         // Do function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                HandleRequestedSubMode();           // Do function.
            }
            break;

        case STATE_SUB_MODE_NORMAL_EXT_CONTROL_ENGAGED:
            VerifyEngagedTimerIsRunning();          // Do function.
            if (Sub_Mode_Transitioned == FALSE)
            {
                HandleRequestedSubMode();           // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of Sub_Mode_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Sub Mode state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void SubModeInitialize(void)
{
    Sub_Mode_State = STATE_SUB_MODE_INITIALIZE;
    Sub_Mode_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.
