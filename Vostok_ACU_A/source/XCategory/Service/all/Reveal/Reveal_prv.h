/**
 *  @file
 *
 *  @brief      Private parameters of the Reveal module.
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

#ifndef REVEAL_PRV_H_
#define REVEAL_PRV_H_

#include "SystemConfig.h"

//---------------------------------------------------------------------------------------------------------------------
// Macros to disable/restore context (interrupts/RTOS) on New command/feedback methods

#ifndef REVEAL_FORCE_INTERRUPT_CONTEXT
    #define REVEAL_FORCE_INTERRUPT_CONTEXT      DISABLED
#endif

#ifndef REVEAL_FORCE_EMBOS_CONTEXT
    #define REVEAL_FORCE_EMBOS_CONTEXT          DISABLED
#endif

#ifndef REVEAL_DISABLE_CONTEXT
    #define REVEAL_DISABLE_CONTEXT              DISABLED
#endif

#if(!defined(REVEAL__SAVE_CONTEXT))
    #if(REVEAL_FORCE_INTERRUPT_CONTEXT == ENABLED)
        #include "Micro.h"

        #define REVEAL__DECLARE_CONTEXT()           MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL()
        #define REVEAL__SAVE_CONTEXT()              MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL()
        #define REVEAL__RESTORE_CONTEXT()           MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL()
    #elif((SYSTEM_MCU_HANDLING == DISABLED) || (REVEAL_FORCE_EMBOS_CONTEXT == ENABLED))
        // MCU is not using embOS yet, and don't need context protection
        #include "RTOS.h"

        #define REVEAL__DECLARE_CONTEXT()
        #define REVEAL__SAVE_CONTEXT()              OS_EnterRegion()
        #define REVEAL__RESTORE_CONTEXT()           OS_LeaveRegion()
    #elif((SYSTEM_MCU_HANDLING == ENABLED) || (REVEAL_DISABLE_CONTEXT == ENABLED))
        #define REVEAL__DECLARE_CONTEXT()
        #define REVEAL__SAVE_CONTEXT()
        #define REVEAL__RESTORE_CONTEXT()
    #endif
#endif

//---------------------------------------------------------------------------------------------------------------------

#if (API003DISC_FEATURE == ENABLED)
    #include "API003Disc.h"
#endif

#if (API004DEBUG_FEATURE == ENABLED)
    #include "API004Debug.h"
#endif

#if (API005LOWLEVEL_FEATURE == ENABLED)
    #include "API005LowLevel.h"
#endif

#if (API006HMI_FEATURE == ENABLED)
    #include "API006Hmi.h"
#endif

#if (API007DATA_FEATURE == ENABLED)
    #include "API007Data.h"
#endif

#if (API009SYSTEM_FEATURE == ENABLED)
    #include "API009System.h"
#endif

#if (API010POLLVAR_FEATURE == ENABLED)
    #include "API010PollVar.h"
#endif

#if (API011APPCTRL_FEATURE == ENABLED)
    #include "API011AppCtrl.h"
#endif

#if (API012EXP_FEATURE == ENABLED)
    #include "API012Exp.h"
#endif

#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
    #include "API013RemoteFunction.h"
#endif

#if (API017TOUCH_FEATURE == ENABLED)
    #include "API017Touch.h"
#endif

#if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
    #include "API018Diagnostic.h"
#endif

#if (API019PRODUCTINFO_FEATURE == ENABLED)
    #include "API019ProductInfo.h"
#endif

#if (API020SAFETY_FEATURE == ENABLED)
    #include "SRAPI020Safety.h"
#endif

#if ((API221_CLIENT_FEATURE == ENABLED) || (API221_SERVER_FEATURE == ENABLED))
    #include "API221MotionCtrl.h"
#endif

#if (API220MOTORCONTROL_FEATURE == ENABLED)
    #include "APIMotorControl.h"
#endif

#if (CONNECTIVITY_HMI_FEATURE == ENABLED)
    #include "API141Connectivity.h"
    #include "API144NetworkObjectsComm32.h"
#endif

#if (CONNECTIVITY_FEATURE == ENABLED)
    #include "API147SystemMonitoring.h"
    #include "API151ApplianceInfo.h"
#endif

#if (API201_DEVICE_SECURITY_FEATURE == ENABLED)
    #include "API201DeviceSecurity.h"
#endif

//=====================================================================================================================
/**
 *    @brief  - Registers the APIS to be parsed by Reveal
 *    @details- 1. IF a API doesn't have a specific method is could filled up with NULL.
 *              2. The Node only cause any difference to the system if the Router is enabled.
 *
 *    @note
 * <pre>
 *  REVEAL_PARSER_TYPE const REVEAL_PARSER[] =
 *  {
 *      {API        ,Command Parser             ,Feedback Parser            ,Ack Parser},
 *      {API200UI   ,NULL_CMD_PARSER            ,API200UI__FeedbackParser   ,API200UI__AckParser},
 *      {API201UI   ,API201UI__CommandParser    ,API201UI__FeedbackParser   ,NULL_ACK_PARSER},
 *      {API200CCU  ,API200CCU__CommandParser   ,NULL_FDB_PARSER            ,API200CCU__AckParser},
 *      {API002     ,API002__CommandParser      ,API002__FeedbackParser     ,API002__AckParser}
 *  };
 * </pre>
 */
//=====================================================================================================================

typedef struct
{
    uint8 API;
    REVEAL_PARSER_COMMAND_FUNCTION_TYPE COMMAND_PARSER;
    REVEAL_PARSER_FEEDBACK_FUNCTION_TYPE FEEDBACK_PARSER;
    REVEAL_PARSER_ACK_TYPE ACK_PARSER;
} REVEAL_PARSER_TYPE;

#define REVEAL_MODULE_INTERFACE(api, command_parser, feedback_parser, ack_parser)\
	/* API, COMMAND_PARSER, FEEDBACK_PARSER,    ACK_PARSER */\
	{ api,  command_parser, feedback_parser,    ack_parser }

/*
 * Use the macro REVEAL_MODULE_INTERFACE() to populate the table.
 */
static REVEAL_PARSER_TYPE const REVEAL_PARSER[] =
{
    #if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == ENABLED))
        REVEAL_MODULE_INTERFACE(API221MOTIONCTRL_NUM    ,API221MotionCtrl__CommandParser    ,API221MotionCtrl__FeedbackParser   ,NULL_ACK_PARSER),
    #elif (API221_CLIENT_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API221MOTIONCTRL_NUM    ,NULL_CMD_PARSER                    ,API221MotionCtrl__FeedbackParser   ,NULL_ACK_PARSER),
    #elif (API221_SERVER_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API221MOTIONCTRL_NUM    ,API221MotionCtrl__CommandParser    ,NULL_FDB_PARSER                    ,NULL_ACK_PARSER),
    #endif

    #if ((API020SAFETY_FEATURE == ENABLED) && (API020_NUMBER_OF_RX_CHANNELS > 0))
        REVEAL_MODULE_INTERFACE(API020SAFETY_NUM    ,NULL_CMD_PARSER    ,SRAPI020Safety__FeedbackParser ,NULL_ACK_PARSER),
    #endif

    #if (API019PRODUCTINFO_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API019PRODUCTINFO_NUM   ,API019ProductInfo__CommandParser   ,API019ProductInfo__FeedbackParser  ,API019ProductInfo__AckParser),
    #endif

    #if (API018_SERVER_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API018DIAGNOSTIC_NUM    ,API018Diagnostic__CommandParser    ,NULL_FDB_PARSER                    ,NULL_ACK_PARSER),
    #elif (API018_CLIENT_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API018DIAGNOSTIC_NUM    ,NULL_CMD_PARSER                    ,API018Diagnostic__FeedbackParser   ,NULL_ACK_PARSER),
    #endif

    #if (API017TOUCH_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API017TOUCH ,API017Touch__CommandParser ,NULL_FDB_PARSER    ,NULL_ACK_PARSER),
    #endif

    #if (API013_CLIENT_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API013RMTFCN_NUM    ,NULL_CMD_PARSER                        ,API013RemoteFunction__FeedbackParser   ,NULL_ACK_PARSER),
    #elif (API013_SERVER_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API013RMTFCN_NUM    ,API013RemoteFunction__CommandParser    ,NULL_FDB_PARSER                        ,NULL_ACK_PARSER),
    #endif

    #if (API012EXP_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API012EXP_NUM   ,API012Exp__CommandParser ,API012Exp__FeedbackParser  ,API012Exp__AckParser),
    #endif

    #if (API011APPCTRL_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API011APPCTRL_NUM   ,API011AppCtrl__CommandParser   ,API011AppCtrl__FeedbackParser  ,NULL_ACK_PARSER),
    #endif

    #if (API010POLLVAR_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API010POLLVAR_NUM   ,API010PollVar__CommandParser ,NULL_FDB_PARSER    ,NULL_ACK_PARSER),
    #endif

    #if (API009SYSTEM_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API009SYSTEM_NUM    ,API009System__CommandParser    ,API009System__FeedbackParser   ,API009System__AckParser),
    #endif

    #if (API007DATA_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API007DATA_NUM  ,API007Data__CommandParser   ,API007Data__FeedbackParser   ,API007Data__AckParser),
    #endif

    #if (API006HMI_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API006HMI_NUM   ,API006Hmi__CommandParser   ,NULL_FDB_PARSER    ,NULL_ACK_PARSER),
    #endif

    #if (API005LOWLEVEL_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API005LOWLEVEL_NUM  ,API005LowLevel__CommandParser   ,NULL_FDB_PARSER  ,NULL_ACK_PARSER),
    #endif

    #if (API004DEBUG_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API004DEBUG_NUM ,API004Debug__CommandParser ,NULL_FDB_PARSER    ,NULL_ACK_PARSER),
    #endif

    #if (API003DISC_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API003DISC_NUM  ,API003Disc__CommandParser   ,API003Disc__FeedbackParser   ,NULL_ACK_PARSER),
    #endif

    #if (API220MOTORCONTROL_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API220_MOTOR_CONTROL_NUM    ,APIMotorControl__CommandParser    ,APIMotorControl__FeedbackParser   ,NULL_ACK_PARSER),
    #endif

    #if (CONNECTIVITY_HMI_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API141_NUM  ,API141Connectivity__CommandParser          ,API141Connectivity__FeedbackParser         ,NULL_ACK_PARSER),
        REVEAL_MODULE_INTERFACE(API141_NUM  ,API144NetworkObjectsComm32__CommandParser  ,API144NetworkObjectsComm32__FeedbackParser ,NULL_ACK_PARSER),
    #endif

    #if (CONNECTIVITY_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API147_NUM  ,API147SystemMonitoring__CommandParser  ,API147SystemMonitoring__FeedbackParser ,NULL_ACK_PARSER),
        REVEAL_MODULE_INTERFACE(API151_NUM  ,API151ApplianceInfo__CommandParser     ,API151ApplianceInfo__FeedbackParser    ,NULL_ACK_PARSER),
    #endif

    #if (API201_DEVICE_SECURITY_FEATURE == ENABLED)
        REVEAL_MODULE_INTERFACE(API201DEVICESECURITY_NUM    ,API201DeviceSecurity__CommandParser    ,NULL_FDB_PARSER    ,NULL_ACK_PARSER),
    #endif
};

//=====================================================================================================================
/**
*    @brief     Defines all possible APIs that can be blocked by the Block Messages feature.
*
*    @details   API 1 is intentionally omitted as it should never be blocked.
*               The BLOCKED_APIS_EXTENSION macro can be used to define additional APIs that need to be blocked on a
*               project specific basis, however this list should also be updated to leverage reuse.
*               API 1 should never be added to this list.
*/
//=====================================================================================================================
 static uint8 const POSSIBLE_BLOCKED_APIS_LIST[] =
 {
     3,4,5,7,9,10,11,12,13,17,18,19,20,220,221,141,144,147,151,
     #ifdef BLOCKED_APIS_EXTENSION
         BLOCKED_APIS_EXTENSION
     #endif
 };


//=====================================================================================================================
/**
 *    @brief  - Defines the maximum number of retries a command will be re-send in case of no ack being received .
 *    @details- By spec this value should be 3
 *
 *    @param    - 3
 *    @note     - "#define REVEAL_RETRY_TIMES                      3"
 */
//=====================================================================================================================
#define REVEAL_RETRY_TIMES                      3

//=====================================================================================================================
/**
 *    @brief  - Timeout for the reception of ack for sent commands
 *    @details- This time depends on the system but usually it is set to 200
 *              Since the counter is handled in a 5ms timebase it's command to give a value in (xxx/5) where xxx is in ms
 *
 *    @param    - 100 too 65535
 *    @note     - 200ms in a 5ms timebase
 *    			- "#define REVEAL_TIMEOUT                          (200/5)"
 */
//=====================================================================================================================
#define REVEAL_TIMEOUT                          (200/5)       //*5ms

//=====================================================================================================================
/**
 *    @brief  - Definition of the initial heart beat
 *    @details- Value given in seconds
 *
 *    @param    - 0 to 65535 s
 *    @note     - 0 - Heart beat disabled after reset
 *    			- 5 - Heart beat send every 5s after reset
 *    			- "#define REVEAL_INITIAL_HEARTBEAT              0"
 *                "#define REVEAL_INITIAL_HEARTBEAT              5"
 */
//=====================================================================================================================
#define REVEAL_INITIAL_HEARTBEAT                5   // heart beat after reset in seconds, 0 means disabled

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle a heart beat received from another node
 *    @details- source represents the node who sent the heartbeat
 *
 *    @note     - "#define REVEAL_HEARTBEAT_CALLBACK(source)   MyHandler__ParseHeartBeat(source)"
 */
//=====================================================================================================================
#define REVEAL_HEARTBEAT_CALLBACK(source)

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle acknowledge when received
 *    @details- Reveal 4 does handle the ack when the AckHandler is registered in REVEAL_PARSER, this callback is an extra
 *              call that could be used.
 *                source represents the node who sent the ack
 *                api and opcode the message which is being acknowledged
 *                reason_code is the reason code of the ack message
 *
 *    @note     #define REVEAL_CALLBACK_ACK_MSG(source,api,opcode,reason_code) MyHandler__ParseAck(source,api,opcode,reason_code)
 */
//=====================================================================================================================
#define REVEAL_CALLBACK_ACK_MSG(source,api,opcode,reason_code)

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle ack queue full
 *    @details- When other nodes sent messages to this node it can queue acks for the other nodes. This callback handles
 *     situations where this queue is full.
 *
 *    @note     #define REVEAL_EXCEPTION_ACK_QUEUE_FULL(source,api,opcode,reason_code)
 */
//=====================================================================================================================
#define REVEAL_EXCEPTION_ACK_QUEUE_FULL(source,api,opcode,reason_code)

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle dropped message due to exceed the number of retries with no ack
 *
 *    @note     #define REVEAL_DROPPED_MESSAGE_EXCEPTION(api,opcode,node)
 */
//=====================================================================================================================
#define REVEAL_DROPPED_MESSAGE_EXCEPTION(api,opcode,node)

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle queue full
 *
 *    @note     #define REVEAL_QUEUE_OVERFLOW_NORMAL_EXCEPTION(api,op)
 */
//=====================================================================================================================
#define REVEAL_QUEUE_OVERFLOW_NORMAL_EXCEPTION(api,op)

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle priority queue full
 *
 *    @note     #define REVEAL_QUEUE_OVERFLOW_PRIORITY_EXCEPTION(api,op)
 */
//=====================================================================================================================
#define REVEAL_QUEUE_OVERFLOW_PRIORITY_EXCEPTION(api,op)


//=====================================================================================================================
/**
 *    @brief  - Callback use check the reason code of received messages
 *
 *    @note     #define REVEAL__ACK_INCOMING_MESSAGE(node,api,op_code,reason_code)
 */
//=====================================================================================================================
#define REVEAL__ACK_INCOMING_MESSAGE(node,api,op_code,reason_code)

//=====================================================================================================================
/**
 *    @brief  - Directive to allow API1 OPCODE 6 which is used to read eeprom memory
 *
 *    @note     #define REVEAL_READ_EEPROM_FEATURE		ENABLED
 *              #define REVEAL_READ_EEPROM_FEATURE		DISABLED
 */
//=====================================================================================================================
#define REVEAL_READ_EEPROM_FEATURE          DISABLED

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle request to read eeprom
 *
 *    @note     #define REVEAL_EEPROM_READ(source,address,size)  MyFunctionEepromRead(source,address,size)
 */
//=====================================================================================================================
#define REVEAL_EEPROM_READ(source,address,size)

//=====================================================================================================================
/**
 *    @brief  - Configures the feature to read memory using Reveal
 *    @param    - ENABLED / DISABLED
 *    @note     - "#define REVEAL_READ_MEMORY_FEATURE              ENABLED"
 */
//=====================================================================================================================
#define REVEAL_READ_MEMORY_FEATURE              ENABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the initial interval where the memory to be exposed is located
 *    @details-
 *    @param    - initial position of the RAM
 *    @note     - "#define REVEAL_READ_MEMORY_START                0x0000"
 */
//=====================================================================================================================
#define REVEAL_READ_MEMORY_START                0x0000


//=====================================================================================================================
/**
 *    @brief  - Defines the end of the memory which can be read via Reveal.
 *    @details- If the value configured here is below 0x1FFFFFF it will allow the API001 opcode 5 to read memory (24bits address)
 *              if the value configured here is equal or bigger than 0x1FFFFFF it will allow API 001 opcode 9 (32bits addresses)
 *
 *    @param    - final address of the memory
 *    @note     - #define REVEAL_READ_MEMORY_END                  0xFFFF
 */
//=====================================================================================================================
#define REVEAL_READ_MEMORY_END                  0xFFFFFFFFU

//=====================================================================================================================
//
/**
 *    @brief - Defines the Reveal full search internal
 *    @details- This is used to search parser for internal message routing. In case for one API different parsers need to be called
 *              then this macro should be TRUE, default it is FALSE
 *    @param - TRUE/FALSE
 *    #define REVEAL_FULL_SERACH_INTERNAL    FALSE
 */
//=====================================================================================================================
#define REVEAL_FULL_SERACH_INTERNAL    FALSE

//=====================================================================================================================
/**
 *    @brief  - Callback use to handle REVEAL exception for retry count overload
 *
 *    @note     REVEAL_EXCEPTION_REENTRY_CNT_OVERLOADED(api_id,op_code)  MyFunctionEepromRead(api_id,op_code)
 *    @return   FALSE
 *
 */
//=====================================================================================================================
#define REVEAL_EXCEPTION_REENTRY_CNT_OVERLOADED(api_id,op_code)

//=====================================================================================================================
/**
 *
 */
//=====================================================================================================================
#if ((SETTINGFILE_MASTER_FILE_FEATURE == ENABLED) || (SETTINGFILE_SECTION_DOWNLOAD_FEATURE == ENABLED))
#include "SFUpdater.h"
#define REVEAL_MAX_PAYLOAD_CALLBACK(node,max_size)      SFUpdater__RxMaxPackage(node,max_size)
#else
#define REVEAL_MAX_PAYLOAD_CALLBACK(node,max_size)
#endif

//==========================================================================================================================================================================================================================================
/**
 *    @brief  - Configuration of the low level communication protocol used under Reveal.
 *    @note
 *    <pre>
 *      For WIDE usage
 *          #include "Wide.h"
 *          typedef WIDEPACKAGE_MSG_TYPE        PT_RX_TYPE;
 *          typedef WIDEPACKAGE_MSG_TYPE        PT_TX_TYPE;
 *          typedef WIDE_QUEUED_MSG_TYPE        LLC_QUEUED_TYPE;
 *          #define GET__MESSAGE(sap)           Wide__GetIncomingBuffer(sap)
 *          #define FREE_MESSAGER(sap)
 *          #define ALLOCATE_BUFFER()           Wide__GetBufferFromQueue()
 *          #define QUEUE_MESSAGER()            Wide__QueueMsg()
 *          #define LLC_INVALID_ADDRESS         WIDE_QUEUED_INVALID_ADDRESS
 *          #define LLC_INVALID_SIZE            WIDE_QUEUED_INVALID_ADDRESS
 *          #define LLC_OWN_ADDRESS				address
 *
 *      For WIN usage
 *          #include "Win.h"
 *          typedef WIN_INCOMING_MESSAGE_TYPE   PT_RX_TYPE;
 *          typedef WIN_OUTGOING_MESSAGE_TYPE   PT_TX_TYPE;
 *          typedef WIN_OPER_TYPE               LLC_QUEUED_TYPE;
 *          #define GET__MESSAGE(sap)           Win__GetIncomingMessage(sap)
 *          #define FREE_MESSAGER(sap)          Win__FreeIncomingMessage(sap)
 *          #define ALLOCATE_BUFFER()           Win__AllocateMessage()
 *          #define QUEUE_MESSAGER()            Win__QueueMessage()
 *          #define LLC_INVALID_ADDRESS         WIN_OPER_INVALID_ADDRESS
 *          #define LLC_INVALID_SIZE            WIN_OPER_INVALID_SIZE
 *          #define LLC_OWN_ADDRESS				Win__GetAddress()
 *    </pre>
 */
//=====================================================================================================================

#include "Win.h"
typedef WIN_INCOMING_MESSAGE_TYPE   PT_RX_TYPE;
typedef WIN_OUTGOING_MESSAGE_TYPE   PT_TX_TYPE;
typedef WIN_OPER_TYPE               LLC_QUEUED_TYPE;
#define GET__MESSAGE(sap)           Win__GetIncomingMessage(sap)
#define FREE_MESSAGER(sap)          Win__FreeIncomingMessage(sap)
#define ALLOCATE_BUFFER(destination)           Win__AllocateMessage()
#define QUEUE_MESSAGER(destination)            Win__QueueMessage()
#define LLC_INVALID_ADDRESS         WIN_OPER_INVALID_ADDRESS
#define LLC_INVALID_SIZE            WIN_OPER_INVALID_SIZE
#define LLC_OWN_ADDRESS				Win__GetAddress()


#endif // REVEAL_PRV_H_


