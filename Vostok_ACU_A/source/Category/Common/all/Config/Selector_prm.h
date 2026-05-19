/**
 *  @file
 *
 *  @brief      Public parameters for the Selector module.
 *
 *
 *  @copyright  Copyright 2017-2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SELECTOR_PRM_H_
#define SELECTOR_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Constants & Defines) -------------------------------
//=====================================================================================================================
//! Extend cycle name enumeration with platform specific values (allowed range is 1 - 65279).
#define CYCLE_NAME_PLATFORM_SPECIFIC  \
	 CYCLE_NAME_SPECIFIC_CYCLE_RESUME			=	1        \
	,CYCLE_NAME_SPECIFIC_NORMAL					=	2        \
	,CYCLE_NAME_SPECIFIC_RINSE_ONLY				=	3        \
	,CYCLE_NAME_SPECIFIC_30_MINUTES_WASH		=	4        \
	,CYCLE_NAME_SPECIFIC_1_HOUR_WASH			=	5        \
	,CYCLE_NAME_SPECIFIC_LIGHT_CHINA			=	6        \
	,CYCLE_NAME_SPECIFIC_MIXED					=	7        \
	,CYCLE_NAME_SPECIFIC_SANITIZE	        	=	8        \
	,CYCLE_NAME_SPECIFIC_POTS_AND_PANS	    	=	9        \
	,CYCLE_NAME_SPECIFIC_SENSOR              	=	10       \
	,CYCLE_NAME_SPECIFIC_ECO                 	=	11       \
	,CYCLE_NAME_SPECIFIC_PLATE_WARMER	    	=	12       \
	,CYCLE_NAME_SPECIFIC_HALF_LOAD	        	=	13       \
	,CYCLE_NAME_SPECIFIC_SOAK_AND_CLEAN	    	=	14       \
	,CYCLE_NAME_SPECIFIC_OVERNIGHT	        	=	15       \
	,CYCLE_NAME_SPECIFIC_POWER_CLEAN			=	16       \
	,CYCLE_NAME_SPECIFIC_AUTO_NORMAL			=	17       \
	,CYCLE_NAME_SPECIFIC_AUTO_INTENSIVE	    	=	18       \
	,CYCLE_NAME_SPECIFIC_AUTO_FAST	        	=	19       \
	,CYCLE_NAME_SPECIFIC_STEAM	            	=	20       \
	,CYCLE_NAME_SPECIFIC_WATER_TANK_DRAIN_OUT   =	21       \
	,CYCLE_NAME_SPECIFIC_EMPTY_CYCLE            =	22       \
	,CYCLE_NAME_SPECIFIC_POST_CYCLE_BEHAVIOR    =	23       \
	,CYCLE_NAME_SPECIFIC_CANCEL_DRAIN			=	24       \
	,CYCLE_NAME_SPECIFIC_INSTALL                =	25       \
	,CYCLE_NAME_SPECIFIC_WATER_TANK_TEST        =	26       \
	,CYCLE_NAME_SPECIFIC_DRAIN_PRE_FAULT        =	27       \
	,CYCLE_NAME_SPECIFIC_OVERFILL_FAULT			=	28       \
	,CYCLE_NAME_SPECIFIC_FILL_CHECK_WATER	    =	29       \
	,CYCLE_NAME_SPECIFIC_SC_ADD_WATER	        =	30       \
	,CYCLE_NAME_SPECIFIC_SC_MOTOR_RESTART		=	31       \
	,CYCLE_NAME_SPECIFIC_SC_FILTER_PLUGGED		=	32       \
	,CYCLE_NAME_WASH_MOTOR_FAULT				=	34       \
	,CYCLE_NAME_DRAIN_MOTOR_FAULT				=	35       \
	,CYCLE_NAME_FILL_PREFAULT					=	36       \
	,CYCLE_NAME_VOLTAGE_RANGE_RECOVERY			=	61       \

//! Extend cycle group enumeration with platform specific values.
#define CYCLE_GROUP_PLATFORM_SPECIFIC \
//    CYCLE_GROUP_CLEANING    = 5

//! Extend user phase name enumeration with platform specific values (allowed range is 1 - 65279).
#define USER_PHASE_NAME_PLATFORM_SPECIFIC  \
    USER_PHASE_NAME_PREWASH             = 1, \
    USER_PHASE_NAME_WASH	            = 2, \
//    USER_PHASE_NAME_SENSE               = 3, \
//    USER_PHASE_NAME_COOK                = 4, \
//    USER_PHASE_NAME_COOL                = 5, \
//    USER_PHASE_NAME_CLEAN               = 6, \
//    USER_PHASE_NAME_RAPID_PREHEAT       = 7

#endif /* SELECTOR_PRM_H_ */









