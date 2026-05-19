/**
 *  @file
 *
 *  @brief      API007Data Module implementation
 *
 *  @details    This module implements the code to handle the API007Data GDM for bulk data transfer using REVEAL
 *   			and NVData management using API007NVData module
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2012-2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "API007Data.h"

#include "Utilities.h"
#include "String.h"
#include "Mode.h"
#include "SFUpdater.h"
#include "Flash.h"
#include "Crc16.h"

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
    #include "API007BulkData.h"
#endif

#include "API007NVDataDefs.h"

#ifndef API007DATA_USE_SRTASKMONITOR_SUSPEND
	#define	API007DATA_USE_SRTASKMONITOR_SUSPEND		ENABLED
#endif

#if (API007DATA_USE_SRTASKMONITOR_SUSPEND == ENABLED)
	#include "SRTaskMonitor.h"
	#define		SUSPEND_TASK_MONITORING()		SRTaskMonitor__Suspend()
	#define		RESUME_TASK_MONITORING()		SRTaskMonitor__Resume()
#else
	#define		SUSPEND_TASK_MONITORING()
	#define		RESUME_TASK_MONITORING()
#endif

/**
 * API007_OP_SEGMENT_LIST \n
 * cmd:  payload:  no payload
 * fdb:  payload contains a list of segment ID
 *
 * API007_OP_SEGMENT_INFO \n
 * cmd:  payload: byte[0] = segment id\n
 *
 */
typedef enum
{
    API007_OP_MEM_WRITE_RAM_24BIT = 1,                   //Opcode 1-4 supports API007 backward compatibility
    API007_OP_MEM_WRITE_RAM_32BIT = 3,                   //RAM memory variables for debugging

    API007_OP_SEGMENT_LIST = 5,
    API007_OP_SEGMENT_INFO = 6,
    API007_OP_SEND_SEGMENT_DATA = 7,
    API007_OP_GET_SEGMENT_ALL_DATA = 8,
    API007_OP_CLIENT_WRITE_COMPLETE = 9,
    API007_OP_SERVER_TRANSMISSION_COMPLETE = 10,

    API007_OP_REQ_PUB_SETTING_FILE_INFO = 11,
    API007_OP_REQ_PUB_SETTING_FILE_DATA = 12,

    API007_OP_ERASE_FLASH = 15,
    API007_OP_WRITE_FLASH = 16,
    API007_OP_FLASH_PROCESS_DONE = 17,
    API007_OP_REQ_PUB_FLASH_ALIGNMENT = 18,

    API007_OP_REQ_PUB_SETTING_FILE_ID_INFO = 19,
    API007_OP_REQ_PUB_SETTING_FILE_ID_DATA = 20,

    API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO = 21,
    API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA = 22,

    API007_OP_REQ_CLAIM_NVDATA = 23,
    API007_OP_REQ_READ_NVDATA = 24,
    API007_OP_REQ_WRITE_NVDATA = 25,



    API007_OP_DEBUG = 30,
    API007_OP_ERROR = 31
} API007DATA_OP_TYPE;

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define  API007DATA_MAX_PAYLOAD     REVEAL_PAYLOAD_SIZE

#define API007DATA_SF_UI_TABLE_SIZE()                       ((0x60 ) * 2) // this number should be replaced by the address just before the address pointed by the pointer 0xA

#ifndef API007DATA_WRITE_RAMDATA
    #define API007DATA_WRITE_RAMDATA       DISABLED
#endif

#ifndef API007DATA_USE_PRIORITY_QUEUE
    #define API007DATA_USE_PRIORITY_QUEUE   DISABLED
#endif

#define API007DATA_INVALID_SEGMENT_INDEX       255

#ifndef REVEAL_MMP_BIT
	#define REVEAL_MMP_BIT          ((uint8)0x80)        //reveal more message pending
#endif

#ifndef API007DATA_SETTINGFILE_INFO_TIMEOUT_MS
    #define API007DATA_SETTINGFILE_INFO_TIMEOUT_MS      500
#endif

#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
	// This variable holds the information weather the setting file information is received or not
	// API007 uses the High Priority queue. After the Hot Plug the API007_OP_REQ_PUB_SETTING_FILE_INFO message is prepared first in the Reveal Priority Queue.
	// As ACU did not listed in the Active Node list the message is dropped and the Setting File information is not communicated
	// This varable will check weather the Setting File Information is been communicated to the board or not.
	// If the Setting file information is missed during the power up then the API007Data__Handler() will initiate the request once the ACU is alive
	static BOOL_TYPE SettingFile_Info_Received;
	static uint16 SettingFile_Info_Timer;
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE PublishSettingFileInfo(uint8 address);
static BOOL_TYPE PublishUISettingFileData(uint8 address, uint32 index, uint8 size);
static BOOL_TYPE PublishSettingFileIDInfo(uint8 address, SF_ID_TYPE sf_id);
static BOOL_TYPE PublishSettingFileIDData(uint8 address, SF_ID_TYPE sf_id, uint16 data_index, uint8 size);
static BOOL_TYPE PublishSettingFileDisplacementInfo(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement);
static BOOL_TYPE PublishSettingFileDisplacementData(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 data_index, uint8 size);

#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
	static void RequestSFInfo(void);
#endif

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
static void PublishSegmentInfo(uint8 address ,API007APP_SEGMENT_ID segment_id);
static void PublishSegmentList(uint8 address );
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module API007Data and its variables
 *
 */
void API007Data__Initialize(void)
{
	#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
		SettingFile_Info_Received = FALSE;
	#endif

    Flash__Initialize();

	#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
    	RequestSFInfo();
    #else
        PublishSettingFileInfo(REVEAL_BROADCAST_NODE);

        // Broadcast the setting file ID
        SF_ID_TYPE section_ids[SF_MAX_SECTIONS];
        uint8 section_count = SettingFile__GetSectionIDs(section_ids);
        for (uint8 section = 0; section < section_count; section++)
        {
            SF_ID_TYPE sf_id = section_ids[section];
            if (sf_id != SF_ID_END)
            {
                PublishSettingFileIDInfo(WIN_BROADCAST, sf_id);
            }
        }
    #endif

    #if(API007DATA_BULK_DATA_FEATURE == ENABLED)
        API007BulkDataMgr__Initialize();
    #endif

	#if (API007DATA_NVDATA_FEATURE == ENABLED)
		API007NVData__Initialize();
	#endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This API is called every 5ms from the communication thread
 *
 */
void API007Data__Handler(void)
{
    #if(API007DATA_BULK_DATA_FEATURE == ENABLED)
        API007BulkDataMgr__Handler();
    #endif

	#if (API007DATA_NVDATA_FEATURE == ENABLED)
        API007NVData__Handler();
	#endif

	#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
        if (SettingFile_Info_Received == FALSE)
        {
            //Check is the ACU is alive
            if(SettingFile_Info_Timer > 0)
            {
                SettingFile_Info_Timer--;
            }
            else if ((Reveal__GetNodestatus() & (1 << WIN_ACU_ADDRESS) ) != 0)
        	{
        		RequestSFInfo();
        	}
        }
	#endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses messages target to the API017 sent to a broadcast or exclusively to this node for Reveal 4.x.
 * @param buffer contains the Wide package that contains the message
 * @return REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API007Data__CommandParser(REVEAL_MSG_TYPE * buffer )
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    uint8 * data;
    uint32 address_start;
    uint32 address_end;

    retval = NVO_RS_READY;
    data = buffer->payload;

    // data is a pointer that points to the payload
    switch ((API007DATA_OP_TYPE) buffer->op_code)
    {
        #if(API007DATA_WRITE_RAMDATA_ONLY_REQUIRED == ENABLED)
            case API007_OP_MEM_WRITE_RAM_24BIT:
                //writes RAM data to 24bit RAM addressed memory received from the external node.
                memcpy((uint8*)Utilities__ConvertArrayTo24bits(data), data +3 , buffer->sdu_length - 5);
                break;

            case API007_OP_MEM_WRITE_RAM_32BIT:
                //writes RAM data to 32bit RAM addressed memory received from the external node.
                memcpy((uint8*)Utilities__ConvertArrayTo32bits(data), data +4 , buffer->sdu_length - 6);

                break;
        #endif  //API007DATA_WRITE_RAMDATA_ONLY_REQUIRED

        #if(API007DATA_BULK_DATA_FEATURE == ENABLED)
                case API007_OP_SEGMENT_LIST:
                    //Publish list of segment ID's.
                    PublishSegmentList(REVEAL_BROADCAST_NODE);
                    break;

                case API007_OP_SEGMENT_INFO:
                    //Publish segment detailed info for the requested segment id from this node.
                    PublishSegmentInfo(REVEAL_BROADCAST_NODE, (API007APP_SEGMENT_ID)data[0]);
                    break;

                case API007_OP_GET_SEGMENT_ALL_DATA:
                    //Publish segment data for requested segment ID node.
                    if (API007BulkDataMgr__GetSegmentData(buffer->node_id, (API007APP_SEGMENT_ID)data[0]) == FALSE)
                    {
                        retval = NVO_RS_BUSY;
                    }
                    break;

                case API007_OP_SERVER_TRANSMISSION_COMPLETE:
                    //Received End of Transmission command from external node segment (server) ID side.
                    API007BulkDataMgr__ReceivedTransmissionCompleted(buffer->node_id);
                    break;

                case API007_OP_DEBUG:
                    API007__RequestGetDataSegment(buffer->node_id,data[0]);
                    break;

                case API007_OP_SEND_SEGMENT_DATA:// receive data from the other side
                    // Write this Received data to the destination and then reply with write completed command message.
                    API007BulkDataMgr__WriteSegmentData(buffer->node_id, (API007APP_SEGMENT_ID)data[0],Utilities__Merg2BytesTo16bits(data[1],data[2]),&data[3],buffer->sdu_length-5);
                    break;

                case API007_OP_CLIENT_WRITE_COMPLETE:
                    //Received command message as "ACK" from the external node for each successful segment (index) data write to the client side.
                    API007BulkDataMgr__ReceivedWriteCompleted(buffer->node_id);
                    break;

        #endif //API007DATA_BULK_DATA_FEATURE

		#if (API007DATA_NVDATA_FEATURE == ENABLED)
			case API007_OP_REQ_CLAIM_NVDATA:
				//DATA[0-1] - Claim size
				API007NVData__ProcessClaimSize(buffer->node_id, Utilities__Merg2BytesTo16bits(data[0],data[1]));
				break;

			case API007_OP_REQ_READ_NVDATA:
    			//DATA[0-1]	- size
    			//DATA[2-3]	- index
        		API007NVData__ProcessReadCmd(buffer->node_id, Utilities__Merg2BytesTo16bits(data[0],data[1]), Utilities__Merg2BytesTo16bits(data[2],data[3]));
				break;

			case API007_OP_REQ_WRITE_NVDATA:
				//DATA[0-1] - index
				//DATA[2...] - Write Data
        		API007NVData__ProcessWriteCmd(buffer->node_id, Utilities__Merg2BytesTo16bits(data[0],data[1]), (uint16)(buffer->sdu_length-4), &data[2]);
				break;
		#endif //API007DATA_NVDATA_FEATURE

        case API007_OP_REQ_PUB_SETTING_FILE_INFO:
            PublishSettingFileInfo(buffer->node_id);
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_DATA:
        	if (PublishUISettingFileData(buffer->node_id,Utilities__ConvertArrayTo32bits(data), data[4]) == FALSE)
            {
				retval = NVO_RS_UNSUP_OUT_OF_MEMORY;
			}
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_ID_INFO:
            PublishSettingFileIDInfo(buffer->node_id, (SF_ID_TYPE)buffer->payload[0]);
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_ID_DATA:
            /*
             * Data[0]   - SF_ID
             * Data[1-2] - Data_Index
             * Data[3]   - SF_ID_Size
             */
            if (PublishSettingFileIDData(buffer->node_id, (SF_ID_TYPE)data[0], Utilities__ConvertArrayTo16bits(&data[1]), data[3]) == FALSE)
            {
            	retval = NVO_RS_UNSUP_OUT_OF_MEMORY;
            }
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO:
            /*
             * Data[0-1] - SF_POINTER_ID
             * Data[2-3] - SF_DISPLACEMENT
             */

            PublishSettingFileDisplacementInfo(buffer->node_id, (SETTINGFILE_OFFSET_TYPE)Utilities__ConvertArrayTo16bits(&data[0]), (SETTINGFILE_OFFSET_TYPE)Utilities__ConvertArrayTo16bits(&data[2]));
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA:
            /*
             * Data[0-1] - SF_POINTER_ID
             * Data[2-3] - SF_DISPLACEMENT
             * Data[4-5] - Data_Index
             * Data[6]   - Size
             */
            if (PublishSettingFileDisplacementData(buffer->node_id, (SETTINGFILE_OFFSET_TYPE)Utilities__ConvertArrayTo16bits(&data[0]), Utilities__ConvertArrayTo16bits(&data[2]), Utilities__ConvertArrayTo16bits(&data[4]), data[6]) == FALSE)
            {
            	retval = NVO_RS_UNSUP_OUT_OF_MEMORY;
            }
            break;

        case API007_OP_ERASE_FLASH:
            if (Mode__IsProgrammingEngaged() == TRUE)
            {
                address_start = Utilities__ConvertArrayTo32bits(buffer->payload);
                address_end   = Utilities__ConvertArrayTo32bits(&buffer->payload[4]);

                SUSPEND_TASK_MONITORING();
                if (Flash__Format(address_start,address_end) == FLASH_RESPONSE_DONE)
                {
                    Reveal__QueueMessageU8(API007DATA_NUM,API007_OP_FLASH_PROCESS_DONE,buffer->node_id,FALSE,FALSE,API007_OP_ERASE_FLASH);
                }
                else
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                RESUME_TASK_MONITORING();
            }
            else
            {
                retval = NVO_RS_REJECTED;
            }
            break;
        case API007_OP_WRITE_FLASH:
            if (Mode__IsProgrammingEngaged() == TRUE)
            {
                if (Flash__Write(Utilities__ConvertArrayTo32bits(data),&data[4],(uint32)buffer->size-4) == FLASH_RESPONSE_DONE)
                {
                    Reveal__QueueMessageU8(API007DATA_NUM,API007_OP_FLASH_PROCESS_DONE,buffer->node_id,FALSE,FALSE,API007_OP_WRITE_FLASH);
                }
                else
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
            }
            else
            {
                retval = NVO_RS_REJECTED;
            }
            break;
        case API007_OP_REQ_PUB_FLASH_ALIGNMENT:
            Reveal__QueueMessageU8(API007DATA_NUM,API007_OP_REQ_PUB_FLASH_ALIGNMENT,buffer->node_id,FALSE,FALSE,FLASH_ALIGNMENT);
            break;

        default:
            retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (retval);
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses feedbacks to the API007Data sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message
 */
void API007Data__FeedbackParser(REVEAL_MSG_TYPE * buffer)
{
    SF_ID_TYPE sf_id;
    #if (API007DATA_BULK_DATA_FEATURE == ENABLED) || (API007DATA_NVDATA_FEATURE == ENABLED)
    uint8 * data;
    data = buffer->payload;
    #endif

    switch (buffer->op_code)
    {
        #if (API007DATA_BULK_DATA_FEATURE == ENABLED)
            case API007_OP_SEGMENT_LIST:
                //Write received list of segment ID's for this node.
                API007BulkDataMgr__WriteSegmentList(buffer->node_id,&buffer->payload[0],buffer->sdu_length);
                break;

            case API007_OP_SEGMENT_INFO:
                //DATA[0]               - API007APP_SEGMENT_ID
                //DATA[1] - DATA[2]     - Segment Size
                //DATA[3]               - API007_SEGMENT_ROLE_TYPE
                //DATA[4] - DATA[5]     - CRC

                //Write received segment detailed info for this node.
                API007BulkDataMgr__WriteDetailInfo(buffer->node_id, (API007APP_SEGMENT_ID)data[0], (API007_SEGMENT_ROLE_TYPE)data[3], Utilities__ConvertArrayTo16bits(&data[1]), Utilities__ConvertArrayTo16bits(&data[4]));
                break;

            case API007_OP_ERROR:
                //Write received Error type for this node.
                API007BulkDataMgr__WriteErrorType(buffer->node_id, (API007APP_SEGMENT_ID)data[0], (API007DATA_ERROR_TYPE)data[1]);
                break;

            case API007_OP_CLIENT_WRITE_COMPLETE:
                //Write received Error type for this node.
                API007BulkDataMgr__AckWriteComplete(buffer->node_id);
                break;
        #endif //API007DATA_BULK_DATA_FEATURE

		#if (API007DATA_NVDATA_FEATURE == ENABLED)
            case API007_OP_REQ_CLAIM_NVDATA:
            	//DATA[0-1] - Claim size
				API007NVData__ReceivedClaimSize(Utilities__Merg2BytesTo16bits(data[0],data[1]));
                break;

			case API007_OP_REQ_READ_NVDATA:
				//DATA[0-1]	- index
			    //DATA[2..] - Data
				API007NVData__ReceivedRead(Utilities__Merg2BytesTo16bits(data[0],data[1]),(uint16)(buffer->sdu_length-4), &data[2]);
				break;

			case API007_OP_REQ_WRITE_NVDATA:
				//DATA[0-1]	- index
				API007NVData__ReceivedWrite(Utilities__Merg2BytesTo16bits(data[0],data[1]));
				break;
		#endif //API007DATA_NVDATA_FEATURE

        case API007_OP_REQ_PUB_SETTING_FILE_INFO:
                sf_id = SF_ID_END;

                // uint16 Section CRC
                // uint32 Section Size
                if (buffer->node_id == WIN_UI_ADDRESS)
                {
                    sf_id = SF_ID_HMI;
                }
                else if (buffer->node_id == WIN_ACU_ADDRESS)
                {
                    sf_id = SF_ID_ACU;
                }
                else if (buffer->node_id == WIN_MCU_ADDRESS)
                {
                    sf_id = SF_ID_MCU;
                }

                if (sf_id != SF_ID_END)
                {
                    SFUpdater__RxSFInfo(buffer->node_id, sf_id, Utilities__ConvertArrayTo16bits(buffer->payload), (uint16)Utilities__ConvertArrayTo32bits(&buffer->payload[2]));
                }
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_DATA:
            SFUpdater__RxSFData(buffer->node_id,buffer->size-4, (uint16)Utilities__ConvertArrayTo32bits(buffer->payload), &buffer->payload[4]);
            break;


        case API007_OP_REQ_PUB_SETTING_FILE_ID_INFO:
            /*
             * Data[0]   - SF_ID
             * Data[1-2] - SF_ID_Crc
             * Data[3-4] - SF_ID_Size
             */
			#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
        		SettingFile_Info_Received = TRUE;
			#endif

            SFUpdater__RxSFInfo(buffer->node_id, (SF_ID_TYPE)buffer->payload[0], Utilities__ConvertArrayTo16bits(&buffer->payload[1]), Utilities__ConvertArrayTo16bits(&buffer->payload[3]));
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_ID_DATA:
            /*
             * Data[0]   - SF_ID
             * Data[1-2] - Data_Index
             * Data[3]   - Data[Data_Index]
             */
            SFUpdater__RxSFData(buffer->node_id, buffer->size-3, Utilities__ConvertArrayTo16bits(&buffer->payload[1]), &buffer->payload[3]);
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO:
            /*
             * Data[0-1] - SF_POINTER_ID
             * Data[2-3] - SF_Displacement
             * Data[4-5] - Displacement CRC
             * Data[6-7] - Displacement Size
             */

        	#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
				#if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        			SettingFile_Info_Received = TRUE;
				#endif
			#endif

            SFUpdater__RxSFDisplacementInfo((SETTINGFILE_OFFSET_TYPE)Utilities__ConvertArrayTo16bits(&buffer->payload[0]), Utilities__ConvertArrayTo16bits(&buffer->payload[2]), Utilities__ConvertArrayTo16bits(&buffer->payload[4]), Utilities__ConvertArrayTo16bits(&buffer->payload[6]));
            break;

        case API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA:
            /*
             * Data[0-1] - SF_POINTER_ID
             * Data[2-3] - SF_DISPLACEMENT
             * Data[4-5] - Data_Index
             * Data[6-Size]   - Data[0]
             */
            SFUpdater__RxSFDisplacementData((SETTINGFILE_OFFSET_TYPE)Utilities__ConvertArrayTo16bits(&buffer->payload[0]), Utilities__ConvertArrayTo16bits(&buffer->payload[2]), Utilities__ConvertArrayTo16bits(&buffer->payload[4]), buffer->size - 6, &buffer->payload[6]);
            break;

        default:
            break;
    }
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method handles call backs from Reveal when and ACK is received for this API
 * @param address
 * @param api
 * @param opcode
 * @param reasoncode
 */
void API007Data__AckParser(uint8 address ,uint8 api ,uint8 opcode ,REVEAL_RECIPE_STATUS_TYPE reasoncode )
{
    switch (reasoncode)
    {
        case NVO_RS_READY:
            break;
        case NVO_RS_BUSY:
            break;
        case NVO_RS_REJECTED:
            break;
            //...
        default:
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param address
 * @return
 */
BOOL_TYPE API007Data__RequestSettingFileInfo(uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_INFO,address,1);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_INFO,address,1);
    #endif
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        if (address == WIN_UI_ADDRESS)
        {
            Reveal__AddDataU8(ptr, SF_ID_HMI);
        }
        else
        {
            Reveal__AddDataU8(ptr, SF_ID_MCU);
        }

        Reveal__QueueCommand(ptr);
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param section
 * @return
 */
BOOL_TYPE API007Data__RequestSettingFileIDInfo(uint8 address, SF_ID_TYPE sf_id)
{
    REVEAL_COMMAND_TYPE * ptr;
    BOOL_TYPE result;

    result = FALSE;

    if (sf_id != SF_ID_END)
    {
        #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
            ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_INFO,address,1);
        #else
            ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_INFO, address, 1);
        #endif

        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU8(ptr, sf_id);
            Reveal__QueueCommand(ptr);
            result = TRUE;
        }
    }

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param sf_id
 * @param index
 * @param size
 * @return
 */
BOOL_TYPE API007Data__RequestSettingFileIDData(uint8 address, SF_ID_TYPE sf_id, uint16 data_index, uint8 size)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_DATA,address,4);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_DATA,address,4);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, sf_id);
        Reveal__AddDataU16(ptr,data_index);
        Reveal__AddDataU8(ptr,size);
        Reveal__QueueCommand(ptr);
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param sf_id
 * @param sf_pointer_id
 * @return
 */
BOOL_TYPE API007Data__RequestSettingFileDisplacementInfo(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement)
{
    REVEAL_COMMAND_TYPE * ptr;
    BOOL_TYPE result;

    result = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO,address,4);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO, address, 4);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, sf_pointer_id);
        Reveal__AddDataU16(ptr, sf_displacement);

        Reveal__QueueCommand(ptr);
        result = TRUE;
    }

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param sf_pointer_id
 * @param displacement
 * @param index
 * @param size
 * @return
 */
BOOL_TYPE API007Data__RequestSettingFileDisplacementData(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 data_index, uint8 size)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA,address,7);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA,address,7);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, sf_pointer_id);
        Reveal__AddDataU16(ptr, sf_displacement);
        Reveal__AddDataU16(ptr,data_index);
        Reveal__AddDataU8(ptr,size);
        Reveal__QueueCommand(ptr);
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param address
 * @param index
 * @param size
 * @return
 */
BOOL_TYPE API007Data__RequestSettingData(uint8 address,uint32 data_index, uint8 size)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DATA,address,5);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DATA,address,5);
    #endif
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU32(ptr,data_index);
        Reveal__AddDataU8(ptr,size);
        Reveal__QueueCommand(ptr);
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}


#if (API007DATA_BULK_DATA_FEATURE == ENABLED)

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method send the request of list of segments to receiver node.
 * @param address receiver.
 * @return BOOL_TYPE ret_val that express the result of function.
 */
BOOL_TYPE API007__RequestSegmentsList(uint8 address)
{
    BOOL_TYPE ret_val = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        REVEAL_COMMAND_TYPE *ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_SEGMENT_LIST,address, 0);
    #else
        REVEAL_COMMAND_TYPE *ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_SEGMENT_LIST,address, 0);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        ret_val = (BOOL_TYPE)Reveal__QueueCommand(ptr);
    }

    return (ret_val);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method send the request of segments informations to receiver Node.
 * @param address receiver, segment identification of segment requested.
 * @return BOOL_TYPE ret_val that express the result of function.
 */
BOOL_TYPE API007__RequestInfoSegment(uint8 address, uint8 segment)
{
    BOOL_TYPE ret_val = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        REVEAL_COMMAND_TYPE * ptr  = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_SEGMENT_INFO,address, 1);
    #else
        REVEAL_COMMAND_TYPE * ptr  = Reveal__NewCommand(API007DATA_NUM, API007_OP_SEGMENT_INFO,address, 1);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        ret_val = (BOOL_TYPE)Reveal__AddDataU8(ptr, segment);
        ret_val &= (BOOL_TYPE)Reveal__QueueCommand(ptr);
    }

    return (ret_val);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method send the request of data to receiver node.
 * @param node receiver, segment identification of segment requested.
 * @return BOOL_TYPE ret_val that express the result of function.
 */
BOOL_TYPE API007__RequestGetDataSegment(uint8 address, uint8 segment)
{
    BOOL_TYPE ret_val = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        REVEAL_COMMAND_TYPE * ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_GET_SEGMENT_ALL_DATA,address, 1);
    #else
        REVEAL_COMMAND_TYPE * ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_GET_SEGMENT_ALL_DATA,address, 1);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        ret_val = (BOOL_TYPE)Reveal__AddDataU8(ptr, segment);
        ret_val &= (BOOL_TYPE)Reveal__QueueCommand(ptr);
    }

    return (ret_val);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Publish segment data to the external node for requested segment ID.
 * @param address    Node address who send data.
 * @param segment    Segment ID for a single logical block of data.
 * @param index      Index identify which part of the data belongs to segment.
 * @param data       Pointer to the data source.
 * @param size       Size of the data source.
 * @return
 */
BOOL_TYPE API007Data__PublishSegmentData(uint8 address,uint8 segment, uint16 index, uint8 * data, uint8 size)
{
    REVEAL_COMMAND_TYPE * ptr;
    BOOL_TYPE ret_value = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_SEND_SEGMENT_DATA, address, (size+3));
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_SEND_SEGMENT_DATA, address, (size+3));
    #endif

    if (ptr !=(REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, segment);
        Reveal__AddDataU16(ptr, index);
        Reveal__AddDataU8Buffer(ptr, data, size);

        Reveal__QueueCommand(ptr);

        ret_value = TRUE;
    }

    return ret_value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Send command End of transmission for to the external node.
 * @param address   Node address who send data
 */
void API007Data__SendTransmissionComplete(uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_SERVER_TRANSMISSION_COMPLETE,address,0);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_SERVER_TRANSMISSION_COMPLETE,address,0);
    #endif
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__QueueCommand(ptr);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Publish error type to the external node.
 * @param address         Node address who sends data
 * @param segment_id      Segment ID for a single logical block of data.
 * @param error
 */
void API007Data__SendError(uint8 address ,uint8 segment_id, API007DATA_ERROR_TYPE error)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_ERROR,address,2);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_ERROR,address,2);
    #endif
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, segment_id);
        Reveal__AddDataU8(ptr, error);
        Reveal__QueueCommand(ptr);
    }
}

/**
 *
 * @param address
 */
void API007Data__WriteRequestCompleted(uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_CLIENT_WRITE_COMPLETE,address,0);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_CLIENT_WRITE_COMPLETE,address,0);
    #endif
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__QueueCommand(ptr);
    }
}

#endif



#if (API007DATA_NVDATA_FEATURE == ENABLED)
//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Creates an reveal command for API007_OP_REQ_CLAIM_NVDATA opcode
 *
 * @details
 * DATA[0-1] - Claim Size Request
 *
 * @param size - Claim size
 */
void API007Data__NVDataCmdClaimSizeRequest(uint16 size)
{
    REVEAL_COMMAND_TYPE * ptr;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_CLAIM_NVDATA, API007NVDATA_SERVER_NODE_ID, 2);
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_CLAIM_NVDATA, API007NVDATA_SERVER_NODE_ID, 2);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, size);

        Reveal__QueueCommand(ptr);
    }
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Creates an reveal command for API007_OP_REQ_READ_NVDATA opcode
 *
 * @details
 * DATA[0-1] - Read Index
 * DATA[2-3] - Read Size
 *
 * @param index - Read from this .Index
 * @param size - Claim size
 */
void API007Data__NVDataCmdReadRequest(uint16 index, uint16 size)
{
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        REVEAL_COMMAND_TYPE * ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_READ_NVDATA, API007NVDATA_SERVER_NODE_ID, 4);
    #else
        REVEAL_COMMAND_TYPE * ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_READ_NVDATA, API007NVDATA_SERVER_NODE_ID, 4);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, index);
        Reveal__AddDataU16(ptr, size);

        Reveal__QueueCommand(ptr);
    }
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Creates an reveal command for API007_OP_REQ_WRITE_NVDATA opcode
 *
 * @details
 * DATA[0-1] - Write Index
 * DATA[2..] - Write Data
 *
 * @param index - Write Data from this Index
 * @param size - Write size
 * @param write_data_ptr  - buffer location from where data to be copied
 */
void API007Data__NVDataCmdWriteRequest(uint16 index, uint16 size, uint8 *write_data_ptr)
{
    REVEAL_COMMAND_TYPE * ptr = NULL;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewCommand(API007DATA_NUM, API007_OP_REQ_WRITE_NVDATA, API007NVDATA_SERVER_NODE_ID, (size+2));
    #else
        ptr = Reveal__NewCommand(API007DATA_NUM, API007_OP_REQ_WRITE_NVDATA, API007NVDATA_SERVER_NODE_ID, (size+2));
    #endif

    if (ptr !=(REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, index);
        Reveal__AddDataU8Buffer(ptr, write_data_ptr, size);

        Reveal__QueueCommand(ptr);
    }
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief Send feedback to the API007_OP_REQ_CLAIM_NVDATA opcode by server
 *
 * @details
 * DATA[0-1] - Claim Size -If this value 0 means the claim is denied
 *
 * @param client_node_id - client node
 * @param size - Claim size (If this value 0 means the claim is denied)
 */
void API007Data__NVDataFbkClaimSize(uint8 client_node_id, uint16 size)
{
    REVEAL_COMMAND_TYPE * ptr;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_CLAIM_NVDATA, client_node_id, 2);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_CLAIM_NVDATA, client_node_id, 2);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, size);  //if size 0 means not able to claim

        Reveal__QueueCommand(ptr);
    }
}



//----------------------------------------------------------------------------------------------------------
/**
 * @brief Send feedback to the API007_OP_REQ_READ_NVDATA opcode by server
 *
 * @details
 * DATA[0-1] - Read Index
 * DATA[2..] - Read Data
 *
 * @param client_node_id - client node
 * @param index - Read index
 * @param size - Claim size (If this value 0 means the claim is denied)
 * @param read_data_ptr - read location
 */
void API007Data__NVDataFbkReadData(uint8 client_node_id, uint16 index, uint16 size, uint8 * read_data_ptr)
{
    REVEAL_COMMAND_TYPE * ptr;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_READ_NVDATA, client_node_id, (size+2));
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_READ_NVDATA, client_node_id, (size+2));
    #endif

    if (ptr !=(REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, index);
        Reveal__AddDataU8Buffer(ptr, read_data_ptr, size);

        Reveal__QueueCommand(ptr);
    }
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief Send feedback to the API007_OP_REQ_WRITE_NVDATA opcode by server
 *
 * @details
 * DATA[0-1] - Write Index (send index which is currently pointing after the write cmd)
 *
 * @param client_node_id - client node
 * @param index - Write index (send index which is currently pointing after the write cmd)
 */
void API007Data__NVDataFbkWriteData(uint8 client_node_id, uint16 index)
{
    REVEAL_COMMAND_TYPE * ptr;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_WRITE_NVDATA, client_node_id, 2);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_WRITE_NVDATA, client_node_id, 2);
    #endif

    if (ptr !=(REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, index); //send index which is currently pointing after the write cmd

        Reveal__QueueCommand(ptr);
    }
}

#endif // end of (API007DATA_NVDATA_FEATURE == ENABLED)

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Publish list of segment ID's to the external node.
 * @param address   Node address who send data
 */
static void PublishSegmentList(uint8 address )
{
    uint8 index = 0;
    REVEAL_COMMAND_TYPE * ptr = NULL;
    uint8 size = 0;

    index = 0;

    while (index < sizeof(API007BulkDataMgr__GetSegmentListSize()))
    {
        if ((API007BulkDataMgr__GetSegmentListSize() - index) > API007DATA_MAX_PAYLOAD)
        {
            size = API007DATA_MAX_PAYLOAD;

        }
        else
        {
            size = API007BulkDataMgr__GetSegmentListSize() - index;
        }


        #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_SEGMENT_LIST,address, size);
        #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_SEGMENT_LIST, address, size);
        #endif
        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU8Buffer(ptr, (void *) &(API007BulkDataMgr__GetSegmentList()[index]), size);

            index += size;

            //Check is their required to send more then one messages
            if ((API007BulkDataMgr__GetSegmentListSize() - index) > 0)
            {
                //set the MMP(More message Pending) bit if their are more than one message needs to be send
                ptr->opcode |= REVEAL_MMP_BIT;
            }

            Reveal__QueueCommand(ptr);
        }
        else
        {
            break;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Publish segment ID detailed information to the external node for requested segment ID.
 * @param address        Node address who send data.
 * @param segment_id     Segment ID for a single logical block of data.
 */
static void PublishSegmentInfo(uint8 address, API007APP_SEGMENT_ID segment_id)
{
    REVEAL_COMMAND_TYPE * ptr;
    API007_SEGMENT_INFO_TYPE * info;

    info = API007BulkDataMgr__GetSegmentInfo(segment_id);

    if (info == NULL)
    {
        API007Data__SendError(address, segment_id,API007_ERROR_SEGMENT_NOT_FOUND);
    }
    else
    {
        #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
            ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_SEGMENT_INFO, address ,6);
        #else
            ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_SEGMENT_INFO, address, 6);
        #endif
        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU8(ptr, segment_id);
            Reveal__AddDataU16(ptr, info->size);
            Reveal__AddDataU8(ptr, info->segment_role);
            Reveal__AddDataU16(ptr, info->crc);
            Reveal__QueueCommand(ptr);
        }
    }
}

#endif // end of (API007DATA_BULK_DATA_FEATURE == ENABLED)


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Publish the Setting information (ACU Checksum, UI Checksum and Size)
 * @param address
 * @return
 */
static BOOL_TYPE PublishSettingFileInfo(uint8 address)
{
    BOOL_TYPE retval;
    REVEAL_COMMAND_TYPE * ptr;
    retval = FALSE;
    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_INFO, address,20);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_INFO, address,28);
    #endif
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, SettingFile__GetSettingFileCRC(SF_ID_ACU));
        Reveal__AddDataU32(ptr, (uint32)SettingFile__GetSettingFileStart(SF_ID_ACU));   //lint !e923 Suppress Info: cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
        Reveal__AddDataU32(ptr, SettingFile__GetSettingFileSize(SF_ID_ACU));
        Reveal__AddDataU16(ptr, SettingFile__GetSettingFileCRC(SF_ID_HMI));
        Reveal__AddDataU32(ptr, (uint32)SettingFile__GetSettingFileStart(SF_ID_HMI));   //lint !e923 Suppress Info: cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
        Reveal__AddDataU32(ptr, SettingFile__GetSettingFileSize(SF_ID_HMI));

        Reveal__AddDataU32(ptr, (uint32)SettingFile__GetMemorySectionStart());          //lint !e923 Suppress Info: cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
        Reveal__AddDataU32(ptr, SettingFile__GetMemorySectionSize());

        Reveal__QueueCommand(ptr);
        retval = TRUE;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param section
 * @return
 */
static BOOL_TYPE PublishSettingFileIDInfo(uint8 address, SF_ID_TYPE sf_id)
{
    BOOL_TYPE retval;
    REVEAL_COMMAND_TYPE * ptr;

    retval = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_INFO, address,5);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_INFO, address,5);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, sf_id);
        Reveal__AddDataU16(ptr, SettingFile__GetSettingFileCRC(sf_id));
        Reveal__AddDataU16(ptr, (uint16)SettingFile__GetSettingFileSize(sf_id));

        Reveal__QueueCommand(ptr);
        retval = TRUE;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param sf_id
 * @param index
 * @param size
 * @return
 */
static BOOL_TYPE PublishSettingFileIDData(uint8 address, SF_ID_TYPE sf_id, uint16 data_index, uint8 size)
{
    BOOL_TYPE retval = FALSE;

    void * sf_pointer = SettingFile__GetSettingFilePointerByIndex(sf_id, data_index);

    if (sf_pointer != NULL)
    {
        if (size > API007DATA_MAX_PAYLOAD - 5)
        {
            size = API007DATA_MAX_PAYLOAD - 5;
        }

        #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
            REVEAL_COMMAND_TYPE * ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_DATA, address, size+3);
        #else
            REVEAL_COMMAND_TYPE * ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_ID_DATA, address, size+3);
        #endif

        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU8(ptr, sf_id);
            Reveal__AddDataU16(ptr, data_index);
            Reveal__AddDataU8Buffer(ptr,(uint8 *)sf_pointer, size);
            Reveal__QueueCommand(ptr);
            retval = TRUE;
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param address
 * @param sf_id
 * @param sf_pointer_id
 * @return
 */
static BOOL_TYPE PublishSettingFileDisplacementInfo(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement)
{
    BOOL_TYPE retval;
    REVEAL_COMMAND_TYPE * ptr;
    SETTINGFILE_LOADER_TYPE pointer_data_out;

    retval = FALSE;

    #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
        ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO, address,8);
    #else
        ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_INFO, address,8);
    #endif

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU16(ptr, sf_pointer_id);
        Reveal__AddDataU16(ptr, sf_displacement);

        if (SettingFile__BasicLoader(sf_pointer_id, sf_displacement, &pointer_data_out) == PASS)
        {
            Reveal__AddDataU16(ptr, Crc16__ConstRangeProcess(pointer_data_out.Data, 0xFFFF , pointer_data_out.Length));
            Reveal__AddDataU16(ptr, pointer_data_out.Length);
        }
        else
        {
            Reveal__AddDataU16(ptr, 0);
            Reveal__AddDataU16(ptr, 0);
        }

        Reveal__QueueCommand(ptr);
        retval = TRUE;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param address
 * @param sf_pointer_id
 * @param sf_displacement
 * @param index
 * @param size
 * @return
 */
static BOOL_TYPE PublishSettingFileDisplacementData(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 data_index, uint8 size)
{
    BOOL_TYPE retval = FALSE;
    SETTINGFILE_LOADER_TYPE pointer_data_out;

    if (SettingFile__BasicLoader(sf_pointer_id, sf_displacement, &pointer_data_out) == PASS)
    {
        if (size > API007DATA_MAX_PAYLOAD - 6)
        {
            size = API007DATA_MAX_PAYLOAD - 6;
        }

        #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
            REVEAL_COMMAND_TYPE * ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA, address, size + 6);
        #else
            REVEAL_COMMAND_TYPE * ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DISPLACEMENT_DATA, address, size + 6);
        #endif

        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU16(ptr, sf_pointer_id);
            Reveal__AddDataU16(ptr, sf_displacement);
            Reveal__AddDataU16(ptr, data_index);

            Reveal__AddDataU8Buffer(ptr, &pointer_data_out.Data[data_index],size);
            Reveal__QueueCommand(ptr);
            retval = TRUE;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param address
 * @param index
 * @param size
 * @return
 */
static BOOL_TYPE PublishUISettingFileData(uint8 address,uint32 index, uint8 size)
{
   BOOL_TYPE retval = FALSE;

   void * sf_pointer = SettingFile__GetSettingFilePointerByIndex(SF_ID_HMI,index);

   if (sf_pointer != NULL)
   {
       if (size > API007DATA_MAX_PAYLOAD - 4)
       {
           size = API007DATA_MAX_PAYLOAD - 4;
       }
       #if (API007DATA_USE_PRIORITY_QUEUE == ENABLED)
           REVEAL_COMMAND_TYPE * ptr = Reveal__PriorityNewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DATA, address,size+4);
       #else
           REVEAL_COMMAND_TYPE * ptr = Reveal__NewFeedback(API007DATA_NUM, API007_OP_REQ_PUB_SETTING_FILE_DATA, address,size+4);
       #endif

       if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
       {
           Reveal__AddDataU32(ptr, index);
           Reveal__AddDataU8Buffer(ptr, (uint8 *)sf_pointer, size);
           Reveal__QueueCommand(ptr);
           retval = TRUE;
       }
   }
   return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function Request for the Setting File information for the board which have SETTINGFILE_MASTER_FILE_FEATURE is DISABLED
 * @details
 *
 */
#if (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)
static void RequestSFInfo(void)
{
    #ifdef SFUPDATER_ACU_SECTION_ID
        API007Data__RequestSettingFileIDInfo(WIN_ACU_ADDRESS, SFUPDATER_ACU_SECTION_ID);
    #endif
    #ifdef SFUPDATER_HMI_SECTION_ID
        API007Data__RequestSettingFileIDInfo(WIN_ACU_ADDRESS, SFUPDATER_HMI_SECTION_ID);
    #endif
    #ifdef SFUPDATER_MCU_SECTION_ID
        API007Data__RequestSettingFileIDInfo(WIN_ACU_ADDRESS, SFUPDATER_MCU_SECTION_ID);
    #endif

    SFUpdater__ValidateFlashDisplacementData();
    SettingFile_Info_Timer = API007DATA_SETTINGFILE_INFO_TIMEOUT_MS / COMMUNICATION_THREAD_CALL_RATE_MS;
}
#endif // end of (SETTINGFILE_MASTER_FILE_FEATURE == DISABLED)


