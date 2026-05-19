/**
 * @file
 * @brief       Private parameters for the Millis module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef MILLIS_PRV_H_
#define MILLIS_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Pre-compiler Directives --

//! Specify whether nonvolatile timers are supported on boards that are not the Main ACU.
//!     DISABLED = Nonvolatile timers are not supported
//!      ENABLED = Nonvolatile timers are supported
//! Note: On the Main ACU, nonvolatile timers are always supported.
#define MILLIS_SUPPORT_NV_TIMERS DISABLED

#if (MILLIS_SUPPORT_NV_TIMERS == ENABLED)
#include "API007App.h"   // include the appropriate header file used to control the restore activity
#endif
// -- Private Constant Declarations --

#if ((MILLIS_SUPPORT_NV_TIMERS == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
    //! The number of seconds between saving the free-running counter to API007 data.
    #define MILLIS_SAVE_PERIOD_SECONDS 60

    //! The maximum time to wait for initialization data through API007.
    //! After the timeout, Millis will not restore any NV timers and it will be ready to run timers.
    //! The time is measured in calls to the Millis__SlowHandler() function.
    #define MILLIS_START_UP_TIMEOUT 2000 //millisecond unit measure
#endif


// -- Private Enumerated Constant Declarations --


// -- Private Type Declarations --


// -- Private Flash-Based Constant Definitions --


// -- Private Variable Definitions --

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #pragma location = "_appliance_segment"
    //! A free-running millisecond counter used as a common reference for all timers.
    //! This value does not need to be initialized on an ACU project type.
    //! Refer to comments on Millis__Initialize() for details.
    static uint32 Milliseconds;                         //lint -e728 Symbol not explicitly initialized
#else
    #if (MILLIS_SUPPORT_NV_TIMERS == ENABLED)
        #pragma location = "_api007_client_nv_data"
        //! A copy of Milliseconds that is stored in nonvolatile memory through API007.
        //! This value is only updated periodically (e.g. Once per minute).
        static uint32 Stored_Milliseconds;
    #endif

    //! A free-running millisecond counter used as a common reference for all timers.
    static uint32 Milliseconds;
#endif


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Macro Declarations --
    /**
     * Use this macro to hook your Specific Function used to check if Data flow restoration has been completed;
     * normally it should be "API007App__IsDataRestored"
     * @return      Returns FALSE while restore operation is in progress.
     *              Returns TRUE when the restore operation has been concluded.
     *              If the data could not be restored, it will clear all values to zero and return TRUE.
     */
#if (MILLIS_SUPPORT_NV_TIMERS == ENABLED)
    #define IS_MILLIS_DATA_RESTORED()  TRUE //API007App__IsDataRestored()
#endif

#endif      // MILLIS_PRV_H_
