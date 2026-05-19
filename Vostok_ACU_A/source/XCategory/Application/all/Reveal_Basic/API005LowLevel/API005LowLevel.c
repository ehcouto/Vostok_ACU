/**
 *  @file
 *
 *  @brief      This Reveal API manages the Low Level access to the system which can be used by FVT, Service and Manufacturing.
 *
 *  @details    Detailed description of the file contents
 *
 *  @copyright  Copyright 2013-$Date: 2015/07/24 10:18:59EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (API005LOWLEVEL_FEATURE == ENABLED)

#ifndef API005_USE_LOADS_AND_GIS_FEATURE
    // API005_USE_LOADS_AND_GIS_FEATURE should be ENABLED for applications that use Hbl to control Loads and GIs.
    #error API005_USE_LOADS_AND_GIS_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif
#ifndef API005_USE_OBJECTS_AND_EVENTS_FEATURE
    // API005_USE_OBJECTS_AND_EVENTS_FEATURE should be ENABLED for applications that use HblHmi to control Objects and Events.
    #error API005_USE_OBJECTS_AND_EVENTS_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif
#ifndef API005_CONTROL_CYCLES_FEATURE
    // API005_CONTROL_CYCLES_FEATURE should be ENABLED for applications that use API005 to control cycles.
    #error API005_CONTROL_CYCLES_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#include "API005LowLevel.h"
#include "API005LowLevel_prv.h"

#include "Mode.h"
#include "Reveal.h"
#include <string.h>
#include "Utilities.h"

#if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
    #include "Hbl.h"
    #include "SRHbl.h"
#endif

#if (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
    #include "HblHmi.h"
#endif

#if (API005_CONTROL_CYCLES_FEATURE == ENABLED)
    #include "TestMgr.h"
#endif


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef enum
{
    API005LOWLEVEL_OC_SET_LOAD                              = 0x01,
    API005LOWLEVEL_OC_GET_PUB_LOAD_DATA                     = 0x02,
    API005LOWLEVEL_OC_GET_PUB_LLI_DATA                      = 0x03,
    API005LOWLEVEL_OC_GET_PUB_GI_DATA                       = 0x04,
    API005LOWLEVEL_OC_GET_PUB_ALL_LOADS_CONFIGURATION       = 0x05,
    API005LOWLEVEL_OC_GET_PUB_ALL_LLI_CONFIGURATION         = 0x06,
    API005LOWLEVEL_OC_GET_PUB_ALL_GI_CONFIGURATION          = 0x07,
    API005LOWLEVEL_OC_PUBLISH_EVENT                         = 0x08,

    API005LOWLEVEL_OC_CALL_DEBUG_FUNCTION                   = 0x10,
    API005LOWLEVEL_OC_DEBUG_LABEL                           = 0x11,
    API005LOWLEVEL_OC_SET_DEBUG_VARIABLE                    = 0x12,

    API005LOWLEVEL_OC_HMI_SET_OBJECT                        = 0x13,
    API005LOWLEVEL_OC_HMI_PLAY_SOUND                        = 0x14,
    API005LOWLEVEL_OC_HMI_PUB_GI_DATA                       = 0x15,
    API005LOWLEVEL_OC_HMI_PUB_LLI_DATA                      = 0x16,
    API005LOWLEVEL_OC_HMI_PUB_GI_CONFIG                     = 0x17,
    API005LOWLEVEL_OC_HMI_PUB_LLI_CONFIG                    = 0x18,


    API005LOWLEVEL_SET_TEST_STORAGE                         = 0x1A,
    API005LOWLEVEL_ENGAGE_TEST_CYCLE                        = 0x1B,
    API005LOWLEVEL_PHASE_TEST_CYCLE                         = 0x1C,

    API005LOWLEVEL_OC_EXTRACT_LOADS                         = 0x1D,
    API005LOWLEVEL_OC_EXTRACT_GIS                           = 0x1E,
    API005LOWLEVEL_STEP_TEST_CYCLE                          = 0x1F
}API005LOWLEVEL_OC_TYPE;


#if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
static uint8 API005_Timer_Divider;
static uint8 API005_Timer;
static BOOL_TYPE API005_Publish_Loads;
static BOOL_TYPE API005_Publish_Inputs;
static const uint8 API005_LOADS_OFF[] = {0,0,0,0,0,0,0,0,0,0};
#endif

#define PAYLOAD_INDEX_DELTA     (PAYLOAD_DATA_SIZE + 1)

#define REVEAL_MMP_BIT          ((uint8)0x80)        //reveal more message pending
/*
 * defines the maximum number of characters for the debug function Label.
 * Resolution: 1 = 1 ASCII character
 */
#define API005_SIZE_OF_LABEL    20

// structure for the debug table
typedef struct
{
    FUNCTION_TYPE   Function;                           // debug function that will be run
    char            Label[API005_SIZE_OF_LABEL];        // label that will be displayed on the WideBox button.  Maximum number of characters is defined by API005_SIZE_OF_LABEL.
} API005LOWLEVEL_DEBUG_TABLE_ELEMENT_TYPE;

// definition of the Debug functions and labels.
static const API005LOWLEVEL_DEBUG_TABLE_ELEMENT_TYPE API005LOWLEVEL_DEBUG_TABLE[] =
{
    API005LOWLEVEL_DEBUG_TABLE_APP_SPECIFIC
};

typedef uint8 (* const HBL_GETDATASIZEBYINDEX)(uint8 index);
typedef void (* const BUILD_REVEAL_MSG)(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);

typedef struct
{
    HBL_GETDATASIZEBYINDEX GetDataSizeByIndex;
    BUILD_REVEAL_MSG BuildMessage;
    uint8 Non_ItemData_Size;
}API005LOWLEVEL_PUBLISH_DATA_HOOK_TYPE;

#define API005_NUM_DEBUG_FUNCTIONS (sizeof (API005LOWLEVEL_DEBUG_TABLE) / sizeof (API005LOWLEVEL_DEBUG_TABLE_ELEMENT_TYPE))

//! Define how many Opcodes the API005_USE_LOADS_AND_GIS_FEATURE publishes
#define PUBLISH_DATA_NUM_OF_LOADs_AND_GI_OPCODES    (6)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if ((API005_USE_LOADS_AND_GIS_FEATURE == ENABLED) || (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED))
    static REVEAL_RECIPE_STATUS_TYPE PublishData(API005LOWLEVEL_OC_TYPE oc, uint8 load_index, uint8 num_loads, uint8 node_id);
    static uint8 GetDummyDataSize(uint8 index);
#endif

#if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
    static void BuildLoadDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildLLIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildGIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildAllLoadConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildAllLLIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildAllGIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static uint8 HblGetGIConvertedDataSizeByIndex(uint8 index);
#endif

#if (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
    void API005ReceiveEvent(HBLHMI_GI_EVENT_TYPE event);
    static void BuildHmiGIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildHmiLLIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildHmiAllLLIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
    static void BuildHmiAllGIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr);
#endif

#if( (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED) || (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED))
    const API005LOWLEVEL_PUBLISH_DATA_HOOK_TYPE PUBLISH_DATA_FUNCTION_PTR[] =
    {
    #if(API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
        {Hbl__GetLoadDataSize                   ,BuildLoadDataMessage                  ,4},     //API005LOWLEVEL_OC_GET_PUB_LOAD_DATA
        {Hbl__GetLLIDataSizeByIndex             ,BuildLLIDataMessage                   ,3},     //API005LOWLEVEL_OC_GET_PUB_LLI_DATA
        {HblGetGIConvertedDataSizeByIndex       ,BuildGIDataMessage                    ,1},     //API005LOWLEVEL_OC_GET_PUB_GI_DATA
        {GetDummyDataSize                       ,BuildAllLoadConfigurationMessage      ,2},     //API005LOWLEVEL_OC_GET_PUB_ALL_LOADS_CONFIGURATION
        {GetDummyDataSize                       ,BuildAllLLIConfigurationMessage       ,2},     //API005LOWLEVEL_OC_GET_PUB_ALL_LLI_CONFIGURATION
        {GetDummyDataSize                       ,BuildAllGIConfigurationMessage        ,3},     //API005LOWLEVEL_OC_GET_PUB_ALL_GI_CONFIGURATION
    #endif
    #if (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
        {HblHmi__GetGIDataSizeByIndex           ,BuildHmiGIDataMessage                  ,2},     //API005LOWLEVEL_OC_HMI_PUB_GI_DATA
        {HblHmi__GetLLIDataSizeByIndex          ,BuildHmiLLIDataMessage                 ,3},    //API005LOWLEVEL_OC_HMI_PUB_LLI_DATA
        {GetDummyDataSize                       ,BuildHmiAllGIConfigurationMessage      ,4},    //API005LOWLEVEL_OC_HMI_PUB_GI_CONFIG
        {GetDummyDataSize                       ,BuildHmiAllLLIConfigurationMessage     ,3},    //API005LOWLEVEL_OC_HMI_PUB_LLI_CONFIG
    #endif
    };
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief It Initializes the module API005LowLevel and its variables
 *
 */
void API005LowLevel__Initialize(void)
{
    #if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
        API005_Timer = 0;
        API005_Timer_Divider = 0;
        API005_Publish_Loads = FALSE;
        API005_Publish_Inputs = FALSE;
    #endif

    #if (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
        HblHmi__RegisterGIEventHandler(API005ReceiveEvent);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses messages target to the API005 sent to a broadcast or exclusively to this node for Reveal 4.x.
 * @param buffer contains the Wide package that contains the message
 * @return REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API005LowLevel__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    REVEAL_COMMAND_TYPE * ptr;

    retval = NVO_RS_READY;
    #if ((API005_USE_LOADS_AND_GIS_FEATURE == ENABLED) || (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED))
        uint8 num_items = 0;
        uint8 item_index = 0;
    #endif
    #if(API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
        HBLHMI_GI_EVENT_TYPE gi_event;
    #endif

    switch (buffer->op_code)
    {
        #if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
            case API005LOWLEVEL_OC_SET_LOAD:
                if (Mode__IsPCControlEngaged() == FALSE)
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                else if (buffer->size == 0)
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
                else
                {
                    //Load ID  = 0 to 254 is used to set the individual load
                    //Load ID  = 255 is used to turn-off all the loads. In this case following pay load value is invalid

                    // Payload[0] = LOAD IDx
                    // Payload[1] = LOAD DATA SIZE
                    // Payload[2] = LOAD DATA[0]
                    // Payload[3] = LOAD DATA[n-1]
                    // Payload[4] = LOAD IDy
                    // Payload[5] = LOAD DATA SIZE
                    // Payload[6] = LOAD DATA[0]
                    // Payload[7] = LOAD DATA[n-1]

                    #define PAYLOAD_LOAD_ID                 (buffer->payload[payload_index])
                    #define PAYLOAD_LOAD_DATA_SIZE          (buffer->payload[payload_index + 1])
                    #define PAYLOAD_LOAD_DATA               (buffer->payload[payload_index + 2])
                    #define PAYLOAD_NEXT_LOAD_ID_INDEX      (payload_index + PAYLOAD_LOAD_DATA_SIZE + 2)

                    uint8 payload_index = 0;

                    // define the return message buffer
                    uint8 ret_mess[REVEAL_PAYLOAD_SIZE];

                    if ((buffer->size == 1) &&
                        (PAYLOAD_LOAD_ID == 0xFF))
                    {
                        ret_mess[payload_index] = PAYLOAD_LOAD_ID;

                        uint8 num_loads = Hbl__GetNumLoads();
                        for (uint8 load_loop_counter = 0; load_loop_counter < num_loads; load_loop_counter++)
                        {
                            Hbl__SetLoadRequestByIndex(load_loop_counter, HBL_PRIORITY_PC_CONTROL, (uint8 *)API005_LOADS_OFF); //lint !e926 cast from pointer to pointer
                        }

                        //payl oad has only 1 pay load data
                        payload_index = 1;
                    }
                    else
                    {
                        /***** loop for parsing the message for controlling the loads and building the return message ******/

                        do
                        {
                            HBL_LOAD_ENUM_TYPE hbl_load_id = (HBL_LOAD_ENUM_TYPE)PAYLOAD_LOAD_ID;
                            uint8 hbl_load_index = Hbl__GetLoadIndexByLoadID(hbl_load_id);

                            ret_mess[payload_index] = hbl_load_id;

                            /******** verify the hbl load index is valid *************/
                            if (hbl_load_index >= Hbl__GetNumLoads())
                            {
                                retval = NVO_RS_UNSUP_INVALID_PARAM;
                                break;  // exit do while loop
                            }

                            /** verify the buffer payload data size and the Hbl module size are the same **/
                            uint8 size = Hbl__GetLoadDataSize(hbl_load_index);
                            if (PAYLOAD_LOAD_DATA_SIZE != size)
                            {
                                retval = NVO_RS_UNSUP_INVALID_PARAM;
                                break; // exit do while loop
                            }

                            /** set the Hbl load **/
                            if (size > 0)
                            {
                                (void)Hbl__SetLoadRequestByLoadID(hbl_load_id, HBL_PRIORITY_PC_CONTROL, &PAYLOAD_LOAD_DATA);

                                /**** Write the hbl load size and data to the return message buffer ****/
                                ret_mess[payload_index + 1] = size;
                                uint8 * data = Hbl__GetProcessedLoad(hbl_load_index);
                                if (data == NULL)
                                {
                                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                                    break;  // exit do while loop
                                }
                                else
                                {
                                    memcpy(&ret_mess[payload_index + 2], data, PAYLOAD_LOAD_DATA_SIZE);
                                }
                            }

                            payload_index = PAYLOAD_NEXT_LOAD_ID_INDEX;
                        } while (payload_index < buffer->size);

                    }

                    if (retval == NVO_RS_READY)     // verify the do while loop was not prematurely exited
                    {
                        ptr = Reveal__NewFeedback(API005LOWLEVEL_NUM, API005LOWLEVEL_OC_SET_LOAD, buffer->node_id, payload_index);
                        if (ptr == (REVEAL_COMMAND_TYPE *)NULL)
                        {
                            retval = NVO_RS_UNSUPPORTED;
                        }
                        else
                        {
                            (void)Reveal__AddDataU8Buffer(ptr, ret_mess, payload_index);
                            (void)Reveal__QueueCommand(ptr);
                        }
                    }
                }
                break;

            case API005LOWLEVEL_OC_GET_PUB_LOAD_DATA:
                //For LOAD ID - 0 to 254 it publishes the requested LOAD ID DATA
                if (buffer->payload[0] != 0xFF)
                {
                    item_index = Hbl__GetLoadIndexByLoadID((HBL_LOAD_ENUM_TYPE)buffer->payload[0]);

                    if (item_index != HBL_INDEX_INVALID)
                    {
                        num_items = 1;
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    //For LOAD ID - 255 is used to request all load data
                    num_items = Hbl__GetNumLoads();
                    item_index = 0;
                }

                if (retval == NVO_RS_READY)
                {
                    retval = PublishData(API005LOWLEVEL_OC_GET_PUB_LOAD_DATA, item_index, num_items, buffer->node_id);
                }
                break;

            case API005LOWLEVEL_OC_GET_PUB_LLI_DATA:
                //For LLI TYPE - 0 to 254 it publishes the requested LLI Data
                if (buffer->payload[0] != 0xFF)
                {
                    item_index = HBl__GetLLIIndexByID((HBL_LLI_ENUM_TYPE)buffer->payload[0], buffer->payload[1]);

                    if (item_index != HBL_INDEX_INVALID)
                    {
                        num_items = 1;
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    //For LLI TYPE - 255 is used to publish all LLI Data
                    num_items = Hbl__GetNumLLI();
                    item_index = 0;
                }

                if (retval == NVO_RS_READY)
                {
                    retval = PublishData(API005LOWLEVEL_OC_GET_PUB_LLI_DATA, item_index, num_items, buffer->node_id);
                }
                break;

            case API005LOWLEVEL_OC_GET_PUB_GI_DATA:
                //For GI ID - 0 to 254 it publishes the requested GI CONVERTED DATA
                if (buffer->payload[0] != 0xFF)
                {
                    item_index = Hbl__GetGIIndexByGIID((HBL_GENERIC_INPUT_ENUM_TYPE)buffer->payload[0]);

                    if (item_index != HBL_INDEX_INVALID)
                    {
                        num_items = 1;
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    //For GI ID - 255 is used to request all GI CONVERTED DATA
                    num_items = Hbl__GetNumGI();
                    item_index = 0;
                }

                if (retval == NVO_RS_READY)
                {
                    retval = PublishData(API005LOWLEVEL_OC_GET_PUB_GI_DATA, item_index, num_items, buffer->node_id);
                }
                break;

            case API005LOWLEVEL_OC_GET_PUB_ALL_LOADS_CONFIGURATION:
                retval = PublishData(API005LOWLEVEL_OC_GET_PUB_ALL_LOADS_CONFIGURATION, 0, Hbl__GetNumLoads(), buffer->node_id);
                break;

            case API005LOWLEVEL_OC_GET_PUB_ALL_LLI_CONFIGURATION:
                retval = PublishData(API005LOWLEVEL_OC_GET_PUB_ALL_LLI_CONFIGURATION, 0, Hbl__GetNumLLI(), buffer->node_id);
                break;

            case API005LOWLEVEL_OC_GET_PUB_ALL_GI_CONFIGURATION:
                retval = PublishData(API005LOWLEVEL_OC_GET_PUB_ALL_GI_CONFIGURATION, 0, Hbl__GetNumGI(), buffer->node_id);
                break;

            case API005LOWLEVEL_OC_EXTRACT_LOADS:
                API005_Publish_Loads = (BOOL_TYPE)(buffer->payload[0] & 1);
                break;
            case API005LOWLEVEL_OC_EXTRACT_GIS:
                API005_Publish_Inputs = (BOOL_TYPE)(buffer->payload[0] & 1);
                break;
        #endif

        #if (API005_CONTROL_CYCLES_FEATURE == ENABLED)
            case API005LOWLEVEL_SET_TEST_STORAGE:
                TestMgr__SetStorage((BOOL_TYPE)(buffer->payload[0] & 1));
                break;
            case API005LOWLEVEL_ENGAGE_TEST_CYCLE:
                if (TestMgr__EngageTest(buffer->payload[0],(TESTMGR_CYCLE_TYPE)buffer->payload[1]) == FAIL)
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                break;
            case API005LOWLEVEL_PHASE_TEST_CYCLE:
                if (TestMgr__SetPhase(buffer->payload[0],(TESTMGR_PHASE_TYPE)buffer->payload[1]) == FAIL)
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                break;
            case API005LOWLEVEL_STEP_TEST_CYCLE:
                if (TestMgr__SetStep(buffer->payload[0],(TESTMGR_PHASE_TYPE)buffer->payload[1]) == FAIL)
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                break;
        #endif

        #if (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
            case API005LOWLEVEL_OC_HMI_SET_OBJECT:
                if (Mode__IsPCControlEngaged())
                {
                    //VISUAL_OBJECT_TYPE object_type, uint16 object_index, uint16 object_value
                    if (buffer->size == 5)
                    {
                        uint8 object_intensity = (Utilities__ConvertArrayTo16bits(&buffer->payload[3]) != 0) ? 100 : 0;
                        HblHmi__SetObject((VISUAL_OBJECT_TYPE)buffer->payload[0],
                                          Utilities__ConvertArrayTo16bits(&buffer->payload[1]),
                                          Utilities__ConvertArrayTo16bits(&buffer->payload[3]),
                                          object_intensity,
                        #if (HBLHMI_LED_PRIORITY_FEATURE == ENABLED)
                              HBLHMI_LED_PRIORITY_SYSTEM);
                        #else
                              HBLHMI_LED_PRIORITY_MAX);
                        #endif
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                break;
            case API005LOWLEVEL_OC_HMI_PLAY_SOUND:
                if (Mode__IsPCControlEngaged())
                {
                    if (buffer->size == 2)
                    {
                        HblHmi__PlaySoundSimple(buffer->payload[0],(SOUND_VOLUME_TYPE)buffer->payload[1]);
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
                break;
            case API005LOWLEVEL_OC_HMI_PUB_GI_DATA:
                //For GI ID - 0 to 254 it publishes the requested GI CONVERTED DATA
                if (buffer->payload[0] != 0xFF)
                {
                    gi_event.GI_Position = (buffer->payload[0])&(0x3F);    //0b00111111: Remove the READ TYPE bits
                    gi_event.GI_Read_Type = (HBLHMI_GI_READ_TYPE)(buffer->payload[0]>>6);
                    item_index = HblHmi__GetGIConfigIndexByType(gi_event.GI_Read_Type,gi_event.GI_Position);

                    if (item_index != HBL_GI_INVALID)    //GI in range, is a valid GI index
                    {
                        num_items = 1;
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    //For GI ID - 255 is used to request all GI CONVERTED DATA
                    num_items = HblHmi__GetNumGI();
                    item_index = 0;
                }

                if (retval == NVO_RS_READY)
                {
                    retval = PublishData(API005LOWLEVEL_OC_HMI_PUB_GI_DATA, item_index, num_items, buffer->node_id);
                }
                break;
            case API005LOWLEVEL_OC_HMI_PUB_LLI_DATA:
                //For LLI TYPE - 0 to 254 it publishes the requested LLI Data
                if (buffer->payload[0] != 0xFF)
                {
                    item_index = HblHmi__GetLLIIndexByID((HBL_LLI_ENUM_TYPE)buffer->payload[0], buffer->payload[1]);

                    if (item_index != HBL_LLI_INVALID)
                    {
                        num_items = 1;
                    }
                    else
                    {
                        retval = NVO_RS_UNSUP_INVALID_PARAM;
                    }
                }
                else
                {
                    //For LLI TYPE - 255 is used to publish all LLI Data
                    num_items = HblHmi__GetNumLLI();
                    item_index = 0;
                }

                if (retval == NVO_RS_READY)
                {
                    retval = PublishData(API005LOWLEVEL_OC_HMI_PUB_LLI_DATA, item_index, num_items, buffer->node_id);
                }
                break;

            case API005LOWLEVEL_OC_HMI_PUB_GI_CONFIG:
                 retval = PublishData(API005LOWLEVEL_OC_HMI_PUB_GI_CONFIG, 0, HblHmi__GetNumLLI(), buffer->node_id);
                 break;

             case API005LOWLEVEL_OC_HMI_PUB_LLI_CONFIG:
                 retval = PublishData(API005LOWLEVEL_OC_HMI_PUB_LLI_CONFIG, 0, HblHmi__GetNumGI(), buffer->node_id);
                 break;
        #endif
        case API005LOWLEVEL_OC_CALL_DEBUG_FUNCTION:
            if ((buffer->payload[0] < API005_NUM_DEBUG_FUNCTIONS) &&
                (API005LOWLEVEL_DEBUG_TABLE[buffer->payload[0]].Function != NULL_TASK))
            {
                API005LOWLEVEL_DEBUG_TABLE[buffer->payload[0]].Function();
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API005LOWLEVEL_OC_DEBUG_LABEL:
            if (buffer->payload[0] < API005_NUM_DEBUG_FUNCTIONS)
            {
                ptr = Reveal__NewFeedback(API005LOWLEVEL_NUM, API005LOWLEVEL_OC_DEBUG_LABEL, REVEAL_BROADCAST_NODE, API005_SIZE_OF_LABEL + 1);
                if (ptr == (REVEAL_COMMAND_TYPE *)NULL)
                {
                    retval = NVO_RS_UNSUPPORTED;
                }
                else
                {
                    (void)Reveal__AddDataU8(ptr, buffer->payload[0]);
                    (void)Reveal__AddDataU8Buffer(ptr, (uint8 *)API005LOWLEVEL_DEBUG_TABLE[buffer->payload[0]].Label, API005_SIZE_OF_LABEL);    //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    (void)Reveal__QueueCommand(ptr);
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API005LOWLEVEL_OC_SET_DEBUG_VARIABLE:
            API005LOWLEVEL_SET_DEBUG_VAR(buffer->payload[0],Utilities__ConvertArrayTo32bits(&buffer->payload[1]));
            break;
        default:
            retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }
    return (retval);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method publish events related to API005 as they happens.
 * @param event
 */
void API005LowLevel__PublishEvent(uint8 * data, uint8 size)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewFeedback(API005LOWLEVEL_NUM,API005LOWLEVEL_OC_PUBLISH_EVENT,REVEAL_BROADCAST_NODE,size);
    if (ptr != (REVEAL_COMMAND_TYPE *)NULL)
    {
        Reveal__AddDataU8Buffer(ptr,data,size);
        Reveal__QueueCommand(ptr);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method shall be called every 5ms from the Communication Thread
 */
void API005LowLevel__Handler(void)
{
    #if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
        API005_Timer_Divider++;
        if (API005_Timer_Divider > 8)
        {
            API005_Timer_Divider = 0;

            API005_Timer++;

            uint8 num_loads = Hbl__GetNumLoads();
            uint8 num_gis =  Hbl__GetNumGI();

            if (API005_Publish_Loads == TRUE)
            {
                if ( (API005_Timer >= 50) &&
                     (API005_Timer < (50 +num_loads))
                   )
                {
                    PublishData(API005LOWLEVEL_OC_GET_PUB_LOAD_DATA, API005_Timer-50, 1, REVEAL_BROADCAST_NODE);
                }
            }

            if (API005_Publish_Inputs == TRUE)
            {
                if ( (API005_Timer >= (50 +num_loads)) &&
                     (API005_Timer < ((50 +num_loads) + num_gis))
                   )
                {
                    PublishData(API005LOWLEVEL_OC_GET_PUB_GI_DATA, API005_Timer - 50 - num_loads, 1, REVEAL_BROADCAST_NODE);
                }
            }
            if (API005_Timer >= ((50 +num_loads) + num_gis))
            {
                API005_Timer = 0;
            }
        }
    #else

    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if ((API005_USE_LOADS_AND_GIS_FEATURE == ENABLED) || (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED))
//----------------------------------------------------------------------------------------------------------
/**
 * @brief Method used to publish the data or configuration related to Load, GI or LLI
 * @param oc
 * @param item_index
 * @param num_items
 * @param node_id
 * @return REVEAL_RECIPE_STATUS_TYPE
 */
static REVEAL_RECIPE_STATUS_TYPE PublishData(API005LOWLEVEL_OC_TYPE oc, uint8 item_index, uint8 num_items, uint8 node_id)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    retval = NVO_RS_READY;
    REVEAL_COMMAND_TYPE * ptr;

    //The below logic sends multiple messages if all the data does not fit in one message
    do
    {
        ptr = Reveal__NewFeedback(API005LOWLEVEL_NUM, oc, node_id, (REVEAL_PAYLOAD_SIZE - 4));
        if (ptr == (REVEAL_COMMAND_TYPE *)NULL)
        {
            retval = NVO_RS_UNSUPPORTED;
            break;
        }
        else
        {
            //publish_data_function_index is the index PUBLISH_DATA_FUNCTION_PTR constant
            //the offsets are calculated depending on the Features that are enabled i.e. function pointers added to the table
            uint8 publish_data_function_index = oc - API005LOWLEVEL_OC_GET_PUB_LOAD_DATA;     //this is mapped by default to API005_USE_LOADS_AND_GIS_FEATURE

            #if ((API005_USE_LOADS_AND_GIS_FEATURE == ENABLED) && (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED))
                if (oc >= API005LOWLEVEL_OC_HMI_PUB_GI_DATA)
                {
                    publish_data_function_index =    oc - (API005LOWLEVEL_OC_HMI_PUB_GI_DATA - PUBLISH_DATA_NUM_OF_LOADs_AND_GI_OPCODES);
                }
            #elif (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
                if (oc >= API005LOWLEVEL_OC_HMI_PUB_GI_DATA)
                {
                    publish_data_function_index =    oc - API005LOWLEVEL_OC_HMI_PUB_GI_DATA;
                }
            #endif

            //populate the reveal message with the load data
            for (uint8 payload_data_counter = 0; num_items > 0; num_items--)
            {
                uint8 data_size = PUBLISH_DATA_FUNCTION_PTR[publish_data_function_index].GetDataSizeByIndex(item_index);

                //Determine is this item data could be fit in the message or not
                payload_data_counter += PUBLISH_DATA_FUNCTION_PTR[publish_data_function_index].Non_ItemData_Size + data_size;

                if (payload_data_counter > REVEAL_PAYLOAD_SIZE)
                {
                    break;
                }

                PUBLISH_DATA_FUNCTION_PTR[publish_data_function_index].BuildMessage(item_index, data_size, ptr);

                item_index++;
            }

            if (num_items != 0)
            {
                //set the mmp bit if their are more than one message needs to be send
                ptr->opcode |= REVEAL_MMP_BIT;
            }

            Reveal__QueueCommand(ptr);
        }
    }while(num_items != 0);

    return (retval);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Method returns the return size as 0
 *
 * @param index
 * @return
 */
static uint8 GetDummyDataSize(uint8 index)
{
    return(0);
}
#endif

#if (API005_USE_LOADS_AND_GIS_FEATURE == ENABLED)
//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for the load data
 * @details -
 *     Payload[0]  = LOAD IDx
 *     Payload[1]  = LOAD CONSISTENCY
 *     Payload[2]  = LOAD STATUS
 *     Payload[3]  = LOAD DATA SIZE
 *     Payload[4]  = LOAD DATA[0]
 *     Payload[5]  = LOAD DATA[n-1]
 *     Payload[6]  = LOAD IDy
 *     Payload[7]  = LOAD CONSISTENCY
 *     Payload[8]  = LOAD STATUS
 *     Payload[9]  = LOAD DATA SIZE
 *     Payload[10] = LOAD DATA[0]
 *     Payload[11] = LOAD DATA[n-1]
 *
 * @param item_index
 * @param ptr
 * @return
 */
static void BuildLoadDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    Reveal__AddDataU8(ptr,Hbl__GetLoadIDByIndex(item_index));
    Reveal__AddDataU8(ptr, Hbl__GetLoadConsistency(item_index));
    Reveal__AddDataU8(ptr, *(uint8 *)Hbl__GetLoadStatusByIndex(item_index));
    Reveal__AddDataU8(ptr, data_size);
    Reveal__AddDataU8Buffer(ptr, Hbl__GetProcessedLoad(item_index), data_size);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for the LLI data
 * @details -
 *   Payload[0] = LLI TYPEx
 *   Payload[1] = LLI POSITIONx
 *   Payload[2] = LLI DATA SIZE
 *   Payload[3] = LLI DATA[0]
 *   Payload[4] = LLI DATA[n-1]
 *   Payload[5] = LLI TYPEy
 *   Payload[6] = LLI POSITIONy
 *   Payload[7] = LLI DATA SIZE
 *   Payload[8] = LLI DATA[0]
 *   Payload[9] = LLI DATA[n-1]
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildLLIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    Reveal__AddDataU8(ptr,Hbl__GetLLITypeByIndex(item_index));
    Reveal__AddDataU8(ptr,Hbl__GetLLIPositionByIndex(item_index));
    Reveal__AddDataU8(ptr, data_size);
    Reveal__AddDataU8Buffer(ptr, Hbl__GetLLIDataByIndex(item_index), data_size);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for the GI data
 * @details -
 *   Payload[0]   = GI IDx
 *   Payload[1-4] = GI CONVERTED DATA
 *   Payload[5]   = GI IDy
 *   Payload[6-9] = GI CONVERTED DATA
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildGIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    Reveal__AddDataU8(ptr,Hbl__GetGIIDByIndex(item_index));
    Reveal__AddDataU32(ptr, Hbl__GetGIDataConvertedByIndex(item_index));
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for all the load configuration
 * @details -
 *   Payload[0]     = NUM OF LOADS n
 *   Payload[n+1]   = LOAD ID[n-1]
 *   Payload[n+2]   = PILOT TYPE[n-1]
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildAllLoadConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    if (item_index == 0)
    {
        Reveal__AddDataU8(ptr,Hbl__GetNumLoads());
    }

    Reveal__AddDataU8(ptr,Hbl__GetLoadIDByIndex(item_index));
    Reveal__AddDataU8(ptr,Hbl__GetPilotIDByIndex(item_index));
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for all the LLI configuration
 * @details -
 *   Payload[0]     = NUM OF LLI
 *   Payload[n+1]   = LLI ID[n-1]
 *   Payload[n+2]   = LLI POSITION[n-1]
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildAllLLIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    if (item_index == 0)
    {
        Reveal__AddDataU8(ptr,Hbl__GetNumLLI());
    }

    Reveal__AddDataU8(ptr,Hbl__GetLLITypeByIndex(item_index));
    Reveal__AddDataU8(ptr,Hbl__GetLLIPositionByIndex(item_index));
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for all the GI configuration
 * @details -
 *   Payload[0]     = NUM OF GI n
 *   Payload[n+1]   = GI ID[n-1]
 *   Payload[n+2]   = LLI ID[n-1]
 *   Payload[n+3]   = LLI POSITION[n-1]
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildAllGIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    if (item_index == 0)
    {
        Reveal__AddDataU8(ptr,Hbl__GetNumGI());
    }

    Reveal__AddDataU8(ptr,Hbl__GetGIIDByIndex(item_index));
    Reveal__AddDataU8(ptr,Hbl__GetLLITypeByGIIndex(item_index));
    Reveal__AddDataU8(ptr,Hbl__GetLLIPositionByGIIndex(item_index));
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Method returns the return size of the GI Converted data
 *
 * @param index
 * @return
 */
static uint8 HblGetGIConvertedDataSizeByIndex(uint8 index)
{
    return(4);
}

#endif

#if (API005_USE_OBJECTS_AND_EVENTS_FEATURE == ENABLED)
//----------------------------------------------------------------------------------------------------------
/**
 * @brief Event handler for Hmi when in PC Control Mode
 * @param event
 */
void API005ReceiveEvent(HBLHMI_GI_EVENT_TYPE event)
{

    if (Mode__IsPCControlEngaged())
    {
        API005LowLevel__PublishEvent((uint8 *)(&event), 1);
    }

}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Builds the reveal message for HMI GI data
 * @details
 *   Payload[0] = GI READ_TYPE and GI POSITION
 *   Payload[1] = GI DATA SIZE (n)
 *   Payload[2] = GI DATA[0]
 *   Payload[3] = GI DATA[n-1]
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildHmiGIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    uint8 gi_id;
    gi_id = HblHmi__GetGIPositionByIndex(item_index);
    gi_id = gi_id | (HblHmi__GetGIReadTypeByIndex(item_index)<< 6);

    // Form the message
    Reveal__AddDataU8(ptr,gi_id);
    Reveal__AddDataU8(ptr, data_size);
    Reveal__AddDataU8Buffer(ptr, HblHmi__GetGIByIndex(item_index), data_size);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @details
 *   Payload[0] = LLI TYPEx
 *   Payload[1] = LLI POSITIONx
 *   Payload[2] = LLI DATA SIZE (n)
 *   Payload[3] = LLI DATA[0]
 *   Payload[4] = LLI DATA[n-1]
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildHmiLLIDataMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    Reveal__AddDataU8(ptr,HblHmi__GetLLIIDByIndex(item_index));
    Reveal__AddDataU8(ptr,HblHmi__GetLLIPositionByIndex(item_index));
    Reveal__AddDataU8(ptr, data_size);
    Reveal__AddDataU8Buffer(ptr, HblHmi__GetLLIDataByIndex(item_index), data_size);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for all the LLI configuration
 * @details -
 *   Payload[0]     = NUM OF LLI
 *   Payload[n+1]   = LLI ID[n-1]
 *   Payload[n+2]   = LLI POSITION[n-1]
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildHmiAllLLIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    // Form the message
    if (item_index == 0)
    {
        Reveal__AddDataU8(ptr,HblHmi__GetNumLLI());
    }

    Reveal__AddDataU8(ptr,HblHmi__GetLLIIDByIndex(item_index));
    Reveal__AddDataU8(ptr,HblHmi__GetLLIPositionByIndex(item_index));
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief - This function builds the reveal message for all the GI configuration
 * @details -
 *   Payload[0]     = NUM OF GI n
 *   Payload[n+1]   = GI READ_TYPE and GI POSITION
 *   Payload[n+2]   = GI ID[n-1]
 *   Payload[n+3]   = LLI ID[n-1]
 *   Payload[n+4]   = LLI POSITION[n-1]
 *
 * @param item_index
 * @param data_size
 * @param ptr
 */
static void BuildHmiAllGIConfigurationMessage(uint8 item_index, uint8 data_size, REVEAL_COMMAND_TYPE * ptr)
{
    uint8 gi_id;
    gi_id = HblHmi__GetGIPositionByIndex(item_index);
    gi_id = gi_id | (HblHmi__GetGIReadTypeByIndex(item_index)<< 6);

    // Form the message
    if (item_index == 0)
    {
        Reveal__AddDataU8(ptr,HblHmi__GetNumGI());
    }
    Reveal__AddDataU8(ptr,gi_id);
    Reveal__AddDataU8(ptr,HblHmi__GetGIIDByIndex(item_index));
    Reveal__AddDataU8(ptr,HblHmi__GetLLIIDByGIIndex(item_index));
    Reveal__AddDataU8(ptr,HblHmi__GetLLIPositionByGIIndex(item_index));
}

#endif

#endif  //API005LOWLEVEL_FEATURE
