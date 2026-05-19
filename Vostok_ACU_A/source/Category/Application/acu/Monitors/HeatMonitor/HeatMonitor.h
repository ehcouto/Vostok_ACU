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
#ifndef HEATMONITOR_H_
#define HEATMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Setting file data structure for pointer SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, displacement DISPL_HEATER_DEACTIVATION_CONDITIONS_PARAM
typedef PACKED struct HEAT_MONITOR_INSTANCE_STRUCT
{
	uint8 Heater_Type;
	uint8 Condition_Index;
	uint8 Heater_Activation_Delay;
	uint8 Unused;
	uint16 Heater_Turn_Off_Speed;
	uint16 Heater_Turn_On_Speed;
} HEAT_MONITOR_INSTANCE_TYPE;

//! A list of heater's types
typedef enum HEATER_TYPES_ENUM
{
	CALROD_HEATER		= 0,
	INTEGRATED_HEATER	= 1,
} HEATER_TYPES_TYPE;



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void HeatMonitor__Initialize(void);
void HeatMonitor__Execution(void);
BOOL_TYPE HeatMonitor__IsHeaterRequested(void);
BOOL_TYPE HeatMonitor__IsHeaterAllowed(void);
BOOL_TYPE HeatMonitor__IsHeaterVWIAllowed(void);

#endif // HEATMONITOR_H_


