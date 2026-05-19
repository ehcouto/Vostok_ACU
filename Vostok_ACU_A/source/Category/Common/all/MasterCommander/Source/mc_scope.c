/**
 *  @file
 *
 *  @brief      MC Oscilloscope implementation.
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

#if (MC_USE_SCOPE) && (!(MC_DISABLE))

/***********************************
*  local variables 
***********************************/

static MC_U8  pcm_nScopeVarCount;        /* number of active scope variables */
static MC_ADDR  pcm_pScopeVarAddr[MC_MAX_SCOPE_VARS]; /* addresses of scope variables */
static MC_SIZE8 pcm_pScopeVarSize[MC_MAX_SCOPE_VARS]; /* sizes of scope variables */

/**************************************************************************//*!
*
* @brief    Scope Initialization
*
******************************************************************************/

void MC_InitScope(void)
{   
}

/**************************************************************************//*!
*
* @brief    Handling SETUPSCOPE and SETUPSCOPE_EX command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_SetUpScope(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_U8 i, sz, nVarCnt;

    /* uninitialize scope */
    pcm_nScopeVarCount = 0U;

    /* seek the setup data */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
    
    /* scope variable count  */
    pMessageIO = MC_ValueFromBuffer8(&nVarCnt, pMessageIO);

    /* scope variable information must fit into our buffers */
    if(!nVarCnt || nVarCnt > (MC_U8)MC_MAX_SCOPE_VARS)
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_INVBUFF);
    }
    
    /* get all addresses and sizes */
    for(i=0U; i<nVarCnt; i++)
    {
        /* variable size */
        pMessageIO = MC_ValueFromBuffer8(&sz, pMessageIO);
        pcm_pScopeVarSize[i] = sz;
        
        /* variable address */
        pMessageIO = MC_AddressFromBuffer(&pcm_pScopeVarAddr[i], pMessageIO);

        /* valid numeric variable sizes only */
        if(sz == 0U || sz > 8U)
        {
            return MC_ConstToBuffer8(pResponse, MC_STC_INVSIZE);
        }
        
#if MC_CFG_BUS_WIDTH > 1U
        /* even sizes only */
        if(sz & 0x1)
        {
            return MC_ConstToBuffer8(pResponse, MC_STC_INVSIZE);
        }
#endif
        
#if MC_USE_TSA && MC_USE_TSA_SAFETY
        if(!MC_CheckTsaSpace(pcm_pScopeVarAddr[i], (MC_SIZE8) sz, 0U))
        {
            return MC_ConstToBuffer8(pResponse, MC_STC_EACCESS);
        }
#endif
        
    }

    /* activate scope */
    pcm_nScopeVarCount = nVarCnt;

    /* return just a status */
    return MC_ConstToBuffer8(pResponse, MC_STS_OK);
}

/**************************************************************************//*!
*
* @brief    Handling READSCOPE command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_ReadScope(MC_BPTR pMessageIO)
{
    MC_U8 i;
    
    if(!pcm_nScopeVarCount)
    {
        return MC_ConstToBuffer8(pMessageIO, MC_STC_NOTINIT);
    }
    
    /* success */
    pMessageIO = MC_ConstToBuffer8(pMessageIO, MC_STS_OK);
    
    for (i=0U; i<pcm_nScopeVarCount; i++)
    {
        pMessageIO = MC_CopyToBuffer(pMessageIO, pcm_pScopeVarAddr[i], pcm_pScopeVarSize[i]);
    } 
        
    /* return end position */
    return pMessageIO;  
}

#else  /* (MC_USE_SCOPE) && !(MC_DISABLE) */

/*lint -efile(766, mc_protocol.h) include file is not used in this case */

#endif /* (MC_USE_SCOPE) && !(MC_DISABLE) */

