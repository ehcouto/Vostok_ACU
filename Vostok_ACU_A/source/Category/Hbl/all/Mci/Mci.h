/**
 *  @file       Mci.h
 *  @brief      Motor control API
 *  @details    Define the basic commands to interface the application with the motor control
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MCI_H_
    #define MCI_H_

#include "Mci_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Motor indexer
typedef enum
{
    MOTOR0,                                         //!< 0 - Motor index 0
    MOTOR1,                                         //!< 1 - Motor index 1
    MOTOR2,                                         //!< 2 - Motor index 2
    MOTOR3,                                         //!< 3 - Motor index 3
    MOTOR4                                          //!< 4 - Motor index 4
} MOTOR_ENUM_TYPE;

//! Digital inputs definition to application
typedef enum
{
    MCI_DI_OVER_TORQUE = 0,                         //!<  0 - [T/F] Over torque detected
    MCI_DI_MOTOR_STOP,                              //!<  1 - [T/F] Motor stop
    MCI_DI_REACHED_TARGET_SPEED,                    //!<  2 - [T/F] Target speed == reference speed
    MCI_DI_IDLE,                                    //!<  3 - [T/F] Motor idle, if the speed is zero, no error found.

    MCI_DI_MOTOR_RUNNING,                           //!<  4 - [T/F] Motor running
    MCI_DI_RESIST_ESTIMATED,                        //!<  5 - [T/F] Read resistance was estimated
    MCI_DI_PARAMETERS_LOADED,                       //!<  6 - [T/F] Class A and B - SR_FLAG_MC_PARAMETERS_LOADED

    MCI_DI_NR_OF_DI                                 //!< Total declared of digital inputs
} MCI_DI_TYPE;



//! Analog inputs to application
typedef enum
{
    MCI_AI_TARGET_SPEED_RPM_S16 = 0,                //!<  0 - Target rotor shaft speed in RPM - x65536 - (1<<16)
    MCI_AI_SPEED_REFERENCE_RPM_S16,                 //!<  1 - Speed reference in RPM - x65536 - (1<<16)
    MCI_AI_ACTUAL_SPEED_S16,                        //!<  2 - Estimated rotor shaft speed in RPM - x65536 - (1<<16)
    MCI_AI_SPEED_ERROR_RPM_S16,                     //!<  3 - Difference between estimated and reference speeds in RPM - x65536 - (1<<16)

    MCI_AI_MEAN_SPEED_S16,                          //!<  4 - Mean rotor speed calculation - x65536 - (1<<16)
    MCI_AI_RMS_MOTOR_CURRENT_S16,                   //!<  5 - RMS Motor current - x65536 - (1<<16)
    MCI_AI_ACTIVE_POWER_S16,                        //!<  6 - Measured active power in Watts - x65536 - (1<<16)
    MCI_AI_SHAFT_POWER_S16,                         //!<  7 - Estimated shaft output power in Watts from estimated torque - x65536 - (1<<16)

    MCI_AI_ROTOR_TEMP_S16,                          //!<  8 - Rotor temperature in Celsius - x65536 - (1<<16)
    MCI_AI_STATOR_TEMP_S16,                         //!<  9 - Stator temperature in Celsius - x65536 - (1<<16)
    MCI_AI_LOAD_TORQUE_S16,                         //!< 10 - Read the load torque in Nm - x65536 - (1<<16)
    MCI_AI_SHAFT_POSITION_DEG_S16,                  //!< 11 - Estimated rotor position in deg - x65536 - (1<<16)

    MCI_AI_BUS_CURRENT_S16,                         //!< 12 - Bus current in Amps - x65536 - (1<<16)
    MCI_AI_BUS_VOLTAGE_S16,                         //!< 13 - Bus voltage in Volts - x65536 - (1<<16)
    MCI_AI_INVERTER_TEMP_S16,                       //!< 14 - Inverter temperature in Celsius - x65536 - (1<<16)
    MCI_AI_INVERTER_TEMP_RAW_DATA,                  //!< 15 - Inverter temperature in raw data [ADC counts]

    MCI_AI_VOLTAGE_PHASE_A_S16,                     //!< 16 - Motor phase A voltage in Volts - x65536 - (1<<16)
    MCI_AI_VOLTAGE_PHASE_B_S16,                     //!< 17 - Motor phase B voltage in Volts - x65536 - (1<<16)
    MCI_AI_VOLTAGE_PHASE_C_S16,                     //!< 18 - Motor phase C voltage in Volts - x65536 - (1<<16)
    MCI_AI_SPEED_LOOP_GAIN_TABLE_INDEX,             //!< 19 - Select index of speed loop gain table

    MCI_AI_CURR_PHASE_A_S16,                        //!< 20 - Phase current A in A - x65536 - (1<<16)
    MCI_AI_CURR_PHASE_B_S16,                        //!< 21 - Phase current B in A - x65536 - (1<<16)
    MCI_AI_CURR_PHASE_C_S16,                        //!< 22 - Phase current C in A - x65536 - (1<<16)
    MCI_AI_STATOR_RESISTANCE_S16,                   //!< 23 - Stator resistance in Ohm - x65536 - (1<<16)

    MCI_AI_INTERNAL_MCI_STATE,                      //!< 24 - Returns the internal Mci state
    MCI_AI_CURR_PHASE,                              //!< 25 - Phase current in fixed point (Adcounts)
    MCI_AI_VOLTAGE_PHASE,                           //!< 26 - Motor phase voltage in fixed point (Adcounts)
    MCI_AI_CURR_PHASE_P2P,                          //!< 27 - Peak to Peak phase current
    MCI_AI_VOLTAGE_PHASE_P2P,                       //!< 28 - Peak to Peak phase voltage

    MCI_AI_NR_OF_AI                                 //!< Total declared of analog inputs
} MCI_AI_TYPE;




//! Digital outputs from application
typedef enum
{
    MCI_DO_SET_DYN_BRAKE_ACTIVE = 0,                //!< 0 - [T/F] Returns if dynamic brake is active
    MCI_DO_FORCE_RES_MEAS,                          //!< 1 - [T/F] Forces resistance estimation
    MCI_DO_IMPROVE_SENSING,                         //!< 2 - [T/F] Improve sensing (enlarges current, POLLING required!)
    MCI_DO_CLEAR_RES_ESTIMATED_FLAG,                //!< 3 - [T/F] Clears the estimated resistance flag.
    MCI_DO_MCI_PARAMS_LOADED,                       //!< 4 - [T/F] MCI Params loaded from Setting File.

    MCI_DO_NR_OF_DO                                 //!< Total of declared digital outputs
} MCI_DO_TYPE;




//! Analog outputs from application
typedef enum
{
    MCI_AO_SPEED_LOOP_PROP_GAIN_S16,                //!<  0 - Proportional speed loop gain - x65536 - (1<<16)
    MCI_AO_SPEED_LOOP_INT_GAIN_S16,                 //!<  1 - Integral speed loop gain - x65536 - (1<<16)
    MCI_AO_SPEED_LOOP_GAIN_TABLE_INDEX,             //!<  2 - Select index of speed loop gain table
    MCI_AO_SPEED_FIR_TAPS_TABLE_INDEX,              //!<  3 - Select index of speed fir taps

    MCI_AO_SOFTSTART_COUNTER_INDEX,                 //!<  4 - Select index of soft start counter
    MCI_AO_SPEED_LIMIT_INDEX,                       //!<  5 - Select index of speed controller limit
    MCI_AO_MCL_PARAMETERS_ADDRESS,                  //!<  6 - Set the address of Mcl parameters structure
    MCI_AO_ADD_DELTA_SPEED_REF_S16,                 //!<  7 - Add (force) a delta speed on the speed reference - x65536 - (1<<16)

    MCI_AO_MOTOR_TEMPERATURE_S16,                   //!<  8 - Write motor temperature value - x65536 - (1<<16)
    MCI_AO_PUBLISH_MOTOR_SELECTION,                 //!<  9 - Publish when a motor is set.
    MCI_AO_PUBLISH_FAILURE_COUNTER,                 //!<  10 - Publish restart counter

    MCI_AO_NR_OF_AO                                 //!< Total of declared analog outputs
} MCI_AO_TYPE;




//! Manual control method of the inverter
typedef enum
{
    MCI_MANUAL_INJECTION_DC_VOLTAGE = 0,            //!<  0 - Inject DC voltage in the motor, requires voltage level and position
    MCI_MANUAL_INJECTION_AC_VOLTAGE,                //!<  1 - Inject AC voltage in the motor, requires voltage level and frequency
    MCI_MANUAL_INJECTION_DC_CURRENT,                //!<  2 - Inject DC current in the motor, requires current level [A], current rate [A/s] and position [degree]
    MCI_MANUAL_INJECTION_AC_CURRENT,                //!<  3 - Inject AC current in the motor, requires current level and frequency
    MCI_MANUAL_INJECTION_FIXED_ANGLE,               //!<  4 - Inject fixed angle - for single phase motors
} MCI_INJECTION_TYPE;




//! Response definition
typedef enum
{
    MCI_CMD_ANGLE_OUT_OF_RANGE      = -8,           //!< -8 - [T/F] - command was denied by Mci because angle set is lower than minimum
    MCI_CMD_INDEX_OUT_RANGE         = -7,           //!< -7 - [T/F] - command was denied by Mci because selected motor does not exist
    MCI_CMD_STOP_REQUESTED          = -6,           //!< -6 - [T/F] - command was denied by Mci because motor is stopping by user request
    MCI_CMD_PARAM_NOT_LOADED        = -5,           //!< -5 - [T/F] - command was denied by Mci because parameters are not loaded

    MCI_CMD_TIMEOUT                 = -4,           //!< -4 - [T/F] - Command was timeout due to long feedback time
    MCI_CMD_RAMP_OUT_OF_RANGE       = -3,           //!< -3 - [T/F] - Command was denied by Mci due to acceleration out of range
    MCI_CMD_SPEED_OUT_OF_RANGE      = -2,           //!< -2 - [T/F] - Command was denied by Mci due to speed out of range
    MCI_CMD_DENIED                  = -1,           //!< -1 - [T/F] - Command was denied by Mci - reason not disclosed.

    MCI_CMD_ACCEPTED                =  0,           //!<  0 - [T/F] - Command was accepted by Mci
} MCI_CMD_RESPONSE_TYPE;



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void Mci__Initialize(void);                         //!< Initialize Class A once after power up/reset
void Mci__PwmHandler(void);                         //!< Handler running at MCPwm rate
void Mci__HallHandler(void);                        //!< Handler running at Hall sensors rate
void Mci__MotorControlHandler(void);                //!< Handler running at 200 us (5 kHz) rate
void Mci__TriacFireHandler(void);
void Mci__250usHandler(void);                       //!< Handler running at 250 us (4 kHz) rate
void Mci__1msHandler(void);                         //!< Handler running at 1 ms (1 kHz) rate
void Mci__5msHandler(void);                         //!< Handler running at 5 ms (200 Hz) rate
void Mci__SlowHandler(void);                        //!< Handler running at 25 ms (40 Hz) rate

//! Commands to the motor
MCI_CMD_RESPONSE_TYPE Mci__Run(MOTOR_ENUM_TYPE motor, sint16 speed_rpm, sint16 ramp_rpm_per_s);
MCI_CMD_RESPONSE_TYPE Mci__Stop(MOTOR_ENUM_TYPE motor, sint16 ramp_rpm_per_s);
void Mci__SetWashMotorType(APL_WASH_MOTOR_TYPE wash_type);

//! Get and read data from the motor controller
uint8  Mci__GetError(MOTOR_ENUM_TYPE motor);
uint32 Mci__GetErrorList(MOTOR_ENUM_TYPE motor);
uint8  Mci__GetDigital(MOTOR_ENUM_TYPE motor, MCI_DI_TYPE read_data);
uint32 Mci__GetDigitalList(MOTOR_ENUM_TYPE motor);
sint32 Mci__GetAnalog(MOTOR_ENUM_TYPE motor, MCI_AI_TYPE read_data);

//! Set and write data to the motor controller
uint8 Mci__ClearError(MOTOR_ENUM_TYPE motor);
uint8 Mci__SetDigital(MOTOR_ENUM_TYPE motor, MCI_DO_TYPE write_data, uint8 value);
uint8 Mci__SetAnalog(MOTOR_ENUM_TYPE motor, MCI_AO_TYPE write_data, sint32 value);

//! Abstraction Layer Interface
void Mci__ZeroCrossHandler(TC_EVENT_TYPE event, uint16 ccr_value);

#endif // MCI_H_
