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

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "master_commander.h"
#include "mc_llmem.h"
#include "mc_private.h"
#include "mc_protocol.h"

#if (MC_USE_RECORDER) && (!(MC_DISABLE))

#include "../Source/mc_rec.h"

#if MC_USE_FASTREC
#include "mc_fastrec.h"
#endif

/********************************************************
*  global variables (shared with FastRecorder if used)
********************************************************/

/* configuration variables */
MC_U16  pcm_wRecTotalSmps;        /* number of samples to measure */

#if MC_REC_STATIC_POSTTRIG == 0
MC_U16  pcm_wRecPostTrigger;      /* number of post-trigger samples to keep */
#endif

#if (MC_USE_FASTREC) == 0
MC_U8   pcm_nRecTriggerMode;      /* trigger mode (0 = disabled, 1 = _/, 2 = \_) */
#endif

#if (MC_REC_STATIC_DIVISOR) == 0
MC_U16  pcm_wRecTimeDiv;          /* divisor of recorder "clock" */
#endif

MC_U8    pcm_nRecVarCount;        /* number of active recorder variables */
MC_ADDR  pcm_pRecVarAddr[MC_MAX_SCOPE_VARS]; /* addresses of recorded variables */
MC_SIZE8 pcm_pRecVarSize[MC_MAX_SCOPE_VARS]; /* sizes of recorded variables */

/* runtime variables  */
#if (MC_REC_STATIC_DIVISOR) != 1
MC_U16  pcm_wRecTimeDivCtr;       /* recorder "clock" divisor counter */
#endif

MC_U16  pcm_wStoprecCountDown;    /* post-trigger countdown counter */

/* recorder flags */
MC_REC_FLAGS pcm_wRecFlags;

/***********************************
*  local variables 
***********************************/

#if (MC_USE_FASTREC) == 0
MC_U16   pcm_wRecBuffStartIx;     /* first sample index */

/* Recorder buffer pointers */
MC_ADDR pcm_dwRecWritePtr;        /* write pointer in recorder buffer */
MC_ADDR pcm_dwRecEndBuffPtr;      /* pointer to end of active recorder buffer */

/* configuration variables */
static MC_ADDR pcm_nTrgVarAddr;          /* trigger variable address */
static MC_U8   pcm_nTrgVarSize;          /* trigger variable threshold size */
static MC_U8   pcm_bTrgVarSigned;        /* trigger compare mode (0 = unsigned, 1 = signed) */

/*lint -e{960} using union */
static union 
{
#if MC_CFG_BUS_WIDTH == 1
    MC_U8  u8;
    MC_S8  s8;
#endif
    MC_U16 u16;
    MC_S16 s16;
    MC_U32 u32;
    MC_S32 s32;
#if MC_REC_FLOAT_TRIG
    MC_FLOAT fp;
#endif
} pcm_uTrgThreshold;                        /* trigger threshold level (1,2 or 4 bytes) */
#endif /* (MC_USE_FASTREC) == 0 */

static MC_ADDR  pcm_nRecBuffAddr;        /* recorder buffer address */
#if MC_REC_OWNBUFF
static MC_SIZE  pcm_wRecBuffSize;        /* recorder buffer size */
#endif
/* compare functions prototype */
typedef MC_BOOL (*MC_PCOMPAREFUNC)(void);

/*/ pointer to active compare function */
static MC_PCOMPAREFUNC pcm_pCompareFunc;

#if !MC_REC_OWNBUFF && (MC_USE_FASTREC) == 0
/* put buffer into far memory ? */
#if MC_REC_FARBUFF
#pragma section fardata begin
#endif /* MC_REC_FARBUFF */
/* statically allocated recorder buffer (MC_REC_OWNBUFF is FALSE) */
static MC_U8 pcm_pOwnRecBuffer[MC_REC_BUFF_SIZE];
/* end of far memory section */
#if MC_REC_FARBUFF
#pragma section fardata end
#endif /* MC_REC_FARBUFF */
#endif /* MC_REC_OWNBUFF */

/***********************************
*  local functions
***********************************/

static MC_BOOL MC_Compare8S(void);
static MC_BOOL MC_Compare8U(void);
static MC_BOOL MC_Compare16S(void);
static MC_BOOL MC_Compare16U(void);
static MC_BOOL MC_Compare32S(void);
static MC_BOOL MC_Compare32U(void);
#if MC_REC_FLOAT_TRIG
static MC_BOOL MC_Comparefloat(void);
#endif
static void MC_Recorder2(void);

/**************************************************************************//*!
*
* @brief    Recorder Initialization
*
******************************************************************************/

void MC_InitRec(void)
{   
    /* initialize Recorder flags*/
    pcm_wRecFlags.all = 0U;

    /* setup buffer pointer and size so IsInRecBuffer works even  
       before the recorder is first initialized and used */
    
#if MC_REC_OWNBUFF
    /* user wants to use his own buffer */
    pcm_nRecBuffAddr = 0U;
    pcm_wRecBuffSize = 0U;
#elif MC_USE_FASTREC
    /* Initialize Fast Recorder Buffer  */
    MC_InitFastRec();
#else
    /* size in native sizeof units (=bytes on most platforms) */
    MC_ARR2ADDR(pcm_nRecBuffAddr, pcm_pOwnRecBuffer);
    
    /*lint -esym(528, pcm_pOwnRecBuffer) this symbol is used outside of lint sight */
#endif

}

/**************************************************************************//*!
*
* @brief    Abort and de-initialize recorder
*
******************************************************************************/

#if defined(MC_PLATFORM_HC08) || defined(MC_PLATFORM_HC12)
#pragma INLINE
#elif defined(__ARMCC_VERSION)
__inline
#else
static inline
#endif
void MC_AbortRec(void)
{
    /* clear flags */
    pcm_wRecFlags.all = 0U;
}

/**************************************************************************//*!
*
* @brief    API: Replacing the recorder buffer with the user's one
*
* @param    pBuffer - user buffer pointer
* @param    wBuffSize - buffer size
*
* @note Use the MC_SetUpBuff32 to pass the forced 32bit address in SDM
*
******************************************************************************/

void MC_SetUpRecBuff(MC_ADDR pBuffer, MC_SIZE nBuffSize)
{    
#if MC_REC_OWNBUFF
    pcm_nRecBuffAddr = pBuffer;
    pcm_wRecBuffSize = nBuffSize;
#else
    MC_UNUSED(pBuffer);
    MC_UNUSED(nBuffSize);
#endif
}

/**************************************************************************//*!
*
* @brief    Handling SETUPREC and SETUPREC_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_SetUpRec(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_SIZE8 nRecVarsetSize;
    MC_SIZE blen;
    MC_U8 i, sz;
    MC_U8 nResponseCode;
    
    /* de-initialize first   */
    MC_AbortRec();

#if MC_REC_OWNBUFF
    /* user wants to use his own buffer, check if it is valid */
    if(!pcm_nRecBuffAddr || !pcm_wRecBuffSize)
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_INVBUFF);
    }
#elif (MC_USE_FASTREC) == 0
    /* size in native sizeof units (=bytes on most platforms) */
    MC_ARR2ADDR(pcm_nRecBuffAddr, pcm_pOwnRecBuffer);
#endif

    /* seek the setup data */
#if (MC_USE_FASTREC) == 0
    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = MC_ValueFromBuffer8(&pcm_nRecTriggerMode, pMessageIO);
#else /* (MC_USE_FASTREC) == 0 */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 3U);
#endif /* (MC_USE_FASTREC) == 0 */

    pMessageIO = MC_ValueFromBuffer16(&pcm_wRecTotalSmps, pMessageIO);

#if (MC_REC_STATIC_POSTTRIG) == 0
    pMessageIO = MC_ValueFromBuffer16(&pcm_wRecPostTrigger, pMessageIO);
#else /* (MC_REC_STATIC_POSTTRIG) == 0 */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
#endif /* (MC_REC_STATIC_POSTTRIG) == 0 */

#if (MC_REC_STATIC_DIVISOR) == 0
    pMessageIO = MC_ValueFromBuffer16(&pcm_wRecTimeDiv, pMessageIO);
#else /* (MC_REC_STATIC_DIVISOR) == 0 */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
#endif /* (MC_REC_STATIC_DIVISOR) == 0 */

#if (MC_USE_FASTREC) == 0
    /* address & size of trigger variable */
    pMessageIO = MC_AddressFromBuffer(&pcm_nTrgVarAddr, pMessageIO);
    pMessageIO = MC_ValueFromBuffer8(&pcm_nTrgVarSize, pMessageIO);

    /* trigger compare mode  */
    pMessageIO = MC_ValueFromBuffer8(&pcm_bTrgVarSigned, pMessageIO);

    /* threshold value  */
    pMessageIO = MC_ValueFromBuffer32(&pcm_uTrgThreshold.u32, pMessageIO);
#else /* (MC_USE_FASTREC) == 0 */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 8U);
#endif /* (MC_USE_FASTREC) == 0 */
    
    /* recorder variable count */
    pMessageIO = MC_ValueFromBuffer8(&pcm_nRecVarCount, pMessageIO);

    /* rec variable information must fit into our buffers */
    if(!pcm_nRecVarCount || pcm_nRecVarCount > (MC_U8)MC_MAX_REC_VARS)
    {
#if MC_REC_COMMON_ERR_CODES
        goto MC_SetUpRec_exit_error;
#else
        nResponseCode = MC_STC_INVBUFF;
        goto MC_SetUpRec_exit;
#endif
    }

    /* calculate sum of sizes of all variables */
    nRecVarsetSize = 0U;

    /* get all addresses and sizes */
    for(i=0U; i<pcm_nRecVarCount; i++)
    {
        /* variable size */
        pMessageIO = MC_ValueFromBuffer8(&sz, pMessageIO);
        
        pcm_pRecVarSize[i] = sz;
        nRecVarsetSize += sz;
        
        /* variable address */
        pMessageIO = MC_AddressFromBuffer(&pcm_pRecVarAddr[i], pMessageIO);

        /* valid numeric variable sizes only */
        if(sz == 0U || sz > 8U)
        {
#if MC_REC_COMMON_ERR_CODES
            goto MC_SetUpRec_exit_error;
#else
            nResponseCode = MC_STC_INVSIZE;
            goto MC_SetUpRec_exit;
#endif
        }

#if MC_CFG_BUS_WIDTH > 1U
        /* even sizes only */
        if(sz & 0x1)
        {
#if MC_REC_COMMON_ERR_CODES
            goto MC_SetUpRec_exit_error;
#else
            nResponseCode = MC_STC_INVSIZE;
            goto MC_SetUpRec_exit;
#endif
        }
#endif /* MC_CFG_BUS_WIDTH > 1U */
        
#if MC_USE_TSA && MC_USE_TSA_SAFETY
        if(!MC_CheckTsaSpace(pcm_pRecVarAddr[i], (MC_SIZE8)sz, 0U))
        {
#if MC_REC_COMMON_ERR_CODES
            goto MC_SetUpRec_exit_error;
#else
            nResponseCode = MC_STC_EACCESS;
            goto MC_SetUpRec_exit;
#endif
        }
#endif /* MC_USE_TSA && MC_USE_TSA_SAFETY */
    }

    /* fast recorder handles trigger by itself */
#if (MC_USE_FASTREC) == 0
    /* any trigger? */
    pcm_pCompareFunc = NULL;
    if(pcm_nRecTriggerMode)
    {
        /* access to trigger variable? */
#if MC_USE_TSA && MC_USE_TSA_SAFETY
        if(!MC_CheckTsaSpace(pcm_nTrgVarAddr, (MC_SIZE8)pcm_nTrgVarSize, 0U))
        {
#if MC_REC_COMMON_ERR_CODES
            goto MC_SetUpRec_exit_error;
#else
            nResponseCode = MC_STC_EACCESS;
            goto MC_SetUpRec_exit;
#endif
        }
#endif /* MC_USE_TSA && MC_USE_TSA_SAFETY */
        /* get compare function */

#if MC_REC_FLOAT_TRIG
        if(pcm_bTrgVarSigned&MC_REC_FLOAT_TRIG_MASK)
        {
            pcm_pCompareFunc = MC_Comparefloat;
        }
        else
#else
        if(pcm_bTrgVarSigned&MC_REC_FLOAT_TRIG_MASK)
        {
#if MC_REC_COMMON_ERR_CODES
            goto MC_SetUpRec_exit_error;
#else
            nResponseCode = MC_STC_FLOATDISABLED;
            goto MC_SetUpRec_exit;
#endif
        }
#endif
        {
        switch(pcm_nTrgVarSize)
        {
#if MC_CFG_BUS_WIDTH == 1U
        case 1: pcm_pCompareFunc = pcm_bTrgVarSigned ? MC_Compare8S : MC_Compare8U; break;
#endif
        case 2: pcm_pCompareFunc = pcm_bTrgVarSigned ? MC_Compare16S : MC_Compare16U; break;
        case 4: pcm_pCompareFunc = pcm_bTrgVarSigned ? MC_Compare32S : MC_Compare32U; break;
        
        /* invalid trigger variable size  */
        default:
#if MC_REC_COMMON_ERR_CODES
            goto MC_SetUpRec_exit_error;
#else
            nResponseCode = MC_STC_INVSIZE;
            goto MC_SetUpRec_exit;
#endif
            }
        }
    }
#endif /* (MC_USE_FASTREC) == 0 */
    
    /* total recorder buffer length in native sizeof units (=bytes on most platforms) */
    blen = (MC_SIZE) (pcm_wRecTotalSmps * nRecVarsetSize / MC_CFG_BUS_WIDTH);

    /* recorder memory available? */
    if(blen > MC_GetRecBuffSize())
    {
#if MC_REC_COMMON_ERR_CODES
        goto MC_SetUpRec_exit_error;
#else
        nResponseCode = MC_STC_INVSIZE;
        goto MC_SetUpRec_exit;
#endif
    }

#if (MC_USE_FASTREC) == 0
    /* remember the effective end of circular buffer */
    pcm_dwRecEndBuffPtr = pcm_nRecBuffAddr + blen;
#endif /* (MC_USE_FASTREC) == 0 */

#if MC_USE_FASTREC
    if(!MC_SetUpFastRec())
    {
#if MC_REC_COMMON_ERR_CODES
        goto MC_SetUpRec_exit_error;
#else /* MC_REC_COMMON_ERR_CODES */
        nResponseCode = MC_STC_FASTRECERR;
        goto MC_SetUpRec_exit;
#endif /* MC_REC_COMMON_ERR_CODES */
    }
#endif /* MC_USE_FASTREC */

    /* everything is okay    */
    pcm_wRecFlags.flg.bIsConfigured = 1U;
    nResponseCode = MC_STS_OK;
#if MC_REC_COMMON_ERR_CODES
    goto MC_SetUpRec_exit;
MC_SetUpRec_exit_error:
    nResponseCode = MC_STC_INVSIZE;
#endif
MC_SetUpRec_exit:
    return MC_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    API: Pull the trigger of the recorder
*
* This function starts the post-trigger stop countdown
*
******************************************************************************/

void MC_TriggerRec(void)
{
    if(!pcm_wRecFlags.flg.bIsStopping)
    {
        pcm_wRecFlags.flg.bIsStopping = 1U;
#if (MC_REC_STATIC_POSTTRIG) == 0
        pcm_wStoprecCountDown = pcm_wRecPostTrigger;
#else
        pcm_wStoprecCountDown = MC_REC_STATIC_POSTTRIG;
#endif
    }
}

/**************************************************************************//*!
*
* @brief    Handling STARTREC command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the length
*           of the response filled into the buffer (including status byte)
*
* This function starts recording (initializes internal recording variables 
* and flags)
*
******************************************************************************/

MC_BPTR MC_StartRec(MC_BPTR pMessageIO)
{
     MC_U8 nResponseCode;
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
#if MC_REC_COMMON_ERR_CODES
        goto MC_StartRec_exit_error;
#else
        nResponseCode = MC_STC_NOTINIT;
        goto MC_StartRec_exit;
#endif
    }
        
    /* already running ? */
    if(pcm_wRecFlags.flg.bIsRunning)
    {
#if MC_REC_COMMON_ERR_CODES
        goto MC_StartRec_exit_error;
#else
        nResponseCode = MC_STS_RECRUN;
        goto MC_StartRec_exit;
#endif
    }

#if (MC_USE_FASTREC) == 0
    /* initialize write pointer */
    pcm_dwRecWritePtr = pcm_nRecBuffAddr;

    /* current (first) sample index */
    pcm_wRecBuffStartIx = 0U;
#endif /* (MC_USE_FASTREC) == 0 */

    /* initialize time divisor */
#if (MC_REC_STATIC_DIVISOR) != 1
    pcm_wRecTimeDivCtr = 0U;
#endif

    /* initiate virgin cycle */
    pcm_wRecFlags.flg.bIsStopping = 0U;          /* no trigger active */
    pcm_wRecFlags.flg.bTrgCrossActive = 0U;      /* waiting for threshold crossing */
    pcm_wRecFlags.flg.bInvirginCycle = 1U;       /* initial cycle */
    /* run now */

    /* start fast recorder */
#if MC_USE_FASTREC
    MC_StartFastRec();
#endif /* (MSTR_USE_FASTREC */

    /* run now */
    pcm_wRecFlags.flg.bIsRunning = 1U;           /* is running now! */

    nResponseCode = MC_STS_OK;
#if MC_REC_COMMON_ERR_CODES
    goto MC_StartRec_exit;
MC_StartRec_exit_error:
    nResponseCode = MC_STC_NOTINIT;
#endif

MC_StartRec_exit:
    return MC_ConstToBuffer8(pMessageIO, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling STOPREC command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the length
*           of the response filled into the buffer (including status byte)
*
* This function stops recording (same as manual trigger)
*
******************************************************************************/

MC_BPTR MC_StopRec(MC_BPTR pMessageIO)
{
    MC_U8 nResponseCode;
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
        nResponseCode = MC_STC_NOTINIT;
        goto MC_StopRec_exit;
    }
        
    /* already stopped ? */
    if(!pcm_wRecFlags.flg.bIsRunning)
    {
        nResponseCode = MC_STS_RECDONE;
        goto MC_StopRec_exit;
    }
    
    /* simulate trigger */
    MC_TriggerRec();
    nResponseCode = MC_STS_OK;
    
MC_StopRec_exit:
    return MC_ConstToBuffer8(pMessageIO, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling GETRECSTS command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
* This function returns current recorder status
*
******************************************************************************/

MC_BPTR MC_GetRecStatus(MC_BPTR pMessageIO)
{
    MC_U16 nResponseCode = (MC_U16) (pcm_wRecFlags.flg.bIsRunning ?
        MC_STS_RECRUN : MC_STS_RECDONE);
    
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
        nResponseCode = MC_STC_NOTINIT;
    }
        
    /* get run/stop status */
    return MC_ConstToBuffer8(pMessageIO, (MC_U8) nResponseCode);
}

/* now follows the recorder only routines, skip that if FastRecorder is used */
#if (MC_USE_FASTREC) == 0

/**************************************************************************//*!
*
* @brief    Get recorder memory size 
*
* @return   Recorder memory size in native sizeof units (=bytes on most platforms)
*
******************************************************************************/

MC_SIZE MC_GetRecBuffSize()
{
#if MC_REC_OWNBUFF
    return pcm_wRecBuffSize;
#else
    return (MC_SIZE) MC_REC_BUFF_SIZE;
#endif
}

/**************************************************************************//*!
*
* @brief    Check wether given memory region is inside the recorder buffer
*
* @param    dwAddr - address of the memory to be checked
* @param    wSize  - size of the memory to be checked
*
* @return   This function returns non-zero if user space is in recorder buffer
*
* This function is called as a part of TSA-checking process when the PC host
* is requesting memory contents
*
******************************************************************************/

MC_BOOL MC_IsInRecBuffer(MC_ADDR dwAddr, MC_SIZE8 nSize)
{
    MC_BOOL bRet = 0U;
    
    if(dwAddr >= pcm_nRecBuffAddr)
    {
        bRet = (MC_BOOL)((dwAddr + nSize) <= (pcm_nRecBuffAddr + MC_GetRecBuffSize()) ? MC_TRUE : MC_FALSE);
    }
    
    return bRet;
}


/**************************************************************************//*!
*
* @brief    Handling GETRECBUFF and GETRECBUFF_EX command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
* This function returns recorder buffer information
*
******************************************************************************/

MC_BPTR MC_GetRecBuff(MC_BPTR pMessageIO)
{
    volatile MC_BPTR pResponse;
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
        return MC_ConstToBuffer8(pMessageIO, MC_STC_NOTINIT);
    }
    
    /* must be stopped */
    if(pcm_wRecFlags.flg.bIsRunning)
    {
        return MC_ConstToBuffer8(pMessageIO, MC_STC_SERVBUSY);
    }
    
    /* fill the return info */
    pResponse = MC_ConstToBuffer8(pMessageIO, MC_STS_OK);
    pResponse = MC_AddressToBuffer(pResponse, pcm_nRecBuffAddr);
    return MC_ValueToBuffer16(pResponse, pcm_wRecBuffStartIx);
}

/**************************************************************************//*!
*
* @brief    Compare macro used in trigger detection
*
* @param    v - original command 
* @param    t - response buffer
*
* @return   zero when value is lower than threshold.
* @return   non-zero when value is greater than or equal as threshold
*
******************************************************************************/

#define CMP(v,t) ((MC_BOOL)(((v) < (t)) ? 0 : 1))

#if MC_CFG_BUS_WIDTH == 1U

static MC_BOOL MC_Compare8S()
{
    return CMP(MC_GetS8(pcm_nTrgVarAddr), pcm_uTrgThreshold.s8);
}

static MC_BOOL MC_Compare8U()
{
    return CMP(MC_GetU8(pcm_nTrgVarAddr), pcm_uTrgThreshold.u8);
}

#endif

static MC_BOOL MC_Compare16S()
{
    return CMP(MC_GetS16(pcm_nTrgVarAddr), pcm_uTrgThreshold.s16);
}

static MC_BOOL MC_Compare16U()
{
    return CMP(MC_GetU16(pcm_nTrgVarAddr), pcm_uTrgThreshold.u16);
}

static MC_BOOL MC_Compare32S()
{
    return CMP(MC_GetS32(pcm_nTrgVarAddr), pcm_uTrgThreshold.s32);
}

static MC_BOOL MC_Compare32U()
{
    return CMP(MC_GetU32(pcm_nTrgVarAddr), pcm_uTrgThreshold.u32);
}

#if MC_REC_FLOAT_TRIG
static MC_BOOL MC_Comparefloat()
{
    return CMP(MC_GetFloat(pcm_nTrgVarAddr), pcm_uTrgThreshold.fp);
}
#endif

/**************************************************************************//*!
*
* @brief    API: Recorder worker routine - can be called from application's timer ISR
*
*
* This returns quickly if recorder is not running, otherwise it calls quite lengthy 
* recorder routine which does all the recorder work (sampling, triggering)
*
******************************************************************************/

#if defined(MC_PLATFORM_56F8xxx) || defined(MC_PLATFORM_56F8xx)
#pragma interrupt called
#endif

void MC_Recorder(void)
{
    /* recorder not active */
    if(!pcm_wRecFlags.flg.bIsRunning)
    {
        return ;
    }
    
    /* do the hard work      */
    MC_Recorder2();
}

/**************************************************************************//*!
*
* @brief    Recorder function called when recorder is active
*
******************************************************************************/

#if defined(MC_PLATFORM_56F8xxx) || defined(MC_PLATFORM_56F8xx)
#pragma interrupt called
#endif

static void MC_Recorder2(void)
{
    MC_SIZE8 sz;
    MC_BOOL cmp;
    MC_U8 i;

#if (MC_REC_STATIC_DIVISOR) != 1
    /* skip this call ? */
    if(pcm_wRecTimeDivCtr)
    {
        /* maybe next time... */
        pcm_wRecTimeDivCtr--;
        return;
    }
    
    /* re-initialize divider */
#if (MC_REC_STATIC_DIVISOR) == 0
    pcm_wRecTimeDivCtr = pcm_wRecTimeDiv;
#else 
    pcm_wRecTimeDivCtr = MC_REC_STATIC_DIVISOR;
#endif /* (MC_REC_STATIC_DIVISOR) == 0 */
#endif /* (MC_REC_STATIC_DIVISOR) != 1 */

    /* take snapshot of variable values */
    for (i=0U; i<pcm_nRecVarCount; i++)
    {
        sz = pcm_pRecVarSize[i];
        MC_CopyMemory(pcm_dwRecWritePtr, pcm_pRecVarAddr[i], sz);
        sz /= MC_CFG_BUS_WIDTH;
        pcm_dwRecWritePtr += sz;
    }
    
    /* another sample taken (startIx "points" after sample just taken) */
    /* i.e. it points to the oldest sample */
    pcm_wRecBuffStartIx++;
    
    /* wrap around (circular buffer) ? */
    if(pcm_dwRecWritePtr >= pcm_dwRecEndBuffPtr)
    {   
        pcm_dwRecWritePtr = pcm_nRecBuffAddr;
        pcm_wRecFlags.flg.bInvirginCycle = 0U;
        pcm_wRecBuffStartIx = 0U;
    }

    /* no trigger testing in virgin cycle */
    if(pcm_wRecFlags.flg.bInvirginCycle)
    {
        return;
    }
    
    /* test trigger condition if still running */
    if(!pcm_wRecFlags.flg.bIsStopping && pcm_pCompareFunc != NULL)
    {
        /* compare trigger threshold */
        cmp = pcm_pCompareFunc();
        
        /* negated logic (falling-edge) ? */
        if(pcm_nRecTriggerMode == 2U)
        {
            cmp = (MC_BOOL) !cmp;
        }
        
        /* above threshold ? */
        if(cmp)
        {
            /* were we at least once below threshold ? */
            if(pcm_wRecFlags.flg.bTrgCrossActive)
            {
                /* EDGE TRIGGER ! */
                MC_TriggerRec();
            }
        }
        else
        {
            /* we got bellow threshold, now wait for being above threshold */
            pcm_wRecFlags.flg.bTrgCrossActive = 1U;
        }
    }
    
    /* in stopping mode ? (note that this bit might have been set just above!) */
    if(pcm_wRecFlags.flg.bIsStopping)
    {
        /* count down post-trigger samples expired ? */
        if(!pcm_wStoprecCountDown)
        {
            /* STOP RECORDER */
            pcm_wRecFlags.flg.bIsRunning = 0U;
            return;
        }
        
        /* perhaps next time */
        pcm_wStoprecCountDown--;
    }
}

#endif /* (MC_USE_FASTREC) == 0 */

#else /* MC_USE_RECORDER && (!MC_DISABLE) */

/* use void recorder API functions */
void MC_Recorder(void)
{ 
}

void MC_TriggerRec(void)
{ 
}

void MC_SetUpRecBuff(MC_ADDR pBuffer, MC_SIZE wBuffSize)
{ 
    MC_UNUSED(pBuffer);
    MC_UNUSED(wBuffSize);
}

/*lint -efile(766, mc_protocol.h) include file is not used in this case */

#endif /* MC_USE_RECORDER && (!MC_DISABLE) */
