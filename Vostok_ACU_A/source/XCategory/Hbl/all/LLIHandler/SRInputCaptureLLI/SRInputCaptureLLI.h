/**
 *  @file      SRInputCapture.h
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @defgroup CLASS_B
 *
 *  $Header: SRInputCaptureLLI.h 1.7 2015/08/10 11:59:22EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/10 11:59:22EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRINPUTCAPTURELLI_H
	#define SRINPUTCAPTURELLI_H
#include "C_Extensions.h"
#include "SRSystemConfig.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_SRINPUTCAPTURE
    #define HBL_LLI_NUM_SRINPUTCAPTURE    0
#endif

#if (HBL_LLI_NUM_SRINPUTCAPTURE > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
PACKED typedef struct
{
    uint16 Period;                    // Period in terms of micro second
    uint16 Duty;                      // Duty in terms of %
    uint8 Valid_Sample_Count;
    uint8 InValid_Sample_Count;
    uint8 Sequence_Count;             // This is used to synchronize the reading
} SRINPUTCAPTURE_PUBLIC_TYPE;

#define SRINPUTCAPTURE_LLI_REGISTER         SRInputCaptureLLI__Initialize,        \
                                            SRInputCaptureLLI__Allocate,          \
                                            SRInputCaptureLLI__InitializeInstance,\
                                            SRInputCaptureLLI__Get,               \
                                            SRInputCaptureLLI__AsynchProcess,     \
                                            NULL,                                 \
                                            SRInputCaptureLLI__ICProcess,         \
                                            NULL,                                 \
                                            sizeof(SRINPUTCAPTURE_PUBLIC_TYPE)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRInputCaptureLLI__Initialize(void);
uint8 SRInputCaptureLLI__Allocate(void);
void SRInputCaptureLLI__Handler(void);
PASS_FAIL_TYPE SRInputCaptureLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * SRInputCaptureLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRInputCaptureLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE SRInputCaptureLLI__ICProcess(HBL_LLI_TYPE * input);

#endif
#endif // INPUTCAPTURELLI_H


