/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCISLFACNEW_H_
#define MCISLFACNEW_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "C_Extensions.h"
#include "Mci.h"
#include "MclTypes.h"
#include "TimeCapture.h"
#include "MclObserver.h"
#include "MclFireAngleCtrl.h"

#pragma pack(4)
typedef struct
{
    uint32 Mcl_Fault;
    BOOL_TYPE Zero_Cross_Event;
    APL_WASH_MOTOR_TYPE MciSlfac_Wash_Motor_Type;    //!< Wash motor type
    MCI_ERROR_TYPE MciSlfac_Last_Failure;     //!< Last failure
    BOOL_TYPE MciSlfac_Prev_VZC_State;       //!< Last voltage zero crossing state
    ROTOR_STATE_TYPE Rotor_State;            //!< Rotor state calculated based on observer signal
    ROTOR_STATE_TYPE Direct_BEMF_State;      //!< Direct BEMF state
    uint8 Power_Drop_Exit_Deb;               //!< Power drop exit debounce
    uint8 Power_Drop_Enter_Deb;              //!< Power drop enter debounce
    BOOL_TYPE Triac_Fired;                   //!< TRUE when triac was fired at last semi cycle
    BOOL_TYPE Triac_Fired_Prev;              //!< TRUE when triac was fired at before last semi cycle
    BOOL_TYPE Zero_Current_Detected;         //!< TRUE when zero current detected
    BOOL_TYPE Triac_Trigger_VZC_State;       //!< VZC at which triac should be triggered
    BOOL_TYPE Triac_Trigger_Event;           //!< TRUE when triac trigger interrupt was triggered
    BOOL_TYPE Check_Bemf;                    //!< Directly check bemf to tirgger triac
    BOOL_TYPE Bemf_Aligned;                  //!< Bemf aligned with line voltage
    BOOL_TYPE Prev_Volt_Max_Ready;           //!< Prev_Volt_Max is ready
    BOOL_TYPE Triac_State;                   //!< Triac state
    uint16 Semi_Period_Time_Raw;             //!< Line voltage semi period time raw value
    sint16 Triac_Start_Time;                 //!< Triac trigger time in open loop. PI format
    sint16 Triac_Start_Time_Init;            //!< Init for triac trigger time in open loop. PI format
    uint16 VZC_Time_P;                       //!< Last positive VZC time
    uint16 VZC_Time_N;                       //!< Last negative VZC time
    uint16 State_Timeout;                    //!< Generic state timeout event
    uint16 Stop_Debounce_Event;              //!< Stop debounce event
    uint16 Offset_Curr;                      //!< Current offset
    uint16 Offset_Volt;                      //!< Voltage offset
    uint16 Prev_Volt;                        //!< Previous raw voltage value
    sint16 Filt_Volt_Increase;               //!< Filt voltage increase
    sint16 Volt_At_Cycle_End_Inc;            //!< Voltage at cycle end increase
    uint16 Filt_Bemf_Volt;                   //!< Filtered BEMF voltage
    uint16 MciSlfac_Active_Power;            //!< Active power after prescaling
    uint16 MciSlfac_Active_Power_Prev;       //!< Previous active power after prescaling
    uint16 Correct_Trigger_Debounce;         //!< Correct triac trigger debounce
    uint16 Volt_Max;                         //!< Max volt
    uint16 Prev_Volt_Max;                    //!< Prev max volt
    uint16 Filt_Volt_Sym;                    //!< Filtered voltage symmetry
    uint32 IIR_Prev_Volt_Symmetry;           //!< Previous value for voltage symmetry iir filter
    PI_TYPE Pi_Bemf_Shift;                   //!< PI regulator for shift control
} MCL_QUANTITIES_TYPE;


#pragma pack(4)
typedef struct
{
    BOOL_TYPE zero_current_prev;
    uint16 raw_volt;
    uint16 filt_volt;
    sint16 sig_raw_volt;
    sint16 sig_filt_volt;
    uint16 raw_curr;
    sint16 sig_raw_curr;
    uint16 abs_raw_curr;
    sint16 dif_raw_curr;
    sint16 bemf_volt_tmp;

}MCL_INTERNAL_CL_TYPE;

typedef enum MCL_DEBUG_MODE_ENUM
{
    DEBUG_MODE_MCI = 0,
    DEBUG_MODE_ALWAYS_ON,
    DEBUG_MODE_OPEN_LOOP,
    DEBUG_MODE_DISABLED,
} MCL_DEBUG_MODE_TYPE;


extern MCL_CS_TYPE Mcl_Cs;
extern MCL_QUANTITIES_TYPE Mcl_Quantities;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Mcl__Initialize(void);
void Mcl__RunningHandler(void);
void Mcl__SlowHandler(void);
void Mcl__ZeroCrossHandler(TC_EVENT_TYPE event, uint16 ccr_value);
void Mcl__TriacFireHandler(TC_EVENT_TYPE event);
MCI_CMD_RESPONSE_TYPE Mcl__Run(MOTOR_ENUM_TYPE motor, sint16 speed_rpm, sint16 ramp_rpm_per_s);
MCI_CMD_RESPONSE_TYPE Mcl__Stop(MOTOR_ENUM_TYPE motor, sint16 ramp_rpm_per_s);
MCI_CMD_RESPONSE_TYPE Mcl__ManualControl(MCL_DEBUG_MODE_TYPE mode, sint32 param0, sint32 param1);
void Mcl__SetWashMotorType(APL_WASH_MOTOR_TYPE wash_type);

void Mcl__GoToCloseLoop(void);

BOOL_TYPE Mcl__GetError(MOTOR_ENUM_TYPE motor);
uint32 Mcl__GetErrorList(void);
BOOL_TYPE Mcl__GetDigital(MCI_DI_TYPE read_bit);
sint32 Mcl__GetAnalog(MCI_AI_TYPE read_data);
uint8 Mcl__ClearError(void);

BOOL_TYPE Mcl__SetParams(MCL_PARAMS_TYPE* params);
uint16 Mcl__GetParamsID(void);

#if (SIL_MODE == ENABLED)
//! Analog inputs to application
typedef enum
{
    MCL_GD_FAILURE_STATE,                           //!< 0 - Failure State
    MCL_GD_BACK_EMF,                                //!< 1 - Motor Back EMF
    MCL_GD_MAIN_COUNTER,                            //!< 2 - MCL main counter
    MCL_GD_MOTOR_TYPE,                              //!< 3 - Selected Motor Type

    MCL_GD_NR_OF_GD                                 //!< Total declared of Get Data
} MCL_GET_DATA_SIL_TYPE;

sint32 Mcl__GetDataSIL(MCL_GET_DATA_SIL_TYPE data);
#endif

#endif // MCISLFACNEW_H_
