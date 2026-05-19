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
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_STABILITYCONTROL_STABILITYCONTROL_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_STABILITYCONTROL_STABILITYCONTROL_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Reactions triggered by the Stability Control Task.
typedef enum STABILITY_CONTROL_REACTIONS_ENUM
{
	SC_REACTION_NONE 							= 0,
	SC_REACTION_ADD_WATER						= 1,
	SC_REACTION_RESTART_PHASE					= 2,
	SC_REACTION_SKIP_PHASE						= 3,
	SC_REACTION_ADD_WATER_THEN_RESTART_PHASE 	= 4,
	SC_REACTION_ADD_WATER_THEN_SKIP_PHASE		= 5,
	SC_REACTION_RESTART_PHASE_THEN_SKIP_PHASE	= 6,
	SC_REACTION_MOTOR_RESTART_SUBCYCLE			= 7,
	SC_REACTION_FILTER_PLUGGED_SUBCYCLE			= 8,
	SC_REACTION_ERROR_DETECTED					= 9,
	SC_REACTION_REDUCE_MOTOR_SPEED				= 10,
	SC_REACTION_ADD_WATER_THEN_REDUCE_SPEED		= 11,
	SC_REACTION_DEFOAMING_SUBCYCLE				= 12,
	SC_REACTION_FILTER_PLUGGED_2_SUBCYCLE		= 13,

	SC_REACTIONS_NUM	// Max 16!!
} STABILITY_CONTROL_REACTIONS_TYPE;

//! Struct for the setting file common parameters for the Stability Control task
typedef PACKED struct STABILITY_CONTROL_COMMON_PARAMS_STRUCT
{
	uint8 No_Water_Detection_Time_x2; 			//! Time that no water condition must be detected continuously before reacting to No Water.
	uint8 Very_Low_Water_Detection_Time_x2;		//! Time that no water condition must be detected before reacting to Very Low Water
	uint8 Instability_Detection_Time_x2;		//! Time that instability condition must be detected continuously before reacting to Instability
	uint8 Water_Present_Time_x2;				//! If water is detected as present for this amount of time continuously, reset the timer monitoring for "Very Low Water"
	uint8 Max_Phase_Repetitions;				//! Maximum number of times that a cycle phase (Prewash, Main Wash, etc) can be repeated due to detected events
	uint8 Add_Water_Subcycle_Priority;			//! Priority of the SC Add Water Subcycle
	uint8 Motor_Restart_Subcycle_Priority;		//! Priority of the SC Motor Restart Subcycle
	uint8 Filter_Plugged_Subcycle_Priority;		//! Priority of the SC Filter Plugged Subcycle
	uint8 Max_Add_Water_Subcycle_Calls;			//! The maximum number of times that the SC Add Water Subcycle can be called in the same instance of the Stability Control Task.
	uint8 Max_Motor_Restart_Subcycle_Calls;		//! The maximum number of times that the SC Motor Restart Subcycle can be called in the same instance of the Stability Control Task.
	uint8 Max_Filter_Plugged_Subcycle_Calls;	//! The maximum number of times that the SC Filter Plugged Subcycle can be called in the same instance of the Stability Control Task.
	uint8 Defoaming_Subcycle_Priority;			//! Priority of the SC Defoaming Subcycle
	uint8 Filter_Plugged_2_Subcycle_Priority;	//! Priority of the SC Filter Plugged #2 Subcycle
	uint8 Max_Defoaming_Subcycle_Calls;			//! The maximum number of times that the SC Defoaming Subcycle can be called in the same instance of the Stability Control Task.
	uint8 Max_Filter_Plugged_2_Subcycle_Calls;	//! The maximum number of times that the SC Filter Plugged #2 Subcycle can be called in the same instance of the Stability Control Task.
	uint8 Timer_1_Threshold;					//! Initialization value for Timer 1 in Stability Control Loop Detection algorithm.
	uint8 Timer_2_Threshold;					//! Initialization value for Timer 2 in Stability Control Loop Detection algorithm.
	uint8 Relay_Counter_Threshold;				//! Threshold for relay transitions in Stability Control Loop Detection Algorithm.
	uint8 Primary_Reaction_Counter_Threshold;	//! Limit of how many times the Primary Reaction can occur in Stability Control Loop Detection.
	uint8 Secondary_Reaction_Counter_Threshold;	//! Limit of how many times the Secondary Reaction can occur in Stability Control Loop Detection.
} STABILITY_CONTROL_COMMON_PARAMS_TYPE;


//! Indexes in array SC_Subcycle_Execution_Counters[]
typedef enum SC_SUBCYCLE_EXECUTION_COUNTERS_ENUM
{
	ADD_WATER_INDEX			= 0,
	MOTOR_RESTART_INDEX,
	FILTER_PLUGGED_INDEX,
	DEFOAMING_INDEX,
	FILTER_PLUGGED_2_INDEX
} SC_SUBCYCLE_EXECUTION_COUNTERS_TYPE;

//! Size of array SC_Subcycle_Execution_Counters[]
#define SC_SUBC_EXEC_COUNTERS_NUM	5

//! Speed reduction states
typedef enum SPEED_REDUCTION_STATUS_ENUM
{
	SC_NO_SPEED_REDUCTION			= 0,
	SC_1ST_SPEED_REDUCTION,
	SC_2ND_SPEED_REDUCTION
} SPEED_REDUCTION_STATUS_TYPE;

//! Stability_Control_Flags bit definition
typedef enum STABILITY_CONTROL_FLAGS_ENUM
{
	SC_WATER_ADDED			= 0,
	SC_PHASE_RESTARTED,
	SC_PHASE_SKIPPED,
	SC_PUMP_RPM_REDUCED_ONCE,
	SC_PUMP_RPM_REDUCED_TWICE,
	SC_VWI_lOOP_DETECTED,
	SC_FILTER_CLEANED,
	SC_FAULT_DETECTED
} STABILITY_CONTROL_FLAGS_TYPE;

//! Mask of flags in Stability_Control_Flags to be reset at the start of a cycle
#define SC_FLAGS_START_RESET_MASK	((uint8)(1 << SC_WATER_ADDED) | \
									 (uint8)(1 << SC_PHASE_RESTARTED) | \
									 (uint8)(1 << SC_PHASE_SKIPPED) | \
									 (uint8)(1 << SC_PUMP_RPM_REDUCED_ONCE) | \
									 (uint8)(1 << SC_PUMP_RPM_REDUCED_TWICE) | \
									 (uint8)(1 << SC_VWI_lOOP_DETECTED) | \
									 (uint8)(1 << SC_FILTER_CLEANED))

//! Mask of flags in Stability_Control_Flags to be reset at the end of a cycle
#define SC_FLAGS_END_RESET_MASK		((uint8)(1 << SC_FAULT_DETECTED))

extern uint8 SC_Subcycle_Execution_Counters[SC_SUBC_EXEC_COUNTERS_NUM];
extern uint8 Stability_Control_Flags;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
ACTIVE_TYPE StabilityControl__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE StabilityControl__Task(uint8 compartment);
void StabilityControl__NVDataRestore(uint8 compartment);
void StabilityControl__Terminate(uint8 compartment);
void StabilityControl__VariablesInitialize(void);
SPEED_REDUCTION_STATUS_TYPE StabilityControl__GetSpeedReduction(void);
void StabilityControl__ReduceSpeed(uint16 *motor_speed, uint8 diverter_position);
ACTIVE_TYPE StabilityControl__GetStatus(void);
STABILITY_CONTROL_REACTIONS_TYPE StabilityControl__GetLDPrimaryReaction(void);
STABILITY_CONTROL_REACTIONS_TYPE StabilityControl__GetLDSecondaryReaction(void);
void StabilityControl__UpdateReaction(STABILITY_CONTROL_REACTIONS_TYPE *reaction_ptr);
void StabilityControl__SetFlagsMask(uint8 set_mask);
void StabilityControl__ResetFlagsMask(uint8 reset_mask);


#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_STABILITYCONTROL_STABILITYCONTROL_H_


