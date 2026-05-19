/**
 *  @file       
 *
 *  @brief      Implements ExpansionLLI.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "ExpansionLLI.h" 

#include <string.h>

#if (HBL_LLI_NUM_EXPANSION > 0)

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Tracks the number of expansion LLI allocations.
static uint8 Expansion_LLI_Allocation;

//! Holds position of GI in API012 GI buffer.
static uint8 Position[HBL_LLI_NUM_EXPANSION];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ExpansionLLI and its variables
 *
 */
void ExpansionLLI__Initialize(void)
{
    Expansion_LLI_Allocation = 0;
    memset(Position, 0, sizeof(Position));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate instance for ExpansionLLI.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Expansion LLI allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 ExpansionLLI__Allocate(void)
{
    uint8 retval;

    retval = 0xFF;

    if (Expansion_LLI_Allocation < HBL_LLI_NUM_EXPANSION)
    {
        retval = Expansion_LLI_Allocation;
        Expansion_LLI_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize Expansion LLI instance.
 *
 *  @param      HBL_LLI_TYPE * input - Pointer to the expansion LLI instance.
 *  @param      HBL_LLI_STATUS *status - Pointer to the expansion LLI instance status.
 *
 *  @return     PASS_FAIL_TYPE result - Result of expansion LLI instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL invalid instance
 */
PASS_FAIL_TYPE ExpansionLLI__InitializeInstance(HBL_LLI_TYPE *input)
{
    PASS_FAIL_TYPE result;

    result = FAIL;

    if (input != NULL)
    {
        if (input->Data_Index < Expansion_LLI_Allocation)
        {
            result = PASS;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get expansion LLI data.
 *
 *  @details    LLI data contains the position of the GI in the API012 Buffer.
 *
 *  @param      HBL_LLI_TYPE * input - Pointer to the expansion LLI instance.
 *
 *  @return     void *positon_pointer - pointer to index of GI in the API012 Buffer.
 *  @retval     Address of value of instance LLI. Cast it using *((uint8 *)ptr) to get value.
 *  @retval     NULL - Invalid input index.
 */
void * ExpansionLLI__Get(HBL_LLI_TYPE *input)
{
    void *positon_pointer;

    positon_pointer = NULL;

    if (input != NULL)
    {
        if (input->Data_Index < Expansion_LLI_Allocation)
        {
            Position[input->Data_Index] = input->LLI_Position;
            positon_pointer = &Position[input->Data_Index];
        }
    }

    return (positon_pointer);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif /* HBL_LLI_NUM_EXPANSION */
