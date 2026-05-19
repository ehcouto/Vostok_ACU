/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef EXTINTERRUPT_PRM_H_
#define EXTINTERRUPT_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables the single External Interrupt IRQ modules on Gpio Ports.
 *    @details- The macro must be Enabled to use that resource in the application.
 *
 *    @param  - ENABLED(1)  - Which Enables the Specified ExternalInterrupt line
 *              DISABLED(0) – Which Disables the Specified ExternalInterrupt line
 *
 *    @note   - Following are the Examples\n
 *              Any pin on PIN<x>, where <x> can be from 0 to 15
 *              Pins 0 and 1 are grouped together
 *              Pins 2 and 3 are grouped together
 *              Pins 4 to 15 are all grouped together
 *
 *              "#define EXTINTERRUPT_PIN<x>_FEATURE        ENABLED"   This Enables ExtInterrupt on these ports
 *              "#define EXTINTERRUPT_PIN<x>_FEATURE        DISABLED"  This Disables ExtInterrupt on these ports
 */
#define         EXTINTERRUPT_PIN0_1_FEATURE           DISABLED
#define         EXTINTERRUPT_PIN2_3_FEATURE           DISABLED
#define         EXTINTERRUPT_PIN4_15_FEATURE          ENABLED

#endif // EXTINTERRUPT_PRM_H_
