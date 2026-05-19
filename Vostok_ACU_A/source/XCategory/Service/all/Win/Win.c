/**
 *  @file       
 *
 *  @brief      WIN Protocol Module implementation
 *
 *  @details    This module implements the code to handle the WIN communication protocol using Uart driver from Vortex.
 *              This module is designed to operate with all micro controllers supported by Vortex.
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
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Win.h" 
#include "Win_prv.h"
#include "Crc8.h"
#include "Uart.h"
#include "String.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef WIN_DEBUG
    #define WIN_DEBUG DISABLED
#endif

#ifndef WIN_NODE_STATUS
    #define WIN_NODE_STATUS  DISABLED
#endif

#ifndef WIN_DATA_SEGMENT1
    #define WIN_DATA_SEGMENT1
#endif

#ifndef WIN_DATA_SEGMENT2
    #define WIN_DATA_SEGMENT2
#endif

#ifndef WIN_DATA_SEGMENT3
    #define WIN_DATA_SEGMENT3
#endif

#ifndef WIN_NODE
    #error: "WIN_NODE must be defined in Win_prm.h as 0 to 13, 14 is reserved for the PC and 15 is broadcast."
#else
    #if (WIN_NODE > 13)
    #error: "WIN_NODE must be defined in Win_prm.h as 0 to 13, 14 is reserved for the PC and 15 is broadcast."
    #endif
#endif

#ifndef WIN_COM
    #error: "WIN_COM must be defined in Win_prm.h as COM0, COM1 ..4"
#endif


#if (WIN_MAX_OUTGOING_DATA_SIZE >  WIN_MAX_INCOMING_DATA_SIZE)
    #error: "WIN_MAX_INCOMING_DATA_SIZE must be define equal or greater than WIN_MAX_OUTGOING_DATA_SIZE"
#endif

#if ((WIN_MAX_OUTGOING_DATA_SIZE > 255) || (WIN_MAX_OUTGOING_DATA_SIZE < 15))
    #error: "WIN_MAX_OUTGOING_DATA_SIZE has to be defined from 15 to 255"
#endif

#if ((WIN_MAX_INCOMING_DATA_SIZE > 255) || (WIN_MAX_INCOMING_DATA_SIZE < 15))
    #error: "WIN_MAX_INCOMING_DATA_SIZE has to be defined from 15 to 255"
#endif

#ifndef WIN_DEFAULT_BREAK_SIGNAL_TIME_IN_MILLI_SECONDS
    #define WIN_DEFAULT_BREAK_SIGNAL_TIME_IN_MILLI_SECONDS       (1000)
#elif (WIN_DEFAULT_BREAK_SIGNAL_TIME_IN_MILLI_SECONDS < 350)
    #error : "The minimum break signal shall be more then 350ms"
#endif

#if  (WIN_DYNAMIC_ADDRESS == DISABLED)
    #define Win_Node    WIN_NODE
#else
    static uint8 Win_Node;
#endif

/**
 * Defines the interval time in between 2 nodes in ms.
 */
#define WIN_DELTA_TIME      2

/**
 * Defines the base time for the time for each node in ms.
 */
#define WIN_OFFSET_TIME     4

/**
 * Defines the time for which no data in the bus will consider the bus in idle (no node sending any data)
 */
#define WIN_IDLE_TIME       3

/**
 * The math for the timer for each node is basically   (node address * WIN_DELTA_TIME) + WIN_OFFSET_TIME.\n
 * For instance: Node 2 = (2 * 2) + 4 = 8ms
 */
#define WIN_NODE_TIME           ((Win_Node * WIN_DELTA_TIME) + WIN_OFFSET_TIME) //((Win_Node * 2)+4)

/**
 * When a node fail and requires uart reconfiguration the node will stay away from the bus for (15 *2) + 4 = 34ms
 */
#define WIN_NODE_MAX_TIME   ((15 * WIN_DELTA_TIME) + WIN_OFFSET_TIME) //((Win_Node * 2)+4)

#ifndef WIN_MAX_RX_PARSER_TIMEOUT
    #define WIN_MAX_RX_PARSER_TIMEOUT   25        // 25ms time to alow overwrite in the RX buffer if the application doesn't get it.
#endif

/**
 * After reset the node will stay out of the bus for 150ms
 */
#ifndef WIN_POWERUP_TIME
    #define WIN_POWERUP_TIME    (150)
#endif

#if (WIN_POWERUP_TIME > 220)
    #error: "WIN_POWERUP_TIME must be defined from 0 to 220ms since the time used will be 220 + Node Bus Time limited to a uint8";
#endif

/**
 * This configuration limits the amount of data managed in each interaction.
 */
#ifndef WIN_MAX_BYTES_HANDLED_AT_ONCE
    #define WIN_MAX_BYTES_HANDLED_AT_ONCE   (5)
#endif

WIN_DATA_SEGMENT1 static uint8 Win_Timer;
static uint8 * Win_Tx_Pointer;
static uint8 * Win_Check_Tx_Pointer;
WIN_DATA_SEGMENT3 static uint16 Win_Check_Index;
WIN_DATA_SEGMENT1 static uint16 Win_Remaining_Data;

static WIN_OUTGOING_MESSAGE_TYPE Win_Tx_Buffer;
static WIN_INCOMING_MESSAGE_TYPE Win_Rx_Buffer;
WIN_DATA_SEGMENT3 static uint16  Win_Rx_Index;                // We did use short instead of char because of 255 + crc could potentially get to 256
WIN_DATA_SEGMENT3 static uint8 Win_Rx_Crc;
WIN_DATA_SEGMENT1 static uint8 Win_Bus_Idle;
WIN_DATA_SEGMENT1 static uint8 Win_Bus_Idle_Timer;

#define WIN_NUM_RETRIES     3
static uint8 Win_Retry;

#ifndef WIN_NUM_RETRIES_COLLISION
    #define WIN_NUM_RETRIES_COLLISION     10
#endif

#if ((WIN_NUM_RETRIES_COLLISION < 10) || (WIN_NUM_RETRIES_COLLISION > 255))
    #error: "The number of Retries due to collision must be from 10 to 255"
#endif
static uint8 Win_Retry_Collision;

#if (WIN_DEBUG == ENABLED)
    static uint16 Win_Error_Ack_Retries_Counter;
    static uint16 Win_Error_Collision_Counter;
    static uint16 Win_Error_Collision_Retries_Counter;
    static uint16 Win_Error_Crc_Counter;
    static uint16 Win_Error_Dropped_Counter;
    static uint16 Win_Error_Echo_Counter;
    static uint16 Win_Error_Grant_Bus_Counter;
    static uint16 Win_Error_Uart_Incoming_Counter;
    static uint16 Win_Error_Uart_Outgoing_Counter;
#endif

typedef enum
{
    WIN_TX_STATE_IDLE,
    WIN_TX_STATE_GRANT_BUS,
    WIN_TX_STATE_SENDING
}WIN_TX_STATE_TYPE;
WIN_DATA_SEGMENT2 static WIN_TX_STATE_TYPE Win_Tx_State;

typedef enum
{
    WIN_RX_STATE_IDLE,
    WIN_RX_STATE_STX,
    WIN_RX_STATE_ADDRESS,
    WIN_RX_STATE_SAP,
    WIN_RX_STATE_SIZE,
    WIN_RX_STATE_DATA,
    WIN_RX_STATE_CRC,
    WIN_RX_STATE_WAIT_FOR_IDLE
}WIN_RX_STATE_TYPE;
WIN_DATA_SEGMENT2 static WIN_RX_STATE_TYPE Win_Rx_State;
static WIN_HEADER_TYPE Win_Header;

typedef enum
{
    WIN_MESSAGE_FREE,
    WIN_MESSAGE_PROCESSING,
    WIN_MESSAGE_READY,
    WIN_MESSAGE_SENDING
} WIN_MESSAGE_STATE_TYPE;
WIN_DATA_SEGMENT2 WIN_MESSAGE_STATE_TYPE Win_Rx_Message_State;
WIN_DATA_SEGMENT2 WIN_MESSAGE_STATE_TYPE Win_Tx_Message_State;

#define WIN_DLE         0x10                        // DLE
#define WIN_STX         0x02                        // Start of text which means being of the package
#define WIN_ACK         0x06                        // ACK

typedef enum
{
    WIN_TX_STATUS_FREE,
    WIN_TX_STATUS_SENDING,
    WIN_TX_STATUS_COLLISION,
    WIN_TX_STATUS_SENT
} WIN_TX_STATUS_TYPE;
static WIN_TX_STATUS_TYPE Win_Tx_Status;

#define WIN_MSG_COUNTER_LIMIT   15
#define WIN_MSG_COUNTER_START   1
#define WIN_MSG_COUNTER_RESET   0
static uint8 Msg_Count;

#ifndef WIN_TIME_HOLDING_MSG
    #define WIN_TIME_HOLDING_MSG    100
#endif

#if ((WIN_TIME_HOLDING_MSG < 1) || (WIN_TIME_HOLDING_MSG > 255))
    #error "WIN_TIME_HOLDING_MSG must be defined from 1 to 255"
#endif

#ifndef WIN_MAX_NUM_MESSAGES
    #define WIN_MAX_NUM_MESSAGES    20
#endif
#if ((WIN_MAX_NUM_MESSAGES < 1) || (WIN_MAX_NUM_MESSAGES > 255))
    #error "WIN_MAX_NUM_MESSAGES must be defined from 1 to 255"
#endif
static uint8 Node_Sent_Counter;             // counter used to engage the node bus contention.
static uint8 Node_Not_Sending_Timer;        // Max timer which the node will stay with the bus access contention feature enabled.
static uint8 Prev_Rx_Msg_Count[15];

volatile uint8 Uart_Data;
volatile uint8 Uart_Error;

#define WIN_PC_NODE      14

static uint8 Win_Rx_Message_Time;
#if (WIN_NODE_STATUS == ENABLED)
    static uint16 Win_Node_Status;
#endif

static uint16 Win_Break_Counter;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void WinOutgoingProcess(void);
static void WinIncomingProcess(void);
static void SendData(void);
static void DropSendingMessage(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Win and its variables
 *
 */
void Win__Initialize(void)
{
#if  (WIN_DYNAMIC_ADDRESS == ENABLED)
    Win_Node = WIN_NODE;
#endif
    Win_Tx_Status = WIN_TX_STATUS_FREE;
    Win_Timer = WIN_NODE_TIME + WIN_POWERUP_TIME;   // initial timer for which the node will be off the bus.
    Win_Tx_State = WIN_TX_STATE_IDLE;
    Win_Rx_State = WIN_RX_STATE_IDLE;
    Win_Rx_Message_State = WIN_MESSAGE_FREE;
    Win_Tx_Message_State = WIN_MESSAGE_FREE;
    Msg_Count = WIN_MSG_COUNTER_RESET;
    Win_Tx_Buffer.dle = WIN_DLE;
    Win_Tx_Buffer.stx = WIN_STX;
    Win_Tx_Buffer.source = Win_Node;
    Node_Sent_Counter = 0;
    Win_Bus_Idle_Timer = 0;
    Win_Bus_Idle = 0;
    Node_Not_Sending_Timer = 0;
    memset(&Prev_Rx_Msg_Count, 0, sizeof(Prev_Rx_Msg_Count));
    Uart__Initialize(WIN_COM,WIN_SPEED,UART_STOP_BIT_1,UART_PARITY_NONE);
    Uart_Data =0;
    Uart_Error = 0;
    Win_Retry = 0;                                  // Retry for no ack
    Win_Retry_Collision = 0;                        // retry for collision.
    Win_Check_Index = 0;
#if (WIN_DEBUG == ENABLED)
    Win_Error_Ack_Retries_Counter = 0;
    Win_Error_Collision_Counter = 0;
    Win_Error_Collision_Retries_Counter = 0;
    Win_Error_Crc_Counter = 0;
    Win_Error_Dropped_Counter = 0;
    Win_Error_Echo_Counter = 0;
    Win_Error_Grant_Bus_Counter = 0;
    Win_Error_Uart_Incoming_Counter = 0;
    Win_Error_Uart_Outgoing_Counter = 0;
#endif
    Win_Rx_Message_Time = 0;
    Win_Break_Counter = 0;
#if (WIN_NODE_STATUS == ENABLED)
    Win_Node_Status = (uint16)((uint16)1 << Win_Node);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to set the address of this node.
 * @details This method only takes effect if the feature WIN_DYNAMIC_ADDRESS is enabled in Win_prm.h
 * @param address from 0 to 13.
 */
void Win__SetAddress(uint8 address)
{
    #if (WIN_DYNAMIC_ADDRESS == ENABLED)
    if (address < WIN_PC_NODE)
    {
        Win_Node = address;
    }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes the current address of this node
 * @return address from 0 to 13
 */
uint8 Win__GetAddress(void)
{
    return (Win_Node);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This Handler Win Communication
 * @details Shall be called periodically from 1 milli second task/thread
 *
 */
void Win__Handle1ms(void)
{
    if (Win_Break_Counter > 0)
    {
        Win_Break_Counter--;
        Win_Tx_State = WIN_TX_STATE_IDLE;
    }
    else
    {
        WinOutgoingProcess();
    }

    WinIncomingProcess();
    Uart__Handler();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks for a pending incoming message that should be parsed by the application protocol
 * @param sap define the application protocol used in this message
 * @return WIN_INCOMING_MESSAGE_TYPE * that points to the buffer or NULL if there's no message pening
 */
WIN_INCOMING_MESSAGE_TYPE * Win__GetIncomingMessage(uint8 sap)
{
    WIN_INCOMING_MESSAGE_TYPE * pt;
    pt = (WIN_INCOMING_MESSAGE_TYPE *)NULL;
    if ( Win_Rx_Message_State == WIN_MESSAGE_READY) // if a message is received and ready for parsing
    {
        if (Win_Rx_Buffer.sap == sap)               // if the sap matches who is requesting
        {
            pt = &Win_Rx_Buffer;                    // get the message
            Win_Rx_Message_State = WIN_MESSAGE_PROCESSING;  // change the message to processing state
        }
    }
    return (pt);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method frees the pending message after the application protocol had parsed it
 * @param sap define the application protocol used in this message
 */
void Win__FreeIncomingMessage(uint8 sap)
{
    if ( Win_Rx_Message_State == WIN_MESSAGE_PROCESSING)    // if the message is being processed
    {
        if (Win_Rx_Buffer.sap == sap)                       // and the sap matches
        {
            Win_Rx_Message_State = WIN_MESSAGE_FREE;        // clear the message as parsed.
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used by the upper layer to allocated the buffer to send a message out.
 * @return the pointer to the buffer
 *  @retval NULL if the buffer can not be allocated
 *  @retval WIN_OUTGOING_MESSAGE_TYPE * pointing to the buffer if the method managed to allocated the buffer.
 */
WIN_OUTGOING_MESSAGE_TYPE * Win__AllocateMessage(void)
{
    WIN_OUTGOING_MESSAGE_TYPE * pt;
    pt = (WIN_OUTGOING_MESSAGE_TYPE *)NULL;
    if (Win_Tx_Message_State == WIN_MESSAGE_FREE)           // if the buffer is free
    {
        Win_Tx_Message_State = WIN_MESSAGE_PROCESSING;      // allocate the buffer
        pt = &Win_Tx_Buffer;                                // return it.
    }
    return (pt);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method queues a message to be sent.
 * @details The message has to be build by Win__AllocateMessage, them the application will fill up the data and call this procedure.
 * @return WIN_OPER_TYPE
 *  @retval WIN_OPER_SUCCEED if the message is successfully composed and queued
 *  @retval WIN_OPER_INVALID_ADDRESS if the destination is set to the sender address
 *  @retval WIN_OPER_INVALID_SIZE is the size is bigger than the buffer or size = 0
 *  @retval WIN_OPER_BUFFER_NOT_ALLOCATED is the buffer wan't allocated and the application is trying to queue the message.
 */
WIN_OPER_TYPE Win__QueueMessage(void)
{
    WIN_OPER_TYPE retval;
    uint8 *pt;

    if (WIN_MESSAGE_PROCESSING == Win_Tx_Message_State) // the message state must be PROCESSING to be able to be queued
    {
       if ((Win_Tx_Buffer.sdu_length <= WIN_MAX_OUTGOING_DATA_SIZE) &&  // address must in the range to be able to be queued
           (Win_Tx_Buffer.sdu_length != 0))
       {
           if (Win_Tx_Buffer.destination != Win_Node)   // the message can no be to this node itself
           {
               // ensure the package formating
               Win_Tx_Buffer.dle = WIN_DLE;
               Win_Tx_Buffer.stx = WIN_STX;
               Win_Tx_Buffer.source = Win_Node;
               Win_Tx_Buffer.msg_count = Msg_Count;

               pt = (uint8 *)&Win_Tx_Buffer;    //lint !e928  Lint: 928 cast from pointer to pointer [MISRA 2004 Rule 11.4]
               pt++;                                // Skip DLE
               pt++;                                // Skip STX
               Win_Tx_Buffer.payload[Win_Tx_Buffer.sdu_length] = Crc8(0xFF,pt,(uint8)(Win_Tx_Buffer.sdu_length + 3));

               // counts from 1 to 15, 0 is used when the system powers up
               Msg_Count++;

               if (Msg_Count > WIN_MSG_COUNTER_LIMIT)
               {
                   Msg_Count = WIN_MSG_COUNTER_START;
               }
               Win_Retry = WIN_NUM_RETRIES;
               Win_Retry_Collision = WIN_NUM_RETRIES_COLLISION;
               Win_Tx_Message_State = WIN_MESSAGE_READY;
               retval = WIN_OPER_SUCCEED;
           }
           else
           {
               retval = WIN_OPER_INVALID_ADDRESS;
               Win_Tx_Message_State = WIN_MESSAGE_FREE;
           }
       }
       else
       {
           retval = WIN_OPER_INVALID_SIZE;
           Win_Tx_Message_State = WIN_MESSAGE_FREE;
       }
    }
    else
    {
        retval = WIN_OPER_BUFFER_NOT_ALLOCATED;
    }
    return (retval);
}

/**
 * @brief This method initializes the Node status variable.
 */
void Win__ClearNodeStatus(void)
{
    #if (WIN_NODE_STATUS == ENABLED)
        Win_Node_Status = (uint16)((uint16)1 << Win_Node);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes a 16bits variable where each bit represents the state of a node in the bus
 * @return 16bits node bus status
 */
uint16 Win__GetNodeStatus(void)
{
    #if (WIN_NODE_STATUS == ENABLED)
        return(Win_Node_Status);
    #else
        return (0);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method starts sending the Break signal on the COM port
 * @details Sends a break signal till WIN_DEFAULT_BREAK_SIGNAL_TIME_IN_MILLI_SECONDS time expires.
 *          If the application needs to send for more then this time then they need to call the API before the timer expires
 *
 */
void Win__SendBreak(void)
{
    Win_Break_Counter = WIN_DEFAULT_BREAK_SIGNAL_TIME_IN_MILLI_SECONDS;
    Uart__SendBreak(WIN_COM);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method stops sending the Break signal on the COM port
 */
void Win__StopBreak(void)
{
    Uart__StopBreak(WIN_COM);
    Win_Break_Counter = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method used to read the COM bus status like receive Pin Status or Break signal status
 *
 * @param bus_status - WIN_BUS_STATUS_TYPE
 *
 * @return BOOL_TYPE
 *  @retval TRUE(1)
 *  @retval FALSE(0)
 */
BOOL_TYPE Win__GetBusStatus(WIN_BUS_STATUS_TYPE bus_status)
{
    BOOL_TYPE response;

    response = FALSE;

    if (bus_status == WIN_BUS_STATUS_BREAK_OUT)
    {
        response = (BOOL_TYPE)Uart__GetStatus(WIN_COM, UART_STATUS_BREAK_OUT);
    }
    else if (bus_status == WIN_BUS_STATUS_RX_PIN)
    {
        response = (BOOL_TYPE)Uart__GetStatus(WIN_COM, UART_STATUS_RX_PIN);
    }

    return response;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Handler that manages the process to send messages
 * @details This procedure must be executed every 1m (or from on TimerInterrupt Slot)
 */
static void WinOutgoingProcess(void)
{
    uint8 bytes_to_read;
    bytes_to_read = Uart__GetStatus(WIN_COM,UART_STATUS_RX_AVAILABLE);

    if (bytes_to_read > 0)                          // if there is any byte in the Uart driver to be parsed
    {
        Win_Bus_Idle_Timer = WIN_NODE_MAX_TIME;     // This timer times 34ms which is used to detect situations of the bus is really in idle and no other nodes want to send data.
        Win_Bus_Idle = WIN_IDLE_TIME;               // This timer is used to make sure the ack is detected within the 3ms window after the message has being sent.
        if (Win_Timer < WIN_NODE_TIME)              // make sure the reload doesn't overwrite a previews bigger value (power up for instance).
        {
            Win_Timer = WIN_NODE_TIME;              // reload node timer which depends on the node address.
        }
    }

    if (Win_Bus_Idle_Timer > 0)
    {
        Win_Bus_Idle_Timer--;
        if (Win_Bus_Idle_Timer == 0)                // if the bus is in idle for 34 ms
        {
            Node_Not_Sending_Timer = 0;             // if the bus idle is detected it will force the bus access contention to disengage.
            Node_Sent_Counter = 0;
        }
    }

    if (Node_Not_Sending_Timer > 0)                 // This is the maximum time for which this node can be prevented to send messages in the bus due to the bus access contention feature.
    {
        Node_Not_Sending_Timer--;
        if (Node_Not_Sending_Timer == 0)
        {
            Node_Sent_Counter = 0;                  // when the timer expires the bus access contention feature is disabled by clear the message counter.
        }
    }

    switch (Win_Tx_State)
    {
        case WIN_TX_STATE_IDLE:
            if (Win_Timer == 0)                     // if the node is allowed to access the bus
            {
                if ((Win_Tx_Message_State == WIN_MESSAGE_READY) &&  // if there is a message to be sent
                    (Node_Sent_Counter < WIN_MAX_NUM_MESSAGES))     // and the bus access contention is not active.
                {
                    Uart__WriteByte(WIN_COM,(uint8)(~Win_Node));    // send the bus grant byte
                    Win_Tx_State = WIN_TX_STATE_GRANT_BUS;
                }
            }
            break;
        case WIN_TX_STATE_GRANT_BUS:
            Win_Tx_State = WIN_TX_STATE_IDLE;       // by default it consider collision
            if (bytes_to_read == 0)                 // if there is no data in the Uart buffer (received)
            {
                if (Win_Timer < WIN_NODE_MAX_TIME)
                {
                    Win_Timer = WIN_NODE_MAX_TIME;  // reload node timer to prevent the module to take over the bus
                }
                Uart__Initialize(WIN_COM,WIN_SPEED,UART_STOP_BIT_1,UART_PARITY_NONE);
                #if (WIN_DEBUG == ENABLED)
                    Win_Error_Grant_Bus_Counter++;
                #endif
            }
            else                                    // if there is some data in the Uart buffer
            {
                if (bytes_to_read == 1)             // if the amount of data is exactly 1 byte, the echo of the bus grant byte.
                {
                    Uart_Data = Uart__ReadByte(WIN_COM);        // receive the data
                    Uart_Error = Uart__GetStatus(WIN_COM,UART_STATUS_ERROR);
                    if ((Uart_Error == FALSE) &&
                        (Uart_Data == (uint8)(~Win_Node)))       // bus granted
                    {
                        Win_Tx_Pointer = (uint8 *)&Win_Tx_Buffer;   //lint !e928 Lint: 928 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        Win_Check_Tx_Pointer = Win_Tx_Pointer;
                        Win_Check_Index = 0;
                        // data size + 2 bytes of header, address, sap, size, crc
                        Win_Remaining_Data = ((uint16)Win_Tx_Buffer.sdu_length) + 6;
                        Win_Tx_Status = WIN_TX_STATUS_SENDING;
                        SendData();                 // start to send the message
                        Win_Tx_State = WIN_TX_STATE_SENDING;
                    }
                    #if (WIN_DEBUG == ENABLED)
                        else if(Uart_Error != FALSE)
                        {
                            Win_Error_Uart_Outgoing_Counter++;
                        }
                    #endif
                }
            }
            break;
        case WIN_TX_STATE_SENDING:
            if (Win_Tx_Status == WIN_TX_STATUS_COLLISION)   // check if there is collision on the echo reception
            {
                Win_Tx_State = WIN_TX_STATE_IDLE;
                if (Win_Retry_Collision > 0)        // this is the retry procedure due to collision.
                {
                    Win_Retry_Collision--;
                    Win_Tx_Message_State = WIN_MESSAGE_READY;
                    WIN_RETRY_OUTGOING_MESSAGE(&Win_Tx_Buffer);
                    #if (WIN_DEBUG == ENABLED)
                        Win_Error_Collision_Retries_Counter++;
                    #endif
                }
                else
                {                                   // when the number of retries expires the message is dropped.
                    WIN_DROPPED_OUTGOING_MESSAGE(&Win_Tx_Buffer);
                    DropSendingMessage();
                    #if (WIN_DEBUG == ENABLED)
                        Win_Error_Collision_Counter++;
                    #endif
                }
            }
            else
            {
                SendData();                         // keep sending message while there are remaining data to be sent.
                if (Win_Bus_Idle == 0)              // if the bus is in idle it means idle for 3ms after sent a message
                {
                    if (Win_Tx_Status == WIN_TX_STATUS_SENT)    // if the message want't acknowledged
                    {
                        if ((Win_Tx_Buffer.destination == WIN_BROADCAST)||  // broadcast messages or messages targeted to the pc node doesn't require ACK.
                            (Win_Tx_Buffer.destination == WIN_PC_NODE))
                        {
                            // Broadcast of PC don't not send ack
                            DropSendingMessage();
                        }
                        // ack  expected and not received.
                        else if (Win_Retry > 0)      // this is the retry procedure for messages not ack from the target
                        {
                            Win_Retry--;
                            Win_Tx_Message_State = WIN_MESSAGE_READY;
                            WIN_RETRY_OUTGOING_MESSAGE(&Win_Tx_Buffer);
                            #if (WIN_DEBUG == ENABLED)
                                Win_Error_Ack_Retries_Counter++;
                            #endif
                        }
                        else
                        {
                            #if (WIN_NODE_STATUS == ENABLED)
                                    BIT_CLR(Win_Node_Status,Win_Tx_Buffer.destination);
                            #endif

                            // when the number of retries expires the message is dropped.
                            WIN_DROPPED_OUTGOING_MESSAGE(&Win_Tx_Buffer);
                            DropSendingMessage();
                            #if (WIN_DEBUG == ENABLED)
                                Win_Error_Dropped_Counter++;
                            #endif
                        }
                    }
                    else
                    {
                        Win_Tx_Message_State = WIN_MESSAGE_READY;
                    }
                    Win_Tx_State = WIN_TX_STATE_IDLE;
                }
            }
            break;
        default:
            Win_Tx_State = WIN_TX_STATE_IDLE;
            break;
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Handler that manages the process to Receive messages
 * @details This procedure must be executed every 1m (or from on TimerInterrupt Slot)
 */
static void WinIncomingProcess(void)
{
    uint8 bytes_to_read;
    uint16 aux_short;

    bytes_to_read = Uart__GetStatus(WIN_COM,UART_STATUS_RX_AVAILABLE);

    if (bytes_to_read > 0)                          // if there are bytes in the Uart driver meaning bus activity
    {
        Win_Bus_Idle_Timer = WIN_NODE_MAX_TIME;     // This timer times 34ms which is used to detect situations of the bus is really in idle and no other nodes want to send data.
        Win_Bus_Idle = WIN_IDLE_TIME;               // This timer is used to make sure the ack is detected within the 3ms window after the message has being sent.
        if (Win_Timer < WIN_NODE_TIME)              // make sure the reload doesn't overwrite a previews bigger value (power up for instance).
        {
            Win_Timer = WIN_NODE_TIME;              // reload node timer which depends on the node address.
        }
    }

    if (Win_Rx_Message_Time > 0)                    // timer to detect stuck message ifor parser
    {
        Win_Rx_Message_Time--;
        if (Win_Rx_Message_Time == 0)
        {
            if ( Win_Rx_Message_State != WIN_MESSAGE_FREE)  // if the timer expire and still not Free
            {
                Win_Rx_Message_State = WIN_MESSAGE_FREE;    // free the message
            }
        }
    }

    if (Win_Timer > 0)                              // decrement the node bus timer
    {
        Win_Timer--;
    }

    if (Win_Bus_Idle > 0)                           // decrement the bus idle for more than 3ms timer.
    {
        Win_Bus_Idle--;
    }
    else
    {
        if (Win_Rx_State != WIN_RX_STATE_IDLE)      // if the RX state machine got stuck in the middle of a process and the bus is idle for more than 3ms it will be re-initialized.
        {
            Win_Rx_State = WIN_RX_STATE_IDLE;
        }
    }

    if (Win_Tx_State != WIN_TX_STATE_GRANT_BUS)
    {
        // reception
        while (bytes_to_read > 0)
        {
            bytes_to_read--;
            Uart_Data = Uart__ReadByte(WIN_COM);        // receive the data, byte by byte
            Uart_Error = Uart__GetStatus(WIN_COM,UART_STATUS_ERROR);

            if (Win_Tx_Status == WIN_TX_STATUS_SENDING)
            {
                // Collision evaluation, checking the echo of the trasmitted data
                Win_Rx_State = WIN_RX_STATE_IDLE;
                if ((Uart_Data != Win_Check_Tx_Pointer[Win_Check_Index]) || // if data doesn't match or error reported by the Uart
                    (Uart_Error == TRUE))
                {
                    #if (WIN_DEBUG == ENABLED)
                        if (Win_Check_Index > 4)
                        {
                            Win_Error_Echo_Counter++;
                        }
                    #endif

                    Win_Tx_Status = WIN_TX_STATUS_COLLISION; // set the collision state
                    Uart__ClearTx(WIN_COM);         // stop the transmission
                }
                else
                {
                    Win_Check_Index++;
                    aux_short = (uint16)Win_Tx_Buffer.sdu_length;
                    aux_short += 6;
                    if (Win_Check_Index >= aux_short)
                    {
                        Win_Tx_Status = WIN_TX_STATUS_SENT; // transmission end with no issues.
                    }
                }
            }
            else
            {
                // reception of messages comming from other nodes.
                if (Uart_Error == FALSE)
                {
                    // Reception state
                    switch (Win_Rx_State)
                    {
                        case WIN_RX_STATE_IDLE:
                            Win_Header.dle = Uart_Data;
                            if (Win_Header.dle == WIN_DLE)      // waiting for the dle
                            {
                                Win_Rx_State = WIN_RX_STATE_STX;
                            }
                            break;
                        case WIN_RX_STATE_STX:
                            Win_Rx_State = WIN_RX_STATE_IDLE;   // by default it goes back to wait for the DLE
                            Win_Header.stx = Uart_Data;
                            if (Win_Header.stx == WIN_STX)     // if the received data is the start of text them move to the next state
                            {
                                Win_Rx_State = WIN_RX_STATE_ADDRESS;
                                Win_Rx_Crc = 0xFF;
                            }
                            else if (Win_Header.stx == WIN_ACK)   // if the received data is the ack
                            {
                                // ack message
                                if (Win_Tx_State == WIN_TX_STATE_SENDING)   // and we are waiting for a ack
                                {
                                    #if (WIN_NODE_STATUS == ENABLED)
                                        BIT_SET(Win_Node_Status,Win_Tx_Buffer.destination);
                                    #endif

                                    DropSendingMessage();       // drop the sending message
                                    Win_Tx_State = WIN_TX_STATE_IDLE;   // go back to wait for the DLE
                                }
                            }
                            break;
                        case WIN_RX_STATE_ADDRESS:
                            Win_Header.address.byte = Uart_Data;
                            Win_Rx_Crc = Crc8__ProcessByte(Win_Rx_Crc,Uart_Data);

                            #if (WIN_NODE_STATUS == ENABLED)
                                BIT_SET(Win_Node_Status,Win_Header.address.bits.source);
                            #endif

                            if ((Win_Header.address.bits.destination == Win_Node) ||    // checking for direct message or broadcast
                                (Win_Header.address.bits.destination == WIN_BROADCAST))
                            {
                                // copy the data from the temporary buffer to the main reception buffer
                                Win_Rx_State = WIN_RX_STATE_SAP;
                            }
                            else
                            {
                                // If the message is not for this node or a broadcast.
                                Win_Rx_State = WIN_RX_STATE_WAIT_FOR_IDLE;
                                // to ensure the next message from this source target to this node won't be ignored.
                                // Prevent also buffer overflow due to a wrong source address
                                if (Win_Header.address.bits.source < ELEMENTS_IN_ARRAY(Prev_Rx_Msg_Count))
                                {
                                    Prev_Rx_Msg_Count[Win_Header.address.bits.source] = 0;
                                }
                            }
                            break;
                        case WIN_RX_STATE_SAP:
                            Win_Header.sap.byte = Uart_Data;
                            Win_Rx_Crc = Crc8__ProcessByte(Win_Rx_Crc,Uart_Data);
                            Win_Rx_State = WIN_RX_STATE_SIZE;
                            break;
                        case WIN_RX_STATE_SIZE:
                            Win_Header.sdu_length = Uart_Data;
                            Win_Rx_Crc = Crc8__ProcessByte(Win_Rx_Crc,Uart_Data);
                            if ((Win_Header.sdu_length <= WIN_MAX_INCOMING_DATA_SIZE) && // Check if the message is not bigger than the maximum size allowed.
                                (Win_Rx_Message_State == WIN_MESSAGE_FREE))             // also check if the previews received message was parsed.
                            {
                                memcpy(&Win_Rx_Buffer,&Win_Header,sizeof(Win_Header));   // transfer the header from the initial reception buffer to the real reeption buffer.
                                Win_Rx_State = WIN_RX_STATE_DATA;
                                Win_Rx_Index = 0;
                            }
                            else
                            {
                                WIN_DROP_INCOMING_MESSAGE(Win_Tx_Buffer.header);        // if the message is too big or the previews message is not parser yet it will dropped.
                                Win_Rx_State = WIN_RX_STATE_WAIT_FOR_IDLE;
                            }
                            break;
                        case WIN_RX_STATE_DATA:
                            Win_Rx_Buffer.payload[Win_Rx_Index] = Uart_Data;            // receiving the payload
                            Win_Rx_Crc = Crc8__ProcessByte(Win_Rx_Crc,Uart_Data);
                            Win_Rx_Index++;
                            if (Win_Rx_Index >= Win_Rx_Buffer.sdu_length)
                            {
                                Win_Rx_State = WIN_RX_STATE_CRC;
                            }
                            break;
                        case WIN_RX_STATE_CRC:
                            Win_Rx_Buffer.payload[Win_Rx_Index] = Uart_Data;
                            if ((Uart_Data == Win_Rx_Crc) &&                                              // Check the CRC of the received message
                                (Win_Header.address.bits.source < ELEMENTS_IN_ARRAY(Prev_Rx_Msg_Count)))  // Prevent buffer overflow due to wrong source address
                            {
                                // Check the message counter, if it is a repeated message it won't be parsed.
                                if((Win_Header.sap.bits.msg_count != Prev_Rx_Msg_Count[Win_Header.address.bits.source]) ||
                                    (Win_Header.sap.bits.msg_count == 0))
                                {
                                    // set the message to be parsed.
                                    if (Win_Rx_Buffer.sdu_length > 0)
                                    {
                                        Win_Rx_Message_Time = WIN_MAX_RX_PARSER_TIMEOUT;    // load the max time for the application to get the message
                                        Win_Rx_Message_State = WIN_MESSAGE_READY;
                                    }
                                }
                                Prev_Rx_Msg_Count[Win_Header.address.bits.source] = Win_Header.sap.bits.msg_count;


                                // if this is a direct message targeted to this node replay with ACK.
                                if (Win_Header.address.bits.destination == Win_Node)
                                {
                                    // send ACK back
                                    Uart__WriteByte(WIN_COM,WIN_DLE);
                                    Uart__WriteByte(WIN_COM,WIN_ACK);
                                }
                            }
                            else
                            {
                                // if the crc doesn't match
                                WIN_BAD_CRC_INCOMING_MESSAGE(Win_Rx_Buffer.header,Uart_Data,Win_Rx_Crc);
                                #if (WIN_DEBUG == ENABLED)
                                    Win_Error_Crc_Counter++;
                                #endif
                            }
                            Win_Rx_State = WIN_RX_STATE_IDLE;
                            break;
                        case WIN_RX_STATE_WAIT_FOR_IDLE:
                            // waiting for the end of the transmission
                            break;
                        default:
                            Win_Rx_State = WIN_RX_STATE_IDLE;
                            break;
                    }
                }
                else
                {
                    #if (WIN_DEBUG == ENABLED)
                        Win_Error_Uart_Incoming_Counter++;
                    #endif
                    Win_Rx_State = WIN_RX_STATE_IDLE;
                }
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method sends data from the transmission buffer controlling the flow depending on the availability on
 * the Uart driver
 */
static void SendData(void)
{
    uint8 space_available;
    if (Win_Remaining_Data > 0)                     // if only perform the action if still bytes to be sent.
    {
        Node_Not_Sending_Timer = WIN_TIME_HOLDING_MSG;

        // make sure still having data to send
        space_available = Uart__GetStatus(WIN_COM,UART_STATUS_TX_AVAILABLE);
      
        // The following code limits the amount of bytes we send to the UART driver at once.
        if(space_available > WIN_MAX_BYTES_HANDLED_AT_ONCE)
        {
            space_available = WIN_MAX_BYTES_HANDLED_AT_ONCE;
        }
        
        // The following code checks if the remaining data to be sent is less that the limits the Uart driver can take.
        if (space_available > Win_Remaining_Data)
        {
            space_available = (uint8)Win_Remaining_Data;
        }
        Win_Remaining_Data -= space_available;
        Uart__Write(WIN_COM,Win_Tx_Pointer,space_available);
        Win_Tx_Pointer += space_available;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method change the state of a sending message to free which means message sent or dropped.
 */
static void DropSendingMessage(void)
{
    Win_Tx_Message_State = WIN_MESSAGE_FREE;
    if (Node_Sent_Counter < 0xFF)
    {
        Node_Sent_Counter++;
    }
}
