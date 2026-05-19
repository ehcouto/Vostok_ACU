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
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCLINPUTPROC_H_
#define MCLINPUTPROC_H_

#include "MclTypes.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclInputProc__Initialize(MCL_INPUT_PROC_IO_TYPE* io, MCL_PARAMS_TYPE* params);
void MclInputProc__ResetState(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params);
void MclInputProc__RunningHandler(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params);
void MclInputProc__CurrentMonitor(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params);
#endif // MCLINPUTPROC_H_


