/**
 * @file
 * @brief       Public interface to the Estimator module.
 *
 * @details     Refer to the Estimator.c source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef ESTIMATOR_H_
#define ESTIMATOR_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include <limits.h>


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! An estimation time that indicates no estimate.
#define NO_ESTIMATION (USHRT_MAX)


// -- Public Enumerated Constant Declarations --


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void Estimator__Initialize(void);
void Estimator__Background(void);
void Estimator__Countdown(uint8 compartment);
void Estimator__EstimateFast(void);
uint16 Estimator__GetBlockInitialEstimate(uint8 compartment);
uint16 Estimator__GetElapsedTimeInCurrentBlock(uint8 compartment);
uint16 Estimator__GetRemainingSeconds(uint8 compartment);


#endif      // ESTIMATOR_H_
