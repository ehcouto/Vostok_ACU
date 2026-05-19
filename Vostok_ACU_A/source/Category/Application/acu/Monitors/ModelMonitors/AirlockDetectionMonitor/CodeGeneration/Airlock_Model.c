/**
 * @file: Airlock_Model.c
 *
 * @model: 'Airlock_Model'.
 *
 * @brief
 *
 * @modelversion:  1.181
 *
 * @coderversion:  8.14 (R2018a) 06-Feb-2018
 *
 * C/C++ source code generated on : Wed Jun 12 08:22:58 2019
 *
 * @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Airlock_Model.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

/* Named constants for Chart: ''Airlock_Definition' (':295')' */
#define Airlock_Mod_IN_ALS_AIRLOCK_RISK ((uint8)2U)
#define Airlock_Mode_IN_NO_ACTIVE_CHILD ((uint8)0U)
#define Airlock_Model_IN_ALS_AIRLOCK   ((uint8)1U)
#define Airlock_Model_IN_ALS_NO_AIRLOCK ((uint8)3U)
#define Airlock_Model_IN_ALS_UNDEFINED ((uint8)4U)

/* Named constants for Chart: ''Variance_Hysteresis' (':296')' */
#define Airlock_Model_IN_HIGH_VARIANCE ((uint8)1U)
#define Airlock_Model_IN_LOW_VARIANCE  ((uint8)2U)

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Block signals and states (default storage) */
DW_Airlock_Model_T Airlock_Model_DW;

/* Forward declaration for local functions */
static void Airlock_Model_ComputeDelta(uint16 avg_before, uint16 avg_after,
    uint16 *delta_value, BOOL_TYPE *delta_flag);

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================

/**
 * Function for Chart: ''Airlock_Definition' (':295')'
 * function [delta_value, delta_flag] = ComputeDelta(avg_before, avg_after)
 *  Computate absolute delta value
 */
static void Airlock_Model_ComputeDelta(uint16 avg_before, uint16 avg_after,
    uint16 *delta_value, BOOL_TYPE *delta_flag)
{
    uint32 qY;

    /* MATLAB Function 'ComputeDelta': (':295:53') */
    /* (':295:53:4') if (avg_after <= avg_before) */
    if (avg_after <= avg_before)
    {
        /*  If avg_after smaller then avg_before airlock flag = true */
        /* (':295:53:6') delta_flag = true; */
        *delta_flag = true;

        /* (':295:53:7') delta_value = avg_before - avg_after; */
        qY = ((uint32)avg_before) - /*MW:OvSatOk*/ ((uint32)avg_after);
        if (qY > ((uint32)avg_before))
        {
            qY = 0U;
        }

        *delta_value = (uint16)qY;
    }
    else
    {
        /* (':295:53:8') else */
        /*  If avg_after greater then avg_before airlock flag = false */
        /* (':295:53:10') delta_flag = false; */
        *delta_flag = false;

        /* (':295:53:11') delta_value = avg_after - avg_before; */
        qY = ((uint32)avg_after) - /*MW:OvSatOk*/ ((uint32)avg_before);
        if (qY > ((uint32)avg_after))
        {
            qY = 0U;
        }

        *delta_value = (uint16)qY;
    }
}

/** Model step function */
AIRLOCK_STATUS_TYPE AirlockModel__Execute(uint16 Vwi_Signal, uint16 Vwi_Variance,
    const AIRLOCK_PARAMS_TYPE *Sf_AirlockData)
{
    uint16 tempsum;
    uint8 b_index;
    uint16 rtb_vwi_average;
    BOOL_TYPE rtb_high_variance_region;
    sint32 tmp;
    uint32 tmp_0;
    BOOL_TYPE guard1 = false;
    sint32 exitg1;

    /* specified return value */
    AIRLOCK_STATUS_TYPE Airlock_Status;

    /* Outputs for Atomic SubSystem: 'Airlock_Monitor' (':228') */
    /* Chart: 'Signal_Average' (':348') incorporates:
     *  Inport: 'Vwi_Signal' (':94')
     */
    /* Gateway: Airlock_Monitor/Signal_Average */
    /* During: Airlock_Monitor/Signal_Average */
    /* Entry Internal: Airlock_Monitor/Signal_Average */
    /* Transition: (':348:2') */
    /* (':348:2:1') avg_storage(avg_index)=uint16(Vwi_Signal); */
    Airlock_Model_DW.avg_storage[((sint32)Airlock_Model_DW.avg_index) - 1] =
        Vwi_Signal;

    /* (':348:7:1') sf_internal_predicateOutput = ... */
    /* (':348:7:1') avg_index < uint8(AVG_BUFFER_SIZE); */
    tmp = (sint32)AVG_BUFFER_SIZE;
    if (AVG_BUFFER_SIZE > 255)
    {
        tmp = 255;
    }

    if (((sint32)Airlock_Model_DW.avg_index) < tmp)
    {
        /* Transition: (':348:7') */
        /* Transition: (':348:9') */
        /* (':348:9:1') avg_index = avg_index + uint8(1); */
        tmp = (sint32)((uint32)(((uint32)Airlock_Model_DW.avg_index) + 1U));
        if (((uint32)tmp) > 255U)
        {
            tmp = 255;
        }

        Airlock_Model_DW.avg_index = (uint8)tmp;

        /* Transition: (':348:23') */
    }
    else
    {
        /* Transition: (':348:11') */
        /* (':348:11:1') avg_index = uint8(1); */
        Airlock_Model_DW.avg_index = 1U;
    }

    /* Transition: (':348:82') */
    /*  Calculate the sum of all buffer values */
    /* (':348:82:1') tempsum = uint16(0); */
    tempsum = 0U;

    /* (':348:82:3') index = uint8(0); */
    b_index = 0U;
    do
    {
        exitg1 = 0;

        /* (':348:83:1') sf_internal_predicateOutput = ... */
        /* (':348:83:1') index < uint8(AVG_BUFFER_SIZE); */
        tmp = (sint32)AVG_BUFFER_SIZE;
        if (AVG_BUFFER_SIZE > 255)
        {
            tmp = 255;
        }

        if (((sint32)b_index) < tmp)
        {
            /* Transition: (':348:83') */
            /* NEW_PATTERN */
            /* Transition: (':348:80') */
            /* (':348:80:1') tempsum = tempsum + avg_storage(index + uint8(1)); */
            tmp_0 = ((uint32)tempsum) + ((uint32)
                Airlock_Model_DW.avg_storage[b_index]);
            if (tmp_0 > 65535U)
            {
                tmp_0 = 65535U;
            }

            tempsum = (uint16)tmp_0;

            /* Transition: (':348:75') */
            /* (':348:75:1') index = index + uint8(1); */
            b_index = (uint8)((sint32)(((sint32)b_index) + 1));

            /* Transition: (':348:97') */
        }
        else
        {
            exitg1 = 1;
        }
    }
    while (exitg1 == 0);

    /* Transition: (':348:78') */
    /* Transition: (':348:17') */
    /* (':348:17:1') vwi_average=uint16(tempsum/uint16(AVG_BUFFER_SIZE)); */
    rtb_vwi_average = (uint16)(((uint32)tempsum) / ((uint32)((uint16)
        AVG_BUFFER_SIZE)));
    tempsum = (uint16)(((uint32)tempsum) - ((uint32)((uint16)(((uint32)
                           rtb_vwi_average) * ((uint32)((uint16)AVG_BUFFER_SIZE))))));
    if ((((sint32)tempsum) > 0) && (((sint32)tempsum) >= (((sint32)((uint32)
            (((uint32)((uint16)AVG_BUFFER_SIZE)) >> 1))) + (((sint32)((uint16)
             AVG_BUFFER_SIZE)) & 1))))
    {
        rtb_vwi_average = (uint16)(((uint32)rtb_vwi_average) + 1U);
    }

    /* End of Chart: 'Signal_Average' (':348') */

    /* Chart: 'Variance_Hysteresis' (':296') incorporates:
     *  Inport: 'Sf_AirlockData' (':219')
     *  Inport: 'Vwi_Variance' (':95')
     */
    /* Gateway: Airlock_Monitor/Variance_Hysteresis */
    /* During: Airlock_Monitor/Variance_Hysteresis */
    if (((uint32)Airlock_Model_DW.is_active_c4_Airlock_Model) == 0U)
    {
        /* Entry: Airlock_Monitor/Variance_Hysteresis */
        Airlock_Model_DW.is_active_c4_Airlock_Model = 1U;

        /* Entry Internal: Airlock_Monitor/Variance_Hysteresis */
        /* Transition: (':296:2') */
        Airlock_Model_DW.is_c4_Airlock_Model = Airlock_Model_IN_LOW_VARIANCE;

        /* Entry 'LOW_VARIANCE': (':296:1') */
        /* (':296:1:1') high_variance_region = false; */
        rtb_high_variance_region = false;
    }
    else if (((uint32)Airlock_Model_DW.is_c4_Airlock_Model) ==
             Airlock_Model_IN_HIGH_VARIANCE)
    {
        rtb_high_variance_region = true;

        /* During 'HIGH_VARIANCE': (':296:4') */
        /* (':296:7:1') sf_internal_predicateOutput = ... */
        /* (':296:7:1') Vwi_Variance <= Sf_AirlockData.VwiVarianceLowThold; */
        if (Vwi_Variance <= Sf_AirlockData->VwiVarianceLowThold)
        {
            /* Transition: (':296:7') */
            Airlock_Model_DW.is_c4_Airlock_Model = Airlock_Model_IN_LOW_VARIANCE;

            /* Entry 'LOW_VARIANCE': (':296:1') */
            /* (':296:1:1') high_variance_region = false; */
            rtb_high_variance_region = false;
        }
    }
    else
    {
        rtb_high_variance_region = false;

        /* During 'LOW_VARIANCE': (':296:1') */
        /* (':296:5:1') sf_internal_predicateOutput = ... */
        /* (':296:5:1') Vwi_Variance >= Sf_AirlockData.VwiVarianceHighThold; */
        if (Vwi_Variance >= Sf_AirlockData->VwiVarianceHighThold)
        {
            /* Transition: (':296:5') */
            Airlock_Model_DW.is_c4_Airlock_Model =
                Airlock_Model_IN_HIGH_VARIANCE;

            /* Entry 'HIGH_VARIANCE': (':296:4') */
            /* (':296:4:1') high_variance_region = true; */
            rtb_high_variance_region = true;
        }
    }

    /* End of Chart: 'Variance_Hysteresis' (':296') */

    /* Chart: 'Airlock_Definition' (':295') incorporates:
     *  Inport: 'Sf_AirlockData' (':219')
     */
    /* Gateway: Airlock_Monitor/Airlock_Definition */
    /* During: Airlock_Monitor/Airlock_Definition */
    if (((uint32)Airlock_Model_DW.is_active_c3_Airlock_Model) == 0U)
    {
        /* Entry: Airlock_Monitor/Airlock_Definition */
        Airlock_Model_DW.is_active_c3_Airlock_Model = 1U;

        /* Entry Internal: Airlock_Monitor/Airlock_Definition */
        /* Transition: (':295:2') */
        Airlock_Model_DW.is_c3_Airlock_Model = Airlock_Model_IN_ALS_UNDEFINED;
        Airlock_Model_DW.Airlock_Status = ALS_UNDEFINED;

        /* Entry 'ALS_UNDEFINED': (':295:1') */
        /* (':295:1:1') prev_avg = vwi_average; */
        Airlock_Model_DW.prev_avg = rtb_vwi_average;
    }
    else
    {
        guard1 = false;
        switch (Airlock_Model_DW.is_c3_Airlock_Model)
        {
          case Airlock_Model_IN_ALS_AIRLOCK:
            /* During 'ALS_AIRLOCK': (':295:60') */
            /* (':295:78:1') sf_internal_predicateOutput = ... */
            /* (':295:78:1') high_variance_region == true; */
            if (rtb_high_variance_region)
            {
                /* Transition: (':295:78') */
                /* Transition: (':295:81') */
                /* Transition: (':295:82') */
                Airlock_Model_DW.is_c3_Airlock_Model =
                    Airlock_Model_IN_ALS_UNDEFINED;
                Airlock_Model_DW.Airlock_Status = ALS_UNDEFINED;

                /* Entry 'ALS_UNDEFINED': (':295:1') */
                /* (':295:1:1') prev_avg = vwi_average; */
                Airlock_Model_DW.prev_avg = rtb_vwi_average;
            }
            break;

          case Airlock_Mod_IN_ALS_AIRLOCK_RISK:
            /* During 'ALS_AIRLOCK_RISK': (':295:3') */
            /* (':295:80:1') sf_internal_predicateOutput = ... */
            /* (':295:80:1') high_variance_region == true; */
            if (rtb_high_variance_region)
            {
                /* Transition: (':295:80') */
                /* Transition: (':295:82') */
                Airlock_Model_DW.is_c3_Airlock_Model =
                    Airlock_Model_IN_ALS_UNDEFINED;
                Airlock_Model_DW.Airlock_Status = ALS_UNDEFINED;

                /* Entry 'ALS_UNDEFINED': (':295:1') */
                /* (':295:1:1') prev_avg = vwi_average; */
                Airlock_Model_DW.prev_avg = rtb_vwi_average;
            }
            else
            {
                /* (':295:61:1') sf_internal_predicateOutput = ... */
                /* (':295:61:1') timeout_count >= ... */
                /* (':295:61:1')  uint16((Sf_AirlockData.AirlockRiskTimeout * 1000)/TIME_TO_TICK); */
                tmp_0 = ((uint32)Sf_AirlockData->AirlockRiskTimeout) * 1000U;
                if (tmp_0 > 65535U)
                {
                    tmp_0 = 65535U;
                }

                if (((sint32)((uint16)TIME_TO_TICK)) == 0)
                {
                    if (((sint32)((uint16)tmp_0)) == 0)
                    {
                        rtb_vwi_average = 0U;
                    }
                    else
                    {
                        rtb_vwi_average = MAX_uint16_T;
                    }
                }
                else
                {
                    rtb_vwi_average = (uint16)(((uint32)((uint16)tmp_0)) /
                        ((uint32)((uint16)TIME_TO_TICK)));
                    tempsum = (uint16)(((uint32)((uint16)tmp_0)) - ((uint32)
                                        ((uint16)(((uint32)rtb_vwi_average) *
                                          ((uint32)((uint16)TIME_TO_TICK))))));
                    if ((((sint32)tempsum) > 0) && (((sint32)tempsum) >=
                            (((sint32)((uint32)(((uint32)((uint16)TIME_TO_TICK))
                             >> 1))) + (((sint32)((uint16)TIME_TO_TICK)) & 1))))
                    {
                        rtb_vwi_average = (uint16)(((uint32)rtb_vwi_average) +
                            1U);
                    }
                }

                if (Airlock_Model_DW.timeout_count >= rtb_vwi_average)
                {
                    /* Transition: (':295:61') */
                    Airlock_Model_DW.is_c3_Airlock_Model =
                        Airlock_Model_IN_ALS_AIRLOCK;
                    Airlock_Model_DW.Airlock_Status = ALS_AIRLOCK;

                    /* Entry 'ALS_AIRLOCK': (':295:60') */
                    /* (':295:60:1') wasAirlocked = true; */
                    Airlock_Model_DW.wasAirlocked = true;
                }
                else
                {
                    /* (':295:3:3') timeout_count = timeout_count + uint16(1); */
                    tmp_0 = ((uint32)Airlock_Model_DW.timeout_count) + 1U;
                    if (tmp_0 > 65535U)
                    {
                        tmp_0 = 65535U;
                    }

                    Airlock_Model_DW.timeout_count = (uint16)tmp_0;
                }
            }
            break;

          case Airlock_Model_IN_ALS_NO_AIRLOCK:
            /* During 'ALS_NO_AIRLOCK': (':295:4') */
            /* (':295:8:1') sf_internal_predicateOutput = ... */
            /* (':295:8:1') high_variance_region == true; */
            if (rtb_high_variance_region)
            {
                /* Transition: (':295:8') */
                /* Transition: (':295:84') */
                Airlock_Model_DW.is_c3_Airlock_Model =
                    Airlock_Model_IN_ALS_UNDEFINED;
                Airlock_Model_DW.Airlock_Status = ALS_UNDEFINED;

                /* Entry 'ALS_UNDEFINED': (':295:1') */
                /* (':295:1:1') prev_avg = vwi_average; */
                Airlock_Model_DW.prev_avg = rtb_vwi_average;
            }
            break;

          default:
            /* During 'ALS_UNDEFINED': (':295:1') */
            /* (':295:5:1') sf_internal_predicateOutput = ... */
            /* (':295:5:1') high_variance_region == false; */
            if (!rtb_high_variance_region)
            {
                /* Transition: (':295:5') */
                /* (':295:5:1') [avg_delta, airlock_flag] = ComputeDelta(prev_avg,vwi_average); */
                Airlock_Model_ComputeDelta(Airlock_Model_DW.prev_avg,
                    rtb_vwi_average, &Airlock_Model_DW.avg_delta,
                    &Airlock_Model_DW.airlock_flag);

                /* (':295:20:1') sf_internal_predicateOutput = ... */
                /* (':295:20:1') airlock_flag && ... */
                /* (':295:20:1') avg_delta >= Sf_AirlockData.VwiAirlockDelta; */
                if ((Airlock_Model_DW.airlock_flag) &&
                        (Airlock_Model_DW.avg_delta >=
                         Sf_AirlockData->VwiAirlockDelta))
                {
                    /* Transition: (':295:20') */
                    guard1 = true;
                }
                else
                {
                    /* (':295:19:1') sf_internal_predicateOutput = ... */
                    /* (':295:19:1') ~airlock_flag && ... */
                    /* (':295:19:1') avg_delta >= Sf_AirlockData.VwiRecoveryDelta; */
                    if (((!Airlock_Model_DW.airlock_flag) &&
                            (Airlock_Model_DW.avg_delta >=
                             Sf_AirlockData->VwiRecoveryDelta)) ||
                            (!Airlock_Model_DW.wasAirlocked))
                    {
                        /* Transition: (':295:19') */
                        /* Transition: (':295:10') */
                        /* Transition: (':295:86') */
                        Airlock_Model_DW.is_c3_Airlock_Model =
                            Airlock_Model_IN_ALS_NO_AIRLOCK;
                        Airlock_Model_DW.Airlock_Status = ALS_NO_AIRLOCK;

                        /* Entry 'ALS_NO_AIRLOCK': (':295:4') */
                        /* (':295:4:1') wasAirlocked = false; */
                        Airlock_Model_DW.wasAirlocked = false;
                    }
                    else
                    {
                        /* Transition: (':295:17') */
                        /* (':295:11:1') sf_internal_predicateOutput = ... */
                        /* (':295:11:1') wasAirlocked; */
                        /* Transition: (':295:11') */
                        /* (':295:90:1') sf_internal_predicateOutput = ... */
                        /* (':295:90:1') timeout_count >= uint16((Sf_AirlockData.AirlockRiskTimeout * 1000)/TIME_TO_TICK); */
                        tmp_0 = ((uint32)Sf_AirlockData->AirlockRiskTimeout) *
                            1000U;
                        if (tmp_0 > 65535U)
                        {
                            tmp_0 = 65535U;
                        }

                        if (((sint32)((uint16)TIME_TO_TICK)) == 0)
                        {
                            if (((sint32)((uint16)tmp_0)) == 0)
                            {
                                rtb_vwi_average = 0U;
                            }
                            else
                            {
                                rtb_vwi_average = MAX_uint16_T;
                            }
                        }
                        else
                        {
                            rtb_vwi_average = (uint16)(((uint32)((uint16)tmp_0))
                                / ((uint32)((uint16)TIME_TO_TICK)));
                            tempsum = (uint16)(((uint32)((uint16)tmp_0)) -
                                               ((uint32)((uint16)(((uint32)
                                                   rtb_vwi_average) * ((uint32)
                                ((uint16)TIME_TO_TICK))))));
                            if ((((sint32)tempsum) > 0) && (((sint32)tempsum) >=
                                 (((sint32)((uint32)(((uint32)((uint16)
                                       TIME_TO_TICK)) >> 1))) + (((sint32)
                                    ((uint16)TIME_TO_TICK)) & 1))))
                            {
                                rtb_vwi_average = (uint16)(((uint32)
                                    rtb_vwi_average) + 1U);
                            }
                        }

                        if (Airlock_Model_DW.timeout_count >= rtb_vwi_average)
                        {
                            /* Transition: (':295:90') */
                            /* Transition: (':295:76') */
                            Airlock_Model_DW.is_c3_Airlock_Model =
                                Airlock_Model_IN_ALS_AIRLOCK;
                            Airlock_Model_DW.Airlock_Status = ALS_AIRLOCK;

                            /* Entry 'ALS_AIRLOCK': (':295:60') */
                            /* (':295:60:1') wasAirlocked = true; */
                            Airlock_Model_DW.wasAirlocked = true;
                        }
                        else
                        {
                            /* Transition: (':295:91') */
                            guard1 = true;
                        }
                    }
                }
            }
            break;
        }

        if (guard1)
        {
            Airlock_Model_DW.is_c3_Airlock_Model =
                Airlock_Mod_IN_ALS_AIRLOCK_RISK;
            Airlock_Model_DW.Airlock_Status = ALS_AIRLOCK_RISK;

            /* Entry 'ALS_AIRLOCK_RISK': (':295:3') */
            /* (':295:3:1') timeout_count = uint16(0); */
            Airlock_Model_DW.timeout_count = 0U;

            /* (':295:3:1') wasAirlocked = true; */
            Airlock_Model_DW.wasAirlocked = true;
        }
    }

    /* End of Chart: 'Airlock_Definition' (':295') */
    /* End of Outputs for SubSystem: 'Airlock_Monitor' (':228') */

    /* Outport: 'Airlock_Status' (':96') */
    Airlock_Status = Airlock_Model_DW.Airlock_Status;
    return Airlock_Status;
}

/** Model initialize function */
void AirlockModel__Initialize(void)
{
    /* Registration code */

    /* states (dwork) */
    (void) memset((void *)&Airlock_Model_DW, 0,
                  sizeof(DW_Airlock_Model_T));

    {
        Airlock_Model_DW.Airlock_Status = ALS_NONE;
    }

    {
        sint32 i;

        /* Start for DataStoreMemory: 'avg_index_DS' (':332') */
        Airlock_Model_DW.avg_index = 1U;

        /* SystemInitialize for Atomic SubSystem: 'Airlock_Monitor' (':228') */
        /* SystemInitialize for Chart: 'Variance_Hysteresis' (':296') */
        Airlock_Model_DW.is_active_c4_Airlock_Model = 0U;
        Airlock_Model_DW.is_c4_Airlock_Model = Airlock_Mode_IN_NO_ACTIVE_CHILD;

        /* SystemInitialize for Chart: 'Airlock_Definition' (':295') */
        Airlock_Model_DW.is_active_c3_Airlock_Model = 0U;
        Airlock_Model_DW.is_c3_Airlock_Model = Airlock_Mode_IN_NO_ACTIVE_CHILD;
        Airlock_Model_DW.Airlock_Status = ALS_NONE;

        /* End of SystemInitialize for SubSystem: 'Airlock_Monitor' (':228') */

        /* SystemInitialize for Atomic SubSystem: 'Initialize' (':224') */
        /* DataStoreWrite: 'avg_index_Initialize' (':334') incorporates:
         *  Constant: 'Constant1' (':335')
         */
        Airlock_Model_DW.avg_index = 1U;

        /* DataStoreWrite: 'avg_storage_Initialize' (':333') */
        for (i = 0; i < AVG_BUFFER_SIZE; i++)
        {
            Airlock_Model_DW.avg_storage[i] = 0U;
        }

        /* End of DataStoreWrite: 'avg_storage_Initialize' (':333') */

        /* DataStoreWrite: 'wasAirlocked_Initialize' (':329') */
        Airlock_Model_DW.wasAirlocked = false;

        /* DataStoreWrite: 'prev_avg_Initialize' (':345') */
        Airlock_Model_DW.prev_avg = 0U;

        /* End of SystemInitialize for SubSystem: 'Initialize' (':224') */
    }
}
