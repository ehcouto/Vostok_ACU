/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef STACKANALYSIS_H_
#define STACKANALYSIS_H_

#include "SystemConfig.h"

#ifndef STACK_ANALYSIS
    #define STACK_ANALYSIS              ENABLED
#endif
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct STACKANALYSIS_STACK_DATA_STRUCT
{
    //Stack Related
    uint16 Total_Size;
    uint16 Free_Space;
} STACKANALYSIS_STACK_DATA_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
STACKANALYSIS_STACK_DATA_TYPE StackAnalysis__GetStackInformation(STACK_ID_TYPE stack_index);

#endif // STACKANALYSIS_H_


