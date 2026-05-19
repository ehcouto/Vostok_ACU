/**
 *  @file       
 *
 *  @brief      MasterCommander low level memory access routines
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "mc_llmem.h"

#include "C_Extensions.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------

#if !(MC_DISABLE)
/*******************************************************************************
*
*  @brief    API: User callback called from MC_Isr() handler
*
*******************************************************************************/

#if defined(MC_ISR_CALLBACK)
  extern void MC_ISR_CALLBACK(void);
#endif

/*******************************************************************************
*
* @brief    API: Main SCI / JTAG / CAN Interrupt handler call
*
* This Interrupt Service Routine handles the SCI or CAN interrupts for the mc
* driver. In case you want to handle the interrupt in the application yourselves,
* call the MC_ProcessSCI, MC_ProcessCanRx or MC_ProcessCanTx functions which
* does the same job but is not compiled as an Interrupt Service Routine.
*
* In poll-driven mode (MC_POLL_DRIVEN) this function does nothing.
*
*******************************************************************************/

/* 56F8xxx interrupt routine declaration, user has to  */
/* direct vector to the MC_Isr function */
////#pragma interrupt saveall

void MC_Isr(void)
{
#if (MC_LONG_INTR) || (MC_SHORT_INTR)

    /* process serial interface */
#if MC_USE_SCI
    MC_ProcessSCI();

    /* process Jtag EOnCE interface */
#elif MC_USE_JTAG
    MC_ProcessJTAG();

#endif /* MC_USE_SCI */

    /* process application callback */
#if defined(MC_ISR_CALLBACK)
    if((MC_ISR_CALLBACK) != NULL)
        MC_ISR_CALLBACK();
#endif

#endif
}

/* automatic inlining may cause problems with C calling convention assumed below */
//#pragma dont_inline on

/**************************************************************************//*!
*
* @brief    The "memcpy" used internally in MC driver
*
* @param    nDestAddr - destination memory address
* @param    nSrcAddr  - source memory address
* @param    nSize     - memory size (always in bytes)
*
* @return This function returns a pointer to next destination byte
*
******************************************************************************
*
* This function accepts both 16bit or 32bit address and works correctly
* even in the SDM mode. Buffer address is a standard ANSI C pointer
*
* Warning: This call assumes CodeWarrior style of passing parameters,
* i.e. 32 bit operands passed in A and B accumulators
*
* Warning2: MC_ADDR type MUST be numeric! (UWord type)
*
******************************************************************************/

void MC_CopyMemory(MC_ADDR nDestAddr, MC_ADDR nSrcAddr, MC_SIZE8 nSize)
{
	MC_U8* pSrcAddr = (MC_U8 *) nSrcAddr;
	MC_U8* pDestAddr = (MC_U8 *) nDestAddr;

	while(nSize--)
	{
		*pDestAddr++ = *pSrcAddr++;
	}
}

/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************
*
* This function accepts both 16bit or 32bit address and works correctly
* even in the SDM mode. Buffer address is a standard ANSI C pointer
*
* Warning: This call assumes CodeWarrior style of passing parameters,
* i.e. 32 bit operands passed in A and B accumulators
*
* Warning2: MC_ADDR type MUST be numeric! (UWord type)
*
******************************************************************************/

MC_BPTR MC_CopyToBuffer(MC_BPTR pDestBuff, MC_ADDR nSrcAddr, MC_SIZE8 nSize)
{
	MC_U8* pSrcAddr = (MC_U8 *) nSrcAddr;
	MC_U8 counter = 0;

	while (counter < nSize)
	{
		*pDestBuff++ = *pSrcAddr++;

		counter++;
	}

	return pDestBuff;
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************
*
* This function accepts both 16bit or 32bit address and works correctly
* even in the SDM mode. Buffer address is a standard ANSI C pointer
*
* Warning: This call assumes CodeWarrior style of passing parameters,
* i.e. 32 bit operands passed in A and B accumulators
*
* Warning2: MC_ADDR type MUST be numeric! (UWord type)
*
******************************************************************************/

MC_BPTR MC_CopyFromBuffer(MC_ADDR nDestAddr, MC_BPTR pSrcBuff, MC_SIZE8 nSize)
{
	MC_U8* pDestAddr = (MC_U8 *) nDestAddr;
	MC_U8 counter = 0;

	while (counter < nSize)
	{
		*pDestAddr++ = *pSrcBuff++;

		counter++;
	}

	return pSrcBuff;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************
*
* This function accepts both 16bit or 32bit address and works correctly
* even in the SDM mode. Buffer address is a standard ANSI C pointer
*
* Warning: This call assumes CodeWarrior style of passing parameters,
* i.e. 32 bit operands passed in A and B accumulators
*
* Warning2: MC_ADDR type MUST be numeric! (UWord type)
*
******************************************************************************/

void MC_CopyFromBufferWithMask(MC_ADDR nDestAddr, MC_BPTR pSrcBuff, MC_SIZE8 nSize)
{
	MC_U8* pDestAddr = (MC_U8 *) nDestAddr;
	MC_U8* pSrc = pSrcBuff;
	//MC_U8 counter = 0;
	MC_U16 stmp, dtmp, mask;

	while (nSize--)
	{
		mask = *(pSrc + nSize);
		stmp = *pSrc++;
		dtmp = *pDestAddr;
		stmp = (stmp & mask) | (dtmp & ~mask);
		*pDestAddr++ = stmp;
	}

}

//#pragma dont_inline reset

/**************************************************************************//*!
*
* @brief  Select an address size to be used in next access to a comm. buffer
*
* @param  bNextAddrIsEx - when non zero, next expected address is 32bit wide
*                       - when zero, next expected address is 16bit wide
*
******************************************************************************/

#if MC_USE_EX_CMDS && MC_USE_NOEX_CMDS

static MC_BOOL pcm_bUseExAddr;

void MC_SetExAddr(MC_BOOL bNextAddrIsEx)
{
    pcm_bUseExAddr = bNextAddrIsEx;
}

#endif

/**************************************************************************//*!
*
* @brief    Fetch 16/32 bit address from buffer
*
* @param  pAddr - ANSI C pointer to a variable which is to receive the result
* @param  pSrc  - pointer to a source memory in communication buffer
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

MC_BPTR MC_AddressFromBuffer(MC_ADDR* pAddr, MC_BPTR pSrc)
{
    /* do we have to differentiate the EX and non-EX access? */
#if MC_USE_EX_CMDS && MC_USE_NOEX_CMDS
    if(pcm_bUseExAddr)
#endif
    {
#if MC_USE_EX_CMDS
        /* fetch 32bit value */
        MC_U32 nAddr32;
        pSrc = MC_ValueFromBuffer32(&nAddr32, pSrc);
        /* and convert it to the address type */
        *pAddr = (MC_ADDR) nAddr32;
#endif
    }
#if MC_USE_EX_CMDS && MC_USE_NOEX_CMDS
    else
#endif
    {
#if MC_USE_NOEX_CMDS
        /* fetch 16bit value */
        MC_U16 nAddr16;
        pSrc = MC_ValueFromBuffer16(&nAddr16, pSrc);
        /* and convert it to the address type */
        *pAddr = (MC_ADDR) nAddr16;
#endif
    }

    return pSrc;
}

/**************************************************************************//*!
*
* @brief    Store 16/32 bit address to buffer
*
* @param  pDest - pointer to a destination memory in communication buffer
* @param  pSrc  - memory address value
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

MC_BPTR MC_AddressToBuffer(MC_BPTR pDest, MC_ADDR nAddr)
{
    /* do we have to differentiate the EX and non-EX access? */
#if MC_USE_EX_CMDS && MC_USE_NOEX_CMDS
    if(pcm_bUseExAddr)
#endif
    {
#if MC_USE_EX_CMDS
        /* put the address as a 32bit value */
        pDest = MC_ValueToBuffer32(pDest, (MC_U32) nAddr);
#endif
    }
#if MC_USE_EX_CMDS && MC_USE_NOEX_CMDS
    else
#endif
    {
#if MC_USE_NOEX_CMDS
        /* put the address as a 16bit value */
        pDest = MC_ValueToBuffer16(pDest, (MC_U16) nAddr);
#endif
    }

    return pDest;
}

#endif /* !(MC_DISABLE) */

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
