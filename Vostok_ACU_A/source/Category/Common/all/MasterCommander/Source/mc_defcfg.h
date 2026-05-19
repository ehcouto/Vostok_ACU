/**
 *  @file
 *
 *  @brief      MasterCommander Driver default configuration
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef __MC_DEFCFG_H
#define __MC_DEFCFG_H

/* Read user configuration */
#include "MasterCommander_prm.h"
/******************************************************************************
* Configuration check 
******************************************************************************/

/* polling mode as default when nothing selected */
#if !defined(MC_POLL_DRIVEN) && !defined(MC_LONG_INTR) && !defined(MC_SHORT_INTR)
    #define MC_LONG_INTR   0
    #define MC_SHORT_INTR  0
    #define MC_POLL_DRIVEN 1
#endif

/* otherwise, "undefined" means false for all three options */
#ifndef MC_POLL_DRIVEN
#define MC_POLL_DRIVEN 0
#endif
#ifndef MC_LONG_INTR
#define MC_LONG_INTR 0
#endif
#ifndef MC_SHORT_INTR
#define MC_SHORT_INTR 0
#endif


#if MC_SHORT_INTR
    /* default short-interrupt FIFO size */
    #ifndef MC_COMM_RQUEUE_SIZE
    #define MC_COMM_RQUEUE_SIZE 32
    #endif

    #if !MC_COMM_RQUEUE_SIZE
    #undef  MC_COMM_RQUEUE_SIZE
    #define MC_COMM_RQUEUE_SIZE 32
    #endif
#endif

/* select JTAG BDM, SCI or USB interface */
/* one of these is typically already enabled in mc_cfg.h */
#ifndef MC_USE_JTAG
	#define MC_USE_JTAG 0
#endif

#ifndef MC_USE_USB_CDC
    #define MC_USE_USB_CDC 0
#endif

#ifndef MC_USE_PDBDM
    #define MC_USE_PDBDM 0
#endif

#ifndef MC_DISABLE
    #define MC_DISABLE 0
#endif

/* SCI used by default if BASE address is provided */
#ifndef MC_USE_SCI
    #if defined(MC_SCI_BASE) && !MC_USE_JTAG && !MC_USE_CAN && !MC_USE_MQX_IO && !MC_USE_USB_CDC && !MC_DISABLE && !MC_USE_PDBDM
    #define MC_USE_SCI 1
    #else
    #define MC_USE_SCI 0
    #endif
#endif

/* SCI does not have a transmission double buffer (kind of queue) */
/* (if not specified differently in platform-dependent header file) */
#if MC_USE_SCI
    #ifndef MC_SCI_HAS_TXQUEUE
    #define MC_SCI_HAS_TXQUEUE 0
    #endif
#endif

#if MC_USE_USB_CDC
    #ifndef MC_USB_CDC_ID
    #define  MC_USB_CDC_ID      (0)   /* ID to identify USB CONTROLLER used by mc */
    #endif
#endif

/* Enables or disables byte access to communication buffer */
#ifndef MC_BYTE_BUFFER_ACCESS
#define MC_BYTE_BUFFER_ACCESS 1
#endif

/* Disable call of User callback function */
//#ifndef MC_ISR_CALLBACK
//#define MC_ISR_CALLBACK 0
//#endif

/* read memory commands are ENABLED by default */
#ifndef MC_USE_READMEM
#define MC_USE_READMEM 1
#endif
#ifndef MC_USE_WRITEMEM
#define MC_USE_WRITEMEM 1
#endif
#ifndef MC_USE_WRITEMEMMASK
#define MC_USE_WRITEMEMMASK 1
#endif

/* read variable commands are DISABLED by default */
#ifndef MC_USE_READVAR
#define MC_USE_READVAR 0
#endif
#ifndef MC_USE_WRITEVAR
#define MC_USE_WRITEVAR 0
#endif
#ifndef MC_USE_WRITEVARMASK
#define MC_USE_WRITEVARMASK 0
#endif

/* default scope settings */
#ifndef MC_USE_SCOPE
#define MC_USE_SCOPE 0
#endif

#ifndef MC_MAX_SCOPE_VARS
#define MC_MAX_SCOPE_VARS 8
#endif
/* default recorder settings */
#ifndef MC_USE_RECORDER
#define MC_USE_RECORDER 0
#endif

#ifndef MC_MAX_REC_VARS
#define MC_MAX_REC_VARS 8
#endif

#ifndef MC_REC_FARBUFF
#define MC_REC_FARBUFF 0
#endif

#ifndef MC_REC_OWNBUFF
#define MC_REC_OWNBUFF 0
#endif

#ifndef MC_USE_FASTREC
#define MC_USE_FASTREC 0
#endif

/* Enable code size optimalization */
#ifndef MC_LIGHT_VERSION
#define MC_LIGHT_VERSION 0
#endif

/* Always report single error code from recorder routines  */
#ifndef MC_REC_COMMON_ERR_CODES
#define MC_REC_COMMON_ERR_CODES MC_LIGHT_VERSION
#endif

/* Remove code for single wire communication */
#ifndef MC_SCI_TWOWIRE_ONLY
#define MC_SCI_TWOWIRE_ONLY MC_LIGHT_VERSION
#endif

/* Number of recorder post-trigger samples is by default controlled by PC */
#ifndef MC_REC_STATIC_POSTTRIG
#define MC_REC_STATIC_POSTTRIG 0
#endif

/* Recorder divisor is by default controlled by PC */
#ifndef MC_REC_STATIC_DIVISOR
#define MC_REC_STATIC_DIVISOR 0
#endif

/* Enable Floating point support in Recorder triggering */
#ifndef MC_REC_FLOAT_TRIG
#define MC_REC_FLOAT_TRIG 0
#endif

/* check recorder settings */
#if MC_USE_RECORDER || MC_USE_FASTREC
    
    /* 0 means recorder time base is "unknown" */
    #ifndef MC_REC_TIMEBASE
    #define MC_REC_TIMEBASE 0
    #endif
    
    /* default recorder buffer size is 256 */
    #ifndef MC_REC_BUFF_SIZE
    #define MC_REC_BUFF_SIZE 256
    #endif

#endif  
/* default app.cmds settings */
#ifndef MC_USE_APPCMD
#define MC_USE_APPCMD 0
#endif

#ifndef MC_APPCMD_BUFF_SIZE
#define MC_APPCMD_BUFF_SIZE 16
#endif

#ifndef MC_MAX_APPCMD_CALLS
#define MC_MAX_APPCMD_CALLS 0
#endif

/* TSA configuration check */
#ifndef MC_USE_TSA
#define MC_USE_TSA 0
#endif

#ifndef MC_USE_TSA_SAFETY
#define MC_USE_TSA_SAFETY 0
#endif

/* TSA table allocation modifier */
#ifndef MC_USE_TSA_INROM
#define MC_USE_TSA_INROM 0
#endif

#if MC_USE_TSA_INROM
#define MC_TSA_CDECL const
#else
#define MC_TSA_CDECL
#endif

/* SFIO not used by default */
#ifndef MC_USE_SFIO
#define MC_USE_SFIO 0
#endif

/* use transport "pipe" functionality */
#ifndef MC_USE_PIPES
#define MC_USE_PIPES 0
#endif

/* "pipe" putstring formatting (enabled by default) */
#ifndef MC_USE_PIPE_PRINTF
#define MC_USE_PIPE_PRINTF 1
#endif

/* "pipe" variable-argument printf (enabled by default) */
#ifndef MC_USE_PIPE_PRINTF_VARG
#define MC_USE_PIPE_PRINTF_VARG MC_USE_PIPE_PRINTF
#endif

#if MC_USE_PIPES
    /* one pipe by default */    
    #ifndef MC_MAX_PIPES_COUNT
    #define MC_MAX_PIPES_COUNT 1
    #endif

    /* pipe printf buffer */    
    #ifndef MC_PIPES_PRINTF_BUFF_SIZE
    #define MC_PIPES_PRINTF_BUFF_SIZE 48
    #endif
#endif

/* what kind of board information structure will be sent? */
#ifndef MC_USE_BRIEFINFO
    #if MC_USE_RECORDER
        /* recorder requires full info */
        #define MC_USE_BRIEFINFO 0
    #else
        /* otherwise no brief info is enough */
        #define MC_USE_BRIEFINFO 1
    #endif
#endif

/* automatic buffer size by default */
#ifndef MC_COMM_BUFFER_SIZE
#define MC_COMM_BUFFER_SIZE 0
#endif

/* automatic: determine required buffer size based on features enabled */
#if !MC_COMM_BUFFER_SIZE
    /* smallest for basic commands (getinfobrief, write/read memory etc.) */
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE 11

    /* full info required */
    #if !(MC_USE_BRIEFINFO) && (MC_COMM_BUFFER_SIZE < 35)
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE 35
    #endif

    /* using application commands (must accommodate maximal app.cmd data length) */
    #if MC_USE_APPCMD && (MC_COMM_BUFFER_SIZE < ((MC_APPCMD_BUFF_SIZE)+1))
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE ((MC_APPCMD_BUFF_SIZE)+1)
    #endif

    /* configuring scope (EX) */
    #if MC_USE_SCOPE && (MC_COMM_BUFFER_SIZE < (((MC_MAX_SCOPE_VARS)*5)+1))
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE (((MC_MAX_SCOPE_VARS)*5)+1)
    #endif

    /* configuring recorder (EX) */
    #if MC_USE_RECORDER && (MC_COMM_BUFFER_SIZE < (((MC_MAX_REC_VARS)*5)+18))
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE (((MC_MAX_REC_VARS)*5)+18)
    #endif

    /* SFIO encapsulation (in buffer) */
    #if MC_USE_SFIO
    #if MC_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1)
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE ((SFIO_MAX_INPUT_DATA_LENGTH)+1)
    #endif
    #endif
    
    /* SFIO encapsulation (out buffer) */
    #if MC_USE_SFIO
    #if MC_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1)
    #undef  MC_COMM_BUFFER_SIZE
    #define MC_COMM_BUFFER_SIZE ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1)
    #endif
    #endif
    
#endif

#ifndef MC_BUFFER_ACCESS_BY_FUNCT
	#define MC_BUFFER_ACCESS_BY_FUNCT	0
#endif

#ifndef MC_USE_CAN
	#define MC_USE_CAN					0
#endif

#ifndef MC_USE_MQX_IO
	#define MC_USE_MQX_IO				0
#endif


#endif /* __MC_DEF_CFG_H */
