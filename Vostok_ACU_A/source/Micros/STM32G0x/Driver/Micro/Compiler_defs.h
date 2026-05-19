/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This file contains the compiler related macros like endian, tiny, near etc.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef COMPILER_DEFS_H_
#define COMPILER_DEFS_H_
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifdef __IAR_SYSTEMS_ICC__
    #include <intrinsics.h>
#endif
#include "CortexM0Plus_Macro.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define LITTLE_ENDIAN

#ifdef BIG_ENDIAN
    #undef BIG_ENDIAN
#endif

//! Define a macro for zero-page memory
#define TINY

//! Define a macro for non-zero-page memory
#define NEAR

//! define a macro for far address memory
#define FAR

//! define a macro for interrupt functions
#define INTERRUPT_DIRECTIVE


// ============================================================
// --------------------- MACRO OPERATIONS ---------------------
// ------------------------------------------------------------
#ifdef __IAR_SYSTEMS_ICC__
    //! Macro  sets the alignment of any valid type to 1. This means that:
    //! there is no padding inserted to align the packed object
    //! objects of packed type are read or written using unaligned accesses.
    //! The __packed qualifier applies to all members of a structure or union when it is declared using __packed.
    //! There is no padding between members, or at the end of the structure.
    //! All substructures of a packed structure must be declared using __packed.
    //! Integral subfields of an unpacked structure can be packed individually.
    #define PACKED  __packed

    //! Macro that enables interrupts by using the CLI assembly instruction
    #define ENABLE_INTERRUPTS()     __enable_interrupt()

    //! Macro that disables interrupts by using the SEI assembly instruction
    #define DISABLE_INTERRUPTS()    __disable_interrupt()

    //! Macro to reset the watchdog timer.
    #define SERVICE_WATCHDOG()      {WWDG.CR = WWDG_CR_T;  IWDG.KR = IWDG_RELOAD;}

    #define LOW_POWER_MODE()        //{MRCC.PWRCTRL.bit.lp_done = 0; MRCC.PWRCTRL.bit.lp = 1; MRCC.PWRCTRL.bit.lp = 1;MRCC.PWRCTRL.bit.lp = 0;MRCC.PWRCTRL.bit.lp = 1;}

    //!Macro enables the deep sleep mode
    #define DEEP_SLEEP_MODE()
                                                                            // See section 2.7.4 STR750 Reference Manual
    //! Macro to reset the system by executing an illegal instruction.
    //! The nop that follows the illegal instruction is to prevent the optimizing
    //! compiler from accidentally doing a SYSTEM_RESET() because it gets grouped
    //! with other code.
    #define SYSTEM_RESET()          {SCB.AIRCR = AIRC_VECTKEY_MASK | SCB_AIRCR_SYSRESETREQ_MSK;}

    #define RESET_STACK_POINTER()   __MSR_MSP((uint32)__sfe( "CSTACK" ))

    //! Macro to spend CPU cycles by using the NOP assembly instruction

    #define NOP()                   asm(" nop ")

    //! Macro to allocate a variable into NO INIT section were variables are not initialized during during micro initialization
    #define NO_INIT   __no_init
#else
    #define PACKED

    //! Macro that enables interrupts by using the CLI assembly instruction
    #define ENABLE_INTERRUPTS()

    //! Macro that disables interrupts by using the SEI assembly instruction
    #define DISABLE_INTERRUPTS()

    //! Macro to reset the watchdog timer.
    #define SERVICE_WATCHDOG()

    #define LOW_POWER_MODE()

    #define DEEP_SLEEP_MODE()

    //! Macro to reset the system
    #define SYSTEM_RESET()

    #define RESET_STACK_POINTER()

    //! Macro to spend CPU cycles by using the NOP assembly instruction
    #define NOP()

    //! Macro to allocate a variable into NO INIT section were variables are not initialized during during micro initialization
    #define NO_INIT
    #define __no_init

    #define __UNSIGNED_CHAR_MAX__   (0xFF)

    #define __SIGNED_CHAR_MAX__    (127)
    #define __SIGNED_CHAR_MIN__    (-128)

    #define __UNSIGNED_SHORT_MAX__ (0xFFFF)

    #define __SIGNED_SHORT_MAX__    (32767)
    #define __SIGNED_SHORT_MIN__    (-32768)

    #define __UNSIGNED_LONG_MAX__   (0xFFFFFFFF)
    #define __SIGNED_LONG_MIN__     (0x80000000)
    #define __SIGNED_LONG_MAX__     (0x7FFFFFFF)
#endif

#endif // COMPILER_DEFS_H_
