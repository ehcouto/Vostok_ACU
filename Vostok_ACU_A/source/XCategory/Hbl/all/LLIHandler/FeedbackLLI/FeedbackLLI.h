/**
 *  @file       
 *
 *  @brief      FeedbackLLI API
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/06 17:18:41EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FEEDBACKLLI_H_
#define FEEDBACKLLI_H_
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_FEEDBACK
    #define HBL_LLI_NUM_FEEDBACK     0
#endif

#if (HBL_LLI_NUM_FEEDBACK > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    FEEDBACKLLI_LOW        = 0,
    FEEDBACKLLI_HIGH       = 1,
    FEEDBACKLLI_OPP_PHASE  = 2,
    FEEDBACKLLI_PHASE      = 3,
    FEEDBACKLLI_INVALID    = 255,
} FEEDBACKLLI_VALUE_ENUM_TYPE;


//---------------------------------------------------------------------------------------------------------------------
// Register defines that perform FeedbackLLI reading on OneMsProcess (1ms) and process it on AsynchProcess

#define FEEDBACK_LLI_REGISTER                   FeedbackLLI__Initialize,           \
                                                FeedbackLLI__Allocate,             \
                                                FeedbackLLI__InitializeInstance,   \
												FeedbackLLI__Get,                  \
                                                FeedbackLLI__AsynchProcess,        \
                                                NULL,                              \
                                                NULL,                              \
                                                FeedbackLLI__OneMsProcess,         \
                                                1

#define AC_LLI_REGISTER                         FeedbackLLI__Initialize,           \
                                                FeedbackLLI__Allocate,             \
                                                FeedbackLLI__InitializeInstance,   \
                                                FeedbackLLI__GetAcInput,           \
                                                FeedbackLLI__AsynchProcess,        \
                                                NULL,                              \
                                                NULL,                              \
                                                FeedbackLLI__OneMsProcess,         \
                                                1

//---------------------------------------------------------------------------------------------------------------------
// Register for ChargepumpLLI

#define CHARGEPUMP_LLI_REGISTER             FEEDBACK_LLI_REGISTER

//---------------------------------------------------------------------------------------------------------------------
// Register defines redirected for Backward compatibility

#define FEEDBACK_LLI_PEAK_REGISTER            FEEDBACK_LLI_REGISTER
#define FEEDBACK_LLI_ASYNCH_REGISTER          FEEDBACK_LLI_REGISTER
#define AC_LLI_PEAK_REGISTER                  AC_LLI_REGISTER
#define AC_LLI_ASYNCH_REGISTER                AC_LLI_REGISTER


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void FeedbackLLI__Initialize(void);
uint8 FeedbackLLI__Allocate(void);
PASS_FAIL_TYPE FeedbackLLI__InitializeInstance(HBL_LLI_TYPE * input);
BOOL_TYPE FeedbackLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE FeedbackLLI__OneMsProcess(HBL_LLI_TYPE * input);
void * FeedbackLLI__Get(HBL_LLI_TYPE * input);
void * FeedbackLLI__GetAcInput(HBL_LLI_TYPE * input);

#endif

#endif // FEEDBACKLLI_H_
