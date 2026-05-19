/**
 * @file: HCD_Monitor.h
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
//-------------------
#ifndef RTW_HEADER_HCD_Monitor_h_
#define RTW_HEADER_HCD_Monitor_h_
#include "rtwtypes.h"
#include <math.h>
#include "Vwi.h"
#include "C_Extensions.h"
#include "HeaterCalcificationDetection_defs.h"
#ifndef HCD_Monitor_COMMON_INCLUDES_
# define HCD_Monitor_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* HCD_Monitor_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Includes for objects with custom storage classes. */
#include "HCD_Params_prv.h"

//--------------------
#ifndef DEFINED_TYPEDEF_FOR_HCD_MODELDATA_TYPE_
#define DEFINED_TYPEDEF_FOR_HCD_MODELDATA_TYPE_

typedef struct
{
    uint16 Lowest_Click_Temperature;
    uint16 Lowest_Click_Current;
    uint16 Lowest_Click_Power;
    uint16 Reduction_Temperature;
    uint16 Reduction_Current;
    uint16 Reduction_Power;
    uint16 Heater_Power;
    uint16 Unstable_Clicks;
    uint16 Heater_Energy;
}
HCD_MODELDATA_TYPE;

#endif

#ifndef DEFINED_TYPEDEF_FOR_HCD_MONITORSTATE_TYPE_
#define DEFINED_TYPEDEF_FOR_HCD_MONITORSTATE_TYPE_

typedef enum
{
    HCD_MONITORSTATE_TYPE_None = 0,    /* Default value */
    HCD_MONITORSTATE_TYPE_HEATER_OFF,
    HCD_MONITORSTATE_TYPE_NORMAL_STATE,
    HCD_MONITORSTATE_TYPE_HEATER_CLICK,
    HCD_MONITORSTATE_TYPE_UPDATE_CLICK_THRESHOLDS,
    HCD_MONITORSTATE_TYPE_UNSTABLE_WATER
}
HCD_MONITORSTATE_TYPE;

#endif

/* Block signals and states (default storage) for model 'HCD_Monitor' */
typedef struct
{
    float32 sumheaterenergy;           /* '<S4>/Heater_Condition_Monitoring' */
    struct
    {
        uint_T is_heater_on:3;         /* '<S4>/Heater_Condition_Monitoring' */
        uint_T is_c2_hcd_monitor:2;    /* '<S4>/Heater_Condition_Monitoring' */
        uint_T is_active_c2_hcd_monitor:1;/* '<S4>/Heater_Condition_Monitoring' */
        uint_T initialsamples:1;       /* '<S4>/Heater_Condition_Monitoring' */
    }
    bitsfortid0;

    uint16 beforeclicktemperature;     /* '<S4>/Heater_Condition_Monitoring' */
    uint16 beforeclickcurrent;         /* '<S4>/Heater_Condition_Monitoring' */
    uint16 bckpcurrent;                /* '<S4>/Heater_Condition_Monitoring' */
    uint8 samplenum;                   /* '<S4>/Heater_Condition_Monitoring' */
}
DWHCD_MONITORF_TYPE;

typedef struct
{
    DWHCD_MONITORF_TYPE rtdw;
}
MDLREFDWHCD_MONITOR_TYPE;

/* Imported (extern) states */
extern HCD_MODELDATA_TYPE HCD_ModelData;
                                    /* Simulink.Signal object 'HCD_ModelData' */
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
                 *hcd_avg_heater_current, DWHCD_MONITORF_TYPE *localDW);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'HCD_Monitor'
 * '<S1>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor'
 * '<S2>'   : 'HCD_Monitor/Model Info'
 * '<S3>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor/Cycle Outputs'
 * '<S4>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor/Cycle_Running'
 * '<S5>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor/Cycle_Started'
 * '<S6>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor/Water_Condition_Check'
 * '<S7>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor/Cycle_Running/Heater_Condition_Monitoring'
 * '<S8>'   : 'HCD_Monitor/Heater_Calcification_Detection_Monitor/Water_Condition_Check/Enumerated Constant'
 */
#endif                                 /* RTW_HEADER_HCD_Monitor_h_ */
