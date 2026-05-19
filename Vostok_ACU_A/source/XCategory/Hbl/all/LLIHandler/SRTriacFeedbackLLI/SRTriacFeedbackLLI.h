/**
 *  @file       SRTriacFeedbackLLI.h
 *
 *  @brief      Inteface  for SRTriacFeedbackLLI reading - detects a Nominal/Double grid frequency or a Digital level
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *	@defgroup	CLASS_B
 *
 *  @copyright  Copyright 2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRTRIACFEEDBACKLLI_H
	#define SRTRIACFEEDBACKLLI_H
#include "C_Extensions.h"
#include "SRSystemConfig.h"

#ifndef HBL_LLI_NUM_SRTRIACFEEDBACK
    #define HBL_LLI_NUM_SRTRIACFEEDBACK    0
#endif

#if (HBL_LLI_NUM_SRTRIACFEEDBACK > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum SRTRIAC_FEEDBACK_LLI_OUTPUT_ENUM
{
    SRTRIAC_FB_LLI_0_VALUE = 0,
    SRTRIAC_FB_LLI_1_VALUE,
    SRTRIAC_FB_LLI_NOMINAL_GRID_FREQ,
    SRTRIAC_FB_LLI_DOUBLE_GRID_FREQ,
    SRTRIAC_UNKNOWN_UNKNOWN_VALUE,
}SRTRIAC_FEEDBACK_LLI_OUTPUT_TYPE;

#define SRTRIACFEEDBACK_LLI_REGISTER        SRTriacFeedbackLLI__Initialize,        \
                                            SRTriacFeedbackLLI__Allocate,          \
                                            SRTriacFeedbackLLI__InitializeInstance,\
                                            SRTriacFeedbackLLI__Get,               \
                                            SRTriacFeedbackLLI__AsynchProcess,     \
                                            NULL,                                  \
                                            NULL,                                  \
                                            SRTriacFeedbackLLI__OneMsProcess,      \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRTriacFeedbackLLI__Initialize(void);
uint8 SRTriacFeedbackLLI__Allocate(void);
void SRTriacFeedbackLLI__Handler(void);
PASS_FAIL_TYPE SRTriacFeedbackLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * SRTriacFeedbackLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRTriacFeedbackLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE SRTriacFeedbackLLI__OneMsProcess(HBL_LLI_TYPE * input);

#endif
#endif // SRTRIACFEEDBACKLLI_H


