/**
 * @brief       Cross category Class A definitions for the API004Debug module.
 *
 * @details     This file contains macros to create the Signature information easily and the
 *              definition of the "Debug function" data type
 *
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API004DEBUG_DEFS_H_
#define API004DEBUG_DEFS_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
//! API's opcodes enumeration
typedef enum
{
    API004_CMD_EXECUTE_FUNCTION             = 1, //!< API004_CMD_EXECUTE_FUNCTION
    API004_FBK_CONFIRM_EXECUTION            = 1,//!< API004_FBK_CONFIRM_EXECUTION
    API004_CMD_REQUEST_FUNCTIONS_INFO       = 2,//!< API004_CMD_REQUEST_FUNCTIONS_INFO
    API004_FBK_FUNCTIONS_INFO               = 2,//!< API004_FBK_FUNCTIONS_INFO

    API004_CMD_EXECUTE_FIXED_ARGS_FUNCTIONS    = 3, //!< API004_CMD_EXECUTE_FIXED_ARGS_FUNCTIONS
    API004_FBK_EXECUTE_FIXED_ARGS_FUNCTIONS    = 3, //!< API004_FBK_EXECUTE_FIXED_ARGS_FUNCTIONS

    API004_CMD_LOG_MESSAGES_SEVERITY_FILTER = 14,//!< API004_CMD_LOG_MESSAGES_SEVERITY_FILTER
    API004_FBK_LOG_MESSAGES_SEVERITY_FILTER = 14,//!< API004_FBK_LOG_MESSAGES_SEVERITY_FILTER
    API004_FBK_PUBLISH_LOG_MESSAGES         = 15,//!< API004_FBK_PUBLISH_LOG_MESSAGES
}API004DEBUG_OPCODES_TYPE;


// -- Public Constant Declarations --
#define DATATYPE_VOID               DATATYPE_INVALID
#define ELEMENT_BITS                4
#define ELEMENT_MASK                ((1 << ELEMENT_BITS) - 1)
#define PARAM(pos, datatype)        (uint32)(((datatype) & ELEMENT_MASK) << (pos * ELEMENT_BITS))
#define PARAM1(datatype)            PARAM(1, datatype)
#define PARAM2(datatype)            PARAM(2, datatype)
#define PARAM3(datatype)            PARAM(3, datatype)
#define PARAM4(datatype)            PARAM(4, datatype)
#define PARAM5(datatype)            PARAM(5, datatype)
#define PARAM6(datatype)            PARAM(6, datatype)
#define PARAM7(datatype)            PARAM(7, datatype)
#define RETURNS(datatype)           PARAM(0, datatype)

// -- Public Enumerated Constant Declarations --


// -- Public Type Declarations --
typedef struct DEBUG_FUNCTION_STRUCT
{
    char* Name;
    void* Pointer;
    uint32 Signature;
} DEBUG_FUNCTION_TYPE;


/**
 * Fixed Arguments functions properties
 */
typedef void (*DEBUG_FIXED_ARGS_FUNCTION_HANDLER_TYPE)(uint8 source_node_id, uint8 buffer_length, uint8 *buffer_ptr);

typedef struct
{
    char* Name;
    DEBUG_FIXED_ARGS_FUNCTION_HANDLER_TYPE *Pointer;
} DEBUG_FIXED_ARGS_FUNCTION_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // API004DEBUG_DEFS_H_
