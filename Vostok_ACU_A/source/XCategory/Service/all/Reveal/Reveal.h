/**
 *  @file
 *
 *  @brief      Reveal Module API
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
#ifndef REVEAL_H_
#define REVEAL_H_
#include "C_Extensions.h"
#include "Reveal_def.h"
#include "Reveal_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================



#ifndef REVEAL_QUEUE_PRIORITY
    #define REVEAL_QUEUE_PRIORITY   DISABLED
#endif

typedef struct
{
    uint8 api;
    uint8 opcode;
    uint8 node          :4;
    uint8 feedback      :1;
    uint8 timeout       :1;
    uint8 priority      :1;

    uint8 length;
    uint8 data[REVEAL_PAYLOAD_SIZE];
} REVEAL_COMMAND_TYPE;

#define REVEAL_BROADCAST_NODE    15

#define REVEAL_API_CORE         1

#define REVEAL_SAP              4

#define REVEAL_MMP_BIT          ((uint8)0x80)        //reveal more message pending

//! Function to process node reset event.
typedef void    (*REVEAL_RESET_NODE_HANDLER_TYPE)     (uint8 node_id);
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Reveal__Initialize(void);
void Reveal__Handler(void);
void Reveal__Handler1ms(void);

BOOL_TYPE Reveal__QueueMessageNoData(uint8 api, uint8 opcode, uint8 node,BOOL_TYPE is_command ,BOOL_TYPE is_priority);
BOOL_TYPE Reveal__QueueMessageU8(uint8 api, uint8 opcode, uint8 node,BOOL_TYPE is_command,BOOL_TYPE is_priority, uint8 data );
BOOL_TYPE Reveal__QueueMessageU16(uint8 api, uint8 opcode, uint8 node,BOOL_TYPE is_command,BOOL_TYPE is_priority, uint16 data );
BOOL_TYPE Reveal__QueueMessageS16(uint8 api, uint8 opcode, uint8 node,BOOL_TYPE is_command,BOOL_TYPE is_priority, sint16 data );
BOOL_TYPE Reveal__QueueMessageU32(uint8 api, uint8 opcode, uint8 node,BOOL_TYPE is_command,BOOL_TYPE is_priority, uint32 data );

REVEAL_COMMAND_TYPE* Reveal__NewFeedback(uint8 api, uint8 opcode, uint8 node, uint8 size);
REVEAL_COMMAND_TYPE * Reveal__NewCommand(uint8 api, uint8 opcode, uint8 node, uint8 size);
PASS_FAIL_TYPE Reveal__AddDataU8(REVEAL_COMMAND_TYPE* command, uint8 u8);
PASS_FAIL_TYPE Reveal__AddDataU8Buffer(REVEAL_COMMAND_TYPE* command, uint8 *u8_buffer, uint8 length);
PASS_FAIL_TYPE Reveal__AddDataU16(REVEAL_COMMAND_TYPE* command, uint16 u16);
PASS_FAIL_TYPE Reveal__AddDataS16(REVEAL_COMMAND_TYPE* command, sint16 s16);
PASS_FAIL_TYPE Reveal__AddDataU32(REVEAL_COMMAND_TYPE* command, uint32 u32);
BOOL_TYPE Reveal__QueueCommand(REVEAL_COMMAND_TYPE* command);
uint8 Reveal__GetMinimumQueueSpace(void);
uint8 Reveal__GetFreeQueueSpace(void);
uint8 Reveal__GetNumMessagesQueue(void);

REVEAL_COMMAND_TYPE * Reveal__PriorityNewFeedback(uint8 api, uint8 opcode, uint8 node , uint8 size);
REVEAL_COMMAND_TYPE * Reveal__PriorityNewCommand(uint8 api, uint8 opcode, uint8 node, uint8 size);
uint8 Reveal__GetMinimumPriorityQueueSpace(void);

uint8 Reveal__GetFreePriorityQueueSpace(void);
uint8 Reveal__GetNumMessagesPriorityQueue(void);

void Reveal__SetNodeHeartBeat(uint8 node, uint16 time);


uint16 Reveal__GetNodestatus(void);

#if (REVEAL_1S_EXTERNAL_REFFERENCE == ENABLED)
    void Reveal__1sHandler(void);
#endif

BOOL_TYPE Reveal__RequestResetEvents(REVEAL_RESET_NODE_HANDLER_TYPE node_reset_status_handler);

#endif // REVEAL_H_


