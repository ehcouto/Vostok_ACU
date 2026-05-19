/**
 *  @file
 *
 *  @brief      General defines needed by Reveal and Reveal APIs
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef REVEAL_DEF_H_
#define REVEAL_DEF_H_

#define API001CORE_NUM          1
#define API001CORE_TYPE         3
#define API001CORE_VERSION      2
#define API001CORE_INSTANCES    1

#include "Reveal_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#ifndef REVEAL_RECIPE_SPECIFIC
    #define REVEAL_RECIPE_SPECIFIC
#endif

typedef enum
{
    // NVO - Recipe Status State Values
    NVO_RS_READY = 0,                            //The command is successfully executed and Reveal is ready to accept another command.
    NVO_RS_BUSY,                                 //The Reveal module is currently busy executing a command.
    NVO_RS_REJECTED,                             //The command send to Reveal was rejected, because another command still in progress.
    NVO_RS_ACK_EVENT,                            //Command was not executed because Reveal is currently waiting for ACK.
    NVO_RS_UNSUPPORTED,                          //The command was unsupported for some reason.
    NVO_RS_UNSUP_OP_CODE,                        //Command did not execute due to invalid op code
    NVO_RS_UNSUP_UNAVAILABLE,                    //Command did not execute because it is currently unavailable in this state.
    NVO_RS_UNSUP_INVALID_PARAM,                  //Command did not execute due to an invalid or out of bounds parameter.
    NVO_RS_UNSUP_OUT_OF_MEMORY,                  //Command did not execute because the dynamic heap is out of memory.
    NVO_RS_UNSUP_DOOR_OPEN,                      //Command did not execute because the appliance door was open.
    NVO_RS_UNSUP_BOUND_CMD_INCOMPLETE,           //Bounded CMD was not fully received before a specified timeout, so it was not executed.
    NVO_RS_UNSUP_CANNOT_PAUSE_NOW,               //Unable to pause due to state of appliance process.
    NVO_RS_TBD,
    NVO_RS_DONT_SEND_ACK,
    NVO_RS_TIMEOUT
    REVEAL_RECIPE_SPECIFIC                       //Application specific (from 200 to 255) developers may use these return values in their applications..
} REVEAL_RECIPE_STATUS_TYPE;

typedef struct
{
    uint8 id;
    uint16 type;
    uint16 version;
    uint8 instances;
} REVEAL_API_INFO_TYPE;


typedef struct
{
    uint8 api_id;
    uint8 instance_id;
    uint16 type;
    uint16 version;
} REVEAL_INSTACNE_INFO_TYPE;               //Apologize to spell mistake for "Instances" for compatibility we want to keep as it is
                                           //because Reveal 4 module is used in some projects

#ifndef REVEAL_PAYLOAD_SIZE
    #define REVEAL_PAYLOAD_SIZE 13
#endif

#ifndef REVEAL_LLC_CRC_SIZE
    #define REVEAL_LLC_CRC_SIZE 2
#endif


#if (REVEAL_PAYLOAD_SIZE < 13)
    #error: "REVEAL_PAYLOAD_SIZE must be defined as 155 or greater than 13."
#endif

typedef struct
{
    uint8 destination       :4;                 //Destination node
    uint8 node_id           :4;                 //Source node
    #if (REVEAL_PAYLOAD_SIZE == 13)                     //Wide or similar size
        uint8 sdu_length    :4;
    #else
        uint8 sdu_length    ;
        uint8 not_used      :4;
    #endif

    uint8 sap               :4;                 //SAP
    uint8 api_id              ;                 //API number
    uint8 op_code           :5;                 //Opcode
    uint8 cmd_fb            :1;                 //Command/Feedback
    uint8 frag              :1;                 //Fragmentation
    uint8 mmp               :1;                 //More Message Pending
    uint8 payload[REVEAL_PAYLOAD_INCOMING_SIZE + REVEAL_LLC_CRC_SIZE];
    uint8 size;                                 //Payload_size
}REVEAL_MSG_TYPE;

typedef REVEAL_RECIPE_STATUS_TYPE (* const REVEAL_PARSER_COMMAND_FUNCTION_TYPE)(REVEAL_MSG_TYPE * msg);
typedef void (* const REVEAL_PARSER_FEEDBACK_FUNCTION_TYPE)(REVEAL_MSG_TYPE * msg);
typedef void (* const REVEAL_PARSER_ACK_TYPE)(uint8 address, uint8 api, uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode);

#define NULL_CMD_PARSER         ((REVEAL_PARSER_COMMAND_FUNCTION_TYPE)0)
#define NULL_FDB_PARSER         ((REVEAL_PARSER_FEEDBACK_FUNCTION_TYPE)0)
#define NULL_ACK_PARSER         ((REVEAL_PARSER_ACK_TYPE)0)

#define REVEAL_ROUTER_DISABLED                  0
#define REVEAL_ROUTER_INTERNAL_EXTERNAL         1
#define REVEAL_ROUTER_INTERNAL_ONLY             2


#define REVEAL_ROUTING_EXTERNAL_DEF     0
#define REVEAL_ROUTING_INTERNAL_DEF     1


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // REVEAL_DEF_H_


