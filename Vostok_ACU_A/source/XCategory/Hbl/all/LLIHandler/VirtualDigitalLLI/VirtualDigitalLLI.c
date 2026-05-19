/**
 *  @file       
 *
 *  @brief      The Virtual Digital LLI is used when there is unique processing to determine the
 *              value of a digital input. This module supports a virtual digital with 1 or 4
 *              input pins.
 *
 *  @details    The parameter file for this module should be used to link the project specific
 *              implementation that determines the value of a virtual digital input.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "VirtualDigitalLLI.h"

#if (HBL_LLI_NUM_VIRTUAL_DIGITAL > 0)
#include "VirtualDigitalLLI_prv.h"
#include <string.h>
#include "VirtualPin.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 VirtualDigitalLLI_Allocation;

//! Current state of the input(s) for the virtual digital low level input
//! The number of inputs can be either 1 or 4, based on the LLI ID
static uint8 VirtualDigitalLLI_Data[HBL_LLI_NUM_VIRTUAL_DIGITAL][4];

#define VIRTUAL_DIGITALLOWINPUT_CFG1_INVERT          0x80

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initializes the module VirtualDigitalLLI and its variables
 */
void VirtualDigitalLLI__Initialize(void)
{
    VirtualDigitalLLI_Allocation = 0;
    memset (VirtualDigitalLLI_Data, 0, sizeof(VirtualDigitalLLI_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Performs allocation for an LLI
 *
 * @return  TRUE if the LLI was allocated, else FALSE.
 */
uint8 VirtualDigitalLLI__Allocate(void)
{
    uint8 retval;
    retval = HBL_ALLOCATION_INVALID;

    if (VirtualDigitalLLI_Allocation < HBL_LLI_NUM_VIRTUAL_DIGITAL)
    {
        retval = VirtualDigitalLLI_Allocation;
        VirtualDigitalLLI_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initializes a single instance of an LLI
 *
 *  @return TRUE if the LLI can be initialized, else FALSE
 */
PASS_FAIL_TYPE VirtualDigitalLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (input->Data_Index < VirtualDigitalLLI_Allocation)
    {
        retval = PASS;
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Gets the value of an LLI
 * @param   input: See HBL_LLI_TYPE
 *
 * @return  value of the VirtualDigital LLI
 */
void * VirtualDigitalLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < VirtualDigitalLLI_Allocation)
    {
        switch(input->LLI_ID)
        {
            case HBL_LLI_VIRTUALDIGITAL:
                if(input->Parameter_Ptr.Data[1] & VIRTUAL_DIGITALLOWINPUT_CFG1_INVERT)
                {
                    VirtualDigitalLLI_Data[input->Data_Index][0] = GET_VIRTUAL_DIGITAL_LLI(input->Parameter_Ptr.Data[2]) & BIT0_MASK;
                    VirtualDigitalLLI_Data[input->Data_Index][0] ^= TRUE;
                }
                else
                {
                    VirtualDigitalLLI_Data[input->Data_Index][0] = GET_VIRTUAL_DIGITAL_LLI(input->Parameter_Ptr.Data[2]) & BIT0_MASK;     // Referencing index 2 of the parameters because first 2 bytes are configurations flags common to all LLIs
                }
                break;
            case HBL_LLI_VIRTUALDIGITAL_4:
                VirtualDigitalLLI_Data[input->Data_Index][0] = GET_VIRTUAL_DIGITAL_LLI(input->Parameter_Ptr.Data[2]) & BIT0_MASK;   // Referencing index 2 of the parameters because first 2 bytes are configurations flags common to all LLIs
                VirtualDigitalLLI_Data[input->Data_Index][1] = GET_VIRTUAL_DIGITAL_LLI(input->Parameter_Ptr.Data[3]) & BIT0_MASK;
                VirtualDigitalLLI_Data[input->Data_Index][2] = GET_VIRTUAL_DIGITAL_LLI(input->Parameter_Ptr.Data[4]) & BIT0_MASK;
                VirtualDigitalLLI_Data[input->Data_Index][3] = GET_VIRTUAL_DIGITAL_LLI(input->Parameter_Ptr.Data[5]) & BIT0_MASK;
                break;
            default:
                break;
        }

        return((void *)VirtualDigitalLLI_Data[input->Data_Index]);         //lint !e923 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.1],[MISRA 2004 Rule 11.3]
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Asynchronous processing of the LLI
 * @param   input: see HBL_LLI_TYPE
 * @return  Always TRUE since there is no processing to do
 */
BOOL_TYPE VirtualDigitalLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    // Function needs to return TRUE to increment the LLI sequence number (indicating new sample is available)
    return (TRUE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif //#if (HBL_LLI_NUM_VIRTUAL_DIGITAL > 0)
