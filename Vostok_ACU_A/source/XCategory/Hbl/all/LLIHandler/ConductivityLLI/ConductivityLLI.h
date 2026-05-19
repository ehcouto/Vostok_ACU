/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: ConductivityLLI.h 1.7 2015/08/10 11:59:22EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/10 11:59:22EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CONDUCTIVITYLLI_H_
#define CONDUCTIVITYLLI_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_CONDUCTIVITY
    #define HBL_LLI_NUM_CONDUCTIVITY    0
#endif

#if (HBL_LLI_NUM_CONDUCTIVITY > 0)
#include "HblDefs.h"
#include "Filter.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
PACKED typedef struct
{
    uint16 Average_Duty;
    uint16 Maximum_Duty;
    uint8 Valid_Sample_Count;
    uint8 InValid_Sample_Count;
    uint8 Threshold_Sample_Count;
    uint8 Reading_Sync_Count;           //This count is used to synchronize the reading

    //This values are used for the instantaneous reading
    IIR_FILTER_MEASURE_TYPE Private_Duty;
    uint16 Private_Maximum_Duty;
    uint8 Private_Valid_Sample_Count;
    uint8 Private_Invalid_Sample_Count;
    uint8 Private_Threshold_Sample_Count;
    uint8 Private_Current_Count;       //This is used for internal sampling count only
} CONDUCTIVITY_DATA_TYPE;

#define CONDUCTIVITY_LLI_REGISTER           ConductivityLLI__Initialize,        \
                                            ConductivityLLI__Allocate,          \
                                            ConductivityLLI__InitializeInstance,\
                                            ConductivityLLI__Get,               \
                                            NULL,                               \
                                            NULL,                               \
                                            ConductivityLLI__ICProcess,         \
                                            NULL,                               \
                                            4
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ConductivityLLI__Initialize(void);
uint8 ConductivityLLI__Allocate(void);
PASS_FAIL_TYPE ConductivityLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * ConductivityLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE ConductivityLLI__ICProcess(HBL_LLI_TYPE * input);

#endif
#endif // CONDUCTIVITYLLI_H_


