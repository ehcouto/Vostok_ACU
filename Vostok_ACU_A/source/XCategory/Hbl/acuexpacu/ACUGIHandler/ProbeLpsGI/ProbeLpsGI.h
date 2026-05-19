/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: ProbeInput.h 1.6 2015/07/24 11:27:57EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:27:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef PROBE_PSENS_LIN_INPUT_H_
#define PROBE_PSENS_LIN_INPUT_H_
#include "C_Extensions.h"
#include "SystemConfig.h"


#ifndef HBL_GI_NUM_PROBE_PSENSLIN
    #define HBL_GI_NUM_PROBE_PSENSLIN    1
#endif

#if (HBL_GI_NUM_PROBE_LPS > 0)
#include "HblCommon.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================




#define PROBE_LPS_GI_REGISTER                 		ProbeLpsGI__Initialize,      \
                                            		ProbeLpsGI__Allocate,       \
													ProbeLpsGI_InitializeInstance,\
													ProbeLpsGI__Get,            \
													ProbeLpsGI__AsynchProcess,  \
													ProbeLpsGI__GetConverted,  \
													2

#define OFFSET_RANGE_TICKS_VALUE					6
#define OFFSET_INPUT_ITEM							(2 + OFFSET_RANGE_TICKS_VALUE)
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void ProbeLpsGI__Initialize(void);
uint8 ProbeLpsGI__Allocate(void);
PASS_FAIL_TYPE ProbeLpsGI_InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence);
void * ProbeLpsGI__Get(HBL_GI_TYPE * generic_input,void * lli_data);
BOOL_TYPE ProbeLpsGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence);
sint32 ProbeLpsGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data);


#endif

#endif // PROBE_PSENS_LIN_INPUT_H_


