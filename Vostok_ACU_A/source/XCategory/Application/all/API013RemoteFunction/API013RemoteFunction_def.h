/**
 * @file
 * @brief       Public interface declarations for remote function modules.
 *
 * @details     Remote functions are implemented by individual modules and managed by API013RemoteFunction.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API013REMOTEFUNCTION_DEF_H_
#define API013REMOTEFUNCTION_DEF_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --

#include "RemoteFunctionInterface.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --
//! A structure that contains API function pointers for a background function.
typedef struct REMOTE_FUNCTION_HOOK_STRUCT
{
    REMOTE_FUNCTION_BACKGROUND_TYPE Background_Function;
    REMOTE_FUNCTION_DO_CONTROL_TYPE Do_Control_Function;
    REMOTE_FUNCTION_GET_DATA_TYPE   Get_Data_Function;
    REMOTE_FUNCTION_GET_STATUS_TYPE Get_Status_Function;
    API013_SERVER_FUNCTION_ID_TYPE  Server_Function_Id;
} REMOTE_FUNCTION_HOOK_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

#endif      /* API013REMOTEFUNCTION_DEF_H_ */
