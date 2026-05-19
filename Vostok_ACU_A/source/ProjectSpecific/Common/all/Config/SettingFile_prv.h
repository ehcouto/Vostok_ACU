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
#ifndef SETTINGFILE_PRV_H_
#define SETTINGFILE_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

/**
 * Allows the use of an ACU setting file that is linked as a binary file with the firmware.
 * Possible values are ENABLED or DISABLED
 */
#define USE_ACU_HARDCODED_SETTING_FILE  DISABLED

/**
 * Allows the use of an HMI setting file that is linked as a binary file with the firmware.
 * Possible values are ENABLED or DISABLED
 */
#define USE_HMI_HARDCODED_SETTING_FILE  DISABLED

#endif // SETTINGFILE_PRV_H_
