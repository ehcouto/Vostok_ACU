/**
 *  @file
 *
 *  @brief      MC Recorder implementation.
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------


#ifndef __MC_REC_H
#define __MC_REC_H

#ifndef _MASTER_COMMANDER_H
#error Please include mc_rec.h after mc.h main header file.
#endif

#include "master_commander.h"
#include "mc_private.h"

/***********************************
*  global recorder types
***********************************/

/* recorder runtime flags */
typedef volatile union 
{
    MC_FLAGS all;
    
    struct 
    {
        unsigned bIsConfigured : 1;     /* recorder is configured */
        unsigned bIsRunning : 1;        /* recorder is running */
        unsigned bIsStopping : 1;       /* trigger activated, sample countdown */
        unsigned bInvirginCycle : 1;    /* virgin cycle of the circular buffer in-progress */
        unsigned bTrgCrossActive : 1;   /* trigger threshold was crossed */
    } flg;
    
} MC_REC_FLAGS;

/* the same flags for optimized asm access (see fast recorder) */
#if defined(MC_PLATFORM_56F8xxx) /* flag allocation tested on 56f8xxx only */
#define MC_REC_FLAG_bIsConfigured   0x01
#define MC_REC_FLAG_bIsRunning      0x02
#define MC_REC_FLAG_bIsStopping     0x04
#define MC_REC_FLAG_bInvirginCycle  0x08
#define MC_REC_FLAG_bTrgCrossActive 0x10
#endif

#define MC_REC_FLOAT_TRIG_MASK      0x02

/***********************************
*  global recorder variables 
***********************************/

/* configuration variables */
extern MC_U16  pcm_wRecTotalSmps;        /* number of samples to measure */
extern MC_U16  pcm_wRecPostTrigger;      /* number of post-trigger samples to keep */
extern MC_U8   pcm_nRecTriggerMode;      /* trigger mode (0 = disabled, 1 = _/, 2 = \_) */
extern MC_U16  pcm_wRecTimeDiv;          /* divisor of recorder "clock" */

extern MC_U8    pcm_nRecVarCount;        /* number of active recorder variables */
extern MC_ADDR  pcm_pRecVarAddr[MC_MAX_SCOPE_VARS]; /* addresses of recorded variables */
extern MC_SIZE8 pcm_pRecVarSize[MC_MAX_SCOPE_VARS]; /* sizes of recorded variables */

/* runtime variables  */
extern MC_U16  pcm_wRecBuffStartIx;      /* first sample index */

extern MC_ADDR pcm_dwRecWritePtr;        /* write pointer in recorder buffer */
extern MC_ADDR pcm_dwRecEndBuffPtr;      /* pointer to end of active recorder buffer */
extern MC_U16  pcm_wRecTimeDivCtr;       /* recorder "clock" divisor counter */
extern MC_U16  pcm_wStoprecCountDown;    /* post-trigger countdown counter */

extern MC_REC_FLAGS pcm_wRecFlags;


#endif /* __MC_REC_H */
