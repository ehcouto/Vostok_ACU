/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _MCI_PRV_H_
#define _MCI_PRV_H_

#include "Mci_prm.h"
#include "MciZeroCross.h"
#include "Mcl.h"
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
#ifndef COMPILE_4_SIMULINK
#if (MCL_DEBUG_MODE == ENABLED)
#include "BoardDebugging.h"
#include "MCUDebug.h"
#define MCISLFAC__DEBUG_INITIALIZE()            BoardDebugging__Initialize(); MCUDebug__Initialize()
#ifdef FREEMASTER_HANDLER_200US
#define MCISLFAC__DEBUG_FASTHANDLER()           MCUDebug_MCHandler(); BoardDebugging__PwmHandler(); BoardDebugging__250usHandler()
#else
#define MCISLFAC__DEBUG_FASTHANDLER()           MCUDebug_MCHandler();
#endif
#define MCISLFAC__DEBUG_SLOWHANDLER()           BoardDebugging__25msHandler(); MCUDebug_SlowHandler()
#ifdef FREEMASTER_HANDLER_1MS
#define MCISLFAC__DEBUG_1MSHANDLER()            BoardDebugging__PwmHandler(); BoardDebugging__250usHandler()
#endif
#ifdef FREEMASTER_HANDLER_5MS
#define MCISLFAC__DEBUG_5MSHANDLER()            BoardDebugging__PwmHandler(); BoardDebugging__250usHandler()
#endif
#define MCISLFAC__DEBUG_250USHANDLER()
#define MCISLFAC__DEBUG_ZEROCROSSHANDLER()
#define MCISLFAC__DEBUG_TRIACHANDLER()          //MCUDebug_TriacHandler()
#endif// SIMULATION_MODE

#include "ActivePower.h"
#define MCI_INITIALIZE_ALT_MOTOR(motor)           ActivePower__Initialize(motor)
#define MCI_RUNNING_ALT_MOTOR()                   ActivePower__FastHandler()
#define MCI_GET_ANALOG_ALT_MOTOR(read_data)       ActivePower__GetData(read_data)
#else
#include "MCUDebug.h"
#define MCISLFAC__DEBUG_INITIALIZE()              MCUDebug__Initialize()
#define MCISLFAC__DEBUG_FASTHANDLER()             MCUDebug_MCHandler()
#define MCISLFAC__DEBUG_SLOWHANDLER()             MCUDebug_SlowHandler()
#define MCISLFAC__DEBUG_250USHANDLER()
#define MCISLFAC__DEBUG_ZEROCROSSHANDLER()
#define MCISLFAC__DEBUG_TRIACHANDLER()

#define MCI_INITIALIZE_ALT_MOTOR(motor)
#define MCI_RUNNING_ALT_MOTOR()
#define MCI_GET_ANALOG_ALT_MOTOR(read_data)       0
#endif

#ifdef MCI_POOLING_ZC_HW_SIGNAL
#define MCISLFAC__FASTHANDLER_ENTRY()             MciZeroCross__GetEvent(0)
#endif

#if (TIM6_FOR_MC_HANDLER == ENABLED)
#define MCISLFAC__FASTHANDLER_ENTRY()            TimerInterruptExt__ClearPerIntF(); //Clear Interruption Flag
#endif

#endif // _MCI_PRV_H_
