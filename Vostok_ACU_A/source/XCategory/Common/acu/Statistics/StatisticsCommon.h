/**
 * @file
 * @brief       Public interfaces for use in category-specific statistics modules.
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef STATISTICSCOMMON_H_
#define STATISTICSCOMMON_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --


// -- Public Enumerated Constant Declarations --

//! Cycle status values that are provided for updates to category-specific cycle statistics.
typedef enum STATISTICS_CYCLE_STATUS_ENUM
{
    STATS_CYCLE_UNKNOWN             = 0,
    STATS_CYCLE_STARTED             = 1,
    STATS_CYCLE_ACTIVE              = 2,
    STATS_CYCLE_CANCELLED           = 3,
    STATS_CYCLE_ENDED               = 4
} STATISTICS_CYCLE_STATE_TYPE;


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // STATISTICSCOMMON_H_
