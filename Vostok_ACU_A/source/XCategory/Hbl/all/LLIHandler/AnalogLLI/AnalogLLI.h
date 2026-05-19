/**
 *  @file       
 *
 *  @brief      AnalogLowInput API
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/06 17:18:39EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ANALOGLLI_H_
#define ANALOGLLI_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_ANALOG
    #define HBL_LLI_NUM_ANALOG  0
#endif

#if (HBL_LLI_NUM_ANALOG > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define ANALOG_LLI_PEAK_REGISTER       		    AnalogLLI__Initialize,         	\
                                                AnalogLLI__Allocate,           	\
                                                AnalogLLI__InitializeInstance, 	\
                                                AnalogLLI__Get,            		\
                                                NULL,                           \
                                                AnalogLLI__PeakProcess,    		\
                                                NULL,                           \
                                                NULL,                           \
                                                sizeof(uint16)

#define ANALOG_LLI_PEAKTOPEAK_REGISTER          AnalogLLI__Initialize,         	\
                                                AnalogLLI__Allocate,           	\
                                                AnalogLLI__InitializeInstance, 	\
                                                AnalogLLI__Get,            		\
                                                NULL,                           \
                                                AnalogLLI__PeakToPeakProcess,   \
                                                NULL,                           \
                                                NULL,                           \
                                                sizeof(uint16)

#define ANALOG_LLI_ASYNCH_REGISTER              AnalogLLI__Initialize,        	\
                                                AnalogLLI__Allocate,          	\
                                                AnalogLLI__InitializeInstance,  \
                                                AnalogLLI__Get,            		\
                                                AnalogLLI__AsynchProcess,       \
                                                NULL,                           \
                                                NULL,                           \
                                                NULL,                           \
                                                sizeof(uint16)

#define ANALOG_LLI_ONEMS_PEAKTOPEAK_REGISTER    AnalogLLI__Initialize,             \
                                                AnalogLLI__Allocate,               \
                                                AnalogLLI__InitializeInstance,     \
                                                AnalogLLI__Get,                    \
                                                NULL,                              \
                                                NULL,                              \
                                                NULL,                              \
                                                AnalogLLI__OneMsPeakToPeakProcess, \
                                                sizeof(uint16)

#define ANALOG_LLI_ONEMS_PEAK_REGISTER          AnalogLLI__Initialize,             \
                                                AnalogLLI__Allocate,               \
                                                AnalogLLI__InitializeInstance,     \
                                                AnalogLLI__Get,                    \
                                                NULL,                              \
                                                NULL,                              \
                                                NULL,                              \
                                                AnalogLLI__OneMsPeakProcess,       \
                                                2

// For backward compatibility
#define	ANALOG_LLI_REGISTER					    ANALOG_LLI_PEAK_REGISTER
#define ANALOG_LLI_ONEMS_REGISTER               ANALOG_LLI_ONEMS_PEAKTOPEAK_REGISTER

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void AnalogLLI__Initialize(void);
uint8 AnalogLLI__Allocate(void);
PASS_FAIL_TYPE AnalogLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * AnalogLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE AnalogLLI__PeakProcess(HBL_LLI_TYPE * input,BOOL_TYPE peak_polarity);
BOOL_TYPE AnalogLLI__PeakToPeakProcess(HBL_LLI_TYPE * input,BOOL_TYPE peak_polarity);
BOOL_TYPE AnalogLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE AnalogLLI__OneMsPeakProcess(HBL_LLI_TYPE * input);
BOOL_TYPE AnalogLLI__OneMsPeakToPeakProcess(HBL_LLI_TYPE * input);
#endif
#endif // ANALOGLLI_H_


