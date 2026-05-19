/**
 *  @file
 *
 *  @brief      Public Parameters for WIN protocol Module
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef WIN_PRM_H_
#define WIN_PRM_H_
#include "C_Extensions.h"
#include "SystemConfig.h"
#include "Uart.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * @brief Definition of the size of the payload for outgoing messages
 * @details When used along with WIDE switching protocol it should be limited to 15 to be compatible with wide.
 * @note from 15 to 255
 */
#define WIN_MAX_OUTGOING_DATA_SIZE  70

/**
 * @brief Definition of the size of the payload for incoming messages
 * @details When used along with WIDE switching protocol it should be limited to 15 to be compatible with wide.
 * @note from 15 to 255
 */

#if (SETTINGFILE_MASTER_FILE_FEATURE == ENABLED)
    #define WIN_MAX_INCOMING_DATA_SIZE  250
#else
    #define WIN_MAX_INCOMING_DATA_SIZE  70
#endif
/**
 * @brief Definition Uart COM port used by WIN
 * @note Valid values COM0, COM1, COM2..
 */
#define WIN_COM                     WIN_PORT


/**
 * @brief Configuration of the dynamic address feature.
 * @details When ENABLED allows the system to change this node address in running time.
 * @note Valid values ENABLED/DISABLED
 */
#define WIN_DYNAMIC_ADDRESS         DISABLED

/**
 * @brief Definition of the WIN address
 * @details IF WIN_DYNAMIC_ADDRESS is DISABLED, WIN_NODE is the hard-coded value for the address, if WIN_DYNAMIC_ADDRESS is
 * ENABLED them WIN_NODE is the default address for this node (after the reset value).
 * @note Valid values 0 to 13
 */
#define WIN_NODE                    WIN_NODE_DEF

/**
 * The WIN node that tools will use when they connect to the Nucleus communications bus.
 */
#define WIN_TOOL_ADDRESS            14


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // WIN_PRM_H_
