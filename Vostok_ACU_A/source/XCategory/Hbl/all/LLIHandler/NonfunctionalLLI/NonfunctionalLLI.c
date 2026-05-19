/**
 *  @file
 *  @brief      LLI that provides an API but no data.
 *
 *  @details    This LLI can be registered with a Virtual GI that does not need data from an LLI.
 *              This will supply an LLI for GESE and HBL to meet API requirements.
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

//lint -e766 Suppress Info: header not used in module 'VirtualGI.c'
#include "SystemConfig.h"

#if (HBL_LLI_NUM_NONFUNCTIONAL > 0)
#include "NonfunctionalLLI.h"

//  --- Private Definitions -------------------------------------------------------------------------------------------
//! Tracks the number of Nonfunctional LLI instances.
static uint8 Allocation;

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
* Initialize the NonfunctionalLLI module's variables.
*/
void NonfunctionalLLI__Initialize(void)
{
    Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Track number of Nonfunctional LLIs registered with Virtual GIs.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Nonfunctional LLI allocation instance.
 *  @retval     HBL_INDEX_INVALID - Allocation failed.
 */
uint8 NonfunctionalLLI__Allocate(void)
{
    uint8 index = HBL_INDEX_INVALID;
    if (Allocation < HBL_LLI_NUM_NONFUNCTIONAL)
    {
        index = Allocation;
        Allocation++;
    }
    return index;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Satisfy HBL Interface. No instance initialization required.
 *
 *  @param input = Pointer to the HBL data structure that references the LLI instance.
 *
 *  @return     PASS_FAIL_TYPE - Result of nonfunctional LLI instance initialization.
 *  @retval     PASS - Instance initialization always succeeds.
 */
PASS_FAIL_TYPE NonfunctionalLLI__InitializeInstance(HBL_LLI_TYPE* input)
{
    return (PASS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get nonfunctional LLI data.
 *
 * Note: A nonfunctional has no data by definition. To satisfy HBL we return a pointer to the allocation value.
 *
 *  @param input = Pointer to the HBL data structure that references the LLI instance.
 *  @return Address of Allocation - Indicates how many nonfunctional LLIs are allocated.
 */
void* NonfunctionalLLI__Get(HBL_LLI_TYPE* input)
{
    return (&Allocation);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif // (HBL_LLI_NUM_NONFUNCTIONAL > 0)
