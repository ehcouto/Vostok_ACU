 /**
 *  @file       
 *
 *  @brief      API007App module implementation
 *
 *  @details    This module takes care of reading and writing mechanism to the memory for bulk data storage.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *
 *  @copyright  Copyright 2013-$Date: 2013/09/13 10:23:07EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "API007Data.h"
#include "API007App.h"
#include "Reveal.h"
#include "NVData.h"
#include "Crc16.h"
#include "Micro.h"
#include "Mode.h"
#include "String.h"
#include "SystemTimers.h"
#include "API007App_prv.h"
#include "Micro.h"
#include "DataFlash.h"

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef enum
{
	API007APP_MACHINE_REQ_INIT  = 0,
	API007APP_MACHINE_REQ_SETUP = 1,
	API007APP_MACHINE_REQ_ID0 = 2,
	API007APP_MACHINE_REQ_ID_MAX
} API007APP_MACHINE_REQUESTOR_TYPE;

//Add here segments Definition with segment ID's
#define API007APP_SEG0      0

#define TIMER_MACHINE_REQUESTOR_SETUP     		250

#define API007DATA_INVALID_SEGMENT_INDEX        255
#define MESSAGE_RESPONSE_TIMER_REQ              5000


//! list of module Log messages
typedef enum
{
     INVALID_CRC_FAILED = 1                          //!< data: INVALID_CRC_FAILED
} MODULE_API007APP_LOG_MESSAGE_ID_TYPE;

typedef enum
{
    API007APP_RX_STATE_IDLE = 0,
    API007APP_RX_STATE_WRITE = 1,
    API007APP_RX_STATE_WAIT = 2,
    API007APP_RX_STATE_COMPLETE = 3,
    API007APP_RX_STATE_ERROR = 4
} API007APP_RX_STATE_TYPE;

static API007_SEGMENT_INFO_TYPE Segment_Info;

static const uint8 Store_Buffer_Datas_Null[STORE_DATAS_ARRAY_MAX_SIZE] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#pragma location = "_appliance_segment"
static uint8 Store_Buffer_Datas[STORE_DATAS_ARRAY_MAX_SIZE_CRC];					// Seg 5 ACU and HMI.

#pragma location = "_appliance_segment"
static SYSTEMTIMER_NV_TIMER_TYPE NV_Message_States_Timer;

static API007APP_MACHINE_REQUESTOR_TYPE State_Machine_Requestor;

static uint16 Message_Response_Request;
static API007APP_RX_STATE_TYPE API007APP_Rx_State;
static uint16 API007APP_Rx_Size;
static uint8*  Pnt_Rx_Buffer;

#if(API007APP_SERVER_FEATURE == ENABLED)
/**
 * Application specific data
 * Define segment variables
 */
#define API007APP_SERVER_DATA_SEGMENT0_SIZE                 (API007APP_SERVER_DATA_SEGMENT0_MAX_SIZE)
#define API007APP_SERVER_DATA_SEGMENT0_CRC                  (API007APP_SERVER_DATA_SEGMENT0_MAX_SIZE + 2)

#pragma location = "_appliance_segment"
static uint8 API007App_ServerData_Segment0[API007APP_SERVER_DATA_SEGMENT0_CRC];
#endif //(API007APP_SERVER_FEATURE == ENABLED)

#if(API007APP_CLIENT_FEATURE == ENABLED)

#if (MONITORING_NVDATA_SEGMENTS == ENABLED)
extern uint32 API007_CLIENT_NV_DATA$$Base;
extern uint32 API007_CLIENT_NV_DATA$$Limit;

static uint8 * const  API007_CLIENT_NV_DATA_START =               (uint8 * )(&API007_CLIENT_NV_DATA$$Base);
static const uint32   API007_CLIENT_NV_DATA_LIMIT =               (uint32)(&API007_CLIENT_NV_DATA$$Limit);

extern uint32 API007_CLIENT_NV_DATA_DEFAULTS$$Base;
extern uint32 API007_CLIENT_NV_DATA_DEFAULTS$$Limit;
static uint8 * const  API007_CLIENT_NV_DATA_DEFAULTS_START =               (uint8 * )(&API007_CLIENT_NV_DATA_DEFAULTS$$Base);
static const uint32   API007_CLIENT_NV_DATA_DEFAULTS_LIMIT =               (uint32)(&API007_CLIENT_NV_DATA_DEFAULTS$$Limit);
#endif

#endif //(API007APP_CLIENT_FEATURE == ENABLED)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module API007App and its variables
 *
 */
void API007App__Initialize(void)
{
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
#endif
		{
			SystemTimers__NVResetTimer(&NV_Message_States_Timer);
		}
#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
		else
		{
			SystemTimers__NVRestore(&NV_Message_States_Timer);
		}
#endif
    }

    Message_Response_Request = MESSAGE_RESPONSE_TIMER_REQ;
    Pnt_Rx_Buffer = NULL;
    State_Machine_Requestor = API007APP_MACHINE_REQ_ID_MAX;
}

void API007App__StartGetData(void)
{
	(void)SystemTimers__NVSetTimerMS(&NV_Message_States_Timer, TIMER_MACHINE_REQUESTOR_SETUP, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
	State_Machine_Requestor = API007APP_MACHINE_REQ_INIT;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Writing and Reading handlers
 *
 */
void API007App__Handler (void)
{
    if (SystemTimers__NVCheckTimer(&NV_Message_States_Timer) == SYSTEMTIMERS_STATE_DONE)
    {
    	Message_Response_Request--;

    	if (State_Machine_Requestor == API007APP_MACHINE_REQ_INIT)
    	{
			if (Mode__GetMode() == MODE_NORMAL)
			{
				(void)API007__RequestSegmentsList (WIN_MCU_ADDRESS);
				(void)API007__RequestInfoSegment (WIN_MCU_ADDRESS, API007APP_SEG0);
				(void)SystemTimers__NVSetTimerMS(&NV_Message_States_Timer, TIMER_MACHINE_REQUESTOR_SETUP, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				State_Machine_Requestor = API007APP_MACHINE_REQ_SETUP;
			}
    	}
    	else if (State_Machine_Requestor >= API007APP_MACHINE_REQ_SETUP)
    	{
    		switch (State_Machine_Requestor)
    		{
				case API007APP_MACHINE_REQ_SETUP:
				{
	    			(void)API007__RequestGetDataSegment(WIN_MCU_ADDRESS, API007APP_SEG0);
	    			(void)SystemTimers__NVSetTimerMS(&NV_Message_States_Timer, TIMER_MACHINE_REQUESTOR_SETUP, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				}
				break;
				case API007APP_MACHINE_REQ_ID0:
				{
					State_Machine_Requestor = API007APP_MACHINE_REQ_ID_MAX;
				}
				break;

				case API007APP_MACHINE_REQ_ID_MAX:
				{
					Message_Response_Request = MESSAGE_RESPONSE_TIMER_REQ;
				}
				break;

				default:
					// Nothing to do
				break;
    		}
    	}
    }
    else if ((State_Machine_Requestor < API007APP_MACHINE_REQ_ID_MAX) && (Message_Response_Request == (uint16)0))
    {
    	State_Machine_Requestor = API007APP_MACHINE_REQ_INIT;
		Message_Response_Request = MESSAGE_RESPONSE_TIMER_REQ;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Exposes the information about a segment_id in this node.
 * @param segment_id   Segment ID for a single logical block of data.
 * @return pointer to the information about the requested segment_id.
 */
API007_SEGMENT_INFO_TYPE * API007App__GetSegmentInfo(API007APP_SEGMENT_ID segment_id)
{
	Segment_Info.crc = 0;

    switch (segment_id)
    {
        case API007APP_SEGMENT_0:
        {
        	Segment_Info.segment_role = API007_SEGMENT_ROLE_SERVER;
            Segment_Info.size = STORE_DATAS_ARRAY_MAX_SIZE;  	//last 2 byte are used for storing the Crc value
        }
        break;
        default:
        	// Nothing to do
        break;
    }

    return (&Segment_Info);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method prepares the segment_id ready to read segment_id data from the defined segment_id ID address
 *
 */
BOOL_TYPE API007App__PrepareSegment(API007APP_SEGMENT_ID segment_id,uint16 index,uint16 size)
{
    BOOL_TYPE ret = FALSE;

    switch (segment_id)
    {
        case API007APP_SEGMENT_0:
            //Set address pointer to read a memory.
            ret = TRUE;
            break;

        default:
        	// Nothing to do
            break;
    }

    return (ret);
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method write the segment_id data received from a server to the defined segment_id ID location.
 * @param buffer contains the Wide package that contains the message
 * Trasmission Packets.
 */
uint8* API007App__IsDataReady(API007APP_SEGMENT_ID segment, uint16 index)
{
    uint8* ret_pnt = NULL;

    switch (segment)
    {
        case API007APP_SEGMENT_0:
		{
			uint16 crc_calc, crc_stored;
			uint8* pnt_store_datas;
			uint16 crc_stored_partial1;
			uint8 crc_stored_partial2;

			// BackUp KO protection.
			pnt_store_datas = (&Store_Buffer_Datas[0]);
			crc_calc = Crc16__RangeProcess(pnt_store_datas, 0xFFFF, STORE_DATAS_ARRAY_MAX_SIZE);
			crc_stored_partial1 = (((uint16)Store_Buffer_Datas[STORE_DATAS_ARRAY_MAX_SIZE] << 8U) & (uint16)0xFF00);
			crc_stored_partial2 = (Store_Buffer_Datas[STORE_DATAS_ARRAY_MAX_SIZE + 1U] & (uint8)0xFF);
			crc_stored = ( crc_stored_partial1 | crc_stored_partial2);

			if (crc_stored != crc_calc)
			{
				ret_pnt = (uint8*)(&Store_Buffer_Datas_Null[0]);
			}
			else
			{
				ret_pnt = &Store_Buffer_Datas[0];
			}
		}
        break;

        default:
            ret_pnt = (NULL);
        break;
    }

    return (ret_pnt);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method writes received segment_id ID data from external node to Received Buffer memory.
 * @param segment_id   Segment ID where the data will be stored
 * @param index     Index within the segment_id where the received data is related to.
 * @param data      Pointer to the data source.
 * @param size      Size of the data source.
 *
 * @return TRUE - If accepted to write the data
 *         FALSE - If rejected to write the data
 *
 */
BOOL_TYPE API007App__WriteSegmentData(API007APP_SEGMENT_ID segment_id ,uint16 index ,uint8 * data ,uint8 size)
{
BOOL_TYPE ret_value = FALSE;

    if (API007APP_Rx_State == API007APP_RX_STATE_IDLE)
    {
        // API007APP_Rx_Segment = segment_id;
        // API007APP_Rx_Segment_Index = GetSegmentIndex(segment_id);
        if ((API007APP_SEGMENT_ID)segment_id != (API007APP_SEGMENT_ID)API007DATA_INVALID_SEGMENT_INDEX)
        {
			API007APP_Rx_Size = size;
			API007APP_Rx_State = API007APP_RX_STATE_IDLE;

			Pnt_Rx_Buffer = data;

			API007Data__WriteRequestCompleted(APPI007APP_SERVER_NODE_ID);
			ret_value = TRUE;
        }
        else
        {
            // Publish error that segment ID not found for this node
            API007Data__SendError(APPI007APP_SERVER_NODE_ID, segment_id, API007_ERROR_SEGMENT_NOT_FOUND);
        }
    }
    else
    {   // Publish an error busy writing
        API007Data__SendError(APPI007APP_SERVER_NODE_ID, segment_id, API007_ERROR_BUSY_WRITING);
    }

    return ret_value;
}



//----------------------------------------------------------------------------------------------------------
/**
 * brief Received End of Transmission command from external node segment_id (server) ID side.
 * @param node
 */
void API007App__ReceivedTransmissionCompleted (API007APP_SEGMENT_ID segment_id)
{
    switch (segment_id)
    {
        case API007APP_SEGMENT_0:
#if(API007APP_SERVER_FEATURE == ENABLED)
            //Calculate CRC for the received data  using FastCRCUpd ()and fill in the result under the CRC of segment_id info.
            crc_local = Crc16__RangeProcess((uint8 *)API007App_ServerData_Segment0, 0xFFFF, (sizeof(API007App_ServerData_Segment0) - 2));
            API007App_ServerData_Segment0[sizeof(API007App_ServerData_Segment0)-2] = (crc_local & 0xFF00)>>8;
            API007App_ServerData_Segment0[sizeof(API007App_ServerData_Segment0)-1] = crc_local & 0x00FF;
#endif //(API007APP_SERVER_FEATURE == ENABLED)
            break;
        default:
        	// Nothing to do
            break;
    }
 }

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method manages the status of the writing mechanism
 *        The writing scheme could be writing on the External EEPROM or Internal Flash memory or RAM memory
 *
 * @param segment_id
 * @return
 */
BOOL_TYPE API007App__IsWriteEngineFree(API007APP_SEGMENT_ID segment_id)
{
   BOOL_TYPE ret_value = FALSE;

   switch (segment_id)
	{                  					//Copy/Write the received segment ID data from Rx_Buffer to defined memory location.
		case API007APP_SEGMENT_0:
		{
		   uint16 crc_local;

		   if (Pnt_Rx_Buffer != NULL)
		   {
			   if (API007APP_Rx_Size <= STORE_DATAS_ARRAY_MAX_SIZE)
			   {
				   memcpy (Store_Buffer_Datas, Pnt_Rx_Buffer, API007APP_Rx_Size);
			   }
			   else
			   {
				   memcpy (Store_Buffer_Datas, Pnt_Rx_Buffer, STORE_DATAS_ARRAY_MAX_SIZE);
			   }
		   }

		   crc_local = Crc16__RangeProcess(Store_Buffer_Datas, 0xFFFF, STORE_DATAS_ARRAY_MAX_SIZE);

		   Store_Buffer_Datas[STORE_DATAS_ARRAY_MAX_SIZE_CRC-2U] = (crc_local & (uint16)0xFF00)>>8;
		   Store_Buffer_Datas[STORE_DATAS_ARRAY_MAX_SIZE_CRC-1U] = crc_local & (uint16)0x00FF;

		   ret_value = TRUE;
		}
		break;

		default:
		   State_Machine_Requestor = API007APP_MACHINE_REQ_ID_MAX;
		   ret_value = TRUE;
		break;
	}

    return ret_value;
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param segment_id
 * @param role
 * @param size
 * @param crc
 */
void API007App__SetSegmentInfo(uint8 segment_id, API007_SEGMENT_ROLE_TYPE role, uint16 size, uint16 crc)
{
#if (API007APP_CLIENT_FEATURE == ENABLED)
    Segment_Info.crc = crc;
    Segment_Info.segment_role = role;
    Segment_Info.size = size;
#endif //(API007APP_CLIENT_FEATURE == ENABLED)
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param node
 */
void API007App__ServerwriteCompleted(uint8 node)
{
    #if(API007APP_CLIENT_FEATURE == ENABLED)
    API007Data__SendTransmissionComplete(APPI007APP_SERVER_NODE_ID);
    #endif//(API007APP_CLIENT_FEATURE == ENABLED)
}

#endif


