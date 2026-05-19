/**
 *  @file       
 *
 *  @brief    This file contains configurable parameter for Flash module

 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2021-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FLASH_PRM_H_
#define FLASH_PRM_H_
#include "C_Extensions.h"

//=====================================================================================================================
/**
 *    @brief  - Defines the lower and upper limits for flash memory.
 *    @details- The application code also sits in the main program area so before configuring the
 *              Data FLASH start address should take care of the application code size.
 *
 *              Note Refer data sheet

 *    @note   - Following are the Examples\n
 *              "#define FLASH_LOW_LIMIT     0x0800F000"
 *              "#define FLASH_HIGH_LIMIT    0x0807FFFF"
 */
extern uint32 __SetingFile_Start;
extern uint32 __SetingFile_End;

#define FLASH_LOW_LIMIT     ((uint32)(&__SetingFile_Start))        // Flash lower boundary.
#define FLASH_HIGH_LIMIT    ((uint32)(&__SetingFile_End))          // Flash upper boundary.

#endif // FLASH_PRM_H_


