/**
 * @brief       Source file for the SeggerRTT module.
 *
 * @details     SEGGER provides a tool called RTT (Real Time Transfer).  This module is meant to be a wrapper to configure the
 *              SEGGER RTT library files into the Nucleus software architecture.
 *
 * @copyright Copyright (c) 2022 WhirlpoolCorporation
 */
//-------------------------------------------------------------------------------------------------------------------

//  --- Include Files -------------------------------------------------------------------------------------------------

#include "SystemConfig.h"

#include "SeggerRTT.h"
#include "Micro.h"

#ifdef SEGGER_RTT_MESSAGES_ENABLED

//  --- Private Declarations ------------------------------------------------------------------------------------------

static struct
{
    BOOL_TYPE Up_Buffer_Configured[SEGGER_RTT_MAX_NUM_UP_BUFFERS];
} SeggerRTT;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================

/*
 *  @brief  Used to configure the SEGGER RTT module to handle messages.
 *          This function should be used instead of the SEGGER_RTT_ConfigUpBuffer() function to prevent the same buffer index being
 *          configured by multiple modules.
 *
 *  @param  buffer  the up buffer to configure, must be less than or equal to SEGGER_RTT_MAX_NUM_UP_BUFFERS
 */
void SeggerRTT__ConfigUpBuffer(uint8 buffer)
{
//    if (( buffer < SEGGER_RTT_MAX_NUM_UP_BUFFERS ) &&
//        ( SeggerRTT.Up_Buffer_Configured[buffer] != TRUE ))
//    {
//        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
//        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
//
//        SeggerRTT.Up_Buffer_Configured[buffer] = TRUE;
//        /* During development, it was found that the micro would reset if this function was called multiple times.
//         * To prevent the micro reset, the flag is used to allow the config function to only be called once. */
//        SEGGER_RTT_ConfigUpBuffer(buffer, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
//
//        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
//    }
}

void SeggerRTT__ConfigureUpBuffer0(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
}
//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

#endif
