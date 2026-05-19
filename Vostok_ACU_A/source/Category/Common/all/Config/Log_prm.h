/**
 *  @file
 *  @brief      Automatically generated public parameters for the Log module.
 *
 *  @details    This file was automatically generated on 11/21/2024 1:40:25 PM by
 *              LogPreprocessor.exe (v1.3.0)  Copyright © 2013-2017 Whirlpool Corporation.
 *
 *              LogPreprocessor.exe attempts to preserve user settings.
 *              The user should test to make sure that changes to this file are handled as expected.
 *
 *  $Header: Program.cs 1.13 2015/10/30 11:22:11EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 *  @copyright  Copyright 2012 - $Date: 2015/10/30 11:22:11EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef LOG_PRM_H_
#define LOG_PRM_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Parameters ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Compiler Directives --


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_MODULE_ENABLE declaration is used to enable or disable the Log module.
 *
 * Valid settings:
 *
 * ENABLED = Module works as intended. Functions are defined and compiled into the code.
 *
 * DISABLED = The module's public interface is replaced with dummy macros so that the system will
 *          compile. However, none of the functionality is present.
 *
 * The Log.h file shall declare LOG_MODULE_ENABLE as ENABLED if it is missing.
 */
#define LOG_MODULE_ENABLE                           (ENABLED)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_PEEK_BUFFER_FEATURE declaration is used to enable or disable the Log__PeekBuffer()
 * function.
 *
 * ENABLED = The Log__PeekBuffer() function is defined and available to the application.
 *
 * DISABLED = The Log__PeekBuffer() function is not defined.
 *
 * The Log.h file shall declare LOG_PEEK_BUFFER_FEATURE as DISABLED if it is missing.
 */
#define LOG_PEEK_BUFFER_FEATURE                     (DISABLED)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_RUN_TIME_FILTERING declaration is used to enable or disable the Log__SetMessageFilter()
 * function.
 *
 * ENABLED = The Log__SetMessageFilter() function is defined and available to the application.
 *          Initial message filters are determined at compile-time through the Log_prva.h file,
 *          but they can be modified at run-time through the Log__SetMessageFilter() function.\
 *          This feature requires an extra byte of RAM for each module that generates Log Messages.
 *
 * DISABLED = The Log__SetMessageFilter() function is not defined. Message filters are determined
 *          at compile-time through the Log_prva.h file.
 *
 * The Log.h file shall declare LOG_RUN_TIME_FILTERING as DISABLED if it is missing.
 */
#define LOG_RUN_TIME_FILTERING                      (DISABLED)


// -- Constant Declarations --

/**
 * The maximum number of Log Messages that the Message Queue can hold.
 * Set this to a value in the range [1..255].
 * The Log module will allocate 5 bytes of RAM for each message that the buffer can hold.
 */
#define LOG_MAX_MESSAGES                            (10)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Automated Parameters ------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Automated list of Module Identifiers for all the application modules that generate Log Messages.
 *
 * The LogPreprocessor.exe tool searches all project code for any source files that declare
 * Message Identifiers. The search looks for the pattern below where <NAME> is replaced with the
 * name of the module.
 * <pre>
 *      typedef enum
 *      {
 *          // Log messages enumerated here...
 *      } MODULE_<NAME>_LOG_MESSAGE_ID_TYPE;
 * </pre>
 *
 * For example, if a 'Hello' module declares the MODULE_HELLO_LOG_MESSAGE_ID_TYPE type, then the
 * LogPreprocessor.exe tool will add an enumerated value 'MODULE_HELLO' to the LOG_MODULE_ID_TYPE
 * declared below.
 */
typedef enum LOG_MODULE_ID_ENUM
{
    MODULE_ABSOLUTE_HUMIDITY_LLI             = 0,   // 0x00
    MODULE_ACCEL_BASELINE                    = 1,   // 0x01
    MODULE_ACTIVATE_TASK                     = 2,   // 0x02
    MODULE_AIRLOCK_MONITOR                   = 3,   // 0x03
    MODULE_ALTERNATE                         = 4,   // 0x04
    MODULE_API007APP                         = 5,   // 0x05
    MODULE_API007BULKDATA                    = 6,   // 0x06
    MODULE_API007NVDATA                      = 7,   // 0x07
    MODULE_API010_POLL_VAR                   = 8,   // 0x08
    MODULE_API011APPCTRL                     = 9,   // 0x09
    MODULE_API012EXPANSION                   = 10,  // 0x0A
    MODULE_API013_REMOTE_FUNCTION            = 11,  // 0x0B
    MODULE_API221_MOTION_CTRL                = 12,  // 0x0C
    MODULE_APPLIANCE_MANAGER                 = 13,  // 0x0D
    MODULE_AUTOSTARTFACTORYCYCLE_MONITOR     = 14,  // 0x0E
    MODULE_BROWNOUT                          = 15,  // 0x0F
    MODULE_CALIBRATION                       = 16,  // 0x10
    MODULE_CLASS_A_FILL                      = 17,  // 0x11
    MODULE_CONDITION                         = 18,  // 0x12
    MODULE_CROSSLOADS                        = 19,  // 0x13
    MODULE_DC_LOAD_MONITOR                   = 20,  // 0x14
    MODULE_DIGITALOUTPUTPILOT                = 21,  // 0x15
    MODULE_DOOR_OPENING_SYSTEM_MONITOR       = 22,  // 0x16
    MODULE_DOOR_OPENING_SYSTEM_PULL          = 23,  // 0x17
    MODULE_DOOR_OPENING_SYSTEM_PUSH          = 24,  // 0x18
    MODULE_EQUATION                          = 25,  // 0x19
    MODULE_ERROR_EVENTS                      = 26,  // 0x1A
    MODULE_ESTIMATOR                         = 27,  // 0x1B
    MODULE_EXTERNAL_MOTOR_PILOT              = 28,  // 0x1C
    MODULE_FAN_MOTOR_FAULT_MONITOR           = 29,  // 0x1D
    MODULE_FAULT                             = 30,  // 0x1E
    MODULE_FILL                              = 31,  // 0x1F
    MODULE_FILL_VALVE_FAULT_MONITOR          = 32,  // 0x20
    MODULE_FILTERS                           = 33,  // 0x21
    MODULE_FXLS8974                          = 34,  // 0x22
    MODULE_HBL                               = 35,  // 0x23
    MODULE_HCD_MONITOR                       = 36,  // 0x24
    MODULE_HEAT                              = 37,  // 0x25
    MODULE_HEAT_MONITOR                      = 38,  // 0x26
    MODULE_HEATER_FAULT_MONITOR              = 39,  // 0x27
    MODULE_HEATER_SHADOWING                  = 40,  // 0x28
    MODULE_HOTHIGHPOT_MONITOR                = 41,  // 0x29
    MODULE_I2C_INFRARED_LLI                  = 42,  // 0x2A
    MODULE_I2C_RHT_SENSOR_LLI                = 43,  // 0x2B
    MODULE_I2CPRESSURESENSOR                 = 44,  // 0x2C
    MODULE_INTERPRETER                       = 45,  // 0x2D
    MODULE_JUMPIF                            = 46,  // 0x2E
    MODULE_LISxDH                            = 47,  // 0x2F
    MODULE_LITT_FAULT_MONITOR                = 48,  // 0x30
    MODULE_LITT_MONITOR                      = 49,  // 0x31
    MODULE_LOADS_CONTROL                     = 50,  // 0x32
    MODULE_MAGNETRON_PILOT                   = 51,  // 0x33
    MODULE_MAGNETRON_PILOT_MAINS_SAMPLER     = 52,  // 0x34
    MODULE_MAGNETRON_PILOT_RELAY_SYNC        = 53,  // 0x35
    MODULE_MAINTAIN                          = 54,  // 0x36
    MODULE_MCI                               = 55,  // 0x37
    MODULE_MODIFIER                          = 56,  // 0x38
    MODULE_MOISTURE_PREVENTION               = 57,  // 0x39
    MODULE_MOTOR_PILOT                       = 58,  // 0x3A
    MODULE_NORMAL_FILL                       = 59,  // 0x3B
    MODULE_NVDATA                            = 60,  // 0x3C
    MODULE_OWI                               = 61,  // 0x3D
    MODULE_OWI_MONITOR                       = 62,  // 0x3E
    MODULE_OWI_ONE_SHOT                      = 63,  // 0x3F
    MODULE_OWI_VRO_READ                      = 64,  // 0x40
    MODULE_PARTIALDRAIN                      = 65,  // 0x41
    MODULE_PILOT_RELAY_FAULT_MONITOR         = 66,  // 0x42
    MODULE_PLUGGING_MONITOR                  = 67,  // 0x43
    MODULE_PRODUCTINFO                       = 68,  // 0x44
    MODULE_PUMPCONTROL                       = 69,  // 0x45
    MODULE_REGENERATE                        = 70,  // 0x46
    MODULE_REGULATIONS                       = 71,  // 0x47
    MODULE_RINSE_AID                         = 72,  // 0x48
    MODULE_RINSE_AID_DISPENSE                = 73,  // 0x49
    MODULE_RINSE_AID_MONITOR                 = 74,  // 0x4A
    MODULE_SALT_SENSING_MONITOR              = 75,  // 0x4B
    MODULE_SANITIZATION                      = 76,  // 0x4C
    MODULE_SELECTOR                          = 77,  // 0x4D
    MODULE_SENSEDDRAIN                       = 78,  // 0x4E
    MODULE_SETLOADQUEUE                      = 79,  // 0x4F
    MODULE_SETTINGFILE                       = 80,  // 0x50
    MODULE_SETVARIABLE                       = 81,  // 0x51
    MODULE_SFVERSIONCHECK                    = 82,  // 0x52
    MODULE_SREXCEPTION                       = 83,  // 0x53
    MODULE_SSDRAIN_MOTOR_FAULT_MONITOR       = 84,  // 0x54
    MODULE_STABILITY_CONTROL                 = 85,  // 0x55
    MODULE_STATISTICS                        = 86,  // 0x56
    MODULE_STATUS                            = 87,  // 0x57
    MODULE_STATUSLED                         = 88,  // 0x58
    MODULE_SUBCYCLE                          = 89,  // 0x59
    MODULE_SUPERVISOR                        = 90,  // 0x5A
    MODULE_SYSTEMTIMERS                      = 91,  // 0x5B
    MODULE_TANK_DRAIN                        = 92,  // 0x5C
    MODULE_TANK_FILL                         = 93,  // 0x5D
    MODULE_THERMISTOR_FAULT_MONITOR          = 94,  // 0x5E
    MODULE_TRIAC_SHORTED_DRAIN_FAULT_MONITOR = 95,  // 0x5F
    MODULE_TRIACMCI                          = 96,  // 0x60
    MODULE_TRIDGE_DETERGENT                  = 97,  // 0x61
    MODULE_VARIABLE                          = 98,  // 0x62
    MODULE_VOLTAGE_RANGE                     = 99,  // 0x63
    MODULE_VSDRAIN_MOTOR_FAULT_MONITOR       = 100, // 0x64
    MODULE_VWI                               = 101, // 0x65
    MODULE_VWI_MONITOR                       = 102, // 0x66
    MODULE_WASH_MOTOR_FAULT_MONITOR          = 103, // 0x67
    MODULE_ZEROCROSS                         = 104, // 0x68

    //! Number of modules that can add Log Messages to the Message Queue.
    NUMBER_OF_MODULES                        = 105
} LOG_MODULE_ID_TYPE;


//---------------------------------------------------------------------------------------------------------------------
/**
 * Automated list of unique identifiers for all application specific Log Messages from all modules.
 *
 * The LogPreprocessor.exe tool searches the code for any source files that declare log messages.
 * Every enumerated message from every module is assigned a unique identifier based on the
 * Module Identifier and the Message Identifier.
 *
 * The debugger will then display the enumeration text instead of the decimal value, making
 * viewing the Log messages much easier during debugging.
 */
typedef enum LOG_MODULE_MESSAGE_ID_ENUM
{
    MODULE_ABSOLUTE_HUMIDITY_LLI_CONFIGURE_ENABLE_PIN_FAILED                        = 1,     // 0x0001
    MODULE_ABSOLUTE_HUMIDITY_LLI_CONFIGURE_FEEDBACK_PIN_FAILED                      = 2,     // 0x0002
    MODULE_ABSOLUTE_HUMIDITY_LLI_CONFIGURE_PWM_PIN_FAILED                           = 3,     // 0x0003
    MODULE_ABSOLUTE_HUMIDITY_LLI_CONFIGURE_PWM_FAILED                               = 4,     // 0x0004
    MODULE_ABSOLUTE_HUMIDITY_LLI_HUMIDITY_SENSOR_ERROR_PRESENT_IN_UNBALANCED        = 5,     // 0x0005
    MODULE_ABSOLUTE_HUMIDITY_LLI_HUMIDITY_SENSOR_ERROR_PRESENT_IN_BALANCED          = 6,     // 0x0006
    MODULE_ABSOLUTE_HUMIDITY_LLI_HUMIDITY_SENSOR_ERROR_CANCEL                       = 7,     // 0x0007
    MODULE_ACCEL_BASELINE_INVALID_ACCELEROMETER_PARAMETERS                          = 257,   // 0x0101
    MODULE_ACCEL_BASELINE_COMMAND_TOOK_TOO_LONG                                     = 258,   // 0x0102
    MODULE_ACCEL_BASELINE_SELF_TEST_NOT_AVAILABLE                                   = 259,   // 0x0103
    MODULE_ACCEL_BASELINE_VALIDATION_TEST_NOT_AVAILABLE                             = 260,   // 0x0104
    MODULE_ACCEL_BASELINE_BASELINE_MEASUREMENT_NOT_AVAILABLE                        = 261,   // 0x0105
    MODULE_ACCEL_BASELINE_VALIDATION_TEST_FAILED                                    = 262,   // 0x0106
    MODULE_ACCEL_BASELINE_SELF_TEST_FAILED                                          = 263,   // 0x0107
    MODULE_ACTIVATE_TASK_INVALID_PROCESS_CONTROL                                    = 513,   // 0x0201
    MODULE_ACTIVATE_TASK_INVALID_PROCESS_PARAMETERS                                 = 514,   // 0x0202
    MODULE_ACTIVATE_TASK_INVALID_TASK_MANIPULATION_PARAMETER                        = 515,   // 0x0203
    MODULE_ACTIVATE_TASK_NON_EXISTENT_TASK                                          = 516,   // 0x0204
    MODULE_AIRLOCK_MONITOR_ERROR_LOADING_PARAMETERS                                 = 769,   // 0x0301
    MODULE_ALTERNATE_ERROR_PROCESS_CONTROL                                          = 1025,  // 0x0401
    MODULE_ALTERNATE_ERROR_LOADING_PARAMETERS                                       = 1026,  // 0x0402
    MODULE_ALTERNATE_ERROR_WAIT_STOPPED                                             = 1027,  // 0x0403
    MODULE_ALTERNATE_ERROR_WAIT_STOPPED_MAX_ATTEMPS                                 = 1028,  // 0x0404
    MODULE_ALTERNATE_ERROR_WAIT_STATUS                                              = 1029,  // 0x0405
    MODULE_ALTERNATE_ERROR_WAIT_STATUS_MAX_ATTEMPS                                  = 1030,  // 0x0406
    MODULE_ALTERNATE_ERROR_MCU_RESET                                                = 1031,  // 0x0407
    MODULE_API007APP_INVALID_CRC_FAILED                                             = 1281,  // 0x0501
    MODULE_API007BULKDATA_MESSAGE_NONE                                              = 1536,  // 0x0600
    MODULE_API007NVDATA_ERROR_INSUFFECIENT_API007NVDATA_CLIENT_NUM_BLOCKS           = 1792,  // 0x0700
    MODULE_API007NVDATA_ERROR_UNABLE_TO_ALLOCATE_CLAIMED_SIZE_IN_SERVER             = 1793,  // 0x0701
    MODULE_API007NVDATA_ERROR_SERVER_NOT_SENDING_FEEDBACK                           = 1794,  // 0x0702
    MODULE_API010_POLL_VAR_TOO_MUCH_DATA                                            = 2049,  // 0x0801
    MODULE_API011APPCTRL_TO_MANY_CONSECUTIVE_REGULATIONS_SETS                       = 2304,  // 0x0900
    MODULE_API011APPCTRL_TO_MANY_CONSECUTIVE_REGULATIONS_REQUESTS                   = 2305,  // 0x0901
    MODULE_API011APPCTRL_TO_MANY_CONSECUTIVE_STATUS_REQUESTS                        = 2306,  // 0x0902
    MODULE_API012EXPANSION_INVALID_CONFIGURATION_PARAMETERS                         = 2561,  // 0x0A01
    MODULE_API012EXPANSION_CONFIGURATION_PARAMETERS_NOT_FOUND                       = 2562,  // 0x0A02
    MODULE_API012EXPANSION_QUEUEING_ON_REVEAL                                       = 2563,  // 0x0A03
    MODULE_API013_REMOTE_FUNCTION_FEEDBACK_MESSAGE_NOT_HANDLED                      = 2817,  // 0x0B01
    MODULE_API221_MOTION_CTRL_MOTOR_STOPPED_DUE_TO_TIMEOUT                          = 3073,  // 0x0C01
    MODULE_API221_MOTION_CTRL_MOTOR_REJECTED_STOP_FROM_TIMEOUT                      = 3074,  // 0x0C02
    MODULE_API221_MOTION_CTRL_PERIODIC_PUBLICATION_FAILED                           = 3075,  // 0x0C03
    MODULE_API221_MOTION_CTRL_FEEDBACK_OPCODE_NOT_SUPPORTED                         = 3076,  // 0x0C04
    MODULE_API221_MOTION_CTRL_ADD_CHANS_INVALID_PARAMS                              = 3077,  // 0x0C05
    MODULE_API221_MOTION_CTRL_CLEAR_FAILURES_INVALID_PARAMS                         = 3078,  // 0x0C06
    MODULE_API221_MOTION_CTRL_MOTION_INVALID_PARAMS                                 = 3079,  // 0x0C07
    MODULE_API221_MOTION_CTRL_PULSE_INVALID_PARAMS                                  = 3080,  // 0x0C08
    MODULE_API221_MOTION_CTRL_PULSE_INVALID_COMMAND                                 = 3081,  // 0x0C09
    MODULE_API221_MOTION_CTRL_ROTATE_INVALID_PARAMS                                 = 3082,  // 0x0C0A
    MODULE_API221_MOTION_CTRL_RUN_INVALID_PARAMS                                    = 3083,  // 0x0C0B
    MODULE_API221_MOTION_CTRL_STOP_INVALID_PARAMS                                   = 3084,  // 0x0C0C
    MODULE_API221_MOTION_CTRL_WASH_INVALID_PARAMS                                   = 3085,  // 0x0C0D
    MODULE_API221_MOTION_CTRL_GET_DATA_INVALID_PARAMS                               = 3086,  // 0x0C0E
    MODULE_API221_MOTION_CTRL_GET_SYNC_DATA_INVALID_PARAMS                          = 3087,  // 0x0C0F
    MODULE_API221_MOTION_CTRL_IS_MOTOR_PRESENT_INVALID_PARAMS                       = 3088,  // 0x0C10
    MODULE_API221_MOTION_CTRL_DEPRECATED_KEEP_RUNNING_API                           = 3089,  // 0x0C11
    MODULE_API221_MOTION_CTRL_REMOVE_CHANS_INVALID_PARAMS                           = 3090,  // 0x0C12
    MODULE_API221_MOTION_CTRL_REQ_ANALOG_DATA_INVALID_PARAMS                        = 3091,  // 0x0C13
    MODULE_API221_MOTION_CTRL_REQ_FAILURES_INVALID_PARAMS                           = 3092,  // 0x0C14
    MODULE_API221_MOTION_CTRL_REQ_PER_DATA_STATUS_INVALID_PARAMS                    = 3093,  // 0x0C15
    MODULE_API221_MOTION_CTRL_REQ_STATUS_INVALID_PARAMS                             = 3094,  // 0x0C16
    MODULE_API221_MOTION_CTRL_SET_PERIOD_INVALID_PARAMS                             = 3095,  // 0x0C17
    MODULE_APPLIANCE_MANAGER_MAX_TIMEOUT_REACHED                                    = 3329,  // 0x0D01
    MODULE_AUTOSTARTFACTORYCYCLE_MONITOR_ERROR_LOADING_PARAMETERS                   = 3585,  // 0x0E01
    MODULE_AUTOSTARTFACTORYCYCLE_MONITOR_ERROR_READING_SELECTOR_DATA                = 3586,  // 0x0E02
    MODULE_BROWNOUT_BROWNOUT_STABLE_POWER_DETECTED                                  = 3841,  // 0x0F01
    MODULE_BROWNOUT_BROWNOUT_EVENT_DETECTED                                         = 3842,  // 0x0F02
    MODULE_CALIBRATION_IMPROPER_BUFFER_SIZE                                         = 4097,  // 0x1001
    MODULE_CLASS_A_FILL_ERROR_LOADING_PARAMETERS                                    = 4353,  // 0x1101
    MODULE_CONDITION_EVAL_ERROR                                                     = 4608,  // 0x1200
    MODULE_CONDITION_LOAD_CONDITION_TABLE                                           = 4609,  // 0x1201
    MODULE_CONDITION_LOAD_VARIABLE_ERROR                                            = 4610,  // 0x1202
    MODULE_CONDITION_LOAD_VARIABLE_AS_OPERAND_ERROR                                 = 4611,  // 0x1203
    MODULE_CONDITION_OPERAND_ERROR                                                  = 4612,  // 0x1204
    MODULE_CONDITION_OPERATOR_ERROR                                                 = 4613,  // 0x1205
    MODULE_CROSSLOADS_NUM_OF_SETTING_CROSSLOADS_EXCEEDEDS_MAX_NUM                   = 4864,  // 0x1300
    MODULE_CROSSLOADS_INVALID_CROSSLOAD_HANDLER                                     = 4865,  // 0x1301
    MODULE_DC_LOAD_MONITOR_ERROR_MISSING_COMPONENTS                                 = 5121,  // 0x1401
    MODULE_DC_LOAD_MONITOR_ERROR_MISSING_PARAMETERS                                 = 5122,  // 0x1402
    MODULE_DIGITALOUTPUTPILOT_INVALID_VIRTUAL_PIN                                   = 5376,  // 0x1500
    MODULE_DOOR_OPENING_SYSTEM_MONITOR_ERROR_MISSING_PARAMETERS                     = 5633,  // 0x1601
    MODULE_DOOR_OPENING_SYSTEM_PULL_ERROR_MISSING_PARAMETERS                        = 5889,  // 0x1701
    MODULE_DOOR_OPENING_SYSTEM_PUSH_ERROR_MISSING_PARAMETERS                        = 6145,  // 0x1801
    MODULE_EQUATION_EQ_NO_ERROR                                                     = 6400,  // 0x1900
    MODULE_EQUATION_EQ_INVALID_SF_EQUATION                                          = 6401,  // 0x1901
    MODULE_EQUATION_EQ_UNSUPPORTED_EQUATION_ELEMENT                                 = 6402,  // 0x1902
    MODULE_EQUATION_EQ_UNBALANCED_EQUATION                                          = 6403,  // 0x1903
    MODULE_EQUATION_EQ_STACK_UNDERFLOW                                              = 6404,  // 0x1904
    MODULE_EQUATION_EQ_STACK_OVERFLOW                                               = 6405,  // 0x1905
    MODULE_EQUATION_EQ_ARITHMETIC_OVERFLOW                                          = 6406,  // 0x1906
    MODULE_EQUATION_EQ_DIVIDE_BY_ZERO                                               = 6407,  // 0x1907
    MODULE_EQUATION_EQ_INVALID_VARIABLE                                             = 6408,  // 0x1908
    MODULE_EQUATION_EQ_MAP_INVALID_SF_LOOK_UP_TABLE                                 = 6416,  // 0x1910
    MODULE_EQUATION_EQ_MAP_INVALID_INPUT_MIN_MAX                                    = 6417,  // 0x1911
    MODULE_EQUATION_EQ_MAP_INVALID_SF_INPUT_LIST                                    = 6418,  // 0x1912
    MODULE_EQUATION_EQ_MAP_INVALID_SF_OUTPUT_LIST                                   = 6419,  // 0x1913
    MODULE_EQUATION_EQ_MAP_OUTPUT_LIST_WRONG_SIZE                                   = 6420,  // 0x1914
    MODULE_ERROR_EVENTS_REGISTER_INVALID_OBSERVER_FAILED                            = 6656,  // 0x1A00
    MODULE_ERROR_EVENTS_UNREGISTER_INVALID_OBSERVER_FAILED                          = 6657,  // 0x1A01
    MODULE_ERROR_EVENTS_NOTIFY_INVALID_ERROR_EVENT_FAILED                           = 6658,  // 0x1A02
    MODULE_ERROR_EVENTS_REQUEST_INVALID_ERROR_EVENT_DATA                            = 6659,  // 0x1A03
    MODULE_ESTIMATOR_FIRST_STATEMENT_OUT_OF_RANGE                                   = 6913,  // 0x1B01
    MODULE_ESTIMATOR_ERROR_READING_SELECTOR_DATA                                    = 6914,  // 0x1B02
    MODULE_ESTIMATOR_CANCELLED_DUE_TO_TOO_MUCH_TIME                                 = 6915,  // 0x1B03
    MODULE_ESTIMATOR_CANCELLED_DUE_TO_PROBABLE_INFINITE_LOOP                        = 6916,  // 0x1B04
    MODULE_EXTERNAL_MOTOR_PILOT_INVALID_MOTOR_REQUEST                               = 7169,  // 0x1C01
    MODULE_EXTERNAL_MOTOR_PILOT_DETECTED_DISCONNECTED_MOTOR_NODE                    = 7170,  // 0x1C02
    MODULE_FAN_MOTOR_FAULT_MONITOR_ERROR_MISSING_COMPONENTS                         = 7425,  // 0x1D01
    MODULE_FAN_MOTOR_FAULT_MONITOR_ERROR_MISSING_PARAMETERS                         = 7426,  // 0x1D02
    MODULE_FAULT_IGNORED_DEFINED_FAULT                                              = 7680,  // 0x1E00
    MODULE_FAULT_REQUESTED_MISSING_FAULT_INFO                                       = 7681,  // 0x1E01
    MODULE_FAULT_SF_MISSING_SPECIFIC_DATA_FAULT_DATA                                = 7682,  // 0x1E02
    MODULE_FAULT_MISSING_SORTED_FAULT_LIST                                          = 7683,  // 0x1E03
    MODULE_FILL_PROCESS_CONTROL_IS_NULL                                             = 7937,  // 0x1F01
    MODULE_FILL_PROCESS_CONTROL_PARAMS_EXCEED_LIMITS                                = 7938,  // 0x1F02
    MODULE_FILL_ERROR_INCONSISTENT_AVERAGE_DATA                                     = 7939,  // 0x1F03
    MODULE_FILL_VALVE_FAULT_MONITOR_ERROR_LOADING_PARAMETERS                        = 8193,  // 0x2001
    MODULE_FILTERS_UNSUPPORTED_FILTER_TYPE                                          = 8449,  // 0x2101
    MODULE_FXLS8974_I2C_APPEARS_STUCK                                               = 8705,  // 0x2201
    MODULE_FXLS8974_ACCEL_NOT_RESPONDING                                            = 8706,  // 0x2202
    MODULE_FXLS8974_NO_DATA_TIMEOUT                                                 = 8707,  // 0x2203
    MODULE_FXLS8974_TOO_MUCH_DATA                                                   = 8708,  // 0x2204
    MODULE_FXLS8974_DETECTED_OVERRUN                                                = 8709,  // 0x2205
    MODULE_FXLS8974_INVALID_AXIS_MAPPING                                            = 8710,  // 0x2206
    MODULE_FXLS8974_TRACE_STATE                                                     = 8832,  // 0x2280
    MODULE_HBL_INVALID_GI_HANDLER_INDEX                                             = 8960,  // 0x2300
    MODULE_HBL_INVALID_LLI_HANDLER_INDEX                                            = 8961,  // 0x2301
    MODULE_HBL_INVALID_LOAD_HANDLER_INDEX                                           = 8962,  // 0x2302
    MODULE_HBL_INVALID_PILOT_HANDLER_INDEX                                          = 8963,  // 0x2303
    MODULE_HBL_GI_ALLOCATION_FAILED                                                 = 8964,  // 0x2304
    MODULE_HBL_GI_INSTANCE_INITIALIZATION_FAILED                                    = 8965,  // 0x2305
    MODULE_HBL_LLI_ALLOCATION_FAILED                                                = 8966,  // 0x2306
    MODULE_HBL_LLI_INSTANCE_INITIALIZATION_FAILED                                   = 8967,  // 0x2307
    MODULE_HBL_LLI_INPUTCAPTURE_ALLOCATION_FAILED                                   = 8968,  // 0x2308
    MODULE_HBL_LOAD_ALLOCATION_FAILED                                               = 8969,  // 0x2309
    MODULE_HBL_LOAD_INSTANCE_INITIALIZATION_FAILED                                  = 8970,  // 0x230A
    MODULE_HBL_LOAD_OUTPUTCOMPARE_ALLOCATION_FAILED                                 = 8971,  // 0x230B
    MODULE_HBL_NUM_OF_SETTING_GIS_EXCEEDEDS_HBL_MAX_NUM_GENERICINPUT                = 8972,  // 0x230C
    MODULE_HBL_NUM_OF_SETTING_LLIS_EXCEEDEDS_HBL_MAX_NUM_LOWLEVELINPUT              = 8973,  // 0x230D
    MODULE_HBL_NUM_OF_SETTING_LOADS_EXCEEDEDS_HBL_MAX_NUM_LOADS                     = 8974,  // 0x230E
    MODULE_HBL_PILOT_ALLOCATION_FAILED                                              = 8975,  // 0x230F
    MODULE_HBL_PILOT_INSTANCE_INITIALIZATION_FAILED                                 = 8976,  // 0x2310
    MODULE_HBL_LOAD_DIRECT_HANDLER_REMAP                                            = 8977,  // 0x2311
    MODULE_HCD_MONITOR_ERROR_LOADING_PARAMETERS                                     = 9217,  // 0x2401
    MODULE_HCD_MONITOR_ERROR_MISSING_PARAMETERS                                     = 9218,  // 0x2402
    MODULE_HEAT_ERROR_PROCESS_CONTROL                                               = 9473,  // 0x2501
    MODULE_HEAT_ERROR_LOADING_PARAMETERS                                            = 9474,  // 0x2502
    MODULE_HEAT_MONITOR_ERROR_LOADING_PARAMETERS                                    = 9729,  // 0x2601
    MODULE_HEATER_FAULT_MONITOR_ERROR_MISSING_COMPONENTS                            = 9985,  // 0x2701
    MODULE_HEATER_FAULT_MONITOR_ERROR_MISSING_PARAMETERS                            = 9986,  // 0x2702
    MODULE_HEATER_SHADOWING_ERROR_MISSING_COMPONENTS                                = 10241, // 0x2801
    MODULE_HEATER_SHADOWING_ERROR_MISSING_PARAMETERS                                = 10242, // 0x2802
    MODULE_HOTHIGHPOT_MONITOR_ERROR_LOADING_PARAMETERS                              = 10497, // 0x2901
    MODULE_I2C_INFRARED_LLI_ERROR_COMMUNICATION                                     = 10753, // 0x2A01
    MODULE_I2C_INFRARED_LLI_ERROR_DATA_STUCK                                        = 10754, // 0x2A02
    MODULE_I2C_RHT_SENSOR_LLI_ERROR_COMMUNICATION                                   = 11009, // 0x2B01
    MODULE_I2C_RHT_SENSOR_LLI_ERROR_DATA_STUCK                                      = 11010, // 0x2B02
    MODULE_I2CPRESSURESENSOR_ERROR_COMMUNICATION                                    = 11265, // 0x2C01
    MODULE_I2CPRESSURESENSOR_ERROR_DATA_STUCK                                       = 11266, // 0x2C02
    MODULE_INTERPRETER_COMPARTMENT_OUT_OF_RANGE                                     = 11521, // 0x2D01
    MODULE_INTERPRETER_PROCESS_FUNCTION_NULL                                        = 11522, // 0x2D02
    MODULE_INTERPRETER_NOP_PROCESS                                                  = 11523, // 0x2D03
    MODULE_INTERPRETER_FIRST_STATEMENT_OUT_OF_RANGE                                 = 11524, // 0x2D04
    MODULE_INTERPRETER_NULL_POINTER                                                 = 11525, // 0x2D05
    MODULE_INTERPRETER_INVALID_SET_STATE_FLAG                                       = 11526, // 0x2D06
    MODULE_INTERPRETER_INVALID_END_SUB_CYCLE                                        = 11527, // 0x2D07
    MODULE_INTERPRETER_INVALID_COMPLETION_CODE                                      = 11528, // 0x2D08
    MODULE_INTERPRETER_SF_PTR_SUB_CYCLE_FAIL                                        = 11529, // 0x2D09
    MODULE_INTERPRETER_JUMP_OCCURRED                                                = 11530, // 0x2D0A
    MODULE_INTERPRETER_STARTED_NEW_STEP                                             = 11531, // 0x2D0B
    MODULE_INTERPRETER_STARTED_NEW_PHASE                                            = 11532, // 0x2D0C
    MODULE_INTERPRETER_JUMP_TO_CYCLE_OCCURRED                                       = 11533, // 0x2D0D
    MODULE_INTERPRETER_SUB_CYCLE_COMPLETION_INVALID_PHASE                           = 11534, // 0x2D0E
    MODULE_JUMPIF_INVALID_PROCESS_CONTROL                                           = 11777, // 0x2E01
    MODULE_JUMPIF_INVALID_PROCESS_PARAMETERS                                        = 11778, // 0x2E02
    MODULE_JUMPIF_NO_TARGET_AVAILABLE                                               = 11779, // 0x2E03
    MODULE_JUMPIF_TARGET_STATEMENT_OUT_OF_RANGE                                     = 11780, // 0x2E04
    MODULE_JUMPIF_TARGET_PHASE_OUT_OF_RANGE                                         = 11781, // 0x2E05
    MODULE_JUMPIF_TARGET_CYCLE_OUT_OF_RANGE                                         = 11782, // 0x2E06
    MODULE_JUMPIF_GESE_SHOULD_USE_NEG_128                                           = 11783, // 0x2E07
    MODULE_JUMPIF_CPS_NULL_REFERENCE                                                = 11784, // 0x2E08
    MODULE_LISxDH_I2C_APPEARS_STUCK                                                 = 12033, // 0x2F01
    MODULE_LISxDH_ACCEL_NOT_RESPONDING                                              = 12034, // 0x2F02
    MODULE_LISxDH_NO_DATA_TIMEOUT                                                   = 12035, // 0x2F03
    MODULE_LISxDH_TOO_MUCH_DATA                                                     = 12036, // 0x2F04
    MODULE_LISxDH_DETECTED_OVERRUN                                                  = 12037, // 0x2F05
    MODULE_LISxDH_INVALID_AXIS_MAPPING                                              = 12038, // 0x2F06
    MODULE_LISxDH_TRACE_STATE                                                       = 12160, // 0x2F80
    MODULE_LITT_FAULT_MONITOR_ERROR_MISSING_COMPONENTS                              = 12289, // 0x3001
    MODULE_LITT_MONITOR_ERROR_LOADING_PARAMETERS                                    = 12545, // 0x3101
    MODULE_LOADS_CONTROL_PROCESS_CONTROL_IS_NULL                                    = 12801, // 0x3201
    MODULE_LOADS_CONTROL_PROCESS_CONTROL_PARAMS_EXCEED_LIMITS                       = 12802, // 0x3202
    MODULE_LOADS_CONTROL_SF_BAD_DISPLACEMENT                                        = 12803, // 0x3203
    MODULE_LOADS_CONTROL_SF_BAD_LOADS_DEFINITION                                    = 12804, // 0x3204
    MODULE_LOADS_CONTROL_SF_PULSE_TIMES_MISSING                                     = 12805, // 0x3205
    MODULE_LOADS_CONTROL_SF_BAD_ANALOG_CONFIG_DEFINITION                            = 12806, // 0x3206
    MODULE_LOADS_CONTROL_TRACE_STATE                                                = 12928, // 0x3280
    MODULE_MAGNETRON_PILOT_INVALID_REQUEST_VALUE                                    = 13057, // 0x3301
    MODULE_MAGNETRON_PILOT_INVALID_PILOT_PARAMETER_VERSION                          = 13058, // 0x3302
    MODULE_MAGNETRON_PILOT_REQUEST_REJECT_DUE_TO_TURBO_FEATURE_DISABLE              = 13059, // 0x3303
    MODULE_MAGNETRON_PILOT_NOT_ALLOW_TO_CHANGE_STATE_FROM_ON_TO_TURBO               = 13060, // 0x3304
    MODULE_MAGNETRON_PILOT_DOOR_GI_IS_NOT_CONFIGURED_CORRECTLY                      = 13061, // 0x3305
    MODULE_MAGNETRON_PILOT_INVALID_PILOT_PARAMETER_LENGTH                           = 13062, // 0x3306
    MODULE_MAGNETRON_PILOT_MAINS_SAMPLER_MAINS_SIGNAL_FOUND                         = 13313, // 0x3401
    MODULE_MAGNETRON_PILOT_MAINS_SAMPLER_MAINS_SIGNAL_EARLY_WARNING                 = 13314, // 0x3402
    MODULE_MAGNETRON_PILOT_MAINS_SAMPLER_MAINS_SIGNAL_LOST                          = 13315, // 0x3403
    MODULE_MAGNETRON_PILOT_RELAY_SYNC_MW_RELAY_ON_DELAY_EXCEED_LIMITATION           = 13569, // 0x3501
    MODULE_MAGNETRON_PILOT_RELAY_SYNC_MW_RELAY_OFF_DELAY_EXCEED_LIMITATION          = 13570, // 0x3502
    MODULE_MAGNETRON_PILOT_RELAY_SYNC_MW_RELAY_ERROR_PRESENT                        = 13571, // 0x3503
    MODULE_MAGNETRON_PILOT_RELAY_SYNC_MW_RELAY_ERROR_CANCEL                         = 13572, // 0x3504
    MODULE_MAINTAIN_INVALID_PROCESS_CONTROL                                         = 13825, // 0x3601
    MODULE_MAINTAIN_INVALID_PROCESS_PARAMETERS                                      = 13826, // 0x3602
    MODULE_MAINTAIN_INVALID_NON_VOLATILE_VALUES_RECOVERED                           = 13827, // 0x3603
    MODULE_MCI_MCI_EVENT_GWS_INITIALIZED                                            = 14081, // 0x3701
    MODULE_MCI_MCI_EVENT_RIF_INITIALIZED                                            = 14082, // 0x3702
    MODULE_MCI_MCI_EVENT_FAULT_CLEARED                                              = 14085, // 0x3705
    MODULE_MCI_MCI_EXCEPTION_PARAMETERS_NOT_LOADED                                  = 14090, // 0x370A
    MODULE_MODIFIER_CANNOT_LOAD_PARAMETERS                                          = 14337, // 0x3801
    MODULE_MODIFIER_INVALID_OVERALL_OPERATION                                       = 14338, // 0x3802
    MODULE_MODIFIER_INVALID_DTV_OPTION                                              = 14339, // 0x3803
    MODULE_MODIFIER_CONVERT_STATUS_FAILED                                           = 14340, // 0x3804
    MODULE_MODIFIER_MISSING_TABLE_DATA                                              = 14341, // 0x3805
    MODULE_MODIFIER_NO_32BIT_TABLE_SUPPORT_AVAILABLE                                = 14342, // 0x3806
    MODULE_MODIFIER_CANNOT_GET_VARIABLE_VALUE                                       = 14343, // 0x3807
    MODULE_MODIFIER_INVALID_ACTION                                                  = 14344, // 0x3808
    MODULE_MODIFIER_DATATYPE_NOT_SUPPORTED                                          = 14345, // 0x3809
    MODULE_MOISTURE_PREVENTION_ERROR_LOADING_PARAMETERS                             = 14593, // 0x3901
    MODULE_MOTOR_PILOT_MOTOR_COMMAND_TOO_BIG                                        = 14849, // 0x3A01
    MODULE_NORMAL_FILL_ERROR_LOADING_PARAMETERS                                     = 15105, // 0x3B01
    MODULE_NVDATA_INSUFFICIENT_APPLIENCE_OR_STATISTICS_NONVOLATILE_MEMORY_ALLOCATED = 15360, // 0x3C00
    MODULE_NVDATA_INSUFFICIENT_STATIC_NONVOLATILE_MEMORY_ALLOCATED                  = 15361, // 0x3C01
    MODULE_OWI_PROCESS_CONTROL_IS_NULL                                              = 15617, // 0x3D01
    MODULE_OWI_PROCESS_CONTROL_PARAMS_EXCEED_LIMITS                                 = 15618, // 0x3D02
    MODULE_OWI_MONITOR_ERROR_PROCESS_CONTROL                                        = 15873, // 0x3E01
    MODULE_OWI_MONITOR_ERROR_LOADING_PARAMETERS                                     = 15874, // 0x3E02
    MODULE_OWI_ONE_SHOT_ERROR_PROCESS_CONTROL                                       = 16129, // 0x3F01
    MODULE_OWI_ONE_SHOT_ERROR_LOADING_PARAMETERS                                    = 16130, // 0x3F02
    MODULE_OWI_VRO_READ_INVALID_PROCESS_CONTROL                                     = 16385, // 0x4001
    MODULE_OWI_VRO_READ_ERROR_MISSING_PROCESS_PARAMETERS                            = 16386, // 0x4002
    MODULE_OWI_VRO_READ_ERROR_MISSING_MACHINE_CONFIG_PARAMETERS                     = 16387, // 0x4003
    MODULE_OWI_VRO_READ_ERROR_INCONSISTENT_PARAMETER_VALUE                          = 16388, // 0x4004
    MODULE_OWI_VRO_READ_ERROR_INCONSISTENT_AVERAGE_DATA                             = 16389, // 0x4005
    MODULE_PARTIALDRAIN_PARTIALDRAIN_ERROR_PROCESS_CONTROL                          = 16641, // 0x4101
    MODULE_PARTIALDRAIN_PARTIALDRAIN_ERROR_PROCESS_PARAMETERS                       = 16642, // 0x4102
    MODULE_PARTIALDRAIN_PARTIALDRAIN_ERROR_MOTOR_INVALID                            = 16643, // 0x4103
    MODULE_PILOT_RELAY_FAULT_MONITOR_ERROR_MISSING_COMPONENTS                       = 16897, // 0x4201
    MODULE_PLUGGING_MONITOR_ERROR_LOADING_PARAMETERS                                = 17153, // 0x4301
    MODULE_PRODUCTINFO_INVALID_PROJECT_DATA                                         = 17408, // 0x4400
    MODULE_PRODUCTINFO_INVALID_BOARD                                                = 17409, // 0x4401
    MODULE_PRODUCTINFO_INVALID_PROJECT                                              = 17410, // 0x4402
    MODULE_PUMPCONTROL_ERROR_PROCESS_CONTROL                                        = 17665, // 0x4501
    MODULE_PUMPCONTROL_ERROR_LOADING_PARAMETERS                                     = 17666, // 0x4502
    MODULE_PUMPCONTROL_ERROR_WAIT_STOPPED                                           = 17667, // 0x4503
    MODULE_PUMPCONTROL_ERROR_WAIT_STOPPED_MAX_ATTEMPS                               = 17668, // 0x4504
    MODULE_PUMPCONTROL_ERROR_WAIT_STATUS                                            = 17669, // 0x4505
    MODULE_PUMPCONTROL_ERROR_WAIT_STATUS_MAX_ATTEMPS                                = 17670, // 0x4506
    MODULE_PUMPCONTROL_ERROR_MCU_RESET                                              = 17671, // 0x4507
    MODULE_REGENERATE_INVALID_PROCESS_CONTROL                                       = 17921, // 0x4601
    MODULE_REGENERATE_ERROR_MISSING_PROCESS_PARAMETERS                              = 17922, // 0x4602
    MODULE_REGENERATE_ERROR_MISSING_WATER_SOFTENER_PARAMETERS                       = 17923, // 0x4603
    MODULE_REGULATIONS_GI_POSITION_AND_TYPE_DOES_NOT_EXIST                          = 18176, // 0x4700
    MODULE_REGULATIONS_POSITION_OUT_OF_RANGE                                        = 18177, // 0x4701
    MODULE_REGULATIONS_TOO_MANY_REGULATIONS_DEFINED                                 = 18178, // 0x4702
    MODULE_REGULATIONS_INDEX_OUT_OF_RANGE                                           = 18179, // 0x4703
    MODULE_REGULATIONS_CONFIG_NOT_FOUND                                             = 18180, // 0x4704
    MODULE_RINSE_AID_ERROR_LOADING_PARAMETERS                                       = 18433, // 0x4801
    MODULE_RINSE_AID_DISPENSE_PROCESS_CONTROL_IS_NULL                               = 18689, // 0x4901
    MODULE_RINSE_AID_DISPENSE_PROCESS_CONTROL_PARAMS_EXCEED_LIMITS                  = 18690, // 0x4902
    MODULE_RINSE_AID_MONITOR_ERROR_LOADING_PARAMETERS                               = 18945, // 0x4A01
    MODULE_SALT_SENSING_MONITOR_ERROR_MISSING_PARAMETERS                            = 19201, // 0x4B01
    MODULE_SANITIZATION_ERROR_LOADING_PARAMETERS                                    = 19457, // 0x4C01
    MODULE_SELECTOR_NUMBER_OF_CYCLES_OUT_OF_RANGE                                   = 19713, // 0x4D01
    MODULE_SELECTOR_SF_PTR_CROSS_PRODUCT_CONFIG_FAIL                                = 19714, // 0x4D02
    MODULE_SELECTOR_SELECTOR_DATA_OVERFLOW                                          = 19715, // 0x4D03
    MODULE_SELECTOR_CYCLE_OUT_OF_RANGE                                              = 19716, // 0x4D04
    MODULE_SELECTOR_PHASE_OUT_OF_RANGE                                              = 19717, // 0x4D05
    MODULE_SELECTOR_SF_PHASE_DATA                                                   = 19719, // 0x4D07
    MODULE_SELECTOR_SF_HI_STATEMENTS                                                = 19720, // 0x4D08
    MODULE_SELECTOR_SF_LO_STATEMENTS                                                = 19721, // 0x4D09
    MODULE_SELECTOR_SF_PROGRAM_HEADING_TARGET_FAIL                                  = 19722, // 0x4D0A
    MODULE_SELECTOR_SF_PROGRAM_HEADING_ITEM_SIZE                                    = 19723, // 0x4D0B
    MODULE_SELECTOR_CYCLE_NOT_FOUND                                                 = 19724, // 0x4D0C
    MODULE_SENSEDDRAIN_SENSEDDRAIN_ERROR_PROCESS_CONTROL                            = 19969, // 0x4E01
    MODULE_SENSEDDRAIN_SENSEDDRAIN_ERROR_PROCESS_PARAMETERS                         = 19970, // 0x4E02
    MODULE_SENSEDDRAIN_SENSEDDRAIN_ERROR_MOTOR_INVALID                              = 19971, // 0x4E03
    MODULE_SENSEDDRAIN_SENSEDDRAIN_ERROR_PARTIALDRAIN_PARAMETERS                    = 19972, // 0x4E04
    MODULE_SETLOADQUEUE_QUEUE_DATA_SIZE_TOO_SMALL                                   = 20224, // 0x4F00
    MODULE_SETTINGFILE_UNHANDLED_SECTION_ENDIANNESS                                 = 20480, // 0x5000
    MODULE_SETTINGFILE_UNHANDLED_HARDCODED_ENDIANNESS                               = 20481, // 0x5001
    MODULE_SETVARIABLE_INVALID_PROCESS_CONTROL                                      = 20736, // 0x5100
    MODULE_SETVARIABLE_INVALID_PROCESS_PARAMETERS                                   = 20737, // 0x5101
    MODULE_SETVARIABLE_INVALID_PROCESS_OPTION                                       = 20738, // 0x5102
    MODULE_SETVARIABLE_INVALID_SETTINGFILE_PARAMETERS                               = 20739, // 0x5103
    MODULE_SETVARIABLE_INVALID_VARIABLE                                             = 20740, // 0x5104
    MODULE_SFVERSIONCHECK_INVALID_SETTING_FILE                                      = 20993, // 0x5201
    MODULE_SFVERSIONCHECK_SETTING_FILE_VERSION_OLD_FW_MAJOR_MINOR                   = 20994, // 0x5202
    MODULE_SFVERSIONCHECK_SETTING_FILE_VERSION_OLD_FW_BUILD_REVISION                = 20995, // 0x5203
    MODULE_SREXCEPTION_EVENT_TYPE                                                   = 21248, // 0x5300
    MODULE_SREXCEPTION_EVENT_DATA1                                                  = 21249, // 0x5301
    MODULE_SREXCEPTION_EVENT_DATA2                                                  = 21250, // 0x5302
    MODULE_SSDRAIN_MOTOR_FAULT_MONITOR_ERROR_MISSING_COMPONENTS                     = 21505, // 0x5401
    MODULE_SSDRAIN_MOTOR_FAULT_MONITOR_ERROR_MISSING_PARAMETERS                     = 21506, // 0x5402
    MODULE_SSDRAIN_MOTOR_FAULT_MONITOR_ERROR_VENT_DELAY_OUT_OF_RANGE                = 21507, // 0x5403
    MODULE_STABILITY_CONTROL_ERROR_LOADING_PARAMETERS                               = 21761, // 0x5501
    MODULE_STATISTICS_REQUESTED_CYCLE_OUT_OF_RANGE                                  = 22017, // 0x5601
    MODULE_STATISTICS_REQUESTED_FAULT_OUT_OF_RANGE                                  = 22018, // 0x5602
    MODULE_STATISTICS_REQUESTED_FAULT_BLACK_BOX_OUT_OF_RANGE                        = 22019, // 0x5603
    MODULE_STATISTICS_REQUESTED_LAST_CYCLE_OUT_OF_RANGE                             = 22020, // 0x5604
    MODULE_STATISTICS_REQUESTED_PRE_FAULT_OUT_OF_RANGE                              = 22021, // 0x5605
    MODULE_STATISTICS_REQUESTED_PRE_FAULT_BLACK_BOX_OUT_OF_RANGE                    = 22022, // 0x5606
    MODULE_STATISTICS_DATA_LOST_DUE_TO_CORRUPTION                                   = 22023, // 0x5607
    MODULE_STATUS_STATUS_LOG_BAD_SETTINGFILE                                        = 22272, // 0x5700
    MODULE_STATUS_RECEIVED_INCORRECT_STATUS_MESSAGE_LENGTH                          = 22273, // 0x5701
    MODULE_STATUS_VARIABLE_MISSING                                                  = 22274, // 0x5702
    MODULE_STATUS_INVALID_DATA_TYPE                                                 = 22275, // 0x5703
    MODULE_STATUSLED_INVALID_VIRTUAL_PIN_INDEX                                      = 22529, // 0x5801
    MODULE_STATUSLED_INVALID_PROFILE                                                = 22530, // 0x5802
    MODULE_SUBCYCLE_ERROR_LOADING_SUB_CYCLE_DEFINITIONS                             = 22785, // 0x5901
    MODULE_SUBCYCLE_UNEXPECTED_SUB_CYCLE_FORMAT                                     = 22786, // 0x5902
    MODULE_SUPERVISOR_ATTEMPT_TO_PAUSE_CYCLE_NOT_ALLOWED                            = 23041, // 0x5A01
    MODULE_SUPERVISOR_AUTO_START_CYCLE_ENDED_RESTARTED                              = 23042, // 0x5A02
    MODULE_SUPERVISOR_COMPARTMENT_OUT_OF_RANGE                                      = 23043, // 0x5A03
    MODULE_SUPERVISOR_INVALID_CYCLE_STATE_ON_COLD_RESET                             = 23044, // 0x5A04
    MODULE_SUPERVISOR_MAX_INTERPRETER_STEPS                                         = 23045, // 0x5A05
    MODULE_SUPERVISOR_NOT_SUBMODE_NORMAL_SUPERVISOR                                 = 23046, // 0x5A06
    MODULE_SUPERVISOR_SELECTOR_CLEARED_BEFORE_EXTERNAL_START                        = 23047, // 0x5A07
    MODULE_SYSTEMTIMERS_INVALID_HANDLE                                              = 23297, // 0x5B01
    MODULE_TANK_DRAIN_ERROR_LOADING_PARAMETERS                                      = 23553, // 0x5C01
    MODULE_TANK_FILL_ERROR_LOADING_PARAMETERS                                       = 23809, // 0x5D01
    MODULE_THERMISTOR_FAULT_MONITOR_ERROR_LOADING_PARAMETERS                        = 24065, // 0x5E01
    MODULE_TRIAC_SHORTED_DRAIN_FAULT_MONITOR_ERROR_MISSING_COMPONENTS               = 24321, // 0x5F01
    MODULE_TRIAC_SHORTED_DRAIN_FAULT_MONITOR_ERROR_MISSING_PARAMETERS               = 24322, // 0x5F02
    MODULE_TRIACMCI_INVALID_VIRTUAL_PIN                                             = 24576, // 0x6000
    MODULE_TRIDGE_DETERGENT_ERROR_LOADING_PARAMETERS                                = 24833, // 0x6101
    MODULE_VARIABLE_BOUNDARY_CHECK_INDEX                                            = 25088, // 0x6200
    MODULE_VARIABLE_BOUNDARY_CHECK_OFFSET                                           = 25089, // 0x6201
    MODULE_VARIABLE_BOUNDARY_1_CHECK_INDEX                                          = 25090, // 0x6202
    MODULE_VARIABLE_INVALID_INDEX                                                   = 25091, // 0x6203
    MODULE_VARIABLE_INVALID_SETVARIABLE                                             = 25092, // 0x6204
    MODULE_VARIABLE_REGULATION_ERROR                                                = 25093, // 0x6205
    MODULE_VARIABLE_DATATYPE_ERROR                                                  = 25094, // 0x6206
    MODULE_VARIABLE_POINTER_1_NULL                                                  = 25095, // 0x6207
    MODULE_VARIABLE_POINTER_2_NULL                                                  = 25096, // 0x6208
    MODULE_VARIABLE_BOUNDARY_CHECK_FOR_ESTIMATE                                     = 25097, // 0x6209
    MODULE_VARIABLE_OFFSET_CHECK_FOR_ESTIMATE                                       = 25098, // 0x620A
    MODULE_VARIABLE_NULL_POINTER_FOR_ESTIMATE                                       = 25099, // 0x620B
    MODULE_VARIABLE_NULL_POINTERS_ON_KVP                                            = 25100, // 0x620C
    MODULE_VARIABLE_NOT_WRITABLE                                                    = 25101, // 0x620D
    MODULE_VARIABLE_ENCOUNTER_DIVIDE_BY_ZERO                                        = 25102, // 0x620E
    MODULE_VARIABLE_INVALID_MATH_OPERATION                                          = 25103, // 0x620F
    MODULE_VARIABLE_VARIABLES_FEATURE_DISABLED                                      = 25104, // 0x6210
    MODULE_VOLTAGE_RANGE_VOLTAGERANGE_STATUS_CHANGED                                = 25345, // 0x6301
    MODULE_VOLTAGE_RANGE_VOLTAGERANGE_INSTABILITY_COUNTER                           = 25346, // 0x6302
    MODULE_VSDRAIN_MOTOR_FAULT_MONITOR_ERROR_MISSING_PARAMETERS                     = 25601, // 0x6401
    MODULE_VSDRAIN_MOTOR_FAULT_MONITOR_ERROR_NO_VSM_DRAIN_MOTOR                     = 25602, // 0x6402
    MODULE_VWI_ERROR_LOADING_PARAMETERS                                             = 25856, // 0x6500
    MODULE_VWI_MONITOR_ERROR_LOADING_VWI_PARAMETERS                                 = 26113, // 0x6601
    MODULE_WASH_MOTOR_FAULT_MONITOR_ERROR_MISSING_PARAMETERS                        = 26369, // 0x6701
    MODULE_ZEROCROSS_INVALID_ZEROCROSS_RANGE                                        = 26625, // 0x6801
    MODULE_ZEROCROSS_DEPRECATED_API_ZEROCROSS_EVENTS                                = 26626, // 0x6802
    MODULE_ZEROCROSS_DEPRECATED_API_ATOD_SERVICE                                    = 26627, // 0x6803

    //! This is a purposely invalid Module/Message ID used to identify invalid Log Messages.
    MODULE_LOG_INVALID_MESSAGE                                                      = 65535
} LOG_MODULE_MESSAGE_ID_TYPE;



#endif  // LOG_PRM_H_


