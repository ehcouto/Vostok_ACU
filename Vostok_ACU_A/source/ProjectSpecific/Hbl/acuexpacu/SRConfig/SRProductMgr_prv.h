/**
 *  @file       SRProductMgr_prv.h
 *
 *  @brief      Private parameters for the SRProductMgr module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRPRODUCTMGR_PRV_H_
#define SRPRODUCTMGR_PRV_H_

#include "SRProductMgr_Defs.h"

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

//! List of registers for the functions for each SRCheck module
static const SRCHECK_HOOK_TYPE	SRCHECK_HANDLERS[] =  {
                                                        { DUMMY_SRCHECK_REGISTER },
                                                      };

//! List of registers for the functions for each SRAction module
static const SRACTION_HOOK_TYPE	SRACTION_HANDLERS[] = {
                                                        { DUMMY_SRACTION_REGISTER },
                                                      };

#endif // SRPRODUCTMGR_PRV_H_
