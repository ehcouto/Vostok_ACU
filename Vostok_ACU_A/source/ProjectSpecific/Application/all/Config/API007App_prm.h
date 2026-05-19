/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007APP_PRM_H
#define API007APP_PRM_H

#include "SystemConfig.h"

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Define the NUmber of segments the node is going to manage
 *          End the segment list with the API007APP_MAX_NUMBER_SEGMENTS
 *
 */
typedef enum
{
    API007APP_SEGMENT_0 = 0,     //!< API007APP_SEGMENT_1
//	API007APP_SEGMENT_1,     //!< API007APP_SEGMENT_2
//	API007APP_SEGMENT_2,     //!< API007APP_SEGMENT_3
//	API007APP_SEGMENT_3,     //!< API007APP_SEGMENT_4
//	API007APP_SEGMENT_4,     //!< API007APP_SEGMENT_5
//	API007APP_SEGMENT_5,     //!< API007APP_SEGMENT_6
//    API007APP_SEGMENT_6,     //!< API007APP_SEGMENT_7
    API007APP_MAX_NUMBER_SEGMENTS//!< API007APP_MAX_NUMBER_SEGMENTS
} API007APP_SEGMENT_ID;


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - List the segments the node is going to handle. This list must be from the API007APP_SEGMENT_ID
 *
 */
#define API007_BULK_DATA_SEGMENT_LIST \
{ \
    API007APP_SEGMENT_0 \
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - ENABLED/DISABLED the below macro if the node acts as an server for the data segments
 *
 *          Definition of Server -
 *          Does Management of the Data storing and restoring in the Non Volatile Data Memory
 *          Services the Client request which could be facilitating to write the data/Read the bulk Data from the server
 *
 *          For Nucleus Framework the NVData management is done in the ACU side only. In this case the ACU acts as an server
 *
 */
#define API007APP_SERVER_FEATURE        DISABLED
#define APPI007APP_SERVER_NODE_ID       WIN_ACU_ADDRESS

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - ENABLED/DISABLED the below macro if the node acts as an Client for the data segments
 *
 *          Definition of Client -
 *          Does Management of requesting the Data from the Server or Sending the data to the server to store the data
 *
 *          This could be an HMI or MCU
 *
 */
#define API007APP_CLIENT_FEATURE        ENABLED
#define APPI007APP_CLIENT_NODE_ID       WIN_UI_ADDRESS


#if (API007APP_CLIENT_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This call back will be called when the node receives the Segment information from the client
 *          Used for Server side
 */
#define NOTIFY_SEGMENT_INFO_RECEIVED( segment_id, role, size, crc)      API007App__SetSegmentInfo(segment_id, role, size, crc)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This call back will be called when the node receives the acknowledge on previousely requested write command
 *          Used for Server side
 */
#define NOTIFY_ACK_WRITE_COMPLETE(address)                              API007App__ServerwriteCompleted(address);

#endif

#endif // API007APP_PRM_H
