/**
 *  @file
 *
 *  @brief      API009System private parameters configuration
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
#ifndef API009SYSTEM_PRV_H_
#define API009SYSTEM_PRV_H_
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "AlternateCtrl.h"
    #include "McuGetDataMonitor.h"
    #include "StandbyManager.h"
#endif
#include "SRPowerMgr.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every a node which we sent a message to force a reset respond with ack reason code 1
 */
#define API009SYSTEM__RESET_ACK(node)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time it receives a reset message for a node in the system
 * @brief
 *
 * @param reason        - This is of MICRO_LAST_RESET_TYPE
 * @param reset_mode    - This is of MICRO_RESET_MODE_TYPE
 */
#if (SYSTEM_ACU_HANDLING == ENABLED)
#define API009SYSTEM__NODE_RESET_MSG(node, reason, reset_mode)  \
	do \
	{ \
		AlternateCtrl__McuReset(node, reason, reset_mode);\
		McuGetDataMonitor__McuReset(node, reason, reset_mode);\
	} while (0)
#else
#define API009SYSTEM__NODE_RESET_MSG(node, reason, reset_mode)
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time it receives a feedback from another node publishing his endianess
 * @details endianess = 1 means BIGENDIAN, 0 means LITTLEENDIAN
 */
#define API009SYSTEM__ENDIANESS_REQUESTG(node,endianess)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time this node receives a command from another node requesting it to go in LowPower Mode
 * @details Once the Reveal command is received there is a timer before this call-back is called. The timer is
 defined by API009SYSTEM_GO_LOW_POWER_TIMER
 */
#define API009SYSTEM__GO_LOW_POWER_RECEIVED()       SRPowerMgr__GoLowPowerMode();

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This directive defined a delay in 25ms time base to fire the call-back API009SYSTEM__GO_LOW_POWER_RECEIVED()
 * @details values range from 0 to 255 (in a 25ms time base), 0 disables the call-back
 */
#define API009SYSTEM_GO_LOW_POWER_TIMER			20 // 500ms

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called to check if this node was the source of the wake-up signal
 * @details This function should return FALSE if the wake-up wasn't generated in this board and TRUE is it was generated in this board
 */
#define API009SYSTEM__CHECK_WAKEUP_SOURCE()	   SRPowerMgr__IsThisNodeWakeUpSource()

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called to check if this node is able to go in low power mode.
 * @details This function should return FALSE if this node can not go in low power or TRUE is it accepted the command and it is going to low power mode.
 */
#if (SYSTEM_ACU_HANDLING == ENABLED)
#define API009SYSTEM__CHECK_GO_LOW_POWER_ALLOWED()    StandbyManager__GetPermission()//this function is defined inside new module named "StandbyManager.c/.h"
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called once another node publish a message letting the other nodes know that it generated the wake-up
 * @details the parameter is the source of the wake-up signal
 */
#define API009SYSTEM__PUBLISH_WAKEP_SOURCE(node)  SRPowerMgr__ReceivedWakeUpNodeID(node);


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called when a message other nodes indicate that they can't engage the low power mode.
 */
#define API009SYSTEM__CANCEL_LOW_POWER()

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time an ack is received for a go sleep command which means that the command was sent and received.
 * @details the parameter is the source of the ack message and the reason code.\n
 * reasoncode = NVO_RS_READY (0) means that the command was accepted and the node will go in LowPower. \n
 * reasoncode = NVO_RS_REJECTED(2) means that the node can not go in LowPower. \n
 */
#define API009SYSTEM__GO_LOW_POWER_ACK(address,reasoncode) //AppMgr__LowPowerAckReceived(node)


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called when the command to force a watchdog reset is received.
 */
#if (SYSTEM_ACU_HANDLING == ENABLED)
#define API009SYSTEM_WATCHDOG_RESET()       StandbyManager__ResetActivateStandby()//while (1)
#endif


#endif // API009SYSTEM_PRV_H_
