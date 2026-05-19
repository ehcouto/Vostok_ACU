/**
 *  @file
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  
 *
 *  @copyright  Copyright 2012-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "uc.h"
#include "Rtc.h" 
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

void Rtc__Initialize(void)
{

}

void Rtc__EnableClockOutput(void)
{

}

void Rtc__DisbaleClockOutput(void)
{

}

void Rtc__SetTrim(unsigned short int trim_value)
{

}


unsigned char Rtc__SetCurrentHMS(TIME_TYPE *current_time)
{
    return (0);
}
TIME_TYPE * Rtc__GetCurrentHMS(void)
{
    return (NULL);
}

unsigned char Rtc__SetCurrentDate(DATE_TYPE * current_date)
{
    return (0);
}

DATE_TYPE * Rtc__GetCurrentDate(void)
{
    return (NULL);
}

void Rtc__UpdateMonthWeekDays(DATE_TYPE * current_date)
{

}


unsigned long Rtc__GetCurrentSecond(void)
{
    return (0);
}

void Rtc__SetCurrentSecond(unsigned long int sec)
{

}


void Rtc__SaveContext(void)
{

}

unsigned long Rtc__GetSavedSecond(void)
{
    return (0);
}

DATE_TYPE * Rtc__GetSavedDate(void)
{
    return (NULL);
}

TIME_TYPE * Rtc__GetSavedHMS(void)
{
    return (NULL);
}


void Rtc__Handler(void)
{

}

unsigned long int Rtc__GetEpoch(DATE_TYPE * pt_date,TIME_TYPE * pt_time )
{
    return (0);
}

#if (RTC_DAYLIGHT_SAVINGS_FEATURE == ENABLED)

void Rtc__SetDayLightSavingsFeature(T_BOOL state)
{

}

unsigned long int Rtc__GetDayLightSavingsStart(void)
{
    return (0);
}

unsigned long int Rtc__GetDayLightSavingsStop(void)
{
    return (0);
}

unsigned char Rtc__SetDayLightInfo(DST_TYPE *p_dst_info)
{
    return (0);
}
#endif

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


