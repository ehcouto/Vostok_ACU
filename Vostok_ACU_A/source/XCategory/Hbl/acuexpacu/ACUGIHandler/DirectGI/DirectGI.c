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
 *  $Header: DirectGI.c 1.4 2015/07/24 11:35:20EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:35:20EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DirectGI.h"

#if (HBL_GI_NUM_DIRECT > 0)
#include "SettingFile.h"
#include "string.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void DirectGI__Initialize(void)
{

}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 DirectGI__Allocate(void)
{
    return (1);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DirectGI and its variables
 *
 */
PASS_FAIL_TYPE DirectGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence)
{
    return (PASS);
}


void * DirectGI__Get(HBL_GI_TYPE * generic_input, void * lli_data)
{

    return (lli_data);
}

sint32 DirectGI__GetConverted(HBL_GI_TYPE * generic_input, void * lli_data)
{
    sint32 data_in;

    if (generic_input->LLI_Parameter_Size >= 4)
    {
        data_in = (sint32)(*((PACKED sint32 *)lli_data));
    }
    else if (generic_input->LLI_Parameter_Size >= 2)
    {
        data_in = (sint32)(*((PACKED sint16 *)lli_data));
    }
    else
    {
        data_in = (sint32)(*((sint8 *)lli_data));
    }

    return (data_in);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif
