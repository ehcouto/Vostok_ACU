/*
 * FillValveStuckOn.h
 *
 *  Created on: 24.04.2018
 *      Author: WAZNYP
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_FILLVALVESTUCKON_FILLVALVESTUCKON_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_FILLVALVESTUCKON_FILLVALVESTUCKON_H_

typedef PACKED struct FILL_VALVE_FAULTS_INSTANCE_STRUCT
{
    uint8 Fill_Valve_Debounce_Time;
    uint8 Pulses_Detection_Period;
    uint8 Pulses_Number;
    uint8 Time_On;
    uint8 Max_Num_Of_Retries;
    uint8 Unused;
} FILL_VALVE_FAULTS_INSTANCE_TYPE;

void FillValveStuckOn__Initialize(void);
void FillValveStuckOn__Execution(void);
void FillValveStuckON__ExitFault(void);
BOOL_TYPE FillValveStuckOn__UnexpectedPulsesDetected(void);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_FILLVALVESTUCKON_FILLVALVESTUCKON_H_ */
