/**
 *  @file       SRIsrMonitor.c
 *  @defgroup   CLASS_B
 *  @brief      ISR Monitor module
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRIsrMonitor.h"
#include "SRPowerMgr.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#if (SYSTEM_ENABLE_ISRMONITOR != ENABLED)
    SW05_WARNING(11, "SYSTEM_ENABLE_ISRMONITOR is disabled", SW05_CLASSB)
#endif

#ifndef SRISRMONITOR_MAX
    #define SRISRMONITOR_MAX			0
#endif

#ifndef SRISRMONITOR_MIN
    #define SRISRMONITOR_MIN			0
#endif

#ifndef SRISRMONITOR_MAX_ATTEMPTS
    #define SRISRMONITOR_MAX_ATTEMPTS	0
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (SRISRMONITOR_MAX > 0)
    #if (SRISRMONITOR_MIN > SRISRMONITOR_MAX)
        #error "SRISRMONITOR_MIN must be lower than SRISRMONITOR_MAX!"
    #endif

    #ifndef SRISRMONITOR_LOWSPEED_MAX
        #define SRISRMONITOR_LOWSPEED_MAX       SRISRMONITOR_MAX
    #endif

    #ifndef SRISRMONITOR_LOWSPEED_MIN
        #define SRISRMONITOR_LOWSPEED_MIN       SRISRMONITOR_MIN
    #endif

    #if (SRPOWERMGR_LOWPOWER_MODE == 3) // SRPOWERMGR_LOW_SPEED_MODE
        #if (SRISRMONITOR_LOWSPEED_MIN > SRISRMONITOR_LOWSPEED_MAX)
            #error "SRISRMONITOR_LOWSPEED_MIN must be lower than SRISRMONITOR_LOWSPEED_MAX!"
        #endif
    #endif

    #include "SRException.h"
    #include "SRMicro.h"
    #include "SRData.h"
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        #include "SRTaskSequence.h"
    #endif

    #if ((SRISRMONITOR_MAX < 255) && (SRISRMONITOR_LOWSPEED_MAX < 255))
        typedef uint8 SRISRMONITOR_CNTR_TYPE;

        #define SRDATA_STAT_CNTR_MAX						0xFF
        #define SRDATA__ISRCHECK(var, n_var)				SRData__CheckByte((var), (n_var))
        #define SRDATA__ISRUPDATE(var, n_var, val)			SRData__UpdateByte((var), (n_var), (val))

    #elif ((SRISRMONITOR_MAX < 65535) && (SRISRMONITOR_LOWSPEED_MAX < 65535))
        typedef uint16 SRISRMONITOR_CNTR_TYPE;

        #define SRDATA_STAT_CNTR_MAX						0xFFFF
        #define SRDATA__ISRCHECK(var, n_var)				SRData__CheckShort((var), (n_var))
        #define SRDATA__ISRUPDATE(var, n_var, val)			SRData__UpdateShort((var), (n_var), (val))

    #else
        typedef uint32 SRISRMONITOR_CNTR_TYPE;

        #define SRDATA_STAT_CNTR_MAX						0xFFFFFFFF
        #define SRDATA__ISRCHECK(var, n_var)				SRData__CheckLong((var), (n_var))
        #define SRDATA__ISRUPDATE(var, n_var, val)			SRData__UpdateLong((var), (n_var), (val))
    #endif

    static SRISRMONITOR_CNTR_TYPE Isr_Max;
    static SRISRMONITOR_CNTR_TYPE Isr_Min;
    static SRISRMONITOR_CNTR_TYPE Isr_Enter_Cntr;
    static SRISRMONITOR_CNTR_TYPE Isr_Exit_Cntr;
    static uint8                  Isr_LowSpeed_Mode;
    static uint8                  Isr_BootUp_Skip_Count;

    static SRISRMONITOR_CNTR_TYPE N_Isr_Max;
    static SRISRMONITOR_CNTR_TYPE N_Isr_Min;
    static SRISRMONITOR_CNTR_TYPE N_Isr_Enter_Cntr;
    static SRISRMONITOR_CNTR_TYPE N_Isr_Exit_Cntr;
    static uint8                  N_Isr_LowSpeed_Mode;
    static uint8                  N_Isr_BootUp_Skip_Count;

    //This variable holds the module information only does not interact with the safety code
    static SRISR_STATISTICS_DATA_TYPE Isr_Statistics;
    static BOOL_TYPE Isr_Clear_Stats;

    #if (SRISRMONITOR_MAX_ATTEMPTS > 0)

        #if (SRISRMONITOR_MAX_ATTEMPTS < 255)
            typedef uint8 SRISRMONITOR_ATTEMPT_TYPE;

            #define SRDATA_STAT_ATTEMPT_MAX						0xFF
            #define SRDATA__ATTEMPT_CHECK(var, n_var)			SRData__CheckByte((var), (n_var))
            #define SRDATA__ATTEMPT_UPDATE(var, n_var, val)		SRData__UpdateByte((var), (n_var), (val))
        #elif (SRISRMONITOR_MAX_ATTEMPTS < 65535)
            typedef uint16 SRISRMONITOR_ATTEMPT_TYPE;

            #define SRDATA_STAT_ATTEMPT_MAX						0xFFFF
            #define SRDATA__ATTEMPT_CHECK(var, n_var)			SRData__CheckShort((var), (n_var))
            #define SRDATA__ATTEMPT_UPDATE(var, n_var, val)		SRData__UpdateShort((var), (n_var), (val))
        #else
            typedef uint32 SRISRMONITOR_ATTEMPT_TYPE;

            #define SRDATA_STAT_ATTEMPT_MAX						0xFFFFFFFF
            #define SRDATA__ATTEMPT_CHECK(var, n_var)			SRData__CheckLong((var), (n_var))
            #define SRDATA__ATTEMPT_UPDATE(var, n_var, val)		SRData__UpdateLong((var), (n_var), (val))
        #endif

        static SRISRMONITOR_ATTEMPT_TYPE Isr_Attempt_Cntr;

        static SRISRMONITOR_ATTEMPT_TYPE N_Isr_Attempt_Cntr;
    #endif
#endif


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void reset_stat_data(void);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRIsrMonitor and its variables
 *
 */
void SRIsrMonitor__Initialize(void)
{
#if (SRISRMONITOR_MAX > 0)
	// Reset Enter and Exit counters
    SRDATA__ISRUPDATE(&Isr_Enter_Cntr, &N_Isr_Enter_Cntr, 0);
    SRDATA__ISRUPDATE(&Isr_Exit_Cntr, &N_Isr_Exit_Cntr, 0);

    #if (SRISRMONITOR_MAX_ATTEMPTS > 0)
        // Reset Attempts counters
        SRDATA__ATTEMPT_UPDATE(&Isr_Attempt_Cntr, &N_Isr_Attempt_Cntr, 0);
    #endif

    // Start using Normal Speed mode parameters
    SRDATA__ISRUPDATE(&Isr_Max, &N_Isr_Max, SRISRMONITOR_MAX);
    SRDATA__ISRUPDATE(&Isr_Min, &N_Isr_Min, SRISRMONITOR_MIN);
    SRData__UpdateByte(&Isr_LowSpeed_Mode, &N_Isr_LowSpeed_Mode, (uint8) FALSE);

    Isr_Clear_Stats = FALSE;
    reset_stat_data();
    SRData__UpdateByte(&Isr_BootUp_Skip_Count, &N_Isr_BootUp_Skip_Count, 10);

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_SRISRMONITOR);
    #endif

#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to be called at beginning of every ISR function
 *
 *  @detail		This function checks IsrEnterCntr data integrity and Overrun check
 *  			(IsrEnterCntr higher than expected)	before update it.
 *  			It can be called also when nested interrupt are used.
 *
 */
void SRIsrMonitor__EnterIsr(void)
{
#if (SRISRMONITOR_MAX > 0)
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    // Data integrity
    SRDATA__ISRCHECK(&Isr_Enter_Cntr, &N_Isr_Enter_Cntr);
    SRDATA__ISRCHECK(&Isr_Max, &N_Isr_Max);
	// Catch an ISR overrun
    if (Isr_Enter_Cntr >= Isr_Max)
    {
        SRException__Queue(SREXCEPTION_SRISR_OVERRUN, Isr_Enter_Cntr, 0);
    }
    // Update ISR Enter counter
    SRDATA__ISRUPDATE(&Isr_Enter_Cntr, &N_Isr_Enter_Cntr, Isr_Enter_Cntr+1);
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    //------- End ISR Protected Code ----------------------------------------------------------------------------------
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to be called at end of every ISR function
 *
 *  @detail		This function checks IsrExitCntr data integrity before update it.
 *  			It can be called also when nested interrupt are used.
 *
 */
void SRIsrMonitor__ExitIsr(void)
{
#if (SRISRMONITOR_MAX > 0)
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    // Data integrity
    SRDATA__ISRCHECK(&Isr_Exit_Cntr, &N_Isr_Exit_Cntr);
    // Update ISR Exit counter
    SRDATA__ISRUPDATE(&Isr_Exit_Cntr, &N_Isr_Exit_Cntr, Isr_Exit_Cntr+1);
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    //------- End ISR Protected Code ----------------------------------------------------------------------------------
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      IsrMonitor Hanlder to check proper ISR execution
 *
 *  @detail		This function checks all data integrity, IsrEnterCntr/IsrExitCntr match
 *  			and minimum number of ISRs occurred.
 *  			If ISR number is lower, increases number attempts up to maximum reached.
 *  			If everything is OK, resets all counters.
 *				This function must be called out of any ISR, otherwise IsrEnterCntr and IsrExitCntr will not match.
 *				This function disables interrupts to ensure its proper execution.
 *
 */
void SRIsrMonitor__Handler(void)
{
    #if (SRISRMONITOR_MAX > 0)
        SRData__CheckByte(&Isr_BootUp_Skip_Count, &N_Isr_BootUp_Skip_Count);

        if (Isr_BootUp_Skip_Count == 0)
        {
            //Clear the stats data here
            if (Isr_Clear_Stats == TRUE)
            {
                Isr_Clear_Stats = FALSE;
                reset_stat_data();
            }

            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

            //------- ISR Protected Code --------------------------------------------------------------------------------------
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            // Data integrity check
            SRDATA__ISRCHECK(&Isr_Enter_Cntr, &N_Isr_Enter_Cntr);
            SRDATA__ISRCHECK(&Isr_Exit_Cntr, &N_Isr_Exit_Cntr);
            SRDATA__ISRCHECK(&Isr_Max, &N_Isr_Max);
            SRDATA__ISRCHECK(&Isr_Min, &N_Isr_Min);
            SRData__CheckByte(&Isr_LowSpeed_Mode, &N_Isr_LowSpeed_Mode);
            #if (SRISRMONITOR_MAX_ATTEMPTS > 0)
                SRDATA__ATTEMPT_CHECK(&Isr_Attempt_Cntr, &N_Isr_Attempt_Cntr);
            #endif

            #if (SRPOWERMGR_LOWPOWER_MODE == 3) // SRPOWERMGR_LOW_SPEED_MODE
                // If Low Speed mode has been engaged, update Max and Min values with Low speed ones
                if ((Isr_LowSpeed_Mode == (uint8) FALSE) &&
                    (SRPowerMgr__GetMode() == SRPOWERMGR_LOW_SPEED_MODE))
                {
                    SRData__UpdateByte(&Isr_LowSpeed_Mode, &N_Isr_LowSpeed_Mode, (uint8) TRUE);
                    SRDATA__ISRUPDATE(&Isr_Max, &N_Isr_Max, SRISRMONITOR_LOWSPEED_MAX);
                    SRDATA__ISRUPDATE(&Isr_Min, &N_Isr_Min, SRISRMONITOR_LOWSPEED_MIN);
                }
            #endif

            // Catch an Enter/Exit mismatch - at this point they must be equal!
            if (Isr_Enter_Cntr != Isr_Exit_Cntr)
            {
                SRException__Queue(SREXCEPTION_SRISR_SEQUENCE_MISMATCH, Isr_Enter_Cntr, Isr_Exit_Cntr);
            }
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            //------- End ISR Protected Code ----------------------------------------------------------------------------------

            // Catch interrupts less than lower threshold set
            if (Isr_Enter_Cntr < Isr_Min)
            {
                //------- ISR Protected Code --------------------------------------------------------------------------------------
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                // Handle attempts - do exception only if higher than SRISRMONITOR_MAX_ATTEMPTS
                if(Isr_Attempt_Cntr < SRISRMONITOR_MAX_ATTEMPTS)
                {
                    SRDATA__ATTEMPT_UPDATE(&Isr_Attempt_Cntr, &N_Isr_Attempt_Cntr, Isr_Attempt_Cntr+1);
                }
                else
                {
                    SRException__Queue(SREXCEPTION_SRISR_TIMEOUT, Isr_Enter_Cntr, 0);
                }
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                //------- End ISR Protected Code ----------------------------------------------------------------------------------
            }
            else
            {
                #if (SRISRMONITOR_MAX_ATTEMPTS > 0)
                    if (Isr_Attempt_Cntr > Isr_Statistics.Isr_Attempt_Max_Cntr)
                    {
                        Isr_Statistics.Isr_Attempt_Max_Cntr = Isr_Attempt_Cntr;
                    }

                    if (Isr_Attempt_Cntr < Isr_Statistics.Isr_Attempt_Min_Cntr)
                    {
                        Isr_Statistics.Isr_Attempt_Min_Cntr = Isr_Attempt_Cntr;
                    }

                    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

                    // Everything is going OK - reset attempt counter
                    SRDATA__ATTEMPT_UPDATE(&Isr_Attempt_Cntr, &N_Isr_Attempt_Cntr, 0);

                    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                #endif
            }

            // Update statistics
            if (Isr_Enter_Cntr > Isr_Statistics.Isr_Max_Cntr)
            {
                Isr_Statistics.Isr_Max_Cntr = Isr_Enter_Cntr;
            }

            if (Isr_Enter_Cntr < Isr_Statistics.Isr_Min_Cntr)
            {
                Isr_Statistics.Isr_Min_Cntr = Isr_Enter_Cntr;
            }


            //------- ISR Protected Code --------------------------------------------------------------------------------------
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            // Everything is going OK - reset all counters for next check
            SRDATA__ISRUPDATE(&Isr_Enter_Cntr, &N_Isr_Enter_Cntr, 0);
            SRDATA__ISRUPDATE(&Isr_Exit_Cntr, &N_Isr_Exit_Cntr, 0);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            //------- End ISR Protected Code ----------------------------------------------------------------------------------
        }
        else
        {
            SRData__UpdateByte(&Isr_BootUp_Skip_Count, &N_Isr_BootUp_Skip_Count, (Isr_BootUp_Skip_Count-1));

            SRDATA__ISRUPDATE(&Isr_Enter_Cntr, &N_Isr_Enter_Cntr, 0);
            SRDATA__ISRUPDATE(&Isr_Exit_Cntr, &N_Isr_Exit_Cntr, 0);
        }

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
           SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_MAIN_HANDLER, SRTASKSEQUENCE_MAIN_ISRMONITOR);
        #endif

           //-----------------------------------------------------------------------------------------------------------------
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API returns the static data usually used for debugging
 *
 * @return
 *  @retval SRISR_STATISTICS_DATA_TYPE *
 */
SRISR_STATISTICS_DATA_TYPE * SRIsrMonitor__GetStatisticsData(void)
{
    #if (SRISRMONITOR_MAX > 0)
        Isr_Clear_Stats = TRUE;
        return &Isr_Statistics;
    #else
        return NULL;
    #endif
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
static void reset_stat_data(void)
{
    // Reset data statistics
    Isr_Statistics.Isr_Min_Cntr = SRDATA_STAT_CNTR_MAX;
    Isr_Statistics.Isr_Max_Cntr = 0;
    Isr_Statistics.Isr_Attempt_Min_Cntr = SRDATA_STAT_ATTEMPT_MAX;
    Isr_Statistics.Isr_Attempt_Max_Cntr = 0;
}
