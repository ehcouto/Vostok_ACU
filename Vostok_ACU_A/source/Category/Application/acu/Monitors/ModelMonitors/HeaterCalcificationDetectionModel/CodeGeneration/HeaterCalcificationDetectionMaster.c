/**
 * @file: HeaterCalcificationDetectionMaster.c
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
//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "HeaterCalcificationDetectionMaster.h"

//-------------------------------------- PRIVATE (Constants & Defines) ------------------------------------------------

//-------------------------------------- PRIVATE (Types) --------------------------------------------------------------

//-------------------------------------- PRIVATE (Enumerations) -------------------------------------------------------

//--------------------------------- PUBLIC AND PRIVATE(Variables) -----------------------------------------------------

/* Exported data definition */

/* Definition for custom storage class: FileScope */
static uint16 HCD_Avg_Heater_Current;  /* '<Root>/HCD_MasterModel' */

/* Average Heater Current during normal work of the heater */
static uint16 HCD_Heater_Energy;       /* '<Root>/HCD_MasterModel' */

/* Machine analysis data */
static uint16 HCD_Heater_Power;        /* '<Root>/HCD_MasterModel' */
static uint16 HCD_Unstable_Clicks;     /* '<Root>/HCD_MasterModel' */

/* Debug variable: heater current debouncing while the hydraulic environment in the machine was unstable. */
static BOOL_TYPE HCD_Water_Stable;     /* '<Root>/HCD_MasterModel' */

/* Auxiliary variable: Stability in the system based on VWI status (VWI = 4 means water stable, anything else - unstable). */
static BOOL_TYPE Heater_Circuit_OK;    /* '<S16>/Compare' */

//=====================================================================================================================
//----------------------------------------- Functions -----------------------------------------------------------------
//=====================================================================================================================

/** Model step function */
void HeaterCalcificationDetectionMaster__Execute(RT_MODELHEATERCALCIFICATIO_TYPE
    *const m, BOOL_TYPE u_hcd_enable, uint16 u_heater_current_threshold,
    HEATER_CALCIFICATION_PARAMS_TYPE *u_sf_hcd_parameters, BOOL_TYPE
    u_hcd_cycle_started, uint16 *y_hcd_lowest_click_temperature, uint16
    *y_hcd_lowest_click_current, uint16 *y_hcd_lowest_click_power, uint16
    *y_hcd_reduction_temperature, uint16 *y_hcd_reduction_current, uint16
    *y_hcd_reduction_power, HCD_MONITORSTATE_TYPE *y_hcd_monitoring_states)
{
    DWHEATERCALCIFICATIONDETEC_TYPE *dw = ((DWHEATERCALCIFICATIONDETEC_TYPE *)
        m->dwork);

    /* local block i/o variables */
    HBL_LOAD_STATUS_TYPE heater_status_ptr;
    pVoid GetLoadStatusByIndex;
    sint32 GetGIDataConvertedByIndex;
    sint32 GetGIDataConvertedByIndex_n;
    uint8 GetLoadIndexByLoadID;
    uint8 GetGIIndexByGIID;
    uint8 GetGIIndexByGIID_b;
    VWI_WATER_SENSE_TYPE Vwi_Status;
    DOOR_STATUS_TYPE Door_Status;
    BOOL_TYPE HCD_Enable;

    /* S-Function (sfun_HblGetLoadIndexByLoadID): '<S5>/GetLoadIndexByLoadID' incorporates:
     *  Constant: '<S9>/Constant'
     */
    GetLoadIndexByLoadID = Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASHING_HEATER);

    /* S-Function (sfun_HblGetLoadStatusByIndex): '<S5>/GetLoadStatusByIndex' */
    GetLoadStatusByIndex = (pVoid)Hbl__GetLoadStatusByIndex(GetLoadIndexByLoadID);

    /* S-Function (sfun_HblGetGIIndexByGIID): '<S6>/GetGIIndexByGIID' incorporates:
     *  Constant: '<S17>/Constant'
     */
    GetGIIndexByGIID = Hbl__GetGIIndexByGIID(HBL_GI_I_HEATER);

    /* RelationalOperator: '<S16>/Compare' incorporates:
     *  Constant: '<S16>/Constant'
     *  S-Function (sfun_HblGetGISequenceIDByIndex): '<S6>/GetGISequenceIDByIndex'
     */
    Heater_Circuit_OK = (((sint32)((uint8)Hbl__GetGISequenceIDByIndex
                           (GetGIIndexByGIID))) > 0);

    /* If: '<S5>/If' incorporates:
     *  S-Function (sfun_notNULL): '<S5>/PointerNotNULLCheck'
     */
    if ((*&GetLoadStatusByIndex != NULL))
    {
        /* Outputs for IfAction SubSystem: '<S5>/read_load_status_if' incorporates:
         *  ActionPort: '<S10>/Action Port'
         */
        /* S-Function (sfun_pVoidToBus): '<S10>/pVoid2Bus' */
        heater_status_ptr = *((HBL_LOAD_STATUS_TYPE *)GetLoadStatusByIndex);

        /* If: '<S10>/If2' incorporates:
         *  Constant: '<S10>/Zero'
         *  Constant: '<S11>/Constant'
         *  Constant: '<S12>/Constant'
         *  Constant: '<S13>/Constant'
         *  Inport: '<S14>/In1'
         *  Inport: '<S15>/In1'
         *  Logic: '<S10>/AND'
         *  RelationalOperator: '<S10>/Equal'
         *  RelationalOperator: '<S10>/Equal1'
         */
        if (((((sint32)heater_status_ptr.Load.Cur_Reg) != 0) &&
                (heater_status_ptr.Load.Status ==
                 HBL_LOAD_STATUS_TARGET_COMPLETED)) && (Heater_Circuit_OK))
        {
            /* Outputs for IfAction SubSystem: '<S10>/If Action Subsystem' incorporates:
             *  ActionPort: '<S14>/Action Port'
             */
            dw->heater_status = ON;

            /* End of Outputs for SubSystem: '<S10>/If Action Subsystem' */
        }
        else
        {
            /* Outputs for IfAction SubSystem: '<S10>/If Action Subsystem1' incorporates:
             *  ActionPort: '<S15>/Action Port'
             */
            dw->heater_status = OFF;

            /* End of Outputs for SubSystem: '<S10>/If Action Subsystem1' */
        }

        /* End of If: '<S10>/If2' */
        /* End of Outputs for SubSystem: '<S5>/read_load_status_if' */
    }

    /* End of If: '<S5>/If' */

    /* Switch: '<S6>/Switch' incorporates:
     *  Constant: '<S6>/Zero1'
     *  DataTypeConversion: '<S6>/Data Type Conversion1'
     */
    if (Heater_Circuit_OK)
    {
        /* S-Function (sfun_HblGetGIDataConvertedByIndex): '<S6>/GetGIDataConvertedByIndex' */
        GetGIDataConvertedByIndex_n = Hbl__GetGIDataConvertedByIndex
            (GetGIIndexByGIID);
        dw->measured_heater_current = (uint16)GetGIDataConvertedByIndex_n;
    }
    else
    {
        dw->measured_heater_current = 0U;
    }

    /* End of Switch: '<S6>/Switch' */

    /* CCaller: '<S8>/C Caller' */
    Vwi_Status = Vwi__GetWaterStatus();

    /* If: '<S8>/If' incorporates:
     *  Constant: '<S8>/Constant'
     *  Inport: '<S19>/Inport'
     *  Inport: '<S20>/In1'
     */
    if (Heater_Circuit_OK)
    {
        /* Outputs for IfAction SubSystem: '<S8>/If Action Subsystem' incorporates:
         *  ActionPort: '<S19>/Action Port'
         */
        dw->vwi_status = Vwi_Status;

        /* End of Outputs for SubSystem: '<S8>/If Action Subsystem' */
    }
    else
    {
        /* Outputs for IfAction SubSystem: '<S8>/If Action Subsystem1' incorporates:
         *  ActionPort: '<S20>/Action Port'
         */
        dw->vwi_status = WS_NOT_VALID;

        /* End of Outputs for SubSystem: '<S8>/If Action Subsystem1' */
    }

    /* End of If: '<S8>/If' */

    /* S-Function (sfun_HblGetGIIndexByGIID): '<S7>/GetGIIndexByGIID' incorporates:
     *  Constant: '<S18>/Constant'
     */
    GetGIIndexByGIID_b = Hbl__GetGIIndexByGIID(HBL_GI_WASHING_PROBE);

    /* S-Function (sfun_HblGetGIDataConvertedByIndex): '<S7>/GetGIDataConvertedByIndex' */
    GetGIDataConvertedByIndex = Hbl__GetGIDataConvertedByIndex
        (GetGIIndexByGIID_b);

    /* DataTypeConversion: '<S7>/Data Type Conversion' */
    dw->water_temperature = (uint16)GetGIDataConvertedByIndex;

    /* CCaller: '<S1>/C Caller' */
    Door_Status = DoorMonitor__GetDoorStatus();

    /* Logic: '<Root>/AND' incorporates:
     *  Constant: '<S4>/Constant'
     *  Inport: '<Root>/HCD_Enable'
     *  RelationalOperator: '<S1>/Equal'
     */
    HCD_Enable = (u_hcd_enable && (Door_Status == DOOR_STATUS_CLOSE));

    /* ModelReference: '<Root>/HCD_MasterModel' incorporates:
     *  Inport: '<Root>/HCD_Cycle_Started'
     *  Inport: '<Root>/Heater_Current_Threshold'
     *  Inport: '<Root>/SF_HCD_Parameters'
     *  Outport: '<Root>/HCD_Lowest_Click_Current'
     *  Outport: '<Root>/HCD_Lowest_Click_Power'
     *  Outport: '<Root>/HCD_Lowest_Click_Temperature'
     *  Outport: '<Root>/HCD_Monitoring_States'
     *  Outport: '<Root>/HCD_Reduction_Current'
     *  Outport: '<Root>/HCD_Reduction_Power'
     *  Outport: '<Root>/HCD_Reduction_Temperature'
     */
    HCD_Monitor(HCD_Enable, u_heater_current_threshold,
                dw->measured_heater_current, dw->heater_status,
                dw->water_temperature, dw->vwi_status, u_sf_hcd_parameters,
                u_hcd_cycle_started, y_hcd_lowest_click_temperature,
                y_hcd_lowest_click_current, y_hcd_lowest_click_power,
                y_hcd_reduction_temperature, y_hcd_reduction_current,
                y_hcd_reduction_power, (&(HCD_Heater_Power)),
                (&(HCD_Unstable_Clicks)), (&(HCD_Heater_Energy)),
                (&(HCD_Water_Stable)), y_hcd_monitoring_states,
                (&(HCD_Avg_Heater_Current)),
                &(dw->hcd_mastermodel_instancedata.rtdw));
}

/** Model initialize function */
void HeaterCalcificationDetectionMaster__Initialize
    (RT_MODELHEATERCALCIFICATIO_TYPE *const m)
{
    DWHEATERCALCIFICATIONDETEC_TYPE *dw = ((DWHEATERCALCIFICATIONDETEC_TYPE *)
        m->dwork);

    /* SystemInitialize for IfAction SubSystem: '<S5>/read_load_status_if' */
    /* SystemInitialize for Merge: '<S10>/Merge' */
    dw->heater_status = OFF;

    /* End of SystemInitialize for SubSystem: '<S5>/read_load_status_if' */
}
