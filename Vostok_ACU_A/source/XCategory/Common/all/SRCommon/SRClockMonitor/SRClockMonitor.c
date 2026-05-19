/**
 *  @file
 *
 *  @defgroup CLASS_B
 *
 *  @brief      This module compares an internal clock source given by the 1ms timer interrupt with an external clock source.
 *
 *  @details    This module generates exceptions in case of the clocks get disaligned
 *
 *  $Header: SRClockMonitor.c 1.10 2012/06/18 18:04:25EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2010-$Date: 2012/06/18 18:04:25EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SRClockMonitor.h"
#include "SRClockMonitor_prv.h"
#include "SRException.h"
#include "SRData.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

#ifndef SRCLOCKMONITOR_FEATURE
    #define SRCLOCKMONITOR_FEATURE      ENABLED
#endif

#if (CLASSB_FEATURE == ENABLED)
    #if (SRCLOCKMONITOR_FEATURE != ENABLED)
        SW05_WARNING(2, "SRCLOCKMONITOR_FEATURE is disabled", SW05_CLASSB)
    #endif
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


#if (SRCLOCKMONITOR_FEATURE == ENABLED)
    static uint16 SRClockMonitor_Counter;
    static uint8 SRClock_First_Reading;
    static uint8 SRClockMonitor_ErrorCount;
    static uint8 SRClockMonitor_Event;
    static uint16 N_SRClockMonitor_Counter;
    static uint8 N_SRClock_First_Reading;
    static uint8 N_SRClockMonitor_ErrorCount;
    static uint8 N_SRClockMonitor_Event;

    //This variable holds the module information only does not interact with the safety code
    static SRCLOCKMONITOR_STATISTICS_DATA_TYPE SRClockMonitor_Statistics;
    static BOOL_TYPE Clear_Stats_Data;


    #ifndef SRCLOCKMONITOR_MAX_ERROR_OCCURENCE
        #define SRCLOCKMONITOR_MAX_ERROR_OCCURENCE          4
    #endif

    #ifndef SRCLOCKMONITOR_INITIAL_DISREGARDED_READINGS
        #define SRCLOCKMONITOR_INITIAL_DISREGARDED_READINGS 4
    #endif

    #ifndef SRCLOCK_MAX_ABSOLUT_COUNTER
        #define SRCLOCK_MAX_ABSOLUT_COUNTER         0xFFFF
    #endif

    #if(SRCLOCK_MAX_ABSOLUT_COUNTER > 0xFFFF)
        #error "SRCLOCK_MAX_ABSOLUT_COUNTER cannot be higher than 0xFFFF"
    #endif

#define     SRCLOCKMONITOR_EVENT_KEYWORD            0x51u
#define     SRCLOCKMONITOR_NO_EVENT_KEYWORD         0xA5u

#endif


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if (SRCLOCKMONITOR_FEATURE == ENABLED)
    static void reset_stat_data(void);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRClockMonitor and its variables
 *
 */
void SRClockMonitor__Initialize(void)
{
    #if (SRCLOCKMONITOR_FEATURE == ENABLED)
        SRCLOCKMONITOR_INIT_FLOW();
        SRData__UpdateByte(&SRClockMonitor_Event,&N_SRClockMonitor_Event,SRCLOCKMONITOR_NO_EVENT_KEYWORD);
        SRData__UpdateShort(&SRClockMonitor_Counter,&N_SRClockMonitor_Counter,0u);
        SRData__UpdateByte(&SRClock_First_Reading,&N_SRClock_First_Reading,0u);
        SRData__UpdateByte(&SRClockMonitor_ErrorCount,&N_SRClockMonitor_ErrorCount,0u);

        Clear_Stats_Data = FALSE;
        reset_stat_data();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Monitor 1ms
 *  This method is responsible by the TimerInterrupot clock monitoring.
 *  It counts ticks based on timerInterruption for the MonitorExtclk compare with the external clock source.
 */
void SRClockMonitor__Monitor1ms(void)
{

#if (SRCLOCKMONITOR_FEATURE == ENABLED)
    SRCLOCKMONITOR_1MS_FLOW();
    (void) SRData__CheckShort(&SRClockMonitor_Counter,&N_SRClockMonitor_Counter);
    (void) SRData__CheckByte(&SRClockMonitor_Event,&N_SRClockMonitor_Event);
    
    if(SRClockMonitor_Event == SRCLOCKMONITOR_EVENT_KEYWORD)
    {
        SRData__UpdateByte(&SRClockMonitor_Event,&N_SRClockMonitor_Event,SRCLOCKMONITOR_NO_EVENT_KEYWORD);
        SRData__UpdateShort(&SRClockMonitor_Counter,&N_SRClockMonitor_Counter,0u);
    }

    SRData__UpdateShort(&SRClockMonitor_Counter,&N_SRClockMonitor_Counter,SRClockMonitor_Counter + 1u);
    if (SRClockMonitor_Counter >= (uint16) SRCLOCK_MAX_ABSOLUT_COUNTER)
    {
        SRException__Queue(SREXCEPTION_SRCLOCK_OVERRUN,(uint32)SRClockMonitor_Counter,0u);
        SRData__UpdateShort(&SRClockMonitor_Counter,&N_SRClockMonitor_Counter,0u);
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is responsible to check the internal clock by comparing the ticks counted by the Monitor1ms
 * with the ext_ticks from the external source of clock.
 *
 * @param   ext_ticks   Number of ticks expected to happen in the 1ms interrupt since the last call of this method.
 */
void SRClockMonitor__MonitorExtClk(uint16 ext_ticks)
{
#if (SRCLOCKMONITOR_FEATURE == ENABLED)

    SRCLOCKMONITOR_EXT_FLOW();
    (void) SRData__CheckByte(&SRClock_First_Reading,&N_SRClock_First_Reading);
    //To avoid the lack of precision in the first interaction it ignores the first interactions,
    //after that we do expect the system to be in sync
    if (SRClock_First_Reading < (uint8) SRCLOCKMONITOR_INITIAL_DISREGARDED_READINGS )
    {
        SRData__UpdateByte(&SRClock_First_Reading,&N_SRClock_First_Reading,SRClock_First_Reading+1u);
    }
    else
    {
        //Clear the stats data here
        if (Clear_Stats_Data == TRUE)
        {
            Clear_Stats_Data = FALSE;
            reset_stat_data();
        }

        // Update statistics
        SRClockMonitor_Statistics.External_Ticks       =   ext_ticks;

        uint16 deviation;
        if (ext_ticks > SRClockMonitor_Counter)
        {
            deviation =  ext_ticks - SRClockMonitor_Counter;
        }
        else
        {
            deviation =  SRClockMonitor_Counter - ext_ticks;
        }

        if (SRClockMonitor_Statistics.Max_Delta_Ticks < deviation)
        {
            SRClockMonitor_Statistics.Max_Delta_Ticks = deviation;
        }


        sint32 max = (sint32) ext_ticks + SRCLOCKMONITOR_CLK_TICKS_ABOVE;
        sint32 min = (sint32) ext_ticks - SRCLOCKMONITOR_CLK_TICKS_BELOW;

        (void) SRData__CheckShort(&SRClockMonitor_Counter,&N_SRClockMonitor_Counter);
        (void) SRData__CheckByte(&SRClockMonitor_ErrorCount,&N_SRClockMonitor_ErrorCount);
        if (((sint32) SRClockMonitor_Counter < min ) ||
            ((sint32) SRClockMonitor_Counter > max))
        {
            SRData__UpdateByte(&SRClockMonitor_ErrorCount,&N_SRClockMonitor_ErrorCount,SRClockMonitor_ErrorCount+1u);
            if (SRClockMonitor_ErrorCount >= (uint16) SRCLOCKMONITOR_MAX_ERROR_OCCURENCE)
            {
                // Out of Range
                SRException__Queue(SREXCEPTION_SRCLOCK_TEST,(uint32)SRClockMonitor_Counter,(uint32)ext_ticks);
            }
        }
        else
        {
            SRData__UpdateByte(&SRClockMonitor_ErrorCount,&N_SRClockMonitor_ErrorCount,0u);
        }
    }

    // Set external clock event occurred
    (void) SRData__CheckByte(&SRClockMonitor_Event,&N_SRClockMonitor_Event);
    if(SRClockMonitor_Event == SRCLOCKMONITOR_NO_EVENT_KEYWORD)
    {
        SRData__UpdateByte(&SRClockMonitor_Event,&N_SRClockMonitor_Event,SRCLOCKMONITOR_EVENT_KEYWORD);
    }
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API returns the static data usually used for debugging
 *
 * @return
 *  @retval SRCLOCKMONITOR_STATISTICS_DATA_TYPE *
 */
SRCLOCKMONITOR_STATISTICS_DATA_TYPE * SRClockMonitor__GetStatisticsData(void)
{
    #if (SRCLOCKMONITOR_FEATURE == ENABLED)
        return &SRClockMonitor_Statistics;
    #else
        return NULL;
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
#if (SRCLOCKMONITOR_FEATURE == ENABLED)
static void reset_stat_data(void)
{
        SRClockMonitor_Statistics.External_Ticks  =   SRClockMonitor_Counter;
        SRClockMonitor_Statistics.Max_Delta_Ticks =   0;
}
#endif

