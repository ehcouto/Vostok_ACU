/**
 *  @file       
 *
 *  @brief      SRFeedbackLLI API
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *	@defgroup	CLASS_B
 *
 *  $Header: FeedbackLowInput.h 1.10 2015/08/06 17:18:41EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/06 17:18:41EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRFEEDBACKLLI_H_
#define SRFEEDBACKLLI_H_

#include "SRSystemConfig.h"

#ifndef HBL_LLI_NUM_SRFEEDBACK
    #define HBL_LLI_NUM_SRFEEDBACK     0
#endif

#if (HBL_LLI_NUM_SRFEEDBACK > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    SRFEEDBACKLLI_LOW        = 0,
    SRFEEDBACKLLI_HIGH       = 1,
    SRFEEDBACKLLI_OPP_PHASE  = 2,
    SRFEEDBACKLLI_PHASE      = 3,
    SRFEEDBACKLLI_INVALID    = 255,
} SRFEEDBACKLLI_VALUE_ENUM_TYPE;

//---------------------------------------------------------------------------------------------------------------------

// Register defines that perform FeedbackLLI reading on OneMsProcess (1ms) and process it on AsynchProcess

#define SRFEEDBACK_LLI_REGISTER                 SRFeedbackLLI__Initialize,           \
                                                SRFeedbackLLI__Allocate,             \
                                                SRFeedbackLLI__InitializeInstance,   \
                                                SRFeedbackLLI__Get,                  \
                                                SRFeedbackLLI__AsynchProcess,        \
                                                NULL,                                \
                                                NULL,                                \
                                                SRFeedbackLLI__OneMsProcess,         \
                                                1

#define SRAC_LLI_REGISTER                       SRFeedbackLLI__Initialize,           \
                                                SRFeedbackLLI__Allocate,             \
                                                SRFeedbackLLI__InitializeInstance,   \
                                                SRFeedbackLLI__GetAcInput,           \
                                                SRFeedbackLLI__AsynchProcess,        \
                                                NULL,                                \
                                                NULL,                                \
                                                SRFeedbackLLI__OneMsProcess,         \
                                                1

//---------------------------------------------------------------------------------------------------------------------
// Register for SRChargepumpLLI

#define SRCHARGEPUMP_LLI_REGISTER           SRFEEDBACK_LLI_REGISTER

//---------------------------------------------------------------------------------------------------------------------
// Register defines redirected for Backward compatibility

#define SRFEEDBACK_LLI_PEAK_REGISTER        SRFEEDBACK_LLI_REGISTER
#define SRFEEDBACK_LLI_ASYNCH_REGISTER      SRFEEDBACK_LLI_REGISTER
#define SRAC_LLI_PEAK_REGISTER              SRAC_LLI_REGISTER
#define SRAC_LLI_ASYNCH_REGISTER            SRAC_LLI_REGISTER


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRFeedbackLLI__Initialize(void);
uint8 SRFeedbackLLI__Allocate(void);
void SRFeedbackLLI__Handler(void);
PASS_FAIL_TYPE SRFeedbackLLI__InitializeInstance(HBL_LLI_TYPE * input);
BOOL_TYPE SRFeedbackLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE SRFeedbackLLI__OneMsProcess(HBL_LLI_TYPE * input);
void * SRFeedbackLLI__Get(HBL_LLI_TYPE * input);
void * SRFeedbackLLI__GetAcInput(HBL_LLI_TYPE * input);

#endif

#endif // SRFEEDBACKLLI_H_
