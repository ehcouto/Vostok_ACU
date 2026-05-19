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
#ifndef SOURCE_PROJECTSPECIFIC_COMMON_MASTERCOMMANDER_MASTERCOMMANDSOURCE_MC_LLMEM_H_
#define SOURCE_PROJECTSPECIFIC_COMMON_MASTERCOMMANDER_MASTERCOMMANDSOURCE_MC_LLMEM_H_

#include "mc_private.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void MC_CopyMemory(MC_ADDR nDestAddr, MC_ADDR nSrcAddr, MC_SIZE8 nSize);
MC_BPTR MC_CopyToBuffer(MC_BPTR pDestBuff, MC_ADDR nSrcAddr, MC_SIZE8 nSize);
MC_BPTR MC_CopyFromBuffer(MC_ADDR nDestAddr, MC_BPTR pSrcBuff, MC_SIZE8 nSize);
void MC_CopyFromBufferWithMask(MC_ADDR nDestAddr, MC_BPTR pSrcBuff, MC_SIZE8 nSize);
MC_BPTR MC_AddressFromBuffer(MC_ADDR* pAddr, MC_BPTR pSrc);
MC_BPTR MC_AddressToBuffer(MC_BPTR pDest, MC_ADDR nAddr);

/******************************************************************************
* communication buffer access functions (inlines/functions - depending on cfg)
******************************************************************************/

static inline MC_BPTR MC_ValueFromBuffer16(MC_U16* pDest, MC_BPTR pSrc)
{
    return MC_CopyFromBuffer((MC_ADDR)(MC_U8*)pDest, pSrc, 2);
}

static inline MC_BPTR MC_ValueFromBuffer32(MC_U32* pDest, MC_BPTR pSrc)
{
    return MC_CopyFromBuffer((MC_ADDR)(MC_U8*)pDest, pSrc, 4);
}

static inline MC_BPTR MC_ValueToBuffer16(MC_BPTR pDest, MC_U16 src)
{
    return MC_CopyToBuffer(pDest, (MC_ADDR)(MC_U8*)&src, 2);
}

static inline MC_BPTR MC_ValueToBuffer32(MC_BPTR pDest, MC_U32 src)
{
    return MC_CopyToBuffer(pDest, (MC_ADDR)(MC_U8*)&src, 4);
}


/******************************************************************************
* communication buffer access inlines
******************************************************************************/

static inline MC_BPTR MC_SkipInBuffer(MC_BPTR pDest, MC_SIZE8 nSize)
{
    return pDest + nSize;
}

static inline MC_BPTR MC_ValueFromBuffer8(MC_U8* pDest, register MC_BPTR pSrc)
{
    *pDest = *pSrc++;
    return pSrc;
}

static inline MC_BPTR MC_ValueToBuffer8(MC_BPTR pDest, MC_U8 src)
{
    *pDest++ = src;
    return pDest;
}

static inline MC_BPTR MC_ConstToBuffer8(MC_BPTR pDest, MC_U8 src)
{
    *pDest++ = src;
    return pDest;
}

static inline MC_BPTR MC_ConstToBuffer16(MC_BPTR pDest, MC_U16 src)
{
    return MC_ConstToBuffer8(MC_ConstToBuffer8(pDest, (MC_U8) src), (MC_U8)(src>>8));
}

/****************************************************************************************
* memory access helper macros (used in recorder trigger compare routines)
*****************************************************************************************/

static inline MC_S8 MC_GetS8(register MC_ADDR addr)
{
	return *(MC_S8*)addr;
}

static inline MC_U8 MC_GetU8(register MC_ADDR addr)
{
	return *(MC_U8*)addr;
}

static inline MC_S16 MC_GetS16(register MC_ADDR addr)
{
	return *(MC_S16*)(MC_U8*)addr;
}

static inline MC_U16 MC_GetU16(register MC_ADDR addr)
{
	return *(MC_U16*)(MC_U8*)addr;
}

static inline MC_S32 MC_GetS32(register MC_ADDR addr)
{
	return *(MC_S32*)(MC_U8*)addr;
}
//
static inline MC_U32 MC_GetU32(register MC_ADDR addr)
{
    return *(MC_U32*)(MC_U8*)addr;
}
//
//#if MC_REC_FLOAT_TRIG
//inline MC_FLOAT MC_GetFloat(register MC_ADDR addr)
//{
//    if(sizeof(MC_ADDR) == 4)
//    {
//        register void* raddr;
//        register MC_U32 tmp;
//
//        asm ( tfr addr,A );
//        asm ( move.l A10, raddr );
//        return *(MC_FLOAT*)(MC_U32*)raddr;
//    }
//    else
//    {
//        return *(MC_FLOAT*)(MC_U8*)addr;
//    }
//}
//#endif
//
/****************************************************************************************
* Other helper macros
*****************************************************************************************/

/* This macro assigns C pointer to MC_ADDR-typed variable */
/* should be done in assembly not to trim far-space pointers in SDM */
#ifndef MC_ARR2ADDR
#define MC_ARR2ADDR(addrvar,arr) /*lint -e{717} */ \
    do{ /*lint -esym(529,dw,rreg) -esym(530,dw,rreg) */ \
		addrvar = (MC_ADDR) arr;		\
    } while(0)
#endif

//
//#define MC_PTR2ADDR(tmpAddr, ptr) ( /*lint -e{923} */ tmpAddr = (MC_ADDR) (MC_U8*) ptr )


#endif // SOURCE_PROJECTSPECIFIC_COMMON_MASTERCOMMANDER_MASTERCOMMANDSOURCE_MC_LLMEM_H_
