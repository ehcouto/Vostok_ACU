/**
 *  @file       SRTaskSequence.h
 *  @defgroup   CLASS_B
 *  @brief      Check for the correct execution sequence - Header module
 *
 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRTASKSEQUENCE_H_
#define SRTASKSEQUENCE_H_

#include "SRTaskSequence_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    uint8 NumSequence;
    uint16 MaxElapsed[SRTASKSEQUENCE_NUM_SEQUENCES];
}SRTASKSEQUENCE_STATISTICS_DATA_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRTaskSequence__Initialize(void);
void SRTaskSequence__Handler(void);
void SRTaskSequence__StartSequence(SRTASKSEQUENCE_SEQUENCES_TYPE sequence_id);
void SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_SEQUENCES_TYPE sequence_id, uint8 function_id);
void SRTaskSequence__EndSequence(SRTASKSEQUENCE_SEQUENCES_TYPE sequence_id);
BOOL_TYPE SRTaskSequence__IsError(void);

SRTASKSEQUENCE_STATISTICS_DATA_TYPE *SRTaskSequence__GetStatisticsData(void);

#endif // SRTASKSEQUENCE_H_


