/*
 * File: OWIVoltageRegulatedOutputModel.c
 *
 * Code generated for Simulink model 'OWIVoltageRegulatedOutputModel'.
 *
 * Model version                  : 1.39
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Tue Aug  2 17:21:29 2022
 *
 * Target selection: monitor.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "OWIVoltageRegulatedOutputModel.h"
#include "OWIVoltageRegulatedOutputModel_private.h"

/* Exported data definition */

/* Definition for custom storage class: ExportGlobal */
uint16 OWI_VRO_DutyCycle_Comp;         /* '<S7>/Model' */
uint16 OWI_VRO_DutyCycle_Stable;       /* '<S7>/Model' */

/* Definition for custom storage class: FileScope */
static OWI_VRO_STATEMACHINE_TYPE OWI_VRO_StateMachine;/* '<S7>/Model' */

/* Definition of data with custom storage class Loader */
static SETTINGFILE_LOADER_TYPE loader; /* '<S17>/LoadSettingsFileData' */

/* Definition of data with custom storage class Pointer */
static OWI_VRO_COMMON_SF_PARAMS_TYPE *PARAM_OWIVROData;/* '<S22>/CastWithOffset' */
static uint16 *PARAM_OWIVROLinearizationData_DutyCycle;/* '<S20>/CastWithOffset1' */
static uint16 *PARAM_OWIVROLinearizationData_Multiplier;/* '<S20>/CastWithOffset2' */
static OWI_VRO_SETPOINT_SF_PARAMS_TYPE *PARAM_OWIVROSetpoint1Data;/* '<S30>/CastWithOffset' */
static OWI_VRO_SETPOINT_SF_PARAMS_TYPE *PARAM_OWIVROSetpoint2Data;/* '<S27>/CastWithOffset' */
static OWI_VRO_SETPOINT_SF_PARAMS_TYPE *PARAM_OWIVROSetpoint3Data;/* '<S29>/CastWithOffset' */

/* Definition for custom storage class: FileScope */
static uint32 OWI_VRO_Voltage_Reading; /* '<S10>/Data Type Conversion' */

/* Debug information: Voltage read during the search */
static float32 OWI_VRO_Voltage_Stable; /* '<S7>/Model' */

/* Debug information: Voltage reading when state machine went to complete. If the algorithm is working properly, should be always around the setpoint. Unless a saturation has occured. */

/* Definition for custom storage class: Localizable */
static uint8 OWI_VRO_DutyCycle_Sequence_Id;/* synthesized block */
BOOL_TYPE OWI_VRO_Reset;               /* synthesized block */
static uint8 OWI_VRO_SetPoint_Index;   /* synthesized block */
static BOOL_TYPE OWI_VRO_Stop;         /* synthesized block */
static uint8 PARAM_OWIVROLinearizationNumItems;/* '<S20>/CastWithOffset' */
static uint16 PARAM_OWIVRO_LinearizationTable_DutyCycle[MAX_LINEARIZATION_TABLE];/* synthesized block */
static uint16 PARAM_OWIVRO_LinearizationTable_Multiplier[MAX_LINEARIZATION_TABLE];/* synthesized block */
static BOOL_TYPE SF_OWIVROAllParamPass;/* '<S19>/AND' */

/*
 * Output and update for action system:
 *    '<S18>/If Action Subsystem2'
 *    '<S18>/If Action Subsystem5'
 *    '<S18>/If Action Subsystem6'
 */
void IfActionSubsystem2(RT_MODELOWIVOLTAGEREGULATE_TYPE * const m, const
  OWI_VRO_SETPOINT_SF_PARAMS_TYPE *in1)
{
  UNUSED_PARAMETER(m);

  /* S-Function (sfun_assignNULLToPointer): '<S28>/AssignNULL2Pointer' */
  in1 = NULL;
}

/* Model step function */
void GetSequenceId(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m, uint8
                   *y_owi_vro_dutycycle_sequence_id)
{
  /* RootInportFunctionCallGenerator generated from: '<Root>/GetSequenceId' incorporates:
   *  SubSystem: '<Root>/Get_SequenceId'
   */
  /* Outport: '<Root>/OWI_VRO_DutyCycle_Sequence_Id' incorporates:
   *  DataStoreRead: '<S3>/Data Store Read'
   */
  *y_owi_vro_dutycycle_sequence_id = OWI_VRO_DutyCycle_Sequence_Id;

  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/GetSequenceId' */
  UNUSED_PARAMETER(m);
}

/* Model step function */
void Initialize(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m)
{
  /* local block i/o variables */
  uint16 Length;
  BOOL_TYPE pass_fail_PARAM_OWIVROLineariza;
  BOOL_TYPE pass_fail_PARAM_OWIVROData;
  BOOL_TYPE pass_fail_PARAM_OWIVROSetPoint3;
  BOOL_TYPE pass_fail_PARAM_OWIVROSetPoint2;
  BOOL_TYPE pass_fail_PARAM_OWIVROSetPoint1;
  uint8 Add;

  /* RootInportFunctionCallGenerator generated from: '<Root>/Initialize' incorporates:
   *  SubSystem: '<Root>/Read_SettingFile_Params'
   */
  /* S-Function (sfun_LoadSettingsFile): '<S16>/LoadSettingsFileData1' incorporates:
   *  Constant: '<S16>/Constant2'
   *  Constant: '<S16>/Constant3'
   */
  pass_fail_PARAM_OWIVROLineariza = (SettingFile__MSettingBasicLoader(((uint8)0U),
    SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_OWI_VRO_LINEARIZATION_TABLE, &loader)
    == PASS);
  Length = (&loader)->Length;

  /* Logic: '<S16>/AND' incorporates:
   *  Constant: '<S16>/Constant'
   *  RelationalOperator: '<S16>/Equal'
   */
  pass_fail_PARAM_OWIVROLineariza = ((((sint32)Length) >= 6) &&
    pass_fail_PARAM_OWIVROLineariza);

  /* If: '<S16>/If1' */
  if (pass_fail_PARAM_OWIVROLineariza) {
    /* Outputs for IfAction SubSystem: '<S16>/If Action Subsystem2' incorporates:
     *  ActionPort: '<S20>/Action Port'
     */
    /* S-Function (sfun_castWithOffset): '<S20>/CastWithOffset1' incorporates:
     *  Constant: '<S20>/Constant1'
     */
    PARAM_OWIVROLinearizationData_DutyCycle = (uint16*)(void*)(loader.Data +
      ((uint8)2U));

    /* S-Function (sfun_castWithOffset): '<S20>/CastWithOffset' incorporates:
     *  Constant: '<S20>/Constant'
     */
    PARAM_OWIVROLinearizationNumItems = *(uint8*)(void*)(loader.Data + ((uint8)
      1U));

    /* S-Function (sfun_assignPointer2DSMArray): '<S20>/AssignPointer2DSMArray' */
    memcpy((&(PARAM_OWIVRO_LinearizationTable_DutyCycle[0])),
           PARAM_OWIVROLinearizationData_DutyCycle, sizeof(uint16)*
           PARAM_OWIVROLinearizationNumItems);

    /* Sum: '<S20>/Add' incorporates:
     *  Constant: '<S20>/Constant1'
     *  Gain: '<S20>/Gain'
     */
    Add = (uint8)((uint32)(((uint32)((uint8)((sint32)(((sint32)
      PARAM_OWIVROLinearizationNumItems) * 2)))) + 2U));

    /* S-Function (sfun_castWithOffset): '<S20>/CastWithOffset2' */
    PARAM_OWIVROLinearizationData_Multiplier = (uint16*)(void*)(loader.Data +
      Add);

    /* S-Function (sfun_assignPointer2DSMArray): '<S20>/AssignPointer2DSMArray1' */
    memcpy((&(PARAM_OWIVRO_LinearizationTable_Multiplier[0])),
           PARAM_OWIVROLinearizationData_Multiplier, sizeof(uint16)*
           PARAM_OWIVROLinearizationNumItems);

    /* End of Outputs for SubSystem: '<S16>/If Action Subsystem2' */
  } else {
    /* Outputs for IfAction SubSystem: '<S16>/If Action Subsystem3' incorporates:
     *  ActionPort: '<S21>/Action Port'
     */
    /* S-Function (sfun_assignNULLToPointer): '<S21>/AssignNULL2Pointer' */
    PARAM_OWIVROLinearizationData_DutyCycle = NULL;

    /* S-Function (sfun_assignNULLToPointer): '<S21>/AssignNULL2Pointer1' */
    PARAM_OWIVROLinearizationData_Multiplier = NULL;

    /* End of Outputs for SubSystem: '<S16>/If Action Subsystem3' */
  }

  /* End of If: '<S16>/If1' */
  /* S-Function (sfun_LoadSettingsFile): '<S17>/LoadSettingsFileData' incorporates:
   *  Constant: '<S17>/Constant'
   *  Constant: '<S17>/Constant1'
   */
  pass_fail_PARAM_OWIVROData = (SettingFile__MSettingBasicLoader(((uint8)0U),
    SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_OWI_VRO_COMMON_PARAMS, &loader) ==
    PASS);
  Length = (&loader)->Length;

  /* Logic: '<S17>/AND' incorporates:
   *  RelationalOperator: '<S17>/Equal'
   *  S-Function (sfun_SizeOf): '<S17>/sizeOf'
   */
  pass_fail_PARAM_OWIVROData = ((((uint16)sizeof(OWI_VRO_COMMON_SF_PARAMS_TYPE))
    == Length) && pass_fail_PARAM_OWIVROData);

  /* If: '<S17>/If' */
  if (pass_fail_PARAM_OWIVROData) {
    /* Outputs for IfAction SubSystem: '<S17>/If Action Subsystem' incorporates:
     *  ActionPort: '<S22>/Action Port'
     */
    /* S-Function (sfun_castWithOffset): '<S22>/CastWithOffset' incorporates:
     *  Constant: '<S22>/Constant'
     */
    PARAM_OWIVROData = (OWI_VRO_COMMON_SF_PARAMS_TYPE*)(void*)(loader.Data +
      ((uint16)0U));

    /* End of Outputs for SubSystem: '<S17>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S17>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S23>/Action Port'
     */
    /* S-Function (sfun_assignNULLToPointer): '<S23>/AssignNULL2Pointer' */
    PARAM_OWIVROData = NULL;

    /* End of Outputs for SubSystem: '<S17>/If Action Subsystem1' */
  }

  /* End of If: '<S17>/If' */
  /* S-Function (sfun_LoadSettingsFile): '<S18>/LoadSettingsFileData1' incorporates:
   *  Constant: '<S18>/Constant1'
   *  Constant: '<S25>/Constant'
   */
  pass_fail_PARAM_OWIVROSetPoint3 = (SettingFile__MSettingBasicLoader(((uint8)0U),
    SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_OWI_VRO_SETPOINT_2_PARAMS, &loader) ==
    PASS);
  Length = (&loader)->Length;

  /* Logic: '<S18>/AND1' incorporates:
   *  RelationalOperator: '<S18>/Equal1'
   *  S-Function (sfun_SizeOf): '<S18>/sizeOf1'
   */
  pass_fail_PARAM_OWIVROSetPoint2 = ((((uint16)sizeof
    (OWI_VRO_SETPOINT_SF_PARAMS_TYPE)) == Length) &&
    pass_fail_PARAM_OWIVROSetPoint3);

  /* If: '<S18>/If1' */
  if (pass_fail_PARAM_OWIVROSetPoint2) {
    /* Outputs for IfAction SubSystem: '<S18>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S27>/Action Port'
     */
    /* S-Function (sfun_castWithOffset): '<S27>/CastWithOffset' incorporates:
     *  Constant: '<S27>/Constant'
     */
    PARAM_OWIVROSetpoint2Data = (OWI_VRO_SETPOINT_SF_PARAMS_TYPE*)(void*)
      (loader.Data + ((uint16)0U));

    /* End of Outputs for SubSystem: '<S18>/If Action Subsystem1' */
  } else {
    /* Outputs for IfAction SubSystem: '<S18>/If Action Subsystem2' incorporates:
     *  ActionPort: '<S28>/Action Port'
     */
    IfActionSubsystem2(m, PARAM_OWIVROSetpoint2Data);

    /* End of Outputs for SubSystem: '<S18>/If Action Subsystem2' */
  }

  /* End of If: '<S18>/If1' */
  /* S-Function (sfun_LoadSettingsFile): '<S18>/LoadSettingsFileData2' incorporates:
   *  Constant: '<S18>/Constant6'
   *  Constant: '<S24>/Constant'
   */
  pass_fail_PARAM_OWIVROSetPoint3 = (SettingFile__MSettingBasicLoader(((uint8)0U),
    SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_OWI_VRO_SETPOINT_1_PARAMS, &loader) ==
    PASS);
  Length = (&loader)->Length;

  /* Logic: '<S18>/AND' incorporates:
   *  RelationalOperator: '<S18>/Equal'
   *  S-Function (sfun_SizeOf): '<S18>/sizeOf'
   */
  pass_fail_PARAM_OWIVROSetPoint1 = ((((uint16)sizeof
    (OWI_VRO_SETPOINT_SF_PARAMS_TYPE)) == Length) &&
    pass_fail_PARAM_OWIVROSetPoint3);

  /* If: '<S18>/If2' */
  if (pass_fail_PARAM_OWIVROSetPoint1) {
    /* Outputs for IfAction SubSystem: '<S18>/If Action Subsystem4' incorporates:
     *  ActionPort: '<S30>/Action Port'
     */
    /* S-Function (sfun_castWithOffset): '<S30>/CastWithOffset' incorporates:
     *  Constant: '<S30>/Constant'
     */
    PARAM_OWIVROSetpoint1Data = (OWI_VRO_SETPOINT_SF_PARAMS_TYPE*)(void*)
      (loader.Data + ((uint16)0U));

    /* End of Outputs for SubSystem: '<S18>/If Action Subsystem4' */
  } else {
    /* Outputs for IfAction SubSystem: '<S18>/If Action Subsystem5' incorporates:
     *  ActionPort: '<S31>/Action Port'
     */
    IfActionSubsystem2(m, PARAM_OWIVROSetpoint1Data);

    /* End of Outputs for SubSystem: '<S18>/If Action Subsystem5' */
  }

  /* End of If: '<S18>/If2' */
  /* S-Function (sfun_LoadSettingsFile): '<S18>/LoadSettingsFileData3' incorporates:
   *  Constant: '<S18>/Constant2'
   *  Constant: '<S26>/Constant'
   */
  pass_fail_PARAM_OWIVROSetPoint3 = (SettingFile__MSettingBasicLoader(((uint8)0U),
    SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_OWI_VRO_SETPOINT_3_PARAMS, &loader) ==
    PASS);
  Length = (&loader)->Length;

  /* Logic: '<S18>/AND2' incorporates:
   *  RelationalOperator: '<S18>/Equal2'
   *  S-Function (sfun_SizeOf): '<S18>/sizeOf2'
   */
  pass_fail_PARAM_OWIVROSetPoint3 = ((((uint16)sizeof
    (OWI_VRO_SETPOINT_SF_PARAMS_TYPE)) == Length) &&
    pass_fail_PARAM_OWIVROSetPoint3);

  /* If: '<S18>/If3' */
  if (pass_fail_PARAM_OWIVROSetPoint3) {
    /* Outputs for IfAction SubSystem: '<S18>/If Action Subsystem3' incorporates:
     *  ActionPort: '<S29>/Action Port'
     */
    /* S-Function (sfun_castWithOffset): '<S29>/CastWithOffset' incorporates:
     *  Constant: '<S29>/Constant'
     */
    PARAM_OWIVROSetpoint3Data = (OWI_VRO_SETPOINT_SF_PARAMS_TYPE*)(void*)
      (loader.Data + ((uint16)0U));

    /* End of Outputs for SubSystem: '<S18>/If Action Subsystem3' */
  } else {
    /* Outputs for IfAction SubSystem: '<S18>/If Action Subsystem6' incorporates:
     *  ActionPort: '<S32>/Action Port'
     */
    IfActionSubsystem2(m, PARAM_OWIVROSetpoint3Data);

    /* End of Outputs for SubSystem: '<S18>/If Action Subsystem6' */
  }

  /* End of If: '<S18>/If3' */

  /* Logic: '<S19>/AND' incorporates:
   *  Logic: '<S18>/AND3'
   */
  SF_OWIVROAllParamPass = ((((pass_fail_PARAM_OWIVROSetPoint1 &&
    pass_fail_PARAM_OWIVROSetPoint2) && pass_fail_PARAM_OWIVROSetPoint3) &&
    pass_fail_PARAM_OWIVROData) && pass_fail_PARAM_OWIVROLineariza);

  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/Initialize' */
}

/* Model step function */
void Monitor(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m)
{
  /* local block i/o variables */
  sint32 owi_sensor_reading_a;
  uint16 OWI_VRO_Avg_Coeff_SP;
  uint8 GetLoadIndexByLoadID;
  uint8 owi_index_l;
  uint8 GetPilotIDByIndex;
  HBL_PILOT_ENUM_TYPE OWI_Pilot_Type;

  /* RootInportFunctionCallGenerator generated from: '<Root>/Monitor' incorporates:
   *  SubSystem: '<Root>/Call_OWI_VRO_Monitor'
   */
  /* Outputs for Enabled SubSystem: '<S1>/Enable_OWI_VRO_Monitor' incorporates:
   *  EnablePort: '<S7>/Enable'
   */
  /* SignalConversion generated from: '<S7>/Enable' */
  if (SF_OWIVROAllParamPass) {
    /* DataStoreRead: '<S7>/Data Store Read1' */
    m->blockIO.owi_vro_reset = OWI_VRO_Reset;

    /* S-Function (sfun_HblGetLoadIndexByLoadID): '<S9>/GetLoadIndexByLoadID' incorporates:
     *  Constant: '<S12>/Constant'
     */
    GetLoadIndexByLoadID = Hbl__GetLoadIndexByLoadID(HBL_LOAD_OWI);

    /* S-Function (sfun_HblGetPilotIDByIndex): '<S9>/GetPilotIDByIndex' */
    GetPilotIDByIndex = Hbl__GetPilotIDByIndex(GetLoadIndexByLoadID);

    /* DataTypeConversion: '<S9>/Data Type Conversion' */
    OWI_Pilot_Type = (HBL_PILOT_ENUM_TYPE)GetPilotIDByIndex;

    /* CCaller: '<S8>/C Caller' incorporates:
     *  DataStoreRead: '<S8>/Data Store Read'
     */
    OWI_VRO_Avg_Coeff_SP = OwiVroRead__GetAvgCoeff((uint8)OWI_VRO_SetPoint_Index);

    /* S-Function (sfun_HblGetGIIndexByGIID): '<S10>/owi_index' incorporates:
     *  Constant: '<S13>/Constant'
     */
    owi_index_l = Hbl__GetGIIndexByGIID(HBL_GI_TURBIDITY_SENSOR_OWI_VRO);

    /* S-Function (sfun_HblGetGIDataConvertedByIndex): '<S10>/owi_sensor_reading' */
    owi_sensor_reading_a = Hbl__GetGIDataConvertedByIndex(owi_index_l);

    /* DataTypeConversion: '<S10>/Data Type Conversion' */
    OWI_VRO_Voltage_Reading = (uint32)owi_sensor_reading_a;

    /* ModelReference generated from: '<S7>/Model' incorporates:
     *  DataStoreRead: '<S7>/Data Store Read'
     *  DataStoreRead: '<S7>/Data Store Read2'
     *  DataStoreRead: '<S7>/Data Store Read3'
     *  DataStoreRead: '<S7>/Data Store Read4'
     *  DataStoreWrite: '<S7>/Data Store Write'
     */
    OWI_VRO_Monitor__Execute(OWI_VRO_Stop, (m)->blockIO.owi_vro_reset,
      OWI_Pilot_Type, OWI_VRO_Avg_Coeff_SP, OWI_VRO_Voltage_Reading,
      OWI_VRO_SetPoint_Index, PARAM_OWIVROSetpoint1Data->Voltage_Setpoint,
      PARAM_OWIVROSetpoint1Data->Compensation_Exponent,
      PARAM_OWIVROSetpoint2Data->Voltage_Setpoint,
      PARAM_OWIVROSetpoint2Data->Compensation_Exponent,
      PARAM_OWIVROSetpoint3Data->Voltage_Setpoint,
      PARAM_OWIVROSetpoint3Data->Compensation_Exponent, PARAM_OWIVROData,
      PARAM_OWIVROLinearizationNumItems,
      (&(PARAM_OWIVRO_LinearizationTable_DutyCycle[0])),
      (&(PARAM_OWIVRO_LinearizationTable_Multiplier[0])), &(m)
      ->blockIO.search_dutycycle, (&(OWI_VRO_DutyCycle_Stable)),
      (&(OWI_VRO_DutyCycle_Comp)), (&(OWI_VRO_DutyCycle_Sequence_Id)),
      (&(OWI_VRO_StateMachine)), (&(OWI_VRO_Voltage_Stable)));

    /* S-Function (sfun_HblSetLoadRequestByLoadID): '<S11>/SetLoadRequestByLoadID' incorporates:
     *  Constant: '<S14>/Constant'
     *  Constant: '<S15>/Constant'
     */
    (void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_OWI, HBL_PRIORITY_APPLICATION_HI,
      (uint8 *)(void *)&(m)->blockIO.search_dutycycle);
  }

  /* End of SignalConversion generated from: '<S7>/Enable' */
  /* End of Outputs for SubSystem: '<S1>/Enable_OWI_VRO_Monitor' */
  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/Monitor' */
}

/* Model step function */
void Reset(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m, uint8 u_setpoint_index)
{
  /* RootInportFunctionCallGenerator generated from: '<Root>/Reset' incorporates:
   *  SubSystem: '<Root>/Reset_Monitor'
   */
  /* DataStoreWrite: '<S5>/Data Store Write' incorporates:
   *  Inport: '<Root>/setpoint_index'
   */
  OWI_VRO_SetPoint_Index = u_setpoint_index;

  /* DataStoreWrite: '<S5>/Data Store Write1' incorporates:
   *  Constant: '<S5>/Constant'
   */
  OWI_VRO_Reset = true;

  /* DataStoreWrite: '<S5>/Data Store Write2' incorporates:
   *  Constant: '<S5>/Constant1'
   */
  OWI_VRO_Stop = false;

  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/Reset' */
  UNUSED_PARAMETER(m);
}

/* Model step function */
void Stop(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m)
{
  /* RootInportFunctionCallGenerator generated from: '<Root>/Stop' incorporates:
   *  SubSystem: '<Root>/Stop_Monitor'
   */
  /* DataStoreWrite: '<S6>/Data Store Write1' incorporates:
   *  Constant: '<S6>/Constant'
   */
  OWI_VRO_Stop = true;

  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/Stop' */
  UNUSED_PARAMETER(m);
}

/* Model initialize function */
void OWIVoltageRegulatedOutputModel__Initialize(RT_MODELOWIVOLTAGEREGULATE_TYPE *
  const m)
{
  /* Registration code */
  {
    /* block I/O */

    /* custom signals */
    memset(&loader, 0, sizeof(SETTINGFILE_LOADER_TYPE));
    PARAM_OWIVROSetpoint3Data = NULL;
    PARAM_OWIVROSetpoint1Data = NULL;
    PARAM_OWIVROSetpoint2Data = NULL;
    PARAM_OWIVROData = NULL;
    OWI_VRO_Voltage_Reading = 0U;
    OWI_VRO_Voltage_Stable = 0.0F;
    OWI_VRO_StateMachine = OWI_VRO_STATEMACHINE_TYPE_None;
    PARAM_OWIVROLinearizationData_DutyCycle = NULL;
    PARAM_OWIVROLinearizationData_Multiplier = NULL;
    PARAM_OWIVROLinearizationNumItems = 0U;
    SF_OWIVROAllParamPass = false;

    /* states (dwork) */

    /* custom states */
    {
      sint32 i;
      for (i = 0; i < MAX_LINEARIZATION_TABLE; i++) {
        PARAM_OWIVRO_LinearizationTable_DutyCycle[i] = 0U;
      }
    }

    {
      sint32 i;
      for (i = 0; i < MAX_LINEARIZATION_TABLE; i++) {
        PARAM_OWIVRO_LinearizationTable_Multiplier[i] = 0U;
      }
    }

    OWI_VRO_DutyCycle_Sequence_Id = 0U;
    OWI_VRO_SetPoint_Index = 0U;
    OWI_VRO_Reset = false;
    OWI_VRO_Stop = false;
  }

  /* Start for DataStoreMemory generated from: '<S7>/Data Store Read2' */
  OWI_VRO_Stop = true;
  UNUSED_PARAMETER(m);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
