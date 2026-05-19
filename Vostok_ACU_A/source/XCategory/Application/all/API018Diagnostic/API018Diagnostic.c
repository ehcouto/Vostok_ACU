/**
 *  @file
 *
 *  @brief      API Responsible to Expose Fault Codes and Statistics information.
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))

#include "API018Diagnostic.h"
#include "API018Diagnostic_prv.h"

// -- Other Modules --
#include <limits.h>
#include <string.h>
#include "Utilities.h"
#if (API018_SERVER_FEATURE == ENABLED)
    #include "Statistics.h"
#endif
#if (API018_CLIENT_FEATURE == ENABLED)
    #include "HmiFaultMgr.h"
#endif


// -- Compile-Time Assertions --


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The bit that represents a request for version numbers in the payload of the
//! API018_CMD_REQUEST_STATISTICS OpCode.
#define API018_USE_VERSION_NUMBERS 7

#ifndef REVEAL_MMP_BIT
    //! An OpCode bit indicating that more Reveal messages are expected after the current message.
    #define REVEAL_MMP_BIT          ((uint8)0x80)
#endif

#if (API018_SERVER_FEATURE == DISABLED)
    #ifndef API018_MAX_STORED_FAULTS
        //! The maximum number of fault error codes that the API018_CLIENT will hold.
        //! These fault error codes are transferred from the API018_SERVER at run-time.
        #define API018_MAX_STORED_FAULTS    5
    #endif
#endif

#ifndef STATISTICS_CROSS_PLATFORM_VERSION
    #define STATISTICS_CROSS_PLATFORM_VERSION 0
#endif
#ifndef STATISTICS_PLATFORM_SPECIFIC_VERSION
    #define STATISTICS_PLATFORM_SPECIFIC_VERSION 0
#endif


// -- Private Enumerated Constant Declarations --

//! All the API018 command and feedback OpCodes.
typedef enum API018DIAGNOSTIC_OPCODE_ENUM
{
    API018_CMD_REQUEST_STATISITCS_VERSION   = 1,    //!< Request the Statistics version numbers.
    API018_FBK_PUBLISH_STATISTICS_VERSION   = 1,    //!< Publish the Statistics version numbers.
    API018_CMD_GENERATE_FAULT               = 2,    //!< Generate a fault.
    API018_CMD_CLEAR_SAVED_FAULTS           = 3,    //!< Clear all fault information.
    API018_CMD_REMOVE_FAULT                 = 4,    //!< Remove an active fault.
    API018_CMD_REQUEST_FAULT_CODES          = 5,    //!< Request fault codes for all recorded faults.
    API018_FBK_PUBLISH_FAULT_CODES          = 5,    //!< Publish fault codes for all recorded faults.
    API018_CMD_REQUEST_STATISTICS           = 6,    //!< Request statistics data.
    API018_FBK_PUBLISH_STATISTICS           = 6,    //!< Publish statistics data.
    API018_CMD_REQUEST_COMMON_LIFETIME_DATA = 7,    //!< Request cycle data.
    API018_FBK_PUBLISH_COMMON_LIFETIME_DATA = 7,    //!< Publish cycle data.

    API018_CMD_REQUEST_DIAGNOSTIC_CONFIG    = 30,   //!< Request diagnostic configuration information.
    API018_FBK_PUBLISH_DIAGNOSTIC_CONFIG    = 30    //!< Publish diagnostic configuration information.
} API018DIAGNOSTIC_OPCODE_TYPE;

//! Enumerated list of the sections of the Statistics data.
//! The order of these sections must match the order of the sections in the Statistics structure.
typedef enum API018_SECTION_ENUM
{
    API018_INACTIVE                     = 0,
    API018_SECTION_HEADER               = 0,

    API018_SECTION_FIRST                = 1,

    API018_SECTION_LIFETIME             = 1,
    API018_SECTION_CYCLES               = 2,
    API018_SECTION_FAULTS               = 3,
    API018_SECTION_PRE_FAULTS           = 4,
    API018_SECTION_LAST_CYCLES          = 5,
    API018_SECTION_FAULTS_BLACK_BOX     = 6,
    API018_SECTION_PRE_FAULTS_BLACK_BOX = 7,

    API018_SECTION_LAST                 = 7,

    API018_SECTION_NONE_REMAINING       = 0x20,

    API018_SECTION_ALL                  = 0x40,     //!< Request for all statistics sections.
} API018_SECTION_TYPE;


// -- Private Structure Type Declarations --

//! Data structure for the variables required by the API018Diagnostic module.
typedef struct API018_DIAGNOSTIC_DATA_STRUCT
{
    #if (API018_SERVER_FEATURE == ENABLED)
        //! The offset into the section of the statistics data being sent.
        //! This value is used to remember at what position (mid structure) the next statistics data packet should start.
        uint16 Sending_Section_Offset;

        //! The offset into the full set of statistics data being sent.
        //! This value is used as the 2nd & 3rd bytes (big-endian) of every statistics data packet.
        uint16 Sending_Total_Offset;

        //! This is the section of the statistics data that was requested.
        //! A value of 255 indicates that all sections were requested.
        //! Refer to API018_SECTION_TYPE for section numbers.
        //! This value is used as the first byte of every statistics data packet.
        API018_SECTION_TYPE Requested_Sections;

        //! These are the sections of the statistics data that are being published.
        //! Each bit represents a section. Refer to API018_SECTION_TYPE for bit values.
        //! As the sections are sent out, the bits are cleared.
        uint8 Sending_Sections_Mask;

        //! The node that requested the statistics data.
        uint8 Requesting_Node;
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        //! An array of error codes that have occurred on the server.
        API018_ERROR_CODE_TYPE Error_Codes[API018_MAX_STORED_FAULTS];
        //! A structure that holds the common lifetime data
        API018_COMMON_LIFETIME_TYPE Common_Lifetime_Data;
    #endif
    #if (API018_CLIENT_FEATURE == ENABLED)
        //! The number of error codes received and stored in the Error_Codes[] array.
        uint8 Number_Of_Error_Codes;
    #endif
} API018_DIAGNOSTIC_DATA_TYPE;

//! The data structure repeated for each statistics section in the header section.
typedef struct API018_HEADER_ENTRY_STRUCT
{
    uint8 Number_Of_Elements;
    uint8 Size_Of_Elements;
} API018_HEADER_ENTRY_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

#if (API018_SERVER_FEATURE == ENABLED)
//! A packet that conveys platform-specific information about diagnostic information.
//! This is the payload of the API018_FBK_PUBLISH_DIAGNOSTIC_CONFIG message.
static const PACKED API018_HEADER_ENTRY_TYPE API018_HEADER_PACKET[] =
{
    {
        1,                                          // Number of lifetime data elements
        sizeof(STATISTICS_LIFETIME_TYPE)            // Size of each lifetime data element
    },
    {
        STATISTICS_SAVED_CYCLES,                    // Number of saved cycle data elements
        #if (STATISTICS_SAVED_CYCLES > 0)
            sizeof(STATISTICS_CYCLE_TYPE)           // Size of each saved cycle data element
        #else
            0
        #endif
    },
    {
        STATISTICS_SAVED_FAULTS,                    // Number of fault data elements
        sizeof(STATISTICS_FAULT_TYPE)               // Size of each fault data element
    },
    {
        STATISTICS_SAVED_PRE_FAULTS,                // Number of pre-fault data elements
        sizeof(STATISTICS_FAULT_TYPE)               // Size of each pre-fault data element
    },
    {
        STATISTICS_SAVED_LAST_CYCLES,               // Number of saved last cycle data elements
        #if (STATISTICS_SAVED_LAST_CYCLES > 0)
            sizeof(PLATFORM_LAST_CYCLE_TYPE)        // Size of each saved last cycle data element
        #else
            0
        #endif
    },
    {
        STATISTICS_SAVED_FAULT_BLACK_BOX,           // Number of black box data elements for faults
        #if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
            sizeof(PLATFORM_BLACK_BOX_TYPE)         // Size of each saved black box data element
        #else
            0
        #endif
    },
    {
        STATISTICS_SAVED_PRE_FAULT_BLACK_BOX,       // Number of black box data elements for pre-faults
        #if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
            sizeof(PLATFORM_BLACK_BOX_TYPE)         // Size of each saved black box data element
        #else
            0
        #endif
    }
};
CT_ASSERT(ELEMENTS_IN_ARRAY(API018_HEADER_PACKET) == (API018_SECTION_LAST - API018_SECTION_FIRST) + 1);
#endif      // #if (API018_SERVER_FEATURE == ENABLED)


// -- Private Variable Definitions --

//! The variables required by the API018Diagnostic module.
static API018_DIAGNOSTIC_DATA_TYPE Api018;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

#if (API018_SERVER_FEATURE == ENABLED)
    static BOOL_TYPE GenerateFault(uint8 node, API018_FAULT_KEY_TYPE fault_key);
    static API018_SECTION_TYPE GetNextSectionID(uint8 requested);
    static void* GetSectionAddress(API018_SECTION_TYPE requested);
    static uint16 GetSectionSize(API018_SECTION_TYPE section);
    static BOOL_TYPE RemoveFault(uint8 node, API018_FAULT_KEY_TYPE fault_key);
#endif      // #if (API018_SERVER_FEATURE == ENABLED)


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the API018Diagnostic module variables.
 */
void API018Diagnostic__Initialize(void)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        memset(&Api018, 0, sizeof(Api018));
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        memset(&Api018, UCHAR_MAX, sizeof(Api018));
    #endif
    #if (API018_CLIENT_FEATURE == ENABLED)
        Api018.Number_Of_Error_Codes = 0;
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Background function for the API018Diagnostic module.
 */
void API018Diagnostic__Handler25ms(void)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        if (Api018.Requested_Sections != API018_INACTIVE)
        {
            // Setup local variables to build the transmission packet.
            // If any failures are detected, the static variables are unchanged and we can try again after 25ms.
            uint8  sending_sections       = Api018.Sending_Sections_Mask ;
            uint16 sending_section_offset = Api018.Sending_Section_Offset;
            uint16 sending_total_offset   = Api018.Sending_Total_Offset  ;

            // Setup a message packet for transmission with the requested sections and the buffer offset.
            REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API018DIAGNOSTIC_NUM, API018_FBK_PUBLISH_STATISTICS, Api018.Requesting_Node, REVEAL_PAYLOAD_SIZE);
            uint8 payload_remaining = REVEAL_PAYLOAD_SIZE;
            PASS_FAIL_TYPE success = PASS;
            if (Api018.Sending_Total_Offset == 0)
            {
                // This is the first packet. Include the section(s) and the size of the section(s).
                success = Reveal__AddDataU8(msg, Api018.Requested_Sections);
                payload_remaining -= sizeof(uint8);
                BOOL_TYPE use_version_numbers = (BOOL_TYPE)BIT_TEST(Api018.Requested_Sections, API018_USE_VERSION_NUMBERS);
                if (use_version_numbers)
                {
                    success &= Reveal__AddDataU8(msg, STATISTICS_CROSS_PLATFORM_VERSION);
                    success &= Reveal__AddDataU8(msg, STATISTICS_PLATFORM_SPECIFIC_VERSION);
                    payload_remaining -= 2 * sizeof(uint8);
                }
                API018_SECTION_TYPE section = (API018_SECTION_TYPE)(Api018.Requested_Sections & ~(1 << API018_USE_VERSION_NUMBERS));
                uint16 total_size = GetSectionSize(section);
                success &= Reveal__AddDataU16(msg, total_size);
                payload_remaining -= sizeof(uint16);
            }

            // Add data to the message until the payload is full or there are no more statistics sections.
            while ((success == PASS) && (payload_remaining > 0) && (sending_sections > 0))
            {
                // Get information on the next section that must be sent.
                API018_SECTION_TYPE section = GetNextSectionID(sending_sections);
                uint8* section_address = GetSectionAddress(section);
                uint16 section_size = 0;
                if (section != API018_SECTION_NONE_REMAINING)
                {
                    section_size = API018_HEADER_PACKET[section - 1].Number_Of_Elements * API018_HEADER_PACKET[section - 1].Size_Of_Elements;
                }
                if ((section_address == NULL) || (section_size == 0))
                {
                    // Empty section was passed successfully.
                    BIT_CLR(sending_sections, section);
                }
                else
                {
                    // Determine what part of the section fits in the payload.
                    uint16 section_remaining = section_size - sending_section_offset;
                    uint8 section_transmit = ((section_remaining <= payload_remaining) ? section_remaining : payload_remaining);

                    // Send part (or all) of the next section.
                    success = Reveal__AddDataU8Buffer(msg, &section_address[sending_section_offset], section_transmit);
                    if (success == PASS)
                    {
                        if (section_transmit == section_remaining)
                        {
                            // All of the statistics section was sent.
                            BIT_CLR(sending_sections, section);
                            sending_section_offset = 0;
                        }
                        else
                        {
                            // Only part of the statistics section was sent.
                            sending_section_offset += section_transmit;
                        }
                        sending_total_offset += section_transmit;
                    }
                    payload_remaining -= section_transmit;
                }
            }

            if (success == PASS)
            {
                if (sending_sections > 0)
                {
                    msg->opcode |= REVEAL_MMP_BIT;
                }
                if (Reveal__QueueCommand(msg) == TRUE)
                {
                    if (sending_sections > 0)
                    {
                        // Save status for the next statistics data packet.
                        Api018.Sending_Sections_Mask  = sending_sections      ;
                        Api018.Sending_Section_Offset = sending_section_offset;
                        Api018.Sending_Total_Offset   = sending_total_offset  ;
                    }
                    else
                    {
                        Api018.Requested_Sections = API018_INACTIVE;
                    }
                }
            }
        }
    #endif      // #if (API018_SERVER_FEATURE == ENABLED)
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API018 commands sent to a broadcast node or exclusively to this node.
 * @param cmd = Pointer to the received command message.
 * @return REVEAL_RECIPE_STATUS_TYPE that expresses the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API018Diagnostic__CommandParser(REVEAL_MSG_TYPE* cmd)
{
    REVEAL_RECIPE_STATUS_TYPE reveal_response = NVO_RS_UNSUP_UNAVAILABLE;

    switch ((API018DIAGNOSTIC_OPCODE_TYPE)cmd->op_code)
    {
    #if (API018_SERVER_FEATURE == ENABLED)
        case API018_CMD_REQUEST_STATISITCS_VERSION:
            // No payload
            {
                REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(API018DIAGNOSTIC_NUM, API018_FBK_PUBLISH_STATISTICS_VERSION, cmd->node_id, 2);
                if ((Reveal__AddDataU8(fbk, STATISTICS_CROSS_PLATFORM_VERSION) == PASS) &&
                    (Reveal__AddDataU8(fbk, STATISTICS_PLATFORM_SPECIFIC_VERSION) == PASS) &&
                    (Reveal__QueueCommand(fbk) == TRUE))
                {
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API018_CMD_GENERATE_FAULT:
            // Client Fault Key (U16).
            if (cmd->size >= 2)
            {
                API018_FAULT_KEY_TYPE fault_key = (API018_FAULT_KEY_TYPE)Utilities__ConvertArrayTo16bits(&cmd->payload[0]);
                if (GenerateFault(cmd->node_id, fault_key) == TRUE)
                {
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API018_CMD_CLEAR_SAVED_FAULTS:
            // No payload.
            Fault__ResetAllObservers();
            Statistics__ClearFaultData();
            reveal_response = NVO_RS_READY;
            break;

        case API018_CMD_REMOVE_FAULT:
            // Client Fault Key (U16).
            if (cmd->size >= 2)
            {
                API018_FAULT_KEY_TYPE fault_key = (API018_FAULT_KEY_TYPE)Utilities__ConvertArrayTo16bits(&cmd->payload[0]);
                if (RemoveFault(cmd->node_id, fault_key) == TRUE)
                {
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API018_CMD_REQUEST_FAULT_CODES:
            // No payload = Send all fault codes.
            // Or expect one parameter.
            //      Fault Index (U8) 0 = Most recently fault, 1 = Next most recent fault, ..., 255 = Send all fault codes.
            {
                uint8 index = ((cmd->size >= 1) ? cmd->payload[0] : 255);
                if (API018Diagnostic__PublishFaultBasic(index) == PASS)
                {
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API018_CMD_REQUEST_STATISTICS:
            // No payload = Send all statistics data.
            // Or expect one parameter with which section should be published.
            //      Section Number (U8) 1 = Section 1, 2 = Section 2, etc.
            if (Api018.Requested_Sections == API018_INACTIVE)
            {
                // Save the request byte to echo back in the response.
                // Valid values are [0x01..0x07,0x80..0x87]
                uint8 requested = ((cmd->size > 0) ? cmd->payload[0] : API018_SECTION_ALL);

                // Determine which section is requested.
                uint8 section = requested;
                BIT_CLR(section, API018_USE_VERSION_NUMBERS);
                section = (section ? section : API018_SECTION_ALL);

                // Validate the request.
                if (((API018_SECTION_FIRST <= section) && (section <= API018_SECTION_LAST)) ||
                    (section == API018_SECTION_ALL))
                {
                    Api018.Requesting_Node = cmd->node_id;
                    Api018.Requested_Sections = (API018_SECTION_TYPE)requested;
                    if (section == API018_SECTION_ALL)
                    {
                        #define API018_SECTION_MASK_ALL ((1 << (API018_SECTION_LAST + 1)) - 2)
                        Api018.Sending_Sections_Mask = API018_SECTION_MASK_ALL;
                    }
                    else
                    {
                        Api018.Sending_Sections_Mask = (1 << section);
                    }
                    Api018.Sending_Section_Offset = 0;
                    Api018.Sending_Total_Offset = 0;
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case  API018_CMD_REQUEST_COMMON_LIFETIME_DATA:
            // No payload.
            {
                PASS_FAIL_TYPE result = PASS;
                #if (STATISTICS_SAVED_CYCLES > 0)
                    REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(API018DIAGNOSTIC_NUM, API018_FBK_PUBLISH_COMMON_LIFETIME_DATA, REVEAL_BROADCAST_NODE, 10);
                #else
                    REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(API018DIAGNOSTIC_NUM, API018_FBK_PUBLISH_COMMON_LIFETIME_DATA, REVEAL_BROADCAST_NODE, 6);
                #endif
                STATISTICS_LIFETIME_TYPE* cycle_lifetime_data = Statistics__GetLifetimeData();
                #if (STATISTICS_SAVED_CYCLES > 0)
                    // Get cycles started if it is available.
                    uint16 cycle_count = cycle_lifetime_data->Cycles_Started;
                #else
                    // Get cycles completed if cycles started is not available.
                    uint16 cycle_count = cycle_lifetime_data->Cycles_Completed;
                #endif
                // Send lifetime seconds and cycle count.
                if ((Reveal__AddDataU32(fbk, cycle_lifetime_data->Seconds) != PASS) ||
                    (Reveal__AddDataU16(fbk, cycle_count) != PASS))
                {
                    result = FAIL;
                }
                #if (STATISTICS_SAVED_CYCLES > 0)
                    // Send cycle run-time if available.
                    if (Reveal__AddDataU32(fbk, cycle_lifetime_data->Seconds_Running_Cycles) != PASS)
                    {
                        result = FAIL;
                    }
                #endif
                // Transmit the message.
                if (result == PASS)
                {
                    if (Reveal__QueueCommand(fbk) == TRUE)
                    {
                        reveal_response = NVO_RS_READY;
                    }
                }
            }
            break;

        case API018_CMD_REQUEST_DIAGNOSTIC_CONFIG:
            // No payload.
            {
                REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(API018DIAGNOSTIC_NUM, API018_FBK_PUBLISH_DIAGNOSTIC_CONFIG, cmd->node_id, sizeof(API018_HEADER_PACKET));
                if ((Reveal__AddDataU8Buffer(fbk, (void*)API018_HEADER_PACKET, sizeof(API018_HEADER_PACKET)) == PASS) &&
                    (Reveal__QueueCommand(fbk) == TRUE))
                {
                    reveal_response = NVO_RS_READY;
                }
            }
            break;
    #endif      // #if (API018_SERVER_FEATURE == ENABLED)

        default:
            reveal_response = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (reveal_response);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API018 feedbacks sent to a broadcast or exclusively to this node.
 * @param fbk = Pointer to the received feedback message.
 */
void API018Diagnostic__FeedbackParser(REVEAL_MSG_TYPE * fbk)
{
    #if ((API018_SERVER_FEATURE == DISABLED) && (API018_CLIENT_FEATURE == ENABLED))
        switch ((API018DIAGNOSTIC_OPCODE_TYPE)fbk->op_code)
        {
            case API018_FBK_PUBLISH_FAULT_CODES:
                // 4 bytes repeated:
                //      Fault Index (U8) 0 = Most recently recorded fault, 1 = Next most recently recorded fault, etc.
                //      Fault Code (U8)
                //      Fault Sub-Code (U8)
                //      Engineering Code (U8)
                Api018.Number_Of_Error_Codes = 0;
                memset(Api018.Error_Codes, UCHAR_MAX, sizeof(Api018.Error_Codes));
                for (uint8 i = 0; i < fbk->size; i += 4)
                {
                    uint8 fault_index = i / 4;
                    if (fault_index < API018_MAX_STORED_FAULTS)
                    {
                        API018_ERROR_CODE_TYPE* e = &Api018.Error_Codes[fault_index];
                        e->F_Code = fbk->payload[i + 1];
                        e->E_Code = fbk->payload[i + 2];
                        e->Eng_Code = fbk->payload[i + 3];
                        Api018.Number_Of_Error_Codes++;
                    }
                }
                HmiFaultMgr__ReceiveFaultData();
                break;

            case API018_FBK_PUBLISH_COMMON_LIFETIME_DATA:
                //Seconds-payload bytes 0 to 3
                //Cycles_Count-payload bytes 4 to 5
                //Seconds_Running_Cycle-payload bytes 6 to 9(When STATISTICS_SAVED_CYCLES>0)
                if (fbk->size >= 6)
                {
                    Api018.Common_Lifetime_Data.Seconds=Utilities__ConvertArrayTo32bits(&fbk->payload[0]);
                    Api018.Common_Lifetime_Data.Cycle_Count=Utilities__ConvertArrayTo16bits(&fbk->payload[4]);
                    if (fbk->size >= 10)
                    {
                        Api018.Common_Lifetime_Data.Seconds_Running_Cycles=Utilities__ConvertArrayTo32bits(&fbk->payload[6]);
                    }
                }
                break;

            default:
                break;
        }
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Acknowledge parser for API018.
 * @param node = Node address that acknowledged the command message.
 * @param api = The API of the previously sent command message.
 * @param opcode = The OpCode of the previously sent command message.
 * @param reasoncode = The response sent in response to the previously sent command message.
 */
void API018Diagnostic__AckParser(uint8 node, uint8 api, uint8 op_code, REVEAL_RECIPE_STATUS_TYPE reason_code)
{
}


#if (API018_SERVER_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Publish the fault code for the indexed fault.
 * @param requested_fault_index = Index into the list of stored faults.
 *                                255 indicates that all faults are requested.
 * @return PASS if the message was sent. Otherwise FAIL.
 */
PASS_FAIL_TYPE API018Diagnostic__PublishFaultBasic(uint8 requested_fault_index)
{
    PASS_FAIL_TYPE result = FAIL;

    // Request a feedback message buffer with the maximum data that may be needed.
    uint8 fault_count = Statistics__ApplyFaultAgeCutoff();
    REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(API018DIAGNOSTIC_NUM, API018_FBK_PUBLISH_FAULT_CODES, REVEAL_BROADCAST_NODE, fault_count * STATISTICS_SAVED_FAULTS);
    if (fbk != NULL)
    {
        // Iterate through the fault data.
        result = (PASS_FAIL_TYPE)(requested_fault_index == 255);
        STATISTICS_FAULT_TYPE* faults = Statistics__GetFaultData(0);
        for (uint8 fault_index = 0; fault_index < fault_count; fault_index++, faults++)
        {
            if ((requested_fault_index >= STATISTICS_SAVED_FAULTS) ||
                (requested_fault_index == fault_index))
            {
                result = PASS;
                if ((Reveal__AddDataU8(fbk, fault_index) != PASS) ||
                    (Reveal__AddDataU8(fbk, faults->Fault_Code) != PASS) ||
                    (Reveal__AddDataU8(fbk, faults->Fault_Sub_Code) != PASS) ||
                    (Reveal__AddDataU8(fbk, faults->Fault_Eng_Code) != PASS))
                {
                    // Failed to build the Reveal packet.
                    result = FAIL;
                    break;                              // Break out of the for() loop.
                }
            }
        }

        // Transmit the message.
        if (result == PASS)
        {
            if (Reveal__QueueCommand(fbk) != TRUE)
            {
                result = FAIL;
            }
        }
    }

    return result;
}


#else       // if only API018_CLIENT_FEATURE is ENABLED
//---------------------------------------------------------------------------------------------------------------------
/**
 * Called by the client to request lifetime data from the server.
 * @param node = The API018 server node.
 * @return PASS if the request was sent. Otherwise FAIL.
 */
PASS_FAIL_TYPE API018Diagnostic__RequestCommonLifeTimeData(uint8 node)
{
    return ((PASS_FAIL_TYPE)Reveal__QueueMessageNoData(API018DIAGNOSTIC_NUM, API018_CMD_REQUEST_COMMON_LIFETIME_DATA, node, TRUE, FALSE));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Called by the client to get lifetime data from the server.
 * @param node = The API018 server node.
 * @return addres if client feature enabled
 */
API018_COMMON_LIFETIME_TYPE* API018Diagnostic__GetCommonLifeTimeData(void)
{
    return (&Api018.Common_Lifetime_Data);
}
#endif      // #if (API018_SERVER_FEATURE == ENABLED)


#if (API018_CLIENT_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Called by the client to request that the server clear all fault data.
 * @param node = The API018 server node.
 * @return PASS if the request was sent. Otherwise FAIL.
 */
PASS_FAIL_TYPE API018Diagnostic__ClearFaults(uint8 node)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        Statistics__ClearFaultData();
        return PASS;
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        API018Diagnostic__Initialize();
        return (PASS_FAIL_TYPE)Reveal__QueueMessageNoData(API018DIAGNOSTIC_NUM, API018_CMD_CLEAR_SAVED_FAULTS, node, TRUE, FALSE);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Generate a fault on the API018_SERVER.
 * @param fault_key = The unique cross-category fault number.
 * @return PASS if the message was successfully sent to the Main ACU.
 */
PASS_FAIL_TYPE API018Diagnostic__GenerateFault(API018_FAULT_KEY_TYPE fault_key)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        return (PASS_FAIL_TYPE)((GenerateFault(WIN_ACU_ADDRESS, fault_key) == TRUE) ? PASS : FAIL);
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        return (PASS_FAIL_TYPE)Reveal__QueueMessageU16(API018DIAGNOSTIC_NUM, API018_CMD_GENERATE_FAULT, WIN_ACU_ADDRESS, TRUE, FALSE, fault_key);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the error code for the specified fault index.
 * @param fault_index = The requested fault index. 0 = most recent fault.
 * @return The error code for the specified fault index.
 *         If there is no error code at the specified fault index, then return Fault Code UCHAR_MAX.
 */
API018_ERROR_CODE_TYPE Api018Diagnostic__GetErrorCode(uint8 fault_index)
{
    API018_ERROR_CODE_TYPE e = { UCHAR_MAX, UCHAR_MAX, UCHAR_MAX };
    #if (API018_SERVER_FEATURE == ENABLED)
        uint8 fault_count = Statistics__ApplyFaultAgeCutoff();
        if (fault_index < fault_count)
        {
            STATISTICS_FAULT_TYPE* fault = Statistics__GetFaultData(fault_index);
            if ((fault != NULL) && (fault->Fault_Id != FAULT_ID_NONE))
            {
                FAULT_CONFIG_TYPE* fault_config = Fault__GetParameters((FAULT_ID_TYPE)fault->Fault_Id);
                if (fault_config != NULL)
                {
                    e.F_Code = fault_config->Code;
                    e.E_Code = fault_config->Sub_Code;
                    e.Eng_Code = fault_config->Engineering_Code;
                }
            }
        }
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        if ((fault_index < API018_MAX_STORED_FAULTS) &&
            (fault_index < Api018.Number_Of_Error_Codes))
        {
            e = Api018.Error_Codes[fault_index];
        }
    #endif
    return e;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of faults that have been received from the API018_SERVER.
 * @return The number of faults that have been received from the API018_SERVER.
 */
uint8 Api018Diagnostic__GetNumberOfErrorCodes(void)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        return Statistics__GetFaultCount();
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        return Api018.Number_Of_Error_Codes;
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove a fault on the API018_SERVER.
 * @param fault_key = The unique cross-category fault number.
 * @return PASS if the message was successfully sent to the Main ACU.
 */
PASS_FAIL_TYPE API018Diagnostic__RemoveFault(API018_FAULT_KEY_TYPE fault_key)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        return (PASS_FAIL_TYPE)((RemoveFault(WIN_ACU_ADDRESS, fault_key) == TRUE) ? PASS : FAIL);
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        return (PASS_FAIL_TYPE)Reveal__QueueMessageU16(API018DIAGNOSTIC_NUM, API018_CMD_REMOVE_FAULT, WIN_ACU_ADDRESS, TRUE, FALSE, fault_key);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Called by the client to request all fault codes from the server.
 * @param node = The API018 server node.
 * @param fault_index = Parameter is ignored. Kept for backward compatibility.
 * @return PASS if the request was sent. Otherwise FAIL.
 */
PASS_FAIL_TYPE API018Diagnostic__RequestFaultBasic(uint8 node, uint8 fault_index)
{
    #if (API018_SERVER_FEATURE == ENABLED)
        return PASS;
    #else   /* if only API018_CLIENT_FEATURE is ENABLED */
        return ((PASS_FAIL_TYPE)Reveal__QueueMessageNoData(API018DIAGNOSTIC_NUM, API018_CMD_REQUEST_FAULT_CODES, node, TRUE, FALSE));
    #endif
}
#endif      // #if (API018_CLIENT_FEATURE == ENABLED)


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#if (API018_SERVER_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Generate a fault.
 * @param node_id = The communication node that reported the fault.
 * @param fault_key = The unique cross-category fault key to translate to a category-specific fault ID.
 * @return TRUE if the fault was generated; otherwise FALSE.
 */
static BOOL_TYPE GenerateFault(uint8 node_id, API018_FAULT_KEY_TYPE fault_key)
{
    for (uint8 i = 0; i < ELEMENTS_IN_ARRAY(MAP_FAULT_KEYS_TO_FAULT_IDS); i++)
    {
        if ((MAP_FAULT_KEYS_TO_FAULT_IDS[i].Node == node_id) && (MAP_FAULT_KEYS_TO_FAULT_IDS[i].Key == fault_key))
        {
            FAULT_ID_TYPE fault_id = MAP_FAULT_KEYS_TO_FAULT_IDS[i].Fault_Id;
            if (fault_id != FAULT_ID_NONE)
            {
                Fault__ConditionDetected(fault_id);
                return TRUE;
            }
        }
    }
    return FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the next statistics section ID based on the specified requested sections.
 * @param requested = The statistics sections that are requested (one for each bit).
 * @return The next statistics section ID based on the specified requested sections.
 */
static API018_SECTION_TYPE GetNextSectionID(uint8 requested)
{
    for (API018_SECTION_TYPE section = API018_SECTION_FIRST; section <= API018_SECTION_LAST; section++)
    {
        if (BIT_TEST(requested, section) == 1)
        {
            return section;
        }
    }
    return API018_SECTION_NONE_REMAINING;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a pointer to the requested statistics section.
 * @param requested = The requested statistics section.
 * @return A pointer to the requested statistics section.
 */
static void* GetSectionAddress(API018_SECTION_TYPE requested)
{
    switch (requested)
    {
        case API018_SECTION_HEADER:
            return (void*)API018_HEADER_PACKET;

        case API018_SECTION_LIFETIME:
            return Statistics__GetLifetimeData();

        case API018_SECTION_CYCLES:
            #if (STATISTICS_SAVED_CYCLES > 0)
                return Statistics__GetCycleData(0);
            #else
                return NULL;
            #endif

        case API018_SECTION_FAULTS:
            return Statistics__GetFaultData(0);

        case API018_SECTION_PRE_FAULTS:
            return Statistics__GetPreFaultData(0);

        case API018_SECTION_LAST_CYCLES:
            #if (STATISTICS_SAVED_LAST_CYCLES > 0)
                return Statistics__GetLastCycleData(0);
            #else
                return NULL;
            #endif

        case API018_SECTION_FAULTS_BLACK_BOX:
            #if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
                return Statistics__GetFaultBlackBoxData(0);
            #else
                return NULL;
            #endif

        case API018_SECTION_PRE_FAULTS_BLACK_BOX:
            #if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
                return Statistics__GetPreFaultBlackBoxData(0);
            #else
                return NULL;
            #endif

        default:
            break;
    }
    return NULL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the size of the specified statistics section.
 * @param section = The specified statistics section.
 *                  All sections are assumed if value is not a valid section number.
 * @return The size of the specified statistics section.
 */
static uint16 GetSectionSize(API018_SECTION_TYPE section)
{
    uint16 size;
    if ((API018_SECTION_FIRST <= section) && (section <= API018_SECTION_LAST))
    {
        uint8 index = section - 1;
        size = API018_HEADER_PACKET[index].Number_Of_Elements * API018_HEADER_PACKET[index].Size_Of_Elements;
    }
    else
    {
        size = 0;
        for (uint8 section = API018_SECTION_FIRST; section <= API018_SECTION_LAST; section++)
        {
            uint8 index = section - 1;
            size += API018_HEADER_PACKET[index].Number_Of_Elements * API018_HEADER_PACKET[index].Size_Of_Elements;
        }
    }
    return size;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove a fault.
 * @param node_id = The communication node that is removing the fault.
 * @param fault_key = The unique cross-category fault key to translate to a category-specific fault ID.
 * @return TRUE if the fault was removed; otherwise FALSE.
 */
static BOOL_TYPE RemoveFault(uint8 node_id, API018_FAULT_KEY_TYPE fault_key)
{
    for (uint8 i = 0; i < ELEMENTS_IN_ARRAY(MAP_FAULT_KEYS_TO_FAULT_IDS); i++)
    {
        if ((MAP_FAULT_KEYS_TO_FAULT_IDS[i].Node == node_id) && (MAP_FAULT_KEYS_TO_FAULT_IDS[i].Key == fault_key))
        {
            FAULT_ID_TYPE fault_id = MAP_FAULT_KEYS_TO_FAULT_IDS[i].Fault_Id;
            if (fault_id != FAULT_ID_NONE)
            {
                Fault__ConditionRemoved(fault_id);
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif      // #if (API018_SERVER_FEATURE == ENABLED)


#endif      // #if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))

