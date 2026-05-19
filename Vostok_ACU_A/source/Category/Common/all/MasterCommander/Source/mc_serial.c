/**
 *  @file
 *
 *  @brief      MasterCommander serial communication routines
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
#include "MasterCommander.h"

#if !(MC_DISABLE)

#if MC_USE_SERIAL

/***********************************
*  local variables 
***********************************/

/* mc communication buffer (in/out) plus the STS and LEN bytes */
static MC_BCHR pcm_pCommBuffer[MC_COMM_BUFFER_SIZE+3];

/* mc runtime flags */
/*lint -e{960} using union */
typedef volatile union 
{
    MC_FLAGS all;
    
    struct
    {
        unsigned bTxActive : 1;         /* response is being transmitted */
#if MC_USE_SCI
        unsigned bTxWaitTC : 1;         /* response sent, wait for transmission complete */
#endif
        unsigned bTxLastCharSOB   : 1;  /* last transmitted char was equal to SOB  */
        unsigned bRxLastCharSOB   : 1;  /* last received character was SOB */
        unsigned bRxMsgLengthNext : 1;  /* expect the length byte next time */
#if MC_USE_JTAG
        unsigned bJtagRIEPending  : 1;  /* JTAG RIE bit failed to be set, try again later */
#endif
#if (MC_USE_USB_CDC) || (MC_USE_MQX_IO) || (MC_USE_JTAG)
        unsigned bTxFirstSobSend  : 1;  /* to send SOB char at the begin of the packet */
#endif
#if MC_USE_MQX_IO
        unsigned bMqxReadyToSend  : 1;  /* to send next character in transmit routine */
#endif
#if MC_USE_USB_CDC
        unsigned bUsbCdcStartApp   : 1; /* mc USB CDC Application start Init Flag */
        unsigned bUsbCdcStartTrans : 1; /* mc USB CDC Application Carrier Activate Flag */
        unsigned bUsbReadyToDecode : 1; /* mc packet is received, ready to decode in Poll function in Short Interrupt mode */
#endif
    } flg;
    
} MC_SERIAL_FLAGS;

static MC_SERIAL_FLAGS pcm_wFlags;

/* receive and transmit buffers and counters */
static MC_SIZE8 pcm_nTxTodo;     /* transmission to-do counter (0 when tx is idle) */
static MC_SIZE8 pcm_nRxTodo;     /* reception to-do counter (0 when rx is idle) */
static MC_BPTR  pcm_pTxBuff;     /* pointer to next byte to transmit */
static MC_BPTR  pcm_pRxBuff;     /* pointer to next free place in RX buffer */
static MC_BCHR  pcm_nRxCheckSum; /* checksum of data being received */

/***********************************
*  local function prototypes
***********************************/

static void MC_Listen(void);
static void MC_SendError(MC_BCHR nErrCode);

#else /* MC_USE_SERIAL */

/*lint -efile(766, mc_protocol.h) include file is not used in this case */

#endif /* MC_USE_SERIAL */

#if (MC_USE_SCI) || (MC_USE_JTAG)
/***********************************
*  local variables 
***********************************/

/* SHORT_INTR receive queue (circular buffer) */
#if MC_SHORT_INTR
static MC_BCHR  pcm_pRQueueBuffer[MC_COMM_RQUEUE_SIZE];
static MC_BPTR  pcm_pRQueueRP;   /* SHORT_INTR queue read-pointer */
static MC_BPTR  pcm_pRQueueWP;   /* SHORT_INTR queue write-pointer */
#endif

/***********************************
*  local function prototypes
***********************************/

#if MC_SHORT_INTR
static void MC_RxQueue(MC_BCHR nRxChar);
static void MC_RxDequeue(void);
#endif

/*lint -esym(752,MC_RxQueue) this may be unreferenced in some cases */
/*lint -esym(752,MC_RxDequeue) this may be unreferenced in some cases */

/*******************************************************************************
*
* @brief    Routine to quick-receive a character (put to a queue only)
*
* This function puts received character into a queue and exits as soon as possible.
*
*******************************************************************************/

#if MC_SHORT_INTR

static void MC_RxQueue(MC_BCHR nRxChar)
{
    /* future value of write pointer */
    MC_BPTR wpnext = pcm_pRQueueWP + 1;
    
    /*lint -e{946} pointer arithmetic is okay here (same array) */
    if(wpnext >= (pcm_pRQueueBuffer + MC_COMM_RQUEUE_SIZE))
    {
        wpnext = pcm_pRQueueBuffer;
    }
    
    /* any space in queue? */
    if(wpnext != pcm_pRQueueRP)
    {
        *pcm_pRQueueWP = (MC_U8) nRxChar;
        pcm_pRQueueWP = wpnext;
    }
}

#endif /* MC_SHORT_INTR  */

/*******************************************************************************
*
* @brief    Late processing of queued characters
*
* This function takes the queued characters and calls MC_Rx() for each of them,
* just like as the characters would be received from SCI one by one.
*
*******************************************************************************/

#if MC_SHORT_INTR

static void MC_RxDequeue(void)
{ 
    MC_BCHR nChar = 0U;
    
    /* get all queued characters */
    while(pcm_pRQueueRP != pcm_pRQueueWP)
    {
        nChar = *pcm_pRQueueRP++;

        /*lint -e{946} pointer arithmetic is okay here (same array) */
        if(pcm_pRQueueRP >= (pcm_pRQueueBuffer + MC_COMM_RQUEUE_SIZE))
        {
            pcm_pRQueueRP = pcm_pRQueueBuffer;
        }
        
        /* emulate the SCI receive event */
        if(!pcm_wFlags.flg.bTxActive)
        {
            (void)MC_Rx(nChar);
        }
    }
}

#endif /* MC_SHORT_INTR */

#endif /* (MC_USE_SCI) || (MC_USE_JTAG) */

#if MC_USE_SCI

/**************************************************************************//*!
*
* @brief    Handle SCI communication (both TX and RX)
*
* This function checks the SCI flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from SCI ISR or from the polling routine
*
******************************************************************************/

void MC_ProcessSCI(void)
{
	unsigned char i;
    /* read & clear status     */
    MC_SCISR wSciSR;

    /* transmitter active and empty? */
    if (pcm_wFlags.flg.bTxActive)
    {
    	wSciSR = Uart__GetStatus(MASTERCOMMANDER_COMPORT, UART_STATUS_TX_AVAILABLE);

        /* able to accept another character? */
        if(wSciSR)
        {
			MC_U8 ch;
			/* just put the byte into the SCI transmit buffer */
			if(!MC_Tx(&ch))
				Uart__WriteByte(MASTERCOMMANDER_COMPORT, ch);
        }
        
#if MC_SCI_HAS_TXQUEUE
        /* waiting for transmission complete flag? */
        if(pcm_wFlags.flg.bTxWaitTC && (wSciSR & MC_SCISR_TC))
        {
            /* after TC, we can switch to listen mode safely */
            MC_Listen();
        }
#endif


#if !MC_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        if(wSciSR & MC_SCISR_RDRF)
        {
            /*lint -esym(550, nRxChar) */
            volatile MC_U16 nRxChar;
            nRxChar = Uart__ReadByte(MASTERCOMMANDER_COMPORT);
        }
#endif
    }
    /* transmitter not active, able to receive */
    else
    {
    	wSciSR = Uart__GetStatus(MASTERCOMMANDER_COMPORT, UART_STATUS_RX_AVAILABLE);

        /* data byte received? */
        if(wSciSR)
        {
        	for(i = 0; i < wSciSR; i++)
        	{
				MC_BCHR nRxChar = 0U;
				nRxChar = (MC_BCHR)Uart__ReadByte(MASTERCOMMANDER_COMPORT);

#if MC_SHORT_INTR
			MC_RxQueue(nRxChar);
#else
			(void)MC_Rx(nRxChar);
#endif
        	}
        }
    }
}

#endif /* MC_USE_SCI */


#if MC_USE_JTAG

/***********************************
*  local function prototypes
***********************************/

static void MC_FlushJtagTx(void);

/**************************************************************************//*!
*
* @brief    Flush one communication JTAG word 
*
* This function gets the 4 bytes from mc TX buffer and place them to
* JTAG transmit register.
*
******************************************************************************/

static void MC_FlushJtagTx(void)
{
    MC_U8 i;
    MC_U32 wJtagTxData;
    register MC_U16 wJtagSR = MC_JTAG_GETSR();
    /* complete word to send */
    for (i=0; i<4; i++)
    {
        MC_U8 ch;
        if (MC_Tx(&ch))
            ch = 0xff;
        wJtagTxData = (wJtagTxData << 8) | ch;
    }

    /* send the word just completed */
    MC_JTAG_PUTDWORD(wJtagTxData);
}

/**************************************************************************//*!
*
* @brief    Handle JTAG communication (both TX and RX)
*
* This function checks the JTAG flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from JTAG ISR or from the polling routine
*
******************************************************************************/

void MC_ProcessJTAG(void)
{
    /* read & clear status     */
    register MC_U16 wJtagSR = MC_JTAG_GETSR();

    /* transmitter active? */
    if (pcm_wFlags.flg.bTxActive)
    {
        /* able to transmit a new character? (TX must be empty = read-out by PC) */
        if(!(wJtagSR & MC_JTAG_OTXRXSR_TDF))
        {
        
#if MC_USE_JTAG_TXFIX
            /* if TDF bit is useless due to silicon bug, use the RX flag */
            /* instead (PC sends us a dummy word to kick the RX flag on) */
            if(wJtagSR & MC_JTAG_OTXRXSR_RDF)
#endif
            {
                MC_FlushJtagTx();
            }               
        }

        /* ignore (read-out) the JTAG-received word */
        if(wJtagSR & MC_JTAG_OTXRXSR_RDF)
        {
            /*lint -esym(550, nRxWord) */
            volatile MC_U16 nRxWord;
            nRxWord = MC_JTAG_GETWORD();
        }
    }
    /* transmitter not active */
    else
    {
        /* JTAG 32bit word (four bytes) received? */
        if(wJtagSR & MC_JTAG_OTXRXSR_RDF)
        {
            register MC_U32 nRxDWord;
            MC_INDEX i;
            
            nRxDWord = MC_JTAG_GETDWORD();
            
            /* process all bytes, MSB first */
            for(i=0; i<4; i++)
            {
#if MC_SHORT_INTR
                MC_RxQueue((MC_BCHR)((nRxDWord >> 24U) & 0xffU));

#else
                (void)MC_Rx((MC_BCHR)((nRxDWord >> 24U) & 0xffU));
            
                /* ignore the rest if previous bytes triggered a transmission */
                /* (i.e. the packet was complete and only filled-up to 32bit word) */
                if(pcm_wFlags.flg.bTxActive)
                {
                    break;
                }
#endif
                /* next byte of 32bit word */
                nRxDWord = nRxDWord << 8;
            }
        }
    }
}

#endif /* MC_USE_JTAG */

/******************************************************************************
****************************************************************************//*!
*
* mc MQX IO serial communication routines
* 
*******************************************************************************/

#if MC_USE_MQX_IO

#include <mqx.h>
#include <bsp.h>

/***********************************
*  local variables 
***********************************/

static FILE_PTR devfd = NULL;       /* pointer to open mc communication interface */

/***********************************
*  local function prototypes
***********************************/

static MC_BOOL MC_InitMQX(void);
static void MC_ProcessMQXIO(void);

/**************************************************************************//*!
*
* @brief    MQX communication interface initialization
*
******************************************************************************/

static MC_BOOL MC_InitMQX(void)
{   
    /* Open communication port */
    devfd = fopen (MC_MQX_IO_CHANNEL, (char const *)MC_MQX_IO_PARAMETER);

    return (devfd != NULL);
}

/*******************************************************************************
*
* @brief    Handle MQX IO serial communication (both TX and RX)
*
* This function calls MQX IO fread() function to get character and process it by 
* 
* MC_Rx function when mc packet is receiving. This function also transmit
*
* mc response. Character to be send is provided by call of MC_Tx function
* 
* and passed down to fwrite() function.
*
*******************************************************************************/

static void MC_ProcessMQXIO(void)
{ 
    if (devfd != NULL)
    {
         static MC_U8 TxChar = 0;
        /* transmitter not active, able to receive */
        if ((!pcm_wFlags.flg.bTxActive) && (!pcm_wFlags.flg.bMqxReadyToSend))
        {
            MC_U8 nRxChar;
            /* read all available bytes from communication interface */
            while (fread(&nRxChar, 1, 1, devfd)) { 
                if(MC_Rx(nRxChar))
                    break;
            }
        }

        /* transmitter active and empty? */
        if (pcm_wFlags.flg.bTxActive || pcm_wFlags.flg.bMqxReadyToSend)
        {
            while (1)
            {
                /* write character when is valid */
                if (pcm_wFlags.flg.bMqxReadyToSend)
                {
                    if (fwrite(&TxChar, 1, 1, devfd))
                        /* character was successfully send, ready to get next character */
                        pcm_wFlags.flg.bMqxReadyToSend = 0;
                    else
                        /* character write failed, needs to be send next call */
                        break;

                }
                /* is ready to get next character? */
                if (MC_Tx((MC_U8*)&TxChar))
                    /* mc packet is sent, exit loop */
                    break;
                /* read next character, set its validity to be send */
                pcm_wFlags.flg.bMqxReadyToSend = 1;
            }
        }	
    }
}

#else /* MC_USE_MQX_IO */

/*lint -efile(766, mc_protocol.h) include file is not used in this case */

#endif /* MC_USE_MQX_IO */

/******************************************************************************
****************************************************************************//*!
*
* mc USB CDC serial communication routines
* 
*******************************************************************************/

#if MC_USE_USB_CDC
#include "usb_cdc.h"        /* USB CDC Class Header File */

/***********************************
*  Global variables
***********************************/
MC_U8 send_buf[DIC_BULK_IN_ENDP_PACKET_SIZE]; //buffer for store packet to send

/***********************************
*  local function prototypes
***********************************/

static MC_BOOL MC_InitUSB(void);
static void MC_ProcessUSB(void);
static void MC_SendUsbPacket(void);

static void MC_USB_CDC_Callback(MC_U8 controller_ID,
                        MC_U8 event_type, void* val);
static void MC_USB_CDC_Notify_Callback(MC_U8 controller_ID,
                        MC_U8 event_type, void* val);

/**************************************************************************//*!
*
* @brief    USB CDC communication interface initialization
*
******************************************************************************/

static MC_BOOL MC_InitUSB(void)
{   
    MC_U8   error;

    /* Initialize the USB interface */
    error = USB_Class_CDC_Init(MC_USB_CDC_ID,MC_USB_CDC_Callback,
                                NULL,MC_USB_CDC_Notify_Callback, TRUE);
    if(error != USB_OK)
    {
        /* Error initializing USB-CDC Class */
        return MC_FALSE;
    }

    return MC_TRUE;
}

/**************************************************************************//*!
*
* @brief    Handle USB CDC class periodic task and initialize mc TX packet
*
* This function perform USB CDC periodic task. In Short Interrupt mode also decode
* received mc packet and initialize transmit
*
******************************************************************************/
static void MC_ProcessUSB(void)
{ 
    /* call the periodic task function */
    USB_Class_CDC_Periodic_Task();

#if MC_SHORT_INTR
    /*check whether enumeration is complete or not */
    if((pcm_wFlags.flg.bUsbCdcStartApp) && (pcm_wFlags.flg.bUsbCdcStartTrans))
    {
        /*transmitter not active, able to receive */
        if ((!pcm_wFlags.flg.bTxActive) && (pcm_wFlags.flg.bUsbReadyToDecode))
        {
            MC_ProtocolDecoder(pcm_pCommBuffer);
            pcm_wFlags.flg.bUsbReadyToDecode = 0;
        }
    }
#endif	
}


/**************************************************************************//*!
*
* @brief    Get data from mc TX buffer and send the data to USB device
*
* This function checks the mc transmit state and when mc is
* ready to send response to PC, copy mc TX data to local buffer and
* pass down the buffer to USB CDC TX function. 
*
******************************************************************************/

static void MC_SendUsbPacket(void)
{
    /* transmitter active and empty? */
    if (pcm_wFlags.flg.bTxActive)
    {
        MC_U8 index;

        /* fill in the transmitter buffer */
        for (index = 0; index < DIC_BULK_IN_ENDP_PACKET_SIZE; index++){
            MC_U8 TxChar;
            if (MC_Tx(&TxChar))
                break;
            send_buf[index] = TxChar;
        }
        
       USB_Class_CDC_Interface_DIC_Send_Data(MC_USB_CDC_ID, send_buf,index);
    }
}

/******************************************************************************
 *
 *    @name        MC_USB_CDC_Callback
 *
 *    @brief       This function handles Class callback
 *
 *    @param       controller_ID    : Controller ID
 *    @param       event_type       : Value of the event
 *    @param       val              : gives the configuration value
 *
 *    @return      None
 *
 *****************************************************************************
 * This function is called from the class layer whenever reset occurs or enum
 * is complete. After the enum is complete this function sets a variable so
 * that the application can start.
 * This function also receives DATA Send and RECEIVED Events
 *****************************************************************************/

static void MC_USB_CDC_Callback (
    MC_U8 controller_ID,   /* [IN] Controller ID */
    MC_U8 event_type,      /* [IN] value of the event */
    void* val               /* [IN] gives the configuration value */
)
{
    UNUSED (controller_ID)
    UNUSED (val)
    if(event_type == USB_APP_BUS_RESET)
    {
        pcm_wFlags.flg.bUsbCdcStartApp = 0U;
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {
        pcm_wFlags.flg.bUsbCdcStartApp = 1U;
    }
    else if((event_type == USB_APP_DATA_RECEIVED) && (pcm_wFlags.flg.bUsbCdcStartTrans))
    {
        /* manage received data */
        if (pcm_wFlags.flg.bUsbCdcStartApp)
        {
            /* Copy Received Data buffer to Application Buffer */
            USB_PACKET_SIZE BytesToBeCopied;
            APP_DATA_STRUCT* dp_rcv = (APP_DATA_STRUCT*)val;
            BytesToBeCopied = dp_rcv->data_size;

            /* transmitter not active, able to receive */
            if (!pcm_wFlags.flg.bTxActive)
            {
                MC_U8 index;
                for(index = 0 ; index<BytesToBeCopied ; index++){
                    if(MC_Rx(dp_rcv->data_ptr[index])){
                        break;
                    }
                }
                /* continue receiving, when packet is not decoded */
                if (index == BytesToBeCopied) /* receive next packet only when are not received all bytes of packet */
                    (void)USB_Class_CDC_Interface_DIC_Recv_Data(MC_USB_CDC_ID, NULL, 0);
            }
        }
    }
    else if((event_type == USB_APP_SEND_COMPLETE) && (pcm_wFlags.flg.bUsbCdcStartTrans))
    {
        if ((pcm_wFlags.flg.bUsbCdcStartApp) && (pcm_wFlags.flg.bTxActive))
            /* Finalize USB packet to transmit and send packet */
            MC_SendUsbPacket();
        else
            /* Previous Send is complete. Queue next receive */
            (void)USB_Class_CDC_Interface_DIC_Recv_Data(MC_USB_CDC_ID, NULL, 0);
    }

    return;
}

/******************************************************************************
 *
 *    @name        MC_USB_CDC_Notify_Callback
 *
 *    @brief       This function handles PSTN Sub Class callbacks
 *
 *    @param       controller_ID    : Controller ID
 *    @param       event_type       : PSTN Event Type
 *    @param       val              : gives the configuration value
 *
 *    @return      None
 *
 *****************************************************************************
 * This function handles USB_APP_CDC_CARRIER_ACTIVATED and
 * USB_APP_CDC_CARRIER_DEACTIVATED PSTN Events
 *****************************************************************************/

static void MC_USB_CDC_Notify_Callback (
    MC_U8 controller_ID,   /* [IN] Controller ID */
    MC_U8 event_type,      /* [IN] PSTN Event Type */
    void* val               /* [IN] gives the configuration value */
)
{
    UNUSED (controller_ID)
    UNUSED (val)
    if(pcm_wFlags.flg.bUsbCdcStartApp)
    {
        if(event_type == USB_APP_CDC_CARRIER_ACTIVATED)
        {
            pcm_wFlags.flg.bUsbCdcStartTrans = 1U;
        }
        else if(event_type == USB_APP_CDC_CARRIER_DEACTIVATED)
        {
            pcm_wFlags.flg.bUsbCdcStartTrans = 0U;
        }
    }
    return;
}

#else /* MC_USE_USB_CDC */

/*lint -efile(766, mc_protocol.h) include file is not used in this case */

#endif /* MC_USE_USB_CDC */


#if MC_USE_SERIAL

/**************************************************************************//*!
*
* @brief    Start listening on a serial line
*
* Reset the receiver machine and start listening on a serial line
*
******************************************************************************/

static void MC_Listen(void)
{
    pcm_nRxTodo = 0U;

    /* disable transmitter state machine */
    pcm_wFlags.flg.bTxActive = 0U;

#if MC_USE_SCI
    pcm_wFlags.flg.bTxWaitTC = 0U;
    /* disable transmitter, enable receiver (enables single-wire connection) */
#if !MC_SCI_TWOWIRE_ONLY
    MC_SCI_TD();
    MC_SCI_RE();
#endif
#endif /* MC_USE_SCI */

    /* disable transmit, enable receive interrupts */
#if MC_SHORT_INTR || MC_LONG_INTR
#if MC_USE_SCI
    MC_SCI_DTXI();   /* disable SCI transmit interrupt */
    MC_SCI_ERXI();   /* enable SCI receive interrupt */
    
#elif MC_USE_JTAG
    MC_JTAG_DTXI();  /* disable JTAG transmit interrupt  */
    MC_JTAG_ERXI();  /* enable JTAG receive interrupt  */

    /* RIE bit is forced low by HW until EONCE is first accessed, we will try again in MC_Poll */
    if(!MC_JTAG_ERXI_CHECK())
        pcm_wFlags.flg.bJtagRIEPending = 1;

#endif /* MC_USE_SCI / MC_USE_JTAG  */
#endif /* MC_SHORT_INTR || MC_LONG_INTR */
}

/**************************************************************************//*!
*
* @brief    Send response of given error code (no data) 
*
* @param    nErrCode - error code to be sent
*
******************************************************************************/

static void MC_SendError(MC_BCHR nErrCode)
{
    /* fill & send single-byte response */
    *pcm_pCommBuffer = nErrCode;
    MC_SendResponse(pcm_pCommBuffer, 1U);
}

/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (inlcuding the status byte). It computes the check sum and kicks on tx.
*
******************************************************************************/

void MC_SendResponse(MC_BPTR pResponse, MC_SIZE8 nLength)
{
    MC_U16 chSum = 0U;
    MC_SIZE8 i;
    MC_U8 c;

    /* remember the buffer to be sent */
    pcm_pTxBuff = pResponse;
    
    /* status byte and data are already there, compute checksum only     */
    for (i=0U; i<nLength; i++)
    {
        c = 0U;
        pResponse = MC_ValueFromBuffer8(&c, pResponse);
        /* add character to checksum */
        chSum += c;
        /* prevent saturation to happen on DSP platforms */
        chSum &= 0xffU;
    }
    
    /* store checksum after the message */
    pResponse = MC_ValueToBuffer8(pResponse, (MC_U8) (((MC_U8)(~chSum)) + 1U));

    /* send the message and the checksum and the SOB */
    pcm_nTxTodo = (MC_SIZE8) (nLength + 1U);
    
    /* now transmitting the response */
    pcm_wFlags.flg.bTxActive = 1U;

#if (MC_USE_USB_CDC) || (MC_USE_MQX_IO) || (MC_USE_JTAG)
    pcm_wFlags.flg.bTxFirstSobSend = 1U;
#endif

#if MC_USE_SCI
    pcm_wFlags.flg.bTxWaitTC = 0U;
#endif

    /* do not replicate the initial SOB  */
    pcm_wFlags.flg.bTxLastCharSOB = 0U;
    
#if MC_USE_SCI
    {
        /*lint -esym(550, dummySR) */        
        volatile MC_SCISR dummySR;

        /* disable receiver, enable transmitter (single-wire communication) */
#if !MC_SCI_TWOWIRE_ONLY
        MC_SCI_RD();
        MC_SCI_TE();
#endif        
        /* kick on the SCI transmission (also clears TX Empty flag on some platforms) */
        dummySR = MC_SCI_GETSR();
        MC_SCI_PUTCHAR(MC_SOB);
    }
    
#elif MC_USE_JTAG
    /* kick on the JTAG transmission */
    MC_FlushJtagTx();
#endif

    /* TX interrupt enable, RX interrupt disable */
#if (MC_LONG_INTR) || (MC_SHORT_INTR)
#if MC_USE_SCI
    MC_SCI_DRXI();
    MC_SCI_ETXI();
    
#elif MC_USE_JTAG
#if MC_USE_JTAG_TXFIX
    /* in TX-bugfix mode, keep the RX interrupt enabled as it */
    /* is used as "able-to-TX" notification from the PC */
    MC_JTAG_ERXI();
#else
    /* otherwise, JTAG is very same as the SCI */
    if(pcm_wFlags.flg.bTxActive) //enable TX interrupt only when is not one word response 
    {
        MC_JTAG_DRXI();
        MC_JTAG_ETXI();
    }
#endif              

#elif MC_USE_USB_CDC
    if ((pcm_wFlags.flg.bUsbCdcStartApp) && (pcm_wFlags.flg.bTxActive) && (pcm_wFlags.flg.bUsbCdcStartTrans))
        /* Finalize USB packet to transmit and send packet */
        MC_SendUsbPacket();
    else
        /* Previous Send is complete. Queue next receive */
        (void)USB_Class_CDC_Interface_DIC_Recv_Data(MC_USB_CDC_ID, NULL, 0);
#endif
#endif /* MC_LONG_INTR || MC_SHORT_INTR */
}

/**************************************************************************//*!
*
* @brief    Output buffer transmission
* 
* @param  pTxChar  The character to be transmit 
*
* get ready buffer(prepare data to send)
*
******************************************************************************/

MC_BOOL MC_Tx(MC_U8* pTxChar)
{
#if (MC_USE_USB_CDC) || (MC_USE_MQX_IO) || (MC_USE_JTAG)
    /* to send first SOB byte*/
    if (pcm_wFlags.flg.bTxFirstSobSend)
    {
        *pTxChar = MC_SOB;
        pcm_wFlags.flg.bTxFirstSobSend = 0U;
        return MC_FALSE;
    }
#endif      
    if (pcm_nTxTodo)
    {
        /* fetch & send character ready to transmit */
        /*lint -e{534} ignoring return value */
        (void)MC_ValueFromBuffer8(pTxChar, pcm_pTxBuff);
        
        /* first, handle the replicated SOB characters */
        if (*pTxChar == MC_SOB)
        {
            pcm_wFlags.flg.bTxLastCharSOB ^= 1U;
            if ((pcm_wFlags.flg.bTxLastCharSOB))
            {
                /* yes, repeat the SOB next time */
                return MC_FALSE;
            }
        }
        /* no, advance tx buffer pointer */
        pcm_nTxTodo--;
        pcm_pTxBuff = MC_SkipInBuffer(pcm_pTxBuff, 1U);
        return MC_FALSE;
    }
    
    /* when SCI TX buffering is enabled, we must first wait until all 
       characters are physically transmitted (before disabling transmitter) */
#if MC_USE_SCI
  #if MC_SCI_HAS_TXQUEUE
    pcm_wFlags.flg.bTxWaitTC = 1;

    /* wait for SCI TC interrupt */
    #if MC_SHORT_INTR || MC_LONG_INTR
    MC_SCI_ETCI();
    #endif
  #else
    /* start listening immediately */
    MC_Listen();
  #endif  
#else
    /* start listening immediately */
    MC_Listen();
#endif

    return MC_TRUE;
}


/**************************************************************************//*!
*
* @brief  Handle received character 
*
* @param  nRxChar  The character to be processed 
* 
* Handle the character received and -if the message is complete- call the 
* protocol decode routine. 
*
******************************************************************************/

MC_BOOL MC_Rx(MC_BCHR nRxChar)
{
    MC_SERIAL_FLAGS * pflg = &pcm_wFlags;
    /* first, handle the replicated SOB characters */
    if(nRxChar == MC_SOB)
    {
        pflg->flg.bRxLastCharSOB ^= 1;
        if(pflg->flg.bRxLastCharSOB)
        {
            /* this is either the first byte of replicated SOB or a  */
            /* real Start-of-Block mark - we will decide next time in MC_Rx */
            return MC_FALSE;
        }
    }
    
    /* we have got a common character preceded by the SOB -  */
    /* this is the command code! */
    if(pflg->flg.bRxLastCharSOB)
    {
        /* reset receiving process */
        pcm_pRxBuff = pcm_pCommBuffer;
        *pcm_pRxBuff++ = nRxChar;

        /* start computing the checksum */
        pcm_nRxCheckSum = nRxChar;
        pcm_nRxTodo = 0U;
    
        /* if the standard command was received, the message length will come in next byte */
        pflg->flg.bRxMsgLengthNext = 1U;

        /* fast command? */
        if(!((~nRxChar) & MC_FASTCMD))
        {
            /* fast command received, there will be no length information */
            pflg->flg.bRxMsgLengthNext = 0U;
            /* as it is encoded in the command byte directly */
            pcm_nRxTodo = (MC_SIZE8)
                (((((MC_SIZE8)nRxChar) & MC_FASTCMD_DATALEN_MASK) >> MC_FASTCMD_DATALEN_SHIFT) + 1U);
        }

        /* command code stored & processed */
        pflg->flg.bRxLastCharSOB = 0U;
        return MC_FALSE;
    }

    /* we are waiting for the length byte */
    if(pflg->flg.bRxMsgLengthNext)
    {
        /* this byte, total data length and the checksum */
        pcm_nRxTodo = (MC_SIZE8) (1U + ((MC_SIZE8)nRxChar) + 1U);
        /* now read the data bytes */
        pflg->flg.bRxMsgLengthNext = 0U;

    }

    /* waiting for a data byte? */
    if(pcm_nRxTodo)
    {
        /* add this byte to checksum */
        pcm_nRxCheckSum += nRxChar;

        /* decrease number of expected bytes */
        pcm_nRxTodo--;
        /* was it the last byte of the message (checksum)? */
        if(!pcm_nRxTodo)
        {
            /* receive buffer overflow? */
            if(pcm_pRxBuff == NULL)
            {
                MC_SendError(MC_STC_CMDTOOLONG);
            }
            /* checksum error? */
            else if((pcm_nRxCheckSum & 0xffU) != 0U)
            {
                MC_SendError(MC_STC_CMDCSERR);
            }
            /* message is okay */
            else 
            {
#if (MC_USE_USB_CDC) && (MC_SHORT_INTR)
                /* Decode protocol and send response in Poll function */
                pflg->flg.bUsbReadyToDecode = 1U;
#else
                
                /* do decode now! */
                MC_ProtocolDecoder(pcm_pCommBuffer);
#endif
            }

            return MC_TRUE;
        }
        /* not the last character yet */
        else 
        {   
            /* is there still a space in the buffer? */
            if(pcm_pRxBuff)
            {
                /*lint -e{946} pointer arithmetic is okay here (same array) */
                if(pcm_pRxBuff < (pcm_pCommBuffer + MC_COMM_BUFFER_SIZE))
                {
                    /* store byte  */
                    *pcm_pRxBuff++ = nRxChar;
                }
                /* buffer is full! */
                else
                {
                    /* NULL rx pointer means buffer overflow - but we still need */
                    /* to receive all message characters (for the single-wire mode) */
                    /* so keep "receiving" - but throw away all characters from now */
                    pcm_pRxBuff = NULL;
                }
            }
        }
    }
    return MC_FALSE;
}

/**************************************************************************//*!
*
* @brief    Serial communication initialization
*
******************************************************************************/

MC_BOOL MC_InitSerial(void)
{   
    /* initialize all state variables */
    pcm_wFlags.all = 0U;
    pcm_nTxTodo = 0U;

/* Initialize SCI and JTAG interface */
#if (MC_USE_SCI) && (MC_SCI_TWOWIRE_ONLY)
    /* to enable TX and RX together in mc initialization */
    // RICUPL
    //MC_SCI_TE_RE();
#endif
    
#if (MC_SHORT_INTR) & ((MC_USE_SCI) || (MC_USE_JTAG))
    pcm_pRQueueRP = pcm_pRQueueBuffer;
    pcm_pRQueueWP = pcm_pRQueueBuffer;
#endif

#if MC_USE_MQX_IO
    if (!MC_InitMQX())
        return MC_FALSE;
#endif

#if MC_USE_USB_CDC
    if (!MC_InitUSB())
        return MC_FALSE;
#endif

    /* start listening for commands */
    MC_Listen();
    return MC_TRUE;
}


/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
* This function either handles all the SCI communication (polling-only mode = 
* MC_POLL_DRIVEN) or decodes messages received on the background by SCI interrupt
* (short-interrupt mode = MC_SHORT_INTR).
*
* In the JTAG interrupt-driven mode (both short and long), this function also checks
* if setting the JTAG RIE bit failed recently. This may happen because of the 
* RIE is held low by the EONCE hardware until the EONCE is first accessed from host.
* MC_Init (->MC_Listen) is often called while the PC-side mc is still
* turned off. So really, the JTAG is not enabled by this time and RIE bit is not set.
* This problem is detected (see how bJtagRIEPending is set above in FSMTR_Listen)
* and it is tried to be fixed periodically here in MC_Poll.
*
*******************************************************************************/

void MC_Poll(void)
{ 
#if !MC_POLL_DRIVEN && MC_USE_JTAG
    /* in the interrupt-driven JTAG mode, the JTAG RIE may have failed to be set recently */
    if(pcm_wFlags.flg.bJtagRIEPending)
    {
        MC_JTAG_ERXI();  /* try to enable JTAG receive interrupt now */

        /* succeeded? */
        if(MC_JTAG_ERXI_CHECK())
            pcm_wFlags.flg.bJtagRIEPending = 0; /* yes!, enough until it fails again (never?) */
    }
#endif    

#if MC_USE_MQX_IO
    /* polled MQX IO mode */
    MC_ProcessMQXIO();
#endif

#if MC_POLL_DRIVEN

#if MC_USE_SCI
    /* polled SCI mode */
    MC_ProcessSCI();
    
#elif MC_USE_JTAG
    /* polled JTAG mode */
    MC_ProcessJTAG();
#endif
    
#elif MC_SHORT_INTR

#if MC_USE_USB_CDC
    /*  */
    MC_ProcessUSB();
#elif (MC_USE_SCI || MC_USE_JTAG)

    /* process queued SCI characters */
    MC_RxDequeue();
#endif

#endif
}

#endif /* MC_USE_SERIAL */

#endif /* !(MC_DISABLE) */
