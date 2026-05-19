/**
 *  @file
 *    @brief    This module implements the standard Data Flash API (Application Programming Interface)
 *              for STM32F0x
 *    @details  As STM32G0 does not have seperate DATA memory so we use FLASH memory as DATA memory
 *              This module referes the Flash.h module
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2021-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef FLASH_DEF_H_
#define FLASH_DEF_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * It defines the type of the address variable
 */
typedef uint32 FLASH_ADDRESS_TYPE;

/**
 * It defines the type of the size variable
 */
typedef uint16 FLASH_SIZE_TYPE;


#define FLASH_ALIGNMENT             8       //64bit aligned

#define FLASH_PAGE_FORMAT_SIZE      0x00000800U

#endif // FLASH_DEF_H_


