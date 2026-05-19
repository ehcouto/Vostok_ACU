/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: InputCaptureLLI.h 1.7 2015/08/10 11:59:22EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/10 11:59:22EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef INPUTCAPTURELLI_H
	#define INPUTCAPTURELLI_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_INPUTCAPTURE
    #define HBL_LLI_NUM_INPUTCAPTURE    0
#endif

#if (HBL_LLI_NUM_INPUTCAPTURE > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
PACKED typedef struct
{
    uint16 Period;                    // Period in terms of micro seconds
    uint16 Duty;                      // Duty Cycle in terms of %.  Resolution: 1 = 0.01%
    uint8 Valid_Sample_Count;
    uint8 InValid_Sample_Count;
    uint8 Sequence_Count;             // This is used to synchronize the reading
} INPUTCAPTURE_PUBLIC_TYPE;

#define INPUTCAPTURE_LLI_REGISTER           InputCaptureLLI__Initialize,        \
                                            InputCaptureLLI__Allocate,          \
                                            InputCaptureLLI__InitializeInstance,\
                                            InputCaptureLLI__Get,               \
                                            InputCaptureLLI__AsynchProcess,     \
                                            NULL,                               \
                                            InputCaptureLLI__ICProcess,         \
                                            NULL,                               \
                                            sizeof(INPUTCAPTURE_PUBLIC_TYPE)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void InputCaptureLLI__Initialize(void);
uint8 InputCaptureLLI__Allocate(void);
PASS_FAIL_TYPE InputCaptureLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * InputCaptureLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE InputCaptureLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE InputCaptureLLI__ICProcess(HBL_LLI_TYPE * input);

#endif
#endif // INPUTCAPTURELLI_H


