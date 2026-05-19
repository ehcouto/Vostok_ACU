/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: ProbeInput.h 1.6 2023/03/20 11:27:57EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:27:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef PROBEINPUT_H_
#define PROBEINPUT_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_GI_NUM_PROBE
    #define HBL_GI_NUM_PROBE    0
#endif

#if (HBL_GI_NUM_PROBE > 0)
#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================




#define PROBE_GI_REGISTER                   ProbeInput__Initialize,      \
                                            ProbeInput__Allocate,       \
                                            ProbeInput_InitializeInstance,\
                                            ProbeInput__Get,            \
                                            ProbeInput__AsynchProcess,  \
                                            ProbeInput__GetConverted,  \
                                            2

#define PROBE_GI_FLOAT_REGISTER             ProbeInput__Initialize,      \
                                            ProbeInput__Allocate,       \
                                            ProbeInput_InitializeInstance,\
                                            ProbeInput__GetFloat,       \
                                            ProbeInput__AsynchProcess,  \
                                            ProbeInput__GetConverted,  \
                                            4

#define PROBE_GI_Q15TOFLOAT_REGISTER        ProbeInput__Initialize,      \
                                            ProbeInput__Allocate,       \
                                            ProbeInput_InitializeInstance,\
                                            ProbeInput__GetFloatFromQ15Converted,       \
                                            ProbeInput__AsynchProcess,  \
                                            ProbeInput__GetConverted,  \
                                            4

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void ProbeInput__Initialize(void);
uint8 ProbeInput__Allocate(void);
PASS_FAIL_TYPE ProbeInput_InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence);
void * ProbeInput__Get(HBL_GI_TYPE * generic_input,void * lli_data);
BOOL_TYPE ProbeInput__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence);
sint32 ProbeInput__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data);
void * ProbeInput__GetFloat (HBL_GI_TYPE * generic_input,void * lli_data);
void * ProbeInput__GetFloatFromQ15Converted(HBL_GI_TYPE * generic_input,void * lli_data);

#endif

#endif // PROBEINPUT_H_


