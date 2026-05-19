/**
 *  @file       
 *  @defgroup   CLASS_B
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Micro.h 1.1 2015/07/21 09:45:03EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/21 09:45:03EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRMICRO_H_
#define SRMICRO_H_

#include "Micro.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//Safety Relevant Methods
#define SRMicro__Initialize()                 Micro__Initialize()
#define SRMicro__ServiceWatchdog()            Micro__ServiceWatchdog()

#define SRMicro__SetLowSpeedMode()            Micro__SetLowSpeedMode()
#define SRMicro__SetNormalSpeedMode()         Micro__SetNormalSpeedMode()
#define SRMicro__SetFullSpeedMode()           Micro__SetFullSpeedMode()
#define SRMicro__GoSleep()                    Micro__GoSleep()

#define SRMicro__Trap()                       Micro__Trap()

//Non Safety relevant Methods
#define SRMicro__GetClock()                   Micro__GetClock()
#define SRMicro__DelayNumNops(delay)          Micro__DelayNumNops(delay)
#define SRMicro__GetFlashSize()               Micro__GetFlashSize()
#define SRMicro__GetRamSize()                 Micro__GetRamSize()
#define SRMicro__GetCoreID()                  Micro__GetCoreID()
#define SRMicro__GetLastResetstatus()         Micro__GetLastResetstatus()


//Methods only for Class B

//Low Voltage detector methods
#define SRMicro__ConfigureLvd()               Micro__ConfigureLvd()
#define SRMicro__ClearLvdIsrRegisters()       Micro__ClearLvdIsrRegisters()
#define SRMicro__GetLvdStatus()               Micro__GetLvdStatus()

//Reset Handling methods
#define SRMicro__ForceReset(reset_mode)       Micro__ForceReset(reset_mode)

#define SRMicro__GetResetMode()               Micro__GetResetMode()

#define SRMicro__SetSkipFlag(flag)            Micro__SetSkipFlag(flag)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRMICRO_H_


