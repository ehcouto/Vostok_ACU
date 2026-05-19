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
#ifndef MCIZEROCROSS_H_
#define MCIZEROCROSS_H_

#include "MclTypes.h"
#include "TimeCapture.h"

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
void MclZeroCross__Initialize(MCL_ZERO_CROSS_IO_TYPE *io, MCL_PARAMS_TYPE* params);
void MclZeroCross__FastHandler(MCL_ZERO_CROSS_IO_TYPE *io);
void MclZeroCross__ZeroCrossHandler(MCL_ZERO_CROSS_IO_TYPE *io, TC_EVENT_TYPE event, uint16 ccr_value);
void MclZeroCross__EventHandler(MCL_ZERO_CROSS_IO_TYPE *io);
void MclZeroCross__SemiPeriodFilter(MCL_ZERO_CROSS_IO_TYPE *io, MCL_PARAMS_TYPE* params);
uint16 MclZeroCross__SemiPeriodPercent(uint16 percent);

#define MCISLFAC_PERCENT_OF_SEMI_PERIOD(p)          (uint16)MclZeroCross__SemiPeriodPercent(p)

#endif // MCIZEROCROSS_H_
