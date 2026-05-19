/**
 * @file: HCD_Monitor.c
 *
 * @model: 'HCD_Monitor'.
 *
 * @brief
 *
 * @modelversion:  2.180
 *
 * @coderversion:  9.3 (R2020a) 18-Nov-2019
 *
 * C/C++ source code generated on : Wed Jun  7 18:50:59 2023
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "HCD_Monitor.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

/* Named constants for Chart: '<S4>/Heater_Condition_Monitoring' */
#define HCD_Monitor_IN_HEATER_CLICK    ((uint8)1U)
#define HCD_Monitor_IN_HEATER_OFF      ((uint8)1U)
#define HCD_Monitor_IN_HEATER_ON       ((uint8)2U)
#define HCD_Monitor_IN_NORMAL_STATE    ((uint8)2U)
#define HCD_Monitor_IN_NO_ACTIVE_CHILD ((uint8)0U)
#define HCD_Monitor_IN_UNSTABLE_WATER  ((uint8)3U)
#define HCD_Monitor_timestamp          (36000.0F)
#define HCD__IN_UPDATE_CLICK_THRESHOLDS ((uint8)4U)

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

/* user code (top of source file) */
#include "HeaterCalcificationDetectionMonitor.h"

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Forward declaration for local functions */
static float32 HCD_Monitor_CalculatePower(uint16 heater_current, const
    HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters);
static void HCD_Monitor_HeaterEnergy(uint16 measured_heater_current, const
    HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters, DWHCD_MONITORF_TYPE
    *localDW);
static void HCD_Monitor_MeanCurrent(uint16 measured_heater_current, uint16
    *hcd_avg_heater_current, DWHCD_MONITORF_TYPE *localDW);
static void HCD_Monitor_ReductionCheck(uint16 water_temperature, const
    HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters, DWHCD_MONITORF_TYPE
    *localDW);

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================

/** Function for Chart: '<S4>/Heater_Condition_Monitoring' */
static float32 HCD_Monitor_CalculatePower(uint16 heater_current, const
    HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters)
{
    float32 heater_power;
    float32 z;
    if (((sint32)sf_hcd_parameters->Heater_Nominal_Current) > 0)
    {
        /*  Returns heater power in W */
        z = ((float32)heater_current) / ((float32)
            sf_hcd_parameters->Heater_Nominal_Current);
        heater_power = (z * z) * ((float32)
            sf_hcd_parameters->Heater_Nominal_Power);
    }
    else
    {
        heater_power = 0.0F;
    }

    return heater_power;
}

/** Function for Chart: '<S4>/Heater_Condition_Monitoring' */
static void HCD_Monitor_HeaterEnergy(uint16 measured_heater_current, const
    HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters, DWHCD_MONITORF_TYPE
    *localDW)
{
    float32 decimalheaterpower;
    float32 sumheaterenergy;
    sumheaterenergy = localDW->sumheaterenergy;

    /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
     *
     * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
     *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
     *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
     */
    /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
     *  EnablePort: '<S4>/Enable'
     *
     * Block description for '<S4>/Enable':
     *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
     *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
     *  Output: Heater Calcified flag
     */
    /* Chart: '<S4>/Heater_Condition_Monitoring' */
    /*  Calculate heater power and energy for the machine analysis */
    decimalheaterpower = HCD_Monitor_CalculatePower(measured_heater_current,
        sf_hcd_parameters);

    /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
    /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
    sumheaterenergy += decimalheaterpower / HCD_Monitor_timestamp;
    decimalheaterpower = roundf(decimalheaterpower);
    if (decimalheaterpower < 65536.0F)
    {
        if (decimalheaterpower >= 0.0F)
        {
            HCD_ModelData.Heater_Power = (uint16)decimalheaterpower;
        }
        else
        {
            HCD_ModelData.Heater_Power = 0U;
        }
    }
    else
    {
        HCD_ModelData.Heater_Power = USHRT_MAX;
    }

    decimalheaterpower = roundf(sumheaterenergy);
    if (decimalheaterpower < 65536.0F)
    {
        if (decimalheaterpower >= 0.0F)
        {
            HCD_ModelData.Heater_Energy = (uint16)decimalheaterpower;
        }
        else
        {
            HCD_ModelData.Heater_Energy = 0U;
        }
    }
    else
    {
        HCD_ModelData.Heater_Energy = USHRT_MAX;
    }

    localDW->sumheaterenergy = sumheaterenergy;
}

/** Function for Chart: '<S4>/Heater_Condition_Monitoring' */
static void HCD_Monitor_MeanCurrent(uint16 measured_heater_current, uint16
    *hcd_avg_heater_current, DWHCD_MONITORF_TYPE *localDW)
{
    uint8 fparam;
    float32 tmp;
    uint32 qy;
    sint32 tmp0;
    uint16 bckpcurrent;
    uint8 samplenum;
    uint16 beforeclickcurrent;
    beforeclickcurrent = localDW->beforeclickcurrent;
    samplenum = localDW->samplenum;
    bckpcurrent = localDW->bckpcurrent;

    /*  Increase the function execution counter */
    tmp0 = (sint32)((uint32)(((uint32)samplenum) + 1U));
    if (((uint32)tmp0) > 255U)
    {
        tmp0 = 255;
    }

    samplenum = (uint8)tmp0;
    if (localDW->bitsfortid0.initialsamples)
    {
        /*  Check if it's the first group of samples the function is called after switching the heater ON */
        /*  Set up the parameters for the first group of samples */
        fparam = (uint8)tmp0;

        /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
         *
         * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
         *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
         *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
         */
        /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
         *  EnablePort: '<S4>/Enable'
         *
         * Block description for '<S4>/Enable':
         *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
         *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
         *  Output: Heater Calcified flag
         */
        /* Chart: '<S4>/Heater_Condition_Monitoring' */
        bckpcurrent = *hcd_avg_heater_current;
        beforeclickcurrent = *hcd_avg_heater_current;

        /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
        /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
    }
    else
    {
        /* Constant: '<S4>/Constant' */
        /*  Assign the target weight */
        fparam = ((uint8)FILTER_WEIGHT);
    }

    /*  Exponentially average heater current */
    qy = (uint32)(((uint32)fparam) - /*MW:OvSatOk*/ 1U);
    if (qy > ((uint32)fparam))
    {
        qy = 0U;
    }

    /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
     *
     * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
     *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
     *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
     */
    /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
     *  EnablePort: '<S4>/Enable'
     *
     * Block description for '<S4>/Enable':
     *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
     *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
     *  Output: Heater Calcified flag
     */
    /* Chart: '<S4>/Heater_Condition_Monitoring' */
    tmp = roundf(((float32)((sint32)(((sint32)(((sint32)(*hcd_avg_heater_current))
                      * ((sint32)qy))) + ((sint32)measured_heater_current)))) /
                 ((float32)fparam));

    /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
    /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
    if (tmp < 65536.0F)
    {
        /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
         *
         * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
         *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
         *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
         */
        /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
         *  EnablePort: '<S4>/Enable'
         *
         * Block description for '<S4>/Enable':
         *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
         *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
         *  Output: Heater Calcified flag
         */
        /* Chart: '<S4>/Heater_Condition_Monitoring' */
        *hcd_avg_heater_current = (uint16)tmp;

        /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
        /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
    }
    else
    {
        /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
         *
         * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
         *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
         *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
         */
        /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
         *  EnablePort: '<S4>/Enable'
         *
         * Block description for '<S4>/Enable':
         *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
         *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
         *  Output: Heater Calcified flag
         */
        /* Chart: '<S4>/Heater_Condition_Monitoring' */
        *hcd_avg_heater_current = USHRT_MAX;

        /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
        /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
    }

    /* Constant: '<S4>/Constant1' */
    if (((uint8)tmp0) == ((uint8)SAMPLING_FREQUENCY))
    {
        /*  Save the average heater current with sampling frequency for EoC calculations */
        beforeclickcurrent = bckpcurrent;

        /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
         *
         * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
         *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
         *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
         */
        /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
         *  EnablePort: '<S4>/Enable'
         *
         * Block description for '<S4>/Enable':
         *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
         *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
         *  Output: Heater Calcified flag
         */
        /* Chart: '<S4>/Heater_Condition_Monitoring' */
        bckpcurrent = *hcd_avg_heater_current;

        /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
        /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
        samplenum = 0U;
        localDW->bitsfortid0.initialsamples = FALSE;
    }
    else
    {
        /*  Do not refresh beforeClickCurrent between control points */
    }

    /* End of Constant: '<S4>/Constant1' */
    localDW->bckpcurrent = bckpcurrent;
    localDW->samplenum = samplenum;
    localDW->beforeclickcurrent = beforeclickcurrent;
}

/** Function for Chart: '<S4>/Heater_Condition_Monitoring' */
static void HCD_Monitor_ReductionCheck(uint16 water_temperature, const
    HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters, DWHCD_MONITORF_TYPE
    *localDW)
{
    float32 tmp;

    /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
     *
     * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
     *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
     *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
     */
    /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
     *  EnablePort: '<S4>/Enable'
     *
     * Block description for '<S4>/Enable':
     *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
     *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
     *  Output: Heater Calcified flag
     */
    /* Chart: '<S4>/Heater_Condition_Monitoring' */
    if ((HeaterCalcificationDetectionMonitor__GetHeaterClicks() <= ((uint16)
            sf_hcd_parameters->Allowed_Clicks_To_Reduce)) && (water_temperature >=
         HCD_ModelData.Reduction_Temperature))
    {
        /*  Check conditions for the reduction */
        /*  Overwrite reduction parameters */
        HCD_ModelData.Reduction_Temperature = water_temperature;
        HCD_ModelData.Reduction_Current = localDW->beforeclickcurrent;
        tmp = roundf(HCD_Monitor_CalculatePower(HCD_ModelData.Reduction_Current,
                      sf_hcd_parameters));
        if (tmp < 65536.0F)
        {
            if (tmp >= 0.0F)
            {
                HCD_ModelData.Reduction_Power = (uint16)tmp;
            }
            else
            {
                HCD_ModelData.Reduction_Power = 0U;
            }
        }
        else
        {
            HCD_ModelData.Reduction_Power = USHRT_MAX;
        }
    }

    /* End of Chart: '<S4>/Heater_Condition_Monitoring' */
    /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */
    /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
}

/** Output and update for referenced model: 'HCD_Monitor' */
void HCD_Monitor(BOOL_TYPE hcd_enable, uint16 heater_current_threshold, uint16
                 measured_heater_current, ON_OFF_TYPE heater_status, uint16
                 water_temperature, VWI_WATER_SENSE_TYPE vwi_status, const
                 HEATER_CALCIFICATION_PARAMS_TYPE *sf_hcd_parameters, BOOL_TYPE
                 hcd_cycle_started, uint16 *hcd_lowest_click_temperature, uint16
                 *hcd_lowest_click_current, uint16 *hcd_lowest_click_power,
                 uint16 *hcd_reduction_temperature, uint16
                 *hcd_reduction_current, uint16 *hcd_reduction_power, uint16
                 *hcd_heater_power, uint16 *hcd_unstable_clicks, uint16
                 *hcd_heater_energy, BOOL_TYPE *hcd_water_stable,
                 HCD_MONITORSTATE_TYPE *hcd_monitoring_states, uint16
                 *hcd_avg_heater_current, DWHCD_MONITORF_TYPE *localDW)
{
    float32 tmp;
    uint32 qy;
    uint16 beforeclicktemperature;
    beforeclicktemperature = localDW->beforeclicktemperature;

    /* Outputs for Atomic SubSystem: '<Root>/Heater_Calcification_Detection_Monitor'
     *
     * Block description for '<Root>/Heater_Calcification_Detection_Monitor':
     *  The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
     *  Detection is based on the characteristic of the current flowing through the component during a heating phase.
     */
    /* Outputs for Enabled SubSystem: '<S1>/Cycle_Started' incorporates:
     *  EnablePort: '<S5>/Enable'
     */
    if (hcd_cycle_started)
    {
        /* DataStoreWrite: '<S5>/Data Store Write' incorporates:
         *  BusCreator: '<S5>/Bus Creator'
         *  Constant: '<S5>/Constant'
         *  Constant: '<S5>/Constant1'
         */
        HCD_ModelData.Lowest_Click_Temperature = ((uint16)FIXED_POINT_99C);
        HCD_ModelData.Lowest_Click_Current = 0U;
        HCD_ModelData.Lowest_Click_Power = 0U;
        HCD_ModelData.Reduction_Temperature = 0U;
        HCD_ModelData.Reduction_Current = 0U;
        HCD_ModelData.Reduction_Power = 0U;
        HCD_ModelData.Heater_Power = 0U;
        HCD_ModelData.Unstable_Clicks = 0U;
        HCD_ModelData.Heater_Energy = 0U;

        /* DataStoreWrite: '<S5>/Data Store Write1' incorporates:
         *  Constant: '<S5>/Zero'
         */
        HeaterCalcificationDetectionMonitor__SetHeaterClicks((const uint16)0U);
    }

    /* End of Outputs for SubSystem: '<S1>/Cycle_Started' */

    /* Switch: '<S6>/Switch' incorporates:
     *  Constant: '<S8>/Constant'
     *  RelationalOperator: '<S6>/Equal'
     */
    *hcd_water_stable = !(WS_WATER != vwi_status);

    /* Outputs for Enabled SubSystem: '<S1>/Cycle_Running' incorporates:
     *  EnablePort: '<S4>/Enable'
     *
     * Block description for '<S4>/Enable':
     *  Input: Algorithm Enabler, Heater Status, Measured Heater Current, VWI Status, SF Parameter (Allowed Clicks Number), SW Feedback for Reaction (Action Taken)
     *  Description: Heater Calcification Detection algorithm is designed to detect limescale buildup on integrated heater to avoid ongoing component degradation.
     *  Output: Heater Calcified flag
     */
    if (hcd_enable)
    {
        /* Chart: '<S4>/Heater_Condition_Monitoring' */
        /* The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
           Detection is based on the characteristic of the current flowing through the component during a heating phase. */
        if (((uint32)localDW->bitsfortid0.is_active_c2_hcd_monitor) == 0U)
        {
            localDW->bitsfortid0.is_active_c2_hcd_monitor = 1U;
            localDW->sumheaterenergy = 0.0F;
            localDW->bitsfortid0.is_c2_hcd_monitor = HCD_Monitor_IN_HEATER_OFF;
            *hcd_monitoring_states = HCD_MONITORSTATE_TYPE_HEATER_OFF;

            /*  Clear heater power */
            HCD_ModelData.Heater_Power = 0U;
            localDW->bitsfortid0.initialsamples = TRUE;
            *hcd_avg_heater_current = 0U;
        }
        else if (((uint32)localDW->bitsfortid0.is_c2_hcd_monitor) ==
                 HCD_Monitor_IN_HEATER_OFF)
        {
            if ((heater_status == ON) && (measured_heater_current >
                    heater_current_threshold))
            {
                /*  Change of heater state is related to ACU request */
                /*  and heater current value */
                *hcd_monitoring_states = HCD_MONITORSTATE_TYPE_None;
                localDW->bitsfortid0.is_c2_hcd_monitor =
                    HCD_Monitor_IN_HEATER_ON;
                HCD_Monitor_HeaterEnergy(measured_heater_current,
                    sf_hcd_parameters, localDW);
                localDW->bitsfortid0.is_heater_on = HCD_Monitor_IN_NORMAL_STATE;
                *hcd_monitoring_states = HCD_MONITORSTATE_TYPE_NORMAL_STATE;
                localDW->samplenum = 0U;
                beforeclicktemperature = water_temperature;
                HCD_Monitor_MeanCurrent(measured_heater_current,
                                        hcd_avg_heater_current, localDW);
                HCD_Monitor_ReductionCheck(water_temperature, sf_hcd_parameters,
                    localDW);
            }
        }
        else
        {
            /* case IN_HEATER_ON: */
            if (heater_status == OFF)
            {
                /*  To move back to off state, heater must be */
                /*  requested to turn off */
                localDW->bitsfortid0.is_heater_on =
                    HCD_Monitor_IN_NO_ACTIVE_CHILD;
                localDW->bitsfortid0.is_c2_hcd_monitor =
                    HCD_Monitor_IN_HEATER_OFF;
                *hcd_monitoring_states = HCD_MONITORSTATE_TYPE_HEATER_OFF;

                /*  Clear heater power */
                HCD_ModelData.Heater_Power = 0U;
                localDW->bitsfortid0.initialsamples = TRUE;
                *hcd_avg_heater_current = 0U;
            }
            else
            {
                HCD_Monitor_HeaterEnergy(measured_heater_current,
                    sf_hcd_parameters, localDW);
                switch (localDW->bitsfortid0.is_heater_on)
                {
                  case HCD_Monitor_IN_HEATER_CLICK:
                    if (water_temperature <
                            HCD_ModelData.Lowest_Click_Temperature)
                    {
                        /*  Check if the click occured earlier (at lower temperature of water) */
                        localDW->bitsfortid0.is_heater_on =
                            HCD__IN_UPDATE_CLICK_THRESHOLDS;
                        *hcd_monitoring_states =
                            HCD_MONITORSTATE_TYPE_UPDATE_CLICK_THRESHOLDS;

                        /*  Overwrite the thresholds */
                        HCD_ModelData.Lowest_Click_Temperature =
                            beforeclicktemperature;
                        HCD_ModelData.Lowest_Click_Current =
                            localDW->beforeclickcurrent;
                        tmp = roundf(HCD_Monitor_CalculatePower
                                     (localDW->beforeclickcurrent,
                                      sf_hcd_parameters));
                        if (tmp < 65536.0F)
                        {
                            if (tmp >= 0.0F)
                            {
                                HCD_ModelData.Lowest_Click_Power = (uint16)tmp;
                            }
                            else
                            {
                                HCD_ModelData.Lowest_Click_Power = 0U;
                            }
                        }
                        else
                        {
                            HCD_ModelData.Lowest_Click_Power = USHRT_MAX;
                        }
                    }
                    else
                    {
                        /*  Click event has occured above */
                        /*  the current water temp. threshold */
                        if (measured_heater_current > heater_current_threshold)
                        {
                            /*  Check if heater current raises back */
                            localDW->bitsfortid0.is_heater_on =
                                HCD_Monitor_IN_NORMAL_STATE;
                            *hcd_monitoring_states =
                                HCD_MONITORSTATE_TYPE_NORMAL_STATE;
                            localDW->samplenum = 0U;
                            beforeclicktemperature = water_temperature;
                            HCD_Monitor_MeanCurrent(measured_heater_current,
                                                    hcd_avg_heater_current,
                                                    localDW);
                            HCD_Monitor_ReductionCheck(water_temperature,
                                sf_hcd_parameters, localDW);
                        }
                    }
                    break;

                  case HCD_Monitor_IN_NORMAL_STATE:
                    if ((measured_heater_current < heater_current_threshold) &&
                            (*hcd_water_stable))
                    {
                        /*  Detect heater current drop during ON state */
                        /*  while there is no instability in the system */
                        localDW->bitsfortid0.is_heater_on =
                            HCD_Monitor_IN_HEATER_CLICK;
                        *hcd_monitoring_states =
                            HCD_MONITORSTATE_TYPE_HEATER_CLICK;

                        /*  Count up the total number of valid clicks in the cycle */
                        qy = (uint32)(((uint32)
                                       HeaterCalcificationDetectionMonitor__GetHeaterClicks
                                       ()) + 1U);
                        if (qy > 65535U)
                        {
                            qy = 65535U;
                        }

                        HeaterCalcificationDetectionMonitor__SetHeaterClicks
                            ((uint16)qy);
                    }
                    else if ((measured_heater_current < heater_current_threshold)
                             && (vwi_status == WS_UNSTABLE_WATER))
                    {
                        /*  Heater current drop during ON state but with water instability */
                        localDW->bitsfortid0.is_heater_on =
                            HCD_Monitor_IN_UNSTABLE_WATER;
                        *hcd_monitoring_states =
                            HCD_MONITORSTATE_TYPE_UNSTABLE_WATER;

                        /*  State for research purposes */
                        /*  Count up number of clicks while VWI = 3 */
                        qy = (uint32)(((uint32)HCD_ModelData.Unstable_Clicks) +
                                      1U);
                        if (qy > 65535U)
                        {
                            qy = 65535U;
                        }

                        HCD_ModelData.Unstable_Clicks = (uint16)qy;
                    }
                    else
                    {
                        beforeclicktemperature = water_temperature;
                        HCD_Monitor_MeanCurrent(measured_heater_current,
                                                hcd_avg_heater_current, localDW);
                        HCD_Monitor_ReductionCheck(water_temperature,
                            sf_hcd_parameters, localDW);
                    }
                    break;

                  case HCD_Monitor_IN_UNSTABLE_WATER:
                    if (*hcd_water_stable)
                    {
                        /*  If in the meantime the system regains  */
                        /*  water stability (VWI = 4), move the counted */
                        /*  click to main register */
                        qy = (uint32)(((uint32)HCD_ModelData.Unstable_Clicks) -
                                      /*MW:OvSatOk*/ 1U);
                        if (qy > ((uint32)HCD_ModelData.Unstable_Clicks))
                        {
                            qy = 0U;
                        }

                        HCD_ModelData.Unstable_Clicks = (uint16)qy;
                        localDW->bitsfortid0.is_heater_on =
                            HCD_Monitor_IN_HEATER_CLICK;
                        *hcd_monitoring_states =
                            HCD_MONITORSTATE_TYPE_HEATER_CLICK;

                        /*  Count up the total number of valid clicks in the cycle */
                        qy = (uint32)(((uint32)
                                       HeaterCalcificationDetectionMonitor__GetHeaterClicks
                                       ()) + 1U);
                        if (qy > 65535U)
                        {
                            qy = 65535U;
                        }

                        HeaterCalcificationDetectionMonitor__SetHeaterClicks
                            ((uint16)qy);
                    }
                    else if (measured_heater_current > heater_current_threshold)
                    {
                        /*  Check if heater current raises back */
                        localDW->bitsfortid0.is_heater_on =
                            HCD_Monitor_IN_NORMAL_STATE;
                        *hcd_monitoring_states =
                            HCD_MONITORSTATE_TYPE_NORMAL_STATE;
                        localDW->samplenum = 0U;
                        beforeclicktemperature = water_temperature;
                        HCD_Monitor_MeanCurrent(measured_heater_current,
                                                hcd_avg_heater_current, localDW);
                        HCD_Monitor_ReductionCheck(water_temperature,
                            sf_hcd_parameters, localDW);
                    }
                    else
                    {
                        /*  State for research purposes */
                    }
                    break;

                  default:
                    /* case IN_UPDATE_CLICK_THRESHOLDS: */
                    if (measured_heater_current > heater_current_threshold)
                    {
                        /*  Check if heater current raises back */
                        localDW->bitsfortid0.is_heater_on =
                            HCD_Monitor_IN_NORMAL_STATE;
                        *hcd_monitoring_states =
                            HCD_MONITORSTATE_TYPE_NORMAL_STATE;
                        localDW->samplenum = 0U;
                        beforeclicktemperature = water_temperature;
                        HCD_Monitor_MeanCurrent(measured_heater_current,
                                                hcd_avg_heater_current, localDW);
                        HCD_Monitor_ReductionCheck(water_temperature,
                            sf_hcd_parameters, localDW);
                    }
                    break;
                }
            }
        }

        /* End of Chart: '<S4>/Heater_Condition_Monitoring' */
    }

    /* End of Outputs for SubSystem: '<S1>/Cycle_Running' */

    /* DataStoreRead: '<S3>/Data Store Read' */
    *hcd_lowest_click_temperature = HCD_ModelData.Lowest_Click_Temperature;
    *hcd_lowest_click_current = HCD_ModelData.Lowest_Click_Current;
    *hcd_lowest_click_power = HCD_ModelData.Lowest_Click_Power;
    *hcd_reduction_temperature = HCD_ModelData.Reduction_Temperature;
    *hcd_reduction_current = HCD_ModelData.Reduction_Current;
    *hcd_reduction_power = HCD_ModelData.Reduction_Power;
    *hcd_heater_power = HCD_ModelData.Heater_Power;
    *hcd_unstable_clicks = HCD_ModelData.Unstable_Clicks;
    *hcd_heater_energy = HCD_ModelData.Heater_Energy;

    /* End of Outputs for SubSystem: '<Root>/Heater_Calcification_Detection_Monitor' */
    localDW->beforeclicktemperature = beforeclicktemperature;
}
