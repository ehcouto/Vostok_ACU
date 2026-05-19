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
#ifndef MCITRIACDRIVE_H_
#define MCITRIACDRIVE_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "MclTypes.h"
#include "TimeCapture.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclTriacDrive__Initialize(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params);
void MclTriacDrive__FastHandler(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params);
void MclTriacDrive__TriacFireHandler(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params, TC_EVENT_TYPE event);
void MclTriacDrive__ControlTriac(BOOL_TYPE state);

#endif // MCITRIACDRIVE_H_


