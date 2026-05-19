/**
 *  @file       
 *
 *  @brief      Basic description of file contents
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
#ifndef MICROEXT_PRM_H_
#define MICROEXT_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 * @brief Application Interrupt Vector Table start address must be mentioned in the linker (*.icf) file as below -
 * For Example if the Start address is 0x08002000 then -
 *
 *          define symbol __ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__ = 0x08002000;
 *
 *          place at address mem:__ICFEDIT_APPLICATION_INTERRUPT_TABLE_start__ { readonly section Application_interrupt_table };
 *
 */

#endif // MICROEXT_PRM_H_
