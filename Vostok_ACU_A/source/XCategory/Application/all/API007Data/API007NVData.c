/**
 *  @file       
 *
 *  @brief      API007NVData module implementation
 *
 *  @details	This module is responsible for managing the NVData(Non-Volatile) by the all types of expansion and HMI boards
 *
 *
 *
 *
 *		Definitions 			Details
 *
 *		NVData				- 	Non Volatile Data, Data which can be retrieved even after power failure event
 *								Project team shall be very cautions defining which data supposed to be NVData and not
 *
 *		Server 				- 	Management of the Data storing and restoring in the Non Volatile Data Memory
 *      						Performing the NVData Write/Read requests from the client
 *      						Here ACU shall be the Server (As in Nucleus only ACU knows how to store and retrieve the NVData)
 *
 *      Client 				- 	Management of requesting the NVData from the Server or Sending the NVData to the server to store
 *
 *      Client ID			- 	Node ID itself is used as Client ID at the server side
 *
 *      _appliance_segment	-	This is the tag/segment name used to segregate the data in appliance segment
 *      						A data which holds the appliance information qualifies this tag
 *      						For example - Max Cool, Max ICE remaining time, Cooking Selection, Dryer option selection
 *
 *      						Example -
 *      						#pragma location = "_appliance_segment"
 *      						static uint8 User_MaxCool_Option;
 *
 *
 *      _statistics_segment	-	This is the tag/segment name used to segregate the data in statistics segment
 *      						A data which holds the statistics information qualifies this tag
 *      						For example - Number of Normal Washing Cycles run, Number of Days Refrigerator running
 *
 *      						Example -
 *      						#pragma location = "_statistics_segment"
 *      						static uint16 Washing_Cycle_Count;
 *
 *		_header_segment		-	Only used by the module to store the overall memory crc (this segment shall not be used by the user)
 *
 *		Client Block Data	- 	Client NVData memory is divided into blocks
 *								These each block has its own block crc
 *								data is continuously monitored to see the changes
 *								Only that block is sent whose block crc changes, this way we can optimize sending data to the server
 *
 *		Client Block Crc	- 	Crc locally used for each block to detect easily which block changed its data
 *
 *		CDT					- 	Client Description Table used by the server
 *								Refer 'CLIENT_DESCRIPT_TYPE' for details
 *
 *
 *
 *
 *
 *		Client Responsibility
 *		1. Client role only applies to non ACU boards only
 *		2. Client software shall only use the '_appliance_segment and '_statistics_segment' tag to define the NVData
 *		3. Node Id itself treated as client ID in this module
 *		4. Before proceeding Read/Write operations, its responsibility of Client to claim the NVData size from Server during boot-up
 *		5. If the Client fails to claim the NVData then it shall remain in the error state
 *		6. Client shall read the previously stored NVdata from server and compare with 'previously stored crc' If it matches then data is restored or else data is corrupted
 *		7. If the Server is non-responsive during boot-up after the API007NVDATA_CLIENT_MSG_RETRY_COUNT number of retries shall enter to the error state
 *		7. Its responsible for managing NVdata of more then one blocks, it should have a track of blocks index's
 *		8. Its responsible to write blocks only who's block's crc is changed
 *		9. Shall use Arbitrary method of finding which block data is changed so that every block gets equal chance.
 *		10.Block shall be sent to the client with minimum time delay to avoid communication overflow (API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS)
 *		11. During boot up it takes time to read and verify the NVData from Server Client Application shall consider this timing
 *          Recommended to wait until the status becomes other then API007NVDATA_RESTORE_IN_PROGRESS
 *
 *
 *
 *
 *
 *		Server Responsibility
 *		1. ACU is always be the Server
 *		2. Server features shall be enabled even if one client is present in the network
 *		3. Server shall support more then one client and distinguish them by node id as their client id (API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT)
 *		4. Enough memory shall be reserved for Server so that it can serve those claims from client
 *		5. Server not responsible for managing the Crc of the clients
 *		6. If the previously stored claim does not match with the new Claim then server shall allocate new claim if its able to allocate or else reject the request
 *		7. If the claim is failed then server shall notify to the client
 *		8. If any Read/Write request are made then Server shall satisfy those requests only and shall not be involved in managing the data index etc
 *
 *
 *
 *
	@startuml(Module_Diagram.png)

		object API007NVData
		object API007BulkData
		object API007App

		API007Data_NVData <|-- API007NVData

		API007Data_App <|-- API007BulkData
		API007BulkData <|-- API007App

	@enduml






 	@startuml(API007NVData.png)
	 hide footbox
     autonumber

	participant API007NVData_Client
	participant API007Data_Client
	participant API007Data_Server
	participant API007NVData_Server


							==  Info Update ==
	API007NVData_Client -> API007Data_Client : API007Data__NVDataCmdClaimSizeRequest(uint16 size)

	API007Data_Client -> API007Data_Server : CMD, OPCODE - API007_OP_REQ_CLAIM_NVDATA(0x13),\nDATA[0-1] - size

	API007Data_Server -> API007NVData_Server : API007NVData__ProcessClaimSize(uint8 client_node_id, uint16 size)
	API007NVData_Server ->API007Data_Server : API007Data__NVDataFbkClaimSize(uint8 client_node_id, uint16 size)
	API007Data_Server --> API007Data_Client : FBK, OPCODE - API007_OP_REQ_CLAIM_NVDATA(0x13),\nDATA[0-1] - size
	API007Data_Client -> API007NVData_Client : API007NVData__ReceivedClaimSize(uint16 size)


							==  Read ==
	API007NVData_Client -> API007Data_Client : API007Data__NVDataCmdReadRequest(uint16 index, uint16 size)

	API007Data_Client -> API007Data_Server : CMD, OPCODE - API007_OP_REQ_READ_NVDATA(0x14),\nDATA[0-1] - index\nDATA[2-3] - size

	API007Data_Server -> API007NVData_Server : API007NVData__ProcessReadCmd(uint8 client_node_id, uint16 index, uint16 size)
	API007NVData_Server ->API007Data_Server : API007Data__NVDataFbkReadData(uint8 client_node_id, uint16 index, uint16 size, uint8 *read_data_ptr)
	API007Data_Server --> API007Data_Client : FBK, OPCODE - API007_OP_REQ_READ_NVDATA(0x14),\nDATA[0-1] - index\nDATA[2...] - Read Data
	API007Data_Client -> API007NVData_Client : API007NVData__ReceivedRead(uint16 index ,uint16 size, uint8 * data_ptr)


							==  Write ==
	API007NVData_Client -> API007Data_Client : API007Data__NVDataCmdWriteRequest(uint16 index, uint16 size, uint8 *write_data_ptr)

	API007Data_Client -> API007Data_Server : CMD, OPCODE - API007_OP_REQ_WRITE_NVDATA(0x15),\nDATA[0-1] - index\nDATA[2...] - Write Data

	API007Data_Server -> API007NVData_Server : API007NVData__ProcessWriteCmd(uint8 client_node_id, uint16 index ,uint16 size, uint8 * data_ptr)
	API007NVData_Server ->API007Data_Server : API007Data__NVDataFbkWriteData(uint8 client_node_id, uint16 index)
	API007Data_Server --> API007Data_Client : FBK, OPCODE - API007_OP_REQ_WRITE_NVDATA(0x15),\nDATA[0-1] - index(currently pointing after the write cmd)
	API007Data_Client -> API007NVData_Client : API007NVData__ReceivedWrite(uint16 index)



	@enduml
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#include "API007NVData.h"

#if ((SYSTEM_ACU_HANDLING == ENABLED) && ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)))  //P1 board or ACU+EXP HMI
    #include "NVData.h"
#endif
#if (API007DATA_NVDATA_FEATURE == ENABLED)
#include "API007Data.h"

#include "Micro.h"
#include <string.h>

#include "NVData.h"
#include "Crc16.h"
#include "Log.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- IDs FOR LOG messages----------------------------------------------------------
//! A list of log message IDs that are generated by this module.
typedef enum MODULE_SETTINGFILE_LOG_MESSAGE_ID_STRUCT
{
    ERROR_INSUFFECIENT_API007NVDATA_CLIENT_NUM_BLOCKS,                      // make sure 'API007NVDATA_CLIENT_NUM_BLOCKS' has enough space
    ERROR_UNABLE_TO_ALLOCATE_CLAIMED_SIZE_IN_SERVER,                   		// make sure server has enough memory space to accommodate all clients claim for memory
    ERROR_SERVER_NOT_SENDING_FEEDBACK,				                   		// make sure server is configured

} MODULE_API007NVDATA_LOG_MESSAGE_ID_TYPE;

//! The name of this module for use by the Log module.
#define MODULE_NAME     MODULE_API007NVDATA

#define API007NVDATA_HANDLER_CALL_RATE_MS      5


#define INITIAL_CRC16_VALUE               		0xFFFF
#define EMPTY_NODE_ID							0xFF

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#if(API007NVDATA_SERVER_FEATURE == ENABLED)
	typedef struct
	{
		uint16 Start_Index;
		uint16 End_Index;		//Inclusive address
		uint8 Node_ID;
	}CLIENT_DESCRIPT_TYPE;

	/**
     * Application specific data
     * Define segment variables
     */
    #define API007NVDATA_SERVER_DATA_TRUE_SIZE    				(API007NVDATA_SERVER_DATA_SIZE + 1) //+1 as inclusive address always leaves index 0 free
    #define API007NVDATA_SERVER_DATA_TRUE_CRC					(API007NVDATA_SERVER_DATA_SIZE + 2)

	#pragma location = "_appliance_segment"
	static CLIENT_DESCRIPT_TYPE API007NVData_CDTable[API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT];

    #pragma location = "_appliance_segment"
    static uint8 API007NVData_ServerData[API007NVDATA_SERVER_DATA_TRUE_CRC];
#endif //(API007NVDATA_SERVER_FEATURE == ENABLED)



#if(API007NVDATA_CLIENT_FEATURE == ENABLED)
	//! Data is updated every API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS milli seconds at the server side by default
	//! Keeping too small this time would affect the communication bus traffic
	#ifndef API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS
		#define API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS           	(500/API007NVDATA_HANDLER_CALL_RATE_MS)
    #elif(API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS < 50)
        #error :"Minimum value for this macro shall be more then 50 API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS"
	#endif

    //! Defines number of loops that Client waits for data. After timeout elapsed Client restores default values.
    #define API007NVDATA_CLIENT_MSG_REPLY_TIMEOUT_MS 				(800/API007NVDATA_HANDLER_CALL_RATE_MS)        //During analysis it takes less then 800ms for the reply for 65 bytes of data during boot up

    //!
    #define API007NVDATA_CLIENT_MSG_RETRY_COUNT       				5

    //Here _header_segment is used to store the calculated crc of the _appliance_segment and _statistics_segment data only
    #ifdef __IAR_SYSTEMS_ICC__
        extern uint32 NVDATA_SECTION_HEADER$$Base;
        extern uint32 NVDATA_SECTION_STATISTICS$$Limit;
        static uint8 * const  API007NVDATA_DATA_SECTION_START =			(uint8 * )(&NVDATA_SECTION_HEADER$$Base);
        static const uint32   API007NVDATA_DATA_SECTION_LIMIT =			(uint32)(&NVDATA_SECTION_STATISTICS$$Limit);

        #define API007NVDATA_DATA_SECTION_SIZE							((uint16)(API007NVDATA_DATA_SECTION_LIMIT - (uint32)API007NVDATA_DATA_SECTION_START))
        #define COMPILE_TIME_CLIENT_NUM_BLOCKS                          ((API007NVDATA_DATA_SECTION_SIZE/DATA_TRANSFER_SEGMENT_SIZE) + 1)   //+1 to cover if any data lefts after the segment


        //Crc Section excluding the _header_segment
        extern uint32 NVDATA_SECTION_APPLIANCE$$Base;
        extern uint32 NVDATA_SECTION_STATISTICS$$Limit;
        static uint8 * const  API007NVDATA_CRC_SECTION_START =			(uint8 * )(&NVDATA_SECTION_APPLIANCE$$Base);
        static const uint32   API007NVDATA_CRC_SECTION_LIMIT =			(uint32)(&NVDATA_SECTION_STATISTICS$$Limit);

        #define API007NVDATA_CRC_SECTION_SIZE							((uint16)(API007NVDATA_CRC_SECTION_LIMIT - (uint32)API007NVDATA_CRC_SECTION_START))
    #else
        #ifdef UNITY
            extern uint8 * const  API007NVDATA_DATA_SECTION_START;
            extern const uint32   API007NVDATA_DATA_SECTION_LIMIT;

            #define API007NVDATA_DATA_SECTION_SIZE                          ((uint16)(API007NVDATA_DATA_SECTION_LIMIT - (uint32)API007NVDATA_DATA_SECTION_START))
            #define COMPILE_TIME_CLIENT_NUM_BLOCKS                          ((API007NVDATA_DATA_SECTION_SIZE/DATA_TRANSFER_SEGMENT_SIZE) + 1)   //+1 to cover if any data lefts after the segment

            //Crc Section excluding the _header_segment
            extern uint8 * const  API007NVDATA_CRC_SECTION_START;
            extern const uint32   API007NVDATA_CRC_SECTION_LIMIT;

            #define API007NVDATA_CRC_SECTION_SIZE                           ((uint16)(API007NVDATA_CRC_SECTION_LIMIT - (uint32)API007NVDATA_CRC_SECTION_START))
        #endif
    #endif

    //! Possible states of Client state machine.
    typedef enum
    {
    	REQUEST_NVDATA_INFO,
		CHECK_RECEIVED_NVDATA_INFO,

        REQUEST_READ_DATA,
        WAIT_FOR_DATA_RECEPTION,

        REQUEST_WRITE_DATA,
        WAITING_FOR_SERVER_COMPLETED_WRITING,


		ERROR_IRRICOVERABLE_STATE,
        NUM_OF_CLIENT_STATE_MACHINE_STATES
    }API007NVDATA_CLIENT_STATE;


    //! Possible feedback status received from Server
    typedef enum
    {
    	WAITING_FOR_FBK,
    	RECEIVED_NODE_INFO_FBK_PASS,
		RECEIVED_NODE_INFO_FBK_FAILED,
    	RECEIVED_READ_NVDATA_FBK_PASS,
		RECEIVED_READ_NVDATA_FBK_FAILED,
    	RECEIVED_WRITE_NVDATA_FBK_PASS,
    	RECEIVED_WRITE_NVDATA_FBK_FAILED,
    	RECEIVED_ERROR,
    }API007NVDATA_SERVER_FBK;

    //! State machine control variable
    static API007NVDATA_CLIENT_STATE API007NVData_Client_State;

    static API007NVDATA_SERVER_FBK API007NVData_Client_Server_Fbk;

    static API007NVDATA_DATA_RESTORE_STATUS_TYPE API007NVData_Client_Data_Restore_Status;

    static uint8 API007NVData_Client_Msg_Retry_Count;

    //! Counter used to count loops that Client wait for response from Server.
    static uint8 API007NVData_Client_Timeout_Count;

    //All Info/Read/Write related variable information
    static uint16 API007NVData_Client_Data_Index;

    //! Crc calculated on every segment to detect the local data changes
    static uint16 API007NVData_Client_Seg_Crc[API007NVDATA_CLIENT_NUM_BLOCKS];

    #ifdef __IAR_SYSTEMS_ICC__
        #pragma location = "_header_segment"
        static volatile uint16 API007NVData_Client_Stored_Crc;
    #else
        #ifdef UNITY
            extern NVDATA_SEGMENT_DATA NVDataClient_Segment_Data;

            #define API007NVData_Client_Stored_Crc  NVDataClient_Segment_Data.Header_Data[0]
        #endif
    #endif

#endif //(API007NVDATA_CLIENT_FEATURE == ENABLED)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if(API007NVDATA_SERVER_FEATURE == ENABLED)
    static CLIENT_DESCRIPT_TYPE *NVDataGetCDTByClientID(uint8 client_node_id);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module API007NVData and its variables
 *
 *  @details	Function is called from the API007Data module. Should not be called from the Application
 *
 */
void API007NVData__Initialize(void)
{
    #if(API007NVDATA_CLIENT_FEATURE == ENABLED)
		if (API007NVDATA_CLIENT_NUM_BLOCKS >= COMPILE_TIME_CLIENT_NUM_BLOCKS)
		{
	        API007NVData_Client_State 			= REQUEST_WRITE_DATA;
	        API007NVData_Client_Server_Fbk		= WAITING_FOR_FBK;

	        API007NVData_Client_Msg_Retry_Count = 0;
	        API007NVData_Client_Timeout_Count 	= 0;

	        API007NVData_Client_Data_Index		= 0;

	        if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	        {
		        for(uint8 index = 0; index < API007NVDATA_CLIENT_NUM_BLOCKS; index++)
		        {
		            API007NVData_Client_Seg_Crc[index] = INITIAL_CRC16_VALUE;
		        }

	            API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_IN_PROGRESS;

	            //case REQUEST_DATA: This starts with requesting the data
	            API007NVData_Client_State = REQUEST_NVDATA_INFO;
	        }
		}
		else
		{
			API007NVData_Client_State = ERROR_IRRICOVERABLE_STATE;
			LOG_ADD_ERROR(ERROR_INSUFFECIENT_API007NVDATA_CLIENT_NUM_BLOCKS, COMPILE_TIME_CLIENT_NUM_BLOCKS);	//data explains how many of them are required
		}
    #endif //(API007NVDATA_CLIENT_FEATURE == ENABLED)

	#if(API007NVDATA_SERVER_FEATURE == ENABLED)
        if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
        {
        	if(NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY)
        	{
        		//Initialize CDT if the NVData is corrupted
        		for (uint8 index = 0; index < API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT; index++)
        		{
        			API007NVData_CDTable[index].Node_ID 	= EMPTY_NODE_ID;
        			API007NVData_CDTable[index].Start_Index = 0;
        		}
        	}
        }
	#endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * 	@brief 		Writing and Reading handlers
 *
 *  @details	Function is called from the API007Data module. Should not be called from the Application
 *
 * 				Shall be called at 5ms
 * 				If the call rate is changed then should update the API007NVDATA_HANDLER_CALL_RATE_MS macro
 *
 */
void API007NVData__Handler(void)
{
    #if(API007NVDATA_CLIENT_FEATURE == ENABLED)
		uint16 crc_seg;
		uint16 crc_index = 0;
		uint16 block_size = 0;

		API007NVData_Client_Timeout_Count++;

        switch(API007NVData_Client_State)
        {
			case REQUEST_NVDATA_INFO:
				//Wait for the Server to participate in communication
				if ((Reveal__GetNodestatus() & (1 << API007NVDATA_SERVER_NODE_ID) ) != 0)
                {
					API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_IN_PROGRESS;
                    API007NVData_Client_Timeout_Count 		= 0;
                    API007NVData_Client_Server_Fbk			= WAITING_FOR_FBK;
					API007NVData_Client_State 				= CHECK_RECEIVED_NVDATA_INFO;

                    API007Data__NVDataCmdClaimSizeRequest(API007NVDATA_DATA_SECTION_SIZE);
                }
				break;

			case CHECK_RECEIVED_NVDATA_INFO:
				if (API007NVData_Client_Timeout_Count < API007NVDATA_CLIENT_MSG_REPLY_TIMEOUT_MS)
				{
					if (API007NVData_Client_Server_Fbk == RECEIVED_NODE_INFO_FBK_PASS)
					{
						API007NVData_Client_State = REQUEST_READ_DATA;
					}
					else if (API007NVData_Client_Server_Fbk == RECEIVED_NODE_INFO_FBK_FAILED)
					{
						//Their is no use of continuing as we were not able to claim the required size
						API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_ERROR;
						API007NVData_Client_State = ERROR_IRRICOVERABLE_STATE;
						LOG_ADD_ERROR(ERROR_UNABLE_TO_ALLOCATE_CLAIMED_SIZE_IN_SERVER, API007NVDATA_DATA_SECTION_SIZE);	//data explains how much memory its trying to claim in server
					}
				}
				else
				{
                    API007NVData_Client_Msg_Retry_Count++;

                    if (API007NVData_Client_Msg_Retry_Count >= API007NVDATA_CLIENT_MSG_RETRY_COUNT)
                    {
						//Failed to receive feedback from server even though its active
                    	//Diagnosis :Check is Server configured
						API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_ERROR;
						API007NVData_Client_State = ERROR_IRRICOVERABLE_STATE;
						LOG_ADD_ERROR(ERROR_SERVER_NOT_SENDING_FEEDBACK, API007NVDATA_CLIENT_MSG_RETRY_COUNT);	//data explains how many times it retried
                    }
                    else
                    {
                        API007NVData_Client_State = REQUEST_NVDATA_INFO;
                    }
				}

				//Preparation before transit to next state
				if (API007NVData_Client_State == REQUEST_READ_DATA)
				{
					API007NVData_Client_Msg_Retry_Count = 0;
			        API007NVData_Client_Data_Index		= 0;
				}
				break;







            case REQUEST_READ_DATA:
                API007NVData_Client_Server_Fbk			= WAITING_FOR_FBK;
                API007NVData_Client_Timeout_Count		= 0;

                //Calculate how much read data we need to request
            	block_size = DATA_TRANSFER_SEGMENT_SIZE;
            	if ((API007NVData_Client_Data_Index + DATA_TRANSFER_SEGMENT_SIZE) >= API007NVDATA_DATA_SECTION_SIZE)
            	{
            		block_size = API007NVDATA_DATA_SECTION_SIZE - API007NVData_Client_Data_Index;
            	}

				API007Data__NVDataCmdReadRequest(API007NVData_Client_Data_Index, block_size);

				API007NVData_Client_State = WAIT_FOR_DATA_RECEPTION;
                break;

            case WAIT_FOR_DATA_RECEPTION:
            	if (API007NVData_Client_Timeout_Count < API007NVDATA_CLIENT_MSG_REPLY_TIMEOUT_MS)
				{
					if (API007NVData_Client_Server_Fbk == RECEIVED_READ_NVDATA_FBK_PASS)
					{
	                	//All data received
	                	if (API007NVData_Client_Data_Index == API007NVDATA_DATA_SECTION_SIZE)
	                	{
	                		if (API007NVData_Client_Stored_Crc == Crc16__RangeProcess(API007NVDATA_CRC_SECTION_START, INITIAL_CRC16_VALUE, API007NVDATA_CRC_SECTION_SIZE))
	                		{
		                        API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_OKAY;

								//Update CRC if all segments are received
		                        API007NVData_Client_Data_Index			= 0;

		                        for(crc_index = 0; API007NVData_Client_Data_Index < API007NVDATA_DATA_SECTION_SIZE; crc_index++)
		                        {
		                        	block_size = DATA_TRANSFER_SEGMENT_SIZE;

			                        //Calculate for remains
					            	if ((API007NVData_Client_Data_Index + DATA_TRANSFER_SEGMENT_SIZE) >= API007NVDATA_DATA_SECTION_SIZE)
					            	{
					            		block_size = API007NVDATA_DATA_SECTION_SIZE - API007NVData_Client_Data_Index;
					            	}

									API007NVData_Client_Seg_Crc[crc_index] = Crc16__RangeProcess((API007NVDATA_DATA_SECTION_START + API007NVData_Client_Data_Index), INITIAL_CRC16_VALUE, block_size);

									API007NVData_Client_Data_Index += block_size;
		                        }
	                		}
	                		else
	                		{
		                        API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_CORRUPTED;
	                		}

	                        API007NVData_Client_State = REQUEST_WRITE_DATA;
	                	}
	                	else
	                	{
	                		//We still have more data to read
	                        API007NVData_Client_State = REQUEST_READ_DATA;
	                	}
					}
					else if(API007NVData_Client_Server_Fbk == RECEIVED_READ_NVDATA_FBK_FAILED)
					{
						//Restart from the beginning again
                        API007NVData_Client_State 				= REQUEST_NVDATA_INFO;
					}
				}
				else
				{
					API007NVData_Client_Msg_Retry_Count++;

					if (API007NVData_Client_Msg_Retry_Count >= API007NVDATA_CLIENT_MSG_RETRY_COUNT)
					{
						//Failed to receive feedback from server even though its active
						API007NVData_Client_Data_Restore_Status = API007NVDATA_RESTORE_ERROR;
						API007NVData_Client_State = REQUEST_WRITE_DATA;

						LOG_ADD_ERROR(ERROR_SERVER_NOT_SENDING_FEEDBACK, API007NVDATA_CLIENT_MSG_RETRY_COUNT);	//data explains how many times it retried
					}
					else
					{
                        API007NVData_Client_State = REQUEST_READ_DATA;
					}
				}

				//Preparation before transit to next state
				if (API007NVData_Client_State == REQUEST_WRITE_DATA)
				{
					API007NVData_Client_Msg_Retry_Count = 0;
					API007NVData_Client_Data_Index		= 0;
					API007NVData_Client_Timeout_Count   = 0;
				}
                break;








            case REQUEST_WRITE_DATA:
            	//Monitor or update the write data only after the API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS time expires to ease on the communication bus
				if (API007NVData_Client_Timeout_Count > API007NVDATA_CLIENT_DATA_UPDATE_RATE_MS)
				{
					//Data change is detected arbitrarily so that everyone get equal chance

					crc_index = 0;

					for (uint16 num_loops = 0; num_loops < COMPILE_TIME_CLIENT_NUM_BLOCKS; num_loops++)
					{
						if (API007NVData_Client_Data_Index >= API007NVDATA_DATA_SECTION_SIZE)
						{
							API007NVData_Client_Data_Index = 0;

							//calculate data section crc as its stored in the _header_segment which is at the start
	                		API007NVData_Client_Stored_Crc = Crc16__RangeProcess(API007NVDATA_CRC_SECTION_START, INITIAL_CRC16_VALUE, API007NVDATA_CRC_SECTION_SIZE);
						}

						block_size 	= DATA_TRANSFER_SEGMENT_SIZE;
						crc_index 	= API007NVData_Client_Data_Index/DATA_TRANSFER_SEGMENT_SIZE;

						//Calculate for remains
						if ((API007NVData_Client_Data_Index + DATA_TRANSFER_SEGMENT_SIZE) > API007NVDATA_DATA_SECTION_SIZE)
						{
							block_size = API007NVDATA_DATA_SECTION_SIZE - API007NVData_Client_Data_Index;
						}

						crc_seg = Crc16__RangeProcess((API007NVDATA_DATA_SECTION_START + API007NVData_Client_Data_Index), INITIAL_CRC16_VALUE, block_size);

						if(crc_seg != API007NVData_Client_Seg_Crc[crc_index]) // Detect data change
						{
							API007NVData_Client_Seg_Crc[crc_index] 	= crc_seg;
		                    API007NVData_Client_Timeout_Count 		= 0;
		                    API007NVData_Client_Server_Fbk			= WAITING_FOR_FBK;

							//Send modified data
							API007Data__NVDataCmdWriteRequest(API007NVData_Client_Data_Index, block_size, (API007NVDATA_DATA_SECTION_START + API007NVData_Client_Data_Index));

							API007NVData_Client_State 				= WAITING_FOR_SERVER_COMPLETED_WRITING;

							API007NVData_Client_Data_Index += block_size;
							break;
						}

						API007NVData_Client_Data_Index += block_size;
					}
				}
                break;


            case WAITING_FOR_SERVER_COMPLETED_WRITING:
				if (API007NVData_Client_Timeout_Count < API007NVDATA_CLIENT_MSG_REPLY_TIMEOUT_MS)
				{
					if (API007NVData_Client_Server_Fbk == RECEIVED_WRITE_NVDATA_FBK_PASS)
					{
						API007NVData_Client_Timeout_Count = 0;
						API007NVData_Client_State = REQUEST_WRITE_DATA;
					}
				}
				else
				{
					API007NVData_Client_Timeout_Count = 0;
					API007NVData_Client_State = REQUEST_WRITE_DATA;
				}
                break;

            case ERROR_IRRICOVERABLE_STATE :
            	break;

            default:
                API007NVData_Client_State = REQUEST_NVDATA_INFO;
                break;
        }
    #endif //(API007NVDATA_CLIENT_FEATURE == ENABLED)
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This API is been called from the API007Data__CommandParser()
 *          Process the request for claim size by the client
 *
 * @details Enabled only for Server side
 *          Sends the feedback message depending on weather the claim is accepted or rejected
 *
 * @param client_node_id - Client Node ID
 * @param size  - Claim Size
 */
void API007NVData__ProcessClaimSize(uint8 client_node_id, uint16 size)
{
	#if(API007NVDATA_SERVER_FEATURE == ENABLED)
		BOOL_TYPE claim_successfull = FALSE;
		uint16 existing_client_size = 0;
		uint16 re_index = 0;
		uint16 last_client = 0;
		BOOL_TYPE needs_remap_cdt = FALSE;

		for (uint8 index = 0; index < API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT; index++)
		{
			if (API007NVData_CDTable[index].Node_ID != EMPTY_NODE_ID)
			{
				if (API007NVData_CDTable[index].Node_ID == client_node_id)
				{
					//found the node
					//check the size requested matches what CDT already has or else forcedly corrupt the crc
					if (((API007NVData_CDTable[index].End_Index + 1) - API007NVData_CDTable[index].Start_Index) == size)
					{
						claim_successfull = TRUE;
					}
					else
					{
						//Find the last client registered
						for(re_index = index+1; re_index < API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT; re_index++)
						{
							//find the last client registered in the CDT Table
							if (API007NVData_CDTable[re_index].Node_ID == EMPTY_NODE_ID)
							{
								break;
							}
						}

						last_client = re_index;
						re_index--;


						//First make sure we have enough space for claimed size
						existing_client_size = (API007NVData_CDTable[index].End_Index + 1) - API007NVData_CDTable[index].Start_Index;

						//if claim is more then the existing then we need to make sure we have enough space
						if (existing_client_size < size)
						{
							//Does new size fits in the memory
							if ((API007NVDATA_SERVER_DATA_TRUE_SIZE - (API007NVData_CDTable[re_index].End_Index + 1) + existing_client_size) >= size)
							{
								needs_remap_cdt = TRUE;
							}
						}
						else
						{
							//If the new claim is smaller then the existing then we just need remap of CDT Table
							needs_remap_cdt = TRUE;
						}

						if (needs_remap_cdt == TRUE)
						{
							claim_successfull = TRUE;

							API007NVData_CDTable[index].End_Index = API007NVData_CDTable[index].Start_Index + size - 1;	//-1 as end address is inclusive type

							for (re_index = (index+1); re_index < last_client; re_index++)
							{
								existing_client_size = (API007NVData_CDTable[re_index].End_Index + 1) - API007NVData_CDTable[re_index].Start_Index;

								API007NVData_CDTable[re_index].Start_Index = API007NVData_CDTable[(re_index-1)].End_Index + 1;
								API007NVData_CDTable[re_index].End_Index = API007NVData_CDTable[re_index].Start_Index + existing_client_size - 1;	//-1 as end address is inclusive type
							}
						}
						else
						{

						}

						break;	//After Remap CDT break
					}

					break;	//If the claim matches to the CDT then break
				}
			}
			else
			{
				//Node Not registered yet...register it
				//check is enough space available to allocate
				uint16 last_client_end_index = 0;
				if (index != 0)
				{
					last_client_end_index = API007NVData_CDTable[(index-1)].End_Index;
				}

				if ((API007NVDATA_SERVER_DATA_TRUE_SIZE - last_client_end_index) > size)
				{
					API007NVData_CDTable[index].Node_ID 	= client_node_id;
					API007NVData_CDTable[index].Start_Index = last_client_end_index + 1;
					API007NVData_CDTable[index].End_Index 	= API007NVData_CDTable[index].Start_Index + size - 1;

					claim_successfull = TRUE;
				}

                break; //Break after registering into the CDT Table
			}
		}



		if (claim_successfull == FALSE)
		{
			size = 0;
		}

		API007Data__NVDataFbkClaimSize(client_node_id, size);


	#endif
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This API is been called from the API007Data__CommandParser()
 *          Process the request for Read operation by the client
 *
 * @details Enabled only for Server side
 *          Sends the feedback message depending on weather the read operation is accepted or rejected
 *
 * @param client_node_id - Client Node ID
 * @param index - Data index
 * @param size  - Claim Size
 */
void API007NVData__ProcessReadCmd(uint8 client_node_id, uint16 index, uint16 size)
{
	#if(API007NVDATA_SERVER_FEATURE == ENABLED)

		CLIENT_DESCRIPT_TYPE *cdt_client = NVDataGetCDTByClientID(client_node_id);

		if (cdt_client != NULL)
		{
            //make sure we are reading within the client memory area
			if (((cdt_client->End_Index + 1) - cdt_client->Start_Index) >= (index+size))
			{
				API007Data__NVDataFbkReadData(client_node_id, index, size, &API007NVData_ServerData[cdt_client->Start_Index+index]);

			}
			else
			{
			    API007Data__NVDataFbkReadData(client_node_id, index, 0, NULL);    //Reading failed
			}
		}
		else
		{
		    API007Data__NVDataFbkReadData(client_node_id, index, 0, NULL);    //Reading failed
		}
	#endif
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This API is been called from the API007Data__CommandParser()
 *          Process the request for Write operation by the client
 *
 * @details Enabled only for Server side
 *          Sends the feedback message depending on weather the write operation is accepted or rejected
 *
 * @param client_node_id - Client Node ID
 * @param index - Data index
 * @param size  - Claim Size
 * @param *data - Address of data from which to be written
 */
void API007NVData__ProcessWriteCmd(uint8 client_node_id, uint16 index ,uint16 size, uint8 * data_ptr)
{
	#if(API007NVDATA_SERVER_FEATURE == ENABLED)
    	CLIENT_DESCRIPT_TYPE *cdt_client = NULL;

		cdt_client  = NVDataGetCDTByClientID(client_node_id);

		if (cdt_client != NULL)
		{
		    //make sure we are writing within the client memory area
			if (((cdt_client->End_Index + 1) - cdt_client->Start_Index) >= (index + size))
			{
				memcpy((void *)(&API007NVData_ServerData[cdt_client->Start_Index + index]), data_ptr, size);

				API007Data__NVDataFbkWriteData(client_node_id , (index + size));	//send index which is currently pointing after the write cmd
			}
			else
			{
			    API007Data__NVDataFbkWriteData(client_node_id , index);    //writing failed
			}
		}
		else
		{
		    API007Data__NVDataFbkWriteData(client_node_id , index);    //Client not found in CDT table
		}
	#endif
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This API is been called from the API007Data__FeedbackParser() in response of API007_OP_REQ_CLAIM_NVDATA command from server
 *
 * @details Enabled only for client side
 *
 * @param size  - Claim Size
 */
void API007NVData__ReceivedClaimSize(uint16 size)
{
    #if(API007NVDATA_CLIENT_FEATURE == ENABLED)
        if(size == API007NVDATA_DATA_SECTION_SIZE)
        {
            API007NVData_Client_Server_Fbk = RECEIVED_NODE_INFO_FBK_PASS;
        }
        else
        {
            API007NVData_Client_Server_Fbk = RECEIVED_NODE_INFO_FBK_FAILED;
        }
    #endif
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This API is been called from the API007Data__FeedbackParser() in response of API007_OP_REQ_READ_NVDATA command from server
 *
 * @details Enabled only for client side
 *
 * @param index - Read index
 * @param size - Number of Read data size
 * @param data_ptr - Source data memory location
 */
void API007NVData__ReceivedRead(uint16 index ,uint16 size, uint8 * data_ptr)
{
    #if(API007NVDATA_CLIENT_FEATURE == ENABLED)
        if ((index + size) <= API007NVDATA_DATA_SECTION_SIZE)
        {
            memcpy(API007NVDATA_DATA_SECTION_START + index, data_ptr, size);

            API007NVData_Client_Data_Index = index + size;
            API007NVData_Client_Server_Fbk = RECEIVED_READ_NVDATA_FBK_PASS;
        }
        else
        {
            API007NVData_Client_Server_Fbk = RECEIVED_READ_NVDATA_FBK_FAILED;
        }
    #endif//(API007NVDATA_CLIENT_FEATURE == ENABLED)
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This API is been called from the API007Data__FeedbackParser() in response of API007_OP_REQ_WRITE_NVDATA command from server
 *
 * @details Enabled only for client side
 *
 * @param index - Current write index after successful writing
 */
void API007NVData__ReceivedWrite(uint16 index)
{
	#if(API007NVDATA_CLIENT_FEATURE == ENABLED)
		API007NVData_Client_Server_Fbk = RECEIVED_WRITE_NVDATA_FBK_PASS;

		if(index != API007NVData_Client_Data_Index)
		{
			//RECEIVED_WRITE_NVDATA_FBK_FAILED
		}
	#endif
}
#endif


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   API returns the status of the NVData at the client side
 *
 * @details During boot up it takes time to read and verify the NVData from Server
 *          Application shall consider this timing
 *          Recommended to wait until the status becomes other then API007NVDATA_RESTORE_IN_PROGRESS
 *
 *
 * @return API007NVDATA_DATA_RESTORE_STATUS_TYPE
 */
API007NVDATA_DATA_RESTORE_STATUS_TYPE API007NVData__DataRestoreStatus(void)
{
    API007NVDATA_DATA_RESTORE_STATUS_TYPE status = API007NVDATA_RESTORE_CORRUPTED;

    #if ((SYSTEM_ACU_HANDLING == ENABLED) && ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)))  //P1 board or ACU+EXP HMI
        if ((NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered == NVDATA_RESTORE_OKAY) && (NVData__GetLastRestoreStatus()->Bit.Statistics_Data_Recovered == NVDATA_RESTORE_OKAY))
        {
            status = API007NVDATA_RESTORE_OKAY;
        }
    #else
        #if (API007NVDATA_CLIENT_FEATURE == ENABLED)
            status = API007NVData_Client_Data_Restore_Status;
        #endif //(API007NVDATA_CLIENT_FEATURE == ENABLED)
    #endif
    return status;
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Read Client Description information from the CDT table for the requested client_node_id
 *
 *
 * @param client_node_id - client node
 */
#if (API007DATA_NVDATA_FEATURE == ENABLED)
#if(API007NVDATA_SERVER_FEATURE == ENABLED)
static CLIENT_DESCRIPT_TYPE *NVDataGetCDTByClientID(uint8 client_node_id)
{
    CLIENT_DESCRIPT_TYPE *cdt_ptr = NULL;

    for (uint8 index = 0; index < API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT; index++)
    {
        if (API007NVData_CDTable[index].Node_ID == client_node_id)
        {
            cdt_ptr = &API007NVData_CDTable[index];
            break;
        }
    }

    return cdt_ptr;
}
#endif
#endif
