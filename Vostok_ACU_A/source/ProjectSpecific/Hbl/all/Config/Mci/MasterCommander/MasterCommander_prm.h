/**
 *  @file       MasterCommander_prm.h
 *
 *  @brief
 *
 *  @section
 *
 *  $Header: MasterCommander_prm.h 1.6 2016/01/25 15:27:26CET Maciej  Kobuszewski (KOBUSM) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2016/01/25 15:27:26CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MASTERCOMMANDER_PRM_H_
    #define MASTERCOMMANDER_PRM_H_


#include "Uart.h"
#include "SystemConfig.h"
#include "Mci_prm.h"

#define MC_INTERFACE_UART 1
#define MC_INTERFACE_PDBDM 0


/* Serial communication settings per board */
#define MASTERCOMMANDER_BAUDRATE                    B38400
#if MASTER_COMMANDER == ENABLED
#define MASTERCOMMANDER_COMPORT                     COM1
#else
#define MASTERCOMMANDER_COMPORT                     COM6
#endif
#define MC_COMM_INTERFACE              				MC_INTERFACE_UART


#define MC_LONG_INTR                   0
#define MC_SHORT_INTR                  0
#define MC_POLL_DRIVEN                 1
#define MC_USE_SCOPE                   1
#define MC_USE_RECORDER                1
#define MC_REC_BUFF_SIZE               6000

// RECORDER TIME BASE
// decode the timebase
// zzyy yyyy yyyy yyyy
// z are the measure unit
// zz = 0x0 -> sec
// zz = 0x4 -> ms
// zz = 0x8 -> us
// zz = 0xC -> ns

#ifdef FREEMASTER_HANDLER_5MS
// Frequency 200 Hz
#define MC_REC_TIMEBASE                0x4005U
#endif

#ifdef FREEMASTER_HANDLER_1MS
// Frequency 1000 Hz
#define MC_REC_TIMEBASE                0x4001U
#endif

#ifdef FREEMASTER_HANDLER_200US
// Frequency 5000 Hz
#define MC_REC_TIMEBASE                0x80C8U
#endif
/*.
    MasterCommander Internal Configuration
--------------------------------------------
.*/


/* identify our current platform */
#define MC_PLATFORM_56F8xxx 0
#define MC_PLATFORM_RX62T 0
#define MC_PLATFORM_KXX 1


/*
 * The GCT selects the communication channel using an enumeration constant,
 *  we need to convert it into USE_SCI or USE_JTAG settings
 */

#if (MC_COMM_INTERFACE) == MC_INTERFACE_UART
	#define MC_USE_SCI 1
#elif (MC_COMM_INTERFACE) == MC_INTERFACE_PDBDM
	#define MC_USE_PDBDM 1

#else
    #define MC_DISABLE 1

#endif /* COMM_INTERFACE */

//to reduce code size without change key functionality
#define MC_LIGHT_VERSION 1

//use this macro, when mc Recorder is not configured correctly to see the right response error code, Application code size will by increaset by 10W of Flash
//#define MC_REC_COMMON_ERR_CODES 0

//use this macro, when your application is using Serial Single wire comunication, Application code size will be increased by 19W of Flash
//#define MC_SCI_TWOWIRE_ONLY 0

//use this macro, when you do not need to control pretrigger samples by PC,Application code size will be reduced 12W in Flash and 2W in RAM
//The value in macro represent post trigger samples (1 .. 65535), Keep this value less than "Recorded samples" configured in PC
//#define MC_REC_STATIC_POSTTRIG 0

//Use this macro to disable recorder divisor or to set divisor value staticly
//#define MC_REC_STATIC_DIVISOR 1 //this disables the recorder divisor, in fast recorder saves 7W + 25W of Flash and 3W of RAM, in normal recorder saves 21W of flash and 3W of RAM
//#define MC_REC_STATIC_DIVISOR 2 //sets the recorder divisor to static value (2..65535), in fast recorder saves 5W of flash and 2W of RAM, in normal recorder saves 10W of flash and 2W of RAM

//This macro active fast recorder, where is not trigger value controlled by PC
#define MC_USE_FASTREC 0

//enable/disable float point triggering
#define MC_REC_FLOAT_TRIG  1

#define MC_USE_EX_CMDS 1

#endif // MASTERCOMMANDER_PRM_H_

