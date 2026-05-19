/**
 * @file
 * @brief       Public interface to the Statistics module.
 *
 * @details     Refer to the Statistics.c source file for more detailed information.
 *
 * @copyright   Copyright 2015-2019 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "Statistics_prm.h"

// -- Other Modules --
#include "JumpIf.h"                                 // to define the CYCLE_PHASE_STATEMENT_TYPE
#include "StatisticsCommon.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! The version of the cross-platform data structures in the statistics data.
#define STATISTICS_CROSS_PLATFORM_VERSION 1


// -- Public Enumerated Type Declarations --


// -- Public Type Declarations --

//! Measurements related to the total life of the appliance and appliance components.
typedef PACKED struct STATISTICS_LIFETIME_STRUCT
{
    uint32 Seconds;
    uint16 Cold_Reset_Count;
    uint16 Warm_Reset_Count;
    uint16 Cycles_Completed;                        // Cross-category variable

    #if (STATISTICS_SAVED_CYCLES > 0)
        uint16 Cycles_Started;
        uint32 Seconds_Running_Cycles;
        #if (STATISTICS_NUMBER_OF_LIFETIME_CYCLES > 0)
            uint16	Cycle_Started_Count[STATISTICS_NUMBER_OF_LIFETIME_CYCLES];
        #endif
    #endif

    #if (STATISTICS_CUSTOM_LIFETIME_ENABLE == ENABLED)
        PLATFORM_LIFETIME_TYPE Platform;
    #endif
} STATISTICS_LIFETIME_TYPE;

#if (STATISTICS_SAVED_CYCLES > 0)
//! Measurements related to the currently running (or most recently run) cycle of any appliance platform.
typedef PACKED struct STATISTICS_CYCLE_STRUCT
{
    uint16 Run_Seconds;
    uint16 Last_Fault_Eng_Code      : 4;
    uint16 Last_Fault_Sub_Code      : 4;
    uint16 Last_Fault_Code          : 4;
    uint16 Unused                   : 3;
    uint16 Cycle_Completed          : 1;

    CYCLE_PHASE_STATEMENT_TYPE Cps;
    uint8 Cold_Reset_Count          : 4;
    uint8 Warm_Reset_Count          : 4;

    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        uint16 Estimated_Seconds;
    #endif

    #if (APPLIANCE_MAX_COMPARTMENTS > 1)
        uint8 Compartment;
    #endif

	#if (STATISTICS_SAVED_CYCLE_REGULATIONS > 0)
        uint8 Regulations[STATISTICS_SAVED_CYCLE_REGULATIONS];
    #endif

    #if (STATISTICS_CUSTOM_CYCLE_ENABLE == ENABLED)
        PLATFORM_CYCLE_TYPE Platform;
    #endif
} STATISTICS_CYCLE_TYPE;
#endif      // STATISTICS_SAVED_CYCLE > 0

//! Common structure to hold fault and pre-fault data.
typedef PACKED struct STATISTICS_FAULT_STRUCT
{
    uint16 Fault_Eng_Code           : 4;
    uint16 Fault_Sub_Code           : 4;
    uint16 Fault_Code               : 4;
    uint16 Unused                   : 3;
    uint16 Hide_From_Hmi            : 1;
    uint16 Age;
    uint8 Fault_Id;
    uint8 Occurrences;
} STATISTICS_FAULT_TYPE;

//! Structure that holds all statistics data.
typedef PACKED struct STATISTICS_STRUCT
{
    STATISTICS_LIFETIME_TYPE Lifetime;
    #if (STATISTICS_SAVED_CYCLES > 0)
        STATISTICS_CYCLE_TYPE Cycle[STATISTICS_SAVED_CYCLES];
    #endif
    STATISTICS_FAULT_TYPE Fault[STATISTICS_SAVED_FAULTS];
    STATISTICS_FAULT_TYPE Pre_Fault[STATISTICS_SAVED_PRE_FAULTS];
    #if (STATISTICS_SAVED_LAST_CYCLES > 0)
        PLATFORM_LAST_CYCLE_TYPE Last_Cycle[STATISTICS_SAVED_LAST_CYCLES];
    #endif
    #if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
        PLATFORM_BLACK_BOX_TYPE Fault_Black_Box[STATISTICS_SAVED_FAULT_BLACK_BOX];
    #endif
    #if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
        PLATFORM_BLACK_BOX_TYPE Pre_Fault_Black_Box[STATISTICS_SAVED_PRE_FAULT_BLACK_BOX];
    #endif
} STATISTICS_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void Statistics__Initialize(void);
void Statistics__Background(void);
uint8 Statistics__ApplyFaultAgeCutoff(void);
void Statistics__ClearFaultData(void);
void Statistics__CycleStarted(uint8 compartment, uint8 cycle_id);
void Statistics__CycleStopped(BOOL_TYPE cycle_completed_normally, uint8 compartment, CYCLE_PHASE_STATEMENT_TYPE cps, uint16 cycle_seconds);
#if (STATISTICS_SAVED_CYCLES > 0)
    STATISTICS_CYCLE_TYPE* Statistics__GetCycleData(uint8 index);
#endif
uint8 Statistics__GetFaultCount(void);
STATISTICS_FAULT_TYPE* Statistics__GetFaultData(uint8 index);
#if (STATISTICS_SAVED_FAULT_BLACK_BOX > 0)
    PLATFORM_BLACK_BOX_TYPE* Statistics__GetFaultBlackBoxData(uint8 index);
#endif
#if (STATISTICS_SAVED_LAST_CYCLES > 0)
    PLATFORM_LAST_CYCLE_TYPE* Statistics__GetLastCycleData(uint8 index);
#endif
STATISTICS_LIFETIME_TYPE* Statistics__GetLifetimeData(void);
STATISTICS_FAULT_TYPE* Statistics__GetPreFaultData(uint8 index);
#if (STATISTICS_SAVED_PRE_FAULT_BLACK_BOX > 0)
    PLATFORM_BLACK_BOX_TYPE* Statistics__GetPreFaultBlackBoxData(uint8 index);
#endif
void Statistics__SaveFaultData(uint8 compartment, uint8 fault_id, uint8 code, uint8 sub_code, uint8 eng_code);
void Statistics__SavePreFaultData(uint8 compartment, uint8 fault_id, uint8 code, uint8 sub_code, uint8 eng_code);


#endif      /* STATISTICS_H_ */
