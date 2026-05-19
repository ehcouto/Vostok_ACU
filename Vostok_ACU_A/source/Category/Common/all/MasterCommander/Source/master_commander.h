/**
 *  @file
 *
 *  @brief      MasterCommander Driver main API header file
 *
 *
 *  @details    This is the only header file needed to be included by the user application
 *  to implement the MC interface. In addition, user has to specify the configuration for this
 *  module in the MasterCommander_prm.h file
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef _MASTER_COMMANDER_H
#define _MASTER_COMMANDER_H

/* Master&Commander configuration */
#include "mc_defcfg.h"

/*****************************************************************************
* Global types
******************************************************************************/

typedef unsigned long  MC_ADDR;              /* CPU address type (use integer data type on 56F8xxx) */
typedef unsigned short MC_SIZE;              /* general size type (at least 16 bits) */
typedef unsigned short MC_BOOL;              /* general boolean type  */


/* application command-specific types */
typedef unsigned short MC_APPCMD_CODE;
typedef unsigned short MC_APPCMD_DATA, *MC_APPCMD_PDATA;
typedef unsigned short MC_APPCMD_RESULT;

/* pointer to application command callback handler */
typedef MC_APPCMD_RESULT (*MC_PAPPCMDFUNC)(MC_APPCMD_CODE,MC_APPCMD_PDATA,MC_SIZE);

/* pipe-related types */
typedef void* MC_HPIPE;                      /* pipe handle */
typedef unsigned short MC_PIPE_PORT;         /* pipe port identifier (unsigned, 7 bits used) */
typedef unsigned short MC_PIPE_SIZE;         /* pipe buffer size type (unsigned, at least 8 bits) */

/* pointer to pipe event handler */
typedef void (*MC_PPIPEFUNC)(MC_HPIPE);

/*****************************************************************************
* TSA-related user types and macros
******************************************************************************/

//#include "mc_tsa.h"

/*****************************************************************************
* Fast Recorder inline code and macros
******************************************************************************/

#if defined(MC_USE_FASTREC)
#if MC_USE_FASTREC
#include "mc_fastrec.h"
#endif
#endif

/*****************************************************************************
* Constants
******************************************************************************/

/* application command status information  */
#define MC_APPCMDRESULT_NOCMD      0xffU
#define MC_APPCMDRESULT_RUNNING    0xfeU
#define MFSTR_APPCMDRESULT_LASTVALID  0xf7U  /* F8-FF are reserved  */

/* recorder time base declaration helpers */
#define MC_REC_BASE_SECONDS(x)  ((x) & 0x3fffU)
#define MC_REC_BASE_MILLISEC(x) (((x) & 0x3fffU) | 0x4000U)
#define MC_REC_BASE_MICROSEC(x) (((x) & 0x3fffU) | 0x8000U)
#define MC_REC_BASE_NANOSEC(x)  (((x) & 0x3fffU) | 0xc000U)

/******************************************************************************
* NULL needed
******************************************************************************/

#ifndef NULL
#ifdef __cplusplus
#define NULL (0)
#else
#define NULL ((void *) 0)
#endif
#endif

/*****************************************************************************
* Global functions
******************************************************************************/
#ifdef __cplusplus
  extern "C" {
#endif

/* mc serial communication API */
MC_BOOL MC_Init(void);    /* general initialization */
void MC_Poll(void);          /* polling call, use in SHORT_INTR and POLL_DRIVEN modes */
void MC_Isr(void);           /* SCI/JTAG/CAN interrupt handler for LONG_INTR and SHORT_INTR modes */

/* Recorder API */
void MC_Recorder(void);
void MC_TriggerRec(void);
void MC_SetUpRecBuff(MC_ADDR nBuffAddr, MC_SIZE nBuffSize);

/* Application commands API */
MC_APPCMD_CODE  MC_GetAppCmd(void);
MC_APPCMD_PDATA MC_GetAppCmdData(MC_SIZE* pDataLen);
MC_BOOL         MC_RegisterAppCmdCall(MC_APPCMD_CODE nAppCmdCode, MC_PAPPCMDFUNC pCallbackFunc);

void MC_AppCmdAck(MC_APPCMD_RESULT nResultCode);
void MC_AppCmdSetResponseData(MC_ADDR nResultDataAddr, MC_SIZE nResultDataLen);

/* Loss-less Communication Pipes API */
MC_HPIPE MC_PipeOpen(MC_PIPE_PORT nPort, MC_PPIPEFUNC pCallback,
                           MC_ADDR pRxBuff, MC_PIPE_SIZE nRxSize,
                           MC_ADDR pTxBuff, MC_PIPE_SIZE nTxSize);
void MC_PipeClose(MC_HPIPE hpipe);
MC_PIPE_SIZE MC_PipeWrite(MC_HPIPE hpipe, MC_ADDR addr, MC_PIPE_SIZE length);
MC_PIPE_SIZE MC_PipeWrite8(MC_HPIPE hpipe, MC_ADDR addr, MC_PIPE_SIZE length);
MC_PIPE_SIZE MC_PipeRead(MC_HPIPE hpipe, MC_ADDR addr, MC_PIPE_SIZE length);
MC_PIPE_SIZE MC_PipeRead8(MC_HPIPE hpipe, MC_ADDR addr, MC_PIPE_SIZE length);

/* Pipe printing and formatting */
MC_BOOL MC_PipePuts(MC_HPIPE hpipe, const char* pszStr);
MC_BOOL MC_PipePrintf(MC_HPIPE hpipe, const char* pszFmt, ...);
MC_BOOL MC_PipePrintfU8(MC_HPIPE hpipe, const char* pszFmt, unsigned char arg);
MC_BOOL MC_PipePrintfS8(MC_HPIPE hpipe, const char* pszFmt, signed char arg);
MC_BOOL MC_PipePrintfU16(MC_HPIPE hpipe, const char* pszFmt, unsigned short arg);
MC_BOOL MC_PipePrintfS16(MC_HPIPE hpipe, const char* pszFmt, signed short arg);
MC_BOOL MC_PipePrintfU32(MC_HPIPE hpipe, const char* pszFmt, unsigned long arg);
MC_BOOL MC_PipePrintfS32(MC_HPIPE hpipe, const char* pszFmt, signed long arg);

#ifdef __cplusplus
  }
#endif

#endif /* _MASTER_COMMANDER_H */

