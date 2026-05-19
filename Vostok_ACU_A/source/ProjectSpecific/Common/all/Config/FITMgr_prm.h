/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: FITMgr_prm.h 1.2 2015/01/12 12:09:47EST Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2015/01/12 12:09:47EST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FITMGR_PRM_H_
#define FITMGR_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * @brief   - List the FIT tables in the below enum
 * @details - Each label in the FITMGR_TABLE_TYPE must map with the FIT_TABLE_LIST table
 *
 * @param -
 *              typedef enum
 *              {
 *                  FIT_IAP_TABLE,
 *                  FIT_APPLICATION_TABLE,
 *              }FITMGR_TABLE_TYPE;
 */
typedef enum
{
    FIT_APPLICATION_TABLE_INDEX,
}FITMGR_TABLE_TYPE;

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Define the number labels defined in the FITMGR_TABLE_TYPE
 * @details This number indicates how many FIT tables the FITMgr has to handle.
 * @param   The number here shall match the number of elements in the FITMGR_TABLE_TYPE
 */
#define FITMGR_NUM_FIT_TABLE       1

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Define the FIT table
 * @details Each label in the FITMGR_TABLE_TYPE must map with the FIT_TABLE_LIST table
 *
 * @param -
 *
 *          #define FITMGR_TABLE_LIST          {   \
 *                                                 (uint32 *)FIT_IAP_BLOCK,                                \
 *                                                 (uint32 *)FIT_APPLICATION_TABLE                         \
 *                                             }
 */

#define FITMGR_TABLE_LIST      {   \
                                   (uint32 *)FIT_APPLICATION_TABLE                         \
                               }

#endif // FITMGR_PRM_H_
