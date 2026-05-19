/**
 * @file
 * @brief       Private parameters for the Estimator module.
 *
 * @details     Refer to the Estimator.c source file for more detailed information.
 *
 * $Header: Estimator_prv.h 1.1 2015/08/14 11:03:46EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 * @copyright  Copyright 2015 - $Date: 2015/08/14 11:03:46EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef ESTIMATOR_PRV_H_
#define ESTIMATOR_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Process.h"
#include "Regulations.h"


//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Private Constant Declarations --

//! Number of steps to estimate in a single call for the normal estimation speed.
#define ESTIMATOR_NORMAL_STATEMENTS_PER_CALL 100

//! Number of steps to estimate in a single call for the fast estimation speed.
//! This speed is used in response to a selection change when the Supervisor is in the programming state.
#define ESTIMATOR_FAST_STATEMENTS_PER_CALL 1000


// -- Private Type Declarations --

//! A structure to associate an auto-accepted regulation to a set of compartments that must be re-estimated.
typedef struct AUTO_ACCEPT_REGULATION_STRUCT
{
    REGULATIONS_FUNCTION_ID_TYPE Regulation;
    uint8 Compartment_Mask;
} AUTO_ACCEPT_REGULATION_TYPE;


// -- Private Flash-Based Constant Definitions --

//! Array of automatically accepted regulations.
//! Each regulation has a set of compartments associated with it.
//! The associated compartments will have a fast estimation triggered when the regulation is accepted.
//static const AUTO_ACCEPT_REGULATION_TYPE AUTO_ACCEPT_REGULATIONS[] =
//{
//   REGULATIONS_FUNCITON_ID_TYPE,  Compartment Mask (bit 'n' for compartment 'n')
 //   {FUNC_ID_DRYNESS                ,  1},
//    {FUNC_ID_HEAT_LOW_TEMPERATURE   ,  1},
//    {FUNC_ID_TIMED_DRY              ,  1},
//    {FUNC_ID_POST_CARE              ,  1}
//};


#endif      /* ESTIMATOR_PRV_H_ */
