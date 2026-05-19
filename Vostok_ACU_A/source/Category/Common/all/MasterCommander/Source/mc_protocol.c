/**
 *  @file
 *
 *  @brief      MasterCommander protocol handler
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "mc_protocol.h"

#include "master_commander.h"
#include "mc_llmem.h"
#include "mc_private.h"

#if !(MC_DISABLE)

#ifndef READFIX_CALLBACK
    #define READFIX_CALLBACK()
#endif
#ifndef WRITEFIX_CALLBACK
    #define WRITEFIX_CALLBACK()
#endif
#ifndef SINGLEREQ_CALLBACK
    #define SINGLEREQ_CALLBACK()
#endif
#ifndef COPY_FROM_VAR
    #define COPY_FROM_VAR()
#endif
#ifndef COPY_TO_VAR
    #define COPY_TO_VAR()
#endif


#if MC_USE_FIXED_CMDS == 1

input Receive;
output Transmit;

#endif /* MC_USE_FIXED_CMDS */

/**************************************************************************//*!
*
* @brief    mc driver initialization
*
******************************************************************************/

MC_BOOL MC_Init(void)
{   
#if MC_USE_TSA
    /* initialize TSA */
    MC_InitTsa();
#endif

#if MC_USE_SCOPE
    /* initialize Scope */
    MC_InitScope();
#endif

#if MC_USE_RECORDER
    /* initialize Recorder */
    MC_InitRec();
#endif

#if MC_USE_APPCMD
    /* initialize application commands */
    MC_InitAppCmds();
#endif

#if MC_USE_SFIO
    /* initialize SFIO encapsulation layer */
    MC_InitSfio();
#endif

#if MC_USE_PIPES
    /* initialize PIPES interface */
    MC_InitPipes();
#endif

#if MC_USE_PDBDM
    /* initialize Packet Driven BDM communication */
    MC_InitPDBdm();
#endif  
    
#if MC_USE_SERIAL
    /* initialize communication and start listening for commands */
    if (!MC_InitSerial())
      return MC_FALSE;
#endif  
    return MC_TRUE;
}

/**************************************************************************//*!
*
* @brief    Decodes the mc protocol and calls appropriate handlers
*
* @param    pMessageIO - message in/out buffer
*
* @return   TRUE if frame was valid and any output was generated to IO buffer
*
* This Function decodes given message and invokes proper command handler
* which fills in the response. The response transmission is initiated
* in this call as well.
*
******************************************************************************/

MC_BOOL MC_ProtocolDecoder(MC_BPTR pMessageIO)
{
    MC_BPTR pResponseEnd;
    MC_U8 nCmd;
    
    /* no EX access by default */
    MC_SetExAddr(MC_FALSE);
    
    /* command code comes first in the message */
    /*lint -e{534} return value is not used */
    (void)MC_ValueFromBuffer8(&nCmd, pMessageIO);

    /* process command   */
    switch (nCmd)
    {
    
#if MC_USE_READVAR

        /* read byte */
#if MC_USE_EX_CMDS
        case MC_CMD_READVAR8_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_READVAR8:
#endif          
            pResponseEnd = MC_ReadVar(pMessageIO, 1U);
            break;
            
        /* read word */
#if MC_USE_EX_CMDS
        case MC_CMD_READVAR16_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_READVAR16:
#endif
            pResponseEnd = MC_ReadVar(pMessageIO, 2U);
            break;
            
        /* read dword */
#if MC_USE_EX_CMDS
        case MC_CMD_READVAR32_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_READVAR32:
#endif
            pResponseEnd = MC_ReadVar(pMessageIO, 4U);
            break;
#endif /* MC_USE_READVAR */

#if MC_USE_READMEM

        /* read a block of memory */
#if MC_USE_EX_CMDS
        case MC_CMD_READMEM_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_READMEM:
#endif
            pResponseEnd = MC_ReadMem(pMessageIO);
            break;
            
#endif /* MC_USE_READMEM */

#if MC_USE_SCOPE

        /* prepare scope variables */
#if MC_USE_EX_CMDS
        case MC_CMD_SETUPSCOPE_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_SETUPSCOPE:
#endif
            pResponseEnd = MC_SetUpScope(pMessageIO);
            break;  
            
        case MC_CMD_READSCOPE:
            pResponseEnd = MC_ReadScope(pMessageIO);
            break;
#endif /* MC_USE_SCOPE */

#if MC_USE_RECORDER

        /* get recorder status */
        case MC_CMD_GETRECSTS:
            pResponseEnd = MC_GetRecStatus(pMessageIO);
            break;

        /* start recorder */
        case MC_CMD_STARTREC:
            pResponseEnd = MC_StartRec(pMessageIO);
            break;

        /* stop recorder */
        case MC_CMD_STOPREC:
            pResponseEnd = MC_StopRec(pMessageIO);
            break;

        /* setup recorder */
#if MC_USE_EX_CMDS
        case MC_CMD_SETUPREC_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_SETUPREC:
#endif
            pResponseEnd = MC_SetUpRec(pMessageIO);
            break;
            
        /* get recorder buffer information (force EX instead of non-EX) */
#if MC_USE_EX_CMDS
        case MC_CMD_GETRECBUFF_EX:
            MC_SetExAddr(MC_TRUE);
#elif MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_GETRECBUFF:
#endif
            pResponseEnd = MC_GetRecBuff(pMessageIO);
            break; 
#endif /* MC_USE_RECORDER */
 
#if MC_USE_APPCMD

        /* accept the application command */
        case MC_CMD_SENDAPPCMD:
            pResponseEnd = MC_StoreAppCmd(pMessageIO);
            break;

        /* get the application command status */
        case MC_CMD_GETAPPCMDSTS:
            pResponseEnd = MC_GetAppCmdStatus(pMessageIO);
            break;

        /* get the application command data */
        case MC_CMD_GETAPPCMDDATA:
            pResponseEnd = MC_GetAppCmdRespData(pMessageIO);
            break;
#endif /* MC_USE_APPCMD */

#if MC_USE_WRITEMEM

        /* write a block of memory */
#if MC_USE_EX_CMDS
        case MC_CMD_WRITEMEM_EX:
            MC_SetExAddr(MC_TRUE);
#endif          
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_WRITEMEM:
#endif
            pResponseEnd = MC_WriteMem(pMessageIO);
            break;
#endif /* MC_USE_WRITEMEM */

#if MC_USE_WRITEMEMMASK

        /* write block of memory with a bit mask */
#if MC_USE_EX_CMDS
        case MC_CMD_WRITEMEMMASK_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_WRITEMEMMASK:
#endif
            pResponseEnd = MC_WriteMemMask(pMessageIO);
            break;
#endif /* MC_USE_WRITEMEMMASK */
            
#if MC_USE_WRITEVAR && MC_USE_NOEX_CMDS

        /* write byte */
        case MC_CMD_WRITEVAR8:
            pResponseEnd = MC_WriteVar(pMessageIO, 1U);
            break;

        /* write word */
        case MC_CMD_WRITEVAR16:
            pResponseEnd = MC_WriteVar(pMessageIO, 2U);
            break;

        /* write dword */
        case MC_CMD_WRITEVAR32:
            pResponseEnd = MC_WriteVar(pMessageIO, 4U);
            break;
#endif /* MC_USE_WRITEVAR && MC_USE_NOEX_CMDS */

#if MC_USE_WRITEVARMASK && MC_USE_NOEX_CMDS

        /* write byte with mask */
        case MC_CMD_WRITEVAR8MASK:
            pResponseEnd = MC_WriteVarMask(pMessageIO, 1U);
            break;

        /* write word with mask */
        case MC_CMD_WRITEVAR16MASK:
            pResponseEnd = MC_WriteVarMask(pMessageIO, 2U);
            break;

#endif /* MC_USE_WRITEVARMASK && MC_USE_NOEX_CMDS */

#if MC_USE_TSA

        /* get TSA table (force EX instead of non-EX) */
#if MC_USE_EX_CMDS
        case MC_CMD_GETTSAINFO_EX:
            MC_SetExAddr(MC_TRUE);
#elif MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_GETTSAINFO:
#endif
            pResponseEnd = MC_GetTsaInfo(pMessageIO);
            break;
            
#if MC_USE_EX_CMDS
        case MC_CMD_GETSTRLEN_EX:
            MC_SetExAddr(MC_TRUE);
#endif
#if MC_USE_NOEX_CMDS
        /*lint -fallthrough */
        case MC_CMD_GETSTRLEN:
#endif
            pResponseEnd = MC_GetStringLen(pMessageIO);
            break;

#endif /* MC_USE_TSA */

#if MC_USE_BRIEFINFO
        /* retrieve a subset of board information structure */
        case MC_CMD_GETINFOBRIEF:
#else   
        /* retrieve board information structure */
        case MC_CMD_GETINFO:
#endif
            pResponseEnd = MC_GetBoardInfo(pMessageIO);
            break;

#if MC_USE_SFIO
        case MC_CMD_SFIOFRAME_0:
        case MC_CMD_SFIOFRAME_1:
            pResponseEnd = MC_SfioFrame(pMessageIO);
            break;
        case MC_CMD_SFIOGETRESP_0:
        case MC_CMD_SFIOGETRESP_1:
            pResponseEnd = MC_SfioGetResp(pMessageIO);
            break;
#endif /* MC_USE_SFIO */

#if MC_USE_PIPES
        case MC_CMD_PIPE:
            pResponseEnd = MC_PipeFrame(pMessageIO);
            break;
#endif /* MC_USE_PIPES */

        /* Master&Commander Commands */

        case MC_CMD_READLF:
			pResponseEnd = MC_ReadLF(pMessageIO);
			break;

		case MC_CMD_WRITELF:
			pResponseEnd = MC_WriteLF(pMessageIO);
			break;

		case MC_CMD_WRITELF_BYTE:
			pResponseEnd = MC_WriteLF_Byte(pMessageIO);
			break;

#if MC_USE_FIXED_CMDS
#if 0 // not supported
		case MC_CMD_READFIX:
		    pResponseEnd = MC_ReadFix(pMessageIO);
		    READFIX_CALLBACK();
            break;

        case MC_CMD_WRITEFIX:
            pResponseEnd = MC_WriteFix(pMessageIO);
            WRITEFIX_CALLBACK();
            break;
#endif
        case MC_CMD_SINGLEREQ:
            pResponseEnd = MC_SingleReq(pMessageIO);
            break;
#endif /* MC_USE_FIXED_CMDS */

        /* unknown command */
        default:
            pResponseEnd = MC_ConstToBuffer8(pMessageIO, MC_STC_INVCMD);
            break;          
    }
    
    /* anything to send back? */
    if(pResponseEnd != pMessageIO)
    {
        /*lint -e{946,960} subtracting pointers is appropriate here */
        MC_SIZE8 nSize = (MC_SIZE8)(pResponseEnd - pMessageIO);
        MC_SendResponse(pMessageIO, nSize);
        return MC_TRUE;
    }
    else
    {
        /* nothing sent out */
        return MC_FALSE;
    }
}

/**************************************************************************//*!
*
* @brief    Handling GETINFO or GETINFO_BRIEF
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_GetBoardInfo(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_U16 wTmp;
#if (!MC_LIGHT_VERSION)
    MC_U8 *pStr;
#endif

    pResponse = MC_ConstToBuffer8(pResponse, MC_STS_OK);
    pResponse = MC_ConstToBuffer8(pResponse, (MC_U8)(MC_PROT_VER));            /* protVer */
    pResponse = MC_ConstToBuffer8(pResponse, (MC_U8)(MC_CFG_FLAGS));           /* cfgFlags */
    pResponse = MC_ConstToBuffer8(pResponse, (MC_U8)(MC_CFG_BUS_WIDTH));       /* dataBusWdt */
    pResponse = MC_ConstToBuffer8(pResponse, (MC_U8)(MC_GLOB_VERSION_MAJOR));  /* globVerMajor */
    pResponse = MC_ConstToBuffer8(pResponse, (MC_U8)(MC_GLOB_VERSION_MINOR));  /* globVerMinor */
    pResponse = MC_ConstToBuffer8(pResponse, (MC_U8)(MC_COMM_BUFFER_SIZE));    /* cmdBuffSize  */

    /* that is all for brief info */
#if MC_USE_BRIEFINFO
    MC_UNUSED(wTmp);
#else /* MC_USE_BRIEFINFO */

#if MC_USE_RECORDER

    /* recorder buffer size is always measured in bytes */
    wTmp = MC_GetRecBuffSize();
    wTmp *= MC_CFG_BUS_WIDTH;

    /* send size and timebase    */
    pResponse = MC_ValueToBuffer16(pResponse, wTmp);
    pResponse = MC_ConstToBuffer16(pResponse, (MC_U16) MC_REC_TIMEBASE);
#else /* MC_USE_RECORDER */

    MC_UNUSED(wTmp);

    /* recorder info zeroed */
    pResponse = MC_ConstToBuffer16(pResponse, 0);
    pResponse = MC_ConstToBuffer16(pResponse, 0);
#endif /* MC_USE_RECORDER */

#if MC_LIGHT_VERSION
    pResponse = MC_SkipInBuffer(pResponse, (MC_U8)MC_DESCR_SIZE);
#else
    /* description string */
    pStr = (MC_U8*)  MC_IDT_STRING;
    for(wTmp = 0U; wTmp < (MC_U8)(MC_DESCR_SIZE); wTmp++)
    {
        pResponse = MC_ValueToBuffer8(pResponse, *pStr);

        /* terminating zero used to clear the remainder of the buffer */        
        if(*pStr)
        {
            pStr ++;
        }
    }
#endif /* SEND_IDT_STRING */
    
#endif /* MC_USE_BRIEFINFO */

    return pResponse;   
}

/**************************************************************************//*!
*
* @brief    Handling READMEM and READMEM_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_ReadMem(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nSize;

    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = MC_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);

#if MC_USE_TSA && MC_USE_TSA_SAFETY
    if(!MC_CheckTsaSpace(nAddr, (MC_SIZE8) nSize, MC_FALSE))
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_EACCESS);
    }
#endif

    /* check the response will safely fit into comm buffer */
    if(nSize > (MC_U8)MC_COMM_BUFFER_SIZE)
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_RSPBUFFOVF);
    }
    
    /* success  */
    pResponse = MC_ConstToBuffer8(pResponse, MC_STS_OK);
    
    return MC_CopyToBuffer(pResponse, nAddr, (MC_SIZE8) nSize);
}   

/**************************************************************************//*!
*
* @brief    Handling READVAR and READVAR_EX commands (for all sizes 1,2,4)
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_ReadVar(MC_BPTR pMessageIO, MC_SIZE8 nSize)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;

    pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);
    pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);

#if MC_USE_TSA && MC_USE_TSA_SAFETY
    if(!MC_CheckTsaSpace(nAddr, nSize, MC_FALSE))
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_EACCESS);
    }
#endif

    /* success  */
    pResponse = MC_ConstToBuffer8(pResponse, MC_STS_OK);
    
    return MC_CopyToBuffer(pResponse, nAddr, nSize);
}   

/**************************************************************************//*!
*
* @brief    Handling WRITEMEM and WRITEMEM_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_WriteMem(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nSize,nResponseCode;

    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = MC_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);

#if MC_USE_TSA && MC_USE_TSA_SAFETY
    if(!MC_CheckTsaSpace(nAddr, (MC_SIZE8) nSize, MC_TRUE))
    {
        nResponseCode = MC_STC_EACCESS;
        goto MC_WriteMem_exit;
    }
#endif

    /*lint -e{534} ignoring function return value */
    MC_CopyFromBuffer(nAddr, pMessageIO, (MC_SIZE8) nSize);
    nResponseCode = MC_STS_OK;

#if MC_USE_TSA && MC_USE_TSA_SAFETY
MC_WriteMem_exit:
#endif      
      
    return MC_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling WRITEVAR command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
* @param    nSize - variable size
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_WriteVar(MC_BPTR pMessageIO, MC_SIZE8 nSize)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nResponseCode;

    pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);
    pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);

#if MC_USE_TSA && MC_USE_TSA_SAFETY
    if(!MC_CheckTsaSpace(nAddr, nSize, MC_TRUE))
    {
        nResponseCode = MC_STC_EACCESS;
        goto MC_WriteVar_exit;
    }
#endif

    /*lint -e{534} ignoring function return value */
    MC_CopyFromBuffer(nAddr, pMessageIO, nSize);
    nResponseCode = MC_STS_OK;

#if MC_USE_TSA && MC_USE_TSA_SAFETY
MC_WriteVar_exit:
#endif
      
    return MC_ConstToBuffer8(pResponse, nResponseCode);
}


/**************************************************************************//*!
*
* @brief    Handling WRITEMEMMASK and WRITEMEMMASK_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_WriteMemMask(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nSize,nResponseCode;

    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = MC_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);

#if MC_USE_TSA && MC_USE_TSA_SAFETY
    if(!MC_CheckTsaSpace(nAddr, (MC_SIZE8)nSize, MC_TRUE))
    {
        nResponseCode = MC_STC_EACCESS;
        goto MC_WriteMemMask_exit;
    }
#endif

#if MC_CFG_BUS_WIDTH > 1U
    /* size must be divisible by bus width (mask must not begin in half of memory word) */
    if(nSize % MC_CFG_BUS_WIDTH)
    {
        nResponseCode = MC_STC_INVSIZE;
        goto MC_WriteMemMask_exit;
    }
#endif

    /* put the data */
    MC_CopyFromBufferWithMask(nAddr, pMessageIO, (MC_SIZE8)nSize);
    nResponseCode = MC_STS_OK;

#if (MC_USE_TSA && MC_USE_TSA_SAFETY) || (MC_CFG_BUS_WIDTH > 1U)
MC_WriteMemMask_exit:
#endif  
      
    return MC_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling WRITEVARMASK command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
* @param    nSize - variable size
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

MC_BPTR MC_WriteVarMask(MC_BPTR pMessageIO, MC_SIZE8 nSize)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nResponseCode;
    
    pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);
    pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);

#if MC_USE_TSA && MC_USE_TSA_SAFETY
    if(!MC_CheckTsaSpace(nAddr, nSize, MC_TRUE))
    {
        nResponseCode = MC_STC_EACCESS;
        goto MC_WriteVarMask_exit;
    }
#endif

    /* put the data */
    MC_CopyFromBufferWithMask(nAddr, pMessageIO, nSize);
    nResponseCode = MC_STS_OK;

#if MC_USE_TSA && MC_USE_TSA_SAFETY
MC_WriteVarMask_exit:
#endif      

    return MC_ConstToBuffer8(pResponse, nResponseCode);
}


/**************************************************************************//*!
*
* @brief    Private inline implementation of "strlen" used by TSA and Pipes
*
******************************************************************************/

MC_U16 MC_StrLen(MC_ADDR nAddr)
{
    const MC_U8* pStr;
    MC_U16 nLen = 0U;

    #ifdef __HCS12X__
    /* convert from logical to global if needed */
    nAddr = MC_FixHcs12xAddr(nAddr);
    #endif

    /*lint -e{923} casting address value to pointer */
    pStr = (const MC_U8*) nAddr;
    
    while(*pStr++)
    {
        nLen++;
    }
        
    return nLen;
}

/**
*
* @brief    Handling READLF command
*
* @param    pMessageIO - original command (in) and response buffer (out)
*
* @return   As all command handlers, the return value should be the buffer
*           pointer where the response output finished (except checksum)
*
*/
MC_BPTR MC_ReadLF(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nSize;
    MC_U8 i;

    /* Skips only the command byte */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);

    /* Retrieve the number of words to read. This is equal to the LEN */
    pMessageIO = MC_ValueFromBuffer8(&nSize, pMessageIO);

    /* check the response will safely fit into comm buffer */
	if(nSize > (MC_U8)MC_COMM_BUFFER_SIZE)
	{
		return MC_ConstToBuffer8(pResponse, MC_STC_RSPBUFFOVF);
	}

	/* success  */
	pResponse = MC_ConstToBuffer8(pResponse, MC_STS_OK);
	/* fixed this to support 32 bit addressing */
#if MC_PLATFORM_KXX
	nSize >>= (sizeof(MC_ADDR) >> 1);
#else
	nSize >>= 1;
#endif /* MC_PLATFORM_KXX */

    for (i = 0; i < nSize; i++)
    {
    	pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);
    	pResponse = MC_CopyToBuffer(pResponse, nAddr, (MC_SIZE8) 2);
    }

    return pResponse;

}



/**
*
* @brief    Handling WRITELF command
*
* @param    pMessageIO - original command (in) and response buffer (out)
*
* @return   As all command handlers, the return value should be the buffer
*           pointer where the response output finished (except checksum)
*
*/
MC_BPTR MC_WriteLF(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nSize, nResponseCode, i = 0;

    /* Skips only the command byte */
	pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);

	/* Retrieve the number of words to write. This is equal to the LEN / 4 */
	pMessageIO = MC_ValueFromBuffer8(&nSize, pMessageIO);

	while (i < nSize)
	{
		/* Retrieve the address */
		pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);
		pMessageIO = MC_CopyFromBuffer(nAddr, pMessageIO, (MC_SIZE8) 2);
#if MC_PLATFORM_KXX
		i += (2 + sizeof(MC_ADDR));
#else
		i += 4;
#endif /* MC_PLATFORM_KXX */
	}


    nResponseCode = MC_STS_OK;

    return MC_ConstToBuffer8(pResponse, nResponseCode);
}


/**
*
* @brief    Handling WRITELF_BYTE command
*
* @param    pMessageIO - original command (in) and response buffer (out)
*
* @return   As all command handlers, the return value should be the buffer
*           pointer where the response output finished (except checksum)
*
*/
MC_BPTR MC_WriteLF_Byte(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr;
    MC_U8 nSize, nResponseCode, i = 0;

    /* Skips only the command byte */
	pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);

	/* Retrieve the number of bytes to write. For each variable we have 3 bytes. */
	/* 2 for ADDR, 1 for VAL */
	pMessageIO = MC_ValueFromBuffer8(&nSize, pMessageIO);

    while (i < nSize)
    {
    	/* Retrieve the address */
    	pMessageIO = MC_AddressFromBuffer(&nAddr, pMessageIO);
    	pMessageIO = MC_CopyFromBuffer(nAddr, pMessageIO, (MC_SIZE8) 1);
#if MC_PLATFORM_KXX
    	i += (1 + sizeof(MC_ADDR));
#else
    	i += 3;
#endif /* MC_PLATFORM_KXX */
    }


    nResponseCode = MC_STS_OK;

    return MC_ConstToBuffer8(pResponse, nResponseCode);
}

#if MC_USE_FIXED_CMDS
#if 0  // not supported
/**
*
* @brief    Handling READFIX command
*
* @param    pMessageIO - original command (in) and response buffer (out)
*
* @return   As all command handlers, the return value should be the buffer
*           pointer where the response output finished (except checksum)
*
*/
MC_BPTR MC_ReadFix(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR pAddr = (MC_ADDR) Transmit;

    /* Skips only the command byte */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 1U);

    /* check the response will safely fit into comm buffer */
    if(INPUT_LEN > (MC_U8)MC_COMM_BUFFER_SIZE)
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_RSPBUFFOVF);
    }

    /* success  */
    pResponse = MC_ConstToBuffer8(pResponse, MC_STS_OK);

    // Copy from buffer payload to send buffer
    pResponse = MC_CopyToBuffer(pResponse, pAddr, OUTPUT_LEN);

    return pResponse;
}

/**
*
* @brief    Handling WRITEFIX command
*
* @param    pMessageIO - original command (in) and response buffer (out)
*
* @return   As all command handlers, the return value should be the buffer
*           pointer where the response output finished (except checksum)
*
*/
MC_BPTR MC_WriteFix(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr = (MC_ADDR) Receive;
    MC_U8 nResponseCode;

    /* Skips command byte and Len byte */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);

    /* Retrieve the values */
    pMessageIO = MC_CopyFromBuffer(nAddr, pMessageIO, INPUT_LEN);

    nResponseCode = MC_STS_OK;

    return MC_ConstToBuffer8(pResponse, nResponseCode);
}
#endif

/**
*
* @brief    Handling SINGLEREQ command.
*
* @param    pMessageIO - original command (in) and response buffer (out)
*
* @return   As all command handlers, the return value should be the buffer
*           pointer where the response output finished (except checksum)
*
*/
MC_BPTR MC_SingleReq(MC_BPTR pMessageIO)
{
    MC_BPTR pResponse = pMessageIO;
    MC_ADDR nAddr = (MC_ADDR) Receive;
    MC_ADDR pAddr = (MC_ADDR) Transmit;
    unsigned char index = 0;

    /* Skips command byte and Len byte */
    pMessageIO = MC_SkipInBuffer(pMessageIO, 2U);

    /* Retrieve the index byte */
    pMessageIO = MC_ValueFromBuffer8(&index, pMessageIO);

    /* Retrieve the values */
    pMessageIO = MC_CopyFromBuffer(nAddr, pMessageIO, SIM_INPUT_LEN_ARRAY[index]);

    // Copy to the mapped variables
    COPY_TO_VAR(index, Receive);

    // Call back
    SINGLEREQ_CALLBACK(index);

    // Read operation
    // check the response will safely fit into comm buffer
    if(SIM_INPUT_LEN_ARRAY[index] > (MC_U8)MC_COMM_BUFFER_SIZE)
    {
        return MC_ConstToBuffer8(pResponse, MC_STC_RSPBUFFOVF);
    }

    // Success
    pResponse = MC_ConstToBuffer8(pResponse, MC_STS_OK);
    // Copy from variables to buffer payload
    COPY_FROM_VAR(index, Transmit);

    // Copy from buffer payload to send buffer
    pResponse = MC_CopyToBuffer(pResponse, pAddr, SIM_OUTPUT_LEN_ARRAY[index]);

    return pResponse;
}

#endif /* MC_USE_FIXED_CMDS */
#else /* !MC_DISABLE */

/**************************************************************************//*!
*
* @brief    mc driver initialization is disabled
*
******************************************************************************/

MC_BOOL MC_Init(void)
{   
    return MC_FALSE;
}

/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
*******************************************************************************/

void MC_Poll(void)
{ 
}

/* HC12 interrupt routine declaration, must be in non-paged code memory */
#if defined(MC_PLATFORM_HC12) && (!defined(__S12Z__))
    #include "non_bank.sgm"
#endif

/*******************************************************************************
*
* @brief    API: API: Main SCI / CAN Interrupt handler call
*
*******************************************************************************/
#if !defined(MC_PLATFORM_MQX)
#if defined(MC_PLATFORM_MPC55xx)
void MC_Isr(unsigned long vec)
{
    MC_UNUSED(vec);
}
#else
void MC_Isr(void)
{
}
#endif
#endif

#if defined(MC_PLATFORM_HC12) || defined(MC_PLATFORM_HC08) || defined(MC_PLATFORM_MCF51xx)
/*******************************************************************************
*
* @brief    API: API: The 2nd MC interrupt handler
*
*******************************************************************************/

void MC_Isr2(void)
{
}

#endif

/* restore HC12 code segment */
#if defined(MC_PLATFORM_HC12) && (!defined(__S12Z__))
    #include "default.sgm"
#endif

#endif /* !MC_DISABLE */
