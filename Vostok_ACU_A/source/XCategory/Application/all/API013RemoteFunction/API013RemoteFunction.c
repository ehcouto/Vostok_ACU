/**
 * @file
 * @brief       Communications to control application-layer remote functions.
 *
 * @details     The same API013RemoteFunction module is used on the client and the server.
 *                  Client Declarations:
 *                      API013_CLIENT_FEATURE should be ENABLED
 *                      API013_SERVER_FEATURE should be DISABLED
 *                  Server Declarations:
 *                      API013_CLIENT_FEATURE should be DISABLED
 *                      API013_SERVER_FEATURE should be ENABLED
 *              Using the same module on the client and server helps ensure that the client and
 *              server are using the same type and version of the API.
 *
 *              API013RemoteFunction allows each client to control one or more remote function
 *              servers that each controls one or more remote functions. For each of the remote
 *              functions, the API manages three types of data:
 *                  - Controls
 *                  - Background Function
 *                  - Status (that can be handled as events)
 *                  - Data Acquisition
 *
 *              Control (Required by each Remote Function):
 *              Each remote function must support the following standard controls:
 *                  - Control Command 0 = Disable the remote function
 *                  - Control Command 1 = Enable the remote function
 *              The remote functions can have additional application-specific control commands
 *              with command-specific parameters.
 *
 *              Background Function (Optional for each Remote Function):
 *              Each remote function may provide a background function that will only be called
 *              while the remote function is enabled.
 *
 *              Status / Events (Optional for each Remote Function):
 *              Each remote function may implement a status message. If implemented, the first
 *              status byte must indicate whether the remote function is enabled (1) or disabled
 *              (0). The remote function may have additional application-specific status
 *              information.
 *              A remote function can generate an event by publishing a status message that was
 *              not requested by the client.
 *
 *              Data Acquisition (Optional for each Remote Function):
 *              A client may setup periodic publication of server data. A server will publish all
 *              available data channels from all enabled servers at each publication.
 *
 * @copyright   Copyright 2016-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#ifndef API013_CLIENT_FEATURE
    // API013_CLIENT_FEATURE should be ENABLED for applications that control application-layer functions on remote boards.
    #error API013_CLIENT_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif
#ifndef API013_SERVER_FEATURE
    // API013_SERVER_FEATURE should be ENABLED for applications that provide application-layer functions through communications.
    #error API013_SERVER_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
// A single node cannot support both a client and a server
CT_ASSERT((API013_CLIENT_FEATURE != ENABLED) || (API013_SERVER_FEATURE != ENABLED));

#include "API013RemoteFunction.h"
#include "API013RemoteFunction_def.h"
#include "API013RemoteFunction_prv.h"
#include "RemoteFunctionInterface.h"

// -- Other Modules --
#if (API013_CLIENT_FEATURE == ENABLED)
    #include "Callback.h"
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)
#include <limits.h>
#include "Log.h"
#include "Micro.h"
#include <string.h>
#include "Utilities.h"


#if (API013_CLIENT_FEATURE == ENABLED)
    // The data element storage must be able to hold the maximum data elements transmitted in a single packet.
    CT_ASSERT(API013_MAX_STORED_DATA >= API013_MAX_PUBLISHED_DATA);
#endif

#if (API013_SERVER_FEATURE == ENABLED)
    CT_ASSERT((uint16)(API013_MAX_PUBLISHED_DATA * sizeof(REMOTE_FUNCTION_DATA_TYPE)) <= (uint16)REVEAL_PAYLOAD_SIZE);
#endif


//  --- Private Properties --------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

#if (API013_CLIENT_FEATURE == ENABLED)
    #define INVALID_CLIENT_INDEX  0xFFU
    #define INVALID_SERVER_ID     0xFFU
    //! The number of remote functions controlled by the client.
    #define API013_NUM_OF_CLIENT_FUNCTIONS ELEMENTS_IN_ARRAY(API013_CLIENT_FUNCTIONS)
#endif

#if (API013_SERVER_FEATURE == ENABLED)
    //! The number of remote functions controlled by the server.
    #define API013_NUM_OF_SERVER_FUNCTIONS ELEMENTS_IN_ARRAY(API013_SERVER_FUNCTIONS)

    //! The minimum period between publication of remote function data.
    #define MIN_PUBLICATION_PERIOD 100U
#endif


//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_API013_REMOTE_FUNCTION


// -- Private Enumerated Constant Declarations --

//! All the API013 command and feedback OpCodes.
typedef enum API013_OPCODE_ENUM
{
    API013_CMD_SEND_FUNCTION_CONTROL    = 1,//!< API013_CMD_SEND_FUNCTION_CONTROL
    API013_CMD_REQUEST_FUNCTION_STATUS  = 2,//!< API013_CMD_REQUEST_FUNCTION_STATUS
    API013_CMD_SET_PUBLICATION_PERIOD   = 3,//!< API013_CMD_SET_PUBLICATION_PERIOD

    API013_FBK_PUBLISH_FUNCTION_STATUS  = 2,//!< API013_FBK_PUBLISH_FUNCTION_STATUS
    API013_FBK_PUBLISH_FUNCTION_DATA    = 3 //!< API013_FBK_PUBLISH_FUNCTION_DATA
} API013_OPCODE_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_API013_REMOTE_FUNCTION_LOG_MESSAGE_ID_ENUM
{
    FEEDBACK_MESSAGE_NOT_HANDLED = 1
} MODULE_API013_REMOTE_FUNCTION_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

#if (API013_CLIENT_FEATURE == ENABLED)
    //! Variables used to manage the synchronization and freshness of remote function data.
    typedef struct API013_DATA_STRUCT
    {
        sint16 Value;
        API013_CLIENT_FUNCTION_INDEX_TYPE Client_Function_Index;
        uint8 Channel;
        uint8 Sequence;
    } API013_STORED_DATA_TYPE;

#endif      // #if (API013_CLIENT_FEATURE == ENABLED)

//! A structure to hold most of the API013 variables.
typedef struct API013_VARIABLES_STRUCT
{
    #if (API013_CLIENT_FEATURE == ENABLED)
        //! A structure that holds variables to manage the synchronization and freshness of the remote function data.
        API013_STORED_DATA_TYPE Data[API013_MAX_STORED_DATA];

        //! A place to store status
        API013_FUNCTION_STATUS_TYPE Received_Status;
    #endif

    #if (API013_SERVER_FEATURE == ENABLED)
        //! An array that contains a bit for each supported function that may be active.
        //! Bit N in the array indicates that the FUNCTION_IDS[N] function is active.
        uint8 Functions_Active[BIT_STORAGE_SIZE(API013_NUM_OF_SERVER_FUNCTIONS)];

        //! The node to which the remote function data will be sent.
        uint8 Data_Receiver_Node;

        //! The countdown variable for the time between data publications.
        uint8 Publication_Counter;

        //! The number of calls to the background function between each data publication.
        uint8 Publication_Period;
    #endif
} API013_VARIABLES_TYPE;


// -- Private Variable Definitions --

#if (API013_CLIENT_FEATURE == ENABLED)
    // Declare variables for the Callback module.
    DEFINE_CALLBACK_CONTROL_STRUCTURE(API013_Callback, API013_MAX_CALLBACK_HANDLERS);
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)

//! A structure that holds most of the API013 variables.
static API013_VARIABLES_TYPE Api013;

//  --- Private Function Prototypes -----------------------------------------------------------------------------------

#if (API013_CLIENT_FEATURE == ENABLED)
    static uint8 FindDataIndex(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel);
    static void StoreReceivedData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, sint16 value);
    static uint8 ConvertServerIdToClientIndex(uint8 node_id, uint8 server_function_id);
    static uint8 ConvertClientFunctionIndexToServerId(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index);
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)

#if (API013_SERVER_FEATURE == ENABLED)
    static uint8 ConvertServerFunctionIdToIndex(uint8 server_function_id);
    static void PublishAllRemoteFunctionData(void);
    static BOOL_TYPE PublishPreparedData(REMOTE_FUNCTION_DATA_TYPE* data_elements, uint8 count);
    static BOOL_TYPE PublishRemoteFunctionStatus(uint8 destination, uint8 server_function_id, uint8* status, uint8 size);
    static void PublishSingleRemoteFunctionData(uint8 server_function_index);
    static REMOTE_FUNCTION_CONTROL_RESPONSE_TYPE RemoteFunctionDoControl(uint8 server_function_id, uint8 control, uint8* params, uint8 size);
    static uint8 RemoteFunctionGetData(uint8 server_function_index, REMOTE_FUNCTION_DATA_TYPE* data_elements);
    static BOOL_TYPE RemoteFunctionGetStatus(uint8 server_function_id, uint8** status, uint8* size);
    static BOOL_TYPE SetPublicationPeriod(uint16 milliseconds);
#endif      // #if (API013_SERVER_FEATURE == ENABLED)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the API013RemoteFunction module variables.
 */
void API013RemoteFunction__Initialize(void)
{
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        #if (API013_SERVER_FEATURE == ENABLED)
            memset(&Api013, 0, sizeof(Api013));
            Api013.Data_Receiver_Node = WIN_ACU_ADDRESS;
        #endif      // #if (API013_SERVER_FEATURE == ENABLED)

        #if (API013_CLIENT_FEATURE == ENABLED)
            Callback__Initialize(&API013_Callback);
            #if (API013_SERVER_FEATURE == DISABLED)
                memset(&Api013.Received_Status, 0, sizeof(Api013.Received_Status));
            #endif
                memset(Api013.Data, UCHAR_MAX, sizeof(Api013.Data));
        #endif      // #if (API013_CLIENT_FEATURE == ENABLED)
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Background function for the API013RemoteFunction module.
 * Periodically publish data elements from any background function that have data elements ready.
 */
void API013RemoteFunction__Background(void)
{
    #if (API013_SERVER_FEATURE == ENABLED)
        uint8 index;
        REMOTE_FUNCTION_BACKGROUND_TYPE background;

        // Handle all active remote functions.
        for (index = 0; index < API013_NUM_OF_SERVER_FUNCTIONS; index++)
        {
            // Check if the remote function is active.
            if (Utilities__GetBitInArray(Api013.Functions_Active, index) == (uint8)ACTIVE)
            {
                // Check if the remote function has a background function.
                background = API013_SERVER_FUNCTIONS[index].Background_Function;
                if (background != REMOTE_FUNCTION_BACKGROUND_NULL)
                {
                    // Call the background function. Check if it self-terminated.
                    if (background() == INACTIVE)
                    {
                        PublishSingleRemoteFunctionData((uint8)index);
                        Utilities__SetBitInArray(Api013.Functions_Active, index, (uint8)INACTIVE);
                    }
                }
            }
        }

        // Periodically publish data from active remote functions.
        if (Api013.Publication_Period > 0U)
        {
            if (Api013.Publication_Counter > 0U)
            {
                Api013.Publication_Counter--;
            }
            if (Api013.Publication_Counter == 0U)
            {
                // Publish data for all active remote functions.
                PublishAllRemoteFunctionData();
                Api013.Publication_Counter = Api013.Publication_Period;
            }
        }
    #endif      // #if (API013_SERVER_FEATURE == ENABLED)
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API013 commands sent to a broadcast or exclusively to this node.
 * @param cmd = Pointer to the received command or feedback message.
 * @return REVEAL_RECIPE_STATUS_TYPE that expresses the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API013RemoteFunction__CommandParser(REVEAL_MSG_TYPE* cmd)
{
    #if (API013_SERVER_FEATURE == ENABLED)
        // React to the command message by OpCode.
        REVEAL_RECIPE_STATUS_TYPE reveal_response = NVO_RS_UNSUP_INVALID_PARAM;

        switch ((API013_OPCODE_TYPE)cmd->op_code)
        {
            case API013_CMD_SEND_FUNCTION_CONTROL:
                // Expected Payload:
                //  Byte 0 = Server-side remote function id
                //  Byte 1 = Control command code (0 = Disable, 1 = Enable, 2+ = function-specific command)
                //  Byte 2+ = Command-specific parameters
                if (cmd->size >= 2U)
                {
                    REMOTE_FUNCTION_CONTROL_RESPONSE_TYPE control_response = RemoteFunctionDoControl(cmd->payload[0], cmd->payload[1], &(cmd->payload[2]), (cmd->size - 2U));
                    if ((control_response == REMOTE_FUNCTION_CONTROL_RESPONSE_INACTIVE) ||
                        (control_response == REMOTE_FUNCTION_CONTROL_RESPONSE_ACTIVE))
                    {
                        Api013.Data_Receiver_Node = cmd->node_id;
                        reveal_response = NVO_RS_READY;
                    }
                }
                break;

            case API013_CMD_REQUEST_FUNCTION_STATUS:
                // Expected Payload:
                //  Byte 0 = Server-side remote function id
                if (cmd->size >= 1U)
                {
                    uint8 server_function_id = cmd->payload[0];
                    uint8* status;
                    uint8 status_size;
                    if (RemoteFunctionGetStatus(server_function_id, &status, &status_size) != FALSE)
                    {
                        if (PublishRemoteFunctionStatus(cmd->node_id, server_function_id, status, status_size) != FALSE)
                        {
                            Api013.Data_Receiver_Node = cmd->node_id;
                            reveal_response = NVO_RS_READY;
                        }
                    }
                }
                break;

            case API013_CMD_SET_PUBLICATION_PERIOD:
                // Expected Payload:
                //  Byte 0-1 = Big-endian millisecond period
                if (cmd->size >= 2U)
                {
                    uint16 milliseconds = Utilities__ConvertArrayTo16bits(cmd->payload);
                    if (SetPublicationPeriod(milliseconds) != FALSE)
                    {
                        Api013.Data_Receiver_Node = cmd->node_id;
                        reveal_response = NVO_RS_READY;
                    }
                }
                break;

            default:
                reveal_response = NVO_RS_UNSUP_OP_CODE;
                break;
        }

        return reveal_response;
    #else       // #if (API013_SERVER_FEATURE == ENABLED)
        return NVO_RS_UNSUP_OP_CODE;
    #endif      // #if (API013_SERVER_FEATURE == ENABLED)
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API013 feedbacks sent to a broadcast or exclusively to this node.
 * @param fbk = Pointer to the received acknowledged or not acknowledged message.
 */
void API013RemoteFunction__FeedbackParser(REVEAL_MSG_TYPE* fbk)
{
    #if (API013_CLIENT_FEATURE == ENABLED)
        switch ((API013_OPCODE_TYPE)fbk->op_code)
        {
            case API013_FBK_PUBLISH_FUNCTION_STATUS:
                // Expected Payload:
                //  Byte 0 = Server Function Id
                //  Byte 1 = Disabled (0) or Enabled (1)
                //  Bytes 2+ are optional for additional status information.
                if (fbk->size >= 2U)
                {
                    //Verify Server Id present Client side
                    uint8 client_function_index = ConvertServerIdToClientIndex((uint8)fbk->node_id, fbk->payload[0]);
                    if(INVALID_CLIENT_INDEX != client_function_index)
                    {
                        Api013.Received_Status.Client_Function_Index = (API013_CLIENT_FUNCTION_INDEX_TYPE)client_function_index;
                        Api013.Received_Status.Enabled = fbk->payload[1U];
                        Api013.Received_Status.Status = &fbk->payload[2U];
                        Api013.Received_Status.Size = fbk->size - 2U;
                        Callback__Notify(&API013_Callback, (CALLBACK_EVENT_TYPE)Api013.Received_Status.Client_Function_Index);
                    }
                }
                break;

            case API013_FBK_PUBLISH_FUNCTION_DATA:
                // Expected Payload:
                //  Byte 0 = Data ID = High nibble is server_function_id, Low nibble is data channel.
                //  Byte 1 = High byte of sint16 data value.
                //  Byte 2 = Low byte of sint16 data value.
                //  Same three bytes repeat as required to send multiple values.
                if (fbk->size >= sizeof(REMOTE_FUNCTION_DATA_TYPE))
                {
                    uint8 count = (uint8)(fbk->size / sizeof(REMOTE_FUNCTION_DATA_TYPE));
                    REMOTE_FUNCTION_DATA_TYPE* source = (REMOTE_FUNCTION_DATA_TYPE*)(void*)fbk->payload;
                    for (uint8 i = 0; i < count; i++)
                    {
                        uint8 id = source->Channel_Id / API013_MAX_DATA_CHANNELS_PER_FUNCTION;
                        uint8 client_function_index = ConvertServerIdToClientIndex((uint8)fbk->node_id, (uint8)id);

                        if(INVALID_CLIENT_INDEX != client_function_index)
                        {
                            uint16 data = COMBINE_BYTES(source->Value_Hi_Byte, source->Value_Lo_Byte);
                            uint8 channel_number = (uint8)((source->Channel_Id) % API013_MAX_DATA_CHANNELS_PER_FUNCTION);
                            StoreReceivedData((API013_CLIENT_FUNCTION_INDEX_TYPE)client_function_index, channel_number, (sint16)data);
                        }
                        source++;
                    }
                }
                break;

            default:
                LOG_ADD_EXCEPTION((LOG_MODULE_ID_TYPE)FEEDBACK_MESSAGE_NOT_HANDLED, fbk->op_code);
                break;
        }
    #else       // #if (API013_CLIENT_FEATURE == ENABLED)
        LOG_ADD_EXCEPTION((LOG_MODULE_ID_TYPE)FEEDBACK_MESSAGE_NOT_HANDLED, fbk->op_code);
    #endif      // #if (API013_CLIENT_FEATURE == ENABLED)
}


#if (API013_CLIENT_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a command to control a remote function.

    @startuml{ControlBackgroundFunction_sq.png}
        title API013 - Control Background Function
        hide footbox
        autonumber

        participant ACU
        participant "API013RemoteFunction\n(Client)" as Client
        participant "API013RemoteFunction\n(Server)" as Server
        participant FunctionX

        ACU -> Client : API013RemoteFunction__SendControl(function, control, ...)
        Client  -> Server : (39/4) ControlBackgroundFunction(function, control, ...)
        Server -> FunctionX : FunctionX__Control(control, ...)
        Client <-- Server : ACK
    @enduml

 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param control = The control command byte to send to the remote function.
 * @param params = Pointer to bytes that make up parameters for the control command.
 *                  The memory must be valid during the function call.
 * @param size = Number of bytes in the params buffer.
 */
BOOL_TYPE API013RemoteFunction__SendControl(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 control, uint8* params, uint8 size)
{
    REVEAL_COMMAND_TYPE* cmd;
    uint8 server_id;
    BOOL_TYPE ret_value = FALSE;

    server_id = ConvertClientFunctionIndexToServerId(client_function_index);
    if(INVALID_SERVER_ID != server_id)
    {
        cmd = Reveal__NewCommand(API013RMTFCN_NUM, (uint8)API013_CMD_SEND_FUNCTION_CONTROL, API013_CLIENT_FUNCTIONS[client_function_index].Communication_Node, (size + 2U));
        ret_value = (BOOL_TYPE)(((Reveal__AddDataU8(cmd, server_id)  == PASS) &&
                                          (Reveal__AddDataU8(cmd, control) == PASS) &&
                                          (Reveal__AddDataU8Buffer(cmd, params, size) == PASS) &&
                                          (Reveal__QueueCommand(cmd) != FALSE)));
    }
    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a request for the current status of a specific function.

    @startuml{RequestBackgroundFunctionStatus_sq.png}
        title API013 - Request Background Function Status
        hide footbox
        autonumber

        participant ACU
        participant "API013RemoteFunction\n(Client)" as Client
        participant "API013RemoteFunction\n(Server)" as Server
        participant FunctionX

        ACU -> Client : API013RemoteFunction__RequestStatus(function_index)
        Client  -> Server : (39/5) RequestBackgroundFunctionStatus(function_index)
        Server  -> FunctionX : FunctionX__GetStatus(&status, &size)
        Server <-- FunctionX : status, size
        Client <-- Server : ACK
        Client <-- Server : (39/5) PublishBackgroundFunctionStatus(function_index, enabled, status)
        ACU <- Client : handler(&status, size)
    @enduml

 * @param client_function_index = The client-side function index that identifies the remote function.
 */
void API013RemoteFunction__RequestStatus(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index)
{
    uint8 server_id;

    server_id = ConvertClientFunctionIndexToServerId(client_function_index);
    if(INVALID_SERVER_ID != server_id)
    {
        (void)Reveal__QueueMessageU8(API013RMTFCN_NUM, (uint8)API013_CMD_REQUEST_FUNCTION_STATUS,
                                            API013_CLIENT_FUNCTIONS[server_id].Communication_Node,
                                            TRUE, FALSE,
                                            (uint8)API013_CLIENT_FUNCTIONS[server_id].Server_Function_Id);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This function is called by the event handler to get a remote function's status information.
 * The status information will only be valid while the event handler is executing.
 * @return Pointer to a structure that contains status information for the remote function that
 *         Triggered the event handler.
 */
const API013_FUNCTION_STATUS_TYPE* API013RemoteFunction__GetReceivedStatus(void)
{
    return &Api013.Received_Status;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the most recently received value of the specified data.
 * Returns the same value if called again and no new value has been received.
 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param channel = The index of the remote function data.
 * @param value = A pointer to where the value should be stored if it is available.
 * @return TRUE if the remote function data value was available and copied into *data; otherwise FALSE.
 */
BOOL_TYPE API013RemoteFunction__GetData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, sint16* value)
{
    BOOL_TYPE ret_value = FALSE;
    uint8 index;

    index = FindDataIndex(client_function_index, channel);
    if (index != (uint8)UCHAR_MAX)
    {
        *value = Api013.Data[index].Value;
        ret_value = TRUE;
    }

    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the specified remote function data only if it is a new value that was not previously read.
 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param channel = The index of the remote function data.
 * @param sync = The synchronization byte used to detect new values that were not previously read.
 * @param value = A pointer to where the value should be stored if it is available.
 * @return TRUE if the remote function data value was available, new, and copied into *data; otherwise FALSE.
 */
BOOL_TYPE API013RemoteFunction__GetSynchronizedData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, uint8* sync, uint16* value)
{
    uint8 index;
    BOOL_TYPE ret_value = FALSE;

    index = FindDataIndex(client_function_index, channel);
    if (index != (uint8)UCHAR_MAX)
    {
        if (Api013.Data[index].Sequence != *sync)
        {
            *sync = Api013.Data[index].Sequence;
            *value = (uint16)Api013.Data[index].Value;
            ret_value = TRUE;
        }
    }

    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the publication period for data elements that are generated periodically by remote functions.

    @startuml{SetPublicationPeriod_sq.png}
        title API013 - Set Publication Period
        hide footbox
        autonumber

        participant ACU
        participant "API013RemoteFunction\n(Client)" as Client
        participant "API013RemoteFunction\n(Server)" as Server

        ACU -> Client : API013RemoteFunction__SetPublicationPeriod(milliseconds)
        Client -> Server : (39/6) SetPublicationPeriod(milliseconds)
        alt Milliseconds are Supported
            Client <-- Server : ACK
        else else
            Client <-- Server : NAK
        end alt
    @enduml

 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param milliseconds = The target period between publication of data elements.
 */
void API013RemoteFunction__SetPublicationPeriod(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint16 milliseconds)
{
    (void)Reveal__QueueMessageU16(API013RMTFCN_NUM, (uint8)API013_CMD_SET_PUBLICATION_PERIOD, API013_CLIENT_FUNCTIONS[client_function_index].Communication_Node, TRUE, FALSE, milliseconds);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Synchronize the receiver so that it can receive only new samples of the remote function data.
 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param channel = The index of the remote function data.
 * @param sync = Pointer to a byte used to synchronize the caller with API013 messages.
 */
void API013RemoteFunction__SynchronizeData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, uint8* sync)
{
    uint8 index;

    index = FindDataIndex(client_function_index, channel);
    if (index == (uint8)UCHAR_MAX)
    {
        // This remote function data has not been received, so if one is found, it will be new.
        *sync = UCHAR_MAX;
    }
    else
    {
        // This remote function data has been received.
        *sync = Api013.Data[index].Sequence;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Register for events generated by the API013_SERVER_FEATURE.
 * @param event_handler = Pointer to a function that handles API013_SERVER_FEATURE events.
 */
void API013RemoteFunction__RegisterForEvents(API013_EVENT_HANDLER_TYPE event_handler)
{
    Callback__Register(&API013_Callback, event_handler);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Unregister for events generated by the API013_SERVER_FEATURE.
 * @param event_handler = Pointer to a function that handles API013_SERVER_FEATURE events.
 */
void API013RemoteFunction__UnregisterForEvents(API013_EVENT_HANDLER_TYPE event_handler)
{
    Callback__Unregister(&API013_Callback, event_handler);
}
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)


#if (API013_SERVER_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Send an event to the API013_CLIENT_FEATURE.
 * @param event = The event to send to the API013_CLIENT_FEATURE.
 */
void API013RemoteFunction__PublishStatus(uint8 server_function_id, uint8* status, uint8 size)
{
    (void)PublishRemoteFunctionStatus(Api013.Data_Receiver_Node, server_function_id, status, size);
}
#endif      // #if (API013_SERVER_FEATURE == ENABLED)


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


#if (API013_CLIENT_FEATURE == ENABLED)
//----------------------------------------------------------------------------------------------------------------------
/**
 * Convert the client-side function id to a index of client function ID configuration table.
 * @param client_function_index = The server-side function ID that identifies the remote function.
 * @return Index of server function ID configuration table
 */
static uint8 ConvertClientFunctionIndexToServerId(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index)
{
    uint8 server_id = INVALID_SERVER_ID;
    uint8 client_index = (API013_CLIENT_FUNCTION_INDEX_TYPE) client_function_index;

    if(API013_NUM_OF_CLIENT_FUNCTIONS > client_index)
    {
        server_id = API013_CLIENT_FUNCTIONS[client_function_index].Server_Function_Id;
    }

    return server_id;
}
/**
 * Convert the server-side function index to a client-side function index.
 * @param node_id = The server communication node.
 * @param server_function_id = The server-side function id that identifies the remote function.
 * @return The client-side function index that identifies the remote function.
 */
static uint8 ConvertServerIdToClientIndex(uint8 node_id, uint8 server_function_id)
{
    uint8 client_index = INVALID_CLIENT_INDEX;

    for (uint8 index = 0; index < API013_NUM_OF_CLIENT_FUNCTIONS; index++)
    {
        if ((API013_CLIENT_FUNCTIONS[index].Communication_Node == node_id) &&
            ((sint16)API013_CLIENT_FUNCTIONS[index].Server_Function_Id == (sint16)server_function_id))
        {
            client_index = index;
        }
    }
    return client_index;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the index to the data associated with the specified remote function and the specified data channel.
 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param channel = The index of the data from the remote function.
 * @return The index to the stored data that corresponds with the client_function_index and channel.
 *         If there is no stored data, then it returns UCHAR_MAX.
 */
static uint8 FindDataIndex(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel)
{
    uint8 ret_value = (uint8)UCHAR_MAX;
    for (uint8 index = 0; index < API013_MAX_STORED_DATA; index++)
    {
        if ((Api013.Data[index].Client_Function_Index == client_function_index) &&
            (Api013.Data[index].Channel == channel))
        {
            ret_value = index;
        }
    }
    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Insert a received value from a remote function into the array of all stored values.
 * Replace any previously stored value with the client_function_index and channel.
 * Maintain the array order so that the values are sorted from newest to oldest.
 * Drop the oldest value if it does not fit in the array.
 * Api013.Data[0] will contain the inserted analog input value when done.
 * @param client_function_index = The client-side function index that identifies the remote function.
 * @param channel = The remote function's channel for generating this value.
 * @param value = The new value to store.
 */
static void StoreReceivedData(API013_CLIENT_FUNCTION_INDEX_TYPE client_function_index, uint8 channel, sint16 value)
{
    // Look for the presence of the analog input ID in the existing stored data array.
    uint8 num_of_analog_inputs_to_shift = (uint8)(API013_MAX_STORED_DATA - 1U);
    uint8 new_sequence_number = 0;                  // Assume this is the first occurrence in the stored data.

    // The last analog input must be checked to avoid unnecessarily losing the sequence number.
    for (uint8 i = 0; i < API013_MAX_STORED_DATA; i++)
    {
        API013_STORED_DATA_TYPE* stored = &Api013.Data[i];

        if ((stored->Channel == channel) &&
            (stored->Client_Function_Index == client_function_index))
        {
            num_of_analog_inputs_to_shift = i;
            if (stored->Sequence < ((uint8)UCHAR_MAX - 1U))
            {
                new_sequence_number = stored->Sequence + 1U;
            }
            else
            {
                new_sequence_number = 1U;
            }
            break;                                  // Out of the for() loop.
        }
    }

    // Shift the array elements.
    //      This will remove any previously stored copy of the analog input, and...
    //      it will make space for the new analog input at the start of the array.
    if (num_of_analog_inputs_to_shift > 0U)
    {
        memmove(&Api013.Data[1], &Api013.Data[0], num_of_analog_inputs_to_shift * sizeof(API013_STORED_DATA_TYPE));
    }

    // Store the new analog input at the start of the array.
    Api013.Data[0].Value = value;
    Api013.Data[0].Client_Function_Index = client_function_index;
    Api013.Data[0].Channel = channel;
    Api013.Data[0].Sequence = new_sequence_number;
}
#endif      // #if (API013_CLIENT_FEATURE == ENABLED)


#if (API013_SERVER_FEATURE == ENABLED)
//----------------------------------------------------------------------------------------------------------------------
/**
 * Convert the server-side function id to a index of server function ID configuration table.
 * @param server_function_id = The server-side function id that identifies the remote function.
 * @return Index of server function ID configuration table
 */
static uint8 ConvertServerFunctionIdToIndex(uint8 server_function_id)
{
    uint8 ret_value = (uint8)API013_NUM_OF_SERVER_FUNCTIONS;

    for (uint8 index = 0; index < API013_NUM_OF_SERVER_FUNCTIONS; index++)
    {
        if ((API013_SERVER_FUNCTION_ID_TYPE)API013_SERVER_FUNCTIONS[index].Server_Function_Id == (API013_SERVER_FUNCTION_ID_TYPE)server_function_id)
        {
            ret_value = index;
        }
    }
    return ret_value;
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a feedback message with all the data from all the remote functions active on this server.
 */
static void PublishAllRemoteFunctionData(void)
{
    REMOTE_FUNCTION_DATA_TYPE data_elements[API013_MAX_PUBLISHED_DATA];

    // Determine the size of the payload required for all the data elements.
    ACTIVE_TYPE active = INACTIVE;
    uint8 total = 0;
    for (uint8 index = 0; index < API013_NUM_OF_SERVER_FUNCTIONS; index++)
    {
        if (Utilities__GetBitInArray(Api013.Functions_Active, index) == (uint8)ACTIVE)
        {
            active = ACTIVE;
            total += RemoteFunctionGetData(index, &data_elements[total]);
        }
        else if (active != INACTIVE)
        {
            // Support sequentially linked remote function indices where additional remote functions only provide additional data.
            if ((API013_SERVER_FUNCTIONS[index].Background_Function == REMOTE_FUNCTION_BACKGROUND_NULL) &&
                (API013_SERVER_FUNCTIONS[index].Do_Control_Function == REMOTE_FUNCTION_DO_CONTROL_NULL) &&
                (API013_SERVER_FUNCTIONS[index].Get_Status_Function == REMOTE_FUNCTION_GET_STATUS_NULL))
            {
                total += RemoteFunctionGetData(index, &data_elements[total]);
            }
            else
            {
                active = INACTIVE;
            }
        }
    }

    if ((0U < total) && (total <= API013_MAX_DATA_CHANNELS_PER_FUNCTION))
    {
        (void)PublishPreparedData(data_elements, total);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Publish a set of data elements to Reveal.
 * @param data_elements = The data that must be published.
 * @param count = The number of data elements in the 'data_elements' array.
 */
static BOOL_TYPE PublishPreparedData(REMOTE_FUNCTION_DATA_TYPE* data_elements, uint8 count)
{
    BOOL_TYPE ret_value = FALSE;

    // Get a pointer to the feedback message.
    uint8 payload_size = (uint8)(count * sizeof(REMOTE_FUNCTION_DATA_TYPE));

    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API013RMTFCN_NUM, (uint8)API013_FBK_PUBLISH_FUNCTION_DATA, Api013.Data_Receiver_Node, payload_size);
    if (msg != NULL)
    {
        ret_value = (BOOL_TYPE)(((PASS == Reveal__AddDataU8Buffer(msg, (uint8*)data_elements, payload_size)) &&
                                 (FALSE != Reveal__QueueCommand(msg))));
    }
    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Publish the status of a specified remote function.
 * @param server_function_id = The server-side function ID that identifies the remote function.
 * @param status = Pointer to the function status data.
 * @param size = Number of bytes in the function status data.
 * @return TRUE if the message was sent; otherwise FALSE.
 */
static BOOL_TYPE PublishRemoteFunctionStatus(uint8 destination, uint8 server_function_id, uint8* status, uint8 size)
{
    uint8 index;
    BOOL_TYPE ret_value = FALSE;
    index = ConvertServerFunctionIdToIndex(server_function_id);
    if (API013_NUM_OF_SERVER_FUNCTIONS > index)
    {
        REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API013RMTFCN_NUM, (uint8)API013_FBK_PUBLISH_FUNCTION_STATUS, destination, (uint8)(size + 2U));
        ret_value = (BOOL_TYPE)(((PASS  == Reveal__AddDataU8(msg, (uint8)API013_SERVER_FUNCTIONS[index].Server_Function_Id)) &&
                                          (PASS  == Reveal__AddDataU8(msg, Utilities__GetBitInArray(Api013.Functions_Active, index))) &&
                                          (PASS  == Reveal__AddDataU8Buffer(msg, status, size)) &&
                                          (FALSE != Reveal__QueueCommand(msg))));
    }

    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a feedback message with the data elements from a single background function.
 * @param server_function_id = The server-side function ID that identifies the remote function.
 */
static void PublishSingleRemoteFunctionData(uint8 server_function_index)
{
    REMOTE_FUNCTION_DATA_TYPE data_elements[API013_MAX_DATA_CHANNELS_PER_FUNCTION];

    // Get the data from the background function (if any).
    if (server_function_index < API013_NUM_OF_SERVER_FUNCTIONS)
    {
        uint8 count = RemoteFunctionGetData(server_function_index, data_elements);

        if ((0U < count) && (count <= API013_MAX_DATA_CHANNELS_PER_FUNCTION))
        {
            (void)PublishPreparedData(data_elements, count);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Call the DoControl function for the specified function and pass the specified control.
 * @param server_function_id = The server-side function ID that identifies the remote function.
 * @param control = The control that the function should execute.
 * @param params = Pointer to bytes that make up the parameters.
 * @param size = Number of bytes referenced in the parameters.
 * @retval REMOTE_FUNCTION_CONTROL_RESPONSE_INVALID = The function does not exist or the control is not supported.
 * @retval REMOTE_FUNCTION_CONTROL_RESPONSE_INACTIVE = The function evaluated the control and is now INACTIVE.
 * @retval REMOTE_FUNCTION_CONTROL_RESPONSE_ACTIVE = The function evaluated the control and is now ACTIVE.
 */
static REMOTE_FUNCTION_CONTROL_RESPONSE_TYPE RemoteFunctionDoControl(uint8 server_function_id, uint8 control, uint8* params, uint8 size)
{
    REMOTE_FUNCTION_CONTROL_RESPONSE_TYPE control_response = REMOTE_FUNCTION_CONTROL_RESPONSE_INVALID;
    uint8 index;

    index = ConvertServerFunctionIdToIndex(server_function_id);
    if (API013_NUM_OF_SERVER_FUNCTIONS > index)
    {
        REMOTE_FUNCTION_DO_CONTROL_TYPE do_control = API013_SERVER_FUNCTIONS[index].Do_Control_Function;
        if (do_control != REMOTE_FUNCTION_DO_CONTROL_NULL)
        {
            control_response = do_control(control, params, size);
            if ((control != (uint8)REMOTE_FUNCTION_COMMAND_DISABLE) &&
                (control_response == REMOTE_FUNCTION_CONTROL_RESPONSE_ACTIVE))
            {
                Utilities__SetBitInArray(Api013.Functions_Active, index, (uint8)ACTIVE);
            }
            else
            {
                Utilities__SetBitInArray(Api013.Functions_Active, index,(uint8) INACTIVE);
            }
        }
    }

    return (control_response);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Write the data elements of a specific function to an array.
 * @param server_function_index = The server-side remote function configuration table index that identifies the remote function.
 * @param data_elements        = The array that holds the data.
 * @return The number of data elements that were added to the array.
 */
static uint8 RemoteFunctionGetData(uint8 server_function_index, REMOTE_FUNCTION_DATA_TYPE* data_elements)
{
    uint8 count = 0;
    REMOTE_FUNCTION_GET_DATA_TYPE get_data = API013_SERVER_FUNCTIONS[server_function_index].Get_Data_Function;
    if (get_data != REMOTE_FUNCTION_GET_DATA_NULL)
    {
        get_data((uint8)(API013_MAX_DATA_CHANNELS_PER_FUNCTION * (uint8)API013_SERVER_FUNCTIONS[server_function_index].Server_Function_Id), data_elements, &count);
    }

    return count;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the status of the specified function.
 * @param server_function_id = The server-side function ID that identifies the remote function.
 * @param status = Address to set to the buffer that holds the function status data.
 * @param size = Address to fill with the number of bytes in the function status data.
 * @return TRUE if the function status was successfully retrieved; otherwise, FALSE.
 */
static BOOL_TYPE RemoteFunctionGetStatus(uint8 server_function_id, uint8** status, uint8* size)
{
    uint8 index;
    BOOL_TYPE ret_value = FALSE;

    index = ConvertServerFunctionIdToIndex(server_function_id);
    if (API013_NUM_OF_SERVER_FUNCTIONS > index)
    {
        REMOTE_FUNCTION_GET_STATUS_TYPE get_status = API013_SERVER_FUNCTIONS[index].Get_Status_Function;
        if (get_status != REMOTE_FUNCTION_GET_STATUS_NULL)
        {
            get_status(status, size);
            ret_value = TRUE;
        }
    }

    return ret_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the publication period that the server uses to publish remote function data.
 * @param milliseconds = Time between publications of remote function data in milliseconds.
 *              Set to 0 to disable publications of all remote function data.
 * @return TRUE if the publication period is supported by the server; otherwise FALSE.
 */
static BOOL_TYPE SetPublicationPeriod(uint16 milliseconds)
{
    uint16 calls_to_background;
    BOOL_TYPE ret_value = FALSE;
    if (milliseconds == 0U)
    {
        Api013.Publication_Period = 0;                     // Disable data publications.
        ret_value = TRUE;
    }
    else if (milliseconds >= MIN_PUBLICATION_PERIOD)
    {
        calls_to_background = (milliseconds / API013_MS_PER_BACKGROUND_CALL);
        if ((calls_to_background <= (uint16)(UCHAR_MAX)) &&
            ((uint16)(calls_to_background * API013_MS_PER_BACKGROUND_CALL) == (uint16)milliseconds))
        {
            Api013.Publication_Period = (uint8)calls_to_background;
            ret_value = TRUE;
        }
    }

    return ret_value;
}
#endif      // #if (API013_SERVER_FEATURE == ENABLED)

#endif      // #if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))

