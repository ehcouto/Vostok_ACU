/**
 * @file: OWICalibrationModel.c
 *
 * @model: 'OWICalibrationModel'.
 *
 * @brief
 *
 * @modelversion:  1.531
 *
 * @coderversion:  8.12 (R2017a) 16-Feb-2017
 *
 * C/C++ source code generated on : Wed Oct 17 16:50:28 2018
 *
 * @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "OWICalibrationModel.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

/* Named constants for Chart: '<S1>/Chart' */
#define OWICalibrati_IN_BEST_DUTY_CHECK ((uint8)1U)
#define OWICalibrationMode_IN_DUTYCYCLE ((uint8)2U)
#define OWICalibrationMode_IN_STABILIZE ((uint8)6U)
#define OWICalibrationModel_IN_EXIT    ((uint8)3U)
#define OWICalibrationModel_IN_IDLE    ((uint8)4U)
#define OWICalibrationModel_IN_INITIAL ((uint8)5U)

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Exported data definition */

/* Definition for custom storage class: FileScope */
static MODEL_DATA_TYPE OWICalibration_Model_Data;

/* Block states (auto storage) */
DW_OWICalibrationModel_TYPE OWICalibrationModel_DW_GLOBAL;
real_T rt_roundd(real_T u);

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================
real_T rt_roundd(real_T u)
{
    real_T y;
    if (fabs(u) < 4.503599627370496E+15)
    {
        if (u >= 0.5)
        {
            y = floor(u + 0.5);
        }
        else if (u > -0.5)
        {
            y = 0.0;
        }
        else
        {
            y = ceil(u - 0.5);
        }
    }
    else
    {
        y = u;
    }

    return y;
}

/** Model step function */
void OWICalibration__Model(void)
{
    float32 OWICalibration_Current_Error;
    float32 mVtoV;
    uint16 qY;
    BOOL_TYPE guard1 = FALSE;

    /* RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_OWICalibration__Model_at_outport_1' incorporates:
     *  SubSystem: '<Root>/OWI_Calibration'
     */
    /* Outputs for Atomic SubSystem: '<S1>/Subsystem' */
    /* DataStoreWrite: '<S3>/Data Store Write' incorporates:
     *  Inport: '<Root>/getModelData'
     */
    OWICalibration_Model_Data = OWICalibration__GetModelData();

    /* End of Outputs for SubSystem: '<S1>/Subsystem' */

    /* Product: '<S1>/mVtoV' incorporates:
     *  Constant: '<S1>/10Bit'
     *  Constant: '<S1>/Const'
     *  Constant: '<S1>/mVconstant'
     *  Inport: '<Root>/OWICalibration_TurbidityVoltage'
     *  Inport: '<Root>/Sf_OWICalibration_Config'
     *  Product: '<S1>/Divide'
     *  Product: '<S1>/Product'
     *  Product: '<S1>/VtomV'
     */
    mVtoV = OWICalibration__GetTurbidityVoltage() * ((*Sf_OWICalibration_Config)
        .OWICalibration_Input_DC_Voltage * 1000.0F / 1023.0F) / 1000.0F;

    /* Chart: '<S1>/Chart' incorporates:
     *  Inport: '<Root>/Sf_OWICalibration_Config'
     *  Inport: '<Root>/SystemTimers_DelayTimeState'
     *  Inport: '<Root>/SystemTimers_TimeoutState'
     */
    /* Gateway: OWI_Calibration/Chart */
    /* During: OWI_Calibration/Chart */
    if ((uint16)OWICalibrationModel_DW_GLOBAL.is_active_c3_OWICalibrationMode ==
        0U)
    {
        /* Entry: OWI_Calibration/Chart */
        OWICalibrationModel_DW_GLOBAL.is_active_c3_OWICalibrationMode = 1U;

        /* Entry Internal: OWI_Calibration/Chart */
        /* Transition: '<S2>:1071' */
        OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
            OWICalibrationModel_IN_INITIAL;

        /* Entry 'INITIAL': '<S2>:1055' */
        /* '<S2>:1055:1' OWICalibration_Upper_DutyCycle =uint16(32768); */
        OWICalibrationModel_DW_GLOBAL.OWICalibration_Upper_DutyCycle = 32768U;

        /* '<S2>:1055:3' OWICalibration_Lower_DutyCycle =uint16(0); */
        OWICalibrationModel_DW_GLOBAL.OWICalibration_Lower_DutyCycle = 0U;

        /* '<S2>:1055:4' OWICalibration_Drive_DutyCycle = OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
        OWICalibration__SetDutyCycle
            (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

        /* '<S2>:1055:5' OWICalibration_Prev_Error = 5; */
        OWICalibrationModel_DW_GLOBAL.OWICalibration_Prev_Error = 5.0F;

        /* '<S2>:1055:5' OWICalibration_Current_Error = 0; */
        /* '<S2>:1055:6' OWICalibration_Best_Turb_Voltage = 0; */
        OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag = 0.0F;

        /* '<S2>:1055:8' OWICalibration_Best_Duty_Cycle = 0; */
        OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Duty_Cycle = 0U;
    }
    else
    {
        guard1 = FALSE;
        switch (OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel)
        {
          case OWICalibrati_IN_BEST_DUTY_CHECK:
            /* During 'BEST_DUTY_CHECK': '<S2>:1003' */
            /* Transition: '<S2>:1033' */
            /* '<S2>:1078:1' sf_internal_predicateOutput = ... */
            /* '<S2>:1078:1' OWICalibration_Best_Turb_Voltage<= (Sf_OWICalibration_Config.OWICalibration_Turb_TargetV*(1+single(Sf_OWICalibration_Config.OWICalibration_Turb_TargetV_Tol)/100)) ...  */
            /* '<S2>:1078:3' &&... */
            /* '<S2>:1078:3' OWICalibration_Best_Turb_Voltage >=(Sf_OWICalibration_Config.OWICalibration_Turb_TargetV*(1-single(Sf_OWICalibration_Config.OWICalibration_Turb_TargetV_Tol)/100)); */
            if ((OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag <=
                 (*Sf_OWICalibration_Config).OWICalibration_Turb_TargetV * (1.0F
                  + (float32)(*Sf_OWICalibration_Config).
                    OWICalibration_Turb_TargetV_Tol / 100.0F)) &&
                    (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag
                     >= (*Sf_OWICalibration_Config).OWICalibration_Turb_TargetV *
                     (1.0F - (float32)(*Sf_OWICalibration_Config).
                      OWICalibration_Turb_TargetV_Tol / 100.0F)))
            {
                /* Transition: '<S2>:1078' */
                /* Transition: '<S2>:1077' */
                /* '<S2>:1077:1' OWICalibration_Status = PASS_FAIL_TYPE.PASS; */
                OWICalibration__SetStatus(PASS);
            }
            else
            {
                /* Transition: '<S2>:1030' */
                /* Transition: '<S2>:1081' */
                /* '<S2>:1081:1' OWICalibration_Status = PASS_FAIL_TYPE.FAIL; */
                OWICalibration__SetStatus(FAIL);
            }

            /* Transition: '<S2>:1075' */
            /* Transition: '<S2>:1074' */
            OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                OWICalibrationModel_IN_EXIT;

            /* Entry Internal 'EXIT': '<S2>:1079' */
            /* Transition: '<S2>:1080' */
            /* '<S2>:1080:1' OWICalibration_State = OWI_CALIB_STATE_TYPE.OWICALIBRATION_STATE_EXIT; */
            OWICalibration__SetState(OWICALIBRATION_STATE_EXIT);

            /* '<S2>:1080:3' OWICalibration_Result = PROCESS_RESULT_TYPE.PROCESS_COMPLETE; */
            OWICalibration__SetProcessResult(PROCESS_COMPLETE);

            /* '<S2>:1080:4' OWICalibration_isRunning = false; */
            OWICalibration__SetRunningFlag(FALSE);
            break;

          case OWICalibrationMode_IN_DUTYCYCLE:
            /* During 'DUTYCYCLE': '<S2>:1005' */
            /* '<S2>:1089:1' sf_internal_predicateOutput = ... */
            /* '<S2>:1089:1' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_DONE; */
            if (OWICalibration__GetTimeoutState() == SYSTEMTIMERS_STATE_DONE)
            {
                /* Transition: '<S2>:1089' */
                /* Transition: '<S2>:1091' */
                /* Transition: '<S2>:1088' */
                guard1 = TRUE;
            }
            else
            {
                /* '<S2>:1087:1' sf_internal_predicateOutput = ... */
                /* '<S2>:1087:1' (OWICalibration_Upper_DutyCycle - OWICalibration_Lower_DutyCycle) ==1 && ... */
                /* '<S2>:1087:2' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_RUNNING; */
                qY = (uint16)
                    (OWICalibrationModel_DW_GLOBAL.OWICalibration_Upper_DutyCycle
                     - /*MW:OvSatOk*/
                     OWICalibrationModel_DW_GLOBAL.OWICalibration_Lower_DutyCycle);
                if (qY >
                        OWICalibrationModel_DW_GLOBAL.OWICalibration_Upper_DutyCycle)
                {
                    qY = 0U;
                }

                if ((qY == 1U) && (OWICalibration__GetTimeoutState() ==
                                   SYSTEMTIMERS_STATE_RUNNING))
                {
                    /* Transition: '<S2>:1087' */
                    guard1 = TRUE;
                }
                else
                {
                    /* Transition: '<S2>:1086' */
                    /* Exit 'DUTYCYCLE': '<S2>:1005' */
                    /* '<S2>:1005:5' OWICalibration_Drive_DutyCycle =OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                    /* '<S2>:1005:5' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                    OWICalibration__SetTurbidityVoltageReading(mVtoV);
                    OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                        OWICalibrationModel_IN_IDLE;

                    /* Entry Internal 'IDLE': '<S2>:1070' */
                    /* Transition: '<S2>:1057' */
                    /* '<S2>:1057:1' OWICalibration_Drive_DutyCycle =... */
                    /* '<S2>:1057:1' OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                    OWICalibration__SetDutyCycle
                        (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

                    /* '<S2>:1057:3' OWICalibration_isRunning = true; */
                    OWICalibration__SetRunningFlag(TRUE);
                }
            }
            break;

          case OWICalibrationModel_IN_EXIT:
            /* During 'EXIT': '<S2>:1079' */
            break;

          case OWICalibrationModel_IN_IDLE:
            /* During 'IDLE': '<S2>:1070' */
            /* '<S2>:1076:1' sf_internal_predicateOutput = ... */
            /* '<S2>:1076:1' OWICalibration_TurbidityVoltage<= (Sf_OWICalibration_Config.OWICalibration_Turb_TargetV*(1+single(Sf_OWICalibration_Config.OWICalibration_Turb_TargetV_Tol)/10000))...  */
            /* '<S2>:1076:3' &&... */
            /* '<S2>:1076:3' OWICalibration_TurbidityVoltage >=(Sf_OWICalibration_Config.OWICalibration_Turb_TargetV*(1-single(Sf_OWICalibration_Config.OWICalibration_Turb_TargetV_Tol)/10000))... */
            /* '<S2>:1076:5' &&... */
            /* '<S2>:1076:5' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_RUNNING; */
            if ((mVtoV <= (*Sf_OWICalibration_Config).
                    OWICalibration_Turb_TargetV * (1.0F + (float32)
                    (*Sf_OWICalibration_Config).OWICalibration_Turb_TargetV_Tol /
                  10000.0F)) && ((mVtoV >= (*Sf_OWICalibration_Config).
                                  OWICalibration_Turb_TargetV * (1.0F - (float32)
                    (*Sf_OWICalibration_Config).OWICalibration_Turb_TargetV_Tol /
                   10000.0F)) && (OWICalibration__GetTimeoutState() ==
                                  SYSTEMTIMERS_STATE_RUNNING)))
            {
                /* Transition: '<S2>:1076' */
                /* Transition: '<S2>:1073' */
                /* '<S2>:1073:1' OWICalibration_Status = PASS_FAIL_TYPE.PASS; */
                OWICalibration__SetStatus(PASS);

                /* Exit 'IDLE': '<S2>:1070' */
                /* '<S2>:1070:3' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                OWICalibration__SetTurbidityVoltageReading(mVtoV);
                OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                    OWICalibrationModel_IN_EXIT;

                /* Entry Internal 'EXIT': '<S2>:1079' */
                /* Transition: '<S2>:1080' */
                /* '<S2>:1080:1' OWICalibration_State = OWI_CALIB_STATE_TYPE.OWICALIBRATION_STATE_EXIT; */
                OWICalibration__SetState(OWICALIBRATION_STATE_EXIT);

                /* '<S2>:1080:3' OWICalibration_Result = PROCESS_RESULT_TYPE.PROCESS_COMPLETE; */
                OWICalibration__SetProcessResult(PROCESS_COMPLETE);

                /* '<S2>:1080:4' OWICalibration_isRunning = false; */
                OWICalibration__SetRunningFlag(FALSE);
            }
            else
            {
                /* '<S2>:1031:1' sf_internal_predicateOutput = ... */
                /* '<S2>:1031:1' OWICalibration_State == OWI_CALIB_STATE_TYPE.OWICALIBRATION_STATE_STABILIZE... */
                /* '<S2>:1031:2' &&... */
                /* '<S2>:1031:2' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_RUNNING; */
                if ((OWICalibration__GetState() ==
                        OWICALIBRATION_STATE_STABILIZE) &&
                        (OWICalibration__GetTimeoutState() ==
                         SYSTEMTIMERS_STATE_RUNNING))
                {
                    /* Transition: '<S2>:1031' */
                    /* Exit 'IDLE': '<S2>:1070' */
                    /* '<S2>:1070:3' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                    OWICalibration__SetTurbidityVoltageReading(mVtoV);
                    OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                        OWICalibrationMode_IN_STABILIZE;

                    /* Entry Internal 'STABILIZE': '<S2>:1002' */
                    /* Transition: '<S2>:1015' */
                    /* '<S2>:1015:1' OWICalibration_Drive_DutyCycle =OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                    OWICalibration__SetDutyCycle
                        (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

                    /* '<S2>:1015:2' OWICalibration_isRunning = true; */
                    OWICalibration__SetRunningFlag(TRUE);

                    /* '<S2>:1016:1' sf_internal_predicateOutput = ... */
                    /* '<S2>:1016:1' SystemTimers_DelayTimeState ==... */
                    /* '<S2>:1016:1'  SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_RUNNING; */
                    if (OWICalibration__GetDelayTimeState() ==
                            SYSTEMTIMERS_STATE_RUNNING)
                    {
                        /* Transition: '<S2>:1016' */
                        /* Transition: '<S2>:1018' */
                        /* No Action */
                        /* Transition: '<S2>:1021' */
                    }
                    else
                    {
                        /* Transition: '<S2>:1014' */
                        /* '<S2>:1014:1' SystemTimers_DelayTime = ... */
                        /* '<S2>:1014:1' uint32(Sf_OWICalibration_Config.OWICalibration_Calib_TV_Read_Delay)*uint32(1000) */
                        OWICalibration__SetDelayTimeMs((uint32)((uint32)
                            (*Sf_OWICalibration_Config).
                            OWICalibration_Calib_TV_Read_Delay * 1000UL));
                    }

                    /* Transition: '<S2>:1020' */
                    /* '<S2>:1020:1' OWICalibration_State = OWI_CALIB_STATE_TYPE.OWICALIBRATION_STATE_DUTYCYCLE */
                    OWICalibration__SetState(OWICALIBRATION_STATE_DUTYCYCLE);
                }
                else
                {
                    /* '<S2>:1085:1' sf_internal_predicateOutput = ... */
                    /* '<S2>:1085:1' OWICalibration_State == OWI_CALIB_STATE_TYPE.OWICALIBRATION_STATE_DUTYCYCLE ... */
                    /* '<S2>:1085:2' && SystemTimers_DelayTimeState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_DONE ... */
                    /* '<S2>:1085:3' && SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_RUNNING; */
                    if ((OWICalibration__GetState() ==
                            OWICALIBRATION_STATE_DUTYCYCLE) &&
                            (OWICalibration__GetDelayTimeState() ==
                             SYSTEMTIMERS_STATE_DONE) &&
                            (OWICalibration__GetTimeoutState() ==
                             SYSTEMTIMERS_STATE_RUNNING))
                    {
                        /* Transition: '<S2>:1085' */
                        /* Exit 'IDLE': '<S2>:1070' */
                        /* '<S2>:1070:3' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                        OWICalibration__SetTurbidityVoltageReading(mVtoV);
                        OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                            OWICalibrationMode_IN_DUTYCYCLE;

                        /* Entry 'DUTYCYCLE': '<S2>:1005' */
                        /* '<S2>:1005:1' OWICalibration_Current_Error = abs(OWICalibration_TurbidityVoltage - Sf_OWICalibration_Config.OWICalibration_Turb_TargetV); */
                        OWICalibration_Current_Error = fabsf(mVtoV -
                            (*Sf_OWICalibration_Config).
                            OWICalibration_Turb_TargetV);

                        /* '<S2>:1005:3' if OWICalibration_Current_Error < OWICalibration_Prev_Error */
                        if (OWICalibration_Current_Error <
                                OWICalibrationModel_DW_GLOBAL.OWICalibration_Prev_Error)
                        {
                            /* '<S2>:1005:4' OWICalibration_Best_Turb_Voltage = OWICalibration_TurbidityVoltage; */
                            OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag
                                = mVtoV;

                            /* '<S2>:1005:5' OWICalibration_Best_Duty_Cycle = OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                            OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Duty_Cycle
                                =
                                OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle;

                            /* '<S2>:1005:6' OWICalibration_Prev_Error = OWICalibration_Current_Error; */
                            OWICalibrationModel_DW_GLOBAL.OWICalibration_Prev_Error
                                = OWICalibration_Current_Error;
                        }

                        /* Entry Internal 'DUTYCYCLE': '<S2>:1005' */
                        /* Transition: '<S2>:1007' */
                        /* '<S2>:1007:1' OWICalibration_Drive_DutyCycle =... */
                        /* '<S2>:1007:1' OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                        OWICalibration__SetDutyCycle
                            (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

                        /* '<S2>:1007:3' OWICalibration_isRunning = true; */
                        OWICalibration__SetRunningFlag(TRUE);

                        /* '<S2>:1022:1' sf_internal_predicateOutput = ... */
                        /* '<S2>:1022:1' OWICalibration_TurbidityVoltage > Sf_OWICalibration_Config.OWICalibration_Turb_TargetV; */
                        if (mVtoV > (*Sf_OWICalibration_Config).
                                OWICalibration_Turb_TargetV)
                        {
                            /* Transition: '<S2>:1022' */
                            /* Transition: '<S2>:1010' */
                            /* '<S2>:1010:1' OWICalibration_Lower_DutyCycle = OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                            OWICalibrationModel_DW_GLOBAL.OWICalibration_Lower_DutyCycle
                                =
                                OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle;

                            /* Transition: '<S2>:1013' */
                            /* Transition: '<S2>:1054' */
                        }
                        else
                        {
                            /* Transition: '<S2>:1034' */
                            /* '<S2>:1026:1' sf_internal_predicateOutput = ... */
                            /* '<S2>:1026:1' OWICalibration_TurbidityVoltage < Sf_OWICalibration_Config.OWICalibration_Turb_TargetV; */
                            if (mVtoV < (*Sf_OWICalibration_Config).
                                    OWICalibration_Turb_TargetV)
                            {
                                /* Transition: '<S2>:1026' */
                                /* Transition: '<S2>:1056' */
                                /* '<S2>:1056:1' OWICalibration_Upper_DutyCycle = OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                                OWICalibrationModel_DW_GLOBAL.OWICalibration_Upper_DutyCycle
                                    =
                                    OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle;

                                /* Transition: '<S2>:1054' */
                            }
                            else
                            {
                                /* Transition: '<S2>:1058' */
                                /* Transition: '<S2>:1060' */
                            }
                        }

                        /* Transition: '<S2>:1059' */
                        /* '<S2>:1059:1' OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle=... */
                        /* '<S2>:1059:3' (OWICalibration_Upper_DutyCycle + OWICalibration_Lower_DutyCycle) / 2; */
                        qY = (uint16)
                            (OWICalibrationModel_DW_GLOBAL.OWICalibration_Upper_DutyCycle
                             + /*MW:OvSatOk*/
                             OWICalibrationModel_DW_GLOBAL.OWICalibration_Lower_DutyCycle);
                        if (qY <
                                OWICalibrationModel_DW_GLOBAL.OWICalibration_Upper_DutyCycle)
                        {
                            qY = MAX_uint16_T;
                        }

                        OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle
                            = (uint16)rt_roundd((real_T)qY / 2.0);

                        /* '<S2>:1059:4' OWICalibration_State = ... */
                        /* '<S2>:1059:4' OWI_CALIB_STATE_TYPE.OWICALIBRATION_STATE_STABILIZE; */
                        OWICalibration__SetState(OWICALIBRATION_STATE_STABILIZE);
                    }
                    else
                    {
                        /* '<S2>:1090:1' sf_internal_predicateOutput = ... */
                        /* '<S2>:1090:1' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_DONE; */
                        if (OWICalibration__GetTimeoutState() ==
                                SYSTEMTIMERS_STATE_DONE)
                        {
                            /* Transition: '<S2>:1090' */
                            /* Transition: '<S2>:1091' */
                            /* Transition: '<S2>:1088' */
                            /* Transition: '<S2>:1084' */
                            /* Transition: '<S2>:1082' */
                            /* Exit 'IDLE': '<S2>:1070' */
                            /* '<S2>:1070:3' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                            OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel
                                = OWICalibrati_IN_BEST_DUTY_CHECK;

                            /* Entry 'BEST_DUTY_CHECK': '<S2>:1003' */
                            /* '<S2>:1003:1' OWICalibration_Drive_DutyCycle = OWICalibration_Best_Duty_Cycle; */
                            OWICalibration__SetDutyCycle
                                (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Duty_Cycle);

                            /* '<S2>:1003:2' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_Best_Turb_Voltage; */
                            OWICalibration__SetTurbidityVoltageReading
                                (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag);
                        }
                        else
                        {
                            /* '<S2>:1070:1' OWICalibration_Drive_DutyCycle =... */
                            /* '<S2>:1070:1'     OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                            OWICalibration__SetDutyCycle
                                (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

                            /* '<S2>:1070:3' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                            OWICalibration__SetTurbidityVoltageReading(mVtoV);
                        }
                    }
                }
            }
            break;

          case OWICalibrationModel_IN_INITIAL:
            /* During 'INITIAL': '<S2>:1055' */
            /* Transition: '<S2>:1072' */
            OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                OWICalibrationModel_IN_IDLE;

            /* Entry Internal 'IDLE': '<S2>:1070' */
            /* Transition: '<S2>:1057' */
            /* '<S2>:1057:1' OWICalibration_Drive_DutyCycle =... */
            /* '<S2>:1057:1' OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
            OWICalibration__SetDutyCycle
                (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

            /* '<S2>:1057:3' OWICalibration_isRunning = true; */
            OWICalibration__SetRunningFlag(TRUE);
            break;

          default:
            /* During 'STABILIZE': '<S2>:1002' */
            /* '<S2>:1032:1' sf_internal_predicateOutput = ... */
            /* '<S2>:1032:1' SystemTimers_DelayTimeState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_DONE... */
            /* '<S2>:1032:2' &&... */
            /* '<S2>:1032:2' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_RUNNING; */
            if ((OWICalibration__GetDelayTimeState() == SYSTEMTIMERS_STATE_DONE)
                && (OWICalibration__GetTimeoutState() ==
                    SYSTEMTIMERS_STATE_RUNNING))
            {
                /* Transition: '<S2>:1032' */
                OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                    OWICalibrationModel_IN_IDLE;

                /* Entry Internal 'IDLE': '<S2>:1070' */
                /* Transition: '<S2>:1057' */
                /* '<S2>:1057:1' OWICalibration_Drive_DutyCycle =... */
                /* '<S2>:1057:1' OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                OWICalibration__SetDutyCycle
                    (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

                /* '<S2>:1057:3' OWICalibration_isRunning = true; */
                OWICalibration__SetRunningFlag(TRUE);
            }
            else
            {
                /* '<S2>:1083:1' sf_internal_predicateOutput = ... */
                /* '<S2>:1083:1' SystemTimers_TimeoutState == SYSTEMTIMERS_STATE_TYPE.SYSTEMTIMERS_STATE_DONE; */
                if (OWICalibration__GetTimeoutState() == SYSTEMTIMERS_STATE_DONE)
                {
                    /* Transition: '<S2>:1083' */
                    /* Transition: '<S2>:1082' */
                    OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                        OWICalibrati_IN_BEST_DUTY_CHECK;

                    /* Entry 'BEST_DUTY_CHECK': '<S2>:1003' */
                    /* '<S2>:1003:1' OWICalibration_Drive_DutyCycle = OWICalibration_Best_Duty_Cycle; */
                    OWICalibration__SetDutyCycle
                        (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Duty_Cycle);

                    /* '<S2>:1003:2' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_Best_Turb_Voltage; */
                    OWICalibration__SetTurbidityVoltageReading
                        (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag);
                }
                else
                {
                    /* '<S2>:1002:1' OWICalibration_Drive_DutyCycle =OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
                    OWICalibration__SetDutyCycle
                        (OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle);

                    /* '<S2>:1002:2' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
                    OWICalibration__SetTurbidityVoltageReading(mVtoV);
                }
            }
            break;
        }

        if (guard1)
        {
            /* Transition: '<S2>:1084' */
            /* Transition: '<S2>:1082' */
            /* Exit 'DUTYCYCLE': '<S2>:1005' */
            /* '<S2>:1005:5' OWICalibration_Drive_DutyCycle =OWICalibration_Model_Data.OWICalibration_Drive_Duty_Cycle; */
            /* '<S2>:1005:5' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_TurbidityVoltage; */
            OWICalibrationModel_DW_GLOBAL.is_c3_OWICalibrationModel =
                OWICalibrati_IN_BEST_DUTY_CHECK;

            /* Entry 'BEST_DUTY_CHECK': '<S2>:1003' */
            /* '<S2>:1003:1' OWICalibration_Drive_DutyCycle = OWICalibration_Best_Duty_Cycle; */
            OWICalibration__SetDutyCycle
                (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Duty_Cycle);

            /* '<S2>:1003:2' OWICalibration_Turbidity_Voltage_Reading = OWICalibration_Best_Turb_Voltage; */
            OWICalibration__SetTurbidityVoltageReading
                (OWICalibrationModel_DW_GLOBAL.OWICalibration_Best_Turb_Voltag);
        }
    }

    /* End of Chart: '<S1>/Chart' */

    /* Outputs for Atomic SubSystem: '<S1>/Subsystem1' */
    /* DataStoreRead: '<S4>/Data Store Read' */
    OWICalibration__SetModelData(OWICalibration_Model_Data);

    /* End of Outputs for SubSystem: '<S1>/Subsystem1' */
    /* End of Outputs for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_OWICalibration__Model_at_outport_1' */
}

/** Model initialize function */
void OWICalibrationModel_initialize(void)
{
    /* SystemInitialize for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_OWICalibration__Model_at_outport_1' incorporates:
     *  SystemInitialize for SubSystem: '<Root>/OWI_Calibration'
     */
    /* SystemInitialize for Chart: '<S1>/Chart' */
    OWICalibration__SetStatus(FAIL);

    /* End of SystemInitialize for RootInportFunctionCallGenerator: '<Root>/RootFcnCall_InsertedFor_OWICalibration__Model_at_outport_1' */
}
