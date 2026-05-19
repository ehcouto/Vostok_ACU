/**
 * @file
 * @brief       Public interface to the SubCycle module.
 *
 * @details     Refer to the SubCycle.c source file for more detailed information.
 *
 * $Header: $
 *
 * @copyright  Copyright 2015 - $Date: $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SUBCYCLE_H_
#define SUBCYCLE_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void        SubCycle__Initialize    (void);
BOOL_TYPE   SubCycle__RequestStart  (uint8 compartment, uint8 priority);
uint8       SubCycle__GetActive     (uint8 compartment, uint8 current_sub_cycle_id);


#endif      /* SUBCYCLE_H_ */
