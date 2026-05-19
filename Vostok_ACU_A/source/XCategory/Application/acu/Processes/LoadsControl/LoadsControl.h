/**
 * @file
 * @brief       Public interface to the LoadsControl module.
 *
 * @details     Refer to the LoadsControl.c source file for more detailed information.
 *
 * @copyright   Copyright 2015-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef LOADSCONTROL_H_
#define LOADSCONTROL_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Process.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void LoadsControl__Initialize(void);
PROCESS_RESULT_TYPE LoadsControl__Process(PROCESS_CONTROL_TYPE* control);


#endif      /* LOADSCONTROL_H_ */
