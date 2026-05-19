/**
 * @file
 * @brief       Automatically accept regulations at the end of the main application thread.
 *
 * @details     This module will automatically accept all regulations that have not already been
 *              accepted at the end of the main application thread.
 *
 *              There is an option to specify platform-specific regulations that should not be
 *              automatically accepted.
 *
 *              This module will also call Estimator to restart time estimations if any new
 *              regulation is accepted.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "RegulationAccept.h"
#include "RegulationAccept_prv.h"

// -- Other Modules --
#include "Estimator.h"
#include "Regulations.h"
#include <string.h>


//  --- Private Declarations ------------------------------------------------------------------------------------------


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

//! Array of regulations that must never be automatically accepted.
static const REGULATIONS_FUNCTION_ID_TYPE EXTERNALLY_MANAGED_REGULATIONS[] =
{
    FUNC_ID_START,
    FUNC_ID_SELECTOR,
    FUNC_ID_DELAY_HH,
    FUNC_ID_DELAY_MM,
    FUNC_ID_DELAY_SS,
    FUNC_ID_ADVANCE_PHASE,
    FUNC_ID_ADVANCE_STEP,
    FUNC_ID_TEST_CYCLE,
    PLATFORM_EXTERNALLY_MANAGED_REGULATIONS
};


//  --- Private Function Prototypes -----------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Accept any unmanaged regulations that have not already been accepted.
 * This function will also trigger the Estimator module to restart estimations if a regulation is accepted.
 */
void RegulationAccept__Background(void)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        BOOL_TYPE auto_accepted = FALSE;
    #endif

    uint8 num_of_positions = Regulations__GetTotalSize();
    for (uint8 position = 0; position < num_of_positions; position++)
    {
        uint8 function_id = Regulations__GetFunctionIDByPosition(position);
        if (memchr(EXTERNALLY_MANAGED_REGULATIONS, function_id, sizeof(EXTERNALLY_MANAGED_REGULATIONS)) == NULL)
        {
            if (Regulations__IsRegulationChanged(position) == TRUE)
            {
                Regulations__AcceptRequestByPosition(position);
                #if (TIME_ESTIMATION_FEATURE == ENABLED)
                    auto_accepted = TRUE;
                #endif
            }
        }
    }

    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        if (auto_accepted == TRUE)
        {
            Estimator__EstimateFast();
        }
    #endif
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================
