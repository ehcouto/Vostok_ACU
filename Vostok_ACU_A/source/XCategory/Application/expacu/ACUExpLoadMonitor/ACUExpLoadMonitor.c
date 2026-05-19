/**
 *  @file
 *
 *  @brief      Implements setting of comm (expansion) loads received over API012.
 *
 *  @details    Priority of loads set are HBL_PRIORITY_ACU_EXP_LOAD_MONITOR
 *              so that local monitors can select higher or lower priorities as necessary.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"

#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
    #include "ACUExpLoadMonitor.h"

    // -- Other Modules --
    #include "API012Exp.h"
    #include "Hbl.h"
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ACUExpLoadMonitor and its variables
 *
 */

void ACUExpLoadMonitor__Initialize(void)
{
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Iterate through comm (expansion) loads and set local loads to values received from API012.
 *              Uses HBL_PRIORITY_ACU_EXP_LOAD_MONITOR.
 */
void ACUExpLoadMonitor__Execute(void)
{
    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        uint8 *comm_loads;
        uint8 loop_index;
        uint8 num_comm_loads;

        comm_loads = API012Exp__GetCommLoads();
        num_comm_loads = API012Exp__GetNumCommLoads();

        if (comm_loads != NULL)
        {
            // If num_comm_loads is 0, this loop will not run
            for (loop_index = 0; loop_index < num_comm_loads; loop_index++)
            {
                Hbl__SetLoadRequestByIndex(loop_index, HBL_PRIORITY_ACU_EXP_LOAD_MONITOR, &comm_loads[loop_index]);
            }
        }
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
