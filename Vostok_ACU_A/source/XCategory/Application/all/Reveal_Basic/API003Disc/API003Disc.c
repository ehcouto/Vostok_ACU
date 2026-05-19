/**
 *  @file
 *
 *  @brief      This module handles the API 003 messages which implements the Discovery functionality.
 *
 *
 *  $Header: API003Disc.c 1.2 2015/03/17 18:45:03EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/03/17 18:45:03EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (API003DISC_FEATURE == ENABLED)
#include "API003Disc.h" 
#include "API003Disc_prv.h"

#if (API005LOWLEVEL_FEATURE == ENABLED)
    #include "API005LowLevel.h"
#endif
#if (API006HMI_FEATURE == ENABLED)
    #include "API006Hmi.h"
#endif
#include "API007Data.h"
#if (API009SYSTEM_FEATURE == ENABLED)
    #include "API009System.h"
#endif
#if (API010POLLVAR_FEATURE == ENABLED)
    #include "API010PollVar.h"
#endif
#include "API011AppCtrl.h"
#if (API012EXP_FEATURE == ENABLED)
    #include "API012Exp.h"
#endif
#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
    #include "API013RemoteFunction.h"
#endif
#if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
    #include "API018Diagnostic.h"
#endif
#if (API019PRODUCTINFO_FEATURE == ENABLED)
    #include "API019ProductInfo.h"
#endif
#if (API020SAFETY_FEATURE == ENABLED)
    #include "SRAPI020Safety.h"
#endif
#if (API201_DEVICE_SECURITY_FEATURE == ENABLED)
    #include "API201DeviceSecurity.h"
#endif
#if ((API221_CLIENT_FEATURE == ENABLED) || (API221_SERVER_FEATURE == ENABLED))
    #include "API221MotionCtrl.h"
#endif
#include "Reveal.h"
#include <String.h>
#include "Utilities.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

#ifndef API003_FIREWALL_FEATURE
    #define API003_FIREWALL_FEATURE DISABLED
#endif

#ifndef API003DISC__RECEIVED_REVEAL_NODES
    #define API003DISC__RECEIVED_REVEAL_NODES(node,password)
#endif

#ifndef API003DISC__RECEIVED_API_INFO
    #define API003DISC__RECEIVED_API_INFO(node,api,type_16bits,version_16bits,num_instances)
#endif


#ifndef API003DISC__RECEIVED_API_INSTANCE_INFO
    #define API003DISC__RECEIVED_API_INSTANCE_INFO(bnode, api, instance, type_16bits,version_16bits)
#endif


#ifndef API003DISC__RECEIVED_APIS
    #define API003DISC__RECEIVED_APIS(node,api_array,api_array_size)
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef enum
{
    API003_OP_FIND_REVEAL_NODES = 1,
    API003_OP_PUB_REVEAL_NODES,
    API003_OP_GET_APIS,
    API003_OP_PUB_APIS,
    API003_OP_GET_INFO,
    API003_OP_PUB_INFO,
    API003_OP_GET_INSTANCES,
    API003_OP_PUB_INSTANCES
}API003DISC_OPCODES_TYPE;

/**
 * This array defines the description of each api in this system.
 * The structure is defined in Reveal_defs.h is looks like:
 * <pre>
 *  typedef struct
 *  {
 *      unsigned char id;           // API number
 *      unsigned short int type;    // API type
 *      unsigned short int version; // API version
 *      unsigned char instances;    // API number of instances (1 means no other instances, 2 or more means other instances)
 *  } REVEAL_API_INFO_TYPE;
 * </pre>
 */
static const REVEAL_API_INFO_TYPE API003_APIS[] =
{
//  {API Number             ,API Type               ,API Version                ,API Instances              },
    {API001CORE_NUM         ,API001CORE_TYPE        ,API001CORE_VERSION         ,API001CORE_INSTANCES       },
#if (API005LOWLEVEL_FEATURE == ENABLED)
    {API005LOWLEVEL_NUM     ,API005LOWLEVEL_TYPE    ,API005LOWLEVEL_VERSION     ,API005LOWLEVEL_INSTANCES   },
#endif
#if (API006HMI_FEATURE == ENABLED)
    {API006HMI_NUM          ,API006HMI_TYPE         ,API006HMI_VERSION          ,API006HMI_INSTANCES        },
#endif
    {API007DATA_NUM         ,API007DATA_TYPE        ,API007DATA_VERSION         ,API007DATA_INSTANCES       },
#if (API009SYSTEM_FEATURE == ENABLED)
    {API009SYSTEM_NUM       ,API009SYSTEM_TYPE      ,API009SYSTEM_VERSION       ,API009SYSTEM_INSTANCES     },
#endif
#if (API010POLLVAR_FEATURE == ENABLED)
    {API010POLLVAR_NUM      ,API010POLLVAR_TYPE     ,API010POLLVAR_VERSION      ,API010POLLVAR_INSTANCES    },
#endif
    {API011APPCTRL_NUM      ,API011APPCTRL_TYPE     ,API011APPCTRL_VERSION      ,API011APPCTRL_INSTANCES    },
#if (API012EXP_FEATURE == ENABLED)
    {API012EXP_NUM          ,API012EXP_TYPE         ,API012EXP_VERSION          ,API012EXP_INSTANCES        },
#endif
#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
    {API013RMTFCN_NUM       ,API013RMTFCN_TYPE      ,API013RMTFCN_VERSION       ,API013RMTFCN_INSTANCES     },
#endif
#if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
    {API018DIAGNOSTIC_NUM   ,API018DIAGNOSTIC_TYPE  ,API018DIAGNOSTIC_VERSION   ,API018DIAGNOSTIC_INSTANCES },
#endif
#if (API019PRODUCTINFO_FEATURE == ENABLED)
    {API019PRODUCTINFO_NUM  ,API019PRODUCTINFO_TYPE ,API019PRODUCTINFO_VERSION  ,API019PRODUCTINFO_INSTANCES},
#endif
#if (API020SAFETY_FEATURE == ENABLED)
    {API020SAFETY_NUM       ,API020SAFETY_TYPE      ,API020SAFETY_VERSION       ,API020SAFETY_INSTANCES     },
#endif
#if (API201_DEVICE_SECURITY_FEATURE == ENABLED)
    {API201DEVICESECURITY_NUM,API201DEVICESECURITY_TYPE,API201DEVICESECURITY_VERSION,API201DEVICESECURITY_INSTANCES},
#endif
#if ((API221_CLIENT_FEATURE == ENABLED) || (API221_SERVER_FEATURE == ENABLED))
    {API221MOTIONCTRL_NUM   ,API221MOTIONCTRL_TYPE  ,API221MOTIONCTRL_VERSION   ,API221MOTIONCTRL_INSTANCES },
#endif
    {API003DISC_NUM         ,API003DISC_TYPE        ,API003DISC_VERSION         ,API003DISC_INSTANCES       }
};

#define API003_NUM_APIS_REG (sizeof(API003_APIS)/sizeof(REVEAL_API_INFO_TYPE))

#if (API003_NUM_INSTANCES > 0)
    static const REVEAL_INSTACNE_INFO_TYPE API003_INSTANCES[API003_NUM_INSTANCES] = {API003_INSTANCE_LIST};
#endif

#if (API003_FIREWALL_FEATURE == ENABLED)
    static static uint16 API03_Firewall_Password[15];
#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
static void PublishRevealNodes(void);
static uint8 GetAPIIndex(uint8 api);
#if (API003_NUM_INSTANCES > 0)
    static void PublishInstanceInfo(uint8 index,uint8 address);
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module API003Disc and its variables
 *
 */
void API003Disc__Initialize(void)
{
    #if (API003_FIREWALL_FEATURE == ENABLED)
        memset((uint8 *)API03_Firewall_Password,0xFF,sizeof(API03_Firewall_Password));
    #endif
    PublishRevealNodes();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses messages target to the API003Disc sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message
 * @return REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API003Disc__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    uint8 count;
    uint8 index;
    uint8 aux;
    REVEAL_COMMAND_TYPE * ptr;

    retval = NVO_RS_READY;
    switch ((API003DISC_OPCODES_TYPE)buffer->op_code)
    {
        case API003_OP_FIND_REVEAL_NODES:
            PublishRevealNodes();
            break;
        case API003_OP_GET_APIS:
            //API003_OP_PUB_APIS
            ptr = Reveal__NewFeedback(API003DISC_NUM,API003_OP_PUB_APIS,buffer->node_id,API003_NUM_APIS_REG);

            if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
            {
                for (count=0; count< API003_NUM_APIS_REG; count++)
                {
                    Reveal__AddDataU8(ptr,(uint8 )API003_APIS[count].id);
                }
                Reveal__QueueCommand(ptr);
            }
            break;
        case API003_OP_GET_INFO:
            //API003_OP_PUB_INFO
            if (buffer->payload[0] == 0xFF)
            {
                for (count=0; count<API003_NUM_APIS_REG; count++)
                {
                    API003Disc__PublishAPIInfo(count,buffer->node_id);
                }
            }
            else
            {
                count = buffer->sdu_length - 2; // sdu excluding API and Opcode
                for (index = 0; index< count; index++)
                {
                    aux = GetAPIIndex(buffer->payload[index]);
                    if (aux != 0xFF)
                    {
                        API003Disc__PublishAPIInfo(aux,buffer->node_id);
                    }
                }
            }
            break;
        case API003_OP_GET_INSTANCES:
            //API003_OP_PUB_INSTANCES
            #if (API003_NUM_INSTANCES > 0)
                //const REVEAL_INSTACNE_INFO_TYPE API003_INSTANCES[API003_NUM_INSTANCES] = {API003_INSTANCE_LIST};
                if (buffer->payload[0] == 0xFF)
                {
                    for (count=0; count<API003_NUM_INSTANCES; count++)
                    {
                        PublishInstanceInfo(count,buffer->node_id);
                    }
                }
                else
                {
                    count = buffer->sdu_length - 2; // sdu excluding API and Opcode
                    for (index = 0; index< count; index++)
                    {
                        for (count=0; count<API003_NUM_INSTANCES; count++)
                        {
                            if (API003_INSTANCES[count].api_id == buffer->payload[index])
                            {
                                PublishInstanceInfo(count,buffer->node_id);
                            }
                        }
                    }
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

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses feedbacks to the API003Disc sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message
 */
void API003Disc__FeedbackParser(REVEAL_MSG_TYPE * buffer)
{
    switch ((API003DISC_OPCODES_TYPE)buffer->op_code)
    {
#if (API003_FIREWALL_FEATURE == ENABLED)
        case API003_OP_PUB_REVEAL_NODES:
            if (buffer->payload == 2)
            {
                API03_Firewall_Password[buffer->node_id] = Utilities__ConvertArrayTo16bits(buffer->payload);
                API003DISC__RECEIVED_REVEAL_NODES(buffer->node_id,Utilities__ConvertArrayTo16bits(buffer->payload))
            }
            break;
#endif
        case API003_OP_PUB_INFO:
            API003DISC__RECEIVED_API_INFO(buffer->node_id,buffer->payload[0],Utilities__ConvertArrayTo16bits(&buffer->payload[1]),Utilities__ConvertArrayTo16bits(&buffer->payload[3]),buffer->payload[5]);
            break;
        case API003_OP_PUB_APIS:
            API003DISC__RECEIVED_APIS(buffer->node_id,&buffer->payload[0],buffer->size-2);
            break;
        case API003_OP_PUB_INSTANCES:
            API003DISC__RECEIVED_API_INSTANCE_INFO(buffer->node_id,buffer->payload[0],buffer->payload[1],Utilities__ConvertArrayTo16bits(&buffer->payload[2]),Utilities__ConvertArrayTo16bits(&buffer->payload[4]));
            break;
        default:
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes the password of a specific node.
 * @param node node which the external module is requesting the password from.
 * @return password
 */
uint16 API003Disc__GetNodePassword(uint8 node)
{
#if (API003_FIREWALL_FEATURE == ENABLED)
    if (node < 15)
    {
        return (API03_Firewall_Password[node]);
    }
#endif
    return (0xFFFF);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method request for other nodes if they are Reveal nodes and what is their fire-wall password
 * @param node Destination node for the message
 */
void API003Disc__CheckRevealNodes(uint8 node)
{
    REVEAL_COMMAND_TYPE * ptr;
    if (node <= REVEAL_BROADCAST_NODE)
    {
        ptr = Reveal__NewCommand(API003DISC_NUM,API003_OP_FIND_REVEAL_NODES,node,0);
        if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__QueueCommand(ptr);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method request for other nodes what APIs they have.
 * @param node Destination node for the message
 */
void API003Disc__GetAPIs(uint8 node)
{
    REVEAL_COMMAND_TYPE * ptr;
    if (node <= REVEAL_BROADCAST_NODE)
    {
        ptr = Reveal__NewCommand(API003DISC_NUM,API003_OP_GET_APIS,node,0);
        if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__QueueCommand(ptr);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to request the API info from another node.
 * @param node Destination node for the message
 * @param api Specific API requested, 0xFF for all APIs.
 */
void API003Disc__GetAPIInfo(uint8 node,uint8 api)
{
    REVEAL_COMMAND_TYPE * ptr;
    if (node <= REVEAL_BROADCAST_NODE)
    {
        ptr = Reveal__NewCommand(API003DISC_NUM,API003_OP_GET_INFO,node,1);
        if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__AddDataU8(ptr,api);
            Reveal__QueueCommand(ptr);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to request the API instances from another node.
 * @param node Destination node for the message
 * @param api Specific API requested, 0xFF for all APIs.
 */
void API003Disc__GetAPIInstances(uint8 node,uint8 api)
{
    REVEAL_COMMAND_TYPE * ptr;
    if (node <= REVEAL_BROADCAST_NODE)
    {
        ptr = Reveal__NewCommand(API003DISC_NUM,API003_OP_GET_INSTANCES,node,1);
        if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__AddDataU8(ptr,api);
            Reveal__QueueCommand(ptr);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method publishes the API info for a specific line in the API info table
 * @param index defines the item in the API info table to be published
 * @param address defines the addres to whom the message will be send.
 */
void API003Disc__PublishAPIInfo(uint8 index,uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewFeedback(API003DISC_NUM,API003_OP_PUB_INFO,address,6);
    if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
    {
        Reveal__AddDataU8(ptr,API003_APIS[index].id);
        Reveal__AddDataU16(ptr,API003_APIS[index].type);
        Reveal__AddDataU16(ptr,API003_APIS[index].version);
        Reveal__AddDataU8(ptr,API003_APIS[index].instances);
        Reveal__QueueCommand(ptr);
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method publishes the message relate to being an REVEAL node along with the password
 */
static void PublishRevealNodes(void)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewFeedback(API003DISC_NUM,API003_OP_PUB_REVEAL_NODES,REVEAL_BROADCAST_NODE,2);
    if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
    {
        Reveal__AddDataU16(ptr,API003_FIREWALL_PWR);
        Reveal__QueueCommand(ptr);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the index in the API info table about a specific API id.
 * @param api define which API is been searched.
 * @return index in the API info table related to the searched API id, if the API is not found it returst 0xFF.
 */
static uint8 GetAPIIndex(uint8 api)
{
    uint8 count;
    uint8 index;
    index = 0xFF;

    for (count=0; count<API003_NUM_APIS_REG; count++)
    {
        if (API003_APIS[count].id == api)
        {
            index = count;
            break;
        }
    }
    return (index);

}



#if (API003_NUM_INSTANCES > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief THis method publishes the Instance information in a specific item in the instance table
 * @param index defines the item in the instance table to be publish
 * @param address defines the address to whom the message will be send.
 */
static void PublishInstanceInfo(uint8 index,uint8 address)
{
    REVEAL_COMMAND_TYPE * ptr;
    ptr = Reveal__NewFeedback(API003DISC_NUM,API003_OP_PUB_INSTANCES,address,6);
    if (ptr!=(REVEAL_COMMAND_TYPE *)NULL)
    {
        Reveal__AddDataU8(ptr,API003_INSTANCES[index].api_id);
        Reveal__AddDataU8(ptr,API003_INSTANCES[index].instance_id);
        Reveal__AddDataU16(ptr,API003_INSTANCES[index].type);
        Reveal__AddDataU16(ptr,API003_INSTANCES[index].version);
        Reveal__QueueCommand(ptr);
    }
}
#endif

#endif //API003DISC_FEATURE
