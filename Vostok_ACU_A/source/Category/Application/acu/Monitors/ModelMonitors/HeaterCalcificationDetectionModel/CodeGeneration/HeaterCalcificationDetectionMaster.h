/**
 * @file: HeaterCalcificationDetectionMaster.h
 *
 * @model: 'HeaterCalcificationDetectionMaster'.
 *
 * @brief The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
 * Detection is based on the characteristic of the current flowing through the component during a heating phase.
 *
 * @modelversion:  0.160
 *
 * @coderversion:  9.3 (R2020a) 18-Nov-2019
 *
 * C/C++ source code generated on : Wed Jun  7 18:51:09 2023
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------
#ifndef RTW_HEADER_HeaterCalcificationDetectionMaster_h_
#define RTW_HEADER_HeaterCalcificationDetectionMaster_h_
#include "rtwtypes.h"
#include "DoorMonitor.h"
#include "HblDefs_prm.h"
#include "HblDefs.h"
#include "C_Extensions.h"
#include "HeaterCalcificationDetection_defs.h"
#include "Vwi.h"
#ifndef HeaterCalcificationDetectionMaster_COMMON_INCLUDES_
# define HeaterCalcificationDetectionMaster_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "Hbl.h"
#endif                 /* HeaterCalcificationDetectionMaster_COMMON_INCLUDES_ */

/* Child system includes */
#include "HCD_Monitor.h"

/* Model Code Variants */

/* Includes for objects with custom storage classes. */
#include "HCD_Params_prv.h"

//--------------------

/* Macros for accessing real-time model data structure */
#ifndef PVOID_DEFINED

typedef void *pVoid;

#define PVOID_DEFINED
#endif

/* Forward declaration for rtModel */
typedef struct TAG_RTMHEATERCALCIFICATION_TYPE RT_MODELHEATERCALCIFICATIO_TYPE;

/* user code (top of header file) */
//#include "SettingFile.h"
//#include "HeatMonitor.h"
#include "Vwi.h"
#include "DoorMonitor.h"

//#include "Regulations.h"
//#include "Hbl.h"
//#include "Log.h"
#include "string.h"
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

/* Block signals and states (default storage) for system '<Root>' */
typedef struct
{
    MDLREFDWHCD_MONITOR_TYPE hcd_mastermodel_instancedata;/* '<Root>/HCD_MasterModel' */
    ON_OFF_TYPE heater_status;         /* '<S10>/Merge' */
    VWI_WATER_SENSE_TYPE vwi_status;   /* '<S8>/Merge' */
    uint16 measured_heater_current;    /* '<S6>/Switch' */
    uint16 water_temperature;          /* '<S7>/Data Type Conversion' */
}
DWHEATERCALCIFICATIONDETEC_TYPE;

/* Real-time Model Data Structure */
struct TAG_RTMHEATERCALCIFICATION_TYPE
{
    DWHEATERCALCIFICATIONDETEC_TYPE *dwork;
};

/* Imported (extern) states */
extern HCD_MODELDATA_TYPE HCD_ModelData;
                                    /* Simulink.Signal object 'HCD_ModelData' */

/* Model entry point functions */
void HeaterCalcificationDetectionMaster__Initialize
    (RT_MODELHEATERCALCIFICATIO_TYPE *const m);
void HeaterCalcificationDetectionMaster__Execute(RT_MODELHEATERCALCIFICATIO_TYPE
    *const m, BOOL_TYPE u_hcd_enable, uint16 u_heater_current_threshold,
    HEATER_CALCIFICATION_PARAMS_TYPE *u_sf_hcd_parameters, BOOL_TYPE
    u_hcd_cycle_started, uint16 *y_hcd_lowest_click_temperature, uint16
    *y_hcd_lowest_click_current, uint16 *y_hcd_lowest_click_power, uint16
    *y_hcd_reduction_temperature, uint16 *y_hcd_reduction_current, uint16
    *y_hcd_reduction_power, HCD_MONITORSTATE_TYPE *y_hcd_monitoring_states);

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
 * '<Root>' : 'HeaterCalcificationDetectionMaster'
 * '<S1>'   : 'HeaterCalcificationDetectionMaster/Door_Status_Check'
 * '<S2>'   : 'HeaterCalcificationDetectionMaster/HBL_Data'
 * '<S3>'   : 'HeaterCalcificationDetectionMaster/Model Info'
 * '<S4>'   : 'HeaterCalcificationDetectionMaster/Door_Status_Check/Enumerated Constant'
 * '<S5>'   : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status'
 * '<S6>'   : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Measured_Heater_Current'
 * '<S7>'   : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Water_Temperature'
 * '<S8>'   : 'HeaterCalcificationDetectionMaster/HBL_Data/Vwi_Status_Caller'
 * '<S9>'   : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/Enumerated Constant4'
 * '<S10>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/read_load_status_if'
 * '<S11>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/read_load_status_if/Enumerated Constant'
 * '<S12>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/read_load_status_if/Enumerated Constant1'
 * '<S13>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/read_load_status_if/Enumerated Constant2'
 * '<S14>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/read_load_status_if/If Action Subsystem'
 * '<S15>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Heater_Status/read_load_status_if/If Action Subsystem1'
 * '<S16>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Measured_Heater_Current/Compare To Zero'
 * '<S17>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Measured_Heater_Current/Enumerated Constant4'
 * '<S18>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/HBL_Water_Temperature/Enumerated Constant4'
 * '<S19>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/Vwi_Status_Caller/If Action Subsystem'
 * '<S20>'  : 'HeaterCalcificationDetectionMaster/HBL_Data/Vwi_Status_Caller/If Action Subsystem1'
 */
#endif                    /* RTW_HEADER_HeaterCalcificationDetectionMaster_h_ */
