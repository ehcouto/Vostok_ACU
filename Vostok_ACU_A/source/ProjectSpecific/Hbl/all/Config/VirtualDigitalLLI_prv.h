/**
 *  @file       
 *
 *  @brief      
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VIRTUALDIGITALLLI_PRV_H_
#define VIRTUALDIGITALLLI_PRV_H_


//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

/**
 * @brief   Defines the project specific function used to get the value of a digital input based on an index to a local buffer
 * @param   buffer_index: index to a local buffer that contains the value of a digital input
 * @return  Returns a byte, where only bit 0 is used to indicate the value of the digital input.
 */
#define GET_VIRTUAL_DIGITAL_LLI(buffer_index) 0 // This LLI not currently used; Will need to define this macro if this LLI is used

#endif // VIRTUALDIGITALLLI_PRV_H_
