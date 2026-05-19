/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: DoorSwitchGI.h 1.3 2015/07/24 11:35:19EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:35:19EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DIRECTGI_H_
#define DIRECTGI_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_GI_NUM_DIRECT
    #define HBL_GI_NUM_DIRECT    0
#endif

#if (HBL_GI_NUM_DIRECT > 0)
    #include "HblDefs.h"

    //=====================================================================================================================
    //-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
    //=====================================================================================================================

    #define DIRECT_GI_REGISTER                  DirectGI__Initialize,           \
                                                DirectGI__Allocate,             \
                                                DirectGI__InitializeInstance,   \
                                                DirectGI__Get,                  \
                                                NULL,                           \
                                                DirectGI__GetConverted,         \
                                                1

    //=====================================================================================================================
    //-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
    //=====================================================================================================================
    void            DirectGI__Initialize            (void);
    uint8           DirectGI__Allocate              (void);
    PASS_FAIL_TYPE  DirectGI__InitializeInstance    (HBL_GI_TYPE * generic_input, uint8 lli_sequence);
    void *          DirectGI__Get                   (HBL_GI_TYPE * generic_input, void * lli_data);
    sint32          DirectGI__GetConverted          (HBL_GI_TYPE * generic_input, void * lli_data);

#endif // (HBL_GI_NUM_DIRECT > 0)

#endif // DIRECTGI_H_
