/**
 *  @file
 *  @brief
 *
 *
 *  @copyright  Copyright 2020 - Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef MCLPARAMS_PRM_H_
#define MCLPARAMS_PRM_H_

#include "C_Extensions.h"

// startup parameters
//! Adc to voltage gain
#define ADC_TO_VOLT_FACTOR                          (float)(((float)3.3)/((float)4096*(float)0.007014))
//! Adc to current gain
#define ADC_TO_CURR_FACTOR                          (float)(0.0036966824644549763)
//! Undercurrent thr
#define UNDER_CURRENT_THR                           50
//! Overcurrent thr
#define OVER_CURRENT_THR                            700
//! Current failure debounce time in ms
#define CURRENT_FAILURE_DEBOUNCE_TIME               2000
//! Alpha for offset iir filter
#define OFFSET_ALPHA_IIR                            15
//! Alpha for bemf shift iir filter
#define BEMF_SHIFT_ALPHA_IIR                        2
//! Alpha for semi period iir filter
#define SEMI_PERIOD_ALPHA_IIR                       3
//! Alpha for bemf iir filter
#define BEMF_ALPHA_IIR                              7
//! Alpha for bemf iir filter
#define BEMF_2_ALPHA_IIR                            2
//! Bemf shift initial value
#define BEMF_SHIFT_INIT_VALUE                       30
//! P2p detection period in sec
#define P2P_DETECTION_PERIOD                        (0.022)
//! Presaler for active power
#define PRESCALER_FOR_ACTIVE_POWER                  (1024)
//! Variance thr for power drop detection
#define POWER_DROP_VARIANCE_THR                     (6)
//! Time between two initial measurements in ms
#define WAIT_BETWEEN_MEAS_TIME                      300
//! Correct trigger debounce to switch to close loop
#define CORRECT_TIRGGER_DEBOUNCE                    7
//! Meas triac trigger angle in % of semi cycle
#define COMMON_MEAS_PULSE_ANGLE                     55
//! First compensation min limit
#define COMMON_FIRST_PULSE_MIN_LIMIT                7
//! Zero current detection thr for current
#define ZC_CURENT_DETECTION_THR                     150
//! Zero current detection thr for voltage
#define ZC_VOLTAGE_DETECTION_THR                    250
//! Max current limit at which observer is operational
#define OBSERVER_OPERATION_MAX_LIMIT                900
//! Flat bemf detection thr
#define FLAT_BEMF_DETECTION_THR                     60
//! Motor stop detection thr
#define MOTOR_STOP_DETECTION_THR                    40
//! Bemf shift PI A parameter
#define BEMF_SHIFT_PI_A_PARAM                       (30000)
//! Bemf shift PI B parameter
#define BEMF_SHIFT_PI_B_PARAM                       (-20000)
//! Bemf shift PI max output value
#define BEMF_SHIFT_PI_MAX_OUT                       60
//! Voltage symmetry threshold for PI reference updated
#define BEMF_SHIFT_PI_REF_UPDATE_THR                150
//! Triac trigger angle limit in % of semi cycle
#define TRIAC_TRIGGER_ANGLE_LIMIT                   80
//! Motor stop detection time in ms
#define MOTOR_STOP_DETECTION_TIME                   (3.5)
//! Rotor position debounce thr
#define ROTOR_DEBOUNCE_THR                          10
//! Open loop timeout in ms
#define OPEN_LOOP_TIMEOUT                           500



//  ---- Motor parameters ---------------------------------------------------------------------------------------------
// Only parameters that are different for different motors are included in prv.
// All other parameters should be defined in .c file. And they should not be changed.

//  ----- Askoll PW ---------------------------------------------------------------------------------------------------
// Askoll motor 1 parameter before testes
#define ASKOLL_WINDING_RESITANCE_M1_BEFORE          (float)(7.56)   // Ohms
#define ASKOLL_WINDING_INDUCTANCE_M1_BEFORE         (float)(0.103)  // Henry

// Askoll motor 1 parameter after testes
#define ASKOLL_WINDING_RESITANCE_M1_AFTER           (float)(7.49)   // Ohms
#define ASKOLL_WINDING_INDUCTANCE_M1_AFTER          (float)(0.103)  // Henry

// Askoll motor 2 parameter before testes
#define ASKOLL_WINDING_RESITANCE_M2_BEFORE          (float)(7.5)    // Ohms
#define ASKOLL_WINDING_INDUCTANCE_M2_BEFORE         (float)(0.098)  // Henry

#define ASKOLL_PW_WINDING_RESITANCE                 (float)(7.5)    // Averaged from measured motors
#define ASKOLL_PW_WINDING_INDUCTANCE                (float)(0.135)  // Calibrated at direct bemf to calculated bemf switching point

//! Meas triac trigger angle in % of semi cycle
#define ASKOLL_PW_MEAS_PULSE_ANGLE                  50
//! First compensation A coefficient. Compensation equation is: y=x/A-B
#define ASKOLL_PW_FIRST_PULSE_COMPENSATION_A        13
//! First compensation B coefficient. Compensation equation is: y=x/A-B
#define ASKOLL_PW_FIRST_PULSE_COMPENSATION_B        27
//! First compensation max limit
#define ASKOLL_PW_FIRST_PULSE_MAX_LIMIT             50
//! Open loop angle limit add. it is calculate base on first pulse angle
#define ASKOLL_PW_OPEN_LOOP_ANGLE_LIMIT_ADD         15
//! Open loop angle limit max
#define ASKOLL_PW_OPEN_LOOP_ANGLE_MAX_LIMIT         46
//! Bemf shift reference
#define ASKOLL_PW_BEMF_SHIFT_REFERENCE              37

//  ----- Askoll AW ---------------------------------------------------------------------------------------------------
// Askoll AW motor 1 parameter before testes
#define ASKOLL_AW_WINDING_RESITANCE_M1_BEFORE       (float)(8.5)    // Ohms
#define ASKOLL_AW_WINDING_INDUCTANCE_M1_BEFORE      (float)(0.116)  // Henry

// Askoll AW motor 2 parameter before testes
#define ASKOLL_AW_WINDING_RESITANCE_M2_BEFORE       (float)(8.6)    // Ohms
#define ASKOLL_AW_WINDING_INDUCTANCE_M2_BEFORE      (float)(0.112)  // Henry

#define ASKOLL_AW_WINDING_RESITANCE                 (float)(8.5)    // Averaged from measured motors
#define ASKOLL_AW_WINDING_INDUCTANCE                (float)(0.145)  // Calibrated at direct bemf to calculated bemf switching point

//! Second meas triac trigger angle in % of semi cycle
#define ASKOLL_AW_SECOND_MEAS_PULSE_ANGLE           50
//! Compensation change thr
#define ASKOLL_AW_COMPENSATION_CHANGE_THR           1200
//! First compensation A coefficient. Compensation equation is: y=A-B
#define ASKOLL_AW_FIRST_PULSE_COMPENSATION_LOW_A    (0.076628)
//! First compensation B coefficient. Compensation equation is: y=A-B
#define ASKOLL_AW_FIRST_PULSE_COMPENSATION_LOW_B    (46.647510)
//! First compensation A coefficient. Compensation equation is: y=A-B
#define ASKOLL_AW_FIRST_PULSE_COMPENSATION_HIGH_A   (0.044444)
//! First compensation B coefficient. Compensation equation is: y=A-B
#define ASKOLL_AW_FIRST_PULSE_COMPENSATION_HIGH_B   (8.155556)
//! First compensation max limit
#define ASKOLL_AW_FIRST_PULSE_MAX_LIMIT             56
//! Open loop angle limit add. it is calculate base on first pulse angle
#define ASKOLL_AW_OPEN_LOOP_ANGLE_LIMIT_ADD         10
//! Open loop angle limit max
#define ASKOLL_AW_OPEN_LOOP_ANGLE_MAX_LIMIT         46
//! Bemf shift reference
#define ASKOLL_AW_BEMF_SHIFT_REFERENCE              34

//From Settingfile
#ifdef COMPILE_4_SIMULINK
#pragma pack(1)
typedef struct
#else
typedef PACKED struct
#endif
{
    uint8 Motor_Wash_Type;
    uint8 Current_Voltage_Sensor_Mode;
    float32 Winding_Resistance;
    float32 Winding_Inductance;
    uint8 Meas_Pulse_Angle;
    uint16 First_Pulse_Compensation_A;
    uint16 First_Pulse_Compensation_B;
    uint8 First_Pulse_Max_Limit;
    uint8 Open_Loop_Angle_Limit_Add;
    uint8 Open_Loop_Max_Limit;
    uint8 Bemf_Shift_Reference;
    uint16 Power_Drop_Thr;
    uint16 Volt_At_Cycle_End_Thr;
    uint16 Voltage_Base;
    uint8 Current_Base;
    uint16 Power_Base;
    uint32 Current_Sensor_Gain;
    uint32 Voltage_Sensor_Gain;
    uint32 Shirt_Limiter_Coff_A;
    uint32 Shirt_Limiter_Coff_B;
    uint32 Mcisflac_Fast_Handler_Frequency;
    uint32 Mcisflac_Timer_Frequency;
    uint16 Under_Curr_Thr;
    uint16 Over_Curr_Thr;
    uint16 Current_Failure_Debounce_Time;
    uint16 Offset_Alpha_IIR;
    uint16 Bemf_Shift_Alpha_IIR;
    uint16 Semi_Period_Alpha_IIR;
    uint16 Bemf_Alpha_IIR;
    uint16 Bemf_Alpha_2_IIR;
    uint16 Bemf_Shift_Init_Value;
    float32 P2P_Detection_Period;
    uint16 Prescaler_for_Active_Power;
    uint16 Wait_Between_Meas_Time;
    uint16 Correct_Trigger_Debounce;
    uint16 Commom_Meas_Pulse_Angle;
    uint16 ZC_Current_Detection_Thr;
    uint16 ZC_Voltage_Detection_Thr;
    uint16 Observer_Operation_Max_Limit;
    uint16 Flat_Bemf_Detection_Thr;
    uint16 Motor_Stop_Detection_Thr;
    sint32 Bemf_Shift_PI_A_Param;
    sint32 Bemf_Shift_PI_B_Param;
    uint16 Bemf_Shift_PI_Max_Out;
    uint16 Bemf_Shift_PI_Ref_Update_Thr;
    uint16 Triac_Trigger_Angle_Limit;
    float32 Motor_Stop_Detection_Time;
    uint16 Rotor_Debounce_Thr;
    uint16 Open_Loop_Timeout;
    uint8 Mci_Params_ID;
}
MCL_PARAMS_TYPE;

#endif  // MCLPARAMS_PRM_H_
