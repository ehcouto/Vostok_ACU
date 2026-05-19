/**
 * @file
 * @brief       Public interface to the Maintain module.
 *
 * @details     Refer to the Maintain.c source file for more detailed information.
 *
 *  @copyright  Copyright 2015-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef MAINTAIN_H_
#define MAINTAIN_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Process.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void Maintain__Initialize(void);
void Maintain__PauseEntry(uint8 compartment);
PROCESS_RESULT_TYPE Maintain__Process(PROCESS_CONTROL_TYPE* control);


#endif      /* MAINTAIN_H_ */
