/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header:    PulseCounterLLI.h 1.9 2016/06/22 15:48:11EDT KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/06/22 15:48:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef PULSECOUNTERLLI_H
	#define PULSECOUNTERLLI_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_PULSECOUNTER
    #define HBL_LLI_NUM_PULSECOUNTER    0
#endif

#if (HBL_LLI_NUM_PULSECOUNTER > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define PULSECOUNTER_LLI_REGISTER           PulseCounterLLI__Initialize,        \
                                            PulseCounterLLI__Allocate,          \
                                            PulseCounterLLI__InitializeInstance,\
                                            PulseCounterLLI__GetCount,          \
                                            NULL,                               \
                                            NULL,                               \
                                            PulseCounterLLI__ICProcess,         \
                                            PulseCounterLLI__OneMsProcess,      \
                                            2


#define PULSEWIDTH_LLI_REGISTER             PulseCounterLLI__Initialize,        \
                                            PulseCounterLLI__Allocate,          \
                                            PulseCounterLLI__InitializeInstance,\
                                            PulseCounterLLI__GetWidth,          \
                                            NULL,                               \
                                            NULL,                               \
                                            PulseCounterLLI__ICProcess,         \
                                            PulseCounterLLI__OneMsProcess,      \
                                            2

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void PulseCounterLLI__Initialize(void);
uint8 PulseCounterLLI__Allocate(void);
PASS_FAIL_TYPE PulseCounterLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * PulseCounterLLI__GetCount(HBL_LLI_TYPE * input);
void * PulseCounterLLI__GetWidth(HBL_LLI_TYPE * input);
BOOL_TYPE PulseCounterLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE PulseCounterLLI__ICProcess(HBL_LLI_TYPE * input);
BOOL_TYPE PulseCounterLLI__OneMsProcess(HBL_LLI_TYPE * input);

#endif
#endif // PULSECOUNTERLLI_H

