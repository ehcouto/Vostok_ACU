/**
 * @file
 * @brief       Communications to get periodic publication of system and memory-mapped variables
 *
 * @details     API 010 Poll Variables is used to publish standard and custom variables for the
 *              purposes of data acquisition.
 *
 *              Fixed Variables:
 *              Fixed variables are standard variables that are defined for a specific Product
 *              Type. Some boards (such as expansion boards) may not support any standard
 *              variables. Use OpCode 21 to determine what is supported. These standard variables
 *              make it easier to quickly get up and running with a system that can monitor the
 *              basic variables of a product.
 *
 *              Memory-Mapped Variables:
 *              Memory-mapped variables allow a user to customize the data being monitored. Since
 *              memory-mapped variables are a compile-time option in the firmware, some boards
 *              may not support memory-mapped variables. Use OpCode 21 to determine what is
 *              supported. To add a memory-mapped variable to the list of monitored variables,
 *              the user must know the address and size of the variable.
 *
    @startuml{API010Publisher_sm.png}
        title API 10 Publisher State Machine
        ' terminate function unused

        [*] --> Inactive

        Inactive : entry / ResetPublisher()
        Inactive : do / CheckMetadataDue()
        Inactive --> Active : Enable_Publishing

        Active : do / UpdateTimer()\n\t CheckDataDue()
        Active --> Inactive : Disable_Publishing
        Active --> Publishing : Start_Publishing

        Publishing : entry / PublishNextPacket()
        Publishing : do / UpdateTimer()\n\t PublishNextPacket()
        Publishing --> Active : Stop_Publishing
        Publishing --> Inactive : Disable_Publishing
    @enduml
 *
 * @copyright   Copyright 2014-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
/*lint -e9070
    Suppress Note: [9070]: recursive function 'Symbol', location Location

    A sequence of state transitions starting at a state and ending at the same state can represent
    an out-of-control recursive function. This can only happen if each state has an entry function
    that supports the endless loop.

    The API 10 Publisher state machine does not have a loop that does not involve the Active state
    and the Active state does not have an entry function. Therefore, this note can be suppressed.
*/

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#ifndef API010POLLVAR_FEATURE
    // API010POLLVAR_FEATURE should be ENABLED for applications that use API010 for development data acquisition.
    #error API010POLLVAR_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#if (API010POLLVAR_FEATURE == ENABLED)

#include "API010PollVar.h"
#include "API010PollVar_prv.h"
#include "API10Publisher_fsm.h"

#ifndef API010_USES_FIXED_VARIABLES_FEATURE
    // API010_USES_FIXED_VARIABLES_FEATURE should be ENABLED for applications that use the fixed system variables.
    #error API010_USES_FIXED_VARIABLES_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#ifndef API010_CAN_USE_SPI_MODULE
    // Assume that API010 uses Reveal only.
    #define API010_CAN_USE_SPI_MODULE DISABLED
#endif
#ifndef API010_CAN_USE_SPIMGR_MODULE
    // Assume that API010 uses Reveal only.
    #define API010_CAN_USE_SPIMGR_MODULE DISABLED
#endif
#if ((API010_CAN_USE_SPI_MODULE == ENABLED) && (API010_CAN_USE_SPIMGR_MODULE == ENABLED))
    #error The features API010_CAN_USE_SPI_MODULE and API010_CAN_USE_SPIMGR_MODULE cannot be enabled at the same time
#endif
#ifndef API010_DEBUG_ACQ_CONFIG
    // This is only required while debugging API010. Disable by default.
    #define API010_DEBUG_ACQ_CONFIG DISABLED
#endif

// -- Other Modules --
#include "Log.h"
#include "Micro.h"
#include "Millis.h"
#if (API010_CAN_USE_SPI_MODULE == ENABLED)
    #include "Spi.h"
#endif
#if (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
    #include "SpiMgr.h"
#endif
#include <string.h>
#include "Utilities.h"
#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    #include "Variable.h"
    #define API010_FIXED_VARS_ACU_COUNT      ((uint8)(VARIABLE_NUM_ACU_VARIABLES))
    #ifdef HMI_VARIABLE_ID_USERS_ENUM
        #define API010_FIXED_VARS_HMI_COUNT  ((uint8)(VARIABLE_NUM_HMI_VARIABLES))
    #else
        #define API010_FIXED_VARS_HMI_COUNT     0U
    #endif
#else
    #define API010_FIXED_VARS_ACU_COUNT     0U
    #define API010_FIXED_VARS_HMI_COUNT     0U
#endif


// The number of memory mapped variables supported must be at least 1.
CT_ASSERT(API010_MMAP_VARS_COUNT >= 0U);

// There is no point in enabling this module if there are no variables to publish.
// Disable the module in SystemConfig_prm.h by setting API010POLLVAR_FEATURE to DISABLED.
CT_ASSERT((API010_USES_FIXED_VARIABLES_FEATURE == ENABLED) || (API010_MMAP_VARS_COUNT > 0U));


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

#define API010_ACU_MAPPED_VAR_COUNT         (uint8)(API010_FIXED_VARS_ACU_COUNT+API010_MMAP_VARS_COUNT)

#ifndef API010_STREAM_BUFFER_SIZE
    //! The size of the incoming and outgoing data streams for transferring the periodic data.
    // The '6' in the equation is a guess of the average memory used by each variable in the data stream.
    #ifdef HMI_VARIABLE_ID_USERS_ENUM
        #define API010_STREAM_BUFFER_SIZE       (uint16)((API010_ACU_MAPPED_VAR_COUNT+ API010_FIXED_VARS_HMI_COUNT) * 6U)
    #else
        #define API010_STREAM_BUFFER_SIZE       (uint16)(API010_ACU_MAPPED_VAR_COUNT * 6U)
    #endif
#endif

#ifndef API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE
    //! The size of the buffer that can be used for detecting variables "on change".
    #define API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE        0U
#endif

#ifndef API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE
    //! The size of the buffer that can be used for detecting variables "on change".
    #define API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE        0U
#endif

//! The minimum publication period allowed by API010.
#define API010_MINIMUM_PUBLICATION_PERIOD       (10U)

//! The maximum number of metadata that can fit in a Reveal packet.
#define API010_MAXIMUM_METADATA_PER_PACKET      ((uint8)REVEAL_PAYLOAD_SIZE / sizeof(API010_VARIABLE_METADATA_TYPE))

//! The name of this module for use by the Log module.
#define MODULE_NAME                             MODULE_API010_POLL_VAR

//! Number of bytes required to store a bit for each variable.
#define ACU_VARIABLES_BIT_STORAGE_SIZE          (uint8)(BIT_STORAGE_SIZE(API010_ACU_MAPPED_VAR_COUNT))

#ifdef HMI_VARIABLE_ID_USERS_ENUM
    #define HMI_VARIABLES_BIT_STORAGE_SIZE          (uint8)(BIT_STORAGE_SIZE(API010_FIXED_VARS_HMI_COUNT))
    #define HMI_MARKER                              (255U)
#endif



// -- Private Enumerated Constant Declarations --

//! Methods of data extraction supported by API010.
typedef enum API010_ACQUISITION_METHOD_ENUM
{
    API010_METHOD_REVEAL = 0,
    API010_METHOD_SPI,
    API010_METHOD_DIRECT_SERIAL,
    API010_METHOD_SIZE
} API010_METHOD_TYPE;

//! All the API010 command and feedback OpCodes.
typedef enum API010_OPCODE_ENUM
{
    API010_CMD_REQUEST_SPECIFIC_VARIABLE            = 1,    //!< Request data for one specific variable.
    API010_FBK_PUBLISH_SPECIFIC_VARIABLE            = 2,    //!< Publish data for one specific variable.
    API010_CMD_ENABLE_PERIODIC_DATA                 = 3,    //!< Enable or disable the periodic publication of selected variables.
    API010_FBK_PUBLISH_DATA_STREAM                  = 4,    //!< Publish the data with the selected variables.
    API010_CMD_DESELECT_ALL_VARIABLES               = 5,    //!< Remove all variables from the list of published variables.
    API010_CMD_SELECT_ALL_VARIABLES                 = 6,    //!< Add all variables to the list of published variables.
    API010_CMD_DESELECT_SPECIFIC_VARIABLES          = 7,    //!< Remove specific variables from the list of published variables.
    API010_CMD_SELECT_SPECIFIC_VARIABLES            = 8,    //!< Add specific variables to the list of published variables.
    API010_CMD_SETUP_PERIODIC_PUBLICATION           = 9,    //!< Setup the publication period and the method of sending variables.
    API010_CMD_DEFINE_MEMORY_MAPPED_VARIABLE        = 10,   //!< Add a single memory-mapped variable.
    API010_CMD_UNDEFINE_MEMORY_MAPPED_VARIABLE      = 11,   //!< Remove memory-mapped variable(s).
    API010_CMD_CONFIGURE_ON_CHANGE_BEHAVIOR         = 12,   //!< Specify that a variable be published (or not) on change.
    API010_CMD_REQUEST_FIXED_VARIABLE_METADATA      = 13,   //!< Request a fixed variable's metadata.
    API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA      = 14,   //!< Publish a fixed variable's metadata.
    API010_CMD_DESELECT_SPECIFIC_HMI_VARIABLES      = 15,   //!< Remove specific HMI variables from the list of published variables.
    API010_CMD_SELECT_SPECIFIC_HMI_VARIABLES        = 16,   //!< Add specific HMI variables to the list of published variables.
    API010_CMD_REQUEST_ACQUISITION_CONFIGURATION    = 18,   //!< Request the API010 acquisition state.
    API010_FBK_PUBLISH_ACQUISITION_CONFIGURATION    = 19,   //!< Publish the API010 acquisition state.
    API010_CMD_REQUEST_NUM_OF_VARIABLES             = 20,   //!< Request the number of fixed and memory-mapped variables.
    API010_FBK_PUBLISH_NUM_OF_VARIABLES             = 21,   //!< Publish the number of fixed and memory-mapped variables.
    API010_CMD_WRITE_FIXED_VARIABLE_VALUE           = 22,   //!< Set the value of a writable variable.
    API010_CMD_WRITE_FIXED_HMI_VARIABLE_VALUE       = 23,   //!< Set the value of a writable HMI variable.
} API010_OPCODE_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_API010_POLL_VAR_LOG_MESSAGE_ID_ENUM
{
    TOO_MUCH_DATA = 1,
} MODULE_API010_POLL_VAR_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! The control structure that tracks the configuration of a single data variable.
typedef struct API010_DATA_CONTROL_STRUCT
{
    BOOL_TYPE Selected;                             //!< Selected for periodic publication.
#if ((API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)||(API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U))
    BOOL_TYPE On_Change;                            //!< Also selected for publication on change.
    BOOL_TYPE Changed;                              //!< A change was detected in this variable.
#endif
} API010_DATA_CONTROL_TYPE;

//! The first part of the payload when publishing API010 periodic data.
typedef uint16 API010_STREAM_INDEX_TYPE;

//! A structure for all the variables used by API010.
typedef struct API010_RAM_DATA_STRUCT
{
    //! The period of time between publication of API010 data.
    uint16 Publication_Period;

    //! The number of bytes in the Stream_Buffer[] array that must be published.
    API010_STREAM_INDEX_TYPE Stream_Length;

    //! The number of bytes in the Stream_Buffer[] array that have already been published.
    API010_STREAM_INDEX_TYPE Stream_Index;

    #if (API010_MMAP_VARS_COUNT > 0U)
        //! An array of configuration structures for potential memory-mapped variables.
        API010_VARIABLE_TYPE Mem_Mapped_Vars[API010_MMAP_VARS_COUNT];
    #endif

    //! The configuration and control of all of the data variables.
    API010_DATA_CONTROL_TYPE ACU_Control[API010_ACU_MAPPED_VAR_COUNT];

    #ifdef HMI_VARIABLE_ID_USERS_ENUM
        API010_DATA_CONTROL_TYPE HMI_Control[API010_FIXED_VARS_HMI_COUNT];
    #endif

    //! A value that saves the time of the last periodic publication.
    uint8 Publication_Counter;

    #if(API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
        //! Offsets into the Last_Sent_ACU_Var[] array for each variable.
        uint16 Last_Sent_ACU_Var_Offset[API010_ACU_MAPPED_VAR_COUNT + 1U];
        //! The RAM used to detect changed values in "on change" variables.
        uint8 Last_Sent_ACU_Var[API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE];
    #endif

    #if( API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
        //! Offsets into the Last_Sent_ACU_Var[] array for each variable.
        uint16 Last_Sent_HMI_Var_Offset[API010_FIXED_VARS_HMI_COUNT + 1U];
        //! The RAM used to detect changed values in "on change" variables.
        uint8 Last_Sent_HMI_Var[API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE];
    #endif

    //! Buffer used to build a packet for publishing data on Reveal or SPI.
    uint8 Stream_Buffer[API010_STREAM_BUFFER_SIZE];

    #if (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
        //! Buffer required for interactions with SpiMgr module. (Not used by this module.)
        uint8 Rx_Buffer[API010_STREAM_BUFFER_SIZE];
    #endif

    //! The selected extraction method actively being used by API010.
    API010_METHOD_TYPE Method;

    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        //! The number of variables for which the variable metadata must still be sent.
        uint16 Remaining_Metadata;
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)

    //! Node to which packets should be sent.
    uint8 Reply_To;

    //! The checksum for the Stream_Buffer[] array.
    uint8 Stream_Csum;

    //! The sequence number for the Stream_Buffer[] array published with API010_FBK_PUBLISH_DATA_STREAM.
    uint8 Sequence_Number;

    //! Flag that indicates the state of the API010 module.
    API10PUBLISHER_STATE_TYPE State;

    //! A flag that indicates that it is time to publish the API010 periodic data.
    BOOL_TYPE Is_Periodic_Data_Due;

    //! Flag that indicates whether this is the first publication of data since it was enabled.
    //! Used to publish errors only once when they are first detected.
    BOOL_TYPE Enable_Error_Reporting;

    BOOL_TYPE Is_Hmi_Variable_ID;

} API010_RAM_DATA_TYPE;

#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    //! Structure for holding a variable's metadata information.
    typedef PACKED struct API010_VARIABLE_METADATA_STRUCT
    {
        uint8 Variable_Index;
        uint8 Variable_Array_Elements;
        DATATYPE_TYPE Variable_Element_Type;
        uint32 Key;
    } API010_VARIABLE_METADATA_TYPE;
#endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --


// -- Private Variable Definitions --

//! A structure that holds all the variables used by this module.
//! This allows for more efficient initialization and does not add any run-time overhead.
static API010_RAM_DATA_TYPE Api010;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static BOOL_TYPE BuildStream(BOOL_TYPE selected);
static uint8 GetMMapVarsCount(void);
static BOOL_TYPE GetVariableInfo(uint16 variable_index, API010_VARIABLE_TYPE* var_info);
static BOOL_TYPE IsOnChangeDataDue(void);

#if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
    static BOOL_TYPE OnChangeInsertVariable(uint8 variable_index);
    static void OnChangeRemoveVariable(uint8 variable_index);
    static BOOL_TYPE SetOnChangeBehavior(BOOL_TYPE on_change, uint8 first_index, uint8 last_index);
    static void ShiftOffsetsLeft(uint8 first_index, uint8 delta);
    static void ShiftOffsetsRight(uint8 first_index, uint8 delta);
#endif      // #if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)

#ifdef HMI_VARIABLE_ID_USERS_ENUM
    #if (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
        static BOOL_TYPE OnChangeInsertHmiVariable(uint8 variable_index);
        static void OnChangeRemoveHmiVariable(uint8 variable_index);
        static BOOL_TYPE SetOnChangeBehaviorHmiVariable(BOOL_TYPE on_change, uint8 first_index, uint8 last_index);
        static void ShiftOffsetsLeftHmiVariable(uint8 first_index, uint8 delta);
        static void ShiftOffsetsRightHmiVariable(uint8 first_index, uint8 delta);
    #endif      // #if (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
#endif  //HMI_VARIABLE_ID_USERS_ENUM

#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    static void PublishNextMetadata(void);
    static PASS_FAIL_TYPE RevealAppendMetadata(REVEAL_COMMAND_TYPE *msg, uint16 variable_index);
#endif
#if (API010_MMAP_VARS_COUNT > 0U)
    static void RemoveMemoryMappedVariable(uint8 mmap_index);
#endif


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the API010PollVar module variables.
 */
void API010PollVar__Initialize(void)
{
    // Initialize the Spi module if it is only used by API010.
    #if (API010_CAN_USE_SPI_MODULE == ENABLED)
        Spi__Initialize(API010POLLVAR_SPI_CHANNEL);
    #endif

    if (Micro__GetResetMode() != MICRO_RESET_MODE_WARM)
    {
        // Initialize all of the module's RAM to zeros.
        memset(&Api010, 0U, sizeof(Api010));

        // Set other default values.
        Api010.Publication_Period = 1000U;

        // Initialize the state machine.
        API10PublisherInitialize(&Api010.State);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Handle the messages that must be sent on Reveal.
 * Handles the periodic publication of data when the API010_METHOD_REVEAL method is selected.
 * Handles the (lower priority) publication of metadata when requested.
 */
void API010PollVar__SlowHandler(void)
{
    if (Api010.Method == API010_METHOD_REVEAL)
    {
        API10PublisherExecute(&Api010.State);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Handle the messages that must be sent on SPI.
 * Typically managed on a 250us interrupt time base.
 */
void API010PollVar__FastHandler(void)
{
#if (API010_CAN_USE_SPI_MODULE == ENABLED)
    if (Api010.Method == API010_METHOD_SPI)
    {
        API10PublisherExecute(&Api010.State);
    }
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API010 commands sent to a broadcast or exclusively to this node.
 * @param cmd = Pointer to the received command message.
 * @return REVEAL_RECIPE_STATUS_TYPE that expresses the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API010PollVar__CommandParser(REVEAL_MSG_TYPE* cmd)
{
    // For convenience extract parts of the command into automatic variables.
    uint8 size = cmd->size;
    uint8* payload = cmd->payload;

    // Assume that the message is not supported.
    REVEAL_RECIPE_STATUS_TYPE reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
    switch ((API010_OPCODE_TYPE)cmd->op_code)
    {
        case API010_CMD_ENABLE_PERIODIC_DATA:
            // Expected Payload:
            //      One byte: 0 = Stop publishing periodic data, 1 = Start publishing periodic data.
            if (size >= 1U)
            {
                API10PUBLISHER_FSM_VAR_TYPE fsm_var = API10PublisherBuildFsmVar(Api010.State);
                BOOL_TYPE data_requested = ((payload[0U] == 0U) ? FALSE : TRUE);
                if (data_requested)
                {
                    API10PublisherEvent(EVENT_ENABLE_PUBLISHING, &fsm_var);
                }
                else
                {
                    API10PublisherEvent(EVENT_DISABLE_PUBLISHING, &fsm_var);
                }
                Api010.State = fsm_var.State;
                reveal_response = NVO_RS_READY;
            }
            break;

        case API010_CMD_SETUP_PERIODIC_PUBLICATION:
            // Expected Payload:
            //      One byte for the extraction method.
            //      Two big-endian bytes for the publication period in milliseconds.
            if (size >= 3U)
            {
                API010_METHOD_TYPE method = (API010_METHOD_TYPE)payload[0U];
                uint16 milliseconds = Utilities__ConvertArrayTo16bits(&payload[1U]);
                uint8 max_ms = (method == API010_METHOD_REVEAL) ? API010_MINIMUM_PUBLICATION_PERIOD : 1U;
                if (milliseconds >= max_ms)
                {
                    Api010.Method = method;
                    Api010.Publication_Period = milliseconds;
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API010_CMD_SELECT_ALL_VARIABLES:
            // Expected No Payload
            #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                for (uint8 index = 0U; index < API010_FIXED_VARS_ACU_COUNT; index++)
                {
                    Api010.ACU_Control[index].Selected = TRUE;
                }
            #endif      // #if ((API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
            #if (API010_MMAP_VARS_COUNT > 0U)
                for (uint8 index = 0U; index < API010_MMAP_VARS_COUNT; index++)
                {
                    if (Api010.Mem_Mapped_Vars[index].Address != NULL)
                    {
                        Api010.ACU_Control[API010_FIXED_VARS_ACU_COUNT + index].Selected = TRUE;
                    }
                }
            #endif      // #if (API010_MMAP_VARS_COUNT > 0U)
            #ifdef HMI_VARIABLE_ID_USERS_ENUM
                for (uint8 index = 0U; index < API010_FIXED_VARS_HMI_COUNT; index++)
                {
                    Api010.HMI_Control[index].Selected = TRUE;
                }
            #endif      // #ifdef HMI_VARIABLE_ID_USERS_ENUM
            reveal_response = NVO_RS_READY;
            break;

        case API010_CMD_DESELECT_ALL_VARIABLES:
            // Expected No Payload
            for (uint8 index = 0U; index < API010_ACU_MAPPED_VAR_COUNT; index++)
            {
                Api010.ACU_Control[index].Selected = FALSE;
            }
        #ifdef HMI_VARIABLE_ID_USERS_ENUM
            for (uint8 index = 0U; index < API010_FIXED_VARS_HMI_COUNT; index++)
            {
                Api010.HMI_Control[index].Selected = FALSE;
            }
        #endif
            reveal_response = NVO_RS_READY;
            break;

        case API010_CMD_SELECT_SPECIFIC_VARIABLES:
            // Expected Payload:
            //      One byte for each selected variable index.
            reveal_response = NVO_RS_READY;
            for (uint8 index = 0U; index < size; index++)
            {
                uint8 variable_index = payload[index];
                if (
                  #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                    (variable_index < API010_FIXED_VARS_ACU_COUNT)
                    #if (API010_MMAP_VARS_COUNT > 0U)
                     ||
                    #endif
                  #endif
                  #if (API010_MMAP_VARS_COUNT > 0U)
                    ((variable_index < API010_ACU_MAPPED_VAR_COUNT) &&
                     (Api010.Mem_Mapped_Vars[variable_index - API010_FIXED_VARS_ACU_COUNT].Address != NULL))
                  #endif

                   )
                {
                    Api010.ACU_Control[variable_index].Selected = TRUE;
                }
                else
                {
                    reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            break;

        case API010_CMD_DESELECT_SPECIFIC_VARIABLES:
            // Expected Payload:
            //      One byte for each deselected variable index.
            reveal_response = NVO_RS_READY;
            for (uint8 index = 0U; index < size; index++)
            {
                uint8 variable_index = payload[index];
                if (variable_index < API010_ACU_MAPPED_VAR_COUNT)
                {
                    Api010.ACU_Control[variable_index].Selected = FALSE;
                }
                else
                {
                    reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            break;
    #ifdef HMI_VARIABLE_ID_USERS_ENUM
        case API010_CMD_SELECT_SPECIFIC_HMI_VARIABLES:
             // Expected Payload:
             //      One byte for each selected variable index.
             reveal_response = NVO_RS_READY;
             for (uint8 index = 0U; index < size; index++)
             {
                 uint8 variable_index = payload[index];
                 #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                     if(variable_index < API010_FIXED_VARS_HMI_COUNT)
                     {
                         Api010.HMI_Control[variable_index].Selected = TRUE;
                     }
                     else
                     {
                         reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
                     }
                 #endif
             }
             break;

        case API010_CMD_DESELECT_SPECIFIC_HMI_VARIABLES:
            // Expected Payload:
            //      One byte for each deselected variable index.
            reveal_response = NVO_RS_READY;
            for (uint8 index = 0U; index < size; index++)
            {
                uint8 variable_index = payload[index];
                if (variable_index < API010_FIXED_VARS_HMI_COUNT )
                {
                    Api010.HMI_Control[variable_index].Selected = FALSE;
                }
                else
                {
                    reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            break;
    #endif

        case API010_CMD_CONFIGURE_ON_CHANGE_BEHAVIOR:
            // Expected Payload:
            //      One byte for the variable index.
            //      One byte for the "on-change" flag. 0 = do not publish on change, 1 = publish on change.
            //      Optional Byte: One byte for ACU or HMI variable selection, if this byte is not received then it is assume to be 0
            if (size >= 2U)
            {
                uint8 variable_index = payload[0U];
                BOOL_TYPE on_change = ((payload[1U] == 0U) ? FALSE : TRUE);
                uint8 is_hmi_variable_id = 0U;
                if(size >= 3U)
                {
                    is_hmi_variable_id = payload[2U] ;
                }

                if(variable_index == 255U)
                {
                #if ((API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U) && (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE == 0U))
                    // Set the "on change" behavior for all ACU variables.
                    if(SetOnChangeBehavior(on_change, 0U, (API010_ACU_MAPPED_VAR_COUNT - 1U)) != FALSE)
                    {
                        reveal_response = NVO_RS_READY;
                    }
                #endif

                #ifdef HMI_VARIABLE_ID_USERS_ENUM
                    #if((API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE == 0U) && (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U))
                        // Set the "on change" behavior for all HMI variables.
                        if (SetOnChangeBehaviorHmiVariable(on_change, 0U, (API010_FIXED_VARS_HMI_COUNT - 1U)) != FALSE)
                        {
                            reveal_response = NVO_RS_READY;
                        }
                    #elif((API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)&&(API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U))
                        // Set the "on change" behavior for all variables.
                        if((SetOnChangeBehavior(on_change, 0U, (API010_ACU_MAPPED_VAR_COUNT - 1U)) != FALSE) &&
                                (SetOnChangeBehaviorHmiVariable(on_change, 0U, (API010_FIXED_VARS_HMI_COUNT - 1U)) != FALSE))
                        {
                            reveal_response = NVO_RS_READY;
                        }
                    #endif
                #endif
                }

                else
                {
                    if(is_hmi_variable_id == 0U)
                    {
                    #if(API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
                        if((variable_index < API010_ACU_MAPPED_VAR_COUNT) &&
                                (SetOnChangeBehavior(on_change, variable_index, variable_index) != FALSE))
                        {
                            // Set the "on change" behavior for the specified variable.
                            reveal_response = NVO_RS_READY;
                        }
                    #endif
                    }
                    else
                    {
                    #if(API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
                        if ((variable_index < API010_FIXED_VARS_HMI_COUNT) &&
                                (SetOnChangeBehaviorHmiVariable(on_change, variable_index, variable_index) != FALSE ))
                        {
                            reveal_response = NVO_RS_READY;
                        }
                    #endif
                    }
                }
            }
            break;

    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        case API010_CMD_REQUEST_FIXED_VARIABLE_METADATA:
            // Expected Payload:
            //      One byte for the variable index.
            //      Optional Byte: One byte for ACU or HMI variable selection, if this byte is not received then it is assume to be 0
            if (size >= 1U)
            {
                uint8 variable_index = payload[0U];
                uint8 is_hmi_variable_id = 0U;

                if(size >= 2U)
                {
                    is_hmi_variable_id = payload[1U];
                }
                if(Api010.Remaining_Metadata == 0U)
                {
                    if (variable_index != 255U)
                    {
                        if((variable_index < API010_FIXED_VARS_ACU_COUNT) && ( is_hmi_variable_id == 0U))
                        {
                            // Publish metadata for specified variable.
                            REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA, cmd->node_id, sizeof(API010_VARIABLE_METADATA_TYPE));
                            if ((msg != NULL) &&
                                    (RevealAppendMetadata(msg, variable_index) == PASS) &&
                                    (Reveal__QueueCommand(msg)))
                            {
                                reveal_response = NVO_RS_READY;
                            }
                            else
                            {
                                reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                            }

                        }
                    #ifdef HMI_VARIABLE_ID_USERS_ENUM
                        else if ((variable_index < API010_FIXED_VARS_HMI_COUNT) && ( is_hmi_variable_id == 1U))
                        {
                            // Publish metadata for specified variable.
                            REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA, cmd->node_id, (sizeof(API010_VARIABLE_METADATA_TYPE)+1U));

                            if ((msg != NULL) &&
                                (Reveal__AddDataU8(msg, HMI_MARKER) == PASS) &&
                                (RevealAppendMetadata(msg, variable_index) == PASS) &&
                                (Reveal__QueueCommand(msg)))
                            {
                                reveal_response = NVO_RS_READY;
                            }
                            else
                            {
                                reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                            }
                        }
                    #endif
                    }
                    else if (variable_index == 255U)
                    {
                        if(is_hmi_variable_id == 0U)
                        {
                            // Publish metadata for all ACU fixed variables.
                            Api010.Reply_To = cmd->node_id;
                            Api010.Remaining_Metadata = API010_FIXED_VARS_ACU_COUNT;
                            Api010.Is_Hmi_Variable_ID = FALSE;
                            reveal_response = NVO_RS_READY;
                        }
                    #ifdef HMI_VARIABLE_ID_USERS_ENUM
                        else
                        {
                            // Publish metadata for all HMI fixed variables.
                            Api010.Reply_To = cmd->node_id;
                            Api010.Remaining_Metadata = API010_FIXED_VARS_HMI_COUNT ;
                            Api010.Is_Hmi_Variable_ID = TRUE;
                            reveal_response = NVO_RS_READY;
                        }
                    #endif
                    }
                }
            }
            break;
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)

    #if (API010_MMAP_VARS_COUNT > 0U)
        case API010_CMD_DEFINE_MEMORY_MAPPED_VARIABLE:
            // Expected Payload:
            //      One byte for the variable index.
            //      One byte for the size of the variable's data.
            //      Four big-endian bytes for the variable's pointer.
            if (size >= 6U)
            {
                uint8 variable_index = payload[0];
                if (
                    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                     (API010_FIXED_VARS_ACU_COUNT <= variable_index) &&
                    #endif
                     (variable_index < API010_ACU_MAPPED_VAR_COUNT))
                {
                    // adjust index
                    uint8 mmap_index = (uint8)(variable_index - API010_FIXED_VARS_ACU_COUNT);
                    Api010.Mem_Mapped_Vars[mmap_index].Size = payload[1];
                    Api010.Mem_Mapped_Vars[mmap_index].Address = (void*)Utilities__ConvertArrayTo32bits(&payload[2]); //lint !e923 Suppress Info: cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API010_CMD_UNDEFINE_MEMORY_MAPPED_VARIABLE:
            // Expected Payload:
            //      One byte for each undefined variable index.
            if ((size == 1U) && (payload[0] == 255U))
            {
                // Undefine all memory-mapped variables.
                for (uint8 mmap_index = 0U; mmap_index < API010_MMAP_VARS_COUNT; mmap_index++)
                {
                    RemoveMemoryMappedVariable(mmap_index);
                }
                reveal_response = NVO_RS_READY;
            }
            else
            {
                for (uint8 index = 0U; index < size; index++)
                {
                    uint8 variable_index = payload[index];
                    if (
                        #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                         (API010_FIXED_VARS_ACU_COUNT <= variable_index) &&
                        #endif
                         (variable_index < API010_ACU_MAPPED_VAR_COUNT))
                    {
                        // Undefine the specified memory-mapped variable.
                        uint8 mmap_index = variable_index - API010_FIXED_VARS_ACU_COUNT;
                        RemoveMemoryMappedVariable(mmap_index);
                        reveal_response = NVO_RS_READY;
                    }
                }
            }
            break;
    #endif      // #if (API010_MMAP_VARS_COUNT > 0U)

        case API010_CMD_REQUEST_SPECIFIC_VARIABLE:
            // Expected Payload:
            //      One byte for the variable index.
            //      Optional Byte : One byte for ACU or HMI variable selection, if not received it will consider as 0
            if (size >= 1U)
            {
                // This opcode sends the data of one specific variable, this opcode doesn't take care of endianess, it will be send out in the endianess of the micro
                // The same process used by the stream transmission.
                uint8 variable_index = payload[0U];
                uint8 is_hmi_variable_id  = 0U;
                API010_VARIABLE_TYPE variable;

                if (size >= 2U)
                {
                    is_hmi_variable_id= payload[1U];
                }

                // Publish specific ACU variable
                if((is_hmi_variable_id == 0U) && (GetVariableInfo(variable_index, &variable)))
                {
                    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_SPECIFIC_VARIABLE, cmd->node_id, variable.Size + 1U);
                    if ((Reveal__AddDataU8(msg, variable_index) == PASS) &&
                            (Reveal__AddDataU8Buffer(msg, variable.Address, variable.Size) == PASS) &&
                             Reveal__QueueCommand(msg))
                    {
                        reveal_response = NVO_RS_READY;
                    }
                    else
                    {
                        // Reveal rejected the request to publish a message.
                        reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                    }
                }
            #ifdef HMI_VARIABLE_ID_USERS_ENUM
                else
                {
                    // Publish specific HMI variable
                    if (GetVariableInfo((variable_index+VARIABLE_HMI_START_INDEX), &variable))
                    {
                        REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_SPECIFIC_VARIABLE, cmd->node_id, variable.Size + 2U);
                        if ((Reveal__AddDataU8(msg, HMI_MARKER) == PASS) &&
                            (Reveal__AddDataU8(msg, variable_index) == PASS) &&
                            (Reveal__AddDataU8Buffer(msg, variable.Address, variable.Size) == PASS) &&
                             Reveal__QueueCommand(msg))
                        {
                            reveal_response = NVO_RS_READY;
                        }
                        else
                        {
                            // Reveal rejected the request to publish a message.
                            reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                        }
                    }
                }
            #endif
            }
            break;

    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        case API010_CMD_WRITE_FIXED_VARIABLE_VALUE:

        case API010_CMD_WRITE_FIXED_HMI_VARIABLE_VALUE:
            // Expected Payload
            //      One byte for the variable index.
            //      One byte for the array position.
            //      One or more bytes for the variable value in native endian format.
            if (size >= 3U)
            {
                uint16 variable_index = payload[0U];
                uint8 write_acu_variable = (cmd->op_code == (uint8)API010_CMD_WRITE_FIXED_VARIABLE_VALUE )? 1U : 0U;

                if( write_acu_variable == 0U)
                {
                    variable_index = variable_index+(uint16)VARIABLE_HMI_START_INDEX ;
                }

                VARIABLE_TYPE variable;
                if ((Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable) != FALSE ) &&
                        variable.Is_Writable)
                {
                    uint8 array_position = payload[1U];
                    uint8 data_size = size - 2U;
                    uint8 variable_size = VARIABLE_SIZE[variable.Type];
                    uint8 element_count = data_size / variable_size;
                    if (((data_size % variable_size) == 0U) &&
                        ((array_position + element_count) <= variable.Length))
                    {
                        uint8* data_source = &payload[2];
                        uint8* data_destination = variable.Address;
                        data_destination += array_position * variable_size;
                        memcpy(data_destination, data_source, data_size);
                        reveal_response = NVO_RS_READY;
                    }
                }
            }
            break;
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)

    #if (API010_DEBUG_ACQ_CONFIG == ENABLED)
        case API010_CMD_REQUEST_ACQUISITION_CONFIGURATION:
            // Expected No Payload   
            {
                uint8 msg_size = 3U;

                #if ((API010_USES_FIXED_VARIABLES_FEATURE == ENABLED) || (API010_MMAP_VARS_COUNT > 0U))
                    // Prepare ACU variable and/or memory mapped variable configuration data
                    msg_size += (2U * API010_ACU_MAPPED_VAR_COUNT) + 1U;
                    uint8 selected_acu[ACU_VARIABLES_BIT_STORAGE_SIZE];
                    uint8 on_change_acu[ACU_VARIABLES_BIT_STORAGE_SIZE];
                    for (uint8 index = 0U; index < API010_ACU_MAPPED_VAR_COUNT; index++)
                    {
                        Utilities__SetBitInArray(selected_acu, index, (uint8)Api010.ACU_Control[index].Selected);
                        Utilities__SetBitInArray(on_change_acu, index, (uint8)Api010.ACU_Control[index].On_Change);
                    }
                #else
                    uint8 msg_size = 0U;
                #endif

                #ifdef HMI_VARIABLE_ID_USERS_ENUM
                    // Prepare HMI variable variable configuration data
                    msg_size += (2U * API010_FIXED_VARS_HMI_COUNT) + 2U;
                    uint8 selected_hmi[HMI_VARIABLES_BIT_STORAGE_SIZE];
                    uint8 on_change_hmi[HMI_VARIABLES_BIT_STORAGE_SIZE];
                    for (uint8 index = 0U; index < (API010_FIXED_VARS_HMI_COUNT); index++)
                    {
                        Utilities__SetBitInArray(selected_hmi, index, (uint8)Api010.HMI_Control[index].Selected);
                        Utilities__SetBitInArray(on_change_hmi, index, (uint8)Api010.HMI_Control[index].On_Change);
                    }
                #endif

                reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;

                REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_ACQUISITION_CONFIGURATION, cmd->node_id, msg_size);

                if ((Reveal__AddDataU8(msg, Api010.Method) == PASS) &&
                    (Reveal__AddDataU16(msg, Api010.Publication_Period) == PASS) &&
                #if ((API010_USES_FIXED_VARIABLES_FEATURE == ENABLED) || (API010_MMAP_VARS_COUNT > 0U))
                    (Reveal__AddDataU8(msg, ACU_VARIABLES_BIT_STORAGE_SIZE) == PASS) &&
                    (Reveal__AddDataU8Buffer(msg, selected_acu, ACU_VARIABLES_BIT_STORAGE_SIZE) == PASS) &&
                    (Reveal__AddDataU8Buffer(msg, on_change_acu, ACU_VARIABLES_BIT_STORAGE_SIZE) == PASS) &&
                #endif
                #ifdef HMI_VARIABLE_ID_USERS_ENUM
                    (Reveal__AddDataU8(msg, HMI_MARKER) == PASS) &&
                    (Reveal__AddDataU8(msg, HMI_VARIABLES_BIT_STORAGE_SIZE) == PASS) &&
                    (Reveal__AddDataU8Buffer(msg, selected_hmi, HMI_VARIABLES_BIT_STORAGE_SIZE) == PASS) &&
                    (Reveal__AddDataU8Buffer(msg, on_change_hmi, HMI_VARIABLES_BIT_STORAGE_SIZE) == PASS) &&
                #endif
                     Reveal__QueueCommand(msg))
                {
                    reveal_response = NVO_RS_READY;
                }
                break;
            }
    #endif      // #if (API010_DEBUG_ACQ_CONFIG == ENABLED)

        case API010_CMD_REQUEST_NUM_OF_VARIABLES:
            // Expected No Payload
            {
                REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_NUM_OF_VARIABLES, cmd->node_id, 4U);
                if ((Reveal__AddDataU8(msg, API010_FIXED_VARS_ACU_COUNT) == PASS) &&
                    (Reveal__AddDataU8(msg, API010_MMAP_VARS_COUNT) == PASS) &&
                    (Reveal__AddDataU8(msg, GetMMapVarsCount()) == PASS) &&
                    (Reveal__AddDataU8(msg, API010_FIXED_VARS_HMI_COUNT) == PASS) &&
                     Reveal__QueueCommand(msg))
                {
                    reveal_response = NVO_RS_READY;
                }
                else
                {
                    // Reveal rejected the request to publish a message.
                    reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                }
                break;
            }

        default:
            reveal_response = NVO_RS_UNSUP_OP_CODE;
            break;
    }
    return (reveal_response);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the current list of memory-mapped variables.
 * These are the variables configured by an external tool.
 * @param list = Pointer to a buffer to fill with the variable information.
 * @param max_vars = The maximum number of variables that the list can hold.
 * @param publication_period = Pointer to a buffer to fill with the publication period.
 * @return The number of memory-mapped variables that were copied.
 */
uint8 API010PollVar__GetMemMapVarsList(API010_VARIABLE_TYPE* list, uint8 max_vars, uint16* publication_period)
{
    uint8 vars_copied = 0;
    #if API010_MMAP_VARS_COUNT > 0U
        for (uint8 i = 0U; (i < API010_MMAP_VARS_COUNT) && (vars_copied < max_vars); i++)
        {
            if (Api010.Mem_Mapped_Vars[i].Address != NULL)
            {
                list[vars_copied].Address = Api010.Mem_Mapped_Vars[i].Address;
                list[vars_copied].Size = Api010.Mem_Mapped_Vars[i].Size;
                vars_copied++;
            }
        }
    #endif
    *publication_period = Api010.Publication_Period;

    return vars_copied;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Build the stream of data for the specified variables.
 * The stream of data consists of the following:
 *      One byte for the stream sequence number. Incremented for each new stream of data.
 *      For each published variable:
 *          One byte for the variable index.
 *          One or more bytes for the variable value.
 * @param selected = A flag that specifies which set of variables to put in the data stream.
 *              FALSE = Only put the variables that have changed in the data stream.
 *              TRUE = Put all the variables that were selected as part of the periodic data.
 * @return TRUE if the data stream was built and there is data in it.
 */


static BOOL_TYPE BuildStream(BOOL_TYPE selected)
{
	Api010.Stream_Buffer[0] = Api010.Sequence_Number;
    Api010.Sequence_Number++;
    Api010.Stream_Length = 1U;
    for (uint8 index = 0U; index < API010_ACU_MAPPED_VAR_COUNT; index++)
    {
	#if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
        if (Api010.ACU_Control[index].Selected &&
            (selected ||
             (Api010.ACU_Control[index].On_Change &&
              Api010.ACU_Control[index].Changed)))
	#else
        if (Api010.ACU_Control[index].Selected &&
            selected)
	#endif
        {
            API010_VARIABLE_TYPE variable;
            if (GetVariableInfo(index, &variable) &&
                ((Api010.Stream_Length + variable.Size + 1U) <= API010_STREAM_BUFFER_SIZE))
            {
                Api010.Stream_Buffer[Api010.Stream_Length] = index;
                Api010.Stream_Length++;
                memcpy(&Api010.Stream_Buffer[Api010.Stream_Length], variable.Address, variable.Size);
                #if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
                    if (Api010.ACU_Control[index].On_Change)
                    {
                        uint16 offset = Api010.Last_Sent_ACU_Var_Offset[index];
                        memcpy(&Api010.Last_Sent_ACU_Var[offset], variable.Address, variable.Size);
                    }
                    Api010.ACU_Control[index].Changed = FALSE;
                #endif
                Api010.Stream_Length += variable.Size;
            }
            else
            {
                if (Api010.Enable_Error_Reporting)
                {
                    Api010.Enable_Error_Reporting = FALSE;
                    LOG_ADD_EXCEPTION(TOO_MUCH_DATA, index);
                }
                break;
            }
        }
    }
#ifdef HMI_VARIABLE_ID_USERS_ENUM
    for (uint8 index = 0U; index < API010_FIXED_VARS_HMI_COUNT; index++)
    {
	#if (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
        if (Api010.HMI_Control[index].Selected &&
            (selected ||
             (Api010.HMI_Control[index].On_Change &&
              Api010.HMI_Control[index].Changed)))
	#else
        if (Api010.HMI_Control[index].Selected &&
            selected)
	#endif
        {
            API010_VARIABLE_TYPE variable;
            if (GetVariableInfo((index+VARIABLE_HMI_START_INDEX), &variable) &&
                    ((Api010.Stream_Length + variable.Size + 1U) <= API010_STREAM_BUFFER_SIZE))
            {
                Api010.Stream_Buffer[Api010.Stream_Length] = index;
                Api010.Stream_Length++;
                memcpy(&Api010.Stream_Buffer[Api010.Stream_Length], variable.Address, variable.Size);
                #if (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
                    if (Api010.HMI_Control[index].On_Change)
                    {
                        uint16 offset = Api010.Last_Sent_HMI_Var_Offset[index];
                        memcpy(&Api010.Last_Sent_HMI_Var[offset], variable.Address, variable.Size);
                    }
                    Api010.HMI_Control[index].Changed = FALSE;
                #endif
                Api010.Stream_Length += variable.Size;
            }
            else
            {
                if (Api010.Enable_Error_Reporting)
                {
                    Api010.Enable_Error_Reporting = FALSE;
                    LOG_ADD_EXCEPTION(TOO_MUCH_DATA, index);
                }
                break;
            }
        }
    }
#endif
    if (Api010.Stream_Length > 1U)
    {
        // Setup the supporting stream variables.
        Api010.Stream_Csum = 0U;
        for (API010_STREAM_INDEX_TYPE i = 0U; i < Api010.Stream_Length; i++)
        {
            Api010.Stream_Csum += Api010.Stream_Buffer[i];
        }
        Api010.Stream_Index = 0U;
    }
    return (BOOL_TYPE)(Api010.Stream_Length > 1U);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the number of memory mapped variables that have been defined.
 * @return The number of memory mapped variables that have been defined.
 */
static uint8 GetMMapVarsCount(void)
{
    uint8 count = 0U;
    #if (API010_MMAP_VARS_COUNT > 0U)
        for (uint8 i = 0U; i < API010_MMAP_VARS_COUNT; i++)
        {
            if (Api010.Mem_Mapped_Vars[i].Address != NULL)
            {
                count++;
            }
        }
    #endif      // #if (API010_MMAP_VARS_COUNT > 0U)
    return count;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get generic information about a variable whether it is a fixed variable or a memory-mapped variable.
 * @param variable_index = The variable to get.
 * @param var_info = Pointer to a structure to fill with the variable information.
 * @return TRUE if the var_info structure was filled. Otherwise FALSE.
 */
static BOOL_TYPE GetVariableInfo(uint16 variable_index, API010_VARIABLE_TYPE* var_info)
{
    BOOL_TYPE result = FALSE;
    
    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        if (variable_index < API010_FIXED_VARS_ACU_COUNT)
        {
            VARIABLE_TYPE variable;
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable) != FALSE )
            {
                var_info->Address = variable.Address;
                var_info->Size = (uint8)(VARIABLE_SIZE[variable.Type] * variable.Length);
                result = TRUE;
            }
        }
        #if (API010_MMAP_VARS_COUNT > 0U)
        else
        #endif      // #if (API010_MMAP_VARS_COUNT > 0U)
    #endif          // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    #if (API010_MMAP_VARS_COUNT > 0U)
        if (variable_index < API010_ACU_MAPPED_VAR_COUNT)
        {
            uint8 mmap_index = (uint8)variable_index - API010_FIXED_VARS_ACU_COUNT;
            API010_VARIABLE_TYPE* mmap_var = &Api010.Mem_Mapped_Vars[mmap_index];
            if (mmap_var->Address != NULL)
            {
                memcpy(var_info, mmap_var, sizeof(API010_VARIABLE_TYPE));
                result = TRUE;
            }
        }
    #endif      // #if (API010_MMAP_VARS_COUNT > 0U)
    #ifdef HMI_VARIABLE_ID_USERS_ENUM
        if ((variable_index > (uint8)VARIABLE_INVALID_INDEX)&& (variable_index < (uint16)VARIABLE_END_HMI_VARIABLES))
        {
            VARIABLE_TYPE variable;
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable) != FALSE )
            {
                var_info->Address = variable.Address;
                var_info->Size = (uint8)(VARIABLE_SIZE[variable.Type] * variable.Length);
                result = TRUE;
            }
        }
    #endif
    return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Scan the variables that are setup for publication on change for any new changes.
 * @return TRUE if any variables are set as changed.
 */
static BOOL_TYPE IsOnChangeDataDue(void)
{
    BOOL_TYPE change_found = FALSE;
    #if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
        for (uint8 variable_index = 0; variable_index < API010_ACU_MAPPED_VAR_COUNT; variable_index++)
        {
            if (Api010.ACU_Control[variable_index].Selected &&
                Api010.ACU_Control[variable_index].On_Change)
            {
                if (Api010.ACU_Control[variable_index].Changed)
                {
                    change_found = TRUE;
                }
                else
                {
                    API010_VARIABLE_TYPE variable;
                    if (GetVariableInfo(variable_index, &variable) != FALSE )
                    {
                        uint16 offset = Api010.Last_Sent_ACU_Var_Offset[variable_index];
                        uint8* last_sent = &Api010.Last_Sent_ACU_Var[offset];
                        if (memcmp(variable.Address, last_sent, variable.Size) != 0)
                        {
                            Api010.ACU_Control[variable_index].Changed = TRUE;
                            change_found = TRUE;
                        }
                    }
                }
            }
        }
    #endif      // #if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)

    #if (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
        for (uint8 variable_index = 0U; variable_index < API010_FIXED_VARS_HMI_COUNT; variable_index++)
        {
            if (Api010.HMI_Control[variable_index].Selected &&
                Api010.HMI_Control[variable_index].On_Change)
            {
                if (Api010.HMI_Control[variable_index].Changed)
                {
                    change_found = TRUE;
                }
                else
                {
                    API010_VARIABLE_TYPE variable;
                    if (GetVariableInfo((uint16)(variable_index+(uint16)VARIABLE_HMI_START_INDEX), &variable) != FALSE)
                    {
                        uint16 offset = Api010.Last_Sent_HMI_Var_Offset[variable_index];
                        uint8* last_sent = &Api010.Last_Sent_HMI_Var[offset];
                        if (memcmp(variable.Address, last_sent, variable.Size) != 0)
                        {
                            Api010.HMI_Control[variable_index].Changed = TRUE;
                            change_found = TRUE;
                        }
                    }
                }
            }
        }
    #endif      // #if (API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U)
    return change_found;
}


#if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Insert the specified variable into the on change monitoring.
 * @param variable_index = The variable to insert into "on change" monitoring.
 * @return TRUE if the specified variable was inserted into "on change" monitoring. Otherwise FALSE.
 */
static BOOL_TYPE OnChangeInsertVariable(uint8 variable_index)
{
    BOOL_TYPE result = FALSE;
    // Only insert the variable if it is not already inserted.
    if ((variable_index < (API010_ACU_MAPPED_VAR_COUNT)) &&
        (Api010.ACU_Control[variable_index].On_Change == FALSE))
    {
        // Make sure there is space in the Last_Sent_ACU_Var[] array for this variable.
        uint16 space_consumed = Api010.Last_Sent_ACU_Var_Offset[(API010_ACU_MAPPED_VAR_COUNT)];
        uint16 space_available = API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE - space_consumed;
        API010_VARIABLE_TYPE variable;
        if (GetVariableInfo(variable_index, &variable) &&
            (variable.Size <= space_available))
        {
            // Update all the offsets to the Last_Sent_ACU_Var[] data.
            ShiftOffsetsRight(variable_index + 1U, variable.Size);

            // Setup automatic variables to reference the variable's Last_Sent_ACU_Var value.
            uint16 var_offset = Api010.Last_Sent_ACU_Var_Offset[variable_index];
            uint8* var_data = &Api010.Last_Sent_ACU_Var[var_offset];

            // Shift the Last_Sent_ACU_Var[] values to make space for the new variable.
            if (variable_index < (API010_ACU_MAPPED_VAR_COUNT - 1U))
            {
                // Shift the other values to make space for this variable.
                uint8* move_to = &Api010.Last_Sent_ACU_Var[var_offset + variable.Size];
                uint16 bytes_to_shift = space_consumed - var_offset;
                memmove(move_to, var_data, bytes_to_shift);
            }

            // Update the Control of the variable so that it is selected, monitored, and changed.
            Api010.ACU_Control[variable_index].Selected = TRUE;
            Api010.ACU_Control[variable_index].On_Change = TRUE;
            memcpy(var_data, variable.Address, variable.Size);
            Api010.ACU_Control[variable_index].Changed = TRUE;
            result = TRUE;
        }
    }
    return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove the specified variable from the on change monitoring.
 * @param variable_index = The variable to remove from "on change" monitoring.
 */
static void OnChangeRemoveVariable(uint8 variable_index)
{
    // Only remove the variable if it is already inserted.
    if ((variable_index < (API010_ACU_MAPPED_VAR_COUNT)) &&
            (Api010.ACU_Control[variable_index].On_Change))
    {
        // Get the variable information.
        API010_VARIABLE_TYPE variable;
        if (GetVariableInfo(variable_index, &variable))
        {
            // Update all the offsets to the Last_Sent_ACU_Var[] data.
            ShiftOffsetsLeft(variable_index + 1U, variable.Size);

            // Setup automatic variables to reference the variable's Last_Sent_ACU_Var value.
            uint16 var_offset = Api010.Last_Sent_ACU_Var_Offset[variable_index];
            uint8* var_data = &Api010.Last_Sent_ACU_Var[var_offset];

            // Shift the Last_Sent_ACU_Var[] values to collapse the space that is no longer needed.
            if (variable_index < (API010_ACU_MAPPED_VAR_COUNT - 1U))
            {
                // Shift the other values to make space for this variable.
                uint16 space_consumed = Api010.Last_Sent_ACU_Var_Offset[API010_ACU_MAPPED_VAR_COUNT];
                uint8* move_from = &var_data[variable.Size];
                uint16 bytes_to_shift = space_consumed - var_offset;
                memmove(var_data, move_from, bytes_to_shift);
            }

            // Update the Control of the variable so that it is not monitored or changed.
            Api010.ACU_Control[variable_index].On_Change = FALSE;
            Api010.ACU_Control[variable_index].Changed = FALSE;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the "on change" behavior for one or more variables.
 * There are no checks for out of range values.
 *      first_index should be less than or equal to last_index.
 *      Both values should be in the range [0 .. API010_VARS_COUNT).
 * @param on_change = Desired "on change" behavior for the selected variable(s).
 * @param first_index = First variable index in the range of variables to affect.
 * @param last_index = Last variable index in the range of variables to affect.
 * @return TRUE if the on change behavior was updated. Otherwise FALSE.
 */
static BOOL_TYPE SetOnChangeBehavior(BOOL_TYPE on_change, uint8 first_index, uint8 last_index)
{
    uint16 variables_added = 0U;
    for (uint8 variable_index = first_index; variable_index <= last_index; variable_index++)
    {
        if (on_change)
        {
            if (Api010.ACU_Control[variable_index].Selected &&
                OnChangeInsertVariable(variable_index))
            {
                variables_added++;
            }
        }
        else
        {
            OnChangeRemoveVariable(variable_index);
        }
    }
    return (BOOL_TYPE)((on_change == FALSE) || (variables_added > 0U));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add or subtract a delta value from the Last_Sent_ACU_Var_Offset[] array.
 * @param first_index = The first index that needs updating.
 * @param delta = The amount of change to subtract.
 */
static void ShiftOffsetsLeft(uint8 first_index, uint8 delta)
{
    for (uint8 variable_index = first_index; variable_index <= API010_ACU_MAPPED_VAR_COUNT; variable_index++)
    {
        Api010.Last_Sent_ACU_Var_Offset[variable_index] -= delta;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add or subtract a delta value from the Last_Sent_ACU_Var_Offset[] array.
 * @param first_index = The first index that needs updating.
 * @param delta = The amount of change to add.
 */
static void ShiftOffsetsRight(uint8 first_index, uint8 delta)
{
    for (uint8 variable_index = first_index; variable_index <= API010_ACU_MAPPED_VAR_COUNT; variable_index++)
    {
        Api010.Last_Sent_ACU_Var_Offset[variable_index] += delta;
    }
}
#endif


#if(defined(HMI_VARIABLE_ID_USERS_ENUM)&&(API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U))
//---------------------------------------------------------------------------------------------------------------------
/**
 * Insert the specified variable into the on change monitoring.
 * @param variable_index = The variable to insert into "on change" monitoring.
 * @return TRUE if the specified variable was inserted into "on change" monitoring. Otherwise FALSE.
 */
static BOOL_TYPE OnChangeInsertHmiVariable(uint8 variable_index)
{
    BOOL_TYPE result = FALSE;

    if ((variable_index < API010_FIXED_VARS_HMI_COUNT)&&
            (Api010.HMI_Control[variable_index].On_Change == FALSE))
    {
        // Make sure there is space in the Last_Sent_ACU_Var[] array for this variable.
        uint16 space_consumed = Api010.Last_Sent_HMI_Var_Offset[API010_FIXED_VARS_HMI_COUNT];
        uint16 space_available = API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE - space_consumed;
        API010_VARIABLE_TYPE variable;
        if (GetVariableInfo(variable_index, &variable) &&
            (variable.Size <= space_available))
        {
            // Update all the offsets to the Last_Sent_ACU_Var[] data.
            ShiftOffsetsRightHmiVariable(variable_index + 1U, variable.Size);

            // Setup automatic variables to reference the variable's Last_Sent_ACU_Var value.
            uint16 var_offset = Api010.Last_Sent_HMI_Var_Offset[variable_index];
            uint8* var_data = &Api010.Last_Sent_HMI_Var[var_offset];

            // Shift the Last_Sent_ACU_Var[] values to make space for the new variable.
            if (variable_index < VARIABLE_NUM_HMI_VARIABLES )
            {
                // Shift the other values to make space for this variable.
                uint8* move_to = &Api010.Last_Sent_HMI_Var[var_offset + variable.Size];
                uint16 bytes_to_shift = space_consumed - var_offset;
                memmove(move_to, var_data, bytes_to_shift);
            }

            // Update the Control of the variable so that it is selected, monitored, and changed.
            if(variable_index < API010_FIXED_VARS_HMI_COUNT)
            {
                Api010.HMI_Control[variable_index].Selected = TRUE;
                Api010.HMI_Control[variable_index].On_Change = TRUE;
                memcpy(var_data, variable.Address, variable.Size);
                Api010.HMI_Control[variable_index].Changed = TRUE;
                result = TRUE;
            }
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove the specified variable from the on change monitoring.
 * @param variable_index = The variable to remove from "on change" monitoring.
 */
static void OnChangeRemoveHmiVariable(uint8 variable_index)
{
    if ((variable_index < API010_FIXED_VARS_HMI_COUNT) &&
            (Api010.HMI_Control[variable_index].On_Change != FALSE ))
    {
        // Get the variable information.
        API010_VARIABLE_TYPE variable;
        if (GetVariableInfo(variable_index, &variable))
        {
            // Update all the offsets to the Last_Sent_ACU_Var[] data.
            ShiftOffsetsLeftHmiVariable(variable_index + 1U, variable.Size);

            // Setup automatic variables to reference the variable's Last_Sent_ACU_Var value.
            uint16 var_offset = Api010.Last_Sent_HMI_Var_Offset[variable_index];
            uint8* var_data = &Api010.Last_Sent_HMI_Var[var_offset];

            // Shift the Last_Sent_ACU_Var[] values to collapse the space that is no longer needed.
            if (variable_index < API010_FIXED_VARS_HMI_COUNT)
            {
                // Shift the other values to make space for this variable.
                uint16 space_consumed = Api010.Last_Sent_HMI_Var_Offset[API010_FIXED_VARS_HMI_COUNT];
                uint8* move_from = &var_data[variable.Size];
                uint16 bytes_to_shift = space_consumed - var_offset;
                memmove(var_data, move_from, bytes_to_shift);
            }

            // Update the Control of the variable so that it is not monitored or changed.
            if(variable_index < API010_FIXED_VARS_HMI_COUNT)
            {
                Api010.HMI_Control[variable_index].On_Change = FALSE;
                Api010.HMI_Control[variable_index].Changed = FALSE;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the "on change" behavior for one or more variables.
 * There are no checks for out of range values.
 *      first_index should be less than or equal to last_index.
 *      Both values should be in the range [0 .. API010_VARS_COUNT).
 * @param on_change = Desired "on change" behavior for the selected variable(s).
 * @param first_index = First variable index in the range of variables to affect.
 * @param last_index = Last variable index in the range of variables to affect.
 * @return TRUE if the on change behavior was updated. Otherwise FALSE.
 */
static BOOL_TYPE SetOnChangeBehaviorHmiVariable(BOOL_TYPE on_change, uint8 first_index, uint8 last_index)
{
    uint16 variables_added = 0U;
    for (uint8 variable_index = first_index; variable_index <= last_index; variable_index++)
    {
        if (on_change)
        {
            if (Api010.HMI_Control[variable_index].Selected &&
                OnChangeInsertHmiVariable(variable_index))
            {
                variables_added++;
            }
        }
        else
        {
            OnChangeRemoveHmiVariable(variable_index);
        }
    }
    return (BOOL_TYPE)((on_change == FALSE) || (variables_added > 0U));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add or subtract a delta value from the Last_Sent_ACU_Var_Offset[] array.
 * @param first_index = The first index that needs updating.
 * @param delta = The amount of change to subtract.
 */
static void ShiftOffsetsLeftHmiVariable(uint8 first_index, uint8 delta)
{
    for (uint8 variable_index = first_index; variable_index <= API010_FIXED_VARS_HMI_COUNT; variable_index++)
    {
        Api010.Last_Sent_HMI_Var_Offset[variable_index] -= delta;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add or subtract a delta value from the Last_Sent_ACU_Var_Offset[] array.
 * @param first_index = The first index that needs updating.
 * @param delta = The amount of change to add.
 */
static void ShiftOffsetsRightHmiVariable(uint8 first_index, uint8 delta)
{
    for (uint8 variable_index = first_index; variable_index <= API010_FIXED_VARS_HMI_COUNT ; variable_index++)
    {
        Api010.Last_Sent_HMI_Var_Offset[variable_index] += delta;
    }
}

#endif  // #if ((HMI_VARIABLE_FEATURE == ENABLED)&&(API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE > 0U))


#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Send the next packet of metadata information.
 * If Reveal returns an error, then the function will try to send the same packet on the next call.
 */
static void PublishNextMetadata(void)
{
    // Determine the size of the payload.
    uint16 metadata_in_packet = Api010.Remaining_Metadata;
    if (metadata_in_packet > API010_MAXIMUM_METADATA_PER_PACKET)
    {
        metadata_in_packet = API010_MAXIMUM_METADATA_PER_PACKET;
    }
    uint8 payload_size = 0U;
    if(Api010.Is_Hmi_Variable_ID == FALSE)
    {
        payload_size = (uint8)(metadata_in_packet * sizeof(API010_VARIABLE_METADATA_TYPE));
    }
    else
    {
        payload_size = (uint8)((metadata_in_packet * sizeof(API010_VARIABLE_METADATA_TYPE)) + 1U);
    }
    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA, Api010.Reply_To, payload_size);

    // Determine the number of variables whose metadata will fit in the payload.
    if (msg != NULL)
    {
        BOOL_TYPE success = TRUE;       // Assume no errors while building the packet.
        if(Api010.Is_Hmi_Variable_ID == FALSE)
        {
            uint8 first_variable_in_packet = API010_FIXED_VARS_ACU_COUNT - (uint8)Api010.Remaining_Metadata;
            for (uint8 i = 0U; i < metadata_in_packet; i++)
            {
                uint8 variable_index = first_variable_in_packet + i;
                if (RevealAppendMetadata(msg, variable_index) == FAIL)
                {
                    success = FALSE;
                    break;
                }
            }
        }
	#ifdef HMI_VARIABLE_ID_USERS_ENUM
        else
        {
            uint8 first_variable_in_packet = API010_FIXED_VARS_HMI_COUNT - (uint8)Api010.Remaining_Metadata;
            if(Reveal__AddDataU8(msg, HMI_MARKER) == PASS)
            {
                for (uint8 i = 0U; i < metadata_in_packet; i++)
                {
                    uint16 variable_index = (uint16)VARIABLE_HMI_START_INDEX + first_variable_in_packet + i   ;
                    if (RevealAppendMetadata(msg, variable_index) == FAIL)
                    {
                        success = FALSE;
                        break;
                    }
                }
            }
        }
	#endif
        if (success)
        {
            // If we have more messages to send, we set the MMP flag.
            if (metadata_in_packet < Api010.Remaining_Metadata)
            {
                BIT_SET(msg->opcode, 7U);
            }
            //Queuing and updating the index.
            if (Reveal__QueueCommand(msg))
            {
                Api010.Remaining_Metadata -= metadata_in_packet;
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Append the specified variable's metadata information to the provided Reveal message.
 * @param variable_index = The index of the requested variable.
 * @param msg = Pointer to the previously allocated Reveal message. This value must not be NULL.
 * @return PASS if the metadata was successfully appended. Otherwise FAIL.
 */
static PASS_FAIL_TYPE RevealAppendMetadata(REVEAL_COMMAND_TYPE *msg, uint16 variable_index)
{
    PASS_FAIL_TYPE result = FAIL;
    VARIABLE_TYPE variable;
    API010_VARIABLE_METADATA_TYPE metadata;
    memset(&metadata, 0U, sizeof(API010_VARIABLE_METADATA_TYPE));

    if (variable_index < API010_FIXED_VARS_ACU_COUNT)
    {
        if (Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable))
        {
            metadata.Variable_Index = (uint8)variable_index;
            metadata.Variable_Array_Elements = variable.Length;
            metadata.Variable_Element_Type = variable.Type;
            metadata.Key = variable_index;
            result = Reveal__AddDataU8Buffer(msg, (uint8*)(void*)&metadata, sizeof(metadata));
        }
    }
#ifdef HMI_VARIABLE_ID_USERS_ENUM
    else
    {
        if (Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable))
        {
            uint8 hmi_variable = (uint8)(variable_index-(uint16)VARIABLE_HMI_START_INDEX);
            metadata.Variable_Index = hmi_variable;
            metadata.Variable_Array_Elements = variable.Length;
            metadata.Variable_Element_Type = variable.Type;
            metadata.Key = hmi_variable;
            result = Reveal__AddDataU8Buffer(msg, (uint8*)(void*)&metadata, sizeof(metadata));
        }
    }
#endif
    return result;
}
#endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)


#if (API010_MMAP_VARS_COUNT > 0U)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove the specified memory-mapped variable from the list of variables in the periodic data.
 * @param mmap_index = An index that identifies the memory-mapped variable configuration slot.
 *                     This value must be in the range [0..API010_MMAP_VARS_COUNT).
 *                     A value not in this range will have no effect.
 */
static void RemoveMemoryMappedVariable(uint8 mmap_index)
{
    if (mmap_index < API010_MMAP_VARS_COUNT)
    {
        memset(&Api010.Mem_Mapped_Vars[mmap_index], 0U, sizeof(API010_VARIABLE_TYPE));
        Api010.ACU_Control[mmap_index + API010_FIXED_VARS_ACU_COUNT].Selected = FALSE;
        #if (API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE > 0U)
            Api010.ACU_Control[mmap_index + API010_FIXED_VARS_ACU_COUNT].On_Change = FALSE;
        #endif
    }
}
#endif      // #if (API010_MMAP_VARS_COUNT > 0U)


//=====================================================================================================================
// API 10 Publisher State Machine customization functions.
// NOTE: do not modify this header or the StateMachineCodeGenerator will not be able to find it.
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if any data is due for publication.
 * First checks the time to see if the periodic data should be published.
 * Then checks for any changes in variables that are monitored "on change".
 * If data is due for publication, then it will generate the evProceed event.
 *  @param  fsm_var = Pointer to the state machine data structure.
 */
static void CheckDataDue(API10PUBLISHER_FSM_VAR_TYPE * fsm_var)
{
    if (Api010.Is_Periodic_Data_Due)
    {
        Api010.Is_Periodic_Data_Due = FALSE;
        // Start publishing the periodic data.
        if (BuildStream(TRUE))
        {
            API10PublisherEvent(EVENT_START_PUBLISHING, fsm_var);
        }
    }
    else if (IsOnChangeDataDue())
    {
        // Start publishing the changed data.
        if (BuildStream(FALSE))
        {
            API10PublisherEvent(EVENT_START_PUBLISHING, fsm_var);
        }
    }
    else
    {
        // Check if there is still variable metadata that must be sent.
        CheckMetadataDue(fsm_var);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if more variable metadata should be sent out.
 *  @param  fsm_var = Pointer to the state machine data structure.
 */
static void CheckMetadataDue(API10PUBLISHER_FSM_VAR_TYPE * fsm_var)
{
    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        // Check if there is still variable metadata that must be sent.
        if (Api010.Remaining_Metadata > 0U)
        {
            PublishNextMetadata();
        }
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Publish the next Reveal packet that makes up the periodic data.
 * If Reveal returns an error, then the function will try to send the same packet on the next call.
 *  @param  fsm_var = Pointer to the state machine data structure.
 */
static void PublishNextPacket(API10PUBLISHER_FSM_VAR_TYPE * fsm_var)
{
    switch (Api010.Method)
    {
        case API010_METHOD_REVEAL:
            if (Api010.Stream_Index < Api010.Stream_Length)
            {
                // Check if the rest of the stream will fit in a single Reveal packet.
                uint8 size;
                API010_STREAM_INDEX_TYPE remaining_stream_size = Api010.Stream_Length - Api010.Stream_Index;
                if (remaining_stream_size > ((uint8)REVEAL_PAYLOAD_SIZE - sizeof(uint16)))
                {
                    // It does not fit. Send as must as will fit.
                    size = (uint8)REVEAL_PAYLOAD_SIZE - sizeof(uint16);
                }
                else
                {
                    // It does fit. Send the whole thing.
                    size = (uint8)remaining_stream_size;
                }

                // Prepare the Reveal message.
                REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM,
                                                               API010_FBK_PUBLISH_DATA_STREAM,
                                                               REVEAL_BROADCAST_NODE,
                                                               size + sizeof(API010_STREAM_INDEX_TYPE));
                if ((Reveal__AddDataU16(msg, Api010.Stream_Index) == PASS) &&
                    (Reveal__AddDataU8Buffer(msg, &Api010.Stream_Buffer[Api010.Stream_Index], size) == PASS) &&
                    (Reveal__QueueCommand(msg)))
                {
                    // Update the number of bytes sent.
                    Api010.Stream_Index += size;
                }
            }
            else
            {
                // Send the termination packet and return to the STATE_API_10_PUBLISHER_ACTIVE state.
                REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_DATA_STREAM, REVEAL_BROADCAST_NODE, 3U);
                if ((Reveal__AddDataU16(msg, 0xFFFF) == PASS) &&
                    (Reveal__AddDataU8(msg, Api010.Stream_Csum) == PASS) &&
                    (Reveal__QueueCommand(msg)))
                {
                    API10PublisherEvent(EVENT_STOP_PUBLISHING, fsm_var);
                }
            }
            break;

        case API010_METHOD_SPI:
            #if (API010_CAN_USE_SPI_MODULE == ENABLED)
                if (Spi__Write(API010POLLVAR_SPI_CHANNEL, Api010.Stream_Buffer, Api010.Stream_Length))
                {
                    API10PublisherEvent(EVENT_STOP_PUBLISHING, fsm_var);
                }
            #elif (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
                if (SpiMgr__QueueMsg(API010_SPIMGR_DEVICE, Api010.Stream_Buffer, Api010.Rx_Buffer, Api010.Stream_Length))
                {
                    API10PublisherEvent(EVENT_STOP_PUBLISHING, fsm_var);
                }
            #else
                API10PublisherEvent(EVENT_STOP_PUBLISHING, fsm_var);
            #endif
            break;

        default:
            API10PublisherEvent(EVENT_STOP_PUBLISHING, fsm_var);
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset API010 Publisher variables.
 * Restarts the timer for periodic data.
 * Re-enables publication of errors.
 *  @param  fsm_var = Pointer to the state machine data structure.
 */
static void ResetPublisher(API10PUBLISHER_FSM_VAR_TYPE * fsm_var)
{
    Api010.Publication_Counter = 0U;
    Api010.Enable_Error_Reporting = TRUE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the timer that keeps track of when to send the next periodic data.
 *  @param  fsm_var = Pointer to the state machine data structure.
 */
static void UpdateTimer(API10PUBLISHER_FSM_VAR_TYPE * fsm_var)
{
    uint8 new_counter = (uint8)(Millis__GetFreeRunningCounter() / Api010.Publication_Period);
    if (Api010.Publication_Counter != new_counter)
    {
        Api010.Publication_Counter = new_counter;
        Api010.Is_Periodic_Data_Due = TRUE;
    }
}


#endif      // #if (API010POLLVAR_FEATURE == ENABLED)
