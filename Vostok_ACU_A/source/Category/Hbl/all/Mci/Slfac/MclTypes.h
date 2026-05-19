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
#ifndef MCLTYPES_H_
#define MCLTYPES_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "MclBasicTypes.h"
#include "TimeCapture.h"
#include "Pi.h"


typedef enum ROTOR_STATE_ENUM
{
    ROTOR_NEGATIVE = 0,
    ROTOR_POSITIVE = 1,
    ROTOR_UNKNOWN  = 2
} ROTOR_STATE_TYPE;

typedef enum CONTROL_MODE_ENUM
{
    MCISLFAC_IDLE = 0,                          //!< When motor is inactive.
    MCISLFAC_STARTUP,                           //!< First state after start command. Initialization of all motor parameters.
    MCISLFAC_STARTUP_PULSE_MEAS,                //!< First pulse to measure rotor position.
    MCISLFAC_STARTUP_INIT_PULSE_MEAS_OPPOSITE,  //!< Initialization of second pulse to measure rotor position.
    MCISLFAC_STARTUP_PULSE_MEAS_OPPOSITE,       //!< Second pulse to measure rotor position. First pulse to measure winding current and rotate rotor.
    MCISLFAC_STARTUP_PULSE_ROTATE,              //!< Second pulse to measure winding current and rotate rotor.
    MCISLFAC_STARTUP_PULSE_ROTATE_AUX,          //!< Third pulse to rotate rotor.
    MCISLFAC_STARTUP_PULSE_ROTATE_AUX_2,        //!< Fourth pulse to rotate rotor.
    MCISLFAC_STARTUP_WAIT,                      //!< Wait time between MCISLFAC_STARTUP_PULSE_MEAS and MCISLFAC_STARTUP_INIT_PULSE_MEAS_OPPOSITE
    MCISLFAC_OPEN_LOOP,                         //!< Open loop control - triac trigger angle is not controlled
    MCISLFAC_CLOSE_LOOP,                        //!< Close loop control - triac trigger angle is optimize for lowest power consumption.
    MCISLFAC_STOPPING,                          //!< Start entered after stop command
    MCISLFAC_STOP_CHECK                         //!< Check if motor was stopped
} CONTROL_MODE_ENUM_TYPE;

typedef enum
{
    FAILURE_NOT_DETECTED = 0,
    FAILURE_RECOVERY = 1,
    FAILURE_DETECTED = 2
} FAILURE_STATE_TYPE;

typedef struct
{
    CONTROL_MODE_ENUM_TYPE Control_Mode;
    FAILURE_STATE_TYPE Failure_State;

    uint16 Mcl_Main_Counter;
} MCL_CS_TYPE;


//! Motor control macro block - INPUT PROCESSING type definition - floating point
typedef struct
{
    // MclInputProcessing Output
    uint16 *Offset_Volt;
    uint16 *Offset_Curr;
    BOOL_TYPE *Zero_Current_Detected;

    //Internal
    BOOL_TYPE *zero_current_prev;
    uint16 *raw_volt;
    uint16 *raw_curr;
    sint16 *sig_raw_volt;
    sint16 *sig_raw_curr;
    uint16 *abs_raw_curr;
    sint16 *dif_raw_curr;

    // Control specific
    MCL_CS_TYPE *ctrl_specific;                     //!< MCL control specific variables
    uint32 *Mcl_Fault;
} MCL_INPUT_PROC_IO_TYPE;

//! Motor control macro block - OBSERVER type definition
typedef struct
{
    uint16 *VZC_Time_P;                       //!< Last positive VZC time
    BOOL_TYPE *Triac_Fired;                   //!< TRUE when triac was fired at last semi cycle
    BOOL_TYPE *Triac_Fired_Prev;              //!< TRUE when triac was fired at before last semi cycle
    uint16 *MciSlfac_Active_Power;            //!< Active power after prescaling
    PI_TYPE *Pi_Bemf_Shift;                   //!< PI regulator for shift control
    uint8 *Power_Drop_Exit_Deb;               //!< Power drop exit debounce
    uint8 *Power_Drop_Enter_Deb;              //!< Power drop enter debounce
    sint16 *Triac_Start_Time;                 //!< Triac trigger time in open loop. PI format
    sint16 *Triac_Start_Time_Init;            //!< Init for triac trigger time in open loop. PI format
    BOOL_TYPE *MciSlfac_Prev_VZC_State;       //!< Last voltage zero crossing state
    uint16 *Prev_Volt;                        //!< Previous raw voltage value
    BOOL_TYPE *Triac_State;                   //!< Triac state
	APL_WASH_MOTOR_TYPE *MciSlfac_Wash_Motor_Type;    //!< Wash motor type
    uint16 *Offset_Curr;                      //!< Current offset
    BOOL_TYPE *Bemf_Aligned;                  //!< Bemf aligned with line voltage
    uint16 *Filt_Bemf_Volt;                   //!< Filtered BEMF voltage
    ROTOR_STATE_TYPE *Direct_BEMF_State;      //!< Direct BEMF state
    BOOL_TYPE *Zero_Current_Detected;         //!< TRUE when zero current detected
    uint16 *MciSlfac_Active_Power_Prev;       //!< Previous active power after prescaling
    sint16 *Filt_Volt_Increase;               //!< Filt voltage increase
    uint16 *Offset_Volt;                      //!< Voltage offset

    uint16 *Volt_Max;                         //!< Max volt
    uint16 *Prev_Volt_Max;                    //!< Prev max volt
    uint16 *Filt_Volt_Sym;                    //!< Filtered voltage symmetry
    uint32 *IIR_Prev_Volt_Symmetry;           //!< Previous value for voltage symmetry iir filter

    //Internal
    uint16 *raw_volt;
    uint16 *filt_volt;
    sint16 *sig_raw_volt;
    sint16 *sig_raw_curr;
    uint16 *abs_raw_curr;
    sint16 *dif_raw_curr;
    sint16 *bemf_volt_tmp;
    BOOL_TYPE *zero_current_prev;

    // Control specific
    MCL_CS_TYPE *ctrl_specific;                     //!< MCL control specific variables
	uint16 *Mcl_Main_Counter;
} MCL_OBSERVER_IO_TYPE;


//! Motor control macro block - DQ CONTROLLER type definition
typedef struct
{
    // MclFireAngleCtrl Input
    BOOL_TYPE *Zero_Cross_Event;
	APL_WASH_MOTOR_TYPE *MciSlfac_Wash_Motor_Type;    //!< Wash motor type
    BOOL_TYPE *MciSlfac_Prev_VZC_State;       //!< Last voltage zero crossing state
    BOOL_TYPE *Triac_Fired;                   //!< TRUE when triac was fired at last semi cycle
    BOOL_TYPE *Triac_Fired_Prev;              //!< TRUE when triac was fired at before last semi cycle
    BOOL_TYPE *Triac_Trigger_VZC_State;       //!< VZC at which triac should be triggered
    uint16 *Offset_Curr;                      //!< Current offset
    uint16 *Offset_Volt;                      //!< Voltage offset
    sint16 *Triac_Start_Time;                 //!< Triac trigger time in open loop. PI format
    sint16 *Triac_Start_Time_Init;            //!< Init for triac trigger time in open loop. PI format
    BOOL_TYPE *Check_Bemf;                    //!< Directly check bemf to tirgger triac
    BOOL_TYPE *Bemf_Aligned;                  //!< Bemf aligned with line voltage
    uint16 *Correct_Trigger_Debounce;         //!< Correct triac trigger debounce
    PI_TYPE *Pi_Bemf_Shift;                   //!< PI regulator for shift control
    uint16 *MciSlfac_Active_Power;            //!< Active power after prescaling
    uint16 *MciSlfac_Active_Power_Prev;       //!< Previous active power after prescaling
    uint16 *Filt_Volt_Sym;                    //!< Filtered voltage symmetry
    sint16 *Volt_At_Cycle_End_Inc;            //!< Voltage at cycle end increase
    uint8 *Power_Drop_Exit_Deb;               //!< Power drop exit debounce
    uint8 *Power_Drop_Enter_Deb;              //!< Power drop enter debounce
    uint16 *State_Timeout;                    //!< Generic state timeout event
    uint32 *IIR_Prev_Volt_Symmetry;           //!< Previous value for voltage symmetry iir filter
    BOOL_TYPE *Prev_Volt_Max_Ready;           //!< Prev_Volt_Max is ready
    uint16 *Volt_Max;                         //!< Max volt
    uint16 *Prev_Volt;                        //!< Previous raw voltage value
    BOOL_TYPE *Zero_Current_Detected;         //!< TRUE when zero current detected
    uint16 *Prev_Volt_Max;                    //!< Prev max volt
    uint16 *Filt_Bemf_Volt;                   //!< Filtered BEMF voltage
    ROTOR_STATE_TYPE *Rotor_State;            //!< Rotor state calculated based on observer signal
    uint16 *VZC_Time_P;                       //!< Last positive VZC time
    uint16 *VZC_Time_N;                       //!< Last negative VZC time
    uint16 *Stop_Debounce_Event;              //!< Stop debounce event


    //Local Variables
    BOOL_TYPE *zero_current_prev;
    uint16 *raw_volt;
    uint16 *filt_volt;
    uint16 *raw_curr;
    sint16 *bemf_volt_tmp;


    // Control specific
    MCL_CS_TYPE *ctrl_specific;                       //!< MCL control specific variables
    MCI_ERROR_TYPE *MciSlfac_Last_Failure;            //!< Last failure
    uint32 *Mcl_Fault;
    FAILURE_STATE_TYPE *Failure_State;
	uint16 *Mcl_Main_Counter;
} MCL_FIREANGLE_CTRL_IO_TYPE;



//! Motor control macro block - TRIAC DRIVE type definition
typedef struct
{
    BOOL_TYPE *Triac_Trigger_Event;           //!< TRUE when triac trigger interrupt was triggered
    BOOL_TYPE *Triac_Trigger_VZC_State;       //!< VZC at which triac should be triggered
    BOOL_TYPE *MciSlfac_Prev_VZC_State;       //!< Last voltage zero crossing state
    BOOL_TYPE *Check_Bemf;                    //!< Directly check bemf to tirgger triac
    ROTOR_STATE_TYPE *Direct_BEMF_State;      //!< Direct BEMF state
	APL_WASH_MOTOR_TYPE *MciSlfac_Wash_Motor_Type;    //!< Wash motor type
    ROTOR_STATE_TYPE *Rotor_State;            //!< Rotor state calculated based on observer signal
    sint16 *Filt_Volt_Increase;               //!< Filt voltage increase
    BOOL_TYPE *Zero_Current_Detected;         //!< TRUE when zero current detected
    BOOL_TYPE *Bemf_Aligned;                  //!< Bemf aligned with line voltage
    BOOL_TYPE *Triac_Fired_Prev;              //!< TRUE when triac was fired at before last semi cycle
    BOOL_TYPE *Triac_Fired;                   //!< TRUE when triac was fired at last semi cycle
    uint16 *Volt_Max;                         //!< Max volt
    BOOL_TYPE *Prev_Volt_Max_Ready;           //!< Prev_Volt_Max is ready
    uint16 *Prev_Volt_Max;                    //!< Prev max volt
    uint16 *Filt_Volt_Sym;                    //!< Filtered voltage symmetry
    uint32 *IIR_Prev_Volt_Symmetry;           //!< Previous value for voltage symmetry iir filter
    uint16 *Prev_Volt;                        //!< Previous raw voltage value
    uint16 *Offset_Volt;                      //!< Voltage offset
    BOOL_TYPE *Triac_State;                   //!< Triac state
    sint16 *Volt_At_Cycle_End_Inc;            //!< Voltage at cycle end increase
    uint8 *Power_Drop_Enter_Deb;              //!< Power drop enter debounce
    uint16 *Correct_Trigger_Debounce;         //!< Correct triac trigger debounce


    // Control specific
    MCL_CS_TYPE *ctrl_specific;                       //!< MCL control specific variables
    uint32 *Mcl_Fault;
} MCL_TRIAC_DRIVE_IO_TYPE;


//! Motor control macro block - ZERO CROSS type definition
typedef struct
{
    // MclZeroCross Output
    BOOL_TYPE *Zero_Cross_Event;
    BOOL_TYPE *MciSlfac_Prev_VZC_State;       //!< Last voltage zero crossing state
    uint16 *Semi_Period_Time_Raw;             //!< Line voltage semi period time raw value

} MCL_ZERO_CROSS_IO_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // MCLTYPES_H_


