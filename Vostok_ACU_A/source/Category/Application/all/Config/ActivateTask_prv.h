/**
 * @file
 * @brief       Private parameters for the ActivateTask module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * $Header: ActivateTask_prv.h 1.1 2015/08/10 16:20:00EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 * @copyright  Copyright 2015 - $Date: 2015/08/10 16:20:00EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef ACTIVATETASK_PRV_H_
#define ACTIVATETASK_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Task.h"
#include "DoorOpeningSystemPull.h"
#include "DoorOpeningSystemPush.h"
#include "HeaterShadowing.h"
#include "MoisturePrevention.h"
#include "OwiVroContinuousReading.h"
#include "OWIContinousReading.h"
#include "Sanitization.h"
#include "StabilityControl.h"
#include "TimeElapse.h"

//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Flash-Based Constant Definitions --

//! List of task interfaces.
//! For each task, there must be three function pointers: Initialization, Run, and Stop.
//! If the task does not require an initialization function, use TASK_NULL_INIT.
//! If the task does not require a run function, use TASK_NULL_RUN.
//! If the task does not require a stop function, use TASK_NULL_STOP.
//! Add the tasks in the Sequence as they are defined in the requirements
/**
 * NOTE : PLEASE CHANGE THIS COMMENTS SECTION IF THERE IS A UPDATE IN THE DOCUMENT
 *
 *  0 --> Time Elapse
 *  1 --> Sanitization
 *  2 --> Moisture Prevention
 *  3 --> OWI Continous Reading
 *  4 --> Stability Control
 *  5 --> Heater Shadowing
 *  6 --> DOS PUSH
 *  7 --> DOS PULL
 *  8 --> OWI VRO Continuous Reading

 */
const TASK_INTERFACE_TYPE TASK_LIST[] =
{
    // Init Function,                    Recover Function,                     Run Function,                  Stop Function                      On Pause                         On Subcycle
    { TimeElapse__Initialize,            TimeElapse__NVDataRestore,            TimeElapse__Task,              TimeElapse__Terminate,             TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE	},
    { Sanitization__Initialize,          Sanitization__NVDataRestore,          Sanitization__Task,            Sanitization__Terminate,           TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE	},
    { MoisturePrevention__Initialize,    MoisturePrevention__NVDataRestore,    MoisturePrevention__Task,      MoisturePrevention__Terminate,     TASK_OPTION_PAUSE_RESUME_BG_FUN, TASK_OPTION_NO_CHANGE  	},
	{ OWIContinousReading__Initialize,   OWIContinousReading__NVDataRestore,   OWIContinousReading__Task,     OWIContinousReading__Terminate,    TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE  	},
	{ StabilityControl__Initialize,      StabilityControl__NVDataRestore,      StabilityControl__Task,        StabilityControl__Terminate,       TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE  	},
	{ HeaterShadowing__Initialize,       HeaterShadowing__NVDataRestore,       HeaterShadowing__Task,         HeaterShadowing__Terminate,        TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE  	},
	{ DoorOpeningSystemPush__Initialize, DoorOpeningSystemPush__NVDataRestore, DoorOpeningSystemPush__Task,   DoorOpeningSystemPush__Terminate,  TASK_OPTION_TERMINATE,   		TASK_OPTION_NO_CHANGE  	},
	{ DoorOpeningSystemPull__Initialize, DoorOpeningSystemPull__NVDataRestore, DoorOpeningSystemPull__Task,   DoorOpeningSystemPull__Terminate,  TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE  	},
	{ TASK_NULL_INIT,                    TASK_NULL_RECOVER,                    OwiVroContinuousReading__Task, TASK_NULL_STOP,                    TASK_OPTION_NO_CHANGE,   		TASK_OPTION_NO_CHANGE  	},
};

//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* ACTIVATETASK_PRV_H_ */
