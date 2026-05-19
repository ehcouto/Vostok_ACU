/**
 * @file: SensedDrainModel.h
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
//-------------------
#ifndef RTW_HEADER_SensedDrainModel_h_
#define RTW_HEADER_SensedDrainModel_h_
#include "C_Extensions.h"
#include "SensedDrainProcess_def.h"
#include "Fault_prm.h"
#include "Process.h"
#include "HblDefs.h"
#include "SystemTimers.h"
#include "Vwi.h"
#ifndef SensedDrainModel_COMMON_INCLUDES_
# define SensedDrainModel_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* SensedDrainModel_COMMON_INCLUDES_ */

/* Includes for objects with custom storage classes. */
#include "SensedDrainProcess.h"

//--------------------

/* Macros for accessing real-time model data structure */

/* Forward declaration for rtModel */
typedef struct tag_RTM_SensedDrainModel_TYPE RT_MODEL_SensedDrainModel_TYPE;

/* user code (top of header file) */
//#include "SensedDrainProcess.h"
//#include "SensedDrainProcess_def.h"
//#include "Process.h"
//#include "C_Extensions.h"

/* Block states (default storage) for system '<S5>/SensedDrain__Logic' */
typedef struct
{
    uint8 is_active_c5_SensedDrainModel;/* '<S5>/SensedDrain__Logic' */
    uint8 is_SENSED_DRAIN;             /* '<S5>/SensedDrain__Logic' */
    uint8 is_DRAINING_MODE_PUMPS_IN_SERIE;/* '<S5>/SensedDrain__Logic' */
    uint8 is_WASH_PUMP_CONFIRMING;     /* '<S5>/SensedDrain__Logic' */
    uint8 is_CONFIRMING_MODE_PUMPS_IN_SER;/* '<S5>/SensedDrain__Logic' */
    uint8 is_WASH_PUMP;                /* '<S5>/SensedDrain__Logic' */
    uint8 is_DRAIN_PUMP;               /* '<S5>/SensedDrain__Logic' */
    uint8 req_count;                   /* '<S5>/SensedDrain__Logic' */
    BOOL_TYPE timer_started;           /* '<S5>/SensedDrain__Logic' */
    BOOL_TYPE setPreFault_notDrain;    /* '<S5>/SensedDrain__Logic' */
    BOOL_TYPE setPreFault_vwilncons;   /* '<S5>/SensedDrain__Logic' */
    BOOL_TYPE preFaultWasSet_notDrain; /* '<S5>/SensedDrain__Logic' */
    BOOL_TYPE preFaultWasSet_vwiIncons;/* '<S5>/SensedDrain__Logic' */
    BOOL_TYPE sensed_drain_pass;       /* '<S5>/SensedDrain__Logic' */
}
DW_SensedDrain__Logic_Sens_TYPE;

/* Block signals (default storage) */
typedef struct
{
    SENSEDDRAIN_MODELDATA_TYPE SensedDrain_InitialData;/* '<S2>/Bus Creator' */
}
B_SensedDrainModel_TYPE;

/* Block states (default storage) for system '<Root>' */
typedef struct
{
    BOOL_TYPE Subsystem_MODE;          /* '<S1>/Subsystem' */
}
DW_SensedDrainModel_TYPE;

/* model data, for system '<S5>/SensedDrain__Logic' */
typedef struct
{
    DW_SensedDrain__Logic_Sens_TYPE dwork;
}
self_SensedDrain__Logic_Se_TYPE;

/* Self model data, for system '<Root>' */
struct tag_RTM_SensedDrainModel_TYPE
{
    self_SensedDrain__Logic_Se_TYPE self_sf_SensedDrain__Logic;
    DW_SensedDrainModel_TYPE dwork;
    B_SensedDrainModel_TYPE blockIO;
};

/* Model entry point functions */
void SensedDrainModel__Initialize(RT_MODEL_SensedDrainModel_TYPE *const
    SensedDrainModel_M);

/* Exported entry point function */
void SensedDrainModel__Execute(RT_MODEL_SensedDrainModel_TYPE *const
    SensedDrainModel_M, PROCESS_CONTROL_TYPE
    *SensedDrainModel_U_GLOBAL_Process_Control, SENSEDDRAIN_MOTORDATA_TYPE
    *SensedDrainModel_U_GLOBAL_Motor_Data, SENSEDDRAIN_TIMERSTATE_TYPE
    *SensedDrainModel_U_GLOBAL_Timer_States, SENSEDDRAIN_VWIINFO_TYPE
    *SensedDrainModel_U_GLOBAL_Vwi_Information, SF_SENSEDDRAIN_DATA_TYPE
    *SensedDrainModel_U_GLOBAL_Sf_SensedDrain_Config, BOOL_TYPE
    SensedDrainModel_U_GLOBAL_SensedDrain_Enabled, PROCESS_RESULT_TYPE
    *SensedDrainModel_Y_GLOBAL_SensedDrain_Result);

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
 * '<Root>' : 'SensedDrainModel'
 * '<S1>'   : 'SensedDrainModel/SD_Process'
 * '<S2>'   : 'SensedDrainModel/SensedDrain_Initialize_Function'
 * '<S3>'   : 'SensedDrainModel/SD_Process/GetModelData'
 * '<S4>'   : 'SensedDrainModel/SD_Process/SetModelData'
 * '<S5>'   : 'SensedDrainModel/SD_Process/Subsystem'
 * '<S6>'   : 'SensedDrainModel/SD_Process/Subsystem/SensedDrain__Logic'
 */
#endif                                 /* RTW_HEADER_SensedDrainModel_h_ */
