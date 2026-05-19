/**
 * @file
 * @brief       Public interface declarations for remote function modules.
 *
 * @details     Remote functions are implemented by individual modules and managed by API013RemoteFunction.
 *
 *              Remote functions must implement the REMOTE_FUNCTION_DO_CONTROL_TYPE function.
 *              All other functions are optional.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef REMOTEFUNCTIONINTERFACE_H_
#define REMOTEFUNCTIONINTERFACE_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Reveal.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

// Define NULL values for each type of function pointer.
#define REMOTE_FUNCTION_BACKGROUND_NULL ((REMOTE_FUNCTION_BACKGROUND_TYPE)0)
#define REMOTE_FUNCTION_DO_CONTROL_NULL ((REMOTE_FUNCTION_DO_CONTROL_TYPE)0)
#define REMOTE_FUNCTION_GET_DATA_NULL ((REMOTE_FUNCTION_GET_DATA_TYPE)0)
#define REMOTE_FUNCTION_GET_STATUS_NULL ((REMOTE_FUNCTION_GET_STATUS_TYPE)0)

//! The maximum number of elements that an function can generate.
#define API013_MAX_DATA_CHANNELS_PER_FUNCTION 16U


// -- Public Type Declarations --

//! The list of possible responses from the command function.
typedef enum REMOTE_FUNCTION_CONTROL_RESPONSE_ENUM
{
    REMOTE_FUNCTION_CONTROL_RESPONSE_INVALID = -1,
    REMOTE_FUNCTION_CONTROL_RESPONSE_INACTIVE = INACTIVE,
    REMOTE_FUNCTION_CONTROL_RESPONSE_ACTIVE = ACTIVE
} REMOTE_FUNCTION_CONTROL_RESPONSE_TYPE;

typedef enum REMOTE_FUNCTION_COMMAND_ENUM
{
    REMOTE_FUNCTION_COMMAND_DISABLE = 0,
    REMOTE_FUNCTION_COMMAND_ENABLE = 1
} REMOTE_FUNCTION_COMMAND_TYPE;

//! The structure used to describe a single data element.
typedef struct REMOTE_FUNCTION_DATA_STRUCT
{
    uint8 Channel_Id;
    uint8 Value_Hi_Byte;
    uint8 Value_Lo_Byte;
} REMOTE_FUNCTION_DATA_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

/**
 * Background Function API Function: Background function that is called periodically while the background function is active.
 * @return ACTIVE if the background function should be called periodically; otherwise INACTIVE.
 *                  If the background function is active, and if this function returns INACTIVE,
 *                  then the FUNCTION_GET_DATA_ELEMENTS API function will be called one more time.
 */
typedef ACTIVE_TYPE (*REMOTE_FUNCTION_BACKGROUND_TYPE) (void);

/**
 * Background function API Function: Execute a background function command.
 * @param command = The command that the background function should execute.
 *                  A command value of 0 indicates that the background function should disable itself.
 * @param params = A pointer to the parameter bytes that are being passed along with the command.
 *                  The params memory is only valid during the function call.
 * @param size = The number of parameter bytes that are being passed along with the command.
 * @retval REMOTE_FUNCTION_CONTROL_RESPONSE_INACTIVE = The function is inactive. Do not call the background function.
 * @retval REMOTE_FUNCTION_CONTROL_RESPONSE_ACTIVE = The function is active. Call the background function periodically.
 * @retval REMOTE_FUNCTION_CONTROL_RESPONSE_INVALID = The command is not supported.
 */
typedef REMOTE_FUNCTION_CONTROL_RESPONSE_TYPE (*REMOTE_FUNCTION_DO_CONTROL_TYPE) (uint8 command, uint8* params, uint8 size);

/**
 * Background Function API Function: Get the data elements associated with the background function.
 * @param first_data_element_id = The ID to use for data element #0. Data element #1 uses this value + 1, etc.
 * @param data_elements = Array of REMOTE_FUNCTION_DATA_TYPE structures. One for each data element.
 * @param count = The number of data elements that are being returned by this function.
 */
typedef void (*REMOTE_FUNCTION_GET_DATA_TYPE) (uint8 first_data_element_id, REMOTE_FUNCTION_DATA_TYPE* data_elements, uint8* count);

/**
 * Background Function API Function: Get the current status of the background function.
 * @param status = This function sets the referenced pointer to point to the buffer that holds the function status data.
 * @param size = This function sets the referenced byte to the number of bytes in the function status data.
 */
typedef void (*REMOTE_FUNCTION_GET_STATUS_TYPE) (uint8** status, uint8* size);

#endif      /* REMOTEFUNCTIONINTERFACE_H_ */
