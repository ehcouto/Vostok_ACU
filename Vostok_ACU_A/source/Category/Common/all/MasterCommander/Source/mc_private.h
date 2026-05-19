/**
 *  @file
 *
 *  @brief      MasterCommander driver private declarations, used internally by the driver
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef __MC_PRIVATE_H
#define __MC_PRIVATE_H

#include "master_commander.h"

#ifndef _MASTER_COMMANDER_H
#error Please include the master_commander.h master header file before the mc_private.h
#endif

#ifdef MC_USE_MQX_IO
  #if MC_USE_MQX_IO
    /* include MQX headers */
    #include <mqx.h>
    #include <bsp.h>
  #endif
#endif

/******************************************************************************
* Platform-dependent types, macros and functions
******************************************************************************/

#undef MC_PLATFORM

/* platform macro MC_PLATFORM_xxxxxxx should be defined  */
/* as "non-zero" in the "mc.h" file */

#if defined(MC_PLATFORM_56F8xx)
    #if MC_PLATFORM_56F8xx
    #include "mc_56F8xx.h"
    #define MC_PLATFORM 56F8xx
    #else
    #undef MC_PLATFORM_56F8xx
    #endif
#endif

#if defined(MC_PLATFORM_56F8xxx)
    #if MC_PLATFORM_56F8xxx
    #include "mc_hal_56F8xxx.h"
    #define MC_PLATFORM 56F8xxx
    #else
    #undef MC_PLATFORM_56F8xxx
    #endif
#endif

#if defined(MC_PLATFORM_RX62T)
    #if MC_PLATFORM_RX62T
    #include "../Source/HAL/mc_hal_rx62t.h"
    #define MC_PLATFORM RX62T
    #else
    #undef MC_PLATFORM_RX62T
    #endif
#endif


#if defined(MC_PLATFORM_HC12)
    #if MC_PLATFORM_HC12
    #include "mc_HC12.h"
    #define MC_PLATFORM HC12
    #else
    #undef MC_PLATFORM_HC12
    #endif
#endif

#if defined(MC_PLATFORM_HC08)
    #if MC_PLATFORM_HC08
    #include "mc_HC08.h"
    #define MC_PLATFORM HC08
    #else
    #undef MC_PLATFORM_HC08
    #endif
#endif

#if defined(MC_PLATFORM_MPC55xx)
    #if MC_PLATFORM_MPC55xx
    #include "mc_MPC55xx.h"
    #define MC_PLATFORM MPC55xx
    #else
    #undef MC_PLATFORM_MPC55xx
    #endif
#endif

#if defined(MC_PLATFORM_MPC5xx)
    #if MC_PLATFORM_MPC5xx
    #include "mc_MPC5xx.h"
    #define MC_PLATFORM MPC5xx
    #else
    #undef MC_PLATFORM_MPC5xx
    #endif
#endif

#if defined(MC_PLATFORM_MCF51xx)
    #if MC_PLATFORM_MCF51xx
    #include "mc_MCF51xx.h"
    #define MC_PLATFORM MCF51xx
    #else
    #undef MC_PLATFORM_MCF51xx
    #endif
#endif

#if defined(MC_PLATFORM_MCF52xx)
    #if MC_PLATFORM_MCF52xx
    #include "mc_MCF52xx.h"
    #define MC_PLATFORM MCF52xx
    #else
    #undef MC_PLATFORM_MCF52xx
    #endif
#endif

#if defined(MC_PLATFORM_MPC56xx)
    #if MC_PLATFORM_MPC56xx
    #include "mc_MPC56xx.h"
    #define MC_PLATFORM MPC56xx
    #else
    #undef MC_PLATFORM_MPC56xx
    #endif
#endif

#if defined(MC_PLATFORM_MQX)
    #if MC_PLATFORM_MQX
    #include "mc_MQX.h"
    #define MC_PLATFORM MQX
    #else
    #undef MC_PLATFORM_MQX
    #endif
#endif

#if defined(MC_PLATFORM_KXX)
    #if MC_PLATFORM_KXX
    #include "mc_hal_Kxx.h"
    #define MC_PLATFORM KXX
    #else
    #undef MC_PLATFORM_KXX
    #endif
#endif

#ifndef MC_PLATFORM
#error Unknown MC driver platform
#endif

/******************************************************************************
* Boolean values
******************************************************************************/

#ifndef MC_TRUE
#define MC_TRUE (1U)
#endif

#ifndef MC_FALSE
#define MC_FALSE (0U)
#endif


/******************************************************************************
* inline functions 
******************************************************************************/

/* we do not assume the inline is always supported by compiler
  rather each platform header defines its MC_INLINE */
#ifndef MC_INLINE
#define MC_INLINE static
#endif

/* building macro-based inline code */
#define MC_MACROCODE_BEGIN()     do{
#define MC_MACROCODE_END()       }while(0)

/******************************************************************************
* Global non-API functions (used internally in mc driver)
******************************************************************************/

MC_BOOL MC_InitSerial(void);
void MC_SendResponse(MC_BPTR pMessageIO, MC_SIZE8 nLength);
void MC_ProcessSCI(void);
void MC_ProcessJTAG(void);

MC_BOOL MC_Tx(MC_U8 * pTxChar);
MC_BOOL MC_Rx(MC_U8   nRxChar);

MC_BOOL MC_ProtocolDecoder(MC_BPTR pMessageIO);
MC_BPTR MC_GetBoardInfo(MC_BPTR pMessageIO);

MC_BPTR MC_ReadMem(MC_BPTR pMessageIO);
MC_BPTR MC_ReadVar(MC_BPTR pMessageIO, MC_SIZE8 nSize);
MC_BPTR MC_WriteMem(MC_BPTR pMessageIO);
MC_BPTR MC_WriteVar(MC_BPTR pMessageIO, MC_SIZE8 nSize);
MC_BPTR MC_WriteVarMask(MC_BPTR pMessageIO, MC_SIZE8 nSize);
MC_BPTR MC_WriteMemMask(MC_BPTR pMessageIO);

void MC_InitAppCmds(void);
MC_BPTR MC_StoreAppCmd(MC_BPTR pMessageIO);
MC_BPTR MC_GetAppCmdStatus(MC_BPTR pMessageIO);
MC_BPTR MC_GetAppCmdRespData(MC_BPTR pMessageIO);

void MC_InitScope(void);
MC_BPTR MC_SetUpScope(MC_BPTR pMessageIO);
MC_BPTR MC_ReadScope(MC_BPTR pMessageIO);

void MC_InitRec(void);
MC_BPTR MC_SetUpRec(MC_BPTR pMessageIO);
MC_BPTR MC_StartRec(MC_BPTR pMessageIO);
MC_BPTR MC_StopRec(MC_BPTR pMessageIO);
MC_BPTR MC_GetRecStatus(MC_BPTR pMessageIO);
MC_BPTR MC_GetRecBuff(MC_BPTR pMessageIO);
MC_BOOL MC_IsInRecBuffer(MC_ADDR nAddr, MC_SIZE8 nSize);
MC_SIZE MC_GetRecBuffSize(void);

void MC_InitTsa(void);
MC_BPTR MC_GetTsaInfo(MC_BPTR pMessageIO);
MC_BPTR MC_GetStringLen(MC_BPTR pMessageIO);
MC_BOOL MC_CheckTsaSpace(MC_ADDR nAddr, MC_SIZE8 nSize, MC_BOOL bWriteAccess);
MC_U16  MC_StrLen(MC_ADDR nAddr);

void MC_InitSfio(void);
MC_BPTR MC_SfioFrame(MC_BPTR pMessageIO);
MC_BPTR MC_SfioGetResp(MC_BPTR pMessageIO);

void MC_InitPipes(void);
MC_BPTR MC_PipeFrame(MC_BPTR pMessageIO);

void MC_InitCan(void);
void MC_SetCanCmdID(MC_U32 canID);
void MC_SetCanRespID(MC_U32 canID);
MC_BOOL MC_TxCan(void);
MC_BOOL MC_RxCan(void);
void MC_ProcessCanRx(void);
void MC_ProcessCanTx(void);

void MC_InitPDBdm(void);

/* Master&Commander Instructions */
MC_BPTR MC_ReadLF(MC_BPTR pMessageIO);
MC_BPTR MC_WriteLF(MC_BPTR pMessageIO);
MC_BPTR MC_WriteLF_Byte(MC_BPTR pMessageIO);
MC_BPTR MC_ReadFix(MC_BPTR pMessageIO);
MC_BPTR MC_WriteFix(MC_BPTR pMessageIO);
MC_BPTR MC_SingleReq(MC_BPTR pMessageIO);

/****************************************************************************************
* Potentially unused variable declaration
*****************************************************************************************/
#if defined(_lint) || defined(__IAR_SYSTEMS_ICC__) || defined(__ARMCC_VERSION) 
#define MC_UNUSED(sym) /*lint -esym(715,sym) -esym(818,sym) -esym(529,sym) -e{960} */
#elif defined(__GNUC__)
#define MC_UNUSED(x) (void)(x)
#else
#define MC_UNUSED(sym) ((sym),0)
#endif


#if (MC_LONG_INTR && (MC_SHORT_INTR || MC_POLL_DRIVEN)) || \
    (MC_SHORT_INTR && (MC_LONG_INTR || MC_POLL_DRIVEN)) || \
    (MC_POLL_DRIVEN && (MC_LONG_INTR || MC_SHORT_INTR)) || \
    !(MC_POLL_DRIVEN || MC_LONG_INTR || MC_SHORT_INTR)
    /* mismatch in interrupt modes, only one can be selected */
    #error You have to enable exctly one of MC_LONG_INTR or MC_SHORT_INTR or MC_POLL_DRIVEN
#endif

#if MC_SHORT_INTR
    #if MC_COMM_RQUEUE_SIZE < 1
    #error Error in MC_COMM_RQUEUE_SIZE value.
    #endif
#endif

/* only one communication link may be selected */
#if ((((MC_USE_SCI)?1:0) + ((MC_USE_JTAG)?1:0) + ((MC_USE_CAN)?1:0) + ((MC_USE_MQX_IO)?1:0) + ((MC_DISABLE)?1:0) + ((MC_USE_USB_CDC)?1:0) + ((MC_USE_PDBDM)?1:0)) > 1)
#error More than one communication interface selected for mc driver
#endif

/* All Interface options are set to 0 */
#if ((((MC_USE_SCI)?1:0) + ((MC_USE_JTAG)?1:0) + ((MC_USE_CAN)?1:0) + ((MC_USE_MQX_IO)?1:0) + ((MC_DISABLE)?1:0) + ((MC_USE_USB_CDC)?1:0) + ((MC_USE_PDBDM)?1:0)) == 0)
#error All Interface options are set to 0, please chooce one intercace or set MC_DISABLE macro to 1.
#endif


/* MQX IO communication interface warnings and errors */
#if MC_USE_MQX_IO
    
    #if !(defined(MC_PLATFORM_MQX) && (MC_PLATFORM_MQX))
       #error MQX IO communication interface is supported only in MC_PLATFORM_MQX, Please choose another communication interface.
    #endif

    /* define mc IO Channel */
    #ifndef MC_MQX_IO_CHANNEL
        #warning MQX IO Channel is not defined by MC_MQX_IO_CHANNEL macro, mc use "ttya:" IO Channel
        #define MC_MQX_IO_CHANNEL "ttya:"
    #endif
   
    /* configure the mc communication channel to non blocking mode */
    #ifndef MC_MQX_IO_BLOCKING
        #warning MC_MQX_IO_BLOCKING macro is not defined, the MQX IO Channel will be open in non blocking mode
        #define MC_MQX_IO_BLOCKING 0
    #endif

    /* MQX open port parameters */
    #if MC_MQX_IO_BLOCKING
         #define MC_MQX_IO_PARAMETER 0
    #else
         #define MC_MQX_IO_PARAMETER IO_SERIAL_NON_BLOCKING
    #endif

    /* MQX doesn't supports Long interrupt and Short interrupt */
    #if MC_LONG_INTR
         #warning "MQX IO interface doesn't supports the Long Interrupt routines"
    #elif MC_SHORT_INTR
         #warning "MQX IO interface doesn't supports the normal Short Interrupt routines, please open your MQX communication interface in Interrupt mode"
    #endif
#endif

#if defined(MC_PLATFORM_MQX)
  #if MC_PLATFORM_MQX

    /* MQX communication interfaces require MQX header files to be included. */
    #ifndef __MQX__
        #error Please include the MQX header file before the mc.h
    #endif

    #if MC_USE_SCI
    #error "MQX target driver doesn't supports the SCI module as communication interface"
    #elif MC_USE_CAN
    #error "MQX target driver doesn't supports the CAN module as communication interface"
    #elif MC_USE_JTAG
    #error "MQX target driver doesn't supports the JTAG module as communication interface"
    #elif MC_USE_USB_CDC
    #error "MQX target driver doesn't supports the USB CDC as communication interface"
    #endif
  #endif
#endif

#if (MC_USE_SCI) || (MC_USE_JTAG) || (MC_USE_USB_CDC) || (MC_USE_MQX_IO)
  #ifndef MC_USE_SERIAL
    #define MC_USE_SERIAL 1
  #else
    #if MC_USE_SERIAL == 0
      #error "MC_USE_SERIAL macro cannot be configured by user, mc serial driver functionality is not garanted."
    #endif
    #endif
#else
    #ifndef MC_USE_SERIAL
        #define MC_USE_SERIAL 0
    #endif
#endif

#if MC_USE_USB_CDC
    /* USB CDC driver doesn't supports POLL mode */
    #if MC_POLL_DRIVEN
         #error "mc USB CDC driver doesn't supports the POLL mode"
    #endif
#endif

#if MC_USE_PDBDM
    /* Packet Driven BDM communication does not support any interrupt mode */
    #if (MC_SHORT_INTR) || (MC_LONG_INTR)
         #warning "mc Packet Driven BDM driver doesn't support any Interrupt mode. Call the MC_Poll() function in the Timer ISR routine instead."
    #endif
#endif

/* check scope settings */
#if MC_USE_SCOPE
    #if MC_MAX_SCOPE_VARS > 8 || MC_MAX_SCOPE_VARS < 2
        #error Error in MC_MAX_SCOPE_VARS value. Use a value in range 2..8
    #endif
#endif  

/* check recorder settings */
#if (MC_USE_RECORDER) || (MC_USE_FASTREC)
    #if MC_MAX_REC_VARS > 8 || MC_MAX_REC_VARS < 2
        #error Error in MC_MAX_REC_VARS value. Use a value in range 2..8
    #endif
    
    #if !MC_USE_READMEM
        #error Recorder needs the MC_USE_READMEM feature
    #endif
#endif

/* fast recorder requires its own allocation of recorder buffer */
#if MC_USE_FASTREC
    #if MC_REC_OWNBUFF
        #error Fast recorder requires its own buffer allocation
    #endif
#endif

#if MC_USE_TSA
    #if !MC_USE_READMEM
        #error TSA needs the MC_USE_READMEM feature
    #endif
#endif

/* check SFIO settings */
#if MC_USE_SFIO

    /* The SFIO configuration files (sfio.h and optionally also the sfio_cfg.h) exist 
       in project to define SFIO parameters. */
    #include "sfio.h"
    
    #ifndef SFIO_MAX_INPUT_DATA_LENGTH
    #error  SFIO_MAX_INPUT_DATA_LENGTH was not defined in sfio_cfg.h
    #endif
    #ifndef SFIO_MAX_OUTPUT_DATA_LENGTH
    #error  SFIO_MAX_OUTPUT_DATA_LENGTH was not defined in sfio_cfg.h
    #endif
    
#endif

#if MC_USE_PIPES

    #ifdef MC_PIPES_EXPERIMENTAL
    #warning The "pipes" feature is now in experimental code phase. Not yet tested on this platform.
    #endif        

    /* must enable printf for vararg printf */
    #if !(MC_USE_PIPE_PRINTF) && (MC_USE_PIPE_PRINTF_VARG)
    #error You must enable pipe printf for vararg printf (see MC_USE_PIPE_PRINTF)
    #endif

    /* at least one */
    #if MC_MAX_PIPES_COUNT < 1
    #warning No sense to allocate zero-count pipes. Disabling pipes.
    #undef  MC_USE_PIPES
    #define MC_USE_PIPES 0
    #endif

    /* printf buffer should accept one integer printed */    
    #if MC_PIPES_PRINTF_BUFF_SIZE < 8
    #error Pipe printf buffer should be at least 8 (see MC_PIPES_PRINTF_BUFF_SIZE)
    #endif
    #if MC_PIPES_PRINTF_BUFF_SIZE > 255
    #error Pipe printf buffer should not exceed 255 (see MC_PIPES_PRINTF_BUFF_SIZE)
    #endif
    
#endif

/* check what kind of board info is sent */
#if MC_USE_BRIEFINFO
    #if MC_USE_RECORDER
        #warning The full information structure must be used when recorder is to be used
        #undef  MC_USE_BRIEFINFO
        #define MC_USE_BRIEFINFO 1
    #endif
#endif

/* automatic buffer size by default */
#ifndef MC_COMM_BUFFER_SIZE
#define MC_COMM_BUFFER_SIZE 0
#endif

/* check minimal buffer size required for all enabled features */
#if MC_COMM_BUFFER_SIZE
    
    /* basic commands (getinfobrief, write/read memory etc.) */
    #if MC_USE_BRIEFINFO && (MC_COMM_BUFFER_SIZE < 11)
    #error MC_COMM_BUFFER_SIZE set too small for basic mc commands (11 bytes)
    #endif

    /* full info required */
    #if !(MC_USE_BRIEFINFO) && (MC_COMM_BUFFER_SIZE < 35)
    #error MC_COMM_BUFFER_SIZE set too small for GETINFO command (size 35)
    #endif

    /* application commands */
    #if MC_USE_APPCMD && (MC_COMM_BUFFER_SIZE < ((MC_APPCMD_BUFF_SIZE)+1))
    #error MC_COMM_BUFFER_SIZE set too small for SENDAPPCMD command (size MC_APPCMD_BUFF_SIZE+1)
    #endif

    /* configuring scope (EX) */
    #if MC_USE_SCOPE && (MC_COMM_BUFFER_SIZE < (((MC_MAX_SCOPE_VARS)*5)+1))
    #error MC_COMM_BUFFER_SIZE set too small for SETUPSCOPEEX command (size MC_MAX_SCOPE_VARS*5+1)
    #endif

    /* configuring recorder (EX) */
    #if MC_USE_RECORDER && (MC_COMM_BUFFER_SIZE < (((MC_MAX_REC_VARS)*5)+18))
    #error MC_COMM_BUFFER_SIZE set too small for SETUPRECEX command (size MC_MAX_REC_VARS*5+18)
    #endif
    
    /* SFIO encapsulation */
    #if MC_USE_SFIO
    #if (MC_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1)) || \
        (MC_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1))
    #error MC_COMM_BUFFER_SIZE set too small for SFIO encapsulation (see SFIO_MAX_xxx_DATA_LENGTH)
    #endif
    #endif

#endif

#endif /* __MC_PRIVATE_H */

