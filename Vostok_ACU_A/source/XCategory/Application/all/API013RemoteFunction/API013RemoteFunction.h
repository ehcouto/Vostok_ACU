/**
 * @file
 * @brief       Public interface to the API013RemoteFunction module.
 *
 * @details     Refer to the API013RemoteFunction.c source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API013REMOTEFUNCTION_H_
#define API013REMOTEFUNCTION_H_

#include "SystemConfig.h"

#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "API013RemoteFunction_prm.h"

// -- Other Modules --
#include "Reveal.h"
#include "Callback.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! A constant to hole the API number for this module.
#define API013RMTFCN_NUM 13
#define API013RMTFCN_TYPE 1
#define API013RMTFCN_VERSION 2
#define API013RMTFCN_INSTANCES 1

//! Define a NULL value for the API013_EVENT_HANDLER_TYPE.
#define API013_EVENT_HANDLER_NULL ((API013_EVENT_HANDLER_TYPE)0)


// -- Public Function Type Declarations --


#if (API013_CLIENT_FEATURE == ENABLED)
    //! Function to process an event message.
    typedef CALLBACK_HANDLER_TYPE API013_EVENT_HANDLER_TYPE;
    //! A structure that uniquely identifies a client function in a system.
    //! This data type is used in the API013RemoteFunction_prv.h file.
    typedef struct API013_FUNCTION_CONFIG_STRUCT
    {
        API013_SERVER_FUNCTION_ID_TYPE Server_Function_Id;  //!< The Function Id of the remote function on the server.
        uint8 Communication_Node;                           //!< The communication node of the server.
    } API013_FUNCTION_CONFIG_TYPE;
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)


// -- Public Structure Type Declarations --

#if (API013_CLIENT_FEATURE == ENABLED)
    //! The structure used to describe a received status element.
    typedef struct API013_FUNCTION_STATUS_STRUCT
    {
        API013_CLIENT_FUNCTION_INDEX_TYPE Client_Function_Index;
        uint8 Enabled;
        uint8* Status;
        uint8 Size;
    } API013_FUNCTION_STATUS_TYPE;
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void API013RemoteFunction__Initialize(void);
void API013RemoteFunction__Background(void);
REVEAL_RECIPE_STATUS_TYPE API013RemoteFunction__CommandParser(REVEAL_MSG_TYPE* cmd);
void API013RemoteFunction__FeedbackParser(REVEAL_MSG_TYPE* fbk);

#if (API013_CLIENT_FEATURE == ENABLED)
BOOL_TYPE API013RemoteFunction__SendControl(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 control, uint8* params, uint8 size);
void API013RemoteFunction__RequestStatus(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index);
const API013_FUNCTION_STATUS_TYPE* API013RemoteFunction__GetReceivedStatus(void);
BOOL_TYPE API013RemoteFunction__GetData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, sint16* value);
BOOL_TYPE API013RemoteFunction__GetSynchronizedData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, uint8* sync, uint16* value);
void API013RemoteFunction__SetPublicationPeriod(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint16 milliseconds);
void API013RemoteFunction__SynchronizeData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, uint8* sync);
void API013RemoteFunction__RegisterForEvents(API013_EVENT_HANDLER_TYPE event_handler);
void API013RemoteFunction__UnregisterForEvents(API013_EVENT_HANDLER_TYPE event_handler);
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)

#if (API013_SERVER_FEATURE == ENABLED)
void API013RemoteFunction__PublishStatus(uint8 server_function_id, uint8* status, uint8 size);
#endif      // #if (API013_SERVER_FEATURE == ENABLED)

#endif      // #if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))

#endif      /* API013MOTIONCTRL_H_ */
