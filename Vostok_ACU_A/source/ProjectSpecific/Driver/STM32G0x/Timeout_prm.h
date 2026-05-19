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
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TIMEOUT_PRM_H_
#define TIMEOUT_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * MILLISECOND TIMER LIST
 *
 * To add MS Timers, just add timer names to the list. (up to 255 timers).
 * MAKE SURE you adjust the #define MS_NUM to the appropriated number of
 * items in MS_TIMER_LABEL list.
 */
typedef enum
{
    //Add Ms Timeouts here
    MST_IAP_MODE_TIMEOUT,
    MST_IAP_SCHEDULER_TIMEOUT,
} TIMEOUT_LABEL;

#define NUM_OF_TIMEOUT_LABEL     2

#endif // TIMEOUT_PRM_H_
