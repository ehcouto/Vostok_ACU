/**
 * @file
 * @brief       Public parameters to the Statistics module.
 *
 * @details     This parameter file should #include a category-specific header file if there is
 *              any category-specific (custom) content to be included in the data structures.
 *
 *              The following code is a starting point for the content of the category-specific
 *              header file (e.g. StatisticsCooking.h):
 *
 *                  The version of the platform-specific data structures in the statistics data.
 *                  This version number is used by tools to understand the data structures below.
 *                  #define STATISTICS_PLATFORM_SPECIFIC_VERSION 1
 *
 *                  Only define PLATFORM_LIFETIME_TYPE if (STATISTICS_CUSTOM_LIFETIME_ENABLE == ENABLED)
 *                  Custom data structure used to extend the standard lifetime data structure.
 *                  typedef struct PLATFORM_LIFETIME_STRUCT
 *                  {
 *                      uint8 Custom_Fields_Go_Here;
 *                  } PLATFORM_LIFETIME_TYPE;
 *
 *                  Only define PLATFORM_CYCLE_TYPE if (STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED)
 *                  Custom data structure used to extend the standard cycle data structure.
 *                  typedef struct PLATFORM_CYCLE_STRUCT
 *                  {
 *                      uint8 Custom_Fields_Go_Here;
 *                  } PLATFORM_CYCLE_TYPE;
 *
 *                  Only define PLATFORM_LAST_CYCLE_TYPE if (STATISTICS_SAVED_LAST_CYCLES > 0)
 *                  Custom data structure to hold more detailed information about the last few cycles run.
 *                  typedef struct PLATFORM_LAST_CYCLE_STRUCT
 *                  {
 *                      uint8 Custom_Fields_Go_Here;
 *                  } PLATFORM_LAST_CYCLE_TYPE;
 *
 *                  Only define PLATFORM_BLACK_BOX_TYPE if ((STATISTICS_SAVED_FAULT_BLACK_BOX > 0) || (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0))
 *                  Custom data structure to hold more detailed information about the last few detected faults
 *                  and/or the last few detected pre-faults.
 *                  typedef struct PLATFORM_BLACK_BOX_STRUCT
 *                  {
 *                      uint8 Custom_Fields_Go_Here;
 *                  } PLATFORM_BLACK_BOX_TYPE;
 *
 * @copyright   Copyright 2015 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef STATISTICS_PRM_H_
#define STATISTICS_PRM_H_

/* This Define is defined here because "StatisticsDish.h" need of this define
 * In the line 104 is commented*/
#define STATISTICS_SAVED_CYCLE_REGULATIONS          27

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "StatisticsDish.h"
// -- This Module --

// -- Other Modules --
// #include "StatisticsCategory.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! The version of the platform-specific data structures in the statistics data.
//! This version number is used by tools to understand the statistics data structures.
#define STATISTICS_PLATFORM_SPECIFIC_VERSION 6

/**
 * Extends the lifetime data structure to hold the number of times that the first cycles have
 * been started by the user.
 * This value is ignored if STATISTICS_SAVED_CYCLES is 0.
 * Set this value to 0 to disable this feature.
 */
#define STATISTICS_NUMBER_OF_LIFETIME_CYCLES        15

/**
 * Extends the lifetime data structure with category-specific content.
 *
 * If (STATISTICS_CUSTOM_LIFETIME_ENABLE == ENABLED), then the PLATFORM_LIFETIME_TYPE must be
 * declared and the Statistics module will call a category-specific function through the
 * UPDATE_CUSTOM_LIFETIME() macro to update the PLATFORM_LIFETIME_TYPE data structure each time
 * it is required.
 *
 * If (STATISTICS_CUSTOM_LIFETIME_ENABLE == DISABLED), then the standard lifetime data structure
 * will be used with no category-specific content.
 */
#define STATISTICS_CUSTOM_LIFETIME_ENABLE          ENABLED//DISABLED

/**
 * The number of cycle data structures (STATISTICS_CYCLE_TYPE) to store in the statistics data.
 * Set this value to 0 to disable this feature.
 */
#define STATISTICS_SAVED_CYCLES                    10

/**
 * Adds the HMI regulation values at the start of a cycle to the cycle data structure.
 * Set the value to the number of elements in the #STATISTICS_CYCLE_REGULATION_FUNCTION_IDS.
 *
 * This value is ignored if STATISTICS_SAVED_CYCLES is 0.
 *
 * Set this value to 0 to disable this feature.
 */
//#define STATISTICS_SAVED_CYCLE_REGULATIONS          28

/**
 * Extends the cycle data structure with category-specific content.
 *
 * If (STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED), then the PLATFORM_CYCLE_TYPE must be declared
 * and the Statistics module will call a category-specific function through the
 * UPDATE_CUSTOM_CYCLE() macro to set and update the PLATFORM_CYCLE_TYPE data structure each time
 * it is required.
 *
 * If (STATISTICS_CUSTOM_CYCLE_ENABLE == DISABLED), then the standard cycle data structure
 * will be used with no category-specific content.
 *
 * This value is ignored if STATISTICS_SAVED_CYCLES is 0.
 */
#define STATISTICS_CUSTOM_CYCLE_ENABLE              ENABLED//DISABLED

/**
 * The number of last cycle data structures (PLATFORM_LAST_CYCLE_TYPE) to store in the
 * statistics data.
 *
 * If this value is non-zero, then the PLATFORM_LAST_CYCLE_TYPE must be declared and the
 * Statistics module will call a category-specific function through the UPDATE_CUSTOM_CYCLE()
 * macro to update the PLATFORM_LAST_CYCLE_TYPE data structure each time it is required.
 *
 * This value must be less than or equal to STATISTICS_SAVED_CYCLES.
 *
 * Set this value to 0 to disable this feature.
 */
#define STATISTICS_SAVED_LAST_CYCLES              	0

/**
 * The number of fault data structures (STATISTICS_FAULT_TYPE) based on detected fault to store
 * in the statistics data.
 */
#define STATISTICS_SAVED_FAULTS                     5

/**
 * The number of black box data structures (PLATFORM_BLACK_BOX_TYPE) based on detected fault to
 * store in the statistics data.
 *
 * If this value is non-zero, then the PLATFORM_BLACK_BOX_TYPE must be declared and the
 * Statistics module will call a category-specific function through the UPDATE_CUSTOM_FAULT()
 * macro to update the PLATFORM_BLACK_BOX_TYPE data structure each time it is required.
 *
 * This value must be less than or equal to STATISTICS_SAVED_FAULTS.
 *
 * Set this value to 0 to disable this feature.
 */
#define STATISTICS_SAVED_FAULT_BLACK_BOX            2

/**
 * The number of fault data structures (STATISTICS_FAULT_TYPE) based on detected pre-fault to
 * store in the statistics data.
 */
#define STATISTICS_SAVED_PRE_FAULTS                 5

/**
 * The number of black box data structures (PLATFORM_BLACK_BOX_TYPE) based on detected pre-fault
 * to store in the statistics data.
 *
 * If this value is non-zero, then the PLATFORM_BLACK_BOX_TYPE must be declared and the
 * Statistics module will call a category-specific function through the UPDATE_CUSTOM_PRE_FAULT()
 * macro to update the PLATFORM_BLACK_BOX_TYPE data structure each time it is required.
 *
 * This value must be less than or equal to STATISTICS_SAVED_PRE_FAULTS.
 *
 * Set this value to 0 to disable this feature.
 */
#define STATISTICS_SAVED_PRE_FAULT_BLACK_BOX        2


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* STATISTICS_PRM_H_ */
