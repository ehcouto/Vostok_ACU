/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2023. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TTIMINGANALYSIS_H_
#define TTIMINGANALYSIS_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef TIMING_ANALYSIS
    #define TIMING_ANALYSIS             ENABLED
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    TIMING_ANALYSIS_INTERRUPT_START,
    TIMING_ANALYSIS_INTERRUPT_END
} TIMING_ANALYSIS_INTERRUPT_EVENTS_TYPE;

typedef struct TIMING_ANALYSIS_TASK_DATA_STRUCT
{
    // Timing Related
    uint32 Cpu_Entry_Max;
    uint32 Cpu_Entry_Min;
    uint32 Cpu_Max_Load;
} TIMING_ANALYSIS_TASK_DATA_TYPE;

typedef struct TIMING_ANALYSIS_INTERRUPT_DATA_STRUCT
{
    uint32 Elapsed_Time;
    uint32 Max_Elapsed_Time;
} TIMING_ANALYSIS_INTERRUPT_DATA_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#if (TIMING_ANALYSIS == ENABLED)
    void TimingAnalysis__Initialize(void);

    void TimingAnalysis__SampleTask(TASK_ID_TYPE task_index);
    void TimingAnalysis__SampleInterrupt(TIMING_ANALYSIS_INTERRUPT_EVENTS_TYPE event);

    TIMING_ANALYSIS_TASK_DATA_TYPE* TimingAnalysis__GetTaskInfo(TASK_ID_TYPE taskIndex);
    TIMING_ANALYSIS_INTERRUPT_DATA_TYPE* TimingAnalysis__GetInterruptInfo(void);

#else
    #define TimingAnalysis__Initialize()

    #define TimingAnalysis__SampleTask(x)
    #define TimingAnalysis__SampleInterrupt(x)

    #define TimingAnalysis__GetTaskInfo(x)                  NULL
    #define TimingAnalysis__GetInterruptInfo()              NULL
#endif

#endif // TTIMINGANALYSIS_H_


