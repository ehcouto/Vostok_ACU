 /**
 *  @file       
 *
 *  @brief      API007App module implementation
 *
 *  @details    This module takes care of reading and writing mechanism to the memory for bulk data storage.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *
 *  @copyright  Copyright 2013-$Date: 2013/09/13 10:23:07EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#include "API007BulkData.h"
#include "Log.h"
#include "String.h"

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
#include "Reveal.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! list of module Log messages
typedef enum
{
     MESSAGE_NONE = 0
    ,INVALID_CRC_FAILED                           //!< data: na
} MODULE_API007BULKDATA_LOG_MESSAGE_ID_TYPE;
#define MODULE_NAME MODULE_API007BULKDATA

//Invalid Segment ID or Segment ID not found.
#define API007DATA_INVALID_SEGMENT_INDEX                255                    //Invalid Segment ID for the Node

//! Message response timeout
#ifndef MESSAGE_RESPONSE_TIMEOUT_MS
    #define MESSAGE_RESPONSE_TIMEOUT_MS                 5000
#endif

#define MESSAGE_RESPONSE_TIMER_COUNTS                   (MESSAGE_RESPONSE_TIMEOUT_MS / COMMUNICATION_THREAD_CALL_RATE_MS)

#ifndef MESSAGE_WRITE_TIMEOUT_MS
    #define MESSAGE_WRITE_TIMEOUT_MS                    5000
#endif
#define MESSAGE_WRITE_TIMEOUT_COUNTS                   (MESSAGE_WRITE_TIMEOUT_MS / COMMUNICATION_THREAD_CALL_RATE_MS)

#ifndef MESSAGE_GENERIC_TIMEOUT_MS
    #define MESSAGE_GENERIC_TIMEOUT_MS                    5000
#endif
#define MESSAGE_GENERIC_TIMEOUT_COUNTS                   (MESSAGE_GENERIC_TIMEOUT_MS / COMMUNICATION_THREAD_CALL_RATE_MS)

typedef enum
{
    API007BULKDATA_TX_STATE_IDLE = 0,
    API007BULKDATA_TX_STATE_PREPARE = 1,
    API007BULKDATA_TX_STATE_SEND = 2,
    API007BULKDATA_TX_STATE_WAIT = 3,
    API007BULKDATA_TX_STATE_COMPLETE = 4,
    API007BULKDATA_TX_STATE_ERROR = 5
} API007BULKDATA_TX_STATE_TYPE;

static API007BULKDATA_TX_STATE_TYPE API007BulkData_Tx_State;
static API007APP_SEGMENT_ID  API007BulkData_Tx_Segment;
static uint8  API007BulkData_Tx_Segment_Index;
static uint16 API007BulkData_Tx_Index;
static uint16 API007BulkData_Tx_Destination;

typedef enum
{
    API007BULKDATA_RX_STATE_IDLE = 0,
    API007BULKDATA_RX_STATE_WRITE = 1,
    API007BULKDATA_RX_STATE_WAIT = 2,
    API007BULKDATA_RX_STATE_COMPLETE = 3,
    API007BULKDATA_RX_STATE_ERROR = 4
} API007BULKDATA_RX_STATE_TYPE;

static API007BULKDATA_RX_STATE_TYPE API007BulkData_Rx_State;
static API007APP_SEGMENT_ID  API007BulkData_Rx_Segment;
static uint8  API007BulkData_Rx_Segment_Index;
static uint16 API007BulkData_Rx_Index;
static uint16 API007BulkData_Rx_Size;
static uint16 API007BulkData_Rx_Destination;
static uint8  API007BulkData_Rx_Buffer[REVEAL_PAYLOAD_SIZE];

static uint16 API007BulkData_Message_Response_Timer;
static uint16 API007BulkData_Write_Operation_Timeout;
static uint16 API007BulkData_Generic_Timeout;

static const API007APP_SEGMENT_ID API007BULKDATA_SEGMENT_ID[] = API007_BULK_DATA_SEGMENT_LIST;

//! Provides segment_id size
static API007_SEGMENT_INFO_TYPE API007BulkData_Info_List[API007APP_MAX_NUMBER_SEGMENTS];

//! Add here buffer to received the list of segmentID's from external node
static uint8 API007BulkData_Rx_Segment_ListRecived [API007APP_MAX_NUMBER_SEGMENTS];

//! Add here buffer to received segment_id detail info
static API007_SEGMENT_INFO_TYPE API007BulkData_Rx_Segment_IdDetailInfo[API007APP_MAX_NUMBER_SEGMENTS];

static API007DATA_ERROR_TYPE API007BulkData_Rx_ErrorTypeRecived[API007APP_MAX_NUMBER_SEGMENTS];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint8 GetSegmentIndex(API007APP_SEGMENT_ID segment_id);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module API007App and its variables
 *
 */
void API007BulkDataMgr__Initialize(void)
{
    API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
    API007BulkData_Rx_State = API007BULKDATA_RX_STATE_IDLE;
    API007BulkData_Tx_Segment = (API007APP_SEGMENT_ID)0;
    API007BulkData_Tx_Index = 0;
    API007BulkData_Tx_Segment_Index = 0xFF;
    API007BulkData_Write_Operation_Timeout = 0;
    API007BulkData_Message_Response_Timer = 0;

    memset(API007BulkData_Rx_Segment_ListRecived, 0, sizeof(API007BulkData_Rx_Segment_ListRecived));
    memset(API007BulkData_Rx_Segment_IdDetailInfo, 0, sizeof(API007BulkData_Rx_Segment_IdDetailInfo));

    //Read the Segment information from the App module
    for (uint8 segment_index = 0; segment_index < API007APP_MAX_NUMBER_SEGMENTS; segment_index++)
    {
        API007_SEGMENT_INFO_TYPE *segment_info_data = &API007BulkData_Info_List[segment_index];
        API007_SEGMENT_INFO_TYPE *segment_info = API007BulkDataMgr__GetSegmentInfo(API007BULKDATA_SEGMENT_ID[segment_index]);

        if (segment_info != NULL)
        {
            memcpy(segment_info_data, segment_info, sizeof(API007_SEGMENT_INFO_TYPE));
        }
        else
        {
            segment_info_data->crc               = 0;
            segment_info_data->segment_role      = API007_SEGMENT_ROLE_CLIENT;
            segment_info_data->size              = 0;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Exposes the list of segments in this node.
 * @return pointer to the segmenet's list.
 */
API007APP_SEGMENT_ID * API007BulkDataMgr__GetSegmentList(void)
{
    return ((API007APP_SEGMENT_ID *)API007BULKDATA_SEGMENT_ID);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Exposes the number of segments in this node.
 * @return size of the segment_id's list.
 */
uint8 API007BulkDataMgr__GetSegmentListSize(void)
{
    return (sizeof(API007BULKDATA_SEGMENT_ID));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Exposes the information about a segment_id in this node.
 * @param segment_id   Segment ID for a single logical block of data.
 * @return
 */
API007_SEGMENT_INFO_TYPE * API007BulkDataMgr__GetSegmentInfo(API007APP_SEGMENT_ID segment_id)
{
    return (API007App__GetSegmentInfo(segment_id));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Exposes the information about a segment_id in this node.
 * @param segment_id
 * @return
 */
API007_SEGMENT_INFO_TYPE * API007BulkDataMgr__ReadReceivedSegmentInfo(API007APP_SEGMENT_ID segment_id)
{
    uint8 segment_index = GetSegmentIndex(segment_id);

    if(segment_index < API007APP_MAX_NUMBER_SEGMENTS)
    {
        return &API007BulkData_Rx_Segment_IdDetailInfo[segment_index];
    }
    else
    {
        return ((API007_SEGMENT_INFO_TYPE * )NULL);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method prepares transmission state ready to publish segment_id ID data.
 * @param node      Node address who send data.
 * @param segment_id   Segment ID for a single logical block of data.
 * @return
 */
BOOL_TYPE API007BulkDataMgr__GetSegmentData(uint8 node, API007APP_SEGMENT_ID segment_id)
{
    BOOL_TYPE ret_value = FALSE;

    if (API007BulkData_Tx_State == API007BULKDATA_TX_STATE_IDLE)
    {
        //Get Segment ID to API007BulkData_Tx_Segment_Index variable
        API007_SEGMENT_INFO_TYPE * segment_info = API007App__GetSegmentInfo(segment_id);

        if (segment_info != NULL)
        {
            API007BulkData_Tx_Segment_Index = GetSegmentIndex(segment_id);
            if (API007BulkData_Tx_Segment_Index != API007DATA_INVALID_SEGMENT_INDEX)
            {
                //Copy the Segment Information
                memcpy(&API007BulkData_Info_List[API007BulkData_Tx_Segment_Index], segment_info, sizeof(API007_SEGMENT_INFO_TYPE));

                API007BulkData_Tx_Segment = segment_id;
                API007BulkData_Tx_Index = 0;
                API007BulkData_Tx_Destination = node;
                API007BulkData_Tx_State = API007BULKDATA_TX_STATE_PREPARE;

                ret_value = TRUE;
            }
            else
            {
                //Publish an error that segment_id ID not found for this node
                API007Data__SendError(node,segment_id,API007_ERROR_SEGMENT_NOT_FOUND);
            }
        }
        else
        {
            //Publish an error that segment_id ID not found for this node
            API007Data__SendError(node,segment_id,API007_ERROR_SEGMENT_NOT_FOUND);
        }
    }

    return ret_value;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method writes received list of segment_id ID's from external the node.
 * @param address   Node address who send data.
 * @param data      Pointer to the data source.
 * @param size      Size of the data source.
 */
void API007BulkDataMgr__WriteSegmentList(uint8 address,uint8 * data, uint8 size)
{
    if (size > sizeof(API007BulkData_Rx_Segment_ListRecived))
    {
    	size = sizeof(API007BulkData_Rx_Segment_ListRecived);
    }
    memcpy(API007BulkData_Rx_Segment_ListRecived, data, size);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method writes received segment_id ID detailed Info from the external node.
 * @param address   Node address who sent the data.
 * @param segment_id   Segment where the data will be stored.
 * @param index     Index within the segment_id where the received data is related to.
 * @param data      Pointer to the data source.
 * @param size      Size of the data source.
 */
void API007BulkDataMgr__WriteDetailInfo(uint8 address, API007APP_SEGMENT_ID segment_id,API007_SEGMENT_ROLE_TYPE role, uint16 size, uint16 crc)
{
    uint8 segment_index = GetSegmentIndex(segment_id);

    if(segment_index < API007APP_MAX_NUMBER_SEGMENTS)
    {
        API007BulkData_Rx_Segment_IdDetailInfo[segment_index].size          = size;
        API007BulkData_Rx_Segment_IdDetailInfo[segment_index].segment_role  = role;
        API007BulkData_Rx_Segment_IdDetailInfo[segment_index].crc           = crc;

        #ifdef NOTIFY_SEGMENT_INFO_RECEIVED
            //This call back will be called when the node receives the Segment information from the client. Used for Server side
            NOTIFY_SEGMENT_INFO_RECEIVED(segment_id, role, size, crc);
        #endif
    }


}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method writes received segment_id ID data from external node to Received Buffer memory.
 * @param address   Node address who sent the data
 * @param segment_id   Segment ID where the data will be stored
 * @param index     Index within the segment_id where the received data is related to.
 * @param data      Pointer to the data source.
 * @param size      Size of the data source.
 */
void API007BulkDataMgr__WriteSegmentData(uint8 address ,API007APP_SEGMENT_ID segment_id ,uint16 index ,uint8 * data ,uint8 size )
{
    if (API007BulkData_Rx_State == API007BULKDATA_RX_STATE_IDLE)
    {
        API007BulkData_Rx_Segment = segment_id;
        API007BulkData_Rx_Segment_Index = GetSegmentIndex(segment_id);
        if (API007BulkData_Rx_Segment_Index != API007DATA_INVALID_SEGMENT_INDEX)
        {
            if ((API007BulkData_Info_List[API007BulkData_Rx_Segment_Index].segment_role == API007_SEGMENT_ROLE_SERVER) ||
            	(API007BulkData_Rx_Segment_IdDetailInfo[API007BulkData_Rx_Segment_Index].segment_role == API007_SEGMENT_ROLE_SERVER))
            {
                API007BulkData_Rx_Index = index;
                API007BulkData_Rx_Size = size;
                memcpy(API007BulkData_Rx_Buffer, data, size);
                API007BulkData_Rx_State = API007BULKDATA_RX_STATE_WRITE;
                API007BulkData_Rx_Destination = address;
            }
            else
            {
                //Publish error writing data on segment_id ID defined as server
                API007Data__SendError(address, segment_id, API007_ERROR_WRITE_ON_SERVER);
            }
        }
        else
        {
            //Publish error that segment_id ID not found for this node
            API007Data__SendError(address, segment_id, API007_ERROR_SEGMENT_NOT_FOUND);
        }
    }
    else
    {   //Publish an error busy writing
        API007Data__SendError(address, segment_id, API007_ERROR_BUSY_WRITING);
    }
}

void API007BulkDataMgr__AckWriteComplete(uint8 address)
{
    #ifdef NOTIFY_ACK_WRITE_COMPLETE
        //This call back will be called when the node receives the acknowledge on previousely requested write command. Used for Server side
        NOTIFY_ACK_WRITE_COMPLETE(address);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method writes received Error Type from external node to Received Buffer memory.
 * @param address   Node address who sent the data
 * @param segment_id   Segment ID
 * @param error_type
 */
void API007BulkDataMgr__WriteErrorType(uint8 address, API007APP_SEGMENT_ID segment_id, API007DATA_ERROR_TYPE error_type)
{
    uint8 segment_index = GetSegmentIndex(segment_id);

    if(segment_index < API007APP_MAX_NUMBER_SEGMENTS)
    {
        API007BulkData_Rx_ErrorTypeRecived[segment_index] = error_type;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param segment_id
 * @return
 */
API007DATA_ERROR_TYPE API007BulkDataMgr__ReadErrorType(API007APP_SEGMENT_ID segment_id)
{
    uint8 segment_index = GetSegmentIndex(segment_id);

    if(segment_index < API007APP_MAX_NUMBER_SEGMENTS)
    {
        return API007BulkData_Rx_ErrorTypeRecived[segment_index];
    }
    else
    {
        return API007_ERROR_SEGMENT_NOT_FOUND;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param segment_id
 */
void API007BulkDataMgr__ClearErrorType(API007APP_SEGMENT_ID segment_id)
{
    API007BulkDataMgr__WriteErrorType(1, segment_id, API007_ERROR_NO_ERROR);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Received feedback from external node for each successful segment_id (index) data write to the client side.
 * @param node
 */
void API007BulkDataMgr__ReceivedWriteCompleted(uint8 node)
{
   uint16 remaining;
   remaining = 0;

    if (API007BulkData_Tx_State == API007BULKDATA_TX_STATE_WAIT)
    {
        if (API007BulkData_Tx_Segment_Index != API007DATA_INVALID_SEGMENT_INDEX)
        {
            //Calculate the remaining segment_id ID data is to be send.
            remaining = API007BulkData_Info_List[API007BulkData_Tx_Segment_Index].size - API007BulkData_Tx_Index;
        }

        //Check for the remaining segment_id ID data is to be transfered to external node.
        if (remaining > 0)
        {
            API007BulkData_Tx_State = API007BULKDATA_TX_STATE_PREPARE;
        }
        else
        {
            //Send command End of transmission for to the external node.
            API007Data__SendTransmissionComplete(API007BulkData_Tx_Destination);
            API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
        }
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * brief Received End of Transmission command from external node segment_id (server) ID side.
 * @param node
 */
void API007BulkDataMgr__ReceivedTransmissionCompleted (uint8 node)
{
    API007App__ReceivedTransmissionCompleted(API007BulkData_Rx_Segment);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Writing and Reading handlers
 *
 */
void API007BulkDataMgr__Handler(void)
{
    uint8 * pt;
    uint16 size;
    uint16 remaining;

    ///// Transmission State Machine //////////////////////////////////////
    switch (API007BulkData_Tx_State)
    {
        case API007BULKDATA_TX_STATE_IDLE:
            break;

        case API007BULKDATA_TX_STATE_PREPARE:
            if (API007BulkData_Tx_Segment_Index != API007DATA_INVALID_SEGMENT_INDEX)
            {
                remaining = API007BulkData_Info_List[API007BulkData_Tx_Segment_Index].size - API007BulkData_Tx_Index;

				// Module self adjust amount of data send in a single message based on availability of Reveal
				if (remaining <= (DATA_TRANSFER_SEGMENT_SIZE))
				{
					if (API007App__PrepareSegment(API007BulkData_Tx_Segment, API007BulkData_Tx_Index, remaining))
					{
						API007BulkData_Tx_State = API007BULKDATA_TX_STATE_SEND;
					}
					else
					{
						API007BulkData_Tx_State = API007BULKDATA_TX_STATE_ERROR;
					}
				}
				else
				{
					if (API007App__PrepareSegment(API007BulkData_Tx_Segment, API007BulkData_Tx_Index, DATA_TRANSFER_SEGMENT_SIZE))
					{
						API007BulkData_Tx_State = API007BULKDATA_TX_STATE_SEND;
					}
					else
					{
						API007BulkData_Tx_State = API007BULKDATA_TX_STATE_ERROR;
					}
				}
            }
            else
            {
				API007BulkData_Tx_State = API007BULKDATA_TX_STATE_ERROR;
            }

            API007BulkData_Generic_Timeout = MESSAGE_GENERIC_TIMEOUT_COUNTS;

            break;

        case API007BULKDATA_TX_STATE_SEND:

            pt = API007App__IsDataReady(API007BulkData_Tx_Segment, API007BulkData_Tx_Index);

            if (pt != NULL)
            {
                remaining = API007BulkData_Info_List[API007BulkData_Tx_Segment_Index].size - API007BulkData_Tx_Index;
                if (remaining <= (DATA_TRANSFER_SEGMENT_SIZE))
                {
                    size = remaining;
                }
                else
                {
                    size = (DATA_TRANSFER_SEGMENT_SIZE);
                }

                //Publish segment_id data to the external node for requested segment_id ID.
                if (API007Data__PublishSegmentData(API007BulkData_Tx_Destination, API007BulkData_Tx_Segment, API007BulkData_Tx_Index, pt, size) == TRUE)
                {
                    API007BulkData_Tx_Index += size;
                    API007BulkData_Tx_State = API007BULKDATA_TX_STATE_WAIT;
                    API007BulkData_Message_Response_Timer = MESSAGE_RESPONSE_TIMER_COUNTS;
                }
                else
                {
                    API007BulkData_Generic_Timeout--;

                    if (API007BulkData_Generic_Timeout == 0)
                    {
                        API007BulkData_Tx_State = API007BULKDATA_TX_STATE_ERROR;
                    }
                }
            }
            else
            {
                API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
                API007Data__SendError(API007BulkData_Tx_Destination, API007BulkData_Tx_Segment, API007_ERROR_SEGMENT_DATA_CORRUPTED);
            }
            break;

        case API007BULKDATA_TX_STATE_WAIT:

            API007BulkData_Message_Response_Timer--;

            if (API007BulkData_Message_Response_Timer == 0)
            {
                //Publish error if No response from the external node for successful reception of data.
                API007Data__SendError(API007BulkData_Tx_Destination, API007BulkData_Tx_Segment, API007_ERROR_NO_RESPONSE);
                API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
            }
            else if (API007BulkData_Info_List[API007BulkData_Tx_Segment_Index].size == API007BulkData_Tx_Index)
            {
            	API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
            }

            break;

        case API007BULKDATA_TX_STATE_ERROR:
            //Publish error to the external node if Segment ID is not found for this node.
            API007Data__SendError(API007BulkData_Tx_Destination, API007BulkData_Tx_Segment, API007_ERROR_SEGMENT_NOT_FOUND);
            API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
            break;

        default:
            API007BulkData_Tx_State = API007BULKDATA_TX_STATE_IDLE;
            break;
    }



    //// Receiving state machine //////////////////////////////////////////////
    switch (API007BulkData_Rx_State)
    {
        case API007BULKDATA_RX_STATE_IDLE:
            break;

        case API007BULKDATA_RX_STATE_WRITE:
            if (API007App__WriteSegmentData(API007BulkData_Rx_Segment ,API007BulkData_Rx_Index ,API007BulkData_Rx_Buffer ,API007BulkData_Rx_Size) == TRUE)
            {
                API007BulkData_Write_Operation_Timeout = MESSAGE_WRITE_TIMEOUT_COUNTS;
                API007BulkData_Rx_State = API007BULKDATA_RX_STATE_WAIT;
            }
            else
            {
                API007Data__SendError(API007BulkData_Rx_Destination, API007BulkData_Rx_Segment, API007_ERROR_WRITING);
                API007BulkData_Rx_State = API007BULKDATA_RX_STATE_IDLE;
            }

            break;

        case API007BULKDATA_RX_STATE_WAIT:
            API007BulkData_Write_Operation_Timeout--;

            if (API007BulkData_Write_Operation_Timeout == 0)
            {
                API007Data__SendError(API007BulkData_Rx_Destination, API007BulkData_Rx_Segment, API007_ERROR_WRITING);
                API007BulkData_Rx_State = API007BULKDATA_RX_STATE_IDLE;

            }
            else if (API007App__IsWriteEngineFree(API007BulkData_Rx_Segment) == TRUE)
            {
                API007Data__WriteRequestCompleted(API007BulkData_Rx_Destination);
                API007BulkData_Rx_State = API007BULKDATA_RX_STATE_IDLE;
            }

            break;

        default:
        	API007BulkData_Rx_State = API007BULKDATA_RX_STATE_IDLE;
            break;
    }
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method check the received segment_id ID with the list of segment_id ID's present with this node
 * @return segment_id ID if segment_id ID is present for this node
 * @return Invalid ID if segment_id ID is not present for this node
 */
static uint8 GetSegmentIndex(API007APP_SEGMENT_ID segment_id)
{
    uint8 idx;

    for (idx =0; idx < API007APP_MAX_NUMBER_SEGMENTS; idx++)
    {
        if (API007BULKDATA_SEGMENT_ID[idx] == segment_id)
        {
            return(idx);
        }
    }
    return (API007DATA_INVALID_SEGMENT_INDEX);
}

#endif
