/**
 *  @file       SRPowerMgr_prv.h
 *
 *  @brief      Private (prv) configuration file for SRPowerMgr module
 *
 *  @defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _SRPOWERMGR_PRV_H_
#define _SRPOWERMGR_PRV_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "DoorOpeningSystemMonitor.h"
#include "LITTMonitor.h"

#define SRPOWERMGR_WAKEUP_SEQUENCE_COMPLETE()

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - List the wakeup nodes and wake-up procedure in the below list
 *    @details- Define this list only if the SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR is enabled
 *
 *    @param -
 *                  #if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
 *                      //                                                          WakeUp_Node         WakeUp_Port     WakeUp_Pin      WakeUp_Value        WakeUp_Func
 *                      const SRPOWERMGR_WAKEUP_TYPE SRPOWERMGR_WAKEUP_LIST[] = {
 *                                                                                  {WIN_UI_ADDRESS     ,PORTB          ,11             ,FALSE               ,SRPOWER_ACTIVATION_FUNC_NULL      },
 *                                                                                  {WIN_EXP_ADDRESS    ,PORT_NULL      ,0              ,FALSE               ,SRPOWER_ACTIVATION_FUNC_NULL      }
 *                                                                              };
 */

#if ((SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED) && (SRPOWERMGR_LOWPOWER_MODE != 0))
//                                                          WakeUp_Node         WakeUp_Port     WakeUp_Pin      WakeUp_Value        WakeUp_Func
const SRPOWERMGR_WAKEUP_TYPE SRPOWERMGR_WAKEUP_LIST[] = {
                                                            {WIN_UI_ADDRESS     ,PORT_NULL      ,0              ,FALSE               ,SRPOWER_ACTIVATION_FUNC_NULL      }
                                                        };
#endif

//! Customizable actions to take when the board receives a command to go to low power mode
#define SRPOWERMGR__GO_LOW_POWER_RECEIVED()		if (DoorOpeningSystemMonitor__IsDoorOpenedByDOS() == TRUE) \
												{ \
													if (DoorOpeningSystemMonitor__IsDOSPinFullyRetracted() == TRUE) \
													{ \
														DoorOpeningSystemMonitor__SetDoorOpenedByDOS(FALSE); \
													} \
												} \
												LITTMonitor__RestartLITTTimer(); \
												NVData__Store();

#define SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_MS               						300
#define SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_MS                1000
#define SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS              							1000
#define SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS               					2000
#endif // _SRPOWERMGR_PRV_H_
