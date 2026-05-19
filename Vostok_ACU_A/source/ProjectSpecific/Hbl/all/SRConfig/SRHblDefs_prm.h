/**
 *  @file       SRHblDefs_prm.h
 *
 *  @brief      Safety relevant definition parameters file
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  @defgroup CLASS_B
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRHBLDEFS_PRM_H_
#define SRHBLDEFS_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/**
 * HBL_LOAD_ENUM_TYPE items used in Safety relevant modules
 *
 * Populate with the list of Load reference items for all modules, Safety relevant and not.
 * It's not required to highlight them as SRHBL_LOAD_xy unless an existing module refer to it; it can be defined
 * together with the standard HBL_LOAD_xy item, assigning the same value to both.
 *
 * Define this macros as a list of enumeration items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 *
 */
// #define     SRHBL_LOAD_ID_LIST       \
        SRHBL_LOAD_FAN_RC                         = 10, \
        SRHBL_LOAD_FAN_FC                         = 11


/**
 * HBL_GI_ENUM_TYPE items used in Safety relevant modules
 *
 * Populate with the list of Generic Input reference items for all modules, Safety relevant and not.
 * It's not required to highlight them as SRHBL_HI_xy unless an existing module refer to it; it can be defined
 * together with the standard HBL_GI_xy item, assigning the same value to both.
 *
 * Define this macros as a list of enumeration items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 *
 */
// #define SRHBL_GI_LIST \
    SRHBL_GI_SENSOR_FC                        = 2, \
    SRHBL_GI_SENSOR_RC                        = 3,

/**
 * HBL_PRIORITY_TYPE items used in Safety relevant modules
 *
 * Populate with the list of Load priority reference items for Safety relevant modules.
 *
 * SRHBL_PRIORITY_CLASSB_LO must be defined with the minimum value for Safety modules; if unsure, use 128
 *
 * Define this macros as a list of enumeration items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 *
 */

#define SRHBL_LOAD_PRIORITY_LIST         \
    SRHBL_PRIORITY_CLASSB_LO      = 128


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRHBLDEFS_PRM_H_
