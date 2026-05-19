/**
 *  @file
 *
 *  @brief      Implements API 012 expansion module.
 *
 *  @copyright  Copyright 2015 - 2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (API012EXP_FEATURE == ENABLED)
#include "API012Exp.h"
#include "API012Exp_prv.h"

#include "Log.h"
#include "Mode.h"
#include "Utilities.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))
    #include "HblHmi.h"
    #include "LedAnimationMgr.h"
    #if(FEATURE_DYNAMIC_LED_GROUPS == ENABLED)
        #include "LedDynamicGroups.h"
    #endif

    #if(FEATURE_DYNAMIC_ANIMATIONS == ENABLED)
        #include "LedDynamicAnimations.h"
    #endif
#endif

//! ENABLED if the system contains an ACU, else DISABLED
#if ( (SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING  == ENABLED) )
    #include "Hbl.h"
#endif



#ifndef API012EXP_HMI_EVENT_BASED
    #define API012EXP_HMI_EVENT_BASED   DISABLED
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Reveal operation code definitions for API 012 (Expansion).
typedef enum
{
    API012EXP_REQ_ACU_PUB_LOADS_STATUS  =   1,
    API012EXP_SET_ACU_LOADS             =   2,
    API012EXP_REQ_ACU_PUB_GIS           =   3,

    API012EXP_CREATE_LED_GROUP          =   8,
    API012EXP_CREATE_LED_ANIMATION      =   9,
    API012EXP_REQ_HMI_PUB_GIS           =   10,
    API012EXP_SET_VIEW_ELEMENTS         =   11,
    API012EXP_PLAY_SOUND                =   12,
    API012EXP_SET_VOLUME                =   13,
    API012EXP_STOP_SOUND                =   14,
    API012EXP_PLAY_ANIMATION            =   15,
    API012EXP_STOP_ANIMATION            =   16,

    API012EXP_SET_TEST_STATUS           =   17,

    API012EXP_HMI_GI_EVENT              =   18,
    API012EXP_HMI_LED_EVENT             =   19,
    API012EXP_HMI_LAST_STORED_GI_EVENT  =   20
} API012EXP_OPCODE_TYPE;

//! Handler time value to publish Loads, Load Status, GI's and LED's
#define API012_TIMER_COMPARE    40      // 1s on 25ms timebase

#define RESERVE_FIRST_BYTE_TO_STORE_NUM_OF_GI_INFORMATION       1

#define API012_GI_PAYLOAD_SIZE                                  (sizeof(sint32))

//! Set LED commands
#define LED_CMD_SET_STATE   0
#define LED_CMD_SET_ALL     1

#if (API0012EXP_USE_PRIORITY_QUEUE == ENABLED)
        #define API012REVEAL__NEWFEEDBACK(api,opcode,node,size)   Reveal__PriorityNewFeedback(api,opcode,node,size)
        #define API012REVEAL__NEWCOMMAND(api,opcode,node,size)    Reveal__PriorityNewCommand(api,opcode,node,size)
#else
        #define API012REVEAL__NEWFEEDBACK(api,opcode,node,size)   Reveal__NewFeedback(api,opcode,node,size)
        #define API012REVEAL__NEWCOMMAND(api,opcode,node,size)    Reveal__NewCommand(api,opcode,node,size)
#endif

//! Timer used by handler to publish.
static uint8 API012_Timer;

//! Timer used to detect loss of communications.
static uint16 API012_Comm_Timeout;

//If this board handles another board that is expansion ie SYSTEM_HMI_HANDLING ENABLED
// or if this board is an expansion HMI not using the event based opcodes
#if ( (SYSTEM_HMI_HANDLING == ENABLED) || ( (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED) ) )
    static uint8 API12_Buffer[10];
    static uint8 API12_Buffer_Digital[10];

    #if (API012EXP_NUM_OF_EXTERNAL_LEDS != 0)
        static uint8 API12_Led_Buffer[API012EXP_NUM_OF_EXTERNAL_LEDS];
    #endif
#endif

#if (SYSTEM_HMI_HANDLING == ENABLED)
    //! Buffer to hold the HMI GI data for encoder types
    //! HMI_EXP_GI_ENCODER_BUFFER_SIZE should be defined in API012Exp_prv.h
    static API12_ENCODER_BUFFER_TYPE API12_Encoder_Buffer[HMI_EXP_GI_ENCODER_BUFFER_SIZE];
#endif

#if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED) )
    static uint8 AP12_Hmi_Num_GI_Digital;
    static uint8 AP12_Hmi_Num_GI_Analog;
    static uint8 AP12_Hmi_Num_GI_Encoder;
#endif

#if ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) || (SYSTEM_ACU_HANDLING == ENABLED))
    #ifndef API12_ACU_MAX_LOADS
        #define API12_ACU_MAX_LOADS 10
    #endif

    static uint8 AP12_ACU_Num_Loads_Received;
    static uint8 API12_ACU_Loads_Buffer[API12_ACU_MAX_LOADS];

    //! Buffer to hold expansion boards load status
    static uint8 API12_ACU_Loads_Status_Buffer[API12_ACU_MAX_LOADS];

    static BOOL_TYPE Test_Cycle_Engaged;
#endif

#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
    #define ACU_EXP_GI_BUFFER_SIZE                                  (   RESERVE_FIRST_BYTE_TO_STORE_NUM_OF_GI_INFORMATION +       \
                                                                        (HBL_MAX_NUM_GENERICINPUT * (API012_GI_PAYLOAD_SIZE+1))   \
                                                                    )
    static uint8 AP12_ACU_GI_Data_Size;

    //! Array of GI ID that will ship the converted value over reveal. Use HBL_GI_INVALID on the constant if you don't have any.
    static uint8 API12_Expansion_ACU_GI_Buffer[ACU_EXP_GI_BUFFER_SIZE];
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #ifndef API12_ACU_MAX_GIS
        #define API12_ACU_MAX_GIS 1
    #endif

    #define ACU_GI_BUFFER_SIZE                                      (   RESERVE_FIRST_BYTE_TO_STORE_NUM_OF_GI_INFORMATION +             \
                                                                        (API12_ACU_MAX_GIS * (API012_GI_PAYLOAD_SIZE+1))   \
                                                                    )


    typedef struct API012EXP_BUS_CONFIG_STRUCT
    {
        uint8 Address       : 4;
        uint8 unused        : 4;
        uint8 Num_Loads     : 5;
        uint8 unused_1      : 3;
        uint8 Num_Gis       : 5;
        uint8 unused_2      : 3;
    } API012EXP_BUS_CONFIG_TYPE;

    //! Track the number of expansion boards from the Setting File.
    static uint8 Number_Of_Expansion_Boards;

    //! Pointer to the Bus Configuration in the Setting File.
    static API012EXP_BUS_CONFIG_TYPE *Bus_Configuration_Ptr;

    typedef struct API12_ACU_GI_STRUCT
    {
        uint8 Node;
        uint8 Buffer[ACU_GI_BUFFER_SIZE];
    } API12_ACU_GI_TYPE;

    #ifndef API012EXP_MAX_BOARDS
        #define API012EXP_MAX_BOARDS 1
    #endif
    //! Buffer to hold expansion board GI's
    static API12_ACU_GI_TYPE API12_ACU_GI[API012EXP_MAX_BOARDS];
#endif

#define HMI_GI_EVENT_HEADER_LENGHT (3)    // GI type + GI index + GI data size
#define API012_OP20_MAX_DATA_SIZE     2       //Only GI read type and GI index needs to be publish
    static  HBLHMI_GI_EVENT_TYPE Last_GI_Event;        //holds last event info

// --- LOG MODULE SETTINGS ---------------------------
//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_API012EXPANSION

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_API012EXPANSION_LOG_MESSAGE_ID_ENUM
{
    INVALID_CONFIGURATION_PARAMETERS        = 1,    //!< Invalid Setting File Data
    CONFIGURATION_PARAMETERS_NOT_FOUND      = 2,    //!< Expansion boards setting file configuration not found.
    QUEUEING_ON_REVEAL                      = 3     //!< Expansion board not able to queue a reveal message
} MODULE_API012EXPANSION_LOG_MESSAGE_ID_TYPE;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
    #if (API012EXP_HMI_EVENT_BASED == DISABLED)
        static void PublishHMIGIs(uint8 address);
    #else
        static void HmiGIEventHandler(HBLHMI_GI_EVENT_TYPE event);
        static void HmiGIGetAndPublish(HBLHMI_GI_READ_TYPE read_type, uint8 gi_position);
        static REVEAL_RECIPE_STATUS_TYPE LedCommandParser(REVEAL_MSG_TYPE * buffer);
        static REVEAL_RECIPE_STATUS_TYPE GiCommandParser(REVEAL_MSG_TYPE * buffer);
        static void PublishLastGiEvent(void);
        static void ClearLastGiEventInfo(void);
    #endif
#endif
#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
    static BOOL_TYPE UpdateAcuGiStatus(void);
    static BOOL_TYPE UpdateAcuLoadStatus(void);
    static void PublishACUGIs(uint8 node_id);
    static void PublishACULoadsStatus(uint8 node_id);
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED)
    static void PublishACULoads(uint8 address,uint8 * data, uint8 size);
    static void PublishTestCycleEngaged(void);
    static void ReadAcuExpansionConfiguration(void);
    static uint8 *GetBufferGIPtr(uint8 index);
#endif


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module API012Exp and its variables
 *
 */
void API012Exp__Initialize(void)
{
    API012_Timer = 0;
    API012_Comm_Timeout = 0;
    #if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
        #if (API012EXP_HMI_EVENT_BASED == ENABLED)
            ClearLastGiEventInfo();
        #endif
    #endif

    #if ( (SYSTEM_HMI_HANDLING == ENABLED) || ( (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED) ) )
        memset(API12_Buffer, 0, sizeof(API12_Buffer));
        #if ((API012EXP_NUM_OF_EXTERNAL_LEDS != 0)&&(API012EXP_HMI_EVENT_BASED == DISABLED))
            memset(API12_Led_Buffer, 0, sizeof(API12_Led_Buffer));
        #endif
        memset(API12_Buffer_Digital, 0, sizeof(API12_Buffer_Digital));
    #endif

    #if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
        #if (API012EXP_HMI_EVENT_BASED == DISABLED)
            AP12_Hmi_Num_GI_Digital = HblHmi__GetNumGIReadType(HBLHMI_GI_READ_DIGITAL);
            if (BIT_STORAGE_SIZE(AP12_Hmi_Num_GI_Digital) > sizeof(API12_Buffer))
            {
                AP12_Hmi_Num_GI_Digital = (sizeof(API12_Buffer) * 8);
            }

            AP12_Hmi_Num_GI_Analog = HblHmi__GetNumGIReadType(HBLHMI_GI_READ_ANALOG);
            AP12_Hmi_Num_GI_Encoder = HblHmi__GetNumGIReadType(HBLHMI_GI_READ_ENCODER);
        #else
            HblHmi__RegisterGIEventHandler(&HmiGIEventHandler);
        #endif
    #endif

    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        memset(API12_ACU_Loads_Buffer, 0, sizeof(API12_ACU_Loads_Buffer));
        memset(API12_ACU_Loads_Status_Buffer, 0, sizeof(API12_ACU_Loads_Status_Buffer));
        memset(API12_Expansion_ACU_GI_Buffer, 0, sizeof(API12_Expansion_ACU_GI_Buffer));

        //GI Data size
        uint8 num_gi = Hbl__GetNumGI();

        AP12_ACU_GI_Data_Size = RESERVE_FIRST_BYTE_TO_STORE_NUM_OF_GI_INFORMATION + (num_gi * (API012_GI_PAYLOAD_SIZE+1));
    #endif

    #if (SYSTEM_ACU_HANDLING == ENABLED)
        AP12_ACU_Num_Loads_Received = 0;
        memset(API12_ACU_GI, 0, sizeof(API12_ACU_GI));
        Test_Cycle_Engaged = FALSE;
        ReadAcuExpansionConfiguration();
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses messages target to the API017 sent to a broadcast or exclusively to this node for Reveal 4.x.
 * @param buffer contains the Wide package that contains the message
 * @return REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API012Exp__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    #if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
        uint32 pattern;
    #endif

    #if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED))
        uint16 offset;
        uint8 object_index;
    #endif

    retval = NVO_RS_READY;

    switch ((API012EXP_OPCODE_TYPE)buffer->op_code)
    {
        #if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)

        #if(FEATURE_DYNAMIC_LED_GROUPS == ENABLED)
            case API012EXP_CREATE_LED_GROUP:
                /*
                 payload[0] = index of the new LED group
                 payload[1] = group length
                 &payload[2] = pointer to the list of LEDs to be assigned to the new group
                 */
                if(FALSE == LedDynamicGroups__CreateGroup(buffer->payload[0], buffer->payload[1], &(buffer->payload[2])))
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
                break;
        #endif

        #if(FEATURE_DYNAMIC_ANIMATIONS == ENABLED)
            case API012EXP_CREATE_LED_ANIMATION:
                /*
                 payload[0] = index of the new LED animation
                 &payload[1] = pointer to the structure containing new animation paramEters
                              (identical to THE setting file animation structrure)
                 */
                if(FALSE == LedDynamicAnimations__CreateAnimation(buffer->payload[0], &(buffer->payload[1])))
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
                break;
        #endif

        #if (API012EXP_HMI_EVENT_BASED == DISABLED)
            case API012EXP_REQ_HMI_PUB_GIS:
                PublishHMIGIs(buffer->node_id);
                break;

            case API012EXP_SET_VIEW_ELEMENTS:
                offset = Utilities__ConvertArrayTo16bits(buffer->payload);

                for (object_index = 2; object_index < buffer->size; object_index++)
                {
                    HblHmi__SetObject(VISUAL_OBJECT_LED,
                                      offset + object_index - 2,
                                      1,
                                      (uint16)buffer->payload[object_index],HBLHMI_LED_PRIORITY_EMBEDDED);
                }
                break;
        #else
            case API012EXP_HMI_GI_EVENT:
                retval = GiCommandParser(buffer);
                break;
            case API012EXP_HMI_LED_EVENT:
                retval = LedCommandParser(buffer);
                break;
            case API012EXP_HMI_LAST_STORED_GI_EVENT:
                PublishLastGiEvent();
                break;
        #endif

            case API012EXP_PLAY_SOUND:
                HblHmi__PlaySoundSimple(buffer->payload[0], (SOUND_VOLUME_TYPE)buffer->payload[1]);
                break;

            case API012EXP_SET_VOLUME:
                HblHmi__SetVolume((SOUND_VOLUME_TYPE)buffer->payload[0]);
                break;

            case API012EXP_STOP_SOUND:
                HblHmi__StopAllSound();
                break;

            case API012EXP_PLAY_ANIMATION:
                pattern = Utilities__ConvertArrayTo32bits(&(buffer->payload[2]));
                LedAnimationMgr__Start((LED_ANIMATIONS_ENUM_TYPE)buffer->payload[0], (uint16)(buffer->payload[1]), pattern, buffer->payload[6]);
                break;

            case API012EXP_STOP_ANIMATION:
                if(buffer->payload[0] != 0xFF)
                {
                    pattern = Utilities__ConvertArrayTo32bits(&(buffer->payload[1]));
                    LedAnimationMgr__Stop(buffer->payload[0], pattern);
                }
                else
                {
                    LedAnimationMgr__StopAll();
                }
                break;
        #endif  //(SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)

        #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
            case API012EXP_REQ_ACU_PUB_GIS:
                PublishACUGIs(buffer->node_id);
                break;
        #endif

        default:
            retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (retval);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses feedbacks to the API012Exp sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message
 */
void API012Exp__FeedbackParser(REVEAL_MSG_TYPE * buffer)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        uint8 board_index;
        uint8 load_status_index;
    #endif
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        #if(SYSTEM_EXPANSION_HANDLING == ENABLED)
            uint8 encoder_index;
        #endif
        uint8 size;
    #endif
    #if ((SYSTEM_HMI_HANDLING == ENABLED) && (SYSTEM_EXPANSION_HANDLING == ENABLED))
        HBLHMI_GI_TYPE *gi_info_ptr;
    #endif

    #if (SYSTEM_ACU_HANDLING == ENABLED)
        load_status_index = 0;

        // Find the bus configuration of the board that is sending feedback.
        for (board_index = 0; board_index < Number_Of_Expansion_Boards; board_index++)
        {
            if (buffer->node_id == Bus_Configuration_Ptr[board_index].Address)
            {
                break;
            }
            else
            {
                // Find the load status buffer index of the board that is sending feedback.
                load_status_index += Bus_Configuration_Ptr[board_index].Num_Loads;
            }
        }
    #endif

    switch ((API012EXP_OPCODE_TYPE)buffer->op_code)
    {
        #if (SYSTEM_ACU_EXPANSION_HANDLING)
            case API012EXP_SET_ACU_LOADS:
                if (buffer->size <= API12_ACU_MAX_LOADS)
                {
                    API012_Comm_Timeout = 120; //3s at 25ms
                    AP12_ACU_Num_Loads_Received = buffer->size;
                    memcpy(API12_ACU_Loads_Buffer,buffer->payload,AP12_ACU_Num_Loads_Received);
                }
                else
                {
                    AP12_ACU_Num_Loads_Received = 0;
                }
                break;
        #endif

        #if (SYSTEM_ACU_HANDLING == ENABLED)
            case API012EXP_REQ_ACU_PUB_GIS:
                if (board_index < Number_Of_Expansion_Boards)
                {
                    API12_ACU_GI[board_index].Node = buffer->node_id;
                    if ((buffer->size <= sizeof(API12_ACU_GI[board_index].Buffer)))
                    {
                        memcpy(API12_ACU_GI[board_index].Buffer, buffer->payload, buffer->size);
                    }
                }
                break;
        #else
            case API012EXP_REQ_ACU_PUB_GIS:
                API012EXP__PARSE_GI(buffer->node_id, buffer->payload, buffer->size);
                break;
        #endif

        #if (SYSTEM_ACU_HANDLING == ENABLED)
            case API012EXP_REQ_ACU_PUB_LOADS_STATUS:
                if (buffer->size <= API12_ACU_MAX_LOADS)
                {
                    if (board_index < Number_Of_Expansion_Boards)
                    {
                        API012_Comm_Timeout = 120; //3s at 25ms
                        AP12_ACU_Num_Loads_Received = buffer->size;
                        if (AP12_ACU_Num_Loads_Received > Bus_Configuration_Ptr[board_index].Num_Loads)
                        {
                            AP12_ACU_Num_Loads_Received = Bus_Configuration_Ptr[board_index].Num_Loads;
                        }
                        if ((AP12_ACU_Num_Loads_Received + load_status_index) <= API12_ACU_MAX_LOADS)
                        {
                            memcpy(API12_ACU_Loads_Status_Buffer + load_status_index, buffer->payload, AP12_ACU_Num_Loads_Received);
                        }
                    }
                }
                else
                {
                    AP12_ACU_Num_Loads_Received = 0;
                }
                break;

        #endif

        #if (SYSTEM_HMI_HANDLING == ENABLED)
            case API012EXP_HMI_GI_EVENT:
                #if(SYSTEM_EXPANSION_HANDLING == ENABLED)
                    gi_info_ptr  =  HblHmi__GetGITypeByGidata((HBLHMI_GI_READ_TYPE)buffer->payload[0], buffer->payload[1]);
                    encoder_index = gi_info_ptr->LLI_position;          // LLI position provides unique and deterministic number that can be used as index into the API12_Encoder_Buffer variable
                    if (encoder_index < HMI_EXP_GI_ENCODER_BUFFER_SIZE)
                    {
                        API12_Encoder_Buffer[encoder_index].GI_Read_Type = (HBLHMI_GI_READ_TYPE)(buffer->payload[0]);
                        API12_Encoder_Buffer[encoder_index].GI_Position = (uint8)(buffer->payload[1]);
                        API12_Encoder_Buffer[encoder_index].GI_Data_Size = (uint8)(buffer->payload[2]);
                        API12_Encoder_Buffer[encoder_index].Current_Enc_Value = (uint8)(buffer->payload[3]);
                        API12_Encoder_Buffer[encoder_index].Previous_Enc_Value = (uint8)(buffer->payload[4]);
                        API12_Encoder_Buffer[encoder_index].Direction = (HBL_ENC_DIRECTION_TYPE)(buffer->payload[5]);
                    }
                #endif
                break;
            case API012EXP_REQ_HMI_PUB_GIS:
                size = BIT_STORAGE_SIZE(buffer->payload[0]);    // Size for Digital GIs will be number of bits
                if (size > sizeof(API12_Buffer_Digital))
                {
                    size = sizeof(API12_Buffer_Digital);
                }
                if (size != 0)
                {
                    API012_Comm_Timeout = 120; //3s at 25ms
                    memcpy(API12_Buffer_Digital, &buffer->payload[1], size);
                }
                else
                {
                    memset(API12_Buffer_Digital, 0, sizeof(API12_Buffer_Digital));
                }
                break;
            case API012EXP_HMI_LAST_STORED_GI_EVENT:
                {
                    #ifdef API012EXP_HMI_LAST_EVENT_EXECUTE
                        HBLHMI_GI_EVENT_TYPE last_gi_event_info;
                        last_gi_event_info.GI_Read_Type = (HBLHMI_GI_READ_TYPE)buffer->payload[0];
                        last_gi_event_info.GI_Position = (uint8)buffer->payload[1];
                        API012EXP_HMI_LAST_EVENT_EXECUTE(last_gi_event_info);
                    #endif
                }
                break;
        #endif

        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method handles callbacks from Reveal when an ACK is received for this API
 * @param address
 * @param api
 * @param opcode
 * @param reasoncode
 */
void API012Exp__AckParser(uint8 address,uint8 api,uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode)
{
    switch (reasoncode)
    {
        case NVO_RS_READY:
        case NVO_RS_BUSY:
        case NVO_RS_REJECTED:
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method handle processes for the API012Exp on 25ms time bases.
 */
void API012Exp__Handler25ms(void)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        uint8 board_index;
        uint8 offset;
    #endif

    #if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED))
        uint8 gi_index;
        uint8 *pt_data;
    #endif


    #if ((SYSTEM_HMI_HANDLING == ENABLED) && ((API012EXP_NUM_OF_EXTERNAL_LEDS != 0)))
        REVEAL_COMMAND_TYPE * ptr;
        uint8 size;
    #endif

    if ((Mode__GetMode() == MODE_NORMAL) && (Mode__GetRemoteMode() == MODE_NORMAL))
    {
        if (API012_Comm_Timeout > 0)
        {
            API012_Comm_Timeout--;
        }
        else
        {
            #if (SYSTEM_HMI_HANDLING == ENABLED)
                    memset(API12_Buffer_Digital, 0, sizeof(API12_Buffer_Digital));
            #endif

            #if ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) || (SYSTEM_ACU_HANDLING == ENABLED))
                    AP12_ACU_Num_Loads_Received = 0;
            #endif
        }

        #if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED))
            // For the Expansion Board, here is where we publish inputs to the Main Hmi
            if (WIN_UI_ADDRESS < REVEAL_BROADCAST_NODE)
            {

                // HMI Digital Data processing
                for (gi_index = 0; gi_index < AP12_Hmi_Num_GI_Digital; gi_index++)
                {
                    pt_data = HblHmi__GetGIData(HBLHMI_GI_READ_DIGITAL, gi_index);
                    if (pt_data != NULL)
                    {
                        Utilities__SetBitInArray(API12_Buffer, gi_index, *pt_data);
                    }
                }

                if (memcmp(API12_Buffer,API12_Buffer_Digital,BIT_STORAGE_SIZE(AP12_Hmi_Num_GI_Digital)) != 0 )
                {
                    memcpy(API12_Buffer_Digital, API12_Buffer, BIT_STORAGE_SIZE(AP12_Hmi_Num_GI_Digital));
                    API012_Timer = API012_TIMER_COMPARE - 1;
                }
            }
        #endif

        #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
            // Update GI buffer and force publish if changed
            if (UpdateAcuGiStatus() == TRUE)
            {
                // TODO - solve issue with constantly changing GI.
                // API012_Timer = API012_TIMER_COMPARE;
            }
            if (UpdateAcuLoadStatus() == TRUE)
            {
                API012_Timer = API012_TIMER_COMPARE;
            }
        #endif

        API012_Timer++;
        if (API012_Timer > API012_TIMER_COMPARE)
        {
            API012_Timer = 0;

            #if (SYSTEM_HMI_HANDLING == ENABLED)
                #if (API012EXP_NUM_OF_EXTERNAL_LEDS != 0)
                    //TODO: Need to know how many LEDs to send
                    size = sizeof(API12_Led_Buffer);
                    // TODO: Need to account for more than one expansion board; Node 5 is hardcoded for now
                    // Adding 2 to account for offset at start of the message
                    ptr = API012REVEAL__NEWCOMMAND(API012EXP_NUM, API012EXP_SET_VIEW_ELEMENTS, 5, size + 2);

                    if (ptr != (REVEAL_COMMAND_TYPE *)NULL)
                    {
                        Reveal__AddDataU16(ptr, 0);
                        Reveal__AddDataU8Buffer(ptr, API12_Led_Buffer, size);
                        Reveal__QueueCommand(ptr);
                    }
                #endif
            #endif

            #if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED))
                PublishHMIGIs(WIN_UI_ADDRESS);
            #endif

            #if (SYSTEM_ACU_HANDLING == ENABLED)
                offset = 0;
                for (board_index =0; board_index < Number_Of_Expansion_Boards; board_index++)
                {
                    if ((Bus_Configuration_Ptr[board_index].Num_Loads != 0) &&
                        ((offset + Bus_Configuration_Ptr[board_index].Num_Loads) < API12_ACU_MAX_LOADS))
                    {
                        PublishACULoads(Bus_Configuration_Ptr[board_index].Address,
                                        &API12_ACU_Loads_Buffer[offset],
                                        Bus_Configuration_Ptr[board_index].Num_Loads);
                    }
                    offset += Bus_Configuration_Ptr[board_index].Num_Loads;
                }
                PublishTestCycleEngaged();
            #endif

            #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
                PublishACUGIs(REVEAL_BROADCAST_NODE);
                PublishACULoadsStatus(WIN_ACU_ADDRESS);
            #endif

        }
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method expedite the process to send data to the other side
 * @details \n
 *    LEDs from Main Hmi to Expansion Hmi.\n
 *    GIs from Expansion Hmi to Main Hmi.\n
 *    PILOTs From Main ACU to Expansion ACU.\n
 *    GIs from Expansion ACU to Main ACU.
 *
 */
void API012Exp__ForcePublish(BOOL_TYPE immediately)
{
    if (immediately == FALSE)
    {
        API012_Timer = API012_TIMER_COMPARE - 1;
    }
    else
    {
        API012_Timer = API012_TIMER_COMPARE;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Updates the desired state for the LEDs and requests a new message be sent to the expansion board.
 * @param   buffer: desired state for each LED
 * @param   size: number of LEDs
 */
void API012Exp__UpdateLeds(uint8 * buffer, uint8 size)
{
#if ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))
    #if (API012EXP_NUM_OF_EXTERNAL_LEDS != 0)
        if (buffer != NULL)
        {
            // Check requested number of LEDs is not more than the size of the local buffer
            if (size > sizeof(API12_Led_Buffer))
            {
                size = sizeof(API12_Led_Buffer);
            }

            // If there is a change to the state of at least one LED, update the buffer
            if (memcmp(API12_Led_Buffer, buffer, size) != 0 )
            {
                memcpy(API12_Led_Buffer, buffer, size);
                API012_Timer = API012_TIMER_COMPARE-1;
            }
        }
    #endif
#endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns a pointer to the buffer that holds the current state of the keys.
 * @detail  Each bit in the buffer corresponds to the state of one key.
 * @return  Current state of the keys.
 */
uint8 * API012Exp__GetKeyBuffer(void)
{
#if ( (SYSTEM_HMI_HANDLING == ENABLED) || ( (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)&&(API012EXP_HMI_EVENT_BASED == DISABLED) ) )
        return(API12_Buffer_Digital);
    #else
        return (NULL);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns a buffer pointer that holds the current state of the Encoder.
 * @detail  Each Byte of this buffer store data coming from API12 and opcode API012EXP_HMI_GI_EVENT.
 * @return  Return buffer pointer which holds data related with Encoder.
 */
API12_ENCODER_BUFFER_TYPE * API012Exp__GetEncoderBuffer(HBLHMI_GI_READ_TYPE read_type, uint8 gi_position)
{
#if (SYSTEM_HMI_HANDLING == ENABLED)
    HBLHMI_GI_TYPE *gi_info_ptr;
    gi_info_ptr = HblHmi__GetGITypeByGidata(read_type, gi_position);
    return(&API12_Encoder_Buffer[gi_info_ptr->LLI_position]);
#else
        return (NULL);
#endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Sends command to play a sound on an expansion board.
 * @param   sound_index
 * @param   volume
 */
void API012Exp__PlaySound(uint8 node, uint8 sound_index, uint8 volume)
{
    REVEAL_COMMAND_TYPE * ptr;

    ptr = API012REVEAL__NEWCOMMAND(API012EXP_NUM, API012EXP_PLAY_SOUND, node, 2);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, sound_index);
        Reveal__AddDataU8(ptr, volume);
        Reveal__QueueCommand(ptr);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Sends command to play a sound on an expansion board.
 * @param   volume
 */
void API012Exp__SetVolume(uint8 node, uint8 volume)
{
    REVEAL_COMMAND_TYPE * ptr;

    ptr = API012REVEAL__NEWCOMMAND(API012EXP_NUM, API012EXP_SET_VOLUME, node, 1);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, volume);
        Reveal__QueueCommand(ptr);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Send command to stop playing all sounds on an expansion board.
 */
void API012Exp__StopAllSound(uint8 node)
{
    REVEAL_COMMAND_TYPE * ptr;

    ptr = API012REVEAL__NEWCOMMAND(API012EXP_NUM, API012EXP_STOP_SOUND, node, 0);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__QueueCommand(ptr);
    }
}
/**
 * Sends command to play an animation on an expansion board.
 * @param node
 * @param animation_index
 */
void API012Exp__PlayAnimation(uint8 node, uint8 animation_index, uint8 led_group, uint32 pattern, uint8 repeats)
{
    REVEAL_COMMAND_TYPE * ptr;

    ptr = API012REVEAL__NEWCOMMAND(API012EXP_NUM, API012EXP_PLAY_ANIMATION, node, 7);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, animation_index);
        Reveal__AddDataU8(ptr, led_group);
        Reveal__AddDataU32(ptr, pattern);
        Reveal__AddDataU8(ptr, repeats);
        Reveal__QueueCommand(ptr);
    }
}
/**
 * Sends command to stop playing an animation on an expansion board.
 * @param node
 * @param animation_index
 */
void API012Exp__StopAnimation(uint8 node, uint8 led_group, uint32 pattern)
{
    REVEAL_COMMAND_TYPE * ptr;

    ptr = API012REVEAL__NEWCOMMAND(API012EXP_NUM, API012EXP_STOP_ANIMATION, node, 5);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8(ptr, led_group);
        Reveal__AddDataU32(ptr, pattern);
        Reveal__QueueCommand(ptr);
    }
}
//----------------------------------------------------------------------------------------------------------
/**
 * Getter that returns value of the GI requested (by index) from the API012 GI buffer.
 *
 * @param index
 */
sint32 API012Exp__GetGIByIndex(uint8 index)
{
    sint32 result = LONG_MAX;

    #if (SYSTEM_ACU_HANDLING == ENABLED)
        uint8 *data_ptr = GetBufferGIPtr(index);
        if(data_ptr != NULL)
        {
            result = *(data_ptr+1) << 24;
            result += *(data_ptr+2) << 16;
            result += *(data_ptr+3) << 8;
            result += *(data_ptr+4);
        }
    #endif

    return (result);
}

//----------------------------------------------------------------------------------------------------------
/**
 * Getter that returns the sequence ID of the GI item requested (by index).
 *
 * @param index
 */
uint8 API012Exp__GetGISequenceIDByIndex(uint8 index)
{
    uint8 result = 0;

    #if (SYSTEM_ACU_HANDLING == ENABLED)
    uint8 *data_ptr = GetBufferGIPtr(index);
    if(data_ptr != NULL)
    {
        result = *data_ptr;
    }
    #endif

    return (result);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes the number of loads received from the Main ACU, 0 means no loads received.
 * @return
 */
uint8 API012Exp__GetNumCommLoads(void)
{
    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        return (AP12_ACU_Num_Loads_Received);
    #else
        return (0);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes the buffer which contains the data to be applied to the loads in the ACU expansion Board.
 * The data is received from the Main ACU board.
 * @return
 */
uint8 * API012Exp__GetCommLoads(void)
{
    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        return (API12_ACU_Loads_Buffer);
    #else
        return (NULL);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to set HBL Loads that are on an Expansion ACU.
 *        Sets the requested value into the loads buffer sent to the Expansion ACU.
 *
 * @param index
 * @param value
 */
void API012Exp__SetExpansionLoad(uint8 index, uint8 value)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        if (index < API12_ACU_MAX_LOADS)
        {
            if (API12_ACU_Loads_Buffer[index] != value)
            {
                API12_ACU_Loads_Buffer[index] = value;
                API012Exp__ForcePublish(FALSE);
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method gets the data received as feedback from the expansion board.
 * @return
 */
uint8 * API012Exp__GetExpansionLoadsStatus(void)
{
    uint8 *load_status_ptr;

    load_status_ptr = NULL;

    #if (SYSTEM_ACU_HANDLING == ENABLED)
        load_status_ptr = (API12_ACU_Loads_Status_Buffer);
    #endif

    return (load_status_ptr);
}

#if (SYSTEM_ACU_HANDLING == ENABLED)
//----------------------------------------------------------------------------------------------------------
/**
 * @brief Set Test_Cycle_Engaged to engaged. Send API012 message to expansion boards.
 *
 * @param BOOL_TYPE engaged - Indicates if a test cycle is engaged
 */
void API012Exp__SetTestCycleEngaged(BOOL_TYPE engaged)
{
    Test_Cycle_Engaged = engaged;
    PublishTestCycleEngaged();
}
#endif

#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return boolean value to indicate if a test cycle is engaged.
 *
 *  @return     BOOL_TYPE
 *  @retval     TRUE - Test Cycle is engaged.
 *  @retval     FALSE - Test Cycle is not engaged.
 */
BOOL_TYPE API012Exp__IsTestCycleEngaged(void)
{
    return (Test_Cycle_Engaged);
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * Getter that returns value of the Last GI event occured on HMI
 * @return
 */
HBLHMI_GI_EVENT_TYPE API012Exp__GetLastGiEvent(void)
{
    return (Last_GI_Event);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Update ACU GI Status buffer.
 *
 *  @return     BOOL_TYPE changed - Indicates if the status of any GI has changed.
 *  @retval     TRUE - GI Status has changed.
 *  @retval     FALSE - GI Status has not changed
 */
static BOOL_TYPE UpdateAcuGiStatus(void)
{
    uint8 compare_buffer[ACU_EXP_GI_BUFFER_SIZE];

    memcpy(compare_buffer, API12_Expansion_ACU_GI_Buffer, AP12_ACU_GI_Data_Size);

    //DATA[0]--> NUMBER_OF_GI's
    uint8 number_of_gis = Hbl__GetNumGI();
    API12_Expansion_ACU_GI_Buffer[0] = number_of_gis;

    uint8 data_index = 1;

    uint32 gi_converted_data;
    for (uint8 gi_index = 0; gi_index < number_of_gis; gi_index++)
    {
        //DATA[1]-->GI DATA SIZE
        API12_Expansion_ACU_GI_Buffer[data_index]     = Hbl__GetGISequenceIDByIndex(gi_index);

        //DATA[2 - 5]--> GI_DATA
        gi_converted_data = Hbl__GetGIDataConvertedByIndex(gi_index);

        API12_Expansion_ACU_GI_Buffer[data_index + 1] = (gi_converted_data >> 24);
        API12_Expansion_ACU_GI_Buffer[data_index + 2] = (gi_converted_data >> 16);
        API12_Expansion_ACU_GI_Buffer[data_index + 3] = (gi_converted_data >> 8);
        API12_Expansion_ACU_GI_Buffer[data_index + 4] = gi_converted_data;
        data_index += API012_GI_PAYLOAD_SIZE+1;
    }

    if (memcmp(compare_buffer, API12_Expansion_ACU_GI_Buffer, AP12_ACU_GI_Data_Size) != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Update ACU Load Status buffer.
 *
 *  @return     BOOL_TYPE changed - Indicates if the status of any GI has changed.
 *  @retval     TRUE - Load Status has changed.
 *  @retval     FALSE - Load Status has no changed
 */
static BOOL_TYPE UpdateAcuLoadStatus(void)
{
    BOOL_TYPE changed;
    uint8 compare_buffer[API12_ACU_MAX_LOADS];
    uint8 *data_ptr;
    uint8 load_index;
    uint8 num_loads;

    changed = FALSE;
    memcpy(compare_buffer, API12_ACU_Loads_Status_Buffer, sizeof(compare_buffer));

    num_loads = Hbl__GetNumLoads();
    for (load_index = 0; load_index < num_loads; load_index++)
    {
        data_ptr = Hbl__GetLoadStatusByIndex(load_index);
        if (data_ptr != NULL)
        {
            API12_ACU_Loads_Status_Buffer[load_index] = *data_ptr;
        }
        else
        {
            API12_ACU_Loads_Status_Buffer[load_index] = 0xFF;
        }

    }

    if (memcmp(compare_buffer, API12_ACU_Loads_Status_Buffer, sizeof(API12_ACU_Loads_Status_Buffer)) != 0)
    {
        changed = TRUE;
    }

    return (changed);
}
#endif // (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)

#if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
    #if (API012EXP_HMI_EVENT_BASED == DISABLED)
    //----------------------------------------------------------------------------------------------------------
    /**
     *
     * @param address
     */
    static void PublishHMIGIs(uint8 address)
    {
        uint8 index;
        uint8 size;
        uint8 * pt_data;
        REVEAL_COMMAND_TYPE * ptr;

        size = BIT_STORAGE_SIZE(AP12_Hmi_Num_GI_Digital) + 1;
        size += (AP12_Hmi_Num_GI_Analog >> 1) + 1;
        size += AP12_Hmi_Num_GI_Encoder + 1;

        ptr = API012REVEAL__NEWFEEDBACK(API012EXP_NUM, API012EXP_REQ_HMI_PUB_GIS, WIN_UI_ADDRESS,size);

        if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
        {
            Reveal__AddDataU8(ptr, AP12_Hmi_Num_GI_Digital);
            if (AP12_Hmi_Num_GI_Digital > 0)
            {
                Reveal__AddDataU8Buffer(ptr, API12_Buffer_Digital,BIT_STORAGE_SIZE(AP12_Hmi_Num_GI_Digital));
            }

            Reveal__AddDataU8(ptr, AP12_Hmi_Num_GI_Analog);
            if (AP12_Hmi_Num_GI_Analog > 0)
            {
                // HMI Analog Data processing
                for (index = 0; index < AP12_Hmi_Num_GI_Analog; index++)
                {
                    pt_data = HblHmi__GetGIData(HBLHMI_GI_READ_ANALOG,index);
                    if (pt_data != NULL)
                    {
                        Reveal__AddDataU16(ptr,*((uint16 *)pt_data));
                    }
                    else
                    {
                        Reveal__AddDataU16(ptr,0xFFFF);
                    }
                }
            }

            Reveal__AddDataU8(ptr, AP12_Hmi_Num_GI_Encoder);
            if (AP12_Hmi_Num_GI_Encoder > 0)
            {
                for (index = 0; index < AP12_Hmi_Num_GI_Encoder; index++)
                {
                    pt_data = HblHmi__GetGIData(HBLHMI_GI_READ_ENCODER,index);
                    if (pt_data != NULL)
                    {
                        Reveal__AddDataU8(ptr,*pt_data);
                    }
                    else
                    {
                        Reveal__AddDataU8(ptr,0xFF);
                    }
                }
            }
            Reveal__QueueCommand(ptr);
        }
    }
    #else // API012EXP_HMI_EVENT_BASED is ENABLED
    //---------------------------------------------------------------------------------------------------------------------
    /**
     * @brief Parse the API012EXP_HMI_LED_EVENT OpCode
     * @details payload[0] = 0 the command is directed to a single LED or a group of LEDs
     *          payload[0] = 1 the command is directed to ALL Leds listed in the SettingFile
     * @param buffer as received from Reveal
     * @retval NVO_RS_READY command successfully parsed
     * @retval NVO_RS_UNSUP_INVALID_PARAM payload[0] contains an unsupported value
     * @retval NVO_RS_REJECTED unable to parse the received NULL buffer
     */
    REVEAL_RECIPE_STATUS_TYPE LedCommandParser(REVEAL_MSG_TYPE * buffer)
    {
        REVEAL_RECIPE_STATUS_TYPE retval = NVO_RS_READY;
        uint8 instance_field = 0;
        ON_OFF_TYPE led_state = OFF;
        uint8 led_intensity = 0;

        if((NULL != buffer ) && (NULL != buffer->payload))
        {
            // Reveal Payload: Instance |LED State | LED Intensity.
            // If Instance is 255, then it is using Opcode zero to drive a group of LEDs.
            if(buffer->payload[0] == LED_CMD_SET_STATE)
            {
                instance_field = buffer->payload[1];
                led_state = (ON_OFF_TYPE)buffer->payload[2];
                led_intensity = buffer->payload[3];

                if(instance_field < INVALID_LED_INDEX)
                {
                    if(led_state == OFF)
                    {
                        HblHmi__SetObject(VISUAL_OBJECT_LED, instance_field, PATTERN_ALL, LED_INTENSITY_OFF, HBLHMI_LED_PRIORITY_EMBEDDED);
                    }
                    if(led_state == ON)
                    {
                        HblHmi__SetObjectWithPWM(VISUAL_OBJECT_LED, instance_field, PATTERN_ALL, led_intensity, VIEWMGR_PWM_0, HBLHMI_LED_PRIORITY_EMBEDDED);
                    }
                }
                else
                {
                    //Here we need the number of instances and we need to point to them.
                    // Reveal Payload: Set LED | Instance | LED State | LED Intensity | Number of Instances | Instance #1 | Instance #2 | ... | Instance n.
                    uint8 number_of_instances = buffer->payload[4];
                    uint8 *instances = &(buffer->payload[5]);
                    for(uint8 led=0; led< number_of_instances; led++)
                    {
                        if(led_state == OFF)
                        {
                            HblHmi__SetObject(VISUAL_OBJECT_LED, instances[led], PATTERN_ALL, LED_INTENSITY_OFF, HBLHMI_LED_PRIORITY_EMBEDDED);
                        }
                        if(led_state == ON)
                        {
                            HblHmi__SetObjectWithPWM(VISUAL_OBJECT_LED, instances[led], PATTERN_ALL, led_intensity, VIEWMGR_PWM_0, HBLHMI_LED_PRIORITY_EMBEDDED);
                        }
                    }
                }
            }
            else if(buffer->payload[0] == LED_CMD_SET_ALL)
            {
                led_state = (ON_OFF_TYPE)(buffer->payload[1]&0x01);
                led_intensity = buffer->payload[2];
                LedMgr__SetAllLeds(led_intensity*led_state,0);
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
        }
        else
        {
            retval = NVO_RS_REJECTED;
        }

        return retval;
    }

    //---------------------------------------------------------------------------------------------------------------------
    /**
     * @brief HblHmi callback from HblHmi for GI events
     * @details Publishes GI events to SoC
     * @param event received from HblHmi
     */
    static void HmiGIEventHandler(HBLHMI_GI_EVENT_TYPE event)
    {
        HmiGIGetAndPublish(event.GI_Read_Type, event.GI_Position);
        Last_GI_Event.GI_Position = event.GI_Position;
        Last_GI_Event.GI_Read_Type = event.GI_Read_Type;
    }

    //---------------------------------------------------------------------------------------------------------------------
    /**
     * @brief Parse the API012EXP_HMI_GI_EVENT OpCode
     * @param buffer as received from Reveal
     * @retval NVO_RS_READY command successfully parsed
     * @retval NVO_RS_REJECTED unable to parse the received NULL buffer
     */
    REVEAL_RECIPE_STATUS_TYPE GiCommandParser(REVEAL_MSG_TYPE * buffer)
    {
        REVEAL_RECIPE_STATUS_TYPE retval = NVO_RS_READY;
        uint8 gi_index;
        uint8 num_of_gis = HblHmi__GetNumGIReadType(buffer->payload[0]);     //total number of GIs of specific read type as payload[0] is read type

        if(NULL != buffer)
        {
            if(buffer->payload[1] == 0xFF)                                   //payload[1] is used for GI index, special payload 0xFF is used to publish all the GIs of same read type
            {
                for(gi_index = 0; gi_index < num_of_gis; gi_index++)
                {
                    HmiGIGetAndPublish(buffer->payload[0], gi_index);        //publish all GIs of same read type
                }
            }
            else
            {
                HmiGIGetAndPublish(buffer->payload[0], buffer->payload[1]);
            }
        }
        else
        {
            retval = NVO_RS_REJECTED;
        }

        return retval;
    }

//---------------------------------------------------------------------------------------------------------------------
    /**
     * @brief publish event on request because of which system wake up
     * @details Publish last GI event data on API012, opcode 18 (API012EXP_HMI_GI_EVENT)
     * API 012, Opcode 20 publish last event publish on API 012 opcode 18
     * Last_Event_Data[] holds data of latest event and used to publish upon request
     */

    static void PublishLastGiEvent(void)
    {
        REVEAL_COMMAND_TYPE *cmd;
        cmd = Reveal__NewFeedback(API012EXP_NUM, API012EXP_HMI_LAST_STORED_GI_EVENT, REVEAL_BROADCAST_NODE, API012_OP20_MAX_DATA_SIZE);

        if(cmd != NULL)
        {
            Reveal__AddDataU8(cmd, Last_GI_Event.GI_Read_Type);
            Reveal__AddDataU8(cmd, Last_GI_Event.GI_Position);
            if(!Reveal__QueueCommand(cmd))
            {
                LOG_ADD_EXCEPTION(QUEUEING_ON_REVEAL, Reveal__GetFreeQueueSpace());
            }
        }

    }


//---------------------------------------------------------------------------------------------------------------------
    /**
     * @brief clears last GI event variable when reset
     * @details sets invalid GI read type (=3 not defined) and GI position to =63 when system reset
     */
    static void ClearLastGiEventInfo (void)
    {
        memset((void *)&Last_GI_Event, 0xFF, sizeof(Last_GI_Event));
    }

    //---------------------------------------------------------------------------------------------------------------------
    /**
     * @brief Get the data of specified GI and publish it
     * @details Publish GI data on API012, opcode 18 (API012EXP_HMI_GI_EVENT)
     *          Payload composition:
     *          |      Payload[0]      | Payload[1] | Payload[2] | Payload[3] | Payload[4] |  Payload[5]  |
     *          |       GI TYPE        |GI position | Data size  |            |
     *          ------------------------------------------EXAMPLES-----------------------------------------
     *          |HBLHMI_GI_READ_DIGITAL|      n     |     1      |  b0000ppcc*|
     *          |HBLHMI_GI_READ_ANALOG |      n     |     2      |  current   |  previous  |
     *          |HBLHMI_GI_READ_ENCODER|      n     |     3      |  current   |  previous  | direction     |
     *
     *          *pp - previous state,cc - current state {00 - released, 01 - pressed, 11 - unknown}
     *
     * @param read_type Type of the GI to publish
     * @param gi_position Position of the GI to publish
     */
    static void HmiGIGetAndPublish(HBLHMI_GI_READ_TYPE read_type, uint8 gi_position)
    {
        void* gi_data;
        uint8 gi_index;
        uint8 gi_data_size;
        REVEAL_COMMAND_TYPE *cmd;

        gi_data      = HblHmi__GetGIData(read_type, gi_position);
        gi_index     = HblHmi__GetGIConfigIndexByType(read_type, gi_position);
        gi_data_size = HblHmi__GetGIDataSizeByIndex(gi_index);
        cmd = Reveal__NewFeedback(API012EXP_NUM, API012EXP_HMI_GI_EVENT, REVEAL_BROADCAST_NODE, HMI_GI_EVENT_HEADER_LENGHT + gi_data_size);

        if(cmd != NULL)
        {
            Reveal__AddDataU8(cmd, read_type);
            Reveal__AddDataU8(cmd, gi_position);
            Reveal__AddDataU8(cmd, gi_data_size);
            Reveal__AddDataU8Buffer(cmd, (uint8*)gi_data, gi_data_size);
            if(!Reveal__QueueCommand(cmd))
            {
                LOG_ADD_EXCEPTION(QUEUEING_ON_REVEAL, Reveal__GetFreeQueueSpace());
            }
        }
    }
    #endif // (API012EXP_HMI_EVENT_BASED == DISABLED)
#endif // (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)

#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
/**
 *
 * @param address
 */
static void PublishACUGIs(uint8 node_id)
{
    REVEAL_COMMAND_TYPE * reveal_command_ptr;

    reveal_command_ptr = API012REVEAL__NEWFEEDBACK(API012EXP_NUM, API012EXP_REQ_ACU_PUB_GIS, node_id, AP12_ACU_GI_Data_Size);

    if (reveal_command_ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8Buffer(reveal_command_ptr, API12_Expansion_ACU_GI_Buffer, AP12_ACU_GI_Data_Size);

        Reveal__QueueCommand(reveal_command_ptr);
    }
}

/**
 * @brief Publish the status of all loads as a Reveal Feedback.
 * @param uint8 node_id - Destination address, 0x0F for broadcast.
 */
static void PublishACULoadsStatus(uint8 node_id)
{
    REVEAL_COMMAND_TYPE * reveal_command_ptr;

    reveal_command_ptr = API012REVEAL__NEWFEEDBACK(API012EXP_NUM, API012EXP_REQ_ACU_PUB_LOADS_STATUS, node_id, sizeof(API12_ACU_Loads_Status_Buffer));

    if (reveal_command_ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        Reveal__AddDataU8Buffer(reveal_command_ptr, API12_ACU_Loads_Status_Buffer, sizeof(API12_ACU_Loads_Status_Buffer));

        Reveal__QueueCommand(reveal_command_ptr);
    }
}
#endif // (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)

#if (SYSTEM_ACU_HANDLING == ENABLED)
/**
 * @brief This method sets loads into a specific board
 * @param address
 * @param data
 * @param size
 */
static void PublishACULoads(uint8 address, uint8 *data, uint8 size)
{
   REVEAL_COMMAND_TYPE *reveal_command_ptr;

   if (address < REVEAL_BROADCAST_NODE)
   {
       reveal_command_ptr = API012REVEAL__NEWFEEDBACK(API012EXP_NUM, API012EXP_SET_ACU_LOADS, address, size);

       if (reveal_command_ptr != (REVEAL_COMMAND_TYPE *) NULL)
       {
           Reveal__AddDataU8Buffer(reveal_command_ptr, data, size);
           Reveal__QueueCommand(reveal_command_ptr);
       }
   }
}

/**
 * @brief Publishes Test_Cycle_Engaged to all expansion boards.
 */
static void PublishTestCycleEngaged(void)
{
   for (uint8 node = 0; node < Number_Of_Expansion_Boards; node++)
   {
       uint8 address = Bus_Configuration_Ptr[node].Address;
       if (address < REVEAL_BROADCAST_NODE)
       {
           REVEAL_COMMAND_TYPE *reveal_command_ptr = API012REVEAL__NEWFEEDBACK(API012EXP_NUM,
                                                                         API012EXP_SET_TEST_STATUS,
                                                                         address,
                                                                         1);
           if (reveal_command_ptr != (REVEAL_COMMAND_TYPE *) NULL)
           {
               Reveal__AddDataU8(reveal_command_ptr, Test_Cycle_Engaged);
               Reveal__QueueCommand(reveal_command_ptr);
           }
       }
   }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Read system expansion board configuration parameters from setting file.
 */
static void ReadAcuExpansionConfiguration(void)
{
    Number_Of_Expansion_Boards = 0;
    Bus_Configuration_Ptr = NULL;

    SETTINGFILE_LOADER_TYPE loader;
    if (SettingFile__BasicLoader((SF_PTR_ACU_CROSS_PRODUCT_CONFIG), SF_DISPL_ACU_EXPANSION_CONFIGURATION, &loader) == PASS)
    {
        Number_Of_Expansion_Boards = loader.Length / sizeof(API012EXP_BUS_CONFIG_TYPE);
        if (Number_Of_Expansion_Boards <= API012EXP_MAX_BOARDS)
        {
            Bus_Configuration_Ptr = (API012EXP_BUS_CONFIG_TYPE *)(void *)loader.Data;
        }
        else
        {
            Number_Of_Expansion_Boards = 0;
            LOG_ADD_EXCEPTION(INVALID_CONFIGURATION_PARAMETERS, Number_Of_Expansion_Boards);
        }
    }
    else
    {
        LOG_ADD_EXCEPTION(CONFIGURATION_PARAMETERS_NOT_FOUND, Number_Of_Expansion_Boards);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return GI data pointer from the buffer
 */
static uint8 *GetBufferGIPtr(uint8 index)
{
    uint8 *data_ptr = NULL;

    if(index < API12_ACU_MAX_GIS)
    {
        uint8 board_index;
        uint8 position;


        for (board_index = 0; board_index < Number_Of_Expansion_Boards; board_index++)
        {
            if (index < Bus_Configuration_Ptr[board_index].Num_Gis)
            {
                break;
            }
            else
            {
                index -= Bus_Configuration_Ptr[board_index].Num_Gis;
            }
        }

        position = 1 + index*(API012_GI_PAYLOAD_SIZE+1);
        data_ptr = &API12_ACU_GI[board_index].Buffer[position];
    }

    return data_ptr;
}

#endif // (SYSTEM_ACU_HANDLING == ENABLED)

#endif // (API012EXP_FEATURE == ENABLED)
