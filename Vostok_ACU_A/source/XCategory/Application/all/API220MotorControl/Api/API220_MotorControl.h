/**
 *  @file       API220_MotorControl.h
 *
 *  @brief      Define the API of API220 used for motor control purposes. The defines of read and written
 *              paramters from and to Mci must comply with the same sequence defined in the Mci.h. Otherwise,
 *              unpredictable result may occur. Make sure the latest and/or compatible version of
 *              the Mci API (Mci.h) is installed within your project.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header:
 *
 *  @copyright  Copyright 2014-$Date: 2016/01/13 14:00:20BRST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API220_MOTORCONTROL_H_
    #define API220_MOTORCONTROL_H_


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API220_MOTOR_CONTROL_NUM                    220     //!< Define the 220 API number



//! Definition of all OpCodes used in the API220
typedef enum
{
    //Op Code Message Type Firewalled Throws
    API220_OPCODE_MOTOR_CONTROL_RUN                 = 1,    //!< 1 : Run motor  COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_STOP                = 2,    //!< 2 : Stop motor  COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_GET_DIGITAL         = 3,    //!< 3 : Get digital  COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_SET_DIGITAL         = 4,    //!< 4 : Set digital  COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_GET_ANALOG          = 5,    //!< 5 : Get analog  COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_SET_ANALOG          = 6,    //!< 6 : Set analog  COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_MANUAL_CONTROL      = 7,    //!< 7 : Manual injection COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_GET_ERROR           = 8,    //!< 7 : Manual injection COMMAND false N/A
    API220_OPCODE_MOTOR_CONTROL_PUBLISH_PERIOD      = 17,   //!< 17 : Publish analog data periodically
    API220_OPCODE_MOTOR_CONTROL_FVT                 = 18,   //!< 18 : FVT mode, functional verification tests
    API220_OPCODE_MOTOR_CONTROL_DEBUG               = 20,   //!< 20 : Debug command (not intended for production)
} API220_OPCODES_TYPE;




//! FVT Commands. This are definitions used with main OpCode API220_OPCODE_MOTOR_CONTROL_FVT
typedef enum
{
    API220_FVT_START = 1,                                   //!< 1 - Start FVT tests sequence
    API220_FVT_STOP,                                        //!< 2 - Stop FVT tests
    API220_FVT_REQUEST_RESULT,                              //!< 3 - Request results from FVT
    API220_FVT_REQUEST_RESULT_FVT2_1,                       //!< 4 - Request results from FVT 2.0 - first part
    API220_FVT_REQUEST_RESULT_FVT2_2,                       //!< 5 - Request results from FVT 2.0 - second part
} API220_FVT_COMMANDS_TYPE;




//! Digital inputs definition to application
typedef enum
{
    API220_MCI_DI_OVER_TORQUE = 0,                          //!<  0 - [T/F] Over torque detected
    API220_MCI_DI_MOTOR_STOP,                               //!<  1 - [T/F] Motor stop
    API220_MCI_DI_REACHED_TARGET_SPEED,                     //!<  2 - [T/F] Target speed == reference speed
    API220_MCI_DI_IDLE,                                     //!<  3 - [T/F] Motor idle, if the speed is zero, no error found.

    API220_MCI_DI_MOTOR_RUNNING,                            //!<  4 - [T/F] Motor running
    API220_MCI_DI_RESIST_ESTIMATED,                         //!<  5 - [T/F] Read resistance was estimated
    API220_MCI_DI_PARAMETERS_LOADED,                        //!<  6 - [T/F] Class A and B - SR_FLAG_MC_PARAMETERS_LOADED

    API220_MCI_DI_NR_OF_DI                                  //!< Total declared of digital inputs
} API220_MCI_DI_TYPE;




//! Error definition to application
typedef enum
{
    API220_MCI_ERROR_ANY_ERROR_FOUND = 0,                   //!<  0 - [T/F] Any error found?
    API220_MCI_ERROR_INITIALIZATION_FAILED,                 //!<  1 - [T/F] Mci Initialization phase failed
    API220_MCI_ERROR_DCBUS_OVER_VOLTAGE,                    //!<  2 - [T/F] DC bus link over voltage detected
    API220_MCI_ERROR_DCBUS_UNDER_VOLTAGE,                   //!<  3 - [T/F] DC bus link under voltage detected

    API220_MCI_ERROR_INVERTER_OVER_TEMP,                    //!<  4 - [T/F] Module over temperature
    API220_MCI_ERROR_LOCKED_ROTOR_AT_STARTUP,      	        //!<  5 - [T/F] Locked rotor detected at startup
    API220_MCI_ERROR_LOCKED_ROTOR_IN_RUNNING,               //!<  6 - [T/F] Locked rotor detected in running
    API220_MCI_ERROR_MOTOR_OVERHEATING,                     //!<  7 - [T/F] Motor overheating

    API220_MCI_ERROR_CURRENT_SENSOR_FAILED,                 //!<  8 - [T/F] From Class B - SR_FAULT_CURRENT_SENSOR_FAILED
    API220_MCI_ERROR_VOLTAGE_SENSOR_FAILED,                 //!<  9 - [T/F] From Class B - SR_FAULT_VOLTAGE_SENSOR_FAILED
    API220_MCI_ERROR_SW_OVER_CURRENT,                       //!< 10 - [T/F] Motor over current detected by logic/software
    API220_MCI_ERROR_HW_OVER_CURRENT,                       //!< 11 - [T/F] Motor over current detected by peripheral/hardware

    API220_MCI_ERROR_SPEED_CHECK_FAILED,                    //!< 12 - [T/F] Safety Speed check for door unlock failed
    API220_MCI_ERROR_PHASE_LOST,                            //!< 13 - [T/F] Phase lost detected
    API220_MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED,  //!< 14 - [T/F] Safety Input Capture plausibility check failed
    API220_MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED,  //!< 15 - [T/F] Safety Input Capture plausibility check for maximum speed change failed

    API220_MCI_ERROR_INT_DISABLED,                          //!< 16 - [T/F] Interrupt was disabled
    API220_MCI_ERROR_STATOR_OVER_TEMP,                      //!< 17 - [T/F] Stator temperature exceeded the limit and will prevent start for 30 seconds
    API220_MCI_ERROR_CLASS_B_FORCE2STOP,                    //!< 18 - [T/F] Class B forced to stop without any Class B call
    API220_MCI_ERROR_SHUTDOWN_HARDWARE_FAILED,              //!< 19 - [T/F] From Class B - SR_FAULT_SHUTDOWN_HARDWARE_FAILED

	API220_MCI_ERROR_OBSERVER_FAILURE,                      //!< 20 - [T/F] Observer failure - lost observability
	API220_MCI_ERROR_NOT_USED,                              //!< 21 - [T/F] Not Used
	API220_MCI_ERROR_SURGE_RELAY_OPEN,                      //!< 22 - [T/F] if set this bit if surge relay is open

    API220_MCI_ERROR_NR_OF_DI                               //!< Total declared of digital inputs
} API220_MCI_ERROR_TYPE;




//! Analog inputs to application
typedef enum
{
    API220_MCI_AI_TARGET_SPEED_RPM_S16 = 0,                 //!<  0 - Target rotor shaft speed in RPM - x65536 - (1<<16)
    API220_MCI_AI_SPEED_REFERENCE_RPM_S16,                  //!<  1 - Speed reference in RPM - x65536 - (1<<16)
    API220_MCI_AI_ACTUAL_SPEED_S16,                         //!<  2 - Estimated rotor shaft speed in RPM - x65536 - (1<<16)
    API220_MCI_AI_SPEED_ERROR_RPM_S16,                      //!<  3 - Difference between estimated and reference speeds in RPM - x65536 - (1<<16)

    API220_MCI_AI_MEAN_SPEED_S16,                           //!<  4 - Mean rotor speed calculation - x65536 - (1<<16)
    API220_MCI_AI_RMS_MOTOR_CURRENT_S16,                    //!<  5 - RMS Motor current - x65536 - (1<<16)
    API220_MCI_AI_ACTIVE_POWER_S16,                         //!<  6 - Measured active power in Watts - x65536 - (1<<16)
    API220_MCI_AI_SHAFT_POWER_S16,                          //!<  7 - Estimated shaft output power in Watts from estimated torque - x65536 - (1<<16)

    API220_MCI_AI_MOTOR_TEMP_S16,                           //!<  8 - Motor temperature in Celsius - x65536 - (1<<16)
    API220_MCI_AI_STATOR_TEMP_S16,                          //!<  9 - Stator temperature calculated by stator resistance - x65536 - (1<<16)
    API220_MCI_AI_LOAD_TORQUE_S16,                          //!< 10 - Read the load torque in Nm - x65536 - (1<<16)
    API220_MCI_AI_SHAFT_POSITION_DEG_S16,                   //!< 11 - Estimated rotor position in deg - x65536 - (1<<16)

    API220_MCI_AI_BUS_CURRENT_S16,                          //!< 12 - Bus current in Amps - x65536 - (1<<16)
    API220_MCI_AI_BUS_VOLTAGE_S16,                          //!< 13 - Bus voltage in Volts - x65536 - (1<<16)
    API220_MCI_AI_INVERTER_TEMP_S16,                        //!< 14 - Inverter temperature in Celsius - x65536 - (1<<16)
    API220_MCI_AI_INVERTER_TEMP_RAW_DATA,                   //!< 15 - Inverter temperature in raw data [ADC counts]

    API220_MCI_AI_VOLTAGE_PHASE_A_S16,                      //!< 16 - Motor phase A voltage in Volts - x65536 - (1<<16)
    API220_MCI_AI_VOLTAGE_PHASE_B_S16,                      //!< 17 - Motor phase B voltage in Volts - x65536 - (1<<16)
    API220_MCI_AI_VOLTAGE_PHASE_C_S16,                      //!< 18 - Motor phase C voltage in Volts - x65536 - (1<<16)
    API220_MCI_AI_SPEED_LOOP_GAIN_TABLE_INDEX,              //!< 19 - Select index of speed loop gain table

    API220_MCI_AI_CURR_PHASE_A_S16,                         //!< 20 - Phase current A in A - x65536 - (1<<16)
    API220_MCI_AI_CURR_PHASE_B_S16,                         //!< 21 - Phase current B in A - x65536 - (1<<16)
    API220_MCI_AI_CURR_PHASE_C_S16,                         //!< 22 - Phase current C in A - x65536 - (1<<16)
    API220_MCI_AI_STATOR_RESISTANCE_S16,                    //!< 23 - Stator resistance in Ohm - x65536 - (1<<16)

    API220_MCI_AI_INTERNAL_API220_MCI_STATE,                //!< 24 - Returns the internal Mci state

    API220_MCI_AI_NR_OF_AI                                  //!< Total declared of analog inputs
} API220_MCI_AI_TYPE;




//! Digital outputs from application
typedef enum
{
    API220_MCI_DO_SET_DYN_BRAKE_ACTIVE = 0,                 //!< 0 - [T/F] Returns if dynamic brake is active
    API220_MCI_DO_FORCE_RES_MEAS,                           //!< 1 - [T/F] Forces resistance estimation
    API220_MCI_DO_IMPROVE_SENSING,                          //!< 2 - [T/F] Improve sensing (enlarges current, POLLING required!)
    API220_MCI_DO_CLEAR_RES_ESTIMATED_FLAG,                 //!< 3 - [T/F] Clears the estimated resistance flag.

    API220_MCI_DO_NR_OF_DO                                  //!< Total of declared digital outputs
} API220_MCI_DO_TYPE;




//! Analog outputs from application
typedef enum
{
    API220_MCI_AO_SPEED_LOOP_PROP_GAIN_S16,                 //!<  0 - Proportional speed loop gain - x65536 - (1<<16)
    API220_MCI_AO_SPEED_LOOP_INT_GAIN_S16,                  //!<  1 - Integral speed loop gain - x65536 - (1<<16)
    API220_MCI_AO_SPEED_LOOP_GAIN_TABLE_INDEX,              //!<  2 - Select index of speed loop gain table
    API220_MCI_AO_SPEED_FIR_TAPS_TABLE_INDEX,               //!<  3 - Select index of speed fir taps

    API220_MCI_AO_SOFTSTART_COUNTER_INDEX,                  //!<  4 - Select index of soft start counter
    API220_MCI_AO_SPEED_LIMIT_INDEX,                        //!<  5 - Select index of speed controller limit
    API220_MCI_AO_MCL_PARAMETERS_ADDRESS,                   //!<  6 - Set the address of Mcl parameters structure
    API220_MCI_AO_ADD_DELTA_SPEED_REF_S16,                  //!<  7 - Add (force) a delta speed on the speed reference - x65536 - (1<<16)

    API220_MCI_AO_MOTOR_TEMPERATURE_S16,                    //!<  8 - Write motor temperature value - x65536 - (1<<16)

    API220_MCI_AO_NR_OF_AO                                  //!< Total of declared analog outputs
} API220_MCI_AO_TYPE;




//! Manual control method of the inverter
typedef enum
{
    API220_MCI_MANUAL_INJECTION_DC_VOLTAGE = 0,             //!<  0 - Inject DC voltage in the motor, requires voltage level and position
    API220_MCI_MANUAL_INJECTION_AC_VOLTAGE,                 //!<  1 - Inject AC voltage in the motor, requires voltage level and frequency
    API220_MCI_MANUAL_INJECTION_DC_CURRENT,                 //!<  2 - Inject DC current in the motor, requires current level [A], current rate [A/s] and position [degree]
    API220_MCI_MANUAL_INJECTION_AC_CURRENT,                 //!<  3 - Inject AC current in the motor, requires current level and frequency
} API220_MCI_INJECTION_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#endif // API220_MOTORCONTROL_H_





