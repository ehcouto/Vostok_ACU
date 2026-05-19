/**
 * @file: Plugging_Model.c
 *
 * @model: 'Plugging_Model'.
 *
 * @brief
 *
 * @modelversion:  1.207
 *
 * @coderversion:  8.14 (R2018a) 06-Feb-2018
 *
 * C/C++ source code generated on : Wed Nov  6 20:04:09 2019
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "Plugging_Model.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

/*
 * Check that imported macros with storage class "ImportedDefine" are defined
 */
#ifndef PLUG_RISK_DELAY_TIMER_MSEC
#error The variable for the parameter "PLUG_RISK_DELAY_TIMER_MSEC" is not defined
#endif

#ifndef PLUG_RISK_NOISE_VALUE
#error The variable for the parameter "PLUG_RISK_NOISE_VALUE" is not defined
#endif

/* Named constants for Chart: '<S5>/Monitor' */
#define IN_DIVERTER_STABLE             ((uint8)1U)
#define IN_DIVERTER_UNSTABLE           ((uint8)2U)
#define IN_NORMAL                      ((uint8)1U)
#define IN_NO_ACTIVE_CHILD             ((uint8)0U)
#define IN_NO_PLUGGING                 ((uint8)1U)
#define IN_PLUGGING                    ((uint8)2U)
#define IN_PLUGGING_RISK               ((uint8)2U)
#define IN_TIMEOUT_DELAY               ((uint8)3U)

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Exported data definition */

/* Definition for custom storage class: FileScope */
static PLUGGING_MODEL_DATA_TYPE PluggingMonitor_Data;

/* Forward declaration for local functions */
static BOOL_TYPE SafeToTransition(SF_PLUGGING_DATA_TYPE *u_sfdata);

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================

/** Function for Chart: '<S5>/Monitor' */
static BOOL_TYPE SafeToTransition(SF_PLUGGING_DATA_TYPE *u_sfdata)
{
    BOOL_TYPE issafe;
    issafe = FALSE;

    /* Inport: '<Root>/Vwi_Variance' incorporates:
     *  Inport: '<Root>/SFData'
     */
    if ((PluggingMonitor_Data.Diverter_Stable) && (Vwi__GetVariance() <
            u_sfdata->Plugging_VwiVarianceThreshold))
    {
        issafe = TRUE;
    }

    /* End of Inport: '<Root>/Vwi_Variance' */
    return issafe;
}

/** Model step function */
void PluggingModel__Execute(RT_MODELPLUGGING_MODEL_TYPE *const Plugging_Model_M,
    SF_PLUGGING_DATA_TYPE *u_sfdata, BOOL_TYPE u_plugging_enabled,
    PLUGGING_MONITORSTATE_TYPE *y_plugging_monitorstate)
{
    BOOL_TYPE sf_internal_predicateoutput;
    uint32 qy;
    sint32 tmp;
    uint8 is_no_plugging;
    uint8 is_plugging_monitor;
    uint8 is_diverter_stability;
    uint8 is_active_c3_plugging_model;
    PLUGGING_MONITORSTATE_TYPE plugging_monitorstate;

    /* RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' incorporates:
     *  SubSystem: '<Root>/Plugging_Monitor'
     */
    /* Outputs for Enabled SubSystem: '<S1>/Enabled_Subsystem' incorporates:
     *  EnablePort: '<S2>/Enable'
     */
    /* Outputs for Atomic SubSystem: '<S2>/StepFunction' */
    /* Outport: '<Root>/Plugging_MonitorState' incorporates:
     *  Chart: '<S5>/Monitor'
     */
    plugging_monitorstate = *y_plugging_monitorstate;

    /* End of Outputs for SubSystem: '<S2>/StepFunction' */
    /* End of Outputs for SubSystem: '<S1>/Enabled_Subsystem' */
    /* End of Outputs for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' */
    is_active_c3_plugging_model =
        Plugging_Model_M->dwork.is_active_c3_plugging_model;
    is_diverter_stability = Plugging_Model_M->dwork.is_diverter_stability;
    is_plugging_monitor = Plugging_Model_M->dwork.is_plugging_monitor;
    is_no_plugging = Plugging_Model_M->dwork.is_no_plugging;

    /* RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' incorporates:
     *  SubSystem: '<Root>/Plugging_Monitor'
     */
    /* Outputs for Enabled SubSystem: '<S1>/Enabled_Subsystem' incorporates:
     *  EnablePort: '<S2>/Enable'
     */
    /* Inport: '<Root>/Plugging_Enabled' */
    if (u_plugging_enabled)
    {
        /* Outputs for Atomic SubSystem: '<S2>/ModelDataIn' */
        /* DataStoreWrite: '<S3>/Data Store Write' incorporates:
         *  Inport: '<Root>/Plugging_ModelDataIn'
         */
        PluggingMonitor_Data = PluggingMonitor__GetModelData();

        /* End of Outputs for SubSystem: '<S2>/ModelDataIn' */

        /* Outputs for Atomic SubSystem: '<S2>/StepFunction' */
        /* Chart: '<S5>/Monitor' incorporates:
         *  Inport: '<Root>/AirlockStatus'
         *  Inport: '<Root>/Diverter_Information'
         *  Inport: '<Root>/PluggingReset'
         *  Inport: '<Root>/SFData'
         *  Inport: '<Root>/Timer_Input'
         *  Inport: '<Root>/Vwi_HeavyFilterSignal'
         */
        if (((uint32)is_active_c3_plugging_model) == 0U)
        {
            is_active_c3_plugging_model = 1U;
            is_diverter_stability = IN_DIVERTER_STABLE;
            PluggingMonitor_Data.Diverter_Stable = TRUE;
            is_plugging_monitor = IN_NO_PLUGGING;
            PluggingMonitor__SetStatus(PL_NO_PLUGGING);
            is_no_plugging = IN_NORMAL;

            /* Outport: '<Root>/Plugging_MonitorState' */
            plugging_monitorstate = NORMAL;
            PluggingMonitor_Data.RiskCounter = 0;
            PluggingMonitor_Data.PluggingRisk = 0;
        }
        else
        {
            switch (is_diverter_stability)
            {
              case IN_DIVERTER_STABLE:
                if (!(DishAuxVar__GetDiverterPosition()).Is_Stable)
                {
                    is_diverter_stability = IN_DIVERTER_UNSTABLE;
                    PluggingMonitor_Data.Diverter_Stable = FALSE;
                }
                break;

              case IN_DIVERTER_UNSTABLE:
                if ((DishAuxVar__GetDiverterPosition()).Is_Stable)
                {
                    is_diverter_stability = IN_TIMEOUT_DELAY;
                    PluggingMonitor_Data.Diverter_TimeoutCounter = 0;
                }
                break;

              default:
                if (!(DishAuxVar__GetDiverterPosition()).Is_Stable)
                {
                    is_diverter_stability = IN_DIVERTER_UNSTABLE;
                    PluggingMonitor_Data.Diverter_Stable = FALSE;
                }
                else
                {
                    tmp = (sint32)((uint32)(((uint32)
                                     u_sfdata->Plugging_DiverterMovingTimeoutSec)
                                            * 10U));
                    if (((uint32)tmp) > 255U)
                    {
                        tmp = 255;
                    }

                    if (((sint32)PluggingMonitor_Data.Diverter_TimeoutCounter) >=
                        tmp)
                    {
                        is_diverter_stability = IN_DIVERTER_STABLE;
                        PluggingMonitor_Data.Diverter_Stable = TRUE;
                    }
                    else
                    {
                        tmp = (sint32)((uint32)(((uint32)
                                         PluggingMonitor_Data.Diverter_TimeoutCounter)
                                                + 1U));
                        if (((uint32)tmp) > 255U)
                        {
                            tmp = 255;
                        }

                        PluggingMonitor_Data.Diverter_TimeoutCounter = (uint8)
                            tmp;
                    }
                }
                break;
            }

            if (((uint32)is_plugging_monitor) == IN_NO_PLUGGING)
            {
                PluggingMonitor__SetStatus(PL_NO_PLUGGING);
                if (Plugging__GetResetStatus())
                {
                    is_plugging_monitor = IN_NO_PLUGGING;
                    PluggingMonitor__SetStatus(PL_NO_PLUGGING);
                    is_no_plugging = IN_NORMAL;

                    /* Outport: '<Root>/Plugging_MonitorState' */
                    plugging_monitorstate = NORMAL;
                    PluggingMonitor_Data.RiskCounter = 0;
                    PluggingMonitor_Data.PluggingRisk = 0;
                }
                else
                {
                    if ((SafeToTransition(u_sfdata)) &&
                            ((Vwi__GetHeavyFiltSignal() <=
                              u_sfdata->Plugging_VwiSignalLowThreshold) &&
                             (((sint32)PluggingMonitor_Data.PluggingRisk) == 1)))
                    {
                        sf_internal_predicateoutput = TRUE;
                    }
                    else
                    {
                        sf_internal_predicateoutput = FALSE;
                    }

                    if (sf_internal_predicateoutput)
                    {
                        is_no_plugging = IN_NO_ACTIVE_CHILD;
                        is_plugging_monitor = IN_PLUGGING;

                        /* Outport: '<Root>/Plugging_MonitorState' */
                        plugging_monitorstate = PLUGGING;
                        PluggingMonitor__SetStatus(PL_PLUGGING);
                        PluggingMonitor_Data.PluggingRisk = 0;
                    }
                    else if (((uint32)is_no_plugging) == IN_NORMAL)
                    {
                        if ((SafeToTransition(u_sfdata)) &&
                                (Vwi__GetHeavyFiltSignal() <=
                                 u_sfdata->Plugging_VwiSignalRiskThreshold))
                        {
                            sf_internal_predicateoutput = TRUE;
                        }
                        else
                        {
                            sf_internal_predicateoutput = FALSE;
                        }

                        if (sf_internal_predicateoutput)
                        {
                            is_no_plugging = IN_PLUGGING_RISK;

                            /* Outport: '<Root>/Plugging_MonitorState' */
                            plugging_monitorstate = PLUGGING_RISK;
                            PluggingMonitor__SetRiskDelayTimer((const uint32)
                                ((uint32)PLUG_RISK_DELAY_TIMER_MSEC));
                            PluggingMonitor_Data.VwiRiskLastValue =
                                Vwi__GetHeavyFiltSignal();
                        }
                    }
                    else
                    {
                        if ((AirlockMonitor__GetStatus() == ALS_AIRLOCK) ||
                                ((SafeToTransition(u_sfdata)) &&
                                 (Vwi__GetHeavyFiltSignal() >
                                  u_sfdata->Plugging_VwiSignalRecoverThreshold)))
                        {
                            sf_internal_predicateoutput = TRUE;
                        }
                        else
                        {
                            sf_internal_predicateoutput = FALSE;
                        }

                        if (sf_internal_predicateoutput)
                        {
                            is_no_plugging = IN_NORMAL;

                            /* Outport: '<Root>/Plugging_MonitorState' */
                            plugging_monitorstate = NORMAL;
                            PluggingMonitor_Data.RiskCounter = 0;
                            PluggingMonitor_Data.PluggingRisk = 0;
                        }
                        else
                        {
                            if ((PluggingMonitor__GetTimerState() ==
                                    SYSTEMTIMERS_STATE_DONE) && (((sint32)
                                    PluggingMonitor_Data.PluggingRisk) == 0))
                            {
                                PluggingMonitor__SetRiskDelayTimer((const uint32)
                                    ((uint32)PLUG_RISK_DELAY_TIMER_MSEC));
                                if (Vwi__GetHeavyFiltSignal() <
                                        PluggingMonitor_Data.VwiRiskLastValue)
                                {
                                    tmp = (sint32)((uint32)(((uint32)
                                                     PluggingMonitor_Data.RiskCounter)
                                                            + 1U));
                                    if (((uint32)tmp) > 255U)
                                    {
                                        tmp = 255;
                                    }

                                    PluggingMonitor_Data.RiskCounter = (uint8)
                                        tmp;
                                    if (PluggingMonitor_Data.RiskCounter >=
                                            u_sfdata->Plugging_RiskMaxCount)
                                    {
                                        PluggingMonitor_Data.PluggingRisk = 1;
                                        PluggingMonitor_Data.RiskCounter = 0;
                                    }
                                }
                                else
                                {
                                    qy = (uint32)(((uint32)
                                                   Vwi__GetHeavyFiltSignal()) -
                                                  /*MW:OvSatOk*/ ((uint32)
                                                   PluggingMonitor_Data.VwiRiskLastValue));
                                    if (qy > ((uint32)Vwi__GetHeavyFiltSignal()))
                                    {
                                        qy = 0U;
                                    }

                                    if (((sint32)qy) <= ((sint32)((uint16)
                                            PLUG_RISK_NOISE_VALUE)))
                                    {
                                    }
                                    else
                                    {
                                        PluggingMonitor_Data.RiskCounter = 0;
                                    }
                                }

                                PluggingMonitor_Data.VwiRiskLastValue =
                                    Vwi__GetHeavyFiltSignal();
                            }
                        }
                    }
                }
            }
            else
            {
                PluggingMonitor__SetStatus(PL_PLUGGING);
                if ((Plugging__GetResetStatus()) || ((SafeToTransition(u_sfdata))
                     && (Vwi__GetHeavyFiltSignal() >
                         u_sfdata->Plugging_VwiSignalRecoverThreshold)))
                {
                    sf_internal_predicateoutput = TRUE;
                }
                else
                {
                    sf_internal_predicateoutput = FALSE;
                }

                if (sf_internal_predicateoutput)
                {
                    is_plugging_monitor = IN_NO_PLUGGING;
                    PluggingMonitor__SetStatus(PL_NO_PLUGGING);
                    is_no_plugging = IN_NORMAL;

                    /* Outport: '<Root>/Plugging_MonitorState' */
                    plugging_monitorstate = NORMAL;
                    PluggingMonitor_Data.RiskCounter = 0;
                    PluggingMonitor_Data.PluggingRisk = 0;
                }
            }
        }

        /* End of Outputs for SubSystem: '<S2>/StepFunction' */

        /* Outputs for Atomic SubSystem: '<S2>/ModelDataOut' */
        /* DataStoreRead: '<S4>/Data Store Read' */
        PluggingMonitor__SetModelData(PluggingMonitor_Data);

        /* End of Outputs for SubSystem: '<S2>/ModelDataOut' */
    }

    /* End of Inport: '<Root>/Plugging_Enabled' */
    /* End of Outputs for SubSystem: '<S1>/Enabled_Subsystem' */
    /* End of Outputs for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' */
    Plugging_Model_M->dwork.is_no_plugging = is_no_plugging;
    Plugging_Model_M->dwork.is_plugging_monitor = is_plugging_monitor;
    Plugging_Model_M->dwork.is_diverter_stability = is_diverter_stability;
    Plugging_Model_M->dwork.is_active_c3_plugging_model =
        is_active_c3_plugging_model;

    /* RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' incorporates:
     *  SubSystem: '<Root>/Plugging_Monitor'
     */
    /* Outputs for Enabled SubSystem: '<S1>/Enabled_Subsystem' incorporates:
     *  EnablePort: '<S2>/Enable'
     */
    /* Outputs for Atomic SubSystem: '<S2>/StepFunction' */
    /* Outport: '<Root>/Plugging_MonitorState' incorporates:
     *  Chart: '<S5>/Monitor'
     */
    *y_plugging_monitorstate = plugging_monitorstate;

    /* End of Outputs for SubSystem: '<S2>/StepFunction' */
    /* End of Outputs for SubSystem: '<S1>/Enabled_Subsystem' */
    /* End of Outputs for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' */
}

/** Model initialize function */
void Plugging_Model_initialize(RT_MODELPLUGGING_MODEL_TYPE *const
    Plugging_Model_M, PLUGGING_MONITORSTATE_TYPE *y_plugging_monitorstate)
{
    /* SystemInitialize for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' incorporates:
     *  SubSystem: '<Root>/Plugging_Monitor'
     */
    /* SystemInitialize for Enabled SubSystem: '<S1>/Enabled_Subsystem' */
    /* SystemInitialize for Atomic SubSystem: '<S2>/StepFunction' */
    /* InitializeConditions for Outport: '<Root>/Plugging_MonitorState' incorporates:
     *  Chart: '<S5>/Monitor'
     */
    *y_plugging_monitorstate = None;

    /* End of SystemInitialize for SubSystem: '<S2>/StepFunction' */
    /* End of SystemInitialize for SubSystem: '<S1>/Enabled_Subsystem' */
    /* End of SystemInitialize for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_PluggingModel__Execute_at_outport_1' */
    UNUSED_PARAMETER(Plugging_Model_M);
}
