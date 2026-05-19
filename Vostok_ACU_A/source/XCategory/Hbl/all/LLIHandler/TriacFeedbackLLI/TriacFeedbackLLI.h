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
#ifndef TRIACFEEDBACKLLI_H
	#define TRIACFEEDBACKLLI_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_TRIACFEEDBACK
    #define HBL_LLI_NUM_TRIACFEEDBACK    0
#endif

#if (HBL_LLI_NUM_TRIACFEEDBACK > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum TRIAC_FEEDBACK_LLI_OUTPUT_ENUM
{
    TRIAC_FB_LLI_0_VALUE = 0,
    TRIAC_FB_LLI_1_VALUE,
    TRIAC_FB_LLI_NOMINAL_GRID_FREQ,
    TRIAC_FB_LLI_DOUBLE_GRID_FREQ,
    TRIAC_UNKNOWN_UNKNOWN_VALUE,
}TRIAC_FEEDBACK_LLI_OUTPUT_TYPE;

#define TRIACFEEDBACK_LLI_REGISTER          TriacFeedbackLLI__Initialize,        \
                                            TriacFeedbackLLI__Allocate,          \
                                            TriacFeedbackLLI__InitializeInstance,\
                                            TriacFeedbackLLI__Get,               \
                                            TriacFeedbackLLI__AsynchProcess,     \
                                            NULL,                                \
                                            NULL,                                \
                                            TriacFeedbackLLI__OneMsProcess,      \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TriacFeedbackLLI__Initialize(void);
uint8 TriacFeedbackLLI__Allocate(void);
PASS_FAIL_TYPE TriacFeedbackLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * TriacFeedbackLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE TriacFeedbackLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE TriacFeedbackLLI__OneMsProcess(HBL_LLI_TYPE * input);

#endif
#endif // TRIACFEEDBACKLLI_H


