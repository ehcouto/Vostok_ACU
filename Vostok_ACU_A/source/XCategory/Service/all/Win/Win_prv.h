/**
 *  @file       
 *
 *  @brief      Private parameters for WIN protocol module
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
#ifndef WIN_PRV_H_
#define WIN_PRV_H_
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================


/**
 * @brief Exception when a message is retried
 * @details We pass a parameter the pointer to the header of the message (see Win.h WIN_HEADER_TYPE)
 */
#define WIN_RETRY_OUTGOING_MESSAGE(header)

/**
 * @brief Exception when a message is dropped from the queue after the retries with no ack back from the target
 * @details We pass a parameter the pointer to the header of the message (see Win.h WIN_HEADER_TYPE)
 */
#define WIN_DROPPED_OUTGOING_MESSAGE(header)

/**
 * @brief Exception when a message received and the CRC didn't match
 * @details We pass a parameter the pointer to the header of the message (see Win.h WIN_HEADER_TYPE)
 */
#define WIN_BAD_CRC_INCOMING_MESSAGE(header,received_crc,calc_crc)

/**
 * @brief Exception when a message received is dropped due to inconsistent data, basically when the size of the incoming
 * message is bigger than what is configured for in WIN_MAX_INCOMING_DATA_SIZE(Win_prm.h).
 * @details We pass a parameter the pointer to the header of the message (see Win.h WIN_HEADER_TYPE)
 */
#define WIN_DROP_INCOMING_MESSAGE(header)

/**
 * @brief Definition of the speed used by WIN
 * @details By default it uses 38.4KBauds.\n
 *  The speed has to be B38400 baud or faster.\n
 *  The standard speed is B38400 but in some special cases, where the system requires and the microcontrollers allow it can be B57600 or B115200
 * @note Default B38400
 */
#define WIN_SPEED  B38400


/**
 * @brief Definition of the maximum amount of bytes handled in one interaction of the handlers (every 1ms).
 * @details For small microcontrollers it might be too heavy to handle big messages in one interaction, to to prevent slot overrun we can
 * limit the amount of bytes processed per interaction.\n
 * As communication at 38.4KBaud one byte takes 261us in 1ms it will able to transmit /receive around 4 bytes. So for 38.4KBaud this parameter should be configured for 5 or more to prevent gaps in between bytes.
 * If the speed is increase to B115200 for instance one byte will take around 87us in the bus so this parameter should be set to 12 or more.
 * @note Default 5 for B38400
 */
#define WIN_MAX_BYTES_HANDLED_AT_ONCE    5


/**
 * @brief Definition of the initial delay for WIN to start to send messages in the bus, it will be able to receive message and acknowledge them but not send message.
 * @details The intention of this parameter to to prevent WIN do send messages while the other nodes might be in power-up process yet.
 * @note Default 150  which means 150ms \
 * This parameter is limited to 220 ms.
 */
#define WIN_POWERUP_TIME                150

/**
 * @brief Definition of how long the node will be holding communication when reaches WIN_MAX_NUM_MESSAGES messages sent continuously.
 * @details The idea of the Bus access contention feature is to allow nodes with less priority to access the bus once a node with a higher priority to the bus is taking too much from the bus.
 * @note Default 100  which means 100ms.\n
 * To disable the Bus Access Contention feature set this parameter to 1.\n
 * the limits for this parameter are from 1 to 255.
 */
#define WIN_TIME_HOLDING_MSG    100

/**
 * @brief Definition of number of message to engage the Bus Access Contention feature.
 * @details If a node sends WIN_MAX_NUM_MESSAGES in a row without a interval bigger than WIN_TIME_HOLDING_MSG it will
 * engage the Bus Access Contention feature and the node will be prevented to sent messages for WIN_TIME_HOLDING_MSG ms.
 * @note Default 20  which means 20 messages in a row with no interval bigger than WIN_TIME_HOLDING_MSG ms. \n
 * This parameter is limited to 220 ms.\n
 * The limits for this parameter are from 1 to 255.
 */
#define WIN_MAX_NUM_MESSAGES    20

/**
 * @brief Definition of number of retries of a message due to collision.
 * @details If a node retries messages due to it will be limited to this parameter number of retries, after that the message will be dropped..
 * @note Default 10  which means up to 10 reries. \n
 * This parameter ranges from 10 to 255 retries.\n
  */
#define WIN_NUM_RETRIES_COLLISION   10

/**
 * @brief Definition of a modifier to allocate a variables in a memory type such as TINY or NEAR in order to optimize the code
 * @details This is group 1 of variables
 * @note The variables included in this setting are: \n
 *    Win_Timer, Win_Remaining_Data, Win_Bus_Idle, Win_Bus_Idle_Timer\n
 *
 *    For Cosmic is needed:
 *    #define WIN_DATA_SEGMENT1     TINY
 */
#define WIN_DATA_SEGMENT1


/**
 * @brief Definition of a modifier to allocate variables in a memory type such as TINY or NEAR in order to optimize the code
 * @details This is group 2 of variables
 * @note The variables included in this setting are: \n
 *    Win_Tx_State,Win_Rx_State, Win_Rx_Message_State,Win_Tx_Message_State\n
 *
 *    For Cosmic is needed:
 *    #define WIN_DATA_SEGMENT2     TINY
 */
#define WIN_DATA_SEGMENT2

/**
 * @brief Definition of a modifier to allocate variables in a memory type such as TINY or NEAR in order to optimize the code
 * @details This is group 3 of variables
 * @note The variables included in this setting are: \n
 *    Win_Check_Index, Win_Rx_Index,Win_Rx_Crc\n
 *
 *    For Cosmic is needed:
 *    #define WIN_DATA_SEGMENT3     TINY
 */
#define WIN_DATA_SEGMENT3

/**
 * @brief Definition of the timeout for the application to get the message once it became pending ready after received.
 * @brief This parameter shall not be lower than 5 (ms)
 * @note Parameter defines the timeout in ms: \n
 *
 *    #define WIN_MAX_RX_PARSER_TIMEOUT   25
 */
#define WIN_MAX_RX_PARSER_TIMEOUT   25


/**
 * @brief This directive enabled/ disables the node status feature.
 */
#define WIN_NODE_STATUS  ENABLED

#endif // WIN_PRV_H_
