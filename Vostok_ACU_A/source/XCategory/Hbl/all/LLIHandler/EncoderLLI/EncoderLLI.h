/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: DigitalLowInput.h 1.7 2016/03/16 17:18:38EDT G. Ottaviani (OTTAVG) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/03/16 17:18:38EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef ENCODERLLI_H_
#define ENCODERLLI_H_

#ifndef HBL_LLI_NUM_ENCODER
    #define HBL_LLI_NUM_ENCODER 0
#endif

#if (HBL_LLI_NUM_ENCODER > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
	uint8 Rotation	:1;
    uint8 Pending	:1;
    uint8 dummy		:6;
}ENCODER_DATA_TYPE;


#define ENCODER_LLI_REGISTER                EncoderLLI__Initialize,                 \
                                            EncoderLLI__Allocate,                   \
                                            EncoderLLI__InitializeInstance,         \
                                            EncoderLLI__Get,                        \
                                            EncoderLLI__AsynchProcess,              \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void EncoderLLI__Initialize(void);
uint8 EncoderLLI__Allocate(void);
PASS_FAIL_TYPE EncoderLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * EncoderLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE EncoderLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif
#endif // ENCODERLLI_H_


