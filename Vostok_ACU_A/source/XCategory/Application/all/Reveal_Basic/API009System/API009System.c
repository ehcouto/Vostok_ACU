/**
 *  @file       
 *
 *  @brief      API009System interfaces some system functionalities such as Force other boards to reset, or LowPower mode
 *  and also some information about what was the source of reset and what is the endianess of this node.
 *
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "API009System.h"

#if (API009SYSTEM_FEATURE == ENABLED)
#include "Reveal.h"
#include "API009System_prv.h"
#include "Micro.h"

#if (LOW_POWER_FEATURE == ENABLED)
    #include "SRPowerMgr.h"
#endif

#ifndef	NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE
	#define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE DISABLED
#endif

#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
	#include "NVData.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
typedef enum _API009_OPCODE_TYPE
{
	API009_OP_RESET                         = 1,
	API009_OP_ENDIANESS                     = 2,
	API009_OP_REQUEST_LOW_POWER_STATE       = 3,
	API009_OP_WAKEUP_SOURCE                 = 4,
    API009_OP_REQUEST_RESET_STATUS          = 5,
	API009_OP_PUBLISH_LOW_POWER_CHECK       = 8,

	// reserved for other features
	API009_OP_WATCHDOG_RESET                = 31
} API009_OPCODE_TYPE;

typedef enum
{
    API009_RESET_MODE_COLD = 0,
    API009_RESET_MODE_WARM,
    API009_RESET_MODE_ENTER_IAP
}API009_RESET_TYPE;

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef API009SYSTEM_GO_LOW_POWER_TIMER
	#define	API009SYSTEM_GO_LOW_POWER_TIMER	20
#endif

#ifndef API009SYSTEM__CHECK_GO_LOW_POWER_ALLOWED
    #define API009SYSTEM__CHECK_GO_LOW_POWER_ALLOWED()    TRUE
#endif

#ifndef API009SYSTEM_WATCHDOG_RESET
    #define API009SYSTEM_WATCHDOG_RESET()
#endif

#ifndef API009SYSTEM__CANCEL_LOW_POWER
    #define API009SYSTEM__CANCEL_LOW_POWER()
#endif

static uint16 API009System_Reset_Timer;
static API009_RESET_TYPE API009System_Reset_Mode;

#if (LOW_POWER_FEATURE == ENABLED)
    static uint16 API009System_Go_Sleep_Timer;
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE API009SystemPublishResetStatus(API009_OPCODE_TYPE opcode, uint8 node);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief It Initializes the module API009System and its variables.
 */
void API009System__Initialize(void)
{
    API009System_Reset_Timer                   = 0;
    API009System_Reset_Mode                    = API009_RESET_MODE_COLD;

    #if (LOW_POWER_FEATURE == ENABLED)
        API009System_Go_Sleep_Timer = 0;
    #endif

    //Publish the reset status
    API009SystemPublishResetStatus(API009_OP_RESET, REVEAL_BROADCAST_NODE);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses messages target to the API017 sent to a broadcast or exclusively to this node for Reveal 4.x.
 * @param buffer contains the Wide package that contains the message
 * @return REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API009System__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
	REVEAL_COMMAND_TYPE * pt;
    retval = NVO_RS_READY;

    switch ((API009_OPCODE_TYPE)buffer->op_code)
    {
        case API009_OP_RESET:
            // receive command for forced reset
            // Options
            // 2 parameters = time to reset 16 bits (Cold reset)
            // 3 parameters = time to reset 16bits + type of reset (cold / warm)

            API009System_Reset_Timer = (uint16)buffer->payload[0];
            API009System_Reset_Timer <<= 8;
            API009System_Reset_Timer += (uint16)buffer->payload[1];

            if (buffer->size > 2)
            {
                API009System_Reset_Mode = (API009_RESET_TYPE)buffer->payload[2];
            }
            else
            {
                API009System_Reset_Mode = API009_RESET_MODE_COLD;
            }

            if (API009System_Reset_Timer > 0)
            {
                API009System_Reset_Timer /= 25;
                if (API009System_Reset_Timer == 0)
                {
                    API009System_Reset_Timer = 1;
                }
            }
            break;

        case API009_OP_REQUEST_RESET_STATUS:
            //Publish the reset status
            if (API009SystemPublishResetStatus(API009_OP_REQUEST_RESET_STATUS, buffer->node_id) == FALSE)
            {
                retval = NVO_RS_REJECTED;
            }
            break;

        case API009_OP_ENDIANESS:
            // request for the system Endianess
            pt = Reveal__NewFeedback(API009SYSTEM_NUM,API009_OP_ENDIANESS,REVEAL_BROADCAST_NODE,1);
            if (pt != (REVEAL_COMMAND_TYPE *)NULL)
            {
                Reveal__AddDataU8(pt,MICRO_IS_LITTLE_ENDIAN);
                Reveal__QueueCommand(pt);
            }

            break;

        #if (LOW_POWER_FEATURE == ENABLED)
            case API009_OP_REQUEST_LOW_POWER_STATE:
                if (SRPOWERMGR_LOWPOWER_MODE && API009SYSTEM__CHECK_GO_LOW_POWER_ALLOWED())
                {
                    if (buffer->size > 0)
                    {
                        API009System_Go_Sleep_Timer = (uint16)buffer->payload[0];
                        API009System_Go_Sleep_Timer <<= 8;
                        API009System_Go_Sleep_Timer += (uint16)buffer->payload[1];
                    }
                    else
                    {
                        API009System_Go_Sleep_Timer = API009SYSTEM_GO_LOW_POWER_TIMER;
                    }
                }
                else
                {
                    API009System_Go_Sleep_Timer = 0;
                    retval = NVO_RS_REJECTED;
                }

                pt = Reveal__NewFeedback(API009SYSTEM_NUM,API009_OP_PUBLISH_LOW_POWER_CHECK,REVEAL_BROADCAST_NODE,1);
                if (pt != (REVEAL_COMMAND_TYPE *)NULL)
                {
                    if (retval == NVO_RS_REJECTED)
                    {
                        Reveal__AddDataU8(pt,0);
                    }
                    else
                    {
                        Reveal__AddDataU8(pt,1);
                    }
                    Reveal__QueueCommand(pt);
                }
                break;

            case API009_OP_WAKEUP_SOURCE:
                if (SRPowerMgr__IsThisNodeWakeUpSource())
                {
                    pt = Reveal__NewFeedback(API009SYSTEM_NUM,API009_OP_WAKEUP_SOURCE,0xF,0);
                    if (pt != (REVEAL_COMMAND_TYPE *)NULL)
                    {
                        Reveal__QueueCommand(pt);
                    }
                }
                break;
        #endif

        case API009_OP_WATCHDOG_RESET:
            API009SYSTEM_WATCHDOG_RESET();
            break; //lint !e527  Unreachable code at token 'break'

        default:
			retval = NVO_RS_UNSUP_OP_CODE;

            break;
    }

    return (retval);
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses feedbacks to the API009System sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message.
 */
void API009System__FeedbackParser(REVEAL_MSG_TYPE * buffer)
{
    switch ((API009_OPCODE_TYPE)buffer->op_code)
    {
        case API009_OP_RESET:
        case API009_OP_REQUEST_RESET_STATUS:

            API009SYSTEM__NODE_RESET_MSG(buffer->node_id,
                                        buffer->payload[0],                                 // MICRO_LAST_RESET_TYPE information
                                        (MICRO_RESET_MODE_TYPE)buffer->payload[1]);         // MICRO_RESET_MODE_TYPE
            break;

        case API009_OP_ENDIANESS:
            API009SYSTEM__ENDIANESS_REQUESTG(buffer->node_id,buffer->payload[0]);
            break;

        #if (LOW_POWER_FEATURE == ENABLED)
        case API009_OP_WAKEUP_SOURCE:
                SRPowerMgr__ReceivedWakeUpNodeID(buffer->node_id);
        	break;

        case API009_OP_PUBLISH_LOW_POWER_CHECK:
            if (buffer->payload[0] == 0)
            {
                // Cancel Going to Low Power
                if (API009System_Go_Sleep_Timer > 0) // if still timing
                {
                    API009System_Go_Sleep_Timer = 0; // clear the timer
                    API009SYSTEM__CANCEL_LOW_POWER();// Notify the Application that the low power was canceled.
                }
            }
            break;
        #endif

        default:
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method handles callbacks from Reveal when and ACK is received for this API.
 * @param address source of the ack message.
 * @param api   API related to the aCK message.
 * @param opcode OPCODE related to the ACK message.
 * @param reasoncode REASONCODE  related to the ACK message.
 */
void API009System__AckParser(uint8 address,uint8 api,uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode)
{
	switch ((API009_OPCODE_TYPE)opcode)
	{
		case API009_OP_RESET:
			API009SYSTEM__RESET_ACK(address);
			break;

        #if (LOW_POWER_FEATURE == ENABLED)
		case API009_OP_REQUEST_LOW_POWER_STATE:
			API009SYSTEM__GO_LOW_POWER_ACK(address,reasoncode);
			if (reasoncode != NVO_RS_READY)
			{
			    // Rejected
			    // Cancel the process by clear the timer
			    if (API009System_Go_Sleep_Timer > 0)
			    {
			        API009System_Go_Sleep_Timer = 0;
			        API009SYSTEM__CANCEL_LOW_POWER();
			    }
			}

			break;
        #endif

		default:
			break;
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method handle processes for the API009System on 25ms time bases.
 */
void API009System__Handler25ms(void)
{
    if (API009System_Reset_Timer > 0)
    {
        API009System_Reset_Timer--;
        if (API009System_Reset_Timer == 0)
        {
            DISABLE_INTERRUPTS();

            if (API009System_Reset_Mode == API009_RESET_MODE_WARM)
            {
                Micro__ForceReset(MICRO_RESET_MODE_WARM);
            }
            else if (API009System_Reset_Mode == API009_RESET_MODE_ENTER_IAP)
            {
				#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)

					// The Non Volatile Data is stored to the DataFlash memory when an IAP update is requested
					// If the ACU firmware is updated then the NVdata is ignored after the update
            		// If the other then ACU firmware is updated then the Non Volatile data is restored
            		NVData__Store();
				#endif

                Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_IAP);
                Micro__ForceReset(MICRO_RESET_MODE_WARM);
            }
            else
            {
                Micro__ForceReset(MICRO_RESET_MODE_COLD);
            }
        }
    }

    #if (LOW_POWER_FEATURE == ENABLED)
        if (API009System_Go_Sleep_Timer > 0)
        {
            API009System_Go_Sleep_Timer--;
            if (API009System_Go_Sleep_Timer == 0)
            {
                SRPowerMgr__GoLowPowerMode();
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method sends a command to the other nodes to force them to reset in time_ms
 * @param node Node to force the reset (0xF will generate a broadcast command).
 * @param time_ms Time for the other nodes to force the reset in ms.
 * @return TRUE if the command was queued or FALSE if it failed queuing the command.
 */
BOOL_TYPE API009System__ForceReset(uint8 node,uint16 time_ms)
{
    REVEAL_COMMAND_TYPE * pt;
    if (node <= 0xF)
    {
        pt = Reveal__NewCommand(API009SYSTEM_NUM,API009_OP_RESET,node,2);
        if (pt != (REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__AddDataU16(pt,time_ms);
            Reveal__QueueCommand(pt);
            return (TRUE);
        }
    }
    return (FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method sends a command to the requested node to request the reset status
 * @param node
 * @return BOOL_TYPE
 */
BOOL_TYPE API009System__RequestResetStatus(uint8 node)
{
    REVEAL_COMMAND_TYPE * pt;
    if (node <= 0xF)
    {
        pt = Reveal__NewCommand(API009SYSTEM_NUM, API009_OP_REQUEST_RESET_STATUS, node, 0);
        if (pt != (REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__QueueCommand(pt);
            return (TRUE);
        }
    }
    return (FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used for this node to tell the other nodes to do in LowPower Mode.
 * @param node which is the target for this command. 0xF will cause a broadcast command.
 * @return TRUE if the command was queued or FALSE if it failed queuing the command.
 */
BOOL_TYPE API009System__SendGoLowPower(uint8 node)
{
    #if (LOW_POWER_FEATURE == ENABLED)
        BOOL_TYPE retval;
        REVEAL_COMMAND_TYPE * pt;
        retval = FALSE;
        if (node < 0x10)
        {
            if (SRPOWERMGR_LOWPOWER_MODE && API009SYSTEM__CHECK_GO_LOW_POWER_ALLOWED())
            {
                pt = Reveal__NewCommand(API009SYSTEM_NUM,API009_OP_REQUEST_LOW_POWER_STATE,node,0);
                if (pt != (REVEAL_COMMAND_TYPE *)NULL)
                {
                    API009System_Go_Sleep_Timer = API009SYSTEM_GO_LOW_POWER_TIMER;
                    Reveal__QueueCommand(pt);
                    retval = TRUE;
                }
            }
        }
        return (retval);
    #else
        return FALSE;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks is the API009 is timing to go to Sleep mode
 * @return going to sleep
 *   @retval TRUE : Timer is counting down to go to Sleep
 *   @retval FALSE : Timer is clear and there is no action to take the System to Sleep mode
 */
BOOL_TYPE API009System__IsSystemGoingToSleep(void)
{
    #if (LOW_POWER_FEATURE == ENABLED)
        if (API009System_Go_Sleep_Timer > 0)
        {
            return (TRUE);
        }
        else
        {
            return (FALSE);
        }
    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used for this node to tell the other nodes to do in LowPower Mode.
 * @return TRUE is the procedure succeed on queue the command or FALSE is the fail in queue the command.
 */
BOOL_TYPE API009System__RequestWakeUpSource(void)
{
    #if (LOW_POWER_FEATURE == ENABLED)
        REVEAL_COMMAND_TYPE * pt;
        pt = Reveal__NewCommand(API009SYSTEM_NUM,API009_OP_WAKEUP_SOURCE,0xF,0);
        if (pt != (REVEAL_COMMAND_TYPE *)NULL)
        {
            Reveal__QueueCommand(pt);
            return (TRUE);
        }
        return (FALSE);
    #else
        return (FALSE);
    #endif
}

#endif  //API009SYSTEM_FEATURE

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This functions builds the message to send the reset status information
 * @details - This message contains the MICRO_LAST_RESET_TYPE and MICRO_RESET_MODE_TYPE information
 *
 * @param opcode API009_OP_RESET or API009_OP_REQUEST_RESET_STATUS
 * @param node
 *
 * @return BOOL_TYPE
 */
static BOOL_TYPE API009SystemPublishResetStatus(API009_OPCODE_TYPE opcode, uint8 node)
{
    #define API009_OP_RESET_STATUS_PUBLISH_MSG_LENGTH       3
    REVEAL_COMMAND_TYPE * pt = Reveal__NewFeedback(API009SYSTEM_NUM,opcode,node,API009_OP_RESET_STATUS_PUBLISH_MSG_LENGTH);

    if (pt != (REVEAL_COMMAND_TYPE *)NULL)
    {
        Reveal__AddDataU8(pt,*((uint8 *)Micro__GetLastResetstatus()));      //lint !e928  cast from pointer to pointer [MISRA 2004 Rule 11.4]
        Reveal__AddDataU8(pt,(uint8)Micro__GetResetMode());                 //lint !e923  Lint: 923 cast from int to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]

        Reveal__QueueCommand(pt);

        return (TRUE);
    }

    return (FALSE);
}
