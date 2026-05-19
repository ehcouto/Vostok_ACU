/**
 * @file: SensedDrainModel.c
 *
 * @model: 'SensedDrainModel'.
 *
 * @brief Sensed Drain process is designed to detect the point when the Dishwasher gets empty with drain using VWI Heavy filter signal which monitors water level. Indication of drain completion is when the VWI value gets stabilized and hardly no change in the input.
 *
 * @modelversion:  1.1795
 *
 * @coderversion:  8.14 (R2018a) 06-Feb-2018
 *
 * C/C++ source code generated on : Thu Apr  2 15:57:14 2020
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "SensedDrainModel.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

/* Named constants for Chart: '<S5>/SensedDrain__Logic' */
#define IN_CONFIRMING_MODE_PUMPS_IN_SER ((uint8)1U)
#define IN_DRAINING_AND_CONFIRMING_MODE ((uint8)2U)
#define IN_DRAINING_MODE_PUMPS_IN_SERIE ((uint8)3U)
#define IN_START_PUMPS_AND_WAIT_FOR_STA ((uint8)2U)
#define Se_IN_WAIT_DRAIN_PUMP_RAMP_DOWN ((uint8)2U)
#define Sen_IN_WAIT_WASH_PUMP_RAMP_DOWN ((uint8)5U)
#define SensedD_IN_WASH_PUMP_CONFIRMING ((uint8)8U)
#define SensedDra_IN_INITIAL_TIME_DRAIN ((uint8)2U)
#define SensedDrai_IN_DRAIN_AND_MONITOR ((uint8)1U)
#define SensedDrai_IN_MONITOR_NO_CHANGE ((uint8)1U)
#define SensedDrainM_IN_NO_ACTIVE_CHILD ((uint8)0U)
#define SensedDrainM_IN_START_WASH_PUMP ((uint8)1U)
#define SensedDrainModel_IN_EMPTY_DRAIN ((uint8)4U)
#define SensedDrainModel_IN_EXIT_STATE ((uint8)5U)
#define SensedDrainModel_IN_IDLE_STATE ((uint8)6U)
#define SensedDrainModel_IN_OFF        ((uint8)1U)
#define SensedDrainModel_IN_ON         ((uint8)2U)
#define SensedDrainModel_IN_RETRY_STATE ((uint8)7U)
#define SensedDrainModel_IN_VWI_STABLE ((uint8)4U)
#define SensedDrain_IN_START_DRAIN_PUMP ((uint8)3U)

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Exported data definition */

/* Definition for custom storage class: FileScope */
static SENSEDDRAIN_MODELDATA_TYPE SensedDrainData;
static void SensedDrain__Logic_Reset(self_SensedDrain__Logic_Se_TYPE
    *SensedDrainModel_self_arg);
static void SensedDrain__Logic(self_SensedDrain__Logic_Se_TYPE
    *SensedDrainModel_self_arg, const PROCESS_CONTROL_TYPE *Process_Control,
    HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE Drain_Pump_Type,
    BOOL_TYPE WashPump_Is_Stopped, BOOL_TYPE DrainPump_Is_Stopped, BOOL_TYPE
    WashPump_Reached_Target_Speed, BOOL_TYPE DrainPump_Reached_Target_Speed,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_RetryPause_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Initial_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_VwiStable_Timer_,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_NoChange_Timer_S,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_EmptyDrain_Timer_St,
    VWI_WATER_SENSE_TYPE Vwi_Status, uint8 Vwi_Heavy_Filt_Signal, const
    SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config, BOOL_TYPE
    Drain_Confirmed_In, PROCESS_RESULT_TYPE *SensedDrain_Result);
void SensedDrainModel__Execute(RT_MODEL_SensedDrainModel_TYPE *const
    SensedDrainModel_M, PROCESS_CONTROL_TYPE
    *SensedDrainModel_U_GLOBAL_Process_Control, SENSEDDRAIN_MOTORDATA_TYPE
    *SensedDrainModel_U_GLOBAL_Motor_Data, SENSEDDRAIN_TIMERSTATE_TYPE
    *SensedDrainModel_U_GLOBAL_Timer_States, SENSEDDRAIN_VWIINFO_TYPE
    *SensedDrainModel_U_GLOBAL_Vwi_Information, SF_SENSEDDRAIN_DATA_TYPE
    *SensedDrainModel_U_GLOBAL_Sf_SensedDrain_Config, BOOL_TYPE
    SensedDrainModel_U_GLOBAL_SensedDrain_Enabled, PROCESS_RESULT_TYPE
    *SensedDrainModel_Y_GLOBAL_SensedDrain_Result);

/* Forward declaration for local functions */
static uint8 Motor_Request_Decrement(uint8 motor_index, uint8 count);
static void EXIT_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE
                       Drain_Pump_Type, BOOL_TYPE WashPump_Is_Stopped, BOOL_TYPE
                       DrainPump_Is_Stopped, PROCESS_RESULT_TYPE
                       *SensedDrain_Result, DW_SensedDrain__Logic_Sens_TYPE
                       *localDW);
static void IDLE_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE
                       Drain_Pump_Type, DW_SensedDrain__Logic_Sens_TYPE *localDW);
static void CleanPreFault(DW_SensedDrain__Logic_Sens_TYPE *localDW);
static void enter_internal_EXIT_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type,
    HBL_PILOT_ENUM_TYPE Drain_Pump_Type, PROCESS_RESULT_TYPE *SensedDrain_Result,
    DW_SensedDrain__Logic_Sens_TYPE *localDW);
static void RETRY_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE
                        Drain_Pump_Type, BOOL_TYPE WashPump_Is_Stopped,
                        BOOL_TYPE DrainPump_Is_Stopped, SYSTEMTIMERS_STATE_TYPE
                        SensedDrain_RetryPause_Timer_St, const
                        SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config,
                        PROCESS_RESULT_TYPE *SensedDrain_Result,
                        DW_SensedDrain__Logic_Sens_TYPE *localDW);
static BOOL_TYPE VwiStabilityCheck(uint8 Vwi_Current, uint8 Vwi_Previous, const
    SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config);
static void enter_internal_RETRY_STATE(DW_SensedDrain__Logic_Sens_TYPE *localDW);
static void SENSED_DRAIN(const PROCESS_CONTROL_TYPE *Process_Control,
    HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE Drain_Pump_Type,
    BOOL_TYPE WashPump_Is_Stopped, BOOL_TYPE DrainPump_Is_Stopped, BOOL_TYPE
    WashPump_Reached_Target_Speed, BOOL_TYPE DrainPump_Reached_Target_Speed,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_RetryPause_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Initial_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_VwiStable_Timer_,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_NoChange_Timer_S,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_EmptyDrain_Timer_St,
    VWI_WATER_SENSE_TYPE Vwi_Status, uint8 Vwi_Heavy_Filt_Signal, const
    SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config, BOOL_TYPE
    Drain_Confirmed_In, PROCESS_RESULT_TYPE *SensedDrain_Result,
    DW_SensedDrain__Logic_Sens_TYPE *localDW);

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static uint8 Motor_Request_Decrement(uint8 motor_index, uint8 count)
{
    uint8 return_value;
    uint16 tmp;

    /* MATLAB Function 'Motor_Request_Decrement': '<S6>:980' */
    /* Graphical Function 'Motor_Request_Decrement': '<S6>:980' */
    /* '<S6>:982:1' */
    tmp = (uint16)(((uint16)count) + 1U);
    if (tmp > 255U)
    {
        tmp = 255U;
    }

    return_value = (uint8)tmp;

    /* '<S6>:991:1' */
    if (((sint16)((uint8)tmp)) == 10)
    {
        /* '<S6>:992:1' */
        return_value = 0U;

        /* '<S6>:992:1' */
        SensedDrain__RequestMotorStatus((const uint8)motor_index);
    }

    /* '<S6>:990:1' */
    return return_value;
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void EXIT_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE
                       Drain_Pump_Type, BOOL_TYPE WashPump_Is_Stopped, BOOL_TYPE
                       DrainPump_Is_Stopped, PROCESS_RESULT_TYPE
                       *SensedDrain_Result, DW_SensedDrain__Logic_Sens_TYPE
                       *localDW)
{
    /* During 'EXIT_STATE': '<S6>:746' */
    /* Transition: '<S6>:1110' */
    if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type ==
            HBL_PILOT_EXTERNAL_MOTOR))
    {
        /* Transition: '<S6>:1109' */
        /* Transition: '<S6>:748' */
        localDW->req_count = Motor_Request_Decrement(2, localDW->req_count);

        /*  Wash and Drain Motor */
        if (DrainPump_Is_Stopped && WashPump_Is_Stopped)
        {
            /* Transition: '<S6>:754' */
            /* Transition: '<S6>:755' */
            *SensedDrain_Result = PROCESS_COMPLETE;

            /* Transition: '<S6>:783' */
        }
        else
        {
            /* Transition: '<S6>:752' */
        }

        /* Transition: '<S6>:1113' */
    }
    else
    {
        /* Transition: '<S6>:1112' */
    }

    /* Transition: '<S6>:757' */
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void IDLE_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE
                       Drain_Pump_Type, DW_SensedDrain__Logic_Sens_TYPE *localDW)
{
    /* During 'IDLE_STATE': '<S6>:708' */
    /* Transition: '<S6>:718' */
    if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type ==
            HBL_PILOT_EXTERNAL_MOTOR))
    {
        /* Transition: '<S6>:720' */
        /* Transition: '<S6>:721' */
        localDW->is_SENSED_DRAIN = IN_DRAINING_MODE_PUMPS_IN_SERIE;

        /* Entry 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
        /*  Reset DM attempt timer */
        SensedDrain__SetTimerAsReset(DM_ATTEMPT_TIMER);

        /*  Set drain speed as default */
        SensedDrainData.Drain_Pump_Speed_Type = DM_SPEED_DEFAULT;

        /* Entry Internal 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
        /* Transition: '<S6>:727' */
        localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
            Sen_IN_WAIT_WASH_PUMP_RAMP_DOWN;

        /* Entry 'WAIT_WASH_PUMP_RAMP_DOWN': '<S6>:726' */
        localDW->req_count = 0U;
    }
    else
    {
        /* Transition: '<S6>:760' */
        localDW->is_SENSED_DRAIN = IN_DRAINING_AND_CONFIRMING_MODE;

        /* Entry 'DRAINING_AND_CONFIRMING_MODE_PUMPS_IN_PARALLEL_STATE': '<S6>:759' */
        SensedDrainData.Fault_Type = FAULT_ID_NOT_DRAINING;
        localDW->setPreFault_notDrain = FALSE;
        localDW->req_count = 0U;

        /*  Start both pumps */
        SensedDrainData.Wash_Pump_Mode = ON;
        SensedDrainData.Drain_Pump_Mode = ON;
    }
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void CleanPreFault(DW_SensedDrain__Logic_Sens_TYPE *localDW)
{
    BOOL_TYPE sf_internal_predicateOutput;

    /* MATLAB Function 'CleanPreFault': '<S6>:1066' */
    /* Graphical Function 'CleanPreFault': '<S6>:1066' */
    /*  This funciton has the purpose of cleaning a pre-fault of a different type when setting a Fault.  */
    /*  Not doing this results in a pre-fault remaining flaged after the algorithm is finished, causing troubles if the algorithm is run again */
    if ((SensedDrainData.Fault_Type == FAULT_ID_NOT_DRAINING) &&
            (localDW->preFaultWasSet_vwiIncons))
    {
        /* '<S6>:1077:1' */
        sf_internal_predicateOutput = TRUE;
    }
    else
    {
        sf_internal_predicateOutput = FALSE;
    }

    if (sf_internal_predicateOutput)
    {
        /* '<S6>:1079:1' */
        SensedDrain__ClearFault(FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY);
    }

    if ((SensedDrainData.Fault_Type == FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY) &&
        (localDW->preFaultWasSet_notDrain))
    {
        /* '<S6>:1082:1' */
        /* '<S6>:1082:2' */
        sf_internal_predicateOutput = TRUE;
    }
    else
    {
        sf_internal_predicateOutput = FALSE;
    }

    if (sf_internal_predicateOutput)
    {
        /* '<S6>:1084:1' */
        SensedDrain__ClearFault(FAULT_ID_NOT_DRAINING);
    }
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void enter_internal_EXIT_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type,
    HBL_PILOT_ENUM_TYPE Drain_Pump_Type, PROCESS_RESULT_TYPE *SensedDrain_Result,
    DW_SensedDrain__Logic_Sens_TYPE *localDW)
{
    /* Entry Internal 'EXIT_STATE': '<S6>:746' */
    /* Transition: '<S6>:1040' */
    if (localDW->sensed_drain_pass)
    {
        /* Transition: '<S6>:1042' */
        if (localDW->preFaultWasSet_notDrain)
        {
            /* Transition: '<S6>:1049' */
            /* Transition: '<S6>:1050' */
            SensedDrain__ClearFault(FAULT_ID_NOT_DRAINING);

            /* Transition: '<S6>:1051' */
        }
        else
        {
            /* Transition: '<S6>:1046' */
        }

        /* Transition: '<S6>:1047' */
        if (localDW->preFaultWasSet_vwiIncons)
        {
            /* Transition: '<S6>:1053' */
            /* Transition: '<S6>:1055' */
            SensedDrain__ClearFault(FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY);

            /* Transition: '<S6>:1057' */
        }
        else
        {
            /* Transition: '<S6>:1058' */
        }

        /* Transition: '<S6>:1062' */
    }
    else
    {
        /* Transition: '<S6>:1063' */
    }

    /* Transition: '<S6>:1061' */
    if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type ==
            HBL_PILOT_EXTERNAL_MOTOR))
    {
        /* Transition: '<S6>:1099' */
        /* Transition: '<S6>:1101' */
        /*  Turn off both pumps */
        SensedDrainData.Wash_Pump_Mode = OFF;
        SensedDrainData.Drain_Pump_Mode = OFF;
        localDW->req_count = 0U;

        /* Transition: '<S6>:1103' */
    }
    else
    {
        /* Transition: '<S6>:1104' */
        *SensedDrain_Result = PROCESS_COMPLETE;
    }

    /* Transition: '<S6>:1106' */
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void RETRY_STATE(HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE
                        Drain_Pump_Type, BOOL_TYPE WashPump_Is_Stopped,
                        BOOL_TYPE DrainPump_Is_Stopped, SYSTEMTIMERS_STATE_TYPE
                        SensedDrain_RetryPause_Timer_St, const
                        SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config,
                        PROCESS_RESULT_TYPE *SensedDrain_Result,
                        DW_SensedDrain__Logic_Sens_TYPE *localDW)
{
    /* During 'RETRY_STATE': '<S6>:784' */
    if (SensedDrainData.Attempt_Counter >=
            Sf_SensedDrain_Config->Max_Drain_Attempts)
    {
        /* Transition: '<S6>:828' */
        /* Transition Operation: */
        CleanPreFault(localDW);
        SensedDrain__SetSensedDrainStatus(FAIL);
        SensedDrain__SetFault(SensedDrainData.Fault_Type);
        localDW->sensed_drain_pass = FALSE;
        localDW->is_SENSED_DRAIN = SensedDrainModel_IN_EXIT_STATE;

        /* Entry 'EXIT_STATE': '<S6>:746' */
        enter_internal_EXIT_STATE(Wash_Pump_Type, Drain_Pump_Type,
            SensedDrain_Result, localDW);
    }
    else
    {
        /* Transition: '<S6>:786' */
        if (SensedDrain_RetryPause_Timer_St == SYSTEMTIMERS_STATE_DONE)
        {
            /* Transition: '<S6>:787' */
            if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type
                 == HBL_PILOT_EXTERNAL_MOTOR))
            {
                /* Transition: '<S6>:720' */
                /* Transition: '<S6>:721' */
                localDW->is_SENSED_DRAIN = IN_DRAINING_MODE_PUMPS_IN_SERIE;

                /* Entry 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
                /*  Reset DM attempt timer */
                SensedDrain__SetTimerAsReset(DM_ATTEMPT_TIMER);

                /*  Set drain speed as default */
                SensedDrainData.Drain_Pump_Speed_Type = DM_SPEED_DEFAULT;

                /* Entry Internal 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
                /* Transition: '<S6>:727' */
                localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                    Sen_IN_WAIT_WASH_PUMP_RAMP_DOWN;

                /* Entry 'WAIT_WASH_PUMP_RAMP_DOWN': '<S6>:726' */
                localDW->req_count = 0U;
            }
            else
            {
                /* Transition: '<S6>:760' */
                localDW->is_SENSED_DRAIN = IN_DRAINING_AND_CONFIRMING_MODE;

                /* Entry 'DRAINING_AND_CONFIRMING_MODE_PUMPS_IN_PARALLEL_STATE': '<S6>:759' */
                SensedDrainData.Fault_Type = FAULT_ID_NOT_DRAINING;
                localDW->setPreFault_notDrain = FALSE;
                localDW->req_count = 0U;

                /*  Start both pumps */
                SensedDrainData.Wash_Pump_Mode = ON;
                SensedDrainData.Drain_Pump_Mode = ON;
            }
        }
        else
        {
            /* Transition: '<S6>:792' */
            localDW->req_count = Motor_Request_Decrement(2, localDW->req_count);

            /*  Wash and Drain Motor */
            if ((DrainPump_Is_Stopped && WashPump_Is_Stopped) &&
                    (!localDW->timer_started))
            {
                /* Transition: '<S6>:797' */
                /*  Start counter when both motors ramped out */
                /* Transition: '<S6>:799' */
                SensedDrain__SetTimerAsStart(RETRYPAUSE_TIMER);
                localDW->timer_started = TRUE;

                /* Transition: '<S6>:801' */
            }
            else
            {
                /* Transition: '<S6>:800' */
            }

            /* Transition: '<S6>:803' */
        }
    }
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static BOOL_TYPE VwiStabilityCheck(uint8 Vwi_Current, uint8 Vwi_Previous, const
    SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config)
{
    BOOL_TYPE retval;
    uint8 error;
    uint16 qY;

    /* MATLAB Function 'VwiStabilityCheck': '<S6>:616' */
    /* Graphical Function 'VwiStabilityCheck': '<S6>:616' */
    /* '<S6>:640:1' */
    if (Vwi_Current >= Vwi_Previous)
    {
        /* '<S6>:641:1' */
        qY = (uint16)(((uint16)Vwi_Current) - /*MW:OvSatOk*/ ((uint16)
                       Vwi_Previous));
        if (qY > ((uint16)Vwi_Current))
        {
            qY = 0U;
        }

        error = (uint8)qY;
    }
    else
    {
        /* '<S6>:639:1' */
        qY = (uint16)(((uint16)Vwi_Previous) - /*MW:OvSatOk*/ ((uint16)
                       Vwi_Current));
        if (qY > ((uint16)Vwi_Previous))
        {
            qY = 0U;
        }

        error = (uint8)qY;
    }

    /* '<S6>:624:1' */
    if (error <= Sf_SensedDrain_Config->Vwi_Stable_Max_Difference)
    {
        /* '<S6>:629:1' */
        retval = TRUE;
    }
    else
    {
        /* '<S6>:627:1' */
        retval = FALSE;
    }

    /*  return */
    /* '<S6>:632:1' */
    return retval;
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void enter_internal_RETRY_STATE(DW_SensedDrain__Logic_Sens_TYPE *localDW)
{
    /* Entry Internal 'RETRY_STATE': '<S6>:784' */
    /* Transition: '<S6>:946' */
    if ((SensedDrainData.Fault_Type == FAULT_ID_NOT_DRAINING) &&
            (localDW->setPreFault_notDrain))
    {
        /* Transition: '<S6>:947' */
        /* Transition: '<S6>:943' */
        SensedDrain__SetFault(SensedDrainData.Fault_Type);
        localDW->setPreFault_notDrain = FALSE;
        localDW->preFaultWasSet_notDrain = TRUE;

        /* Transition: '<S6>:958' */
        /* Transition: '<S6>:941' */
    }
    else
    {
        /* Transition: '<S6>:945' */
        if ((SensedDrainData.Fault_Type ==
                FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY) &&
                (localDW->setPreFault_vwilncons))
        {
            /* Transition: '<S6>:955' */
            /* Transition: '<S6>:957' */
            SensedDrain__SetFault(SensedDrainData.Fault_Type);
            localDW->setPreFault_vwilncons = FALSE;
            localDW->preFaultWasSet_vwiIncons = TRUE;

            /* Transition: '<S6>:941' */
        }
        else
        {
            /* Transition: '<S6>:953' */
            /*  Do Nothing */
        }
    }

    /* Transition: '<S6>:942' */
}

/** Function for Chart: '<S5>/SensedDrain__Logic' */
static void SENSED_DRAIN(const PROCESS_CONTROL_TYPE *Process_Control,
    HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE Drain_Pump_Type,
    BOOL_TYPE WashPump_Is_Stopped, BOOL_TYPE DrainPump_Is_Stopped, BOOL_TYPE
    WashPump_Reached_Target_Speed, BOOL_TYPE DrainPump_Reached_Target_Speed,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_RetryPause_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Initial_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_VwiStable_Timer_,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_NoChange_Timer_S,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_EmptyDrain_Timer_St,
    VWI_WATER_SENSE_TYPE Vwi_Status, uint8 Vwi_Heavy_Filt_Signal, const
    SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config, BOOL_TYPE
    Drain_Confirmed_In, PROCESS_RESULT_TYPE *SensedDrain_Result,
    DW_SensedDrain__Logic_Sens_TYPE *localDW)
{
    uint16 tmp;

    /* During 'SENSED_DRAIN': '<S6>:689' */
    switch (localDW->is_SENSED_DRAIN)
    {
      case IN_CONFIRMING_MODE_PUMPS_IN_SER:
        /* During 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
        if ((Process_Control->Resume_From_Reset) ||
                (Process_Control->Resume_From_Pause))
        {
            /* Transition: '<S6>:771' */
            /* Exit Internal 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
            localDW->is_SENSED_DRAIN = IN_CONFIRMING_MODE_PUMPS_IN_SER;

            /* Entry 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
            SensedDrainData.Fault_Type = FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY;

            /* Entry Internal 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
            /* Transition: '<S6>:777' */
            localDW->is_CONFIRMING_MODE_PUMPS_IN_SER =
                Se_IN_WAIT_DRAIN_PUMP_RAMP_DOWN;

            /* Entry 'WAIT_DRAIN_PUMP_RAMP_DOWN': '<S6>:780' */
            localDW->req_count = 0U;
        }
        else if (((uint16)localDW->is_CONFIRMING_MODE_PUMPS_IN_SER) ==
                 SensedDrainM_IN_START_WASH_PUMP)
        {
            /* During 'START_WASH_PUMP': '<S6>:779' */
            /* Transition: '<S6>:818' */
            if (WashPump_Reached_Target_Speed)
            {
                /* Transition: '<S6>:820' */
                /* Transition: '<S6>:821' */
                localDW->is_CONFIRMING_MODE_PUMPS_IN_SER =
                    SensedDrainM_IN_NO_ACTIVE_CHILD;
                localDW->is_SENSED_DRAIN = SensedD_IN_WASH_PUMP_CONFIRMING;

                /* Entry 'WASH_PUMP_CONFIRMING': '<S6>:810' */
                SensedDrain__SetTimerAsStart(WM_ATTEMPT_TIMER);

                /*  Reset all timers used by this module */
                SensedDrain__SetTimerAsReset(WM_NOCHANGE_TIMER);

                /* Entry Internal 'WASH_PUMP_CONFIRMING': '<S6>:810' */
                /* Transition: '<S6>:823' */
                localDW->is_WASH_PUMP_CONFIRMING =
                    IN_START_PUMPS_AND_WAIT_FOR_STA;

                /* Entry 'START_PUMPS_AND_WAIT_FOR_STABILITY': '<S6>:822' */
                SensedDrainData.Vwi_Previous = Vwi_Heavy_Filt_Signal;
                SensedDrainData.Vwi_Stability_Status = FALSE;
            }
            else
            {
                localDW->req_count = Motor_Request_Decrement(0,
                    localDW->req_count);

                /*  Wash Motor */
            }
        }
        else
        {
            /* During 'WAIT_DRAIN_PUMP_RAMP_DOWN': '<S6>:780' */
            if (DrainPump_Is_Stopped)
            {
                /* Transition: '<S6>:778' */
                localDW->is_CONFIRMING_MODE_PUMPS_IN_SER =
                    SensedDrainM_IN_START_WASH_PUMP;

                /* Entry 'START_WASH_PUMP': '<S6>:779' */
                localDW->req_count = 0U;
                SensedDrainData.Wash_Pump_Mode = ON;
            }
            else
            {
                localDW->req_count = Motor_Request_Decrement(1,
                    localDW->req_count);

                /*  Drain Motor */
            }
        }
        break;

      case IN_DRAINING_AND_CONFIRMING_MODE:
        /* During 'DRAINING_AND_CONFIRMING_MODE_PUMPS_IN_PARALLEL_STATE': '<S6>:759' */
        if ((Process_Control->Resume_From_Reset) ||
                (Process_Control->Resume_From_Pause))
        {
            /* Transition: '<S6>:763' */
            localDW->is_SENSED_DRAIN = IN_DRAINING_AND_CONFIRMING_MODE;

            /* Entry 'DRAINING_AND_CONFIRMING_MODE_PUMPS_IN_PARALLEL_STATE': '<S6>:759' */
            SensedDrainData.Fault_Type = FAULT_ID_NOT_DRAINING;
            localDW->setPreFault_notDrain = FALSE;
            localDW->req_count = 0U;

            /*  Start both pumps */
            SensedDrainData.Wash_Pump_Mode = ON;
            SensedDrainData.Drain_Pump_Mode = ON;
        }
        else if (WashPump_Reached_Target_Speed && DrainPump_Reached_Target_Speed)
        {
            /* Transition: '<S6>:813' */
            /* Transition: '<S6>:814' */
            localDW->is_SENSED_DRAIN = SensedD_IN_WASH_PUMP_CONFIRMING;

            /* Entry 'WASH_PUMP_CONFIRMING': '<S6>:810' */
            SensedDrain__SetTimerAsStart(WM_ATTEMPT_TIMER);

            /*  Reset all timers used by this module */
            SensedDrain__SetTimerAsReset(WM_NOCHANGE_TIMER);

            /* Entry Internal 'WASH_PUMP_CONFIRMING': '<S6>:810' */
            /* Transition: '<S6>:823' */
            localDW->is_WASH_PUMP_CONFIRMING = IN_START_PUMPS_AND_WAIT_FOR_STA;

            /* Entry 'START_PUMPS_AND_WAIT_FOR_STABILITY': '<S6>:822' */
            SensedDrainData.Vwi_Previous = Vwi_Heavy_Filt_Signal;
            SensedDrainData.Vwi_Stability_Status = FALSE;
        }
        else
        {
            localDW->req_count = Motor_Request_Decrement(2, localDW->req_count);

            /*  Wash and Drain Motor */
        }
        break;

      case IN_DRAINING_MODE_PUMPS_IN_SERIE:
        /* During 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
        if ((Process_Control->Resume_From_Reset) ||
                (Process_Control->Resume_From_Pause))
        {
            /* Transition: '<S6>:735' */
            /* Exit Internal 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
            if (((uint16)localDW->is_DRAINING_MODE_PUMPS_IN_SERIE) ==
                    SensedDra_IN_INITIAL_TIME_DRAIN)
            {
                /* Exit 'INITIAL_TIME_DRAIN': '<S6>:732' */
                SensedDrain__SetTimerAsStart(DM_ATTEMPT_TIMER);
                localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                    SensedDrainM_IN_NO_ACTIVE_CHILD;
            }
            else
            {
                localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                    SensedDrainM_IN_NO_ACTIVE_CHILD;
            }

            localDW->is_SENSED_DRAIN = IN_DRAINING_MODE_PUMPS_IN_SERIE;

            /* Entry 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
            /*  Reset DM attempt timer */
            SensedDrain__SetTimerAsReset(DM_ATTEMPT_TIMER);

            /*  Set drain speed as default */
            SensedDrainData.Drain_Pump_Speed_Type = DM_SPEED_DEFAULT;

            /* Entry Internal 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
            /* Transition: '<S6>:727' */
            localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                Sen_IN_WAIT_WASH_PUMP_RAMP_DOWN;

            /* Entry 'WAIT_WASH_PUMP_RAMP_DOWN': '<S6>:726' */
            localDW->req_count = 0U;
        }
        else
        {
            /* Transition: '<S6>:806' */
            if (SensedDrain_DM_Attempt_Timer_St == SYSTEMTIMERS_STATE_DONE)
            {
                /* Transition: '<S6>:808' */
                SensedDrainData.Fault_Type = FAULT_ID_NOT_DRAINING;

                /* Transition: '<S6>:809' */
                /* Exit Internal 'DRAINING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:713' */
                if (((uint16)localDW->is_DRAINING_MODE_PUMPS_IN_SERIE) ==
                        SensedDra_IN_INITIAL_TIME_DRAIN)
                {
                    /* Exit 'INITIAL_TIME_DRAIN': '<S6>:732' */
                    SensedDrain__SetTimerAsStart(DM_ATTEMPT_TIMER);
                    localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                        SensedDrainM_IN_NO_ACTIVE_CHILD;
                }
                else
                {
                    localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                        SensedDrainM_IN_NO_ACTIVE_CHILD;
                }

                localDW->is_SENSED_DRAIN = SensedDrainModel_IN_RETRY_STATE;

                /* Entry 'RETRY_STATE': '<S6>:784' */
                /*  Turn off both pumps */
                SensedDrainData.Wash_Pump_Mode = OFF;
                SensedDrainData.Drain_Pump_Mode = OFF;
                localDW->req_count = 0U;

                /*  Reset timer flag and timer */
                localDW->timer_started = FALSE;
                SensedDrain__SetTimerAsReset(RETRYPAUSE_TIMER);

                /*  Increase counter */
                tmp = (uint16)(((uint16)SensedDrainData.Attempt_Counter) + 1U);
                if (tmp > 255U)
                {
                    tmp = 255U;
                }

                SensedDrainData.Attempt_Counter = (uint8)tmp;
                enter_internal_RETRY_STATE(localDW);
            }
            else
            {
                switch (localDW->is_DRAINING_MODE_PUMPS_IN_SERIE)
                {
                  case SensedDrai_IN_DRAIN_AND_MONITOR:
                    /* During 'DRAIN_AND_MONITOR': '<S6>:737' */
                    if (Vwi_Status == WS_NO_WATER)
                    {
                        /* Transition: '<S6>:741' */
                        localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                            SensedDrainModel_IN_VWI_STABLE;

                        /* Entry 'VWI_STABLE': '<S6>:739' */
                        SensedDrain__SetTimerAsStart(DM_VWISTABLE_TIMER);
                    }
                    break;

                  case SensedDra_IN_INITIAL_TIME_DRAIN:
                    /* During 'INITIAL_TIME_DRAIN': '<S6>:732' */
                    if (SensedDrain_DM_Initial_Timer_St ==
                            SYSTEMTIMERS_STATE_DONE)
                    {
                        /* Transition: '<S6>:738' */
                        /* Exit 'INITIAL_TIME_DRAIN': '<S6>:732' */
                        SensedDrain__SetTimerAsStart(DM_ATTEMPT_TIMER);
                        localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                            SensedDrai_IN_DRAIN_AND_MONITOR;

                        /* Entry 'DRAIN_AND_MONITOR': '<S6>:737' */
                        SensedDrain__SetTimerAsReset(DM_VWISTABLE_TIMER);
                    }
                    break;

                  case SensedDrain_IN_START_DRAIN_PUMP:
                    /* During 'START_DRAIN_PUMP': '<S6>:730' */
                    if (DrainPump_Reached_Target_Speed)
                    {
                        /* Transition: '<S6>:733' */
                        localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                            SensedDra_IN_INITIAL_TIME_DRAIN;

                        /* Entry 'INITIAL_TIME_DRAIN': '<S6>:732' */
                        SensedDrain__SetTimerAsStart(DM_INITIAL_TIMER);
                    }
                    else
                    {
                        localDW->req_count = Motor_Request_Decrement(1,
                            localDW->req_count);

                        /*  Drain Motor */
                    }
                    break;

                  case SensedDrainModel_IN_VWI_STABLE:
                    /* During 'VWI_STABLE': '<S6>:739' */
                    if (Vwi_Status != WS_NO_WATER)
                    {
                        /* Transition: '<S6>:742' */
                        localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                            SensedDrai_IN_DRAIN_AND_MONITOR;

                        /* Entry 'DRAIN_AND_MONITOR': '<S6>:737' */
                        SensedDrain__SetTimerAsReset(DM_VWISTABLE_TIMER);
                    }
                    else
                    {
                        if (SensedDrain_DM_VwiStable_Timer_ ==
                                SYSTEMTIMERS_STATE_DONE)
                        {
                            /* Transition: '<S6>:744' */
                            localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                                SensedDrainM_IN_NO_ACTIVE_CHILD;
                            localDW->is_SENSED_DRAIN =
                                SensedDrainModel_IN_EMPTY_DRAIN;

                            /* Entry 'EMPTY_DRAIN': '<S6>:745' */
                            /*  Set drain speed as empty */
                            SensedDrainData.Drain_Pump_Speed_Type =
                                DM_SPEED_EMPTY;

                            /*  Start timer */
                            SensedDrain__SetTimerAsStart(EMPTYDRAIN_TIMER);
                        }
                    }
                    break;

                  default:
                    /* During 'WAIT_WASH_PUMP_RAMP_DOWN': '<S6>:726' */
                    if (WashPump_Is_Stopped)
                    {
                        /* Transition: '<S6>:731' */
                        localDW->is_DRAINING_MODE_PUMPS_IN_SERIE =
                            SensedDrain_IN_START_DRAIN_PUMP;

                        /* Entry 'START_DRAIN_PUMP': '<S6>:730' */
                        SensedDrainData.Drain_Pump_Mode = ON;
                        localDW->req_count = 0U;

                        /*  Reset initial drain timer */
                        SensedDrain__SetTimerAsReset(DM_INITIAL_TIMER);
                    }
                    else
                    {
                        localDW->req_count = Motor_Request_Decrement(0,
                            localDW->req_count);

                        /*  Wash Motor */
                    }
                    break;
                }
            }
        }
        break;

      case SensedDrainModel_IN_EMPTY_DRAIN:
        /* During 'EMPTY_DRAIN': '<S6>:745' */
        if ((SensedDrain_EmptyDrain_Timer_St == SYSTEMTIMERS_STATE_DONE) &&
                Drain_Confirmed_In)
        {
            /* Transition: '<S6>:758' */
            /* Transition Operation: */
            SensedDrain__SetSensedDrainStatus(PASS);
            localDW->sensed_drain_pass = TRUE;

            /* Exit 'EMPTY_DRAIN': '<S6>:745' */
            SensedDrainData.Drain_Pump_Mode = OFF;
            localDW->is_SENSED_DRAIN = SensedDrainModel_IN_EXIT_STATE;

            /* Entry 'EXIT_STATE': '<S6>:746' */
            enter_internal_EXIT_STATE(Wash_Pump_Type, Drain_Pump_Type,
                SensedDrain_Result, localDW);
        }
        else
        {
            if ((SensedDrain_EmptyDrain_Timer_St == SYSTEMTIMERS_STATE_DONE) &&
                    (!Drain_Confirmed_In))
            {
                /* Transition: '<S6>:770' */
                /* Exit 'EMPTY_DRAIN': '<S6>:745' */
                SensedDrainData.Drain_Pump_Mode = OFF;
                localDW->is_SENSED_DRAIN = IN_CONFIRMING_MODE_PUMPS_IN_SER;

                /* Entry 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
                SensedDrainData.Fault_Type =
                    FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY;

                /* Entry Internal 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
                /* Transition: '<S6>:777' */
                localDW->is_CONFIRMING_MODE_PUMPS_IN_SER =
                    Se_IN_WAIT_DRAIN_PUMP_RAMP_DOWN;

                /* Entry 'WAIT_DRAIN_PUMP_RAMP_DOWN': '<S6>:780' */
                localDW->req_count = 0U;
            }
        }
        break;

      case SensedDrainModel_IN_EXIT_STATE:
        EXIT_STATE(Wash_Pump_Type, Drain_Pump_Type, WashPump_Is_Stopped,
                   DrainPump_Is_Stopped, SensedDrain_Result, localDW);
        break;

      case SensedDrainModel_IN_IDLE_STATE:
        IDLE_STATE(Wash_Pump_Type, Drain_Pump_Type, localDW);
        break;

      case SensedDrainModel_IN_RETRY_STATE:
        RETRY_STATE(Wash_Pump_Type, Drain_Pump_Type, WashPump_Is_Stopped,
                    DrainPump_Is_Stopped, SensedDrain_RetryPause_Timer_St,
                    Sf_SensedDrain_Config, SensedDrain_Result, localDW);
        break;

      default:
        /* During 'WASH_PUMP_CONFIRMING': '<S6>:810' */
        if ((Process_Control->Resume_From_Reset) ||
                (Process_Control->Resume_From_Pause))
        {
            /* Transition: '<S6>:811' */
            if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type
                 == HBL_PILOT_EXTERNAL_MOTOR))
            {
                /* Transition: '<S6>:765' */
                /* Exit Internal 'WASH_PUMP_CONFIRMING': '<S6>:810' */
                localDW->is_WASH_PUMP_CONFIRMING =
                    SensedDrainM_IN_NO_ACTIVE_CHILD;
                localDW->is_SENSED_DRAIN = IN_CONFIRMING_MODE_PUMPS_IN_SER;

                /* Entry 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
                SensedDrainData.Fault_Type =
                    FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY;

                /* Entry Internal 'CONFIRMING_MODE_PUMPS_IN_SERIES_STATE': '<S6>:764' */
                /* Transition: '<S6>:777' */
                localDW->is_CONFIRMING_MODE_PUMPS_IN_SER =
                    Se_IN_WAIT_DRAIN_PUMP_RAMP_DOWN;

                /* Entry 'WAIT_DRAIN_PUMP_RAMP_DOWN': '<S6>:780' */
                localDW->req_count = 0U;
            }
            else
            {
                /* Transition: '<S6>:767' */
                /* Exit Internal 'WASH_PUMP_CONFIRMING': '<S6>:810' */
                localDW->is_WASH_PUMP_CONFIRMING =
                    SensedDrainM_IN_NO_ACTIVE_CHILD;
                localDW->is_SENSED_DRAIN = IN_DRAINING_AND_CONFIRMING_MODE;

                /* Entry 'DRAINING_AND_CONFIRMING_MODE_PUMPS_IN_PARALLEL_STATE': '<S6>:759' */
                SensedDrainData.Fault_Type = FAULT_ID_NOT_DRAINING;
                localDW->setPreFault_notDrain = FALSE;
                localDW->req_count = 0U;

                /*  Start both pumps */
                SensedDrainData.Wash_Pump_Mode = ON;
                SensedDrainData.Drain_Pump_Mode = ON;
            }
        }
        else if (SensedDrain_WM_Attempt_Timer_St == SYSTEMTIMERS_STATE_DONE)
        {
            /* Transition: '<S6>:827' */
            /* Exit Internal 'WASH_PUMP_CONFIRMING': '<S6>:810' */
            localDW->is_WASH_PUMP_CONFIRMING = SensedDrainM_IN_NO_ACTIVE_CHILD;
            localDW->is_SENSED_DRAIN = SensedDrainModel_IN_RETRY_STATE;

            /* Entry 'RETRY_STATE': '<S6>:784' */
            /*  Turn off both pumps */
            SensedDrainData.Wash_Pump_Mode = OFF;
            SensedDrainData.Drain_Pump_Mode = OFF;
            localDW->req_count = 0U;

            /*  Reset timer flag and timer */
            localDW->timer_started = FALSE;
            SensedDrain__SetTimerAsReset(RETRYPAUSE_TIMER);

            /*  Increase counter */
            tmp = (uint16)(((uint16)SensedDrainData.Attempt_Counter) + 1U);
            if (tmp > 255U)
            {
                tmp = 255U;
            }

            SensedDrainData.Attempt_Counter = (uint8)tmp;
            enter_internal_RETRY_STATE(localDW);
        }
        else if (((uint16)localDW->is_WASH_PUMP_CONFIRMING) ==
                 SensedDrai_IN_MONITOR_NO_CHANGE)
        {
            /* During 'MONITOR_NO_CHANGE': '<S6>:824' */
            if (!SensedDrainData.Vwi_Stability_Status)
            {
                /* Transition: '<S6>:826' */
                localDW->is_WASH_PUMP_CONFIRMING =
                    IN_START_PUMPS_AND_WAIT_FOR_STA;

                /* Entry 'START_PUMPS_AND_WAIT_FOR_STABILITY': '<S6>:822' */
                SensedDrainData.Vwi_Previous = Vwi_Heavy_Filt_Signal;
                SensedDrainData.Vwi_Stability_Status = FALSE;
            }
            else if (SensedDrain_WM_NoChange_Timer_S == SYSTEMTIMERS_STATE_DONE)
            {
                /* Transition: '<S6>:830' */
                if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) &&
                        (Drain_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR))
                {
                    /* Transition: '<S6>:838' */
                    /* Transition: '<S6>:837' */
                    SensedDrain__SetWaterInformation((const uint8)0U);

                    /* Transition: '<S6>:839' */
                }
                else
                {
                    /* Transition: '<S6>:835' */
                }

                /* Transition: '<S6>:847' */
                /* Transition Operation: */
                SensedDrain__SetSensedDrainStatus(PASS);
                SensedDrain__SetDrainConfirmed(TRUE);
                localDW->sensed_drain_pass = TRUE;
                localDW->is_WASH_PUMP_CONFIRMING =
                    SensedDrainM_IN_NO_ACTIVE_CHILD;
                localDW->is_SENSED_DRAIN = SensedDrainModel_IN_EXIT_STATE;

                /* Entry 'EXIT_STATE': '<S6>:746' */
                enter_internal_EXIT_STATE(Wash_Pump_Type, Drain_Pump_Type,
                    SensedDrain_Result, localDW);
            }
            else
            {
                SensedDrainData.Vwi_Stability_Status = VwiStabilityCheck
                    (Vwi_Heavy_Filt_Signal, SensedDrainData.Vwi_Previous,
                     Sf_SensedDrain_Config);
            }
        }
        else
        {
            /* During 'START_PUMPS_AND_WAIT_FOR_STABILITY': '<S6>:822' */
            if ((SensedDrainData.Vwi_Stability_Status) && (Vwi_Heavy_Filt_Signal
                 < Sf_SensedDrain_Config->Vwi_Drained_Threshold))
            {
                /* Transition: '<S6>:825' */
                localDW->is_WASH_PUMP_CONFIRMING =
                    SensedDrai_IN_MONITOR_NO_CHANGE;

                /* Entry 'MONITOR_NO_CHANGE': '<S6>:824' */
                SensedDrain__SetTimerAsStart(WM_NOCHANGE_TIMER);
            }
            else
            {
                SensedDrainData.Vwi_Stability_Status = VwiStabilityCheck
                    (Vwi_Heavy_Filt_Signal, SensedDrainData.Vwi_Previous,
                     Sf_SensedDrain_Config);
                SensedDrainData.Vwi_Previous = Vwi_Heavy_Filt_Signal;
            }
        }
        break;
    }
}

/** System reset for atomic system: '<S5>/SensedDrain__Logic' */
static void SensedDrain__Logic_Reset(self_SensedDrain__Logic_Se_TYPE
    *SensedDrainModel_self_arg)
{
    SensedDrainModel_self_arg->dwork.is_DRAIN_PUMP =
        SensedDrainM_IN_NO_ACTIVE_CHILD;
    SensedDrainModel_self_arg->dwork.is_WASH_PUMP =
        SensedDrainM_IN_NO_ACTIVE_CHILD;
    SensedDrainModel_self_arg->dwork.is_SENSED_DRAIN =
        SensedDrainM_IN_NO_ACTIVE_CHILD;
    SensedDrainModel_self_arg->dwork.is_CONFIRMING_MODE_PUMPS_IN_SER =
        SensedDrainM_IN_NO_ACTIVE_CHILD;
    SensedDrainModel_self_arg->dwork.is_DRAINING_MODE_PUMPS_IN_SERIE =
        SensedDrainM_IN_NO_ACTIVE_CHILD;
    SensedDrainModel_self_arg->dwork.is_WASH_PUMP_CONFIRMING =
        SensedDrainM_IN_NO_ACTIVE_CHILD;
    SensedDrainModel_self_arg->dwork.is_active_c5_SensedDrainModel = 0U;
}

/** Output and update for atomic system: '<S5>/SensedDrain__Logic' */
static void SensedDrain__Logic(self_SensedDrain__Logic_Se_TYPE
    *SensedDrainModel_self_arg, const PROCESS_CONTROL_TYPE *Process_Control,
    HBL_PILOT_ENUM_TYPE Wash_Pump_Type, HBL_PILOT_ENUM_TYPE Drain_Pump_Type,
    BOOL_TYPE WashPump_Is_Stopped, BOOL_TYPE DrainPump_Is_Stopped, BOOL_TYPE
    WashPump_Reached_Target_Speed, BOOL_TYPE DrainPump_Reached_Target_Speed,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_RetryPause_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Initial_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_VwiStable_Timer_,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_NoChange_Timer_S,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_Attempt_Timer_St,
    SYSTEMTIMERS_STATE_TYPE SensedDrain_EmptyDrain_Timer_St,
    VWI_WATER_SENSE_TYPE Vwi_Status, uint8 Vwi_Heavy_Filt_Signal, const
    SF_SENSEDDRAIN_DATA_TYPE *Sf_SensedDrain_Config, BOOL_TYPE
    Drain_Confirmed_In, PROCESS_RESULT_TYPE *SensedDrain_Result)
{
    /* Chart: '<S5>/SensedDrain__Logic' */
    /* Gateway: SD_Process/Subsystem/SensedDrain__Logic */
    /* During: SD_Process/Subsystem/SensedDrain__Logic */
    if (((uint16)SensedDrainModel_self_arg->dwork.is_active_c5_SensedDrainModel)
        == 0U)
    {
        /* Entry: SD_Process/Subsystem/SensedDrain__Logic */
        SensedDrainModel_self_arg->dwork.is_active_c5_SensedDrainModel = 1U;

        /* Entry Internal: SD_Process/Subsystem/SensedDrain__Logic */
        /* Entry Internal 'SENSED_DRAIN': '<S6>:689' */
        /* Transition: '<S6>:977' */
        SensedDrainModel_self_arg->dwork.is_SENSED_DRAIN =
            SensedDrainModel_IN_IDLE_STATE;

        /* Entry 'IDLE_STATE': '<S6>:708' */
        /*  Set process as busy */
        *SensedDrain_Result = PROCESS_BUSY;

        /*  Reset internal variables */
        SensedDrainData.Attempt_Counter = 0;
        SensedDrainModel_self_arg->dwork.setPreFault_notDrain = TRUE;
        SensedDrainModel_self_arg->dwork.setPreFault_vwilncons = TRUE;
        SensedDrainModel_self_arg->dwork.preFaultWasSet_notDrain = FALSE;
        SensedDrainModel_self_arg->dwork.preFaultWasSet_vwiIncons = FALSE;

        /* Entry Internal 'IDLE_STATE': '<S6>:708' */
        /* Transition: '<S6>:1115' */
        if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type ==
                HBL_PILOT_EXTERNAL_MOTOR))
        {
            /* Transition: '<S6>:1117' */
            /* Transition: '<S6>:1121' */
            /*  Start with both pumps OFF */
            SensedDrainData.Wash_Pump_Mode = OFF;
            SensedDrainData.Drain_Pump_Mode = OFF;

            /* Transition: '<S6>:1122' */
        }
        else
        {
            /* Transition: '<S6>:1119' */
            /*  Start with both pumps ON */
            SensedDrainData.Wash_Pump_Mode = ON;
            SensedDrainData.Drain_Pump_Mode = ON;
        }

        /* Transition: '<S6>:1124' */
        /* Entry Internal 'PUMP_CONTROL': '<S6>:643' */
        /* Entry Internal 'WASH_PUMP': '<S6>:644' */
        /* Transition: '<S6>:962' */
        if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type ==
                HBL_PILOT_EXTERNAL_MOTOR))
        {
            /* Transition: '<S6>:1130' */
            /* Transition: '<S6>:1131' */
            SensedDrainModel_self_arg->dwork.is_WASH_PUMP =
                SensedDrainModel_IN_OFF;

            /* Entry 'OFF': '<S6>:649' */
            SensedDrain__SetWashMotorState(OFF);
        }
        else
        {
            /* Transition: '<S6>:1127' */
            /* Transition: '<S6>:1128' */
            SensedDrainModel_self_arg->dwork.is_WASH_PUMP =
                SensedDrainModel_IN_ON;

            /* Entry 'ON': '<S6>:647' */
            SensedDrain__SetWashMotorState(ON);
        }

        /* Entry Internal 'DRAIN_PUMP': '<S6>:664' */
        /* Transition: '<S6>:1133' */
        if ((Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) && (Drain_Pump_Type ==
                HBL_PILOT_EXTERNAL_MOTOR))
        {
            /* Transition: '<S6>:1135' */
            SensedDrainModel_self_arg->dwork.is_DRAIN_PUMP =
                SensedDrainModel_IN_OFF;

            /* Entry 'OFF': '<S6>:861' */
            /* Entry Internal 'OFF': '<S6>:861' */
            /* Transition: '<S6>:1094' */
            if (SensedDrainData.Drain_Pump_Speed_Type == DM_SPEED_EMPTY)
            {
                /* Transition: '<S6>:868' */
                /* Transition: '<S6>:870' */
                SensedDrain__SetDrainMotorEmptySpeedState(OFF);

                /* Transition: '<S6>:863' */
            }
            else
            {
                /* Transition: '<S6>:864' */
                SensedDrain__SetDrainMotorDefaultSpeedState(OFF);
            }

            /* Transition: '<S6>:865' */
        }
        else
        {
            /* Transition: '<S6>:1137' */
            /* Transition: '<S6>:1138' */
            SensedDrainModel_self_arg->dwork.is_DRAIN_PUMP =
                SensedDrainModel_IN_ON;

            /* Entry 'ON': '<S6>:668' */
            /* Entry Internal 'ON': '<S6>:668' */
            /* Transition: '<S6>:1095' */
            if (SensedDrainData.Drain_Pump_Speed_Type == DM_SPEED_EMPTY)
            {
                /* Transition: '<S6>:855' */
                /* Transition: '<S6>:856' */
                SensedDrain__SetDrainMotorEmptySpeedState(ON);

                /* Transition: '<S6>:858' */
            }
            else
            {
                /* Transition: '<S6>:857' */
                SensedDrain__SetDrainMotorDefaultSpeedState(ON);
            }

            /* Transition: '<S6>:859' */
        }
    }
    else
    {
        SENSED_DRAIN(Process_Control, Wash_Pump_Type, Drain_Pump_Type,
                     WashPump_Is_Stopped, DrainPump_Is_Stopped,
                     WashPump_Reached_Target_Speed,
                     DrainPump_Reached_Target_Speed,
                     SensedDrain_RetryPause_Timer_St,
                     SensedDrain_DM_Initial_Timer_St,
                     SensedDrain_DM_VwiStable_Timer_,
                     SensedDrain_WM_NoChange_Timer_S,
                     SensedDrain_DM_Attempt_Timer_St,
                     SensedDrain_WM_Attempt_Timer_St,
                     SensedDrain_EmptyDrain_Timer_St, Vwi_Status,
                     Vwi_Heavy_Filt_Signal, Sf_SensedDrain_Config,
                     Drain_Confirmed_In, SensedDrain_Result,
                     &SensedDrainModel_self_arg->dwork);

        /* During 'PUMP_CONTROL': '<S6>:643' */
        /* During 'WASH_PUMP': '<S6>:644' */
        if (((uint16)SensedDrainModel_self_arg->dwork.is_WASH_PUMP) ==
                SensedDrainModel_IN_OFF)
        {
            /* During 'OFF': '<S6>:649' */
            if (SensedDrainData.Wash_Pump_Mode == ON)
            {
                /* Transition: '<S6>:651' */
                SensedDrainModel_self_arg->dwork.is_WASH_PUMP =
                    SensedDrainModel_IN_ON;

                /* Entry 'ON': '<S6>:647' */
                SensedDrain__SetWashMotorState(ON);
            }
            else
            {
                SensedDrain__SetWashMotorState(OFF);
            }
        }
        else
        {
            /* During 'ON': '<S6>:647' */
            if (SensedDrainData.Wash_Pump_Mode == OFF)
            {
                /* Transition: '<S6>:650' */
                SensedDrainModel_self_arg->dwork.is_WASH_PUMP =
                    SensedDrainModel_IN_OFF;

                /* Entry 'OFF': '<S6>:649' */
                SensedDrain__SetWashMotorState(OFF);
            }
            else
            {
                SensedDrain__SetWashMotorState(ON);
            }
        }

        /* During 'DRAIN_PUMP': '<S6>:664' */
        if (((uint16)SensedDrainModel_self_arg->dwork.is_DRAIN_PUMP) ==
                SensedDrainModel_IN_OFF)
        {
            /* During 'OFF': '<S6>:861' */
            if (SensedDrainData.Drain_Pump_Mode == ON)
            {
                /* Transition: '<S6>:667' */
                SensedDrainModel_self_arg->dwork.is_DRAIN_PUMP =
                    SensedDrainModel_IN_ON;

                /* Entry 'ON': '<S6>:668' */
                /* Entry Internal 'ON': '<S6>:668' */
                /* Transition: '<S6>:1095' */
                if (SensedDrainData.Drain_Pump_Speed_Type == DM_SPEED_EMPTY)
                {
                    /* Transition: '<S6>:855' */
                    /* Transition: '<S6>:856' */
                    SensedDrain__SetDrainMotorEmptySpeedState(ON);

                    /* Transition: '<S6>:858' */
                }
                else
                {
                    /* Transition: '<S6>:857' */
                    SensedDrain__SetDrainMotorDefaultSpeedState(ON);
                }

                /* Transition: '<S6>:859' */
            }
            else
            {
                /* Transition: '<S6>:1094' */
                if (SensedDrainData.Drain_Pump_Speed_Type == DM_SPEED_EMPTY)
                {
                    /* Transition: '<S6>:868' */
                    /* Transition: '<S6>:870' */
                    SensedDrain__SetDrainMotorEmptySpeedState(OFF);

                    /* Transition: '<S6>:863' */
                }
                else
                {
                    /* Transition: '<S6>:864' */
                    SensedDrain__SetDrainMotorDefaultSpeedState(OFF);
                }

                /* Transition: '<S6>:865' */
            }
        }
        else
        {
            /* During 'ON': '<S6>:668' */
            if (SensedDrainData.Drain_Pump_Mode == OFF)
            {
                /* Transition: '<S6>:666' */
                SensedDrainModel_self_arg->dwork.is_DRAIN_PUMP =
                    SensedDrainModel_IN_OFF;

                /* Entry 'OFF': '<S6>:861' */
                /* Entry Internal 'OFF': '<S6>:861' */
                /* Transition: '<S6>:1094' */
                if (SensedDrainData.Drain_Pump_Speed_Type == DM_SPEED_EMPTY)
                {
                    /* Transition: '<S6>:868' */
                    /* Transition: '<S6>:870' */
                    SensedDrain__SetDrainMotorEmptySpeedState(OFF);

                    /* Transition: '<S6>:863' */
                }
                else
                {
                    /* Transition: '<S6>:864' */
                    SensedDrain__SetDrainMotorDefaultSpeedState(OFF);
                }

                /* Transition: '<S6>:865' */
            }
            else
            {
                /* Transition: '<S6>:1095' */
                if (SensedDrainData.Drain_Pump_Speed_Type == DM_SPEED_EMPTY)
                {
                    /* Transition: '<S6>:855' */
                    /* Transition: '<S6>:856' */
                    SensedDrain__SetDrainMotorEmptySpeedState(ON);

                    /* Transition: '<S6>:858' */
                }
                else
                {
                    /* Transition: '<S6>:857' */
                    SensedDrain__SetDrainMotorDefaultSpeedState(ON);
                }

                /* Transition: '<S6>:859' */
            }
        }
    }

    /* End of Chart: '<S5>/SensedDrain__Logic' */
}

/** Model step function for TID1 */
void SensedDrainModel__Execute(RT_MODEL_SensedDrainModel_TYPE *const
    SensedDrainModel_M, PROCESS_CONTROL_TYPE
    *SensedDrainModel_U_GLOBAL_Process_Control, SENSEDDRAIN_MOTORDATA_TYPE
    *SensedDrainModel_U_GLOBAL_Motor_Data, SENSEDDRAIN_TIMERSTATE_TYPE
    *SensedDrainModel_U_GLOBAL_Timer_States, SENSEDDRAIN_VWIINFO_TYPE
    *SensedDrainModel_U_GLOBAL_Vwi_Information, SF_SENSEDDRAIN_DATA_TYPE
    *SensedDrainModel_U_GLOBAL_Sf_SensedDrain_Config, BOOL_TYPE
    SensedDrainModel_U_GLOBAL_SensedDrain_Enabled, PROCESS_RESULT_TYPE
    *SensedDrainModel_Y_GLOBAL_SensedDrain_Result) /* Sample time: [0.1s, 0.0s] */
{
    /* RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_SensedDrainModel__Execute_at_outport_1' incorporates:
     *  SubSystem: '<Root>/SD_Process'
     */
    /* Outputs for Atomic SubSystem: '<S1>/GetModelData' */
    /* DataStoreWrite: '<S3>/Data Store Write' incorporates:
     *  Inport: '<Root>/SensedDrainData_in'
     */
    SensedDrainData = SensedDrain__GetModelData();

    /* End of Outputs for SubSystem: '<S1>/GetModelData' */

    /* Outputs for Enabled SubSystem: '<S1>/Subsystem' incorporates:
     *  EnablePort: '<S5>/Enable'
     */
    /* Inport: '<Root>/SensedDrain_Enabled' */
    if (SensedDrainModel_U_GLOBAL_SensedDrain_Enabled)
    {
        if (!SensedDrainModel_M->dwork.Subsystem_MODE)
        {
            /* SystemReset for Chart: '<S5>/SensedDrain__Logic' */
            SensedDrain__Logic_Reset
                (&SensedDrainModel_M->self_sf_SensedDrain__Logic);
            SensedDrainModel_M->dwork.Subsystem_MODE = TRUE;
        }

        /* Chart: '<S5>/SensedDrain__Logic' incorporates:
         *  Inport: '<Root>/Drain_Confirmed_In'
         *  Inport: '<Root>/Motor_Data'
         *  Inport: '<Root>/Process_Control'
         *  Inport: '<Root>/Sf_SensedDrain_Config'
         *  Inport: '<Root>/Timer_States'
         *  Inport: '<Root>/Vwi_Information'
         *  Outport: '<Root>/SensedDrain_Result'
         */
        SensedDrain__Logic(&SensedDrainModel_M->self_sf_SensedDrain__Logic,
                           SensedDrainModel_U_GLOBAL_Process_Control,
                           SensedDrainModel_U_GLOBAL_Motor_Data->Wash_Pump_Type,
                           SensedDrainModel_U_GLOBAL_Motor_Data->Drain_Pump_Type,
                           SensedDrainModel_U_GLOBAL_Motor_Data->WashPump_Is_Stopped,
                           SensedDrainModel_U_GLOBAL_Motor_Data->DrainPump_Is_Stopped,
                           SensedDrainModel_U_GLOBAL_Motor_Data->WashPump_Reached_Target_Speed,
                           SensedDrainModel_U_GLOBAL_Motor_Data->DrainPump_Reached_Target_Speed,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_RetryPause_Timer_State,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_DM_Initial_Timer_State,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_DM_VwiStable_Timer_State,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_WM_NoChange_Timer_State,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_DM_Attempt_Timer_State,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_WM_Attempt_Timer_State,
                           SensedDrainModel_U_GLOBAL_Timer_States->SensedDrain_EmptyDrain_Timer_State,
                           SensedDrainModel_U_GLOBAL_Vwi_Information->Vwi_Status,
                           SensedDrainModel_U_GLOBAL_Vwi_Information->Vwi_Heavy_Filt_Signal,
                           SensedDrainModel_U_GLOBAL_Sf_SensedDrain_Config,
                           SensedDrain__GetDrainConfirmed(),
                           SensedDrainModel_Y_GLOBAL_SensedDrain_Result);
    }
    else
    {
        if (SensedDrainModel_M->dwork.Subsystem_MODE)
        {
            SensedDrainModel_M->dwork.Subsystem_MODE = FALSE;
        }
    }

    /* End of Inport: '<Root>/SensedDrain_Enabled' */
    /* End of Outputs for SubSystem: '<S1>/Subsystem' */

    /* Outputs for Atomic SubSystem: '<S1>/SetModelData' */
    /* DataStoreRead: '<S4>/Data Store Read' */
    SensedDrain__SetModelData(SensedDrainData);

    /* End of Outputs for SubSystem: '<S1>/SetModelData' */
    /* End of Outputs for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_SensedDrainModel__Execute_at_outport_1' */
}

/** Model initialize function */
void SensedDrainModel__Initialize(RT_MODEL_SensedDrainModel_TYPE *const
    SensedDrainModel_M)
{
    /* SystemInitialize for Atomic SubSystem: '<Root>/SensedDrain_Initialize_Function' */
    /* BusCreator: '<S2>/Bus Creator' incorporates:
     *  Constant: '<S2>/Constant1'
     *  Constant: '<S2>/Constant2'
     *  Constant: '<S2>/Constant3'
     *  Constant: '<S2>/Constant4'
     *  Constant: '<S2>/Constant5'
     *  Constant: '<S2>/Constant6'
     *  Constant: '<S2>/Constant7'
     */
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Drain_Pump_Mode = OFF;
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Wash_Pump_Mode = OFF;
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Drain_Pump_Speed_Type =
        DM_SPEED_DEFAULT;
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Vwi_Stability_Status =
        FALSE;
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Vwi_Previous = 0U;
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Attempt_Counter = 0U;
    SensedDrainModel_M->blockIO.SensedDrain_InitialData.Fault_Type =
        FAULT_ID_NOT_DRAINING;

    /* DataStoreWrite: '<S2>/Data Store Write' */
    SensedDrainData = SensedDrainModel_M->blockIO.SensedDrain_InitialData;

    /* End of SystemInitialize for SubSystem: '<Root>/SensedDrain_Initialize_Function' */
}
