/*
 * DCLoadCurrentMonitor.h
 *
 *  Created on: Jul 7, 2023
 *      Author: WAZNYP
 */

#ifndef SOURCE_CATEGORY_APPLICATION_ACU_MONITORS_DCLOADCURRENTMONITOR_DCLOADCURRENTMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_ACU_MONITORS_DCLOADCURRENTMONITOR_DCLOADCURRENTMONITOR_H_

#include "Hbl_prv.h"
#define DCLOAD_DIAG_DEBUG                                        DISABLED

#if (DCLOAD_DIAG_DEBUG == ENABLED)
    #warning !!DCLOAD_DIAG_DEBUG ENABLED!!
#endif

//! Triacs diagnostic current sensor output type definition.
typedef enum
{
    DC_LOAD_DIAGNOSTIC_UNKNOWN,                                       //!< unknown state, output sense is debouncing
    DC_LOAD_DIAGNOSTIC_NOT_IN_RANGE,                                  //!< load current is not in range
    DC_LOAD_DIAGNOSTIC_IN_RANGE                                       //!< load current is in range
} DC_LOAD_DIAGNOSTIC_STATUS_TYPE;


typedef struct
{
    uint16                          Max_Expected_Value;             //!< Maximal input value change for specific output change
    uint16                          Min_Expected_Value;             //!< Minimal input value change for specific output change
    DC_LOAD_DIAGNOSTIC_STATUS_TYPE  Temporary_Load_Sense_Status;    //!< Temporary status of Triacs Diagnostic module
    DC_LOAD_DIAGNOSTIC_STATUS_TYPE  Load_Sense_Status;              //!< Variable contains status of output sense (unknown, in range or not in range)
    BOOL_TYPE                       DCLoad_Channel_Freeze;           //!< TRUE output value change not allow (disable measurement)
    HBL_LOAD_ENUM_TYPE              Last_Changed_DC_Load_Output;    //!< Holds lately changed output
}DC_LOAD_DIAGNOSTIC_TYPE;

typedef PACKED struct LITT_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_LITT_Min;
   uint16 Current_Sense_LITT_Max;
} LITT_FAULT_PARAMS_TYPE;

typedef PACKED struct DC_FAN_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_DC_Fan_Min;
   uint16 Current_Sense_DC_Fan_Max;
} DC_FAN_FAULT_PARAMS_TYPE;

typedef PACKED struct DOS_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_DOS_Min;
   uint16 Current_Sense_DOS_Max;
} DOS_FAULT_PARAMS_TYPE;


void DCLoadCurrentMonitor__Initialize(void);
void DCLoadCurrentMonitor__Execution(void);
void DCLoadCurrentMonitor__ExitFault(void);


#endif /* SOURCE_CATEGORY_APPLICATION_ACU_MONITORS_DCLOADCURRENTMONITOR_DCLOADCURRENTMONITOR_H_ */
