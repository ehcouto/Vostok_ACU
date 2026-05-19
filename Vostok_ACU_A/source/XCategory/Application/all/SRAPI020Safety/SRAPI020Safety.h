/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Public interface to the SRAPI020Safety module.
 *
 * @details     Refer to the SRAPI020Safety.c source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SRAPI020SAFETY_H_
#define SRAPI020SAFETY_H_


#if (API020SAFETY_FEATURE == ENABLED)


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SRAPI020Safety_prm.h"

// -- Other Modules --
#include "Reveal.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! A constant used to identify the API implemented by this module.
#define API020SAFETY_NUM            0x14            //20
#define API020SAFETY_TYPE           (80 + API020_PLATFORM)
#define API020SAFETY_VERSION        0x0
#define API020SAFETY_INSTANCES      0x1


// -- Public Type Declarations --

//! The structure used to configure a Receive Channel.
//! This structure is required in the SRAPI020Safety_prv.h file.
typedef struct API020_RX_CONFIG_STRUCT
{
    //! Pointer to the buffer that holds the received message data once it is validated by SRAPI020.
    void* Address;

    //! Pointer to the buffer that holds the inverted received message data once it is validated by SRAPI020.
    void* N_Address;

    //! The maximum time (in milliseconds) between received messages.
    //! If received messages are separated in time by more than this value, then the received data is considered invalid.
    uint16 Period;

    //! The Reveal feedback opCode used to communicate this received message.
    //! If the received message is not delivered with this opCode, then the received data is considered invalid.
    uint8 Op_Code;

    //! The Reveal node from which this received message is expected.
    //! If the received message is not sent by this node, then the received data is considered invalid.
    uint8 Node;

    //! The expected length of the received message.
    //! If the received message is not this length, then the received data is considered invalid.
    uint8 Length;
} API020_RX_CONFIG_TYPE;

//! The structure used to configure a Transmit Channel.
//! This structure is required in the SRAPI020Safety_prv.h file.
typedef struct API020_TX_CONFIG_STRUCT
{
    //! The maximum time (in milliseconds) between transmitted messages.
    //! A message is sent each time that SRAPI020Safety__SetTxValue() is called.
    //! If transmitted messages are separated in time by more than this value, then the message sequence broken and restarted.
    uint16 Period;

    //! The Reveal feedback opCode used to transmit this message.
    uint8 Op_Code;

    //! The Reveal node to which this transmitted message must be sent.
    uint8 Node;

    //! The length of this transmitted message.
    uint8 Length;
} API020_TX_CONFIG_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void SRAPI020Safety__Initialize(void);
void SRAPI020Safety__FeedbackParser(REVEAL_MSG_TYPE * fbk);
#if (API020_NUMBER_OF_RX_CHANNELS > 0)
    BOOL_TYPE SRAPI020Safety__GetRxValue(API020_RX_CHANNEL_INDEX_TYPE rx_channel, void* data);
    BOOL_TYPE SRAPI020Safety__GetRxValueAndUpdatedFlag(API020_RX_CHANNEL_INDEX_TYPE rx_channel, void* data, uint8* sequence, BOOL_TYPE* updated);
#endif
#if (API020_NUMBER_OF_TX_CHANNELS > 0)
    BOOL_TYPE SRAPI020Safety__SetTxValue(API020_TX_CHANNEL_INDEX_TYPE tx_channel, void* data);
#endif

#endif      // #if (API020SAFETY_FEATURE == ENABLED)

#endif      // SRAPI020SAFETY_H_
