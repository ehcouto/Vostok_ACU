/**
 *    @file
 *    @brief    It includes the declarations for enumerations and methods used in ZeroCross GDM
 *
 *    @details  This file includes the declarations for enumerations "ZEROCROSS_EVENT_TYPE" - this depicts events generated when
 *              four angles (0, 180 on zeros, 90, 270 on peaks) or zero cross lost signal is detected,
 *              and declarations of all ZeroCross APIs
 *              Add descriptions of all selectable features to the following list:
 *
 *  ZEROCROSS_POLLING_MODE      Enables Polling Mode
 *  ZC_ALLOWED_MIN_FREQUENCY	Minimum allowed ZeroCross frequency in Hz (default is 45)
 *  ZC_ALLOWED_MAX_FREQUENCY	Maximum allowed ZeroCross frequency in Hz (default is 65)
 *  ZC_ADVANCE_PULSE_DELAY		Timing between real Zero event and edge detected by ZeroCross signal in us
 *  ZC_TRIGGER_EDGE				Trigger event to detect ZeroCross zero degree event (EXTINT_TRG_RISE or EXTINT_TRG_FALL)
 *  ZC_DEBOUNCE_CYCLE			Debounce events before ZeroCross signal is considered valid
 *  ZC_MAX_CYCLE_TIMEOUT		Timeout for ZeroCross lost signal event
 *
 * @brief  - Advance delay in micro-seconds
 *
 * @details - This configuration is very important.This configuration defines Timing gaps between Actual zero cross signal detected on main sin wave
 *            & actual Load firing edge with respect to that zero cross signal.
 *            This Load firing time is most of the time not match(Leading) with Actual Zero cross signal detected on main sin wave
 *
 * @note -    In below waveform there is a timing gap of T1,T3 between Actual Zero cross signal on mains Sin wave & Load firing edge.
 *            Loads are not firing exactly on that points where mains sin wave zero cross detect.This timing(T1,T3) get different with different hardware(Not fix).
 *            User should calculate this timing gap and assign the value on ZC_ADVANCE_PULSE_DELAY macro .
 *            this macro will take care to fire the loads on exact Zero cross signal on mains Sin wave.
 *            suppose T1 time gap is 4ms then define macro as below
 *
 *            #define ZC_ADVANCE_PULSE_DELAY          4000
 *
 *            if there is no any time gap between Actual Zero cross signal on mains Sin wave & Load firing edge . then Assign the macro as below
 *
 *            #define ZC_ADVANCE_PULSE_DELAY          0
 *
 *
 *
 *                            |   |               |   |    |               |     |
 *                            |   |<--triac on--->|   |    |<---triac on-->|     |
 *                            |   |               |   T2   |               |     |
 *                            |T1 |               |<------>|--triac off    |     |
 *                 triac off--|<->|               |   | T3 |               |<--->|--triac off
 *                            |   |               |   |    |       -       |     |
 *                            |   |               |   |    |   -       -   |     |
 *                            |   |               |   |    |-             -|     |
 *                            |   |               |   |  - |               |  -  |
 *                          - |   |               |   |-   |               |    -|
 *                     -------------------------------------------------------------------------- Sine Wave
 *                            | - |                  -|
 *                            |   -                -  |
 *                            |   | -           -     |
 *                            |   |    -      -       |
 *                            |   |        -          |
 *                            |                       |
 *                            |
 *                            |    ________________         _______________
 *                            |   |                |       |               |
 *                            |   |                |       |               |
 *                            |___|                |       |               |
 *                                                  -------                 -----------------ZeroCross Signal
 *
 *  @copyright  Copyright 2011-$Date: 2015/07/14 17:37:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef ZEROCROSS_H
	#define ZEROCROSS_H
#include "SystemConfig.h"
#include "ExtInterrupt.h"
#include "Timer.h"
#include "Gpio.h"
#include "Atod.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

// ZeroCross state
typedef enum ZEROCROSS_STATE_ENUM
{
    ZEROCROSS_INITIALIZE = 0,
    ZEROCROSS_BLACKOUT,
    ZEROCROSS_DETECTING,
    ZEROCROSS_DETECTED_SIGNAL
} ZEROCROSS_STATE_TYPE;

// ZeroCross determined Lobes
typedef enum ZEROCROSS_LOBE_ENUM
{
    ZEROCROSS_UNKNOWN_LOBE,
    ZEROCROSS_POS_LOBE,
    ZEROCROSS_NEG_LOBE
} ZEROCROSS_LOBE_TYPE;

// ZeroCross Angle Events
typedef enum ZEROCROSS_EVENT_ENUM
{
    ZEROCROSS_NO_EVENT = 0,
    ZEROCROSS_ANGLE_0_EVENT,
    ZEROCROSS_ANGLE_90_EVENT,
    ZEROCROSS_ANGLE_180_EVENT,
    ZEROCROSS_ANGLE_270_EVENT,
    ZEROCROSS_BLACKOUT_EVENT
} ZEROCROSS_EVENT_TYPE;

PACKED typedef struct
{
    uint16 Gpio_Id;
    BOOL_TYPE Active_value;    //! For Active High = 1, Active Low = 0
    uint8 Active_Degree;       //! Active Degree for Half semi wave
    uint8 Deactive_Degree;     //! De active Degree for Half semi wave..for continuous on set this value more than 180 degree
} ZEROCROSS_GPIO_REQUEST_TYPE;


#define ZEROCROSS_MINIMUM_ANGLE         0
#define ZEROCROSS_HALF_WAVE_ANGLE       180
#define ZEROCROSS_MAXIMUM_ANGLE         360

#define ZEROCROSS_ID_INVALID            0xFFFFU

// Macro to convert a decimal number in Q notation [-32..31.999999]
#define ZC_Q_DECIMALS                   26
#define ZC_FILTER_Q(x)                  ((sint32)(((float32)(1UL << ZC_Q_DECIMALS)) * ((float32) x)))

// Macro to convert a Frequency in Cycle value in usec
#define ZC_CYCLE_USEC(x)               ((uint16)(1000000.0 / ((float32)(x))))

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

// ----------------------------------------------- ZeroCross Public API -----------------------------------------------
uint16 ZeroCross__GetAngle(void);
uint32 ZeroCross__GetCycleCount(void);
uint8 ZeroCross__GetCycleEventsSequence(void);
uint8 ZeroCross__GetFrequency(void);
ZEROCROSS_EVENT_TYPE ZeroCross__GetLastEvent(void);
ZEROCROSS_LOBE_TYPE ZeroCross__GetLobe(void);
uint16 ZeroCross__GetMicroSecondsInLobe(void);
ZEROCROSS_STATE_TYPE ZeroCross__GetState(void);

BOOL_TYPE ZeroCross__GetPinValue(void);

// ---------------------------------------------- ZeroCross Gpio service API ------------------------------------------
uint16 ZeroCross__RegisterGpioActivity(GPIO_PORT_TYPE port, uint8 pin);
PASS_FAIL_TYPE ZeroCross__ReleaseGpioActivity(uint16 gpio_id);
BOOL_TYPE ZeroCross__RequestGpioActivity(ZEROCROSS_GPIO_REQUEST_TYPE *gpio_activity);


// -------------- ZeroCross reserved API for Hbl - do not use those functions outside Hbl module! ---------------------
ZEROCROSS_EVENT_TYPE ZeroCross__AngleEvent(void);
ZEROCROSS_EVENT_TYPE ZeroCross__AsyncHandler(void);
PASS_FAIL_TYPE ZeroCross__Config(EXTINT_CHNL_TYPE ext_chnl, GPIO_PORT_TYPE ext_port, uint8 ext_pin, uint8 ext_remap,
    TIMER_TYPE timer, TIMER_CHANNEL_TYPE timer_channel);
ZEROCROSS_EVENT_TYPE ZeroCross__FastHandler(void);
void ZeroCross__Initialize(void);
void ZeroCross__SignalEvent(void);
void ZeroCross__GpioRequestHandler(void);

// ----------------------------------------------- ZeroCross DEPRECATED API -------------------------------------------
uint16 ZeroCross__GetTimeFromLastEvent(void);
uint16 ZeroCross__RegisterAtodActivity(ATOD_CHANNEL_DEF channel, ATOD_CHANNEL_RESOLUTION_DEF resolution);
PASS_FAIL_TYPE ZeroCross__ReleaseAtodActivity(uint16 atod_id);
BOOL_TYPE ZeroCross__IsAtodReadOverrun(uint16 atod_id);
uint16 ZeroCross__CopyAtodSamples(uint16 atod_id, uint16 *buffer, uint16 max_items);


#endif /*ZEROCROSS_H*/
