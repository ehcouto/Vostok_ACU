/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef STEADYPWMPROFILEPILOT_H
	#define STEADYPWMPROFILEPILOT_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_STEADYPWMPROFILE
    #define HBL_PILOT_NUM_STEADYPWMPROFILE                0
#endif

#if (HBL_PILOT_NUM_STEADYPWMPROFILE > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#define STEADYPWMPROFILEPILOT_REGISTER      SteadyPwmProfilePilot__Initialize,         \
                                            SteadyPwmProfilePilot__Allocate,           \
                                            SteadyPwmProfilePilot__InitializeInstance, \
                                            SteadyPwmProfilePilot__Request,            \
                                            SteadyPwmProfilePilot__Process,            \
                                            SteadyPwmProfilePilot__GetRequested,       \
                                            SteadyPwmProfilePilot__GetProcessed,       \
                                            NULL,                                      \
                                            NULL,                                      \
                                            NULL,                                      \
                                            SteadyPwmProfilePilot__OneMsHandler,       \
                                            SteadyPwmProfilePilot__GetStatus,          \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SteadyPwmProfilePilot__Initialize(void);
uint8 SteadyPwmProfilePilot__Allocate(void);
PASS_FAIL_TYPE SteadyPwmProfilePilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SteadyPwmProfilePilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE SteadyPwmProfilePilot__Process(HBL_LOAD_TYPE * load);
void * SteadyPwmProfilePilot__GetRequested(HBL_LOAD_TYPE * load);
void * SteadyPwmProfilePilot__GetProcessed(HBL_LOAD_TYPE * load);
void * SteadyPwmProfilePilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SteadyPwmProfilePilot__OneMsHandler(HBL_LOAD_TYPE * load);

#endif

#endif // STEADYPWMPROFILEPILOT_H


