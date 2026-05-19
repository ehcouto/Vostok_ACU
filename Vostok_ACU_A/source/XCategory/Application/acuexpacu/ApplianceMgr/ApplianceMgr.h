/**
 *  @file
 *
 *  @brief      Public interface for the ApplianceMgr module.
 *
 *  @copyright  Copyright 2015-2020 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef APPLIANCEMGR_H_
#define APPLIANCEMGR_H_

#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void        ApplianceMgr__Initialize        (void);
void        ApplianceMgr__Handler           (void);

#if (SYSTEM_ACU_HANDLING == ENABLED)
    void        ApplianceMgr__InitializeStable  (void);
    BOOL_TYPE   ApplianceMgr__IsStable          (void);
#endif

#endif // APPLIANCEMGR_H_
