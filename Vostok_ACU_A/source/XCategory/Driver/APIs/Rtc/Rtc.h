/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef RTC_H_
#define RTC_H_
#include "C_Extensions.h"
#include "Rtc_prm.h"

#define RTC_START_YEAR          (2013)
#define RTC_END_YEAR            (RTC_START_YEAR + 144)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    DUMMY_MONTH = 0,
    JANUARY,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} MONTH_TYPE;

typedef enum
{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} WEEKDAY_TYPE;

//! 2013 Jan 1st is Tuesday. When Base Year changes (must be leap year), corresponding weekday required to be change
#define RTC_WEEK_DAY       (TUESDAY)

#if (RTC_DAYLIGHT_SAVINGS_FEATURE == ENABLED)
    typedef enum
    {
        FIRST = 0,
        SECOND,
        THIRD,
        FOURTH,
        LAST                                        //!< Using LAST will select the 5th or 4th if 5th is not valid
    } DST_WEEK;

    typedef struct
    {
        DST_WEEK DSTMultipleWeekStart,DSTMultipleWeekEnd;
        WEEKDAY_TYPE DSTWeekDayStart,DSTWeekDayEnd;
        MONTH_TYPE DSTMonthStart,DSTMonthEnd;
       uint8 DSTHourStart,DSTHourEnd;
    } DST_TYPE;

#endif

typedef struct
{
   uint8 Hrs;
   uint8 Min;
   uint8 Sec;
} TIME_TYPE;

typedef struct
{
   uint8 Day;
   WEEKDAY_TYPE WeekDay;
   MONTH_TYPE Month;
   uint16 Year;
   uint8 TotalDaysInMonth;
} DATE_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            Rtc__Initialize             (void);
void            Rtc__EnableClockOutput      (void);
void            Rtc__DisbaleClockOutput     (void);
void            Rtc__SetTrim                (uint16 trim_value);
uint16          Rtc__GetTrim                (void);

uint8           Rtc__SetCurrentHMS          (TIME_TYPE *current_time);
TIME_TYPE *     Rtc__GetCurrentHMS          (void);

uint8           Rtc__SetCurrentDate         (DATE_TYPE * current_date);
DATE_TYPE *     Rtc__GetCurrentDate         (void);
void            Rtc__UpdateMonthWeekDays    (DATE_TYPE * current_date);

uint32          Rtc__GetCurrentSecond       (void);
void            Rtc__SetCurrentSecond       (uint32 sec);

void            Rtc__SaveContext            (void);
uint32          Rtc__GetSavedSecond         (void);
DATE_TYPE *     Rtc__GetSavedDate           (void);
TIME_TYPE *     Rtc__GetSavedHMS            (void);

void            Rtc__Handler                (void);
uint32          Rtc__GetEpoch               (DATE_TYPE * pt_date,TIME_TYPE * pt_time );

#if (RTC_DAYLIGHT_SAVINGS_FEATURE == ENABLED)
    void        Rtc__SetDayLightSavingsFeature  (BOOL_TYPE state);
   uint32       Rtc__GetDayLightSavingsStart    (void);
   uint32       Rtc__GetDayLightSavingsStop     (void);
   uint8        Rtc__SetDayLightInfo            (DST_TYPE *p_dst_info);
#endif

#endif // RTC_H_
