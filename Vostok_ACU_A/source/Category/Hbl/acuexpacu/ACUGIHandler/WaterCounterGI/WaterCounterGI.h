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
 *  $Header:    WaterCounterGI.h 1.9 2016/06/22 15:48:11EDT KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/06/22 15:48:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef WATERCOUNTERGI_H_
#define WATERCOUNTERGI_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_GI_NUM_WATERCOUNTER
    #define HBL_GI_NUM_WATERCOUNTER    0
#endif

#if (HBL_GI_NUM_WATERCOUNTER > 0)
#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================




#define WATERCOUNTER_GI_REGISTER            WaterCounterGI__Initialize,        \
                                            WaterCounterGI__Allocate,          \
                                            WaterCounterGI__InitializeInstance,\
                                            WaterCounterGI__Get,               \
                                            WaterCounterGI__AsynchProcess,     \
                                            WaterCounterGI__GetConverted,      \
                                            2

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void WaterCounterGI__Initialize(void);
uint8 WaterCounterGI__Allocate(void);
PASS_FAIL_TYPE WaterCounterGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence);
void * WaterCounterGI__Get(HBL_GI_TYPE * generic_input,void * lli_data);
BOOL_TYPE WaterCounterGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence);
sint32 WaterCounterGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data);

#endif

#endif /* WATERCOUNTERGI_H_ */
