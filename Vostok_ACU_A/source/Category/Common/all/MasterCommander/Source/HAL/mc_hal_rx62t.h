/**
 *  @file
 *
 *  @brief      MasterCommander Driver hardware dependent stuff for RX62T
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef __MC_RX62T_H
#define __MC_RX62T_H

#include "master_commander.h"
#include "Uart.h"

/******************************************************************************
* platform-specific default configuration
******************************************************************************/

/* using 16bit addressing commands */
#ifndef MC_USE_NOEX_CMDS
#define MC_USE_NOEX_CMDS 1
#endif

/* using 32bit addressing commands */
#ifndef MC_USE_EX_CMDS
#define MC_USE_EX_CMDS   1
#endif

/* using inline access to buffer memory */
#ifndef MC_USE_INLINE_BUFFER_ACCESS
#define MC_USE_INLINE_BUFFER_ACCESS 0
#endif

/* compiler inline statement */
#define MC_INLINE inline

/*****************************************************************************
* Board configuration information 
******************************************************************************/

#define MC_PROT_VER           3U      /* protocol version 3 */
#define MC_CFG_FLAGS          0U      /* board info flags */
#define MC_CFG_BUS_WIDTH      1U      /* data bus width */
#define MC_GLOB_VERSION_MAJOR 2U      /* driver version */
#define MC_GLOB_VERSION_MINOR 0U
#define MC_IDT_STRING "RX62T MasterCommander Driver"
#define MC_TSA_FLAGS MC_TSA_INFO_HV2BA    /* HawkV2 TSA workaround */

/******************************************************************************
* platform-specific types
******************************************************************************/

typedef unsigned char  MC_U8;        /* smallest memory entity (mostly 8bit) */
typedef unsigned short MC_U16;       /* 16bit value */
typedef unsigned long  MC_U32;       /* 32bit value */

typedef signed char    MC_S8;        /* signed 8bit value */
typedef signed short   MC_S16;       /* signed 16bit value */
typedef signed long    MC_S32;       /* signed 32bit value */

#if MC_REC_FLOAT_TRIG
typedef float          MC_FLOAT;     /* float value */
#endif

typedef unsigned short MC_FLAGS;     /* type to be union-ed with flags (at least 8 bits) */
typedef unsigned short MC_SIZE8;     /* size value (at least 8 bits) */
typedef signed short   MC_INDEX;     /* general for-loop index (must be signed) */

typedef unsigned char  MC_BCHR;      /* type of a single character in comm.buffer */
typedef unsigned char* MC_BPTR;      /* pointer within a communication buffer */

typedef unsigned short MC_SCISR;     /* data type to store SCI status register */

///******************************************************************************
//* communication buffer access functions (non-inline)
//******************************************************************************/

/* MC_SetExAddr is needed only if both EX and non-EX commands are used */
#if (MC_USE_EX_CMDS) && (MC_USE_NOEX_CMDS)
void MC_SetExAddr(MC_BOOL bNextAddrIsEx);
#else
/* otherwise, we know what addresses are used, (ignore MC_SetExAddr) */
#define MC_SetExAddr(bNextAddrIsEx)
#endif


/****************************************************************************************
* Platform-specific configuration
*****************************************************************************************/

/* sanity check, at least one of the modes should be enabled */
#if (!MC_USE_EX_CMDS) && (!MC_USE_NOEX_CMDS)
#error At least one of MC_USE_EX_CMDS or MC_USE_NOEX_CMDS should be set non-zero
#endif

/* JTAG base address definition */
#if MC_USE_JTAG
    /* hardwired JTAG address on all HawkV2 devices */
    #ifndef MC_JTAG_BASE
    #define MC_JTAG_BASE 0xFFFF00
    #endif
#endif

/* MSCAN functionality tested on 56F8xxx */
#define MC_CANHW_MSCAN 1

/* FlexCAN functionality tested on 56F8xxx */
#define MC_CANHW_FLEXCAN 1

/* FlexCAN functionality tested on 56F84xxx family */
#define MC_CANHW_FLEXCAN32 1

/* pipes not yet tested */
#define MC_PIPES_EXPERIMENTAL 1

/****************************************************************************************
* General peripheral space access macros
*****************************************************************************************/

#define MC_SETBIT(base, offset, bit)     (*(volatile MC_U8*)(((MC_U32)(base))+(offset)) |= bit)
#define MC_CLRBIT(base, offset, bit)     (*(volatile MC_U8*)(((MC_U32)(base))+(offset)) &= (MC_U8)~((MC_U8)(bit)))
#define MC_TSTBIT(base, offset, bit)     (*(volatile MC_U8*)(((MC_U32)(base))+(offset)) & (bit))
#define MC_SETREG(base, offset, value)   (*(volatile MC_U8*)(((MC_U32)(base))+(offset)) = value)
#define MC_GETREG(base, offset)          (*(volatile MC_U8*)(((MC_U32)(base))+(offset)))

#define MC_SETBIT32(base, offset, bit)   (*(volatile MC_U32*)(((MC_U32)(base))+(offset)) |= (bit))
#define MC_CLRBIT32(base, offset, bit)   (*(volatile MC_U32*)(((MC_U32)(base))+(offset)) &= (MC_U32)~((MC_U32)(bit)))
#define MC_TSTBIT32(base, offset, bit)   (*(volatile MC_U32*)(((MC_U32)(base))+(offset)) & (bit))
#define MC_SETREG32(base, offset, value) (*(volatile MC_U32*)(((MC_U32)(base))+(offset)) = value)
#define MC_GETREG32(base, offset)        (*(volatile MC_U32*)(((MC_U32)(base))+(offset)))

#define MC_SETBITFIELD(base, offset, mask, value)     (*(volatile MC_U16*)(((MC_U32)(base))+(offset)) = ((*(volatile MC_U16*)(((MC_U32)(base))+(offset))) & ~(mask)) | (value))
#define MC_SETBITFIELD32(base, offset, mask, value)   (*(volatile MC_U32*)(((MC_U32)(base))+(offset)) = (MC_U32)(((*(volatile MC_U32*)(((MC_U32)(base))+(offset))) & ~(mask)) | (value)))


/*******************************************************************************************
* SCI access macros
*****************************************************************************************/

/* transmitter enable/disable */
/*lint -emacro(923,MC_SCI_TE, MC_SCI_TD) : casting long to pointer */
#define MC_SCI_TE() Uart__Enable(MASTERCOMMANDER_COMPORT,UART_OPTION_TX_EN)
//MC_SETBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_TE)
#define MC_SCI_TD() Uart__Disable(MASTERCOMMANDER_COMPORT,UART_OPTION_TX_EN) ///this does not disable but is never used
//MC_CLRBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_TE)

/* receiver enable/disable */
/*lint -emacro(923,MC_SCI_RE, MC_SCI_RD) : casting long to pointer */
#define MC_SCI_RE() Uart__Enable(MASTERCOMMANDER_COMPORT,UART_OPTION_RX_EN)
//MC_SETBIT(MC_SCI_BASE, MC_SCIsCR_OFFSET, MC_SCICR_RE)
#define MC_SCI_RD() Uart__Disable(MASTERCOMMANDER_COMPORT,UART_OPTION_RX_EN) ///this does not disable but is never used
//MC_CLRBIT(MC_SCI_BASE, MC_SCIsCR_OFFSET, MC_SCICR_RE)

//#define MC_SCI_TE_RE() MC_SETBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_RE | MC_SCICR_TE)

/* Transmitter-empty interrupt enable/disable : casting long to pointer */
/*lint -emacro(923,MC_SCI_ETXI, MC_SCI_DTXI) */
#define MC_SCI_ETXI() Uart__Enable(MASTERCOMMANDER_COMPORT,UART_OPTION_TX_EN)//MC_SETBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_TEIE)
#define MC_SCI_DTXI() Uart__Disable(MASTERCOMMANDER_COMPORT,UART_OPTION_TX_EN)//MC_CLRBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_TEIE) ///this does not disable but is never used

/* Receiver-full interrupt enable/disable : casting long to pointer */
/*lint -emacro(923,MC_SCI_ERXI, MC_SCI_DRXI) */
#define MC_SCI_ERXI() Uart__Enable(MASTERCOMMANDER_COMPORT,UART_ENABLE_OPTIONS_DEF)//MC_SETBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_RIE) //never used
#define MC_SCI_DRXI() Uart__Disable(MASTERCOMMANDER_COMPORT,UART_ENABLE_OPTIONS_DEF)//MC_CLRBIT(MC_SCI_BASE, MC_SCISCR_OFFSET, MC_SCICR_RIE) //never used

/* Tranmsit character */
/*lint -emacro(923,MC_SCI_PUTCHAR) : casting long to pointer */
#define MC_SCI_PUTCHAR(ch) Uart__WriteByte(MASTERCOMMANDER_COMPORT, ch)//Uart__Write(UART_ENUM_DEF, ch, 1)//MC_SETREG(MC_SCI_BASE, MC_SCITDR_OFFSET, ch)//Uart__Write(UART_ENUM_DEF, uint8 * buf, uint8 size)//MC_SETREG(MC_SCI_BASE, MC_SCITDR_OFFSET, ch)

/* Get received character */
/*lint -emacro(923,MC_SCI_GETCHAR) : casting long to pointer */
#define MC_SCI_GETCHAR() Uart__ReadByte(MASTERCOMMANDER_COMPORT)//MC_GETREG(MC_SCI_BASE, MC_SCIRDR_OFFSET)

/* read status register */
/*lint -emacro(923,MC_SCI_GETSR) : casting long to pointer */
#define MC_SCI_GETSR()   Uart__GetStatus(MASTERCOMMANDER_COMPORT, UART_STATUS_TX_AVAILABLE) //MC_GETREG(MC_SCI_BASE, MC_SCISSR_OFFSET)

/* read & clear status register (clears error status bits only) */
/*lint -emacro(923,MC_SCI_RDCLRSR) : casting long to pointer */
#define MC_SCI_RDCLRSR() Uart__GetStatus(MASTERCOMMANDER_COMPORT, UART_STATUS_DEF)//MC_SETREG(MC_SCI_BASE, MC_SCISSR_OFFSET,  MC_GETREG(MC_SCI_BASE, MC_SCISSR_OFFSET) &= ~MC_SCISR_ERRORS)


#endif /* __MC_RX62T_H */
