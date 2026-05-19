/**
 *  @file       
 *
 *  @brief      This file consists of interrupt vector table which acts as an Jump table from IAP area to Application
 *
 *  @details    Release project excludes inclusion of this file during linking process
 *              This file is added only for the debug projects
 *
 *              Release Projects - Expects the actual IAP in the IAP location
 *              Debug Project - Does not need IAP. Mainly used by developer
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Micro.h"
#include "uc.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

#ifdef ENABLE_JUMP_TO_APPLICATION_WITHOUT_IAP_FIRMWARE

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;
extern uint32 __ICFEDIT_INTERRUPT_TABLE_start__;

__root static void MicroJumptoResetHandler(void);

//=====================================================================================================================
/**
 * @brief -     Allocates the Non maskable interrupt table for RX210 micro
 * @details -   This table is allocated at the 0xFFFFFF80 location
 *              __root forces to populate the table in the output file
 */
#pragma location = "Non_Maskable_Jump_interrupt_table"
__root const intvec_elem NON_MASKABLE_JUMP_INTERRUPT_TABLE[] =
{
{ .__ptr = __sfe( "CSTACK" ) },
    MicroJumptoResetHandler,                    //1 Initial Program Counter
};



#pragma optimize=low
__root static void MicroJumptoResetHandler(void)
{
    ((intfunc)*(((uint32 *)&__ICFEDIT_INTERRUPT_TABLE_start__ + RESET_HANDLER_INDEX)))();
}

#endif

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


