/**
 *  @file       
 *  @defgroup    CLASS_B
 *
 *  @brief      Public interface for the SRChargePumpPilot module
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:52EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRCHARGEPUMPPILOT_H_
#define SRCHARGEPUMPPILOT_H_

#include "C_Extensions.h"
#include "SRSystemConfig.h"

#ifndef HBL_PILOT_NUM_SRCHARGEPUMP
    #define HBL_PILOT_NUM_SRCHARGEPUMP 0
#endif

#if (HBL_PILOT_NUM_SRCHARGEPUMP > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define SRCHARGEPUMPPILOT_REGISTER          SRChargePumpPilot__Initialize,            \
                                            SRChargePumpPilot__Allocate,              \
                                            SRChargePumpPilot__InitializeInstance,    \
                                            SRChargePumpPilot__Request,               \
                                            SRChargePumpPilot__Process,               \
                                            SRChargePumpPilot__GetRequested,          \
                                            SRChargePumpPilot__GetProcessed,          \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            SRChargePumpPilot__Handler1ms,            \
                                            NULL,                                   \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRChargePumpPilot__Initialize(void);
PASS_FAIL_TYPE SRChargePumpPilot__InitializeInstance(HBL_LOAD_TYPE * load);
uint8 SRChargePumpPilot__Allocate(void);
PASS_FAIL_TYPE SRChargePumpPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE SRChargePumpPilot__Process(HBL_LOAD_TYPE * load);
void * SRChargePumpPilot__GetRequested(HBL_LOAD_TYPE * load);
void * SRChargePumpPilot__GetProcessed(HBL_LOAD_TYPE * load);

PASS_FAIL_TYPE SRChargePumpPilot__Handler1ms(HBL_LOAD_TYPE * load);

#endif  //#if (HBL_PILOT_NUM_SRCHARGEPUMP > 0)
#endif // SRCHARGEPUMPPILOT_H_


