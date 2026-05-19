/**
 *  @file
 *
 *  @brief      API019 implementation.
 *
 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "SystemConfig.h"
#include "API019ProductInfo.h"

#if (API019PRODUCTINFO_FEATURE == ENABLED)
#include "Millis.h"
#include "ProductInfo.h"
#include "String.h"
#include "SRSignature.h"

#if (CALIBRATION_DATA_FEATURE == ENABLED)
    #include "Calibration.h"
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "Micro.h"
    #include "Utilities.h"
    #include "SettingFile.h"
	#include "SettingFile_defs.h"
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED || NVDATA_STORE_STATIC_FEATURE == ENABLED)
    #include "NVData.h"
#endif

//  --- Private Declarations ------------------------------------------------------------------------------------------
// The Data shall come from the Flash
#pragma location = "_static_cfg_segment"
static API019_APPLIANCE_DATA_TYPE API019_Appliance_Data;

#pragma location = "_static_cfg_segment"
static API019_BOARD_DATA_TYPE API019_Board_Data[API019_NUM_BOARDS];

#define API019_NUM_EOL_DATA_TEST    3
#pragma location = "_static_cfg_segment"
static API019_TEST_DATA_TYPE API019_EOL_Test_Data[API019_NUM_EOL_DATA_TEST];

#ifndef API019_NUM_FVT_DATA_TEST
#define API019_NUM_FVT_DATA_TEST    3
#endif
CT_ASSERT(API019_NUM_FVT_DATA_TEST > 0);
#pragma location = "_static_cfg_segment"
static API019_TEST_DATA_TYPE API019_FVT_Data[API019_NUM_FVT_DATA_TEST];

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #pragma location = "_statistics_segment"
    static API019_SERVICE_DATA_TYPE API019_Service_Test_Data;

    uint32 API019_Timer;
#endif

//This symbol is imported from the config.icf file
//This holds the W part of the IAP NUMBER
extern char IAP_WPART_NUMBER;

//  --- Private Definitions -------------------------------------------------------------------------------------------

//  --- Private Function Prototypes -----------------------------------------------------------------------------------
static PASS_FAIL_TYPE PublishApplianceData  (uint8 address);
static PASS_FAIL_TYPE PublishBoardData      (uint8 address, uint8 board_index);
static PASS_FAIL_TYPE PublishProjectData    (uint8 address, API019_BOARD_TYPE board_index, API019_PROJECT_TYPE project_index);
static PASS_FAIL_TYPE PublishTestData       (uint8 address, API019PRODUCTINFO_OPCODE_TYPE opcode,
                                             uint8 index, API019_TEST_DATA_TYPE * test_data);
static void           UpdateTestData        (uint8 * incoming_data, API019_TEST_DATA_TYPE * test_data);
static void           UpdateMaskedArray     (uint8 * data, uint8 * value, uint8 * mask, uint8 size);
static PASS_FAIL_TYPE PublishSignatureData  (uint8 address, API019PRODUCTINFO_OPCODE_TYPE opcode);


#if (SYSTEM_ACU_HANDLING == ENABLED)
    static PASS_FAIL_TYPE PublishProductConfigurationData(uint8 address);
    static PASS_FAIL_TYPE PublishServicetData(uint8 address);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module API019ProductInfo and its variables
 *
 */
void API019ProductInfo__Initialize(void)
{
    uint8 index;

    #if (SYSTEM_ACU_HANDLING == ENABLED)

        if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
        {
            if ((NVData__GetLastRestoreStatus()->Bit.Statistics_Data_Recovered != NVDATA_RESTORE_OKAY) &&
                (NVData__GetLastRestoreStatus()->Bit.Statistics_Data_Recovered != NVDATA_RESTORE_OKAY_BACKUP))
            {
                // Set Default:
                memset(&API019_Service_Test_Data,0,sizeof(API019_SERVICE_DATA_TYPE));
            }
        }

        API019_Timer = Millis__GetFreeRunningCounter();
    #endif
    API019_Appliance_Data.Appliance_Number_Boadrs = API019_NUM_BOARDS;

    for (index=0; index < API019_NUM_BOARDS; index++)
    {
        API019_Board_Data[index].Board_Num_Projects = ProductInfo__GetNumProjects((API019_BOARD_TYPE)index);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses messages target to the API019 sent to a broadcast or exclusively to this node for Reveal 4.x.
 * @param buffer contains the Wide package that contains the message
 * @return REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API019ProductInfo__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    uint8 aux8;
    retval = NVO_RS_READY;

    switch ((API019PRODUCTINFO_OPCODE_TYPE)buffer->op_code)
    {
        case API019PRODUCTINFO_OP_REQ_PUB_APPLIANCE_DATA:
            if (PublishApplianceData(buffer->node_id) == FAIL)
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_SET_APPLIANCE_DATA:
            if (buffer->size == sizeof(API019_APPLIANCE_DATA_TYPE))
            {
                memcpy(&API019_Appliance_Data,buffer->payload,sizeof(API019_APPLIANCE_DATA_TYPE));
                API019_Appliance_Data.Appliance_Number_Boadrs = API019_NUM_BOARDS;
                API019_APPLIANCE_DATA_SET();
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_BOARD_DATA:
            if (PublishBoardData(buffer->node_id, buffer->payload[0]) == FAIL)
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_SET_BOARD_DATA:
            if (buffer->size == (sizeof(API019_BOARD_DATA_TYPE) + 1))
            {
                if (buffer->payload[0] < API019_NUM_BOARDS)
                {
                    memcpy(&API019_Board_Data[buffer->payload[0]],&buffer->payload[1],sizeof(API019_BOARD_DATA_TYPE));

                    API019_Board_Data[buffer->payload[0]].Board_Num_Projects = ProductInfo__GetNumProjects((API019_BOARD_TYPE)(buffer->payload[0]));
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }

            break;
        case API019PRODUCTINFO_OP_REQ_PUB_PROJECT_DATA:
            if (PublishProjectData(buffer->node_id, (API019_BOARD_TYPE)buffer->payload[0], (API019_PROJECT_TYPE)buffer->payload[1]) == FAIL)
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;

        #if (SYSTEM_ACU_HANDLING == ENABLED)
            case API019PRODUCTINFO_OP_REQ_PUB_SERVICE_DATA:
                if (PublishServicetData(buffer->node_id) == FAIL)
                {
                    retval = NVO_RS_BUSY;
                }
                break;
            case API019PRODUCTINFO_OP_SET_SERVICE_DATA:
                if (buffer->size == sizeof(API019_SERVICE_DATA_TYPE))
                {

                    //
                    API019_Service_Test_Data.Tester_ID = buffer->payload[0];

                    API019_Service_Test_Data.Tester_Version[0] = buffer->payload[1];
                    API019_Service_Test_Data.Tester_Version[1] = buffer->payload[2];
                    API019_Service_Test_Data.Tester_Version[2] = buffer->payload[3];
                    aux8 = sizeof(API019_Service_Test_Data.Test_Date);
                    memcpy(API019_Service_Test_Data.Test_Date,&buffer->payload[4],aux8);
                    aux8 += 4;

                    API019_Service_Test_Data.Test_Age = Utilities__ConvertArrayTo32bits(&buffer->payload[aux8]);
                    aux8 += 4;

                    API019_Service_Test_Data.Test_Duration = Utilities__ConvertArrayTo16bits(&buffer->payload[aux8]);
                    aux8 += 2;

                    memcpy(API019_Service_Test_Data.Test_Result,&buffer->payload[aux8],sizeof(API019_Service_Test_Data.Test_Result));
                    API019_Service_Test_Data.Test_Age = 0;

                    if (NVData__ForceBlockingStore() != PASS)
                    {
                        retval = NVO_RS_BUSY;
                    }
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
                break;
            case API019PRODUCTINFO_OP_REQ_PUB_PRODUCT_CONFIG_DATA:
                if (PublishProductConfigurationData(buffer->node_id ) == FAIL)
                {
                    retval = NVO_RS_UNSUPPORTED;
                }
                break;
        #endif
        case API019PRODUCTINFO_OP_REQ_PUB_FVT_DATA:

            if (buffer->payload[0] < API019_NUM_FVT_DATA_TEST)
            {
                if (PublishTestData(buffer->node_id, API019PRODUCTINFO_OP_REQ_PUB_FVT_DATA,buffer->payload[0], &API019_FVT_Data[buffer->payload[0]]) == FAIL)
                {
                    retval = NVO_RS_BUSY;
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_SET_FVT_DATA:
            if (buffer->size == (sizeof(API019_TEST_DATA_TYPE)+1))
            {
                if (buffer->payload[0] < API019_NUM_FVT_DATA_TEST)
                {
                    UpdateTestData(&buffer->payload[1], &API019_FVT_Data[buffer->payload[0]]);
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_SET_FVT_DATA_RESULT:
            aux8 = sizeof(API019_FVT_Data[0].Test_Result);
            if ( buffer->size == ((2*aux8) + 1))
            {
                // Byte 0 index
                // Byte 1 to size+!       = Data
                // Byte size+! to 2xsize+1 = MAsk
                if (buffer->payload[0] < API019_NUM_FVT_DATA_TEST)
                {
                    UpdateMaskedArray(API019_FVT_Data[buffer->payload[0]].Test_Result,&buffer->payload[1], &buffer->payload[aux8+1], aux8);
                }
            }

            break;
        case API019PRODUCTINFO_OP_REQ_PUB_EOL_DATA:
            if (buffer->payload[0] < API019_NUM_EOL_DATA_TEST)
            {
                if (PublishTestData(buffer->node_id, API019PRODUCTINFO_OP_REQ_PUB_EOL_DATA,buffer->payload[0], &API019_EOL_Test_Data[buffer->payload[0]]) == FAIL)
                {
                    retval = NVO_RS_BUSY;
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_SET_EOL_DATA:
            if (buffer->size == (sizeof(API019_TEST_DATA_TYPE)+1))
            {
                if (buffer->payload[0] < API019_NUM_EOL_DATA_TEST)
                {
                    UpdateTestData(&buffer->payload[1], &API019_EOL_Test_Data[buffer->payload[0]]);
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }

            break;
        case API019PRODUCTINFO_OP_SET_EOL_DATA_RESULT:
            aux8 = sizeof(API019_EOL_Test_Data[0].Test_Result);
            if ( buffer->size == ((2*aux8) + 1))
            {
                // Byte 0 index
                // Byte 1 to size+!       = Data
                // Byte size+! to 2xsize+1 = MAsk
                if (buffer->payload[0] < API019_NUM_EOL_DATA_TEST)
                {
                    UpdateMaskedArray(API019_EOL_Test_Data[buffer->payload[0]].Test_Result,&buffer->payload[1], &buffer->payload[aux8+1], aux8);
                }
            }
            break;
#if (CALIBRATION_DATA_FEATURE == ENABLED)
        case API019PRODUCTINFO_OP_SET_CALIB_DATA:
            // Expected payload:
            //      First byte = Calibration Item ID
            //      Additional bytes = Calibration data
            if (buffer->size >= 2)
            {
                CALIBRATION_ITEM_ID_TYPE cal_item = (CALIBRATION_ITEM_ID_TYPE)(buffer->payload[0]);
                uint8 data_length = Calibration__GetDataSize(cal_item);
                if ((data_length > 0) &&
                    (buffer->size == data_length + 1))
                {
                    uint8* data_ptr = Calibration__GetDataPointer(cal_item);
                    memcpy(data_ptr, &buffer->payload[1], data_length);
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_CALIB_DATA:
            // Expected payload:
            //      First byte = Calibration Item ID
            if (buffer->size >= 1)
            {
                CALIBRATION_ITEM_ID_TYPE cal_item = (CALIBRATION_ITEM_ID_TYPE)(buffer->payload[0]);
                uint8 data_length = Calibration__GetDataSize(cal_item);
                if (data_length > 0)
                {
                    uint8* data_ptr = Calibration__GetDataPointer(cal_item);
                    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_CALIB_DATA, buffer->node_id, data_length + 1);
                    if ((msg == NULL) ||
                        (Reveal__AddDataU8(msg, cal_item) == FAIL) ||
                        (Reveal__AddDataU8Buffer(msg, data_ptr, data_length) == FAIL) ||
                        (Reveal__QueueCommand(msg) == FALSE))
                    {
                        retval = NVO_RS_BUSY;
                    }
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
#endif // #if (CALIBRATION_DATA_FEATURE == ENABLED)
        case API019PRODUCTINFO_OP_REQ_PUB_CLASS_B_SIGN:
            if (PublishSignatureData(buffer->node_id, API019PRODUCTINFO_OP_REQ_PUB_CLASS_B_SIGN) == FAIL)
            {
                retval = NVO_RS_BUSY;
            }
            break;
        case API019PRODUCTINFO_OP_STORE_DATA:
            #if (NVDATA_STORE_STATIC_FEATURE == ENABLED)
                if (NVData__StoreStaticData() != PASS)
                {
                    //Failed to store the static data
                    retval = NVO_RS_BUSY;
                }
            #else
                retval = NVO_RS_UNSUP_OP_CODE;
            #endif
            break;
        default:
			retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (retval);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses feedbacks to the API019ProductInfo sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message
 */
void API019ProductInfo__FeedbackParser(REVEAL_MSG_TYPE * buffer)
{

    switch ((API019PRODUCTINFO_OPCODE_TYPE)buffer->op_code)
    {
        case API019PRODUCTINFO_OP_REQ_PUB_APPLIANCE_DATA:
            if (buffer->size == sizeof(API019_APPLIANCE_DATA_TYPE))
            {
                memcpy(&API019_Appliance_Data,buffer->payload,sizeof(API019_APPLIANCE_DATA_TYPE));
                API019_Appliance_Data.Appliance_Number_Boadrs = API019_NUM_BOARDS;
                API019_APPLIANCE_DATA_SET();
            }
            break;
        case API019PRODUCTINFO_OP_SET_APPLIANCE_DATA:
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_BOARD_DATA:
            #if (API019_BOARD_DATA_FEEDBACK == ENABLED)
                if(buffer->size > (1+sizeof(API019_BOARD_DATA_TYPE)))   //the buffer has at least one PROJECT ID
                {
                    ProductInfo__BoardDataFeedback(buffer->node_id,buffer->payload[0],
                                                                (API019_BOARD_DATA_TYPE *)((void *)&buffer->payload[1]),
                                                                (API019_PROJECT_TYPE *)((void *)&buffer->payload[1+sizeof(API019_BOARD_DATA_TYPE)]));
                }
                else
                {
                    ProductInfo__BoardDataFeedback(buffer->node_id,buffer->payload[0],
                                                                                (API019_BOARD_DATA_TYPE *)((void *)&buffer->payload[1]),
                                                                                (API019_PROJECT_TYPE *)NULL);
                }
            #endif
            break;
        case API019PRODUCTINFO_OP_SET_BOARD_DATA:
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_PROJECT_DATA:
            #if (API019_BOARD_DATA_FEEDBACK == ENABLED)
                ProductInfo__ProjectDataFeedback(buffer->node_id,buffer->payload[0],
                                                 (API019_PROJECT_TYPE)buffer->payload[1],
                                                 (API019_PROJECT_DATA_TYPE *)((void *)&buffer->payload[2]));
            #endif
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_SERVICE_DATA:
            break;
        case API019PRODUCTINFO_OP_SET_SERVICE_DATA:
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_FVT_DATA:
            break;
        case API019PRODUCTINFO_OP_SET_FVT_DATA:
            break;
        case API019PRODUCTINFO_OP_SET_FVT_DATA_RESULT:
            break;
        case API019PRODUCTINFO_OP_REQ_PUB_EOL_DATA:
            break;
        case API019PRODUCTINFO_OP_SET_EOL_DATA:
            break;
        case API019PRODUCTINFO_OP_SET_EOL_DATA_RESULT:

            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method handles callbacks from Reveal when and ACK is received for this API
 * @param address
 * @param api
 * @param opcode
 * @param reasoncode
 */
void API019ProductInfo__AckParser(uint8 address,uint8 api,uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode)
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

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method handle processes for the API019ProductInfo on 25ms time bases.
 */
void API019ProductInfo__Handler25ms(void)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        if ((Millis__GetFreeRunningCounter() - API019_Timer) > 1000)
        {
            API019_Timer += 1000;
            API019_Service_Test_Data.Test_Age++;
        }
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @return
 */
API019_SERVICE_DATA_TYPE * API019ProductInfo__GetServiceDataReference(void)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        return (&API019_Service_Test_Data);
    #else
        return (NULL);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param index
 * @return
 */
API019_TEST_DATA_TYPE * API019ProductInfo__GetEOLDataReference(uint8 index)
{
    if (index < API019_NUM_EOL_DATA_TEST)
    {
        return (&API019_EOL_Test_Data[index]);
    }
    else
    {
        return (NULL);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns a reference to an instance of FVT data
 * @param   index
 * @return
 */
API019_TEST_DATA_TYPE const * API019ProductInfo__GetFVTDataReference(uint8 index)
{
    if (index < API019_NUM_FVT_DATA_TEST)
    {
        return (&API019_FVT_Data[index]);
    }
    else
    {
        return (NULL);
    }
}


//----------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get a pointer to constant API019_Appliance_Data.
 *
 *  @return     API019_APPLIANCE_DATA_TYPE const * - Pointer to constant appliance data.
 */
API019_APPLIANCE_DATA_TYPE const * API019ProductInfo__GetApplianceData(void)
{
    return &API019_Appliance_Data;
}

//----------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set API019_Appliance_Data to appliance_data.
 *
 *  @param      API019_APPLIANCE_DATA_TYPE const * - Pointer to constant appliance data.
 *
 *  @return PASS_FAIL_TYPE
 *      @retval PASS(1) - Successfully able to set the appliance data
 *      @retval FAIL(0) - Failed to set the appliance data
 */
PASS_FAIL_TYPE API019ProductInfo__SetApplianceData(API019_APPLIANCE_DATA_TYPE const * appliance_data)
{
    PASS_FAIL_TYPE status = FAIL;
    memcpy(&API019_Appliance_Data, appliance_data, sizeof(API019_APPLIANCE_DATA_TYPE));
    API019_Appliance_Data.Appliance_Number_Boadrs = API019_NUM_BOARDS;

    #if (NVDATA_STORE_STATIC_FEATURE == ENABLED)
        status = NVData__StoreStaticData();
    #else
        status = PASS;
    #endif

    return (status);
}

//----------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get a pointer to constant API019_Board_Data for board_type.
 *
 *  @return     API019_BOARD_DATA_TYPE const * - Pointer to constant board data for board_type.
 */
API019_BOARD_DATA_TYPE const * API019ProductInfo__GetBoardData(uint8 board_type)
{
    if (board_type < API019_NUM_BOARDS)
    {
        return (&API019_Board_Data[board_type]);
    }
    else
    {
        return (NULL);
    }
}

#if (API019_BOARD_DATA_FEEDBACK == ENABLED)
//----------------------------------------------------------------------------------------------------------
/**
 * @brief Queues a Reveal Command requesting API019PRODUCTINFO_OP_REQ_PUB_BOARD_DATA
 * @param node_address
 * @param board_type
 * @retval PASS Command queued successfully
 * @retval FAIL Command was not queued
 */
PASS_FAIL_TYPE API019ProductInfo__RequestBoardData(uint8 node_address, uint8 board_type)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewCommand(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_BOARD_DATA, node_address, 1);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, (uint8)board_type);
        Reveal__QueueCommand(ptr);
        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Queues a Reveal Command requesting API019PRODUCTINFO_OP_REQ_PUB_PROJECT_DATA
 * @param node_address
 * @param board_type
 * @param project_id
 * @retval PASS Command queued successfully
 * @retval FAIL Command was not queued
 */
PASS_FAIL_TYPE API019ProductInfo__RequestProjectData(uint8 node_address, uint8 board_type, API019_PROJECT_TYPE project_id)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewCommand(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_PROJECT_DATA, node_address,2);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, (uint8)board_type);
        Reveal__AddDataU8(ptr, (uint8)project_id);
        Reveal__QueueCommand(ptr);
        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}
#endif
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param address
 * @return
 */
static PASS_FAIL_TYPE PublishApplianceData(uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_APPLIANCE_DATA, address,sizeof(API019_APPLIANCE_DATA_TYPE));

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8Buffer(ptr, (uint8*)&API019_Appliance_Data,sizeof(API019_APPLIANCE_DATA_TYPE));    //lint !e928 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        Reveal__QueueCommand(ptr);
        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param address
 * @param board_index
 * @return
 */
static PASS_FAIL_TYPE PublishBoardData(uint8 address, uint8 board_index)
{
    REVEAL_COMMAND_TYPE * ptr;
    PASS_FAIL_TYPE retval;
    retval = FAIL;
    if (board_index < API019_Appliance_Data.Appliance_Number_Boadrs)
    {
        ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_BOARD_DATA, address,sizeof(API019_BOARD_DATA_TYPE)+1 + API019_Board_Data[board_index].Board_Num_Projects);

        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU8(ptr,board_index);
            Reveal__AddDataU8Buffer(ptr, (uint8*)&API019_Board_Data[board_index],sizeof(API019_BOARD_DATA_TYPE));   //lint !e928 cast from pointer to pointer [MISRA 2004 Rule 11.4]

            API019_PROJECT_TYPE *project_type_pt = ProductInfo__GetProjectIDs((API019_BOARD_TYPE)board_index);

            if(project_type_pt != NULL)
            {
                Reveal__AddDataU8Buffer(ptr, (uint8*)project_type_pt, API019_Board_Data[board_index].Board_Num_Projects);   //lint !e928 cast from pointer to pointer [MISRA 2004 Rule 11.4]
            }

            Reveal__QueueCommand(ptr);
            retval = PASS;
        }
    }
    return (retval);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function prepares the Project data
 * @param address
 * @param board_index
 * @param project_index
 * @return
 */
static PASS_FAIL_TYPE PublishProjectData(uint8 address, API019_BOARD_TYPE board_index, API019_PROJECT_TYPE project_index)
{
    REVEAL_COMMAND_TYPE * ptr;
    API019_PROJECT_DATA_TYPE * pt_data;
    API019_PROJECT_DATA_TYPE iap_project_data;

    PASS_FAIL_TYPE retval;
    retval = FAIL;
    pt_data = NULL;


    #if (SETTINGFILE_MASTER_FILE_FEATURE == ENABLED)
        API019_PROJECT_DATA_TYPE settingfileprojectdata;
        SETTINGFILE_LOADER_TYPE load_sf_data;

        if (project_index == API019_PROJECT_SETTINGFILE)
        {
            //Setting File information is available only with the ACU board

            //Clear the variable
            memset(&settingfileprojectdata, 0, sizeof(settingfileprojectdata));

            //Project_Name
            strcpy(settingfileprojectdata.Project_Name, "Setting FL");

            //Project_Release_Number
            if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_PART_NUMBER, &load_sf_data) == PASS)
            {
                memcpy(settingfileprojectdata.Project_Release_Number, load_sf_data.Data, sizeof(settingfileprojectdata.Project_Release_Number));

            }
            else
            {
                memset(settingfileprojectdata.Project_Release_Number,' ' ,sizeof(settingfileprojectdata.Project_Release_Number));
            }


            //Project_Release_Date[8]
            //Release Hour
            //Release Minute
            if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_PROJECT_TIMESTAMP, &load_sf_data) == PASS)
            {
                memcpy(settingfileprojectdata.Project_Release_Date, load_sf_data.Data, sizeof(settingfileprojectdata.Project_Release_Date));

                #define SF_DISPLACEMENT_TIMESTAMP_TIME_INDEX        8
                #define PROJECT_CLASSB_ID_TIMESTAMP_TIME_SIZE       6
                memcpy(settingfileprojectdata.Project_ClassB_Id, &load_sf_data.Data[SF_DISPLACEMENT_TIMESTAMP_TIME_INDEX], PROJECT_CLASSB_ID_TIMESTAMP_TIME_SIZE);

                //Fill the rest of the string with the NULL character
                #define PROJECT_CLASSB_ID_UNUSED_BYTES              5
                #define PROJECT_CLASSB_ID_UNUSED_BYTES_INDEX        6

                memset(&settingfileprojectdata.Project_ClassB_Id[PROJECT_CLASSB_ID_UNUSED_BYTES_INDEX], ' ', PROJECT_CLASSB_ID_UNUSED_BYTES);

            }
            else
            {
                memset(settingfileprojectdata.Project_Release_Date,' ' ,sizeof(settingfileprojectdata.Project_Release_Date));
            }

            //GESE Project_Release_Version
            if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_GESE_VERSION, &load_sf_data) == PASS)
            {
                memcpy(settingfileprojectdata.Project_Release_Version, load_sf_data.Data, sizeof(settingfileprojectdata.Project_Release_Version));
            }

            // Project Status
            if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_PROJECT_STATUS, &load_sf_data) == PASS)
            {
                memcpy(&settingfileprojectdata.Micro_Vendor, load_sf_data.Data, sizeof(settingfileprojectdata.Micro_Vendor));
            }

            //Platform_Type
            //Product_Type
            if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_PLATFORM_PRODUCT_TYPE, &load_sf_data) == PASS)
            {
                #define SF_PLATFORM_TYPE_INDEX          0
                #define SF_PRODUCT_TYPE_INDEX           1

                settingfileprojectdata.Platform_Type = (API019_PLATFORM_TYPE)load_sf_data.Data[SF_PLATFORM_TYPE_INDEX];
                settingfileprojectdata.Product_Type = load_sf_data.Data[SF_PRODUCT_TYPE_INDEX];
            }

            // Set the Endianess bit from C_Extensions.h
            settingfileprojectdata.Product_Cfg1 = MICRO_IS_LITTLE_ENDIAN;

            //ClassB_CRC
            if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_CLASSB_CRC, &load_sf_data) == PASS)
            {
                settingfileprojectdata.ClassB_CRC = *(uint16 *)(void *)(load_sf_data.Data);
            }

            pt_data = &settingfileprojectdata;
        }
        else
    #endif
        {
            char *iap_w_address = ((char *)&IAP_WPART_NUMBER);

            if ((project_index == API019_PROJECT_IAP) && (iap_w_address[0] == 'W'))
            {
                //Micro_Vendor 			- 	API019_MICRO_VENDOR_NOT_DEFINED = 255
                //Micro_Specific 		- 	API019_MICRO_SPECIFIC_NOT_DEFINED=0xFF
                //Platform_Type 		-	API019_PLATFORM_NOT_AVAILABLE  = 255
                //API019_DEVICE_TYPE 	-	API019_DEVICE_NOT_AVAILABLE = 255
                //Product_Cfg1			-   0xFF
                //Product_Type			- 	0xFF
                //ClassB_CRC			- 	0xFFFFFFFF

                memset(&iap_project_data, 0xFF, sizeof(iap_project_data));

                //Project_Name
                strcpy(iap_project_data.Project_Name, "IAP FIRM");

                //copy W part number
                memcpy(iap_project_data.Project_Release_Number, iap_w_address, 10);

                memset(iap_project_data.Project_Release_Date, 0, 8);

                memset(iap_project_data.Project_Release_Version, 0, 3);

                memset(iap_project_data.Project_ClassB_Id, 0, 11);

                pt_data = &iap_project_data;
            }
            else
            {
                pt_data = ProductInfo__GetProjectData(board_index, project_index);
                if (pt_data != NULL)
                {
                    // Set the Endianess bit from C_Extensions.h
                    pt_data->Product_Cfg1 &= (0xFE); // Clear bit
                    pt_data->Product_Cfg1 |= (0x01 & MICRO_IS_LITTLE_ENDIAN); // Set bit according to macro.
                }
            }
        }

        if (pt_data != NULL)
        {
            //Prepare the Reveal message
            ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_PROJECT_DATA, address,sizeof(API019_PROJECT_DATA_TYPE)+2);

            if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
            {
                Reveal__AddDataU8(ptr,board_index);
                Reveal__AddDataU8(ptr,project_index);
                Reveal__AddDataU8Buffer(ptr, (uint8*)pt_data,sizeof(API019_PROJECT_DATA_TYPE));     //lint !e928 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                Reveal__QueueCommand(ptr);
                retval = PASS;
            }
        }


    return (retval);
}

/**
 *
 * @param address
 * @param opcode
 * @param test_data
 * @return
 */
static PASS_FAIL_TYPE PublishTestData(uint8 address, API019PRODUCTINFO_OPCODE_TYPE opcode, uint8 index,API019_TEST_DATA_TYPE * test_data)
{
    REVEAL_COMMAND_TYPE * ptr;
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, opcode, address,sizeof(API019_TEST_DATA_TYPE)+1);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, index);
        Reveal__AddDataU8Buffer(ptr, (uint8*)test_data,sizeof(API019_TEST_DATA_TYPE));      //lint !e928 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        Reveal__QueueCommand(ptr);
        retval = PASS;
    }

    return (retval);
}

#if (SYSTEM_ACU_HANDLING == ENABLED)
/**
 *
 * @param address
 * @return
 */
static PASS_FAIL_TYPE PublishServicetData(uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_SERVICE_DATA, address,sizeof(API019_SERVICE_DATA_TYPE));

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {


        Reveal__AddDataU8(ptr, API019_Service_Test_Data.Tester_ID);
        Reveal__AddDataU8(ptr, API019_Service_Test_Data.Tester_Version[0]);
        Reveal__AddDataU8(ptr, API019_Service_Test_Data.Tester_Version[1]);
        Reveal__AddDataU8(ptr, API019_Service_Test_Data.Tester_Version[2]);
        Reveal__AddDataU8Buffer(ptr, (uint8*)API019_Service_Test_Data.Test_Date,sizeof(API019_Service_Test_Data.Test_Date));        //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        Reveal__AddDataU32(ptr, API019_Service_Test_Data.Test_Age);
        Reveal__AddDataU16(ptr, API019_Service_Test_Data.Test_Duration);
        Reveal__AddDataU8Buffer(ptr, (uint8*)API019_Service_Test_Data.Test_Result,sizeof(API019_Service_Test_Data.Test_Result));    //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        Reveal__QueueCommand(ptr);
        retval = PASS;
    }

    return (retval);
}
#endif

/**
 *
 * @param buffer
 * @param test_data
 * @return
 */
static void UpdateTestData(uint8 * incoming_data, API019_TEST_DATA_TYPE * test_data)
{
    memcpy(test_data,incoming_data,sizeof(API019_TEST_DATA_TYPE));
}

static void UpdateMaskedArray(uint8 * data, uint8 * value, uint8 * mask, uint8 size)
{

    uint8 aux;
    uint8 index;

    for (index=0; index< size; index++)
    {
        aux = mask[index] & (~value[index]);
        data[index] = data[index] & (~aux);
        data[index] = data[index] | (mask[index] & value[index]);
    }

}

/**
 *
 * @param address
 * @param opcode
 * @param cal_data
 * @return
 */
static PASS_FAIL_TYPE PublishSignatureData(uint8 address, API019PRODUCTINFO_OPCODE_TYPE opcode)
{
    REVEAL_COMMAND_TYPE * ptr;
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, opcode, address, (SRSignature__GetClassBSignatureSize() + 1));

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8Buffer(ptr, SRSignature__GetClassBSignature(), SRSignature__GetClassBSignatureSize());
        Reveal__QueueCommand(ptr);
        retval = PASS;
    }

    return (retval);
}

#if (SYSTEM_ACU_HANDLING == ENABLED)
//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function prepares the Product Configuration data
 * @param address
 * @return PASS / FAIL
 */
static PASS_FAIL_TYPE PublishProductConfigurationData(uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    PASS_FAIL_TYPE retval;
    retval = FAIL;

    SETTINGFILE_LOADER_TYPE load_sf_data;

    //Product Configuration Data
    if (SettingFile__BasicLoader(SF_PTR_ACU_SERIAL_NUMBER, SF_DISPL_PRODUCT_CONFIGURATION_DATA, &load_sf_data) == PASS)
    {
		//Prepare the Reveal message
		ptr = Reveal__NewFeedback(API019PRODUCTINFO_NUM, API019PRODUCTINFO_OP_REQ_PUB_PRODUCT_CONFIG_DATA, address,load_sf_data.Length);
		if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
		{
			if((Reveal__AddDataU8Buffer(ptr, (uint8 *)load_sf_data.Data ,load_sf_data.Length) == PASS) &&
					(Reveal__QueueCommand(ptr) == TRUE))
			{
				retval = PASS;
			}
		}
    }
    return (retval);
}
#endif //#if (SYSTEM_ACU_HANDLING == ENABLED)

#endif //API019PRODUCTINFO_FEATURE
