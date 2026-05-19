/**
 * @file
 * @brief       Public parameters to the API013MotionCtrl module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API013REMOTEFUNCTION_PRM_H_
#define API013REMOTEFUNCTION_PRM_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Enumerated Constant Declarations --

//! An enumerated list of all client function names in the appliance system.
//! This list has a one-to-one correspondence with the API013_CLIENT_FUNCTIONS[] array.
#if (API013_CLIENT_FEATURE == ENABLED)
typedef enum API013_CLIENT_FUNCTION_INDEX_ENUM
{
	CLIENT_REMOTE_FUNCTION_READ_TAG,
	CLIENT_REMOTE_FUNCTION_WRITE_TAG,
	CLIENT_REMOTE_FUNCTION_WRITE_DEFAULT_TAG,
	CLIENT_REMOTE_FUNCTION_GET_CARTRIDGE_STATUS,
	CLIENT_REMOTE_FUNCTION_CONSTANT_TRANSMIT
} API013_CLIENT_FUNCTION_INDEX_TYPE;
#endif// (API013_CLIENT_FEATURE == ENABLED)

//! An enumerated list of all server function ID in the appliance system.
typedef enum API013_SERVER_FUNCTION_ID_ENUM
{
    SERVER_REMOTE_FUNCTION_READ_TAG,
	SERVER_REMOTE_FUNCTION_WRITE_TAG,
	SERVER_REMOTE_FUNCTION_WRITE_DEFAULT_TAG,
	SERVER_REMOTE_FUNCTION_GET_CARTRIDGE_STATUS,
	SERVER_REMOTE_FUNCTION_CONSTANT_TRANSMIT
} API013_SERVER_FUNCTION_ID_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* API013REMOTEFUNCTION_PRM_H_ */
