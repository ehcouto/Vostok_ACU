/**
 *  @file       
 *
 *  @brief      Private parameters of FeedbackLLI module
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: FeedbackLowInput_prv.h 1.2 2015/07/14 17:37:30EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:30EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FEEDBACKLLI_PRV_H_
#define FEEDBACKLLI_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
/**
 *    @brief  - Defines number of samples read to update a new value
 *    @details- Each sample is read at every ZeroCross half wave (when ZeroCross is present)
 */
#define FEEDBACKLLI_NUM_SAMPLES        16


/**
 *    @brief  - Defines number of level value threshold to detect an HIGH or LOW value when ZeroCross is not present
 */
#define FEEDBACKLLI_NOZC_LEVEL_THRES   14


/**
 *    @brief  - Defines number of level value threshold to detect a frequency (PHASE) value when ZeroCross is not present
 */
#define FEEDBACKLLI_NOZC_FREQ_THRES    17


/**
 *    @brief  - Defines number of level value threshold to detect an HIGH or LOW value when ZeroCross is present
 */
#define FEEDBACKLLI_ZC_LEVEL_THRES     14


/**
 *    @brief  - Defines number of level value threshold to detect a frequency (PHASE/OPP_PHASE) value when ZeroCross is present
 */
#define FEEDBACKLLI_ZC_FREQ_THRES      14

/**
 *    @brief  - Function that checks if Feedback peak routine should be executed or not
 *    @details- This function must return a BOOL_TYPE value (TRUE or FALSE)
 */
#define FEEDBACKLLI_PEAK_ISZC_CHECK()      TRUE

/**
 *    @brief  - Function that checks if Feedback asynchronous routine should be executed or not
 *    @details- This function must return a BOOL_TYPE value (TRUE or FALSE)
 */
#define FEEDBACKLLI_ASYNCH_ISNOZC_CHECK()  FALSE

#endif // FEEDBACKLLI_PRV_H_
