/**
 *  @file       SRTaskSequence.c
 *  @defgroup   CLASS_B
 *  @brief      Check for the correct execution sequence of Class B functions.
 *
 * @details     The SRTaskSequence module supports multiple function sequences.
 *
 *              The parameter files are used to define the function calls that make up each of
 *              the sequences. The function calls must occur in the same order that they are
 *              defined in the parameter files.
 *
 *              If a problem is encountered, the SRException contains data that can be used to
 *              diagnose the reason for the problem.
 *
 *              SREXCEPTION_SRTASKSEQ_INCOMPLETED occurs if the sequence is started or stopped in
 *              the middle of executing the sequence.
 *                  First Diagnostic Word: Hi byte = sequence id, Lo byte = sequence step number.
 *
 *              SREXCEPTION_SRTASKSEQ_WRONG_NUMBER occurs if a function was called out of order
 *              in the sequence.
 *                  First Diagnostic Word: Hi byte = sequence id, Lo byte = sequence step number.
 *                  Second Diagnostic Word: Hi byte = function id, Lo byte = expected function id.
 *
 *              SREXCEPTION_SRTASKSEQ_TIMEOUT occurs if the sequence took more than the alloted
 *              time to complete.
 *                  First Diagnostic Word: Hi byte = sequence id, Lo byte = sequence step number.
 *
 * @copyright   Copyright 2015-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SRTaskSequence.h"
#include "SRTaskSequence_prv.h"
#include "SRData.h"
#include "SRException.h"
#include <string.h>
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef SRTASKSEQUENCE_SKIP_RESET
#define SRTASKSEQUENCE_SKIP_RESET           DISABLED
#endif

#ifndef SRTASKSEQUENCE_PENDING_TIMEOUT
#define SRTASKSEQUENCE_PENDING_TIMEOUT      0
#endif


#if (CLASSB_FEATURE == ENABLED)
    #if (SYSTEM_ENABLE_TASKSEQUENCE != ENABLED)
        SW05_WARNING(4, "SYSTEM_ENABLE_TASKSEQUENCE is disabled", SW05_CLASSB)
    #endif

    #ifdef SRTASKSEQUENCE_SKIP_RESET
        #if (SRTASKSEQUENCE_SKIP_RESET != DISABLED)
            SW05_WARNING(5, "SRTASKSEQUENCE_SKIP_RESET is Enabled", SW05_CLASSB)
        #endif
    #endif
#endif

// There must be a "number of steps" defined for each sequence.
CT_ASSERT(ELEMENTS_IN_ARRAY(SRTaskSequcence_NumSteps) == SRTASKSEQUENCE_NUM_SEQUENCES);

// There must be a "list of steps" defined for each sequence.
CT_ASSERT(ELEMENTS_IN_ARRAY(SRTaskSequence_All_Steps) == SRTASKSEQUENCE_NUM_SEQUENCES);

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
static uint8 SRTaskSequence_Error;
static uint8 SRTaskSequence_Step[SRTASKSEQUENCE_NUM_SEQUENCES];
#if (SRTASKSEQUENCE_PENDING_TIMEOUT > 0)
static uint16 SRTaskSequence_Timeout[SRTASKSEQUENCE_NUM_SEQUENCES];

//This variable holds the module information only does not interact with the safety code
SRTASKSEQUENCE_STATISTICS_DATA_TYPE SRTaskSequence_Statistics;

#endif

static uint8 N_SRTaskSequence_Error;
static uint8 N_SRTaskSequence_Step[SRTASKSEQUENCE_NUM_SEQUENCES];
#if (SRTASKSEQUENCE_PENDING_TIMEOUT > 0)
static uint16 N_SRTaskSequence_Timeout[SRTASKSEQUENCE_NUM_SEQUENCES];
#endif
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    static void SetSRSequenceError(SREXCEPTION_EVENT_TYPE message, uint8 data);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize the SRTaskSequence module variables.
 */
void SRTaskSequence__Initialize(void)
{
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    uint8 index;

    for (index = 0; index < SRTASKSEQUENCE_NUM_SEQUENCES; index++)
    {
        SRData__UpdateByte(&SRTaskSequence_Step[index], &N_SRTaskSequence_Step[index], 0);
    #if (SRTASKSEQUENCE_PENDING_TIMEOUT > 0)
            SRData__UpdateShort(&SRTaskSequence_Timeout[index], &N_SRTaskSequence_Timeout[index], SRTASKSEQUENCE_PENDING_TIMEOUT);
            SRTaskSequence_Statistics.MaxElapsed[index] = 0;
    #endif
    }

    SRTaskSequence_Statistics.NumSequence = SRTASKSEQUENCE_NUM_SEQUENCES;

    SRData__UpdateByte(&SRTaskSequence_Error, &N_SRTaskSequence_Error,(uint8) FALSE);
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Check if a running sequence has been running for too long.
 *              If a sequence is running and if the sequence timer runs out, then the module enters an error state.
 */
void SRTaskSequence__Handler(void)
{
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    uint8 index;

    if (SRTaskSequence__IsError() == FALSE)
    {
        for (index = 0; index < SRTASKSEQUENCE_NUM_SEQUENCES; index++)
        {
            SRData__CheckByte(&SRTaskSequence_Step[index], &N_SRTaskSequence_Step[index]);
            #if (SRTASKSEQUENCE_PENDING_TIMEOUT > 0)
                SRData__CheckShort(&SRTaskSequence_Timeout[index], &N_SRTaskSequence_Timeout[index]);

                if ((SRTaskSequence_Step[index] > 0) &&
                    (SRTaskSequence_Step[index] < SRTaskSequcence_NumSteps[index]) )
                {
                    SRData__UpdateShort(&SRTaskSequence_Timeout[index], &N_SRTaskSequence_Timeout[index], SRTaskSequence_Timeout[index]-1);
                    if (SRTaskSequence_Timeout[index] == 0)
                    {
                        SetSRSequenceError(SREXCEPTION_SRTASKSEQ_TIMEOUT, index);
                    }

                    if (SRTaskSequence_Statistics.MaxElapsed[index] < (SRTASKSEQUENCE_PENDING_TIMEOUT - SRTaskSequence_Timeout[index]))
                    {
                        SRTaskSequence_Statistics.MaxElapsed[index] = SRTASKSEQUENCE_PENDING_TIMEOUT - SRTaskSequence_Timeout[index];
                    }
                }
            #endif
        }
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_SRTASKSEQUENCE);
    }
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Let SRTaskSequence know that a sequence of Class B function calls is starting.
 *              If this function is called while the sequence is running, it will generate an SRException.
 *
 *  @param      sequence_id - Sequence to start
 */
void SRTaskSequence__StartSequence(SRTASKSEQUENCE_SEQUENCES_TYPE sequence_id)
{
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    if ((sequence_id < SRTASKSEQUENCE_NUM_SEQUENCES) &&
        (SRTaskSequence__IsError() == FALSE))
    {
        SRData__CheckByte(&SRTaskSequence_Step[sequence_id], &N_SRTaskSequence_Step[sequence_id]);
        if ((SRTaskSequence_Step[sequence_id] == 0) ||
            (SRTaskSequence_Step[sequence_id] == (SRTaskSequcence_NumSteps[sequence_id] + 1)))
        {
            SRData__UpdateByte(&SRTaskSequence_Step[sequence_id], &N_SRTaskSequence_Step[sequence_id], 1);
            #if (SRTASKSEQUENCE_PENDING_TIMEOUT > 0)
                SRData__CheckShort(&SRTaskSequence_Timeout[sequence_id], &N_SRTaskSequence_Timeout[sequence_id]);
                SRData__UpdateShort(&SRTaskSequence_Timeout[sequence_id], &N_SRTaskSequence_Timeout[sequence_id], SRTASKSEQUENCE_PENDING_TIMEOUT);
            #endif
        }
        else
        {
            SetSRSequenceError(SREXCEPTION_SRTASKSEQ_INCOMPLETED, sequence_id);
        }
    }
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Let SRTaskSequence know that a sequence of Class B function calls has moved to the next function call.
 *              If the sequence step does not match, then module will generate an SRException.
 *
 *  @param      sequence_id - The sequence that is running.
 *  @param      function_id - The function ID in the sequence of Class B function calls.
 */
void SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_SEQUENCES_TYPE sequence_id, uint8 function_id)
{
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    if ((sequence_id < SRTASKSEQUENCE_NUM_SEQUENCES) &&
        (SRTaskSequence__IsError() == FALSE))
    {
        SRData__CheckByte(&SRTaskSequence_Step[sequence_id], &N_SRTaskSequence_Step[sequence_id]);
        uint8 expected_function_id = SRTaskSequence_All_Steps[sequence_id][SRTaskSequence_Step[sequence_id] - 1];
        if  (expected_function_id == function_id)
        {
            SRData__UpdateByte(&SRTaskSequence_Step[sequence_id], &N_SRTaskSequence_Step[sequence_id], SRTaskSequence_Step[sequence_id] + 1);
        }
        else
        {
            uint16 data = COMBINE_BYTES(sequence_id, SRTaskSequence_Step[sequence_id] - 1);
            #if (SRTASKSEQUENCE_SKIP_RESET == ENABLED)
                SRData__UpdateByte(&SRTaskSequence_Error, &N_SRTaskSequence_Error, FALSE);
                SRException__Queue(SREXCEPTION_PRODUCT_LEVEL_RECOVERABLE, data, SREXCEPTION_SRTASKSEQ_WRONG_NUMBER);
            #else
                SRData__UpdateByte(&SRTaskSequence_Error, &N_SRTaskSequence_Error, TRUE);
                SRException__Queue(SREXCEPTION_SRTASKSEQ_WRONG_NUMBER, data, COMBINE_BYTES(function_id, expected_function_id));
            #endif
        }
    }
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Let SRTaskSequence know that a sequence of Class B function calls has completed.
 *              If this function is not called after the last step in the sequence, it will generate an SRException.
 *
 *  @param      sequence_id - The sequence that is being completed.
 */
void SRTaskSequence__EndSequence(SRTASKSEQUENCE_SEQUENCES_TYPE sequence_id)
{
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    if ((sequence_id < SRTASKSEQUENCE_NUM_SEQUENCES) &&
        (SRTaskSequence__IsError() == FALSE))
    {
        SRData__CheckByte(&SRTaskSequence_Step[sequence_id], &N_SRTaskSequence_Step[sequence_id]);
        if (SRTaskSequence_Step[sequence_id] == (SRTaskSequcence_NumSteps[sequence_id] + 1))
        {
            #if (SRTASKSEQUENCE_PENDING_TIMEOUT > 0)
                SRData__CheckShort(&SRTaskSequence_Timeout[sequence_id], &N_SRTaskSequence_Timeout[sequence_id]);
                SRData__UpdateShort(&SRTaskSequence_Timeout[sequence_id], &N_SRTaskSequence_Timeout[sequence_id], 0);
            #endif
        }
        else
        {
            SetSRSequenceError(SREXCEPTION_SRTASKSEQ_INCOMPLETED, sequence_id);
        }
    }
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function returns the error state of the module
 *
 *  @return     TRUE if an error occurred, FALSE if working properly
 */
BOOL_TYPE SRTaskSequence__IsError(void)
{
    #if ((SRTASKSEQUENCE_SKIP_RESET == ENABLED) || (SYSTEM_ENABLE_TASKSEQUENCE == DISABLED))
        return FALSE;
    #else
        if (SR_CHECK_BYTE(SRTaskSequence_Error) != SRDATA_OK)
        {
            SR_SET_BYTE(SRTaskSequence_Error, TRUE);
        }
        return (BOOL_TYPE)(SRTaskSequence_Error);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API returns the static data usually used for debugging
 *
 * @return
 *  @retval SRTASKSEQUENCE_STATISTICS_DATA_TYPE *
 */
SRTASKSEQUENCE_STATISTICS_DATA_TYPE * SRTaskSequence__GetStatisticsData(void)
{
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        return &SRTaskSequence_Statistics;
    #else
        return (SRTASKSEQUENCE_STATISTICS_DATA_TYPE *)NULL;
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that raise and logs an Error state
 *
 *  @param      message - Message to log
 *  @param      sequence - The sequence that triggered the SRException message.
 */
static void SetSRSequenceError(SREXCEPTION_EVENT_TYPE message, uint8 sequence)
{
    uint16 data = COMBINE_BYTES(sequence, SRTaskSequence_Step[sequence] - 1);
    #if (SRTASKSEQUENCE_SKIP_RESET == ENABLED)
        SRData__UpdateByte(&SRTaskSequence_Error, &N_SRTaskSequence_Error, (uint8) FALSE);
        SRException__Queue(SREXCEPTION_PRODUCT_LEVEL_RECOVERABLE, data, message);
    #else
        SRData__UpdateByte(&SRTaskSequence_Error, &N_SRTaskSequence_Error, TRUE);
        SRException__Queue(message, data, 0);
    #endif
}
#endif      // #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)

