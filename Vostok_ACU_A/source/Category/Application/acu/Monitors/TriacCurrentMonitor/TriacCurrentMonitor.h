/*
 * TriacCurrentMonitor.h
 *
 *  Created on: 22.05.2018
 *      Author: WAZNYP
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_TriacCurrentMonitor_TriacCurrentMonitor_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_TriacCurrentMonitor_TriacCurrentMonitor_H_
#include "Hbl_prv.h"
#define TRIAC_DIAG_DEBUG                                        DISABLED

#if (TRIAC_DIAG_DEBUG == ENABLED)
    #warning !!TRIAC_DIAG_DEBUG ENABLED!!
#endif

//! Triacs diagnostic current sensor output type definition.
typedef enum
{
    TRIACS_DIAGNOSTIC_UNKNOWN,                                       //!< unknown state, output sense is debouncing
    TRIACS_DIAGNOSTIC_NOT_IN_RANGE,                                  //!< load current is not in range
    TRIACS_DIAGNOSTIC_IN_RANGE                                       //!< load current is in range
} TRIACS_DIAGNOSTIC_STATUS_TYPE;

//! Triacs diagnostic voltage sensor output type definition.
typedef enum
{
    TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN,                                         //!< unknown state, output sense is debouncing
    TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED,                                        //!< load current is not in range
    TRIAC_LOADS_VOLTAGE_SENSOR_CONFIRMED                                        //!< load current is in range
} TRIAC_LOADS_VOLTAGE_SENSOR_TYPE;

typedef struct
{
    unsigned short                  Max_Expected_Value;             //!< Maximal input value change for specific output change
    unsigned short                  Min_Expected_Value;             //!< Minimal input value change for specific output change
    TRIACS_DIAGNOSTIC_STATUS_TYPE   Temporary_Load_Sense_Status;    //!< Temporary status of Triacs Diagnostic module
    TRIACS_DIAGNOSTIC_STATUS_TYPE   Load_Sense_Status;              //!< Variable contains status of output sense (unknown, in range or not in range)
    BOOL_TYPE                       Triac_Channel_Freeze;           //!< TRUE output value change not allow (disable measurement)
    HBL_LOAD_ENUM_TYPE              Last_Changed_Triac_Output;      //!< Holds lately changed output
}TRIACS_DIAGNOSTIC_TYPE;

typedef PACKED struct SSDRAIN_MOTOR_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_SS_Drain_Min;
   uint16 Current_Sense_SS_Drain_Max;
} SSDRAIN_MOTOR_FAULT_PARAMS_TYPE;

typedef PACKED struct FILL_VALVE_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_Fill_Valve_Min;
   uint16 Current_Sense_Fill_Valve_Max;
} FILL_VALVE_FAULT_PARAMS_TYPE;

typedef PACKED struct DISPENSER_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_Dispenser_Min;
   uint16 Current_Sense_Dispenser_Max;
} DISPENSER_FAULT_PARAMS_TYPE;

typedef PACKED struct VENT_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_Vent_Min;
   uint16 Current_Sense_Vent_Max;
} VENT_FAULT_PARAMS_TYPE;

typedef PACKED struct REGEN_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_Regen_Min;
   uint16 Current_Sense_Regen_Max;
} REGEN_FAULT_PARAMS_TYPE;

/**
 * HBL Cross Load Config Struct
 */
typedef struct {
    uint8 On;
    uint8 Off;
} CROSSLOAD_LOAD_POSITION_TYPE;

//------- Crossload Management ---------------------------------------
typedef enum
{
    CROSSLOAD_TYPE_MASTER_RELAY,        //!< CROSSLOAD_TYPE_MASTER_RELAY
    CROSSLOAD_TYPE_SWITCH_RELAY,        //!< CROSSLOAD_TYPE_SWITCH_RELAY
    CROSSLOAD_TYPE_REVERSE_SWITCH_RELAY,//!< CROSSLOAD_TYPE_REVERSE_SWITCH_RELAY
    CROSSLOAD_TYPE_TRANSITION_RELAY,    //!< CROSSLOAD_TYPE_TRANSITION_RELAY
} CROSSLOAD_STATE_MACHINE_SELECTOR_TYPE;


/**
 * Struct to cast the parameters for the Cross Load.
 */
PACKED typedef struct
{
    uint8                                   On_Delay     :6;
    CROSSLOAD_STATE_MACHINE_SELECTOR_TYPE   Type         :2;
    uint8                                   Off_Delay    :6;
    uint8                                   Unused       :2;
    CROSSLOAD_LOAD_POSITION_TYPE            Load_Position [];
} CROSSLOAD_PARAMETERS_TYPE;

PACKED typedef struct
{
    CROSSLOAD_PARAMETERS_TYPE   *Parameter_Ptr;
    uint8                       Crossload_Position;
    uint8                       Number_Of_Couples;
} HBL_CROSS_LOAD_TYPE;





void TriacCurrentMonitor__Initialize(void);
void TriacCurrentMonitor__Execution(void);
void TriacCurrentMonitor__ExitFault(void);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_TriacCurrentMonitor_TriacCurrentMonitor_H_ */
