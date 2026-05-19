/*
 * StatisticsDish.h
 *
 *  Created on: May 7, 2018
 *      Author: DHAMAAD
 */

#ifndef SOURCE_CATEGORY_COMMON_COMMONDISH_STATISTICSDISH_STATISTICSDISH_H_
#define SOURCE_CATEGORY_COMMON_COMMONDISH_STATISTICSDISH_STATISTICSDISH_H_

//=====================================================================================================================
//-------------------------------------------------- Include files ----------------------------------------------------
//=====================================================================================================================

#include "JumpIf.h"
#include "DishAuxVar.h"
#include "StatisticsCommon.h"
#include "Statistics_prm.h"
#include "VoltageRangeMonitor.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

#define MAX_DELAY_SLOTS	12

#define SAVED_OPTIONS_NUM	15

typedef union STATISTICS_FLAGS_UNION
{
	uint8 All;

	struct {
		uint8 Stability_Control_Loop_Reaction_Triggered	:1;
		uint8 Phase_Interrupted							:1;
		uint8 Water_Add_Executed						:1;
		uint8 RPM_Reduceed								:1;
		uint8 Heater_Limescaled_Detected				:1;
		uint8 Unused_Bit_5_7							:3;
	} Bits;
} STATISTICS_FLAGS_TYPE;

typedef PACKED struct PLATFORM_LIFETIME_STRUCT
{
	uint16 Door_Opened_Count;
	uint16 Demo_Duration_Mins;
	uint16 Service_Entered_Cycle_Odometer;
	uint16 Service_Fault_Clear_Odometer;
	uint16 OWI_Calibration_Drive_Duty_Cycle;
	uint16 Selected_Delay_Count[MAX_DELAY_SLOTS];
	uint16 Primary_Reaction_Counter;
	uint16 Secondary_Reaction_Counter;
	uint16 Total_Cycles_Without_Salt;
	uint16 Cycles_Since_Salt_Empty;
	uint16 Num_Phases_Interrupted_by_Water_Indicator;
	uint16 Total_Water_Add_Executions;
	uint16 Total_RPM_Reductions;
	uint16 Total_Cycles_With_Limescale;
	uint16 Total_Cycles_In_HCD_Lockout;
	uint16 OWI_Current_Status_Odometer;
	OWI_STATUS_TYPE OWI_Status;
	uint16 Options_Count[SAVED_OPTIONS_NUM];
	uint16 Low_Voltage_Event_Counter;
	uint16 High_Voltage_Event_Counter;
	VOLTAGERANGE_STATE_TYPE Voltage_Range_Status;
}PLATFORM_LIFETIME_TYPE;

typedef PACKED struct PLATFORM_CYCLE_STRUCT
{
	uint16 Cycles_Since_Salt_Empty;
	uint16 Soil_Measure_Recent;
	uint16 Water_Consumption_Cl;
	sint16 Water_Temperature_First_Fill;
	sint16 Last_Heat_Final_Temperature;
	uint8 Last_Heat_Duration_Mins;
	uint8 Sanitization_Result;
	uint8 RinseAid_Status;
	STATISTICS_FLAGS_TYPE Flags;
	uint8 Soil_Level_1;
	uint8 Cycle_Low_Voltage_Event_Counter;
	uint8 Cycle_High_Voltage_Event_Counter;
	VOLTAGERANGE_STATE_TYPE Cycle_Voltage_Range_Status;
}PLATFORM_CYCLE_TYPE;

typedef PACKED struct PLATFORM_BLACK_BOX_STRUCT
{
	uint8 Fault_Id;
	CYCLE_PHASE_STATEMENT_TYPE Cps;
	uint16 Age;
} PLATFORM_BLACK_BOX_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void StatisticsDish__Initialize(void);
void StatisticsDish__UpdateLifetimeData(PLATFORM_LIFETIME_TYPE *lifetime);
void StatisticsDish__UpdateCycleData(uint8 compartment, STATISTICS_CYCLE_STATE_TYPE state, PLATFORM_CYCLE_TYPE *cycle, void* last_cycle);
void StatisticsDish__UpdateFaultData(PLATFORM_BLACK_BOX_TYPE *black_box, uint8 fault_id);
void StatisticsDish__UpdatePreFaultData(PLATFORM_BLACK_BOX_TYPE *black_box, uint8 fault_id);
void StatisticsDish__SetSCLReactionTriggered(void);
void StatisticsDish__IncreaseSCLPrimaryReactionCounter(void);
void StatisticsDish__IncreaseSCLSecondaryReactionCounter(void);
void StatisticsDish__IncreaseDoorOpenings(void);
void StatisticsDish__UpdateDelayRecord(void);
void StatisticsDish__IncreaseWaterCounter(uint16 water_amount);
void StatisticsDish__DecreaseWaterCounter(uint16 water_amount);
void StatisticsDish__UpdateFstFillWaterTemp(void);
void StatisticsDish__UpdateOWICalibrationDutyCycle(void);
void StatisticsDish__UpdateHeatRelatedMeasures(void);
void StatisticsDish__ResetHeatDurationCounter(void);
void StatisticsDish__UpdateHeatDurationCounter(void);
void StatisticsDish__IncreaseInterruptedPhasesNum(void);
void StatisticsDish__IncreaseWaterAddExecutions(void);
void StatisticsDish__IncreaseRPMReductions(void);
void StatisticsDish__IncreaseCyclesWithLimescale(void);
void StatisticsDish__UpdateServiceFaultClearOdometer(void);
void StatisticsDish__IncreaseLowVoltageEventCounter(void);
void StatisticsDish__IncreaseHighVoltageEventCounter(void);
void StatisticsDish__UpdateVoltageRangeStatus(VOLTAGERANGE_STATE_TYPE voltage_range_status);

#endif /* SOURCE_CATEGORY_COMMON_COMMONDISH_STATISTICSDISH_STATISTICSDISH_H_ */
