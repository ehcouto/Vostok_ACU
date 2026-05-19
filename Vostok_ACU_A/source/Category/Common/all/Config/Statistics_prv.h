/**
 * @file
 * @brief       Private parameters for the Statistics module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2015 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef STATISTICS_PRV_H_
#define STATISTICS_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#if (STATISTICS_SAVED_CYCLE_REGULATIONS > 0)
    #include "Regulations.h"
#endif
//#include "StatisticsCategory.h"


//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Private Constant Declarations --


// -- Private Type Declarations --


// -- Private Flash-Based Constant Definitions --

#if (STATISTICS_SAVED_CYCLE_REGULATIONS > 0)
//! A list of regulations that are saved in statistics data.
//! #STATISTICS_SAVED_CYCLE_REGULATIONS must be set to the size of this array.
static const REGULATIONS_FUNCTION_ID_TYPE STATISTICS_CYCLE_REGULATION_FUNCTION_IDS[] =
{

	FUNC_ID_SELECTOR,
	FUNC_ID_START,
	FUNC_ID_MODE	,
	FUNC_ID_SELECT   ,
	FUNC_ID_SELECT_2  ,
	FUNC_ID_CHILD_LOCK ,
	FUNC_ID_ON_OFF      ,
	FUNC_ID_3D_WASH	,
	FUNC_ID_DELAY_HH ,
	FUNC_ID_DELAY_MM  ,
	FUNC_ID_DELAY_SS   ,
	FUNC_ID_DEMO        ,
	FUNC_ID_RAPID_ADVANCE_MODE ,
	FUNC_ID_TEST_CYCLE			,
	FUNC_ID_LIFE_TEST_MODE		 ,
	FUNC_ID_LIGHT_IN_THE_TUB	  ,
	FUNC_ID_ADVANCE_STEP,
	FUNC_ID_ADVANCE_PHASE,
	FUNC_ID_SANI_RINSE,
	FUNC_ID_MIDDLE_RACK_ONLY,
	FUNC_ID_LOWER_RACK_ONLY,
	FUNC_ID_HEATED_DRY,
	FUNC_ID_HIGH_TEMP,
	FUNC_ID_HALF_LOAD,
	FUNC_ID_TIME_SHORTENING,
	FUNC_ID_POWER_BOOST,

	FUNC_ID_DELAY_PAUSE
};
#endif


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


#if (STATISTICS_CUSTOM_LIFETIME_ENABLE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/*
 * Request platform-specific lifetime data from the platform-specific statistics customization module.
 * This macro is only used if STATISTICS_CUSTOM_LIFETIME_ENABLE is ENABLED.
 * This macro is called once every second.
 * @param lifetime = Pointer to #PLATFORM_LIFETIME_TYPE that must be updated.
 */
#define UPDATE_CUSTOM_LIFETIME(lifetime) StatisticsDish__UpdateLifetimeData(lifetime)
#endif


#if (((STATISTICS_SAVED_CYCLES > 0) && (STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED)) || (STATISTICS_SAVED_LAST_CYCLES > 0))
//---------------------------------------------------------------------------------------------------------------------
/*
 * Request platform-specific cycle data from the platform-specific statistics customization module.
 * This macro is only used if custom cycle data or custom last cycle data is enabled.
 * This macro is called when a cycle stops or when cycle statistics data is requested while the cycle is running.
 * @param compartment = The compartment in which a cycle is actively running.
 * @param state = #STATISTICS_CYCLE_STATE_TYPE for the cycle.
 * @param cycle = Pointer to #PLATFORM_CYCLE_TYPE that should be updated.
 *                NULL if no data needs to be updated.
 * @param last_cycle = Pointer to the #PLATFORM_LAST_CYCLE_TYPE that should be updated.
 *                     NULL if no data needs to be updated.
 */
#define UPDATE_CUSTOM_CYCLE(compartment, state, cycle, last_cycle) StatisticsDish__UpdateCycleData(compartment, state, cycle, last_cycle)
#endif


#if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
//---------------------------------------------------------------------------------------------------------------------
/*
 * Request platform-specific black box data from the platform-specific statistics customization module.
 * This macro is only used if black box data is enabled for faults.
 * This macro is called when a fault occurs if black box data is configured for faults.
 * @param black_box = Pointer to #PLATFORM_BLACK_BOX_TYPE that should be updated.
 * @param fault_id = The fault_id that triggered this update.
 */
#define UPDATE_CUSTOM_FAULT(black_box, fault_id) StatisticsDish__UpdateFaultData(black_box, fault_id)
#endif


#if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
//---------------------------------------------------------------------------------------------------------------------
/*
 * Request platform-specific black box data from the platform-specific statistics customization module.
 * This macro is only used if black box data is enabled for pre-faults.
 * This macro is called when a pre-fault occurs if black box data is configured for pre-faults.
 * @param black_box = Pointer to #PLATFORM_BLACK_BOX_TYPE that should be updated.
 * @param fault_id = The fault_id that triggered this update.
 */
#define UPDATE_CUSTOM_PRE_FAULT(black_box, fault_id) StatisticsDish__UpdatePreFaultData(black_box, fault_id)
#endif


#endif      /* STATISTICS_PRV_H_ */
