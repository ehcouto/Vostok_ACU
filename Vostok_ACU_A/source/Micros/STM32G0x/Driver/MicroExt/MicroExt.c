/**
 *  @file       
 *
 *  @brief      Implementation of the MicroExt API's
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MicroExt.h"
#include "uc.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
extern uint32  __ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__;

#define RELOCATION_ADDRESS            &__ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__       // This symbol comes from the linker file

#define FLASH_EMPTY_VALUE                   0xFFFFFFFF

typedef void( *INTFUNCTION_TYPE )( void );

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MicroExt and its variables
 *
 */
void MicroExt__Initialize(void)
{
    Micro__HardwareInit();
    Micro__WatchdogInit();
    Micro__SetNormalSpeedMode();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API resets, disables all the peripheral of the micro-controller
 *  @details    1. Reset and Disables all the peripheral of the mirco
 *              2. Set the default system clock
 *              3. Disable all the interrupt sources
 */
void MicroExt__ResetAll(void)
{
    Micro__HardwareInit();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API Switches from IAP mode to application mode
 *  @details    Watchdog is refreshed before the jump
 *              The Jump location value is verified
 *
 * @return
 *  @retval - TRUE(1)   -   if its able to jump to the application
 *  @retval - FALSE(0)  -   failed to launch the application, it may be blank
 */
BOOL_TYPE MicroExt__JumptoApplication(void)
{
    BOOL_TYPE response;
    INTFUNCTION_TYPE appl_start;

    appl_start = NULL;

    response = FALSE;

    //Refresh the Watchdog
    Micro__ServiceWatchdog();

    if (((__ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__) != FLASH_EMPTY_VALUE) &&
        ((*(&__ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__ + 1)) != FLASH_EMPTY_VALUE))
    {
        response = TRUE;

        DISABLE_INTERRUPTS();

        // Initialize user application's Stack Pointer
        __set_MSP((uint32)__ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__);

        SCB.VTOR = (uint32)RELOCATION_ADDRESS;

        appl_start =(INTFUNCTION_TYPE)(*(&__ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__ + 1));

        // Jump to application
        appl_start();

        NOP();
    }

    return(response);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


