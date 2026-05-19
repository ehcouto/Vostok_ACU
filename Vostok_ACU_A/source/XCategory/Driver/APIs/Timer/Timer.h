/**
 *  @file       
 *
 *  @defgroup   CLASS_B
 *
 *  @brief      Timer module configures the timer in Pwm, Output Compare and Input Capture mode
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: Timer.h $
 *
 *  @copyright  Copyright 2015-$Date:$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TIMER_H_
#define TIMER_H_
#include "Timer_prm.h"
#include "Gpio.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER6,
    TIMER7,
    TIMER8,
    TIMER9,
    TIMER10,
    TIMER11,
    TIMER12,
    TIMER13,
    TIMER14,
    TIMER15,
    TIMER16,
    TIMER17,
    TIMER18,
    TIMER19
}TIMER_TYPE;

typedef enum
{
    TIMER_CHANNEL0,
    TIMER_CHANNEL1,
    TIMER_CHANNEL2,
    TIMER_CHANNEL3,
    TIMER_CHANNEL4,
    TIMER_CHANNEL5,
    TIMER_CHANNEL6,
    TIMER_CHANNEL7,
    TIMER_CHANNEL8,
    TIMER_CHANNEL9,
    TIMER_CHANNEL10,
    TIMER_CHANNEL11,
    TIMER_CHANNEL12
}TIMER_CHANNEL_TYPE;


#define TIMERS_CHANNEL(timer,channel)          ((timer << 8)+channel)
#define TIMERS_GETTIMER(timer_channel)        ((TIMER_TYPE)(timer_channe >> 8))
#define TIMERS_GETCHANNEL(timer_channel)      ((TIMER_CHNL_TYPE)(timer_channe & 0x0F))

#define TIMER_INVALID_COUNTER_ID                0xFF

typedef enum
{
    TIMER_MODE_DISABLE,
    TIMER_MODE_PWM,
    TIMER_MODE_OUTPUT_COMPARE,
    TIMER_MODE_INPUT_CAPTURE
}TIMER_MODE_TYPE;

typedef enum
{
    TIMER_ONCOMPARE_PIN_HIGH,
    TIMER_ONCOMPARE_PIN_LOW,
    TIMER_ONCOMPARE_PIN_HIGH_INTERRUPT,
    TIMER_ONCOMPARE_PIN_LOW_INTERRUPT,
    TIMER_ONCOMPARE_INTERRUPT
}TIMER_ONCOMPARE_TYPE;

typedef enum
{
    TIMER_CAPTURE_PULSE,
    TIMER_CAPTURE_CYCLE
}TIMER_CAPTURE_TYPE;

typedef enum
{
    TIMER_CAPTUREON_RISING_EDGE,
    TIMER_CAPTUREON_FALLING_EDGE
}TIMER_CAPTURE_EDGE_TYPE;

typedef enum
{
    TIMER_POLARITY_ACTIVE_HIGH,
    TIMER_POLARITY_ACTIVE_LOW
}TIMER_POLARITY_TYPE;

PACKED typedef struct
{
    TIMER_TYPE Timer;
    TIMER_CHANNEL_TYPE Channel;
    TIMER_MODE_TYPE Mode;
    GPIO_PORT_TYPE Port;
    uint8 Pin;
    uint8 Pin_ReMap;
    TIMER_POLARITY_TYPE Polarity;
}TIMER_PARAMETER_TYPE;

#include "Timer_defs.h"

#ifndef TIMER_PWM_FREQUENCY_TYPE
    #define  TIMER_PWM_FREQUENCY_TYPE uint16
#endif
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Timer__Initialize(void);
PASS_FAIL_TYPE Timer__Config(TIMER_PARAMETER_TYPE *timer_parameter);
PASS_FAIL_TYPE Timer__ConfigCheck(TIMER_PARAMETER_TYPE *timer_parameter);

// Stop Counter API
uint8 Timer__StartCount(TIMER_PARAMETER_TYPE *timer_parameter);
uint32 Timer__ReStartCount(uint8 count_id);
uint32 Timer__GetCount(uint8 count_id);
uint32 Timer__StopCount(uint8 count_id);

// Output Compare API
PASS_FAIL_TYPE Timer__OutputCompareStart(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_ONCOMPARE_TYPE action, uint32 timeout_usec);
PASS_FAIL_TYPE Timer__OutputCompareStartFromLast(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_ONCOMPARE_TYPE action, uint32 timeout_usec);

// InputCapture API
PASS_FAIL_TYPE Timer__CaptureStart(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_CAPTURE_TYPE mode, TIMER_CAPTURE_EDGE_TYPE catch_on);
uint32 Timer__GetCaptureDuration(TIMER_PARAMETER_TYPE *timer_parameter);
uint32 Timer__GetCaptureFrequcncy(TIMER_PARAMETER_TYPE *timer_parameter);
uint16 Timer__GetCaptureDuty(TIMER_PARAMETER_TYPE *timer_parameter);

// Pwm API
PASS_FAIL_TYPE Timer__PwmStart(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_PWM_FREQUENCY_TYPE frequency, uint16 duty);
PASS_FAIL_TYPE Timer__PwmSetDutyCycle(TIMER_PARAMETER_TYPE *timer_parameter, uint16 duty);
uint16 Timer__PwmGetDutyCycle(TIMER_PARAMETER_TYPE *timer_parameter);
TIMER_PWM_FREQUENCY_TYPE Timer__PwmGetFrequency(TIMER_PARAMETER_TYPE *timer_parameter);
BOOL_TYPE Timer__PwmStop(TIMER_PARAMETER_TYPE *timer_parameter, BOOL_TYPE stop_state);
PASS_FAIL_TYPE Timer__PwmResume(TIMER_PARAMETER_TYPE *timer_parameter);

#endif // TIMER_H_
