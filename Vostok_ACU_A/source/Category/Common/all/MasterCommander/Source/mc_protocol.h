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

#ifndef __MC_PROTOCOL_H
#define __MC_PROTOCOL_H

/*-------------------------------------
  command message - standard commands
  -------------------------------------*/
  
#define MC_CMD_READMEM           0x01U
#define MC_CMD_WRITEMEM          0x02U
#define MC_CMD_WRITEMEMMASK      0x03U
#define MC_CMD_READMEM_EX        0x04U    /* read a block of memory */
#define MC_CMD_WRITEMEM_EX       0x05U    /* write a block of memory */
#define MC_CMD_WRITEMEMMASK_EX   0x06U    /* write block of memory with bit mask */
#define MC_CMD_SETUPSCOPE        0x08U
#define MC_CMD_SETUPREC          0x09U
#define MC_CMD_SETUPSCOPE_EX     0x0aU    /* setup the oscilloscope */
#define MC_CMD_SETUPREC_EX       0x0bU    /* setup the recorder */
#define MC_CMD_SENDAPPCMD        0x10U    /* send the application command */
#define MC_CMD_GETTSAINFO        0x11U    /* get TSA info */
#define MC_CMD_GETTSAINFO_EX     0x12U    /* get TSA info 32bit */
#define MC_CMD_SFIOFRAME_1       0x13U    /* deliver & execute SFIO frame (even) */
#define MC_CMD_SFIOFRAME_0       0x14U    /* deliver & execute SFIO frame (odd) */
#define MC_CMD_PIPE              0x15U    /* read/write pipe data */
#define MC_CMD_READLF			 0x23U    /**< Read Luigi Fagnano command */
#define MC_CMD_WRITELF			 0x24U    /**< Write Luigi Fagnano command */
#define MC_CMD_WRITELF_BYTE		 0x25U    /**< Write Luigi Fagnano command, byte variables */
#define MC_CMD_WRITEFIX          0x26U    /**< Write to fixed RAM addresses */
#define MC_CMD_SINGLEREQ         0x27U    /**< Single request command */

/*-------------------------------------
  command message - Fast Commands
  -------------------------------------*/
  
/* no data part */
#define MC_CMD_GETINFO           0xc0U    /* retrieve board information structure */
#define MC_CMD_STARTREC          0xc1U    /* start data recorder */
#define MC_CMD_STOPREC           0xc2U    /* stop data recorder */
#define MC_CMD_GETRECSTS         0xc3U    /* get the recorder status */
#define MC_CMD_GETRECBUFF        0xc4U
#define MC_CMD_READSCOPE         0xc5U    /* read the scope data */
#define MC_CMD_GETAPPCMDSTS      0xc6U    /* get the application command status */
#define MC_CMD_GETINFOBRIEF      0xc8U    /* retrieve a subset of board information structure */
#define MC_CMD_GETRECBUFF_EX     0xc9U    /* get the recorder data */
#define MC_CMD_SFIOGETRESP_0     0xcaU    /* retry to get last SFIO response (even) */
#define MC_CMD_SFIOGETRESP_1     0xcbU    /* retry to get last SFIO response (odd) */
#define MC_CMD_READFIX           0xccU    /**< Read from fixed RAM addresses */

/* 2 bytes data part */
#define MC_CMD_READVAR8          0xD0U
#define MC_CMD_READVAR16         0xD1U
#define MC_CMD_READVAR32         0xD2U
#define MC_CMD_GETAPPCMDDATA     0xD3U    /* get the application command data */
#define MC_CMD_GETSTRLEN         0xD4U    /* get string length (required by TSA) */

/* 4 bytes data part */
#define MC_CMD_READVAR8_EX       0xe0U    /* read byte variable */
#define MC_CMD_READVAR16_EX      0xe1U    /* read word variable */
#define MC_CMD_READVAR32_EX      0xe2U    /* read dword variable */
#define MC_CMD_WRITEVAR8         0xe3U    /* write byte variable */
#define MC_CMD_WRITEVAR16        0xe4U    /* write word variable */
#define MC_CMD_WRITEVAR8MASK     0xe5U    /* write specified bits in byte variable  */
#define MC_CMD_GETSTRLEN_EX      0xe6U    /* get string length (required by TSA) */

/* 6 bytes data part */
#define MC_CMD_WRITEVAR32        0xf0U    /* write dword variable */
#define MC_CMD_WRITEVAR16MASK    0xf1U    /* write specified bits in word variable */

/*-------------------------------------
  response message - status byte
  -------------------------------------*/

/* flags in response codes */
#define MC_STSF_ERROR            0x80U    /* FLAG: error answer (no response data) */
#define MC_STSF_VARLEN           0x40U    /* FLAG: variable-length answer (length byte) */
#define MC_STSF_EVENT            0x20U    /* FLAG: reserved */
  
/* confirmation codes */
#define MC_STS_OK                0x00U    /* operation finished successfully */
#define MC_STS_RECRUN            0x01U    /* data recorder is running */
#define MC_STS_RECDONE           0x02U    /* data recorder is stopped */

/* error codes */
#define MC_STC_INVCMD            0x81U    /* unknown command code */
#define MC_STC_CMDCSERR          0x82U    /* command checksum error */
#define MC_STC_CMDTOOLONG        0x83U    /* command is too long */
#define MC_STC_RSPBUFFOVF        0x84U    /* the response would not fit into transmit buffer */
#define MC_STC_INVBUFF           0x85U    /* invalid buffer length or operation */
#define MC_STC_INVSIZE           0x86U    /* invalid size specified */
#define MC_STC_SERVBUSY          0x87U    /* service is busy */
#define MC_STC_NOTINIT           0x88U    /* service is not initialized */
#define MC_STC_EACCESS           0x89U    /* access is denied */
#define MC_STC_SFIOERR           0x8AU    /* Error in SFIO frame */
#define MC_STC_SFIOUNMATCH       0x8BU    /* Even/odd mismatch in SFIO transaction */
#define MC_STC_PIPEERR           0x8CU    /* Pipe error */
#define MC_STC_FASTRECERR        0x8DU    /* Feature not implemented in Fast Recorder */
#define MC_STC_CANTGLERR         0x8EU    /* CAN fragmentation (toggle bit) error */
#define MC_STC_CANMSGERR         0x8FU    /* CAN message format error */
#define MC_STC_FLOATDISABLED     0x90U    /* Floating point triggering is not enabled */

/******************************************************************************
* Protocol constants 
*******************************************************************************/

#define MC_SOB                   0x2bU    /* '+' - start of message*/
#define MC_FASTCMD               0xc0U    /* code of fast cmd 0xc0> */
#define MC_FASTCMD_DATALEN_MASK  0x30U    /* mask of data length part of fast command */
#define MC_FASTCMD_DATALEN_SHIFT 3
#define MC_DESCR_SIZE            25U      /* length board desription string */

/* Board configuration flags  */
#define MC_CFGFLAG_BIGENDIAN       0x01U      /*/< used when CPU is big endian */

/* TSA-global flags  */
#define MC_TSA_INFO_VERSION_MASK   0x000fU    /*/< TSA version  */
#define MC_TSA_INFO_32BIT          0x0100U    /*/< TSA address format (16/32 bit) */
#define MC_TSA_INFO_HV2BA          0x0200U    /*/< TSA HawkV2 byte-addressing mode */

/******************************************************************************
* CAN Protocol constants 
*******************************************************************************/

/* control byte (the first byte in each CAN message) */
#define MC_CANCTL_TGL 0x80   /* toggle bit, first message clear, then toggles */
#define MC_CANCTL_M2S 0x40   /* master to slave direction */
#define MC_CANCTL_FST 0x20   /* first CAN message of mc packet */
#define MC_CANCTL_LST 0x10   /* last CAN message of mc packet */
#define MC_CANCTL_SPC 0x08   /* special command (in data[1], handled by CAN sublayer (no FM protocol decode) */
#define MC_CANCTL_LEN_MASK 0x07   /* number of data bytes after the CTL byte (0..7) */

/* special commands */
#define MC_CANSPC_PING 0xc0

/******************************************************************************
* Packet Driven BDM Protocol constants 
*******************************************************************************/

/* Communication buffer borders */
#define MC_PDBDM_BUFFBORDER_CODE1 0x07e0  /* Beginning Magic Number for Packet driven BDM communication buffer */
#define MC_PDBDM_BUFFBORDER_CODE2 0xf81f  /* Ending Magic Number for Packet driven BDM communication buffer */

/* Packet Driven BDM Communication States */
#define MC_PDBDM_NOT_INIT         0x00    /* mc driver is not initalized */
#define MC_PDBDM_IDLE             0x01    /* mc serial driver is configured to BDM and ready to communicate */
#define MC_PDBDM_RECEIVED_FRAME   0x02    /* mc Communication buffer includes frame to be proceeded */
#define MC_PDBDM_DECODING_FRAME   0x04    /* Received frame is proceeding */
#define MC_PDBDM_FRAME_TO_SEND    0x08    /* mc Communication buffer includes frame to be send back */

#endif /* __MC_PROTOCOL_H */

