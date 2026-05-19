/**
 * @file
 * @brief       Public declarations for support of Task modules.
 *
 * $Header: Task.h 1.1 2015/08/10 14:10:57EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 * @copyright  Copyright 2015 - $Date: 2015/08/10 14:10:57EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef TASK_H_
#define TASK_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

#define TASK_NULL_INIT      ((TASK_INIT_FUNCTION_TYPE)NULL)

#define TASK_NULL_RECOVER   ((TASK_RECOVER_FUNCTION_TYPE)NULL)

#define TASK_NULL_RUN       ((TASK_RUN_FUNCTION_TYPE)NULL)

#define TASK_NULL_STOP      ((TASK_STOP_FUNCTION_TYPE)NULL)

//! List of values to manage the event handled to the module.
typedef enum TASK_EVENT_ACTION_ENUM
{
    TASK_OPTION_NO_CHANGE = 0,
    TASK_OPTION_PAUSE_RESUME_BG_FUN,
    TASK_OPTION_TERMINATE
} TASK_EVENT_ACTION_TYPE;

// -- Public Type Declarations --

//! A function type for tasks to initialize and optionally start running.
typedef ACTIVE_TYPE (*TASK_INIT_FUNCTION_TYPE)		(uint8 compartment, uint8 sf_index, uint8 displacement);

//! A function type for tasks to recover after a micro reset.
typedef void 		(*TASK_RECOVER_FUNCTION_TYPE)	(uint8 compartment);

//! A function type for tasks to run their background functions while the cycle is running.
typedef ACTIVE_TYPE (*TASK_RUN_FUNCTION_TYPE)		(uint8 compartment);

//! A function type for tasks to terminate their activity as demanded by the cycle.
typedef void 		(*TASK_STOP_FUNCTION_TYPE)		(uint8 compartment);

//! Declaration of the interfaces required for each task.
typedef struct TASK_INTERFACE_STRUCT
{
    TASK_INIT_FUNCTION_TYPE     Init;
    TASK_RECOVER_FUNCTION_TYPE  Recover;
    TASK_RUN_FUNCTION_TYPE      Run;
    TASK_STOP_FUNCTION_TYPE     Stop;
    TASK_EVENT_ACTION_TYPE      OnPause;
    TASK_EVENT_ACTION_TYPE      OnSubCycleStarted;
} TASK_INTERFACE_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* TASK_H_ */

