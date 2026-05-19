/**
 *  @file
 *
 *  @brief      MasterCommander BDM communication routines
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "master_commander.h"
#include "mc_private.h"
#include "mc_protocol.h"


#if (MC_COMM_INTERFACE == MC_INTERFACE_PDBDM)
#if (MC_PLATFORM_KXX == 0)
#error BDM interface is only supported for KXX architecture
#endif /* (MC_PLATFORM_KXX == 0) */
typedef struct
{
    MC_U16 buffBorder1;                 // Border code of Packet driven BDM communication buffer
    MC_U8 buffSize;                     // Size of Packet driven BDM communication buffer
    MC_U8 bdmState;                     // State of the Packet driven BDM communication
    MC_BCHR commBuffer[MC_COMM_BUFFER_SIZE+1]; //MasterCommander Packet driven BDM communication buffer (in/out) plus the STS
    MC_U16 buffBorder2;                 // Border code of Packet driven BDM communication buffer
}MC_PDBDM_COMBUFF;

static MC_PDBDM_COMBUFF pcm_bdm_pCommBuffer; //Packet driven communication buffer

/**************************************************************************//*!
*
* @brief    Packet Driven BDM communication initialization
*
******************************************************************************/

void MC_InitPDBdm(void)
{   
    /* Initialize buffer border and size in communication buffer */
    pcm_bdm_pCommBuffer.buffBorder1 = MC_PDBDM_BUFFBORDER_CODE1;
    pcm_bdm_pCommBuffer.buffSize = MC_COMM_BUFFER_SIZE + 1;
    pcm_bdm_pCommBuffer.buffBorder2 = MC_PDBDM_BUFFBORDER_CODE2;

    /* initialize state variables */
    pcm_bdm_pCommBuffer.bdmState = MC_PDBDM_IDLE;
}

/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (including the status byte). Change state of BDM driver.
*
******************************************************************************/

void MC_SendResponse(MC_BPTR pResponse, MC_SIZE8 nLength)
{
    MC_UNUSED(pResponse);
    MC_UNUSED(nLength);
    /* Frame is ready to send */
    pcm_bdm_pCommBuffer.bdmState = MC_PDBDM_FRAME_TO_SEND;
}

/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
*******************************************************************************/

void MC_Poll(void)
{ 
    if (pcm_bdm_pCommBuffer.bdmState == MC_PDBDM_RECEIVED_FRAME)
    {
        /* Packet is received */ 
        pcm_bdm_pCommBuffer.bdmState = MC_PDBDM_DECODING_FRAME;
        /* Decode packet and create response */
        MC_ProtocolDecoder(pcm_bdm_pCommBuffer.commBuffer);
    }
}
#else /* (MC_COMM_INTERFACE == MC_INTERFACE_PDBDM) */

#endif /* (MC_COMM_INTERFACE == MC_INTERFACE_PDBDM) */

