/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: API007Data_prm.h 1.1 2015/06/11 16:54:11EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2012-$Date: 2015/06/11 16:54:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007DATA_PRM_H_
#define API007DATA_PRM_H_

#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Definition to enabled/disabled bulk data transfer.
 * @param    - ENABLED / DISABLED
 * @note     - "API007DATA_BULK_DATA_FEATURE       ENABLED"    API007Data enabled for bulk data transfer
 *           - "API007DATA_BULK_DATA_FEATURE       DISABLED"   API007Data disabled for bulk data transfer
 */

#define API007DATA_BULK_DATA_FEATURE    ENABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Definition to Write data in the RAM memory for the received 32bit/24bit address and data.
 * @param    - ENABLED / DISABLED
 * @note     - "API007DATA_WRITE_RAMDATA_ONLY_REQUIRED       ENABLED"    For testing
 *           - "API007DATA_WRITE_RAMDATA_ONLY_REQUIRED       DISABLED"   For Production software disabled
 */

#define API007DATA_WRITE_RAMDATA_ONLY_REQUIRED   DISABLED

#endif // API007DATA_PRM_H_
