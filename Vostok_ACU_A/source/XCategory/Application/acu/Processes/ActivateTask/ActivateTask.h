/**
 * @file
 * @brief       Public interface to the ActivateTask module.
 *
 * @details     Refer to the ActivateTask.c source file for more detailed information.
 *
 * $Header: ActivateTask.h 1.1 2015/08/10 14:10:56EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 * @copyright  Copyright 2015 - $Date: 2015/08/10 14:10:56EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef ACTIVATETASK_H_
#define ACTIVATETASK_H_
 

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

void ActivateTask__Initialize(void);
PROCESS_RESULT_TYPE ActivateTask__Process(PROCESS_CONTROL_TYPE* control);
void ActivateTask__PauseCompartment(uint8 compartment);
void ActivateTask__ResetCompartment(uint8 compartment);
void ActivateTask__ResumeCompartment(uint8 compartment);
void ActivateTask__RunCompartment(uint8 compartment);
void ActivateTask__SubCycleEnded(uint8 compartment);
void ActivateTask__SubCycleStarted(uint8 compartment);

#endif      /* ACTIVATETASK_H_ */
