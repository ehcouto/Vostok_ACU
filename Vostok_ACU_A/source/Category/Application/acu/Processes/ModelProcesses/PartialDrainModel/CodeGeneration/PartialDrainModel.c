/**
 * @file: PartialDrainModel.c
 *
 * @model: 'PartialDrainModel'.
 *
 * @brief Partial drain algorithm is a global solution to drain an specific amount of water from the dishwasher machine, it uses Virtual Water Indicator (VWI) information and Sensed Drain algorithm, that is an algorithm to fully drain the machine using timers and sensed data, to understand how much water was already drained from the machine and stop the draining at the correct moment.
 * This algorithm can be used, but it's not limited to, filter cleaning, dirty water removal and to improve water consumption.
 *
 * @modelversion:  1.214
 *
 * @coderversion:  9.3 (R2020a) 18-Nov-2019
 *
 * C/C++ source code generated on : Tue Aug 18 12:10:25 2020
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "PartialDrainModel.h"
#include "PartialDrainProcess.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

/* Named constants for Chart: '<Root>/Chart' */
#define IN_DRAIN_AND_MONITOR           ((uint8)1U)
#define IN_EXIT                        ((uint8)2U)
#define IN_OFF                         ((uint8)1U)
#define IN_ON                          ((uint8)2U)
#define IN_START_DRAIN_PUMP            ((uint8)3U)

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Forward declaration for local functions */
static void CheckVwiStable(void);
static void IntegrateVwiSignal(DWPARTIALDRAINMODEL_TYPE *dw);
static void ConfirmPumpStopped(PROCESS_RESULT_TYPE *y_partialdrain_processstatus);
static void Motor_Request_Decrement(DWPARTIALDRAINMODEL_TYPE *dw);
static void MotorRequestFeedback(DWPARTIALDRAINMODEL_TYPE *dw);

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================

/** Function for Chart: '<Root>/Chart' */
static void CheckVwiStable(void)
{
    /* Inport: '<Root>/Vwi_Status'
     *
     * Block description for '<Root>/Vwi_Status':
     *  Vwi Status indicating WATER, NO_WATER and unstable condtions
     */
    if (PartialDrainProcess__GetVwiStatus() != WS_NO_WATER)
    {
        /*  Reset Timer */
        PartialDrainProcess__SetVwiStableTimer(ON);
    }
    else
    {
        /*  Keep timer running
           (do nothing) */
    }

    /* End of Inport: '<Root>/Vwi_Status' */
}

/** Function for Chart: '<Root>/Chart' */
static void IntegrateVwiSignal(DWPARTIALDRAINMODEL_TYPE *dw)
{
    uint32 qy;
    sint32 q1;

    /* Inport: '<Root>/Vwi_Signal' incorporates:
     *  Inport: '<Root>/PartialDrain_InitialValue'
     *
     * Block description for '<Root>/Vwi_Signal':
     *  <Default_Value = 0>
     *  Vwi Signal not filtered calculated in Vwi Algorithm
     *
     * Block description for '<Root>/PartialDrain_InitialValue':
     *  <Default_Value = 0>
     *  "Zero" value for Integrator acquired during full Sensed Drains.
     */
    /*  When VWI Signal is greather then Initial Value */
    if (PartialDrainProcess__GetVwiSignal() >
            PartialDrainProcess__GetDrainInitialValue())
    {
        /*  Integrate value applying the correction factor and increase counter */
        q1 = (sint32)(((sint32)(((sint32)PartialDrainProcess__GetVwiSignal()) -
                                ((sint32)
                         PartialDrainProcess__GetDrainInitialValue()))) *
                      ((sint32)dw->partialdraindata.Integrator_CorrectionFactor));
        if ((dw->partialdraindata.Drain_Integrator < 0) && (q1 < ((sint32)
                (LONG_MIN - dw->partialdraindata.Drain_Integrator))))
        {
            dw->partialdraindata.Drain_Integrator = LONG_MIN;
        }
        else if ((dw->partialdraindata.Drain_Integrator > 0) && (q1 > ((sint32)
                   (LONG_MAX - dw->partialdraindata.Drain_Integrator))))
        {
            dw->partialdraindata.Drain_Integrator = LONG_MAX;
        }
        else
        {
            dw->partialdraindata.Drain_Integrator += q1;
        }

        qy = (uint32)(dw->partialdraindata.Drain_Integrator_Count +
                      /*MW:OvSatOk*/ 1U);
        if (qy < dw->partialdraindata.Drain_Integrator_Count)
        {
            qy = ULONG_MAX;
        }

        dw->partialdraindata.Drain_Integrator_Count = qy;
    }
    else
    {
        /*  Do not integrate
           (do nothing) */
    }

    /* End of Inport: '<Root>/Vwi_Signal' */
}

/** Function for Chart: '<Root>/Chart' */
static void ConfirmPumpStopped(PROCESS_RESULT_TYPE *y_partialdrain_processstatus)
{
    /* Inport: '<Root>/DrainPump_Is_Stopped'
     *
     * Block description for '<Root>/DrainPump_Is_Stopped':
     *  <Default_Value = FALSE>
     *  Information from MCU if drain is stopped
     */
    if (PartialDrainProcess__GetDrainPumpIsStopped())
    {
        /* Outport: '<Root>/PartialDrain_ProcessStatus'
         *
         * Block description for '<Root>/PartialDrain_ProcessStatus':
         *  Process result, might be BUSY when the algorithm is running, COMPLETE
         *  when finished
         */
        *y_partialdrain_processstatus = PROCESS_COMPLETE;
    }
    else
    {
        /*  Keep process running
           while pump is not stopped */
    }

    /* End of Inport: '<Root>/DrainPump_Is_Stopped' */
}

/** Function for Chart: '<Root>/Chart' */
static void Motor_Request_Decrement(DWPARTIALDRAINMODEL_TYPE *dw)
{
    sint32 tmp;
    uint8 fbk_count;
    fbk_count = dw->fbk_count;
    if (((sint32)fbk_count) == 10)
    {
        fbk_count = 0U;
        PartialDrainProcess__SendFbkRequest(TRUE);
    }
    else
    {
        tmp = (sint32)(((sint32)fbk_count) + 1);
        if (tmp > 255)
        {
            tmp = 255;
        }

        fbk_count = (uint8)tmp;
    }

    dw->fbk_count = fbk_count;
}

/** Function for Chart: '<Root>/Chart' */
static void MotorRequestFeedback(DWPARTIALDRAINMODEL_TYPE *dw)
{
    ON_OFF_TYPE drain_request;
    drain_request = dw->drain_request;

    /* Inport: '<Root>/DrainPump_Reached_Target_Speed' incorporates:
     *  Inport: '<Root>/DrainPump_Is_Stopped'
     *
     * Block description for '<Root>/DrainPump_Reached_Target_Speed':
     *  <Default_Value = FALSE>
     *  Information from MCU if Drain reached target speed
     *
     * Block description for '<Root>/DrainPump_Is_Stopped':
     *  <Default_Value = FALSE>
     *  Information from MCU if drain is stopped
     */
    if (((drain_request == ON) &&
            (!PartialDrainProcess__GetDrainPumpReachedSpeed())) ||
            ((drain_request == OFF) &&
             (!PartialDrainProcess__GetDrainPumpIsStopped())))
    {
        /*  Decrement request to avoid filling the bus */
        Motor_Request_Decrement(dw);
    }
    else
    {
        /*  Feedback arrived
           (do nothing) */
    }

    /* End of Inport: '<Root>/DrainPump_Reached_Target_Speed' */
}

/** Model step function */
void PartialDrainModel__Execute(RT_MODELPARTIALDRAINMODEL_TYPE *const m,
    PROCESS_CONTROL_TYPE *u_partialdrain_control,
    SF_PARTIALDRAIN_MACHINECONFIG_TYPE *u_sf_partialdrain_machineconfig,
    SF_PARTIALDRAIN_PROCESSCONFIG_TYPE *u_sf_partialdrain_processconfig,
    PROCESS_RESULT_TYPE *y_partialdrain_processstatus, PASS_FAIL_TYPE
    *y_partialdrain_processresult)
{
    DWPARTIALDRAINMODEL_TYPE *dw = ((DWPARTIALDRAINMODEL_TYPE *) m->dwork);
    sint32 tmp;
    sint32 tmp0;
    PASS_FAIL_TYPE partialdrain_processresult;
    BOOL_TYPE guard1 = FALSE;

    /* Outport: '<Root>/PartialDrain_ProcessResult' incorporates:
     *  Chart: '<Root>/Chart'
     *
     * Block description for '<Root>/PartialDrain_ProcessResult':
     *  PASS or FAIL information for the algorithm run
     */
    partialdrain_processresult = *y_partialdrain_processresult;

    /* Chart: '<Root>/Chart' incorporates:
     *  BusCreator generated from: '<Root>/Chart'
     *  Inport: '<Root>/DrainPump_Reached_Target_Speed'
     *  Inport: '<Root>/Liters_In_Tub'
     *  Inport: '<Root>/PartialDrain_InitialValue'
     *  Inport: '<Root>/SF_PartialDrain_MachineConfig'
     *  Inport: '<Root>/SF_PartialDrain_ProcessConfig'
     * *
     * Block description for '<Root>/DrainPump_Reached_Target_Speed':
     *  <Default_Value = FALSE>
     *  Information from MCU if Drain reached target speed
     *
     * Block description for '<Root>/Liters_In_Tub':
     *  <Default_Value = 0>
     *  Amount of water in the machine
     *
     * Block description for '<Root>/PartialDrain_InitialValue':
     *  <Default_Value = 0>
     *  "Zero" value for Integrator acquired during full Sensed Drains.
     *
     * Block description for '<Root>/SF_PartialDrain_MachineConfig':
     *  Algorithm settings defined in Machine Configuration (cross-cycles)
     *
     * Block description for '<Root>/SF_PartialDrain_ProcessConfig':
     *  Algorithm settings defined in process cofiguration, specific for each
     *  run.
     */
    if (((uint32)dw->bitsfortid0.is_active_c3_partialdrainmodel) == 0U)
    {
        dw->bitsfortid0.is_active_c3_partialdrainmodel = 1U;
        if (((sint32)PartialDrainProcess__GetDrainInitialValue()) == 0)
        {
            /* Outport: '<Root>/PartialDrain_ProcessResult'
             *
             * Block description for '<Root>/PartialDrain_ProcessResult':
             *  PASS or FAIL information for the algorithm run
             */
            /*  Transition Operation: */
            partialdrain_processresult = FAIL;
            dw->bitsfortid0.is_partial_drain = IN_EXIT;

            /*  Turn OFF Pump */
            dw->drain_request = OFF;
        }
        else
        {
            /*  Run Partial Drain */
            dw->bitsfortid0.is_partial_drain = IN_START_DRAIN_PUMP;

            /* Outport: '<Root>/PartialDrain_ProcessStatus'
             *
             * Block description for '<Root>/PartialDrain_ProcessStatus':
             *  Process result, might be BUSY when the algorithm is running, COMPLETE
             *  when finished
             */
            /*  Set process as busy */
            *y_partialdrain_processstatus = PROCESS_BUSY;

            /*  Initialize variables */
            dw->partialdraindata.Drain_Integrator = (sint32)
                u_sf_partialdrain_machineconfig->Starting_Offset;
            dw->partialdraindata.Drain_Integrator_Count = 0U;

            /*  Start Pump */
            dw->drain_request = ON;
        }

        dw->bitsfortid0.is_pump_control = IN_OFF;

        /*  Turn motor OFF */
        PartialDrainProcess__SetDrainPumpMode(OFF);

        /*  Clear fbk count */
        dw->fbk_count = 0U;
    }
    else
    {
        guard1 = FALSE;
        switch (dw->bitsfortid0.is_partial_drain)
        {
          case IN_DRAIN_AND_MONITOR:
            tmp = (sint32)(((sint32)(((sint32)
                              PartialDrainProcess__GetLitersInTub()) * 10)) /
                           100);
            if ((u_sf_partialdrain_processconfig->PartialDrain_Mode ==
                    PARTIALDRAIN_AMOUNT_REMAINING) && (tmp < ((sint32)
                    u_sf_partialdrain_processconfig->PartialDrain_Liters)))
            {
                /* Outport: '<Root>/PartialDrain_ProcessResult'
                 *
                 * Block description for '<Root>/PartialDrain_ProcessResult':
                 *  PASS or FAIL information for the algorithm run
                 */
                /*  Transition Operation: */
                partialdrain_processresult = FAIL;
                guard1 = TRUE;
            }
            else
            {
                tmp0 = (sint32)(((sint32)(((sint32)
                                   u_sf_partialdrain_processconfig->PartialDrain_Liters)
                                  * ((sint32)
                                     u_sf_partialdrain_machineconfig->Liters_Multiplier)))
                                / 10);
                if (((u_sf_partialdrain_processconfig->PartialDrain_Mode ==
                        PARTIALDRAIN_SPECIFIED_AMOUNT) &&
                        (dw->partialdraindata.Drain_Integrator >= tmp0)) ||
                        ((u_sf_partialdrain_processconfig->PartialDrain_Mode ==
                          PARTIALDRAIN_AMOUNT_REMAINING) &&
                         (dw->partialdraindata.Drain_Integrator >= ((sint32)
                        (((sint32)(((sint32)(tmp * ((sint32)
                             u_sf_partialdrain_machineconfig->Liters_Multiplier)))
                                   / 10)) - tmp0)))))
                {
                    /* Outport: '<Root>/PartialDrain_ProcessResult'
                     *
                     * Block description for '<Root>/PartialDrain_ProcessResult':
                     *  PASS or FAIL information for the algorithm run
                     */
                    /*  Transition Operation: */
                    partialdrain_processresult = PASS;
                    guard1 = TRUE;
                }
                else if (((u_partialdrain_control->Resume_From_Reset) ||
                          (u_partialdrain_control->Resume_From_Pause)) ||
                         ((PartialDrainProcess__GetAttemptTimerState() ==
                           SYSTEMTIMERS_STATE_DONE) ||
                          (PartialDrainProcess__GetVwiStableTimerState() ==
                           SYSTEMTIMERS_STATE_DONE)))
                {
                    /* Outport: '<Root>/PartialDrain_ProcessResult'
                     *
                     * Block description for '<Root>/PartialDrain_ProcessResult':
                     *  PASS or FAIL information for the algorithm run
                     */
                    /*  Transition Operation: */
                    partialdrain_processresult = FAIL;
                    guard1 = TRUE;
                }
                else
                {
                    /*  Calculate correction factor */
                    dw->partialdraindata.Integrator_CorrectionFactor = (sint16)
                        ((sint32)(((sint32)(((sint32)(((sint32)(((sint32)
                                 PartialDrainProcess__GetDrainInitialValue()) -
                                ((sint32)
                                 u_sf_partialdrain_machineconfig->Initial_Value_Offset)))
                              * ((sint32)
                                 u_sf_partialdrain_machineconfig->Initial_Value_Multiplier)))
                                            / 200)) + 1));
                    CheckVwiStable();
                    IntegrateVwiSignal(dw);
                }
            }
            break;

          case IN_EXIT:
            ConfirmPumpStopped(y_partialdrain_processstatus);
            break;

          default:
            /* case IN_START_DRAIN_PUMP: */
            if (PartialDrainProcess__GetDrainPumpReachedSpeed())
            {
                /*  Start attempt timer */
                PartialDrainProcess__SetAttemptTimer(ON);
                dw->bitsfortid0.is_partial_drain = IN_DRAIN_AND_MONITOR;

                /*  Start vwi stable timer */
                PartialDrainProcess__SetVwiStableTimer(ON);
            }
            break;
        }

        if (guard1)
        {
            dw->bitsfortid0.is_partial_drain = IN_EXIT;

            /*  Turn OFF Pump */
            dw->drain_request = OFF;
        }

        if (((uint32)dw->bitsfortid0.is_pump_control) == IN_OFF)
        {
            if (dw->drain_request == ON)
            {
                dw->bitsfortid0.is_pump_control = IN_ON;

                /*  Turn Motor ON */
                PartialDrainProcess__SetDrainPumpMode(ON);

                /*  Clear fbk count */
                dw->fbk_count = 0U;
            }
            else
            {
                /*  Keep motor OFF on every step to avoid being turned off by Pump Command */
                PartialDrainProcess__SetDrainPumpMode(OFF);

                /*  Request Feedback when applicable */
                MotorRequestFeedback(dw);
            }
        }
        else
        {
            /* case IN_ON: */
            if (dw->drain_request == OFF)
            {
                dw->bitsfortid0.is_pump_control = IN_OFF;

                /*  Turn motor OFF */
                PartialDrainProcess__SetDrainPumpMode(OFF);

                /*  Clear fbk count */
                dw->fbk_count = 0U;
            }
            else
            {
                /*  Keep motor ON on every step to avoid being turned off by Pump Command */
                PartialDrainProcess__SetDrainPumpMode(ON);

                /*  Request Feedback when applicable */
                MotorRequestFeedback(dw);
            }
        }
    }

    /* Outport: '<Root>/PartialDrain_ProcessResult' incorporates:
     *  Chart: '<Root>/Chart'
     *
     * Block description for '<Root>/PartialDrain_ProcessResult':
     *  PASS or FAIL information for the algorithm run
     */
    *y_partialdrain_processresult = partialdrain_processresult;
}

/** Model initialize function */
void PartialDrainModel__Initialize(RT_MODELPARTIALDRAINMODEL_TYPE *const m,
    PROCESS_RESULT_TYPE *y_partialdrain_processstatus, PASS_FAIL_TYPE
    *y_partialdrain_processresult)
{
    DWPARTIALDRAINMODEL_TYPE *dw = ((DWPARTIALDRAINMODEL_TYPE *) m->dwork);

    /* SystemInitialize for Chart: '<Root>/Chart' */
    dw->drain_request = OFF;

    /* SystemInitialize for Outport: '<Root>/PartialDrain_ProcessStatus' incorporates:
     *  Chart: '<Root>/Chart'
     *
     * Block description for '<Root>/PartialDrain_ProcessStatus':
     *  Process result, might be BUSY when the algorithm is running, COMPLETE
     *  when finished
     */
    *y_partialdrain_processstatus = PROCESS_COMPLETE;

    /* SystemInitialize for Outport: '<Root>/PartialDrain_ProcessResult' incorporates:
     *  Chart: '<Root>/Chart'
     *
     * Block description for '<Root>/PartialDrain_ProcessResult':
     *  PASS or FAIL information for the algorithm run
     */
    *y_partialdrain_processresult = FAIL;

    /* SystemInitialize for Chart: '<Root>/Chart' */
    PartialDrainProcess__SetAttemptTimer(OFF);
    PartialDrainProcess__SetVwiStableTimer(OFF);
    PartialDrainProcess__SetDrainPumpMode(OFF);
}
