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
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef RINSEAIDDISPENSE_H_
#define RINSEAIDDISPENSE_H_

#include "Process.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void RinseAid__Initialize(void);
PROCESS_RESULT_TYPE RinseAid__Manager(PROCESS_CONTROL_TYPE* control);

#endif // RINSEAIDDISPENSE_H_


