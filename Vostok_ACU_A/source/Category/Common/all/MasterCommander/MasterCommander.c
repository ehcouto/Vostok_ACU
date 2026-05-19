/**
 *  @file       MasterCommander.c
 *
 *  @brief
 *
 *  @details
 *
 *  @section
 *
 *  @copyright  Copyright 2013-$Date: 2015/10/06 16:02:53CEST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------

#include "Uart.h"
#include "MasterCommander.h"
#include "master_commander.h"
#include "mc_protocol.h"


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MasterCommander and its variables
 *
 */
void MasterCommander__Initialize(void)
{
    Uart__Initialize(MASTERCOMMANDER_COMPORT, MASTERCOMMANDER_BAUDRATE, UART_STOP_BIT_1, UART_PARITY_NONE);

    MC_Init();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Pwm handler for MasterCommander. Rate: every 62.5 us
 *
 */
void MasterCommander__PwmHandler(void)
{
	MC_Recorder();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Fast handler for MasterCommander. Rate: every 250 us
 *
 */
void MasterCommander__FastHandler(void)
{
    MC_Poll();
}


