/**
 *  @file       
 *
 *  @brief      AnalogLowInput API
 *
 *  @defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: AnalogLowInput.h 1.8 2015/08/06 17:18:39EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/06 17:18:39EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRANALOGLLI_H
	#define SRANALOGLLI_H
#include "C_Extensions.h"
#include "SRSystemConfig.h"

#ifndef HBL_LLI_NUM_SRANALOG
    #define HBL_LLI_NUM_SRANALOG  0
#endif

#if (HBL_LLI_NUM_SRANALOG > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define SRANALOG_LLI_PEAK_REGISTER       	    SRAnalogLLI__Initialize,             \
                                                SRAnalogLLI__Allocate,           	 \
                                                SRAnalogLLI__InitializeInstance,     \
                                                SRAnalogLLI__Get,                    \
                                                NULL,                                \
                                                SRAnalogLLI__PeakProcess,            \
                                                NULL,                                \
                                                NULL,                                \
                                                sizeof(uint16)

#define SRANALOG_LLI_PEAKTOPEAK_REGISTER        SRAnalogLLI__Initialize,             \
                                                SRAnalogLLI__Allocate,               \
                                                SRAnalogLLI__InitializeInstance,     \
                                                SRAnalogLLI__Get,                    \
                                                NULL,                                \
                                                SRAnalogLLI__PeakToPeakProcess,      \
                                                NULL,                                \
                                                NULL,                                \
                                                sizeof(uint16)

#define SRANALOG_LLI_ASYNCH_REGISTER            SRAnalogLLI__Initialize,        	 \
                                                SRAnalogLLI__Allocate,               \
                                                SRAnalogLLI__InitializeInstance,     \
                                                SRAnalogLLI__Get,                    \
                                                SRAnalogLLI__AsynchProcess,          \
                                                NULL,                                \
                                                NULL,                                \
                                                NULL,                                \
                                                sizeof(uint16)

#define SRANALOG_LLI_ONEMS_PEAKTOPEAK_REGISTER  SRAnalogLLI__Initialize,             \
                                                SRAnalogLLI__Allocate,               \
                                                SRAnalogLLI__InitializeInstance,     \
                                                SRAnalogLLI__Get,                    \
                                                NULL,                                \
                                                NULL,                                \
                                                NULL,                                \
                                                SRAnalogLLI__OneMsPeakToPeakProcess, \
                                                sizeof(uint16)

#define SRANALOG_LLI_ONEMS_PEAK_REGISTER        SRAnalogLLI__Initialize,             \
                                                SRAnalogLLI__Allocate,               \
                                                SRAnalogLLI__InitializeInstance,     \
                                                SRAnalogLLI__Get,                    \
                                                NULL,                                \
                                                NULL,                                \
                                                NULL,                                \
                                                SRAnalogLLI__OneMsPeakProcess,       \
                                                sizeof(uint16)

// For backward compatibility
#define SRANALOG_LLI_ONEMS_REGISTER             SRANALOG_LLI_ONEMS_PEAKTOPEAK_REGISTER

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRAnalogLLI__Initialize(void);
uint8 SRAnalogLLI__Allocate(void);
PASS_FAIL_TYPE SRAnalogLLI__InitializeInstance(HBL_LLI_TYPE * input);
void SRAnalogLLI__Handler(void);
void * SRAnalogLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRAnalogLLI__PeakProcess(HBL_LLI_TYPE * input,BOOL_TYPE peak_polarity);
BOOL_TYPE SRAnalogLLI__PeakToPeakProcess(HBL_LLI_TYPE * input,BOOL_TYPE peak_polarity);
BOOL_TYPE SRAnalogLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE SRAnalogLLI__OneMsPeakProcess(HBL_LLI_TYPE * input);
BOOL_TYPE SRAnalogLLI__OneMsPeakToPeakProcess(HBL_LLI_TYPE * input);
#endif
#endif // SRANALOGLLI_H


