/**
 *  @file       
 *
 *  @brief      WIN protocol module standard API
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
#ifndef WIN_H_
#define WIN_H_
#include "C_Extensions.h"
#include "Compiler_defs.h"
#include "Win_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct
{
    uint8 dle;
    uint8 stx;
    union
    {
        uint8 byte;
        struct
        {
            uint8 destination   :4;
            uint8 source        :4;
        } bits;
    }address;

    union
    {
        uint8 byte;
        struct
        {
            uint8 sap           :4;
            uint8 msg_count     :4;
        } bits;
    }sap;
    uint8 sdu_length;
} WIN_HEADER_TYPE;

typedef struct
{
    uint8 dle;
    uint8 stx;

    uint8 destination   :4;
    uint8 source        :4;

    uint8 sap           :4;
    uint8 msg_count     :4;

    uint8 sdu_length;
    uint8 payload[WIN_MAX_OUTGOING_DATA_SIZE + 1]; // Data + CRC8
} WIN_OUTGOING_MESSAGE_TYPE;

typedef struct
{
    uint8 dle;
    uint8 stx;

    uint8 destination   :4;
    uint8 source        :4;

    uint8 sap           :4;
    uint8 reserved      :4;

    uint8 sdu_length;

    uint8 payload[WIN_MAX_INCOMING_DATA_SIZE + 1]; // Data + CRC8
} WIN_INCOMING_MESSAGE_TYPE;


typedef enum
{
    WIN_OPER_SUCCEED,
    WIN_OPER_BUFFER_NOT_ALLOCATED,
    WIN_OPER_INVALID_SIZE,
    WIN_OPER_INVALID_ADDRESS,
    WIN_OPER_FAIL
} WIN_OPER_TYPE;

typedef enum
{
    WIN_BUS_STATUS_BREAK_OUT,
    WIN_BUS_STATUS_RX_PIN
}WIN_BUS_STATUS_TYPE;

#define WIN_BROADCAST   0xF
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Win__Initialize(void);

void Win__Handle1ms(void);


WIN_INCOMING_MESSAGE_TYPE * Win__GetIncomingMessage(uint8 sap);
void Win__FreeIncomingMessage(uint8 sap);

WIN_OUTGOING_MESSAGE_TYPE * Win__AllocateMessage(void);
WIN_OPER_TYPE Win__QueueMessage(void);


void Win__SetAddress(uint8 address);
uint8 Win__GetAddress(void);

void Win__ClearNodeStatus(void);
uint16 Win__GetNodeStatus(void);

void Win__SendBreak(void);
void Win__StopBreak(void);
BOOL_TYPE Win__GetBusStatus(WIN_BUS_STATUS_TYPE bus_status);

#endif // WIN_H_


