/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "StackAnalysis.h"
#include "RTOS.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#if (STACK_ANALYSIS == ENABLED)
    extern OS_TASK MainTCB;
    extern OS_TASK MainAppMgrTCB;
    extern OS_TASK MainCommunicationTCB;
    extern OS_TASK MainProgrammingAsynchTCB;
    extern OS_TASK MainFastAppMgrTCB;

    static OS_TASK* const Task_Control_Block_List[] =
    {
        &MainTCB,
        &MainAppMgrTCB,
        &MainFastAppMgrTCB,
        &MainProgrammingAsynchTCB,
        &MainCommunicationTCB
    };
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that provides Stack information associated to a task or system stacks (Interrupt, user)
 *  @param      stack_index - one of STACKANALYSIS_STACKS_TYPE stack index
 *  @return     copy of Stack information data
 *
 */
STACKANALYSIS_STACK_DATA_TYPE StackAnalysis__GetStackInformation(STACK_ID_TYPE stack_index)
{
    STACKANALYSIS_STACK_DATA_TYPE retval = {0,0};

    #if (STACK_ANALYSIS == ENABLED)
        if (stack_index < NUM_STACKS)
        {
            switch (stack_index)
            {
                case INTERRUPT_STACK:
                    retval.Total_Size = (uint16) OS_GetIntStackSize();
                    retval.Free_Space = (uint16) OS_GetIntStackSpace();
                    break;
                case SYSTEM_STACK:
                    retval.Total_Size = (uint16) OS_GetSysStackSize();
                    retval.Free_Space = (uint16) OS_GetSysStackSpace();
                    break;
                default:
                    retval.Total_Size = (uint16) OS_GetStackSize(Task_Control_Block_List[stack_index]);
                    retval.Free_Space = (uint16) OS_GetStackSpace(Task_Control_Block_List[stack_index]);
                    break;
            }
        }
    #endif

    return retval;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


