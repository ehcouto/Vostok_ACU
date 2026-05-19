/**
 *  @file
 *
 *  @brief      Public declarations used by all process modules.
 *
 *  @copyright  Copyright 2015-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef PROCESS_H_
#define PROCESS_H_
//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

// -- Other Modules --

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
//! The data structure provided by the Interpreter to run a process.
typedef struct PROCESS_CONTROL_STRUCT
{
    uint16 Data_Field_Index;                        //!< The displacement for the data field used by the process.
    uint8 Setting_File_Index;                       //!< The setting file that the process should use.
    uint8 Compartment;                              //!< The appliance compartment in which the process is running.
    uint8 Process_Instance;                         //!< The instance of the process within the same step.
    BOOL_TYPE Step_Initializing     : 1;            //!< TRUE = This is the first call to this process on this step.
    BOOL_TYPE Resume_From_Pause     : 1;            //!< TRUE = Step is resuming after a cycle pause or a sub-cycle.
    BOOL_TYPE Resume_From_Reset     : 1;            //!< TRUE = Step is resuming after a power reset.
    BOOL_TYPE Sub_Cycle_Running     : 1;            //!< TRUE = Interpreter is currently running a sub-cycle.
    uint8 F1_F0_Flags               : 2;            //!< Process-specific configuration flag.
} PROCESS_CONTROL_TYPE;

//! The result message passed from the process to the Interpreter.
typedef enum PROCESS_RESULT_ENUM
{
    PROCESS_COMPLETE    = 0,                        //!< The process is ready for the Interpreter to move to the next step.
    PROCESS_BUSY        = 1                         //!< The process wishes to remain in the current step.
} PROCESS_RESULT_TYPE;

//! Declare a type for the process function that executes the process behavior.
typedef PROCESS_RESULT_TYPE (*PROCESS_FUNCTION_TYPE)(PROCESS_CONTROL_TYPE* control);

//! NULL process definition
#define NULL_PROCESS            ((PROCESS_FUNCTION_TYPE)(0))

//! Declare a type for the process initialization function.
typedef void (*PROCESS_INITIALIZE_FUNCTION_TYPE)(void);

//! NULL process initialize definition
#define NULL_PROCESS_INITIALZE  ((PROCESS_INITIALIZE_FUNCTION_TYPE)(0))

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

#endif      /* PROCESS_H_ */
