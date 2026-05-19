/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This module is responsible to expose the Class B Signature.
 *
 *
 *  @copyright  Copyright 2016. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRSignature.h"

#include "ClassB_Signature.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static const uint8 CLASS_B_SIGNATURE[]          = CLASSB_SIGNATURE;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This process returns the string of values for the Class B Signature;
 * @return
 */
uint8 * SRSignature__GetClassBSignature(void)
{
    return ((uint8*)CLASS_B_SIGNATURE);             //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
}

/**
 * @brief This process returns size of the Class B String signature.
 * @return
 */
uint8 SRSignature__GetClassBSignatureSize(void)
{
    return (sizeof(CLASS_B_SIGNATURE));
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


