/**
 *  @file
 *
 *  @brief      Public Parameters of the Reveal Module
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
#ifndef REVEAL_PRM_H_
#define REVEAL_PRM_H_
#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
/**
 *    @brief  - Defines if the heart beat generation depends on a external source of 1s clock
 *    @details- If set to ENABLED the method Reveal__1sHandler() must be called from 1s time-base
 *
 *    @param    - ENABLED - Reveal uses an internal counter to generate the 1s time-base
				- DISABLED - Reveal__1sHandler() must be called every 1s
 *    @note     - "#define REVEAL_1S_EXTERNAL_REFFERENCE   DISABLED"
 *              - "#define REVEAL_1S_EXTERNAL_REFFERENCE   ENABLED"
 */
//=====================================================================================================================
#define REVEAL_1S_EXTERNAL_REFFERENCE   DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines priority queue feature
 *    @details- When the priority queue is available there are a set of functions to deal with the priority queue
 *              Reveal__PriorityNewFeedback and Reveal__PriorityNewCommand.
 *
 *    @param    - ENABLED - Only the normal queue is available
				- DISABLED - Both Normal and priority queue are available
 *    @note     - "#define REVEAL_QUEUE_PRIORITY           DISABLED"
 *              - "#define REVEAL_QUEUE_PRIORITY           ENABLED"
 */
//=====================================================================================================================
#define REVEAL_QUEUE_PRIORITY           ENABLED

//=====================================================================================================================
/**
 * @brief Defines if the code for queue debug will be present or not in the application
 * --------------------------------------------------------------------------------------------------------------------
 * @note 
 * <pre>
 * When ENABLED adds code to continuously evaluate the minimum space available in the reveal queues.
 * When DISABLED commented out the code to evaluate the reveal queues
 * </pre>
 */
//=====================================================================================================================
#define REVEAL_QUEUE_DEBUG          ENABLED


//=====================================================================================================================
/**
 *    @brief  - Defines the payload size for Reveal
 *    @details- For WIDE it must be defined as 13
 *              For WIN it is in between 1 to 255
 *
 *    @param    - at least 13
 *    @note     - "#define REVEAL_PAYLOAD_SIZE 13"
 */
//=====================================================================================================================
#include "Win.h"
#define REVEAL_PAYLOAD_SIZE                      (WIN_MAX_OUTGOING_DATA_SIZE-5)
#define REVEAL_PAYLOAD_INCOMING_SIZE             (WIN_MAX_INCOMING_DATA_SIZE-5)


//=====================================================================================================================
/**
 * Defines extra reason codes to the reason code enumeration table
 * --------------------------------------------------------------------------------------------------------------------
 * <pre>
 * To leave only the default reason codes leave this define blank
 * To add new reason code you should add them separated by comma ",", start with a comma and don't add comma in the end.
 *   For example:
 *   #define REVEAL_RECIPE_SPECIFIC          ,NVO_RS_MY1,NVO_RS_MY2,NVO_RS_MY3,NVO_RS_MY4
 * </pre>
 */
//=====================================================================================================================
#define REVEAL_RECIPE_SPECIFIC          // ,NVO_RS_MY1,NVO_RS_MY2,NVO_RS_MY3,NVO_RS_MY4

#define REVEAL_SYNCH_PROCESS        ENABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the size of the queue where the reveal commands will be inserted.
 *    @details- Each index contains a full Reveal message allocation.
 *
 *    @param    - 2..0xF
 *    @note     - 5 positions of queue
 *              - "#define REVEAL_QUEUE_SIZE                       5"
 */
//=====================================================================================================================
#ifndef REVEAL_QUEUE_SIZE
    #define REVEAL_QUEUE_SIZE                   15
#endif

//=====================================================================================================================
/**
 *    @brief  - Defines the size of the queue where the reveal self message will be inserted.
 *    @details- It manages the reentrancy and delivers the messages internal/self.
 *
 *    @note
 *              - "#define REVEAL_SELF_QUEUE_SIZE                       2"
 */
//=====================================================================================================================
#ifndef REVEAL_SELF_QUEUE_SIZE
    #define REVEAL_SELF_QUEUE_SIZE                   2
#endif

//=====================================================================================================================
/**
 *    @brief  - Defines the size of the priority queue where the reveal commands will be inserted.
 *    @details- Each index contains a full Reveal message allocation. This directive depends on directive
 *    that enables the priority queue
 *
 *    @param    - 2..0xF
 *    @note     - "#define REVEAL_QUEUE_PRIORITY_SIZE                       5"
 */
//=====================================================================================================================
#ifndef REVEAL_QUEUE_PRIORITY_SIZE
    #define REVEAL_QUEUE_PRIORITY_SIZE          5
#endif



#endif // REVEAL_PRM_H_


