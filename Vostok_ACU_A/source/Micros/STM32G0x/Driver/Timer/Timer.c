/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @defgroup   CLASS_B
 *
 *  Port-bit for Timer channels
 *  <pre>
 *  --------------------------------------------------------
 * |               |  PIN REMAP     |PIN REMAP  |PIN REMAP  |
 * | TIMER CHANNEL |----------------|-----------|-----------|
 * |               |       2        |    1      |    1      |
 * |---------------|----------------|-----------|-----------|
 * |   TIM1_CH1    |   PA8, PC8     |           |   PE9     |
 * |---------------|----------------|-----------|-----------|
 * |   TIM1_CH2    |   PA9, PC9     |   PB3     |   PE11    |
 * |---------------|----------------|-----------|-----------|
 * |   TIM1_CH3    |   PA10, PC10   |   PB6     |   PE13    |
 * |---------------|----------------|-----------|-----------|
 * |   TIM1_CH4    |   PA11, PC11   |           |           |
 *  --------------------------------------------------------
 *
 *  -----------------------------------------------
 * |               |           PIN REMAP           |
 * | TIMER CHANNEL |-------------------------------|
 * |               |              2                |
 * |---------------|-------------------------------|
 * |   TIM2_CH1    |   PA0, PA15, PA5, PA15, PC4   |
 * |---------------|-------------------------------|
 * |   TIM2_CH2    |   PA1, PB3, PC5               |
 * |---------------|-------------------------------|
 * |   TIM2_CH3    |   PA2, PB10, PC6              |
 * |---------------|-------------------------------|
 * |   TIM2_CH4    |   PA3, PB11, PC7              |
 *  -----------------------------------------------
 *
 *  -----------------------------------------
 * |               |        PIN REMAP        |
 * | TIMER CHANNEL |-------------------------|
 * |               |            1            |
 * |---------------|-------------------------|
 * |   TIM3_CH1    |   PA6, PC6, PE3, PB4    |
 * |---------------|-------------------------|
 * |   TIM3_CH2    |   PA7, PC7, PE4, PB5    |
 * |---------------|-------------------------|
 * |   TIM3_CH3    |   PB0, PC8, PE5         |
 * |---------------|-------------------------|
 * |   TIM3_CH4    |   PB1, PC9, PE6         |
 *  ----------------------------------------
 *
 *  ---------------------- ------------
 * |               |     PIN REMAP     |
 * | TIMER CHANNEL |------ |-----------|
 * |               |   9   |    2      |
 * |---------------|------ |-----------|
 * |   TIM4_CH1    |   PB6 |   PD12    |
 * |---------------|------ |-----------|
 * |   TIM4_CH2    |   PB7 |   PD13    |
 * |---------------|------ |-----------|
 * |   TIM4_CH3    |   PB8 |   PD14    |
 * |---------------|------ |-----------|
 * |   TIM4_CH4    |   PB9 |   PD15    |
 *  -----------------------------------
 *
 *
 *  --------------------------------------------------------
 * |               |      PIN REMAP                         |
 * | TIMER CHANNEL |----------------------------------------|
 * |               |    0     |       2       |      4      |
 * |---------------|----------|---------------|-------------|
 * |   TIM14_CH1   |   PB1    |   PC12, PF0   |   PA4, PA7  |
 *  --------------------------------------------------------

 *  -----------------------------------------------------
 * |               |            PIN REMAP                |
 * | TIMER CHANNEL |-------------------------------------|
 * |               |    0     |    2     |    5          |
 * |---------------|----------|----------|---------------|
 * |   TIM15_CH1   |   PF12   |   PC1    |   PA2, PB14   |
 * |---------------|----------|----------|---------------|
 * |   TIM15_CH2   |   PF13   |   PC2    |   PA3, PB15   |
 *  ------------------------------------------------------
 *
 *  -----------------------------------------------------
 * |               |                PIN REMAP            |
 * | TIMER CHANNEL |--------------------------|----------|
 * |               |       2       |    5     |    0     |
 * |---------------|---------------|----------|----------|
 * |   TIM16_CH1   |   PB8, PD0    |   PA6    |   PE0    |
 *  ------------------------------------------------------
 *
 *  -----------------------------------------------------
 * |               |               PIN REMAP             |
 * | TIMER CHANNEL |-------------------------------------|
 * |               |       2       |    5     |    0     |
 * |---------------|---------------|----------|----------|
 * |   TIM17_CH1   |   PB9, PD1    |   PA7    |   PE1    |
 *  -----------------------------------------------------  </pre>
 *
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Timer.h" 
#include "uc.h"
#include "Micro.h"
#include "string.h"
#include "Gpio.h"

//Class B related modules
#include "SRException.h"
#include "SRData.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define TIMER_MAX_TIMER_INDEX                   TIMER17
#define TIMER_MAX_TIMER_CHNL_INDEX              4
#define TIMER_MAX_COUNT_VALUE                   0xFFFF

#define INVALID_MEM_INDEX                       0xFFU
#define INVALID_MEM_ID                          0xFFFFU

// Micro Configuration ----------------------------------------------------------
#define TIM_CR1_CLEAR                         ((uint16)0x0000)
#define TIM_CNT_CLEAR                         ((uint32)0x00000000)

// PWM mode configuration -------------------------------------------------------
#define PWM_MAX_DUTY_CYCLE                      10000

//! It defines the Frequency after the prescaler for PWMs running below TIMER_FREQUENCY_LOW_LIMIT
#define TIMER_FREQUENCY_LOW_BASE                (uint32)65000

//! It defines the Frequency limit considered LOW
#define TIMER_FREQUENCY_LOW_LIMIT               300

//! It defines the Frequency after the prescaler for PWMs running from TIMER_FREQUENCY_LOW_LIMIT to TIMER_FREQUENCY_MID_LIMIT
#define TIMER_FREQUENCY_MID_BASE                (uint32)1000000

//! It defines the Frequency limit considered MID
#define TIMER_FREQUENCY_MID_LIMIT               5000

// OutPutCompare Configuration --------------------------------------------------
#define TIMER_1US_TICK                          1000000

// Keepout time to protect OutputCompareStartFromLast function to ensure next scheduled event would happen in the future
#define GUARD_TICK_VALUE						2u

// Input Capture Configuration --------------------------------------------------
typedef enum
{
    CAPTURE_EDGE_RISING  = 0,
    CAPTURE_EDGE_FALLING = 2
}CAPTURE_EDGE_TYPE;

typedef enum
{
    TIMER_1 = 0,
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_14,
    TIMER_15,
    TIMER_16,
    TIMER_17,
    TIMER_NULL = 0xFF
}TIMERS_INDEX_DEF;

static const TIMERS_INDEX_DEF TIMER_INDEX[] =
{
    TIMER_NULL,
    TIMER_1,
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_NULL,
    TIMER_14,
    TIMER_15,
    TIMER_16,
    TIMER_17
};

typedef struct
{
    TIM_DEF_TYPE *timer;
    IRQ_CHANNEL_TYPE tim_irq_chnl;
    uint8 premp_prior;
}TIMER_LIST_TYPE;

typedef union
{
    uint8 Byte;
    struct
    {
        uint8 remain_edge:4;
        uint8 initial_edge :1;
        uint8 cycle_mode:1;
        uint8 :2;
    }Bit;
}TIMER_CAPTURE_CONFIG_TYPE;

typedef struct
{
    uint16 id;
    TIMER_CAPTURE_CONFIG_TYPE config;
    uint32 prev_cnt;
    uint32 pos_dur;
    uint32 neg_dur;
    uint32 saved_duration;
    uint16 saved_pos_dur;
}TIMER_CAPTURE_TIME_TYPE;

typedef struct
{
    uint8 id;
    uint32 stamp_t0;
}TIMER_COUNT_TYPE;

static const TIMER_LIST_TYPE TIMER_LIST[] =
{
    {&TIM_1   ,TIM1_CC_IRQ_CHANNEL   ,TIMER1_PREEMPTION_PRIORITY    },
    {&TIM_2   ,TIM2_IRQ_CHANNEL      ,TIMER2_PREEMPTION_PRIORITY    },
    {&TIM_3   ,TIM3_TIM4_IRQ_CHANNEL ,TIMER3_4_PREEMPTION_PRIORITY  },
    {&TIM_4   ,TIM3_TIM4_IRQ_CHANNEL ,TIMER3_4_PREEMPTION_PRIORITY  },
    {&TIM_14  ,TIM14_IRQ_CHANNEL     ,TIMER14_PREEMPTION_PRIORITY   },
    {&TIM_15  ,TIM15_IRQ_CHANNEL     ,TIMER15_PREEMPTION_PRIORITY   },
    {&TIM_16  ,TIM16_IRQ_CHANNEL     ,TIMER16_PREEMPTION_PRIORITY   },
    {&TIM_17  ,TIM17_IRQ_CHANNEL     ,TIMER17_PREEMPTION_PRIORITY   }
};

static TIMER_CAPTURE_TIME_TYPE Timer_CatchTimer[TIMER_MAX_CAPTURE_CHNL];
static TIMER_CAPTURE_TIME_TYPE N_Timer_CatchTimer[TIMER_MAX_CAPTURE_CHNL];

static TIMER_COUNT_TYPE Timer_Count[TIMER_MAX_COUNTER_USED];
static TIMER_COUNT_TYPE N_Timer_Count[TIMER_MAX_COUNTER_USED];
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void TimerInit(TIMER_PARAMETER_TYPE *timer_parameter);
static void TimerOutputConfig(TIM_DEF_TYPE * timer_pt, TIMER_CHANNEL_TYPE chnl, TIMER_MODE_DEF mode);
static PASS_FAIL_TYPE TimerValidate(TIMER_TYPE tim, TIMER_CHANNEL_TYPE chnl);
static void Timer1usFreeCntr(TIM_DEF_TYPE * tim_pt, TIMER_CHANNEL_TYPE chnl);
static uint32 TimerBusFreq(TIM_DEF_TYPE * tim_pt);
static void TimerInputCaptureConfig(TIM_DEF_TYPE * timer_pt, TIMER_CHANNEL_TYPE chnl, CAPTURE_EDGE_TYPE edge);
static PASS_FAIL_TYPE AllocateCatchTimer(TIMER_PARAMETER_TYPE *timer_parameter);
static void DeAllocateCatchTimer(TIMER_PARAMETER_TYPE *timer_parameter);
static uint8 GetCatchTimerIndex(TIMER_PARAMETER_TYPE *timer_parameter);
static void PwmStateConfig(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_MODE_DEF mode);
static void PwmTimSetDuty(TIMER_PARAMETER_TYPE *timer_parameter, uint16 duty);
static void OutputCompareStart(TIMER_LIST_TYPE *timer_pt, uint8 channel, TIMER_ONCOMPARE_TYPE action, uint32 new_ccr);

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Configures the specified port-pin into the Alternate function.
 */
#define GPIO__PIN_COFIG_AF(PORT, PORTBIT, AF)\
{\
    if ((PORT < GPIO_PORT_NUM) && (PORTBIT < GPIO_MAX_PIN))\
    {\
        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] = (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] | ((uint32)(AF) << ((uint32)((uint32)PORTBIT & (uint32)0x07) << 2));\
    }\
}\

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Timer and its variables
 *
 */
void Timer__Initialize(void)
{
    SRData__Fill(Timer_CatchTimer,N_Timer_CatchTimer, (uint8) INVALID_MEM_ID, sizeof(Timer_CatchTimer));
    SRData__Fill(Timer_Count,N_Timer_Count, 0xFF, sizeof(Timer_Count));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This module configures the Timer channel into desired modes
 * @param timer_parameter
 * @return
 */
PASS_FAIL_TYPE Timer__Config(TIMER_PARAMETER_TYPE *timer_parameter)
{
    PASS_FAIL_TYPE result;
    TIM_DEF_TYPE *timer_pt;

    result = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;

        TimerInit(timer_parameter);

        switch(timer_parameter->Mode)
        {
            case TIMER_MODE_DISABLE:
                if (Gpio__Validate(timer_parameter->Port, timer_parameter->Pin) == PASS)
                {
                    Gpio__PinConfig(timer_parameter->Port, timer_parameter->Pin, INPUT_FLOAT);
                }
                DeAllocateCatchTimer(timer_parameter);
                break;

            case TIMER_MODE_PWM:
                //Configure the Pwm pin
                break;

            case TIMER_MODE_OUTPUT_COMPARE:
                if (Gpio__Validate(timer_parameter->Port, timer_parameter->Pin) == PASS)
                {
                    Gpio__PinConfig(timer_parameter->Port, timer_parameter->Pin, SPECIAL_FUNCTION_PUSH_PULL);
                }
                Timer1usFreeCntr(timer_pt, timer_parameter->Channel);
                break;

            case TIMER_MODE_INPUT_CAPTURE:
                Timer1usFreeCntr(timer_pt, timer_parameter->Channel);
                result = AllocateCatchTimer(timer_parameter);
                break;

            default:
                break;
        }

        result = PASS;
    }

    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Allocate the counter and starts counting
 *
 * @param timer_parameter
 * @return
 */
uint8 Timer__StartCount(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint8 id;
    uint8 index;

    id = 0xff;

    if (TimerValidate(timer_parameter->Timer, TIMER_CHANNEL1))
    {
        for(index = 0; index < TIMER_MAX_COUNTER_USED; index++)
        {
            if ((SR_CHECK_BYTE(Timer_Count[index].id) == SRDATA_OK) &&
                (SR_CHECK_LONG(Timer_Count[index].stamp_t0) == SRDATA_OK) )
            {
                if (Timer_Count[index].id == 0XFF)
                {
                    //allocate the memory
                    SR_SET_BYTE(Timer_Count[index].id, index + (timer_parameter->Timer << 4));
                    SR_SET_LONG(Timer_Count[index].stamp_t0, (TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer)->CNT);

                    id = Timer_Count[index].id;

                    (TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer)->CR1  |= TIM_CR1_ARPE | TIM_CR1_CEN;   //Enable the auto reload feature, Enable the Timer
                    break;
                }
            }
            else
            {
                SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);
            }
        }
    }

    return(id);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Allocate the counter and starts counting
 * @param tim
 * @return
 */
uint32 Timer__ReStartCount(uint8 count_id)
{
    uint32 elapsed_time;
    uint32 time_stamp1;
    uint8 id;
    TIMER_TYPE tim;

    elapsed_time = 0;
    id     = count_id & 0x0F;

    if (id < TIMER_MAX_COUNTER_USED)
    {
        if ((SR_CHECK_BYTE(Timer_Count[id].id) == SRDATA_OK) &&
            (SR_CHECK_LONG(Timer_Count[id].stamp_t0) == SRDATA_OK))
        {
            if (Timer_Count[id].id == count_id)
            {
                tim = (TIMER_TYPE)((count_id >> 4) & 0x0F);

                time_stamp1    = (TIMER_LIST[TIMER_INDEX[tim]].timer)->CNT;

                // calculation of the distance between time stamps
                if (time_stamp1 >= Timer_Count[id].stamp_t0)
                {
                    elapsed_time = time_stamp1 - Timer_Count[id].stamp_t0;
                }
                else
                {
                    elapsed_time = ((TIMER_LIST[TIMER_INDEX[tim]].timer)->ARR - Timer_Count[id].stamp_t0) + time_stamp1;
                }

                //snap the current count
                SR_SET_LONG(Timer_Count[id].stamp_t0, (TIMER_LIST[TIMER_INDEX[tim]].timer)->CNT);

                (TIMER_LIST[TIMER_INDEX[tim]].timer)->CR1  |= TIM_CR1_ARPE | TIM_CR1_CEN;   //Enable the auto reload feature, Enable the Timer
            }
        }
        else
        {
            SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, time_stamp1,elapsed_time);
        }
    }

    return(elapsed_time);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - returns the elapsed time in micro-second
 * @param watch_id
 * @return
 */
uint32 Timer__GetCount(uint8 count_id)
{
    uint32 elapsed_time;
    uint8 id;
    TIMER_TYPE tim;
    uint32 time_stamp1;

    id              = count_id & 0x0F;
    tim             = (TIMER_TYPE)((count_id >> 4) & 0x0F);

    elapsed_time    = 0;

    if (id < TIMER_MAX_COUNTER_USED)
    {
        if (TimerValidate(tim, TIMER_CHANNEL1))
        {
            if (SR_CHECK_LONG(Timer_Count[id].stamp_t0) == SRDATA_OK)
            {
                time_stamp1    = (TIMER_LIST[TIMER_INDEX[tim]].timer)->CNT;

                // calculation of the distance between time stamps
                if (time_stamp1 >= Timer_Count[id].stamp_t0)
                {
                    elapsed_time = time_stamp1 - Timer_Count[id].stamp_t0;
                }
                else
                {
                    elapsed_time = ((TIMER_LIST[TIMER_INDEX[tim]].timer)->ARR - Timer_Count[id].stamp_t0) + time_stamp1;
                }
            }
            else
            {
                SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, time_stamp1 , TIMER_CHANNEL1);
            }
        }
    }

    return(elapsed_time);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Deallocates the counter and returns the elapsed time in micro-second
 * @param watch_id
 * @return
 */
uint32 Timer__StopCount(uint8 count_id)
{
    uint32 elapsed_time;
    uint8 id;
    TIMER_TYPE tim;
    uint32 time_stamp1;

    id              = count_id & 0x0F;
    tim             = (TIMER_TYPE)((count_id >> 4) & 0x0F);

    elapsed_time    = 0;

    if (id < TIMER_MAX_COUNTER_USED)
    {
        if (TimerValidate(tim, TIMER_CHANNEL1))
        {
            if ((SR_CHECK_LONG(Timer_Count[id].stamp_t0) == SRDATA_OK) &&
                (SR_CHECK_BYTE(Timer_Count[id].id) ==SRDATA_OK))
            {
                time_stamp1    = (TIMER_LIST[TIMER_INDEX[tim]].timer)->CNT;

                // calculation of the distance between time stamps
                if (time_stamp1 >= Timer_Count[id].stamp_t0)
                {
                    elapsed_time = time_stamp1 - Timer_Count[id].stamp_t0;
                }
                else
                {
                    elapsed_time = ((TIMER_LIST[TIMER_INDEX[tim]].timer)->ARR - Timer_Count[id].stamp_t0) + time_stamp1;
                }

                //Deallocate the counter
                SR_SET_BYTE(Timer_Count[id].id, 0xFF);
            }
            else
            {
                SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, time_stamp1,TIMER_CHANNEL1);
            }
        }
    }

    return(elapsed_time);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API Starts the Output compare of mentioned micro second time
 * @param tim
 * @param chnl
 * @param action
 * @param timeout_usec
 * @return
 */
PASS_FAIL_TYPE Timer__OutputCompareStart(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_ONCOMPARE_TYPE action, uint32 timeout_usec)
{
    PASS_FAIL_TYPE result = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        TIMER_LIST_TYPE *timer_pt = (TIMER_LIST_TYPE *) &TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]];
        TIM_DEF_TYPE *tim_pt = timer_pt->timer;

        OutputCompareStart(timer_pt, timer_parameter->Channel, action, timeout_usec + tim_pt->CNT);
        result = PASS;
    }

    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API Starts the Output compare of mentioned micro second time since last event
 * @param timer_parameter
 * @param action
 * @param timeout_usec
 * @return
 */
PASS_FAIL_TYPE Timer__OutputCompareStartFromLast(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_ONCOMPARE_TYPE action, uint32 timeout_usec)
{
    PASS_FAIL_TYPE result = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        TIMER_LIST_TYPE *timer_pt = (TIMER_LIST_TYPE *) &TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]];
        TIM_DEF_TYPE *tim_pt = timer_pt->timer;
        uint16 new_ccr_value = tim_pt->CCR[timer_parameter->Channel-1];

		// Check if the scheduled event can be properly set; if not, fallback to OutputCompareStart
		if(((uint16) (tim_pt->CNT + GUARD_TICK_VALUE - new_ccr_value)) < timeout_usec)
		{
			new_ccr_value += timeout_usec;
		}
		else
		{
			new_ccr_value = timeout_usec + tim_pt->CNT;
		}

        OutputCompareStart(timer_pt, timer_parameter->Channel, action, new_ccr_value);
        result = PASS;
    }

    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to capture the time duration from the specified edge transition
 * @param tim
 * @param chnl
 * @param mode
 * @param catch_on
 * @return
 */
PASS_FAIL_TYPE Timer__CaptureStart(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_CAPTURE_TYPE mode, TIMER_CAPTURE_EDGE_TYPE catch_on)
{
    PASS_FAIL_TYPE result;
    TIM_DEF_TYPE *timer_pt;
    CAPTURE_EDGE_TYPE edge_type;
    TIMER_CAPTURE_CONFIG_TYPE timer_catch_timer_config;
    uint8 index;

    result = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;

        index = GetCatchTimerIndex(timer_parameter);

            if (index != INVALID_MEM_INDEX)
            {
                if ((SR_CHECK_LONG(Timer_CatchTimer[index].prev_cnt) == SRDATA_OK) && (SR_CHECK_LONG(Timer_CatchTimer[index].pos_dur) == SRDATA_OK) &&
                        (SR_CHECK_LONG(Timer_CatchTimer[index].neg_dur) == SRDATA_OK) && (SR_CHECK_LONG(Timer_CatchTimer[index].saved_duration) == SRDATA_OK) &&
                        (SR_CHECK_SHORT(Timer_CatchTimer[index].saved_pos_dur) == SRDATA_OK) &&
                        (SR_CHECK_BYTE(Timer_CatchTimer[index].config.Byte) == SRDATA_OK))
                {

                    //Initialize the timer values
                    SR_SET_LONG(Timer_CatchTimer[index].prev_cnt,0);
                    SR_SET_LONG(Timer_CatchTimer[index].pos_dur, 0);
                    SR_SET_LONG(Timer_CatchTimer[index].neg_dur, 0);
                    SR_SET_LONG(Timer_CatchTimer[index].saved_duration, 0);
                    SR_SET_SHORT(Timer_CatchTimer[index].saved_pos_dur, 0);

                    timer_catch_timer_config.Byte=0;
                    timer_catch_timer_config.Bit.remain_edge = 2;
                    timer_catch_timer_config.Bit.initial_edge = 1;

                    edge_type                           = CAPTURE_EDGE_RISING;

                    //Disable the interrupt
                    timer_pt->DIER                    &= (uint16)(~(1 << timer_parameter->Channel));

                    if (catch_on != TIMER_CAPTUREON_RISING_EDGE)
                    {
                        edge_type = CAPTURE_EDGE_FALLING;
                    }

                    if (mode == TIMER_CAPTURE_CYCLE)
                    {
                        timer_catch_timer_config.Bit.remain_edge  = 3;
                        timer_catch_timer_config.Bit.cycle_mode   = 1;
                    }

                     SR_SET_BYTE(Timer_CatchTimer[index].config.Byte,timer_catch_timer_config.Byte);

                    TimerInputCaptureConfig(timer_pt, timer_parameter->Channel, edge_type);

                    timer_pt->DIER |= (uint16)(1 << timer_parameter->Channel);

                    //Enable the Timer Interrupt
                    Micro__NVICEnableIRQ(TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].tim_irq_chnl, TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].premp_prior);

                    timer_pt->SR  = 0;
                    timer_pt->CR1  |= TIM_CR1_ARPE | TIM_CR1_CEN;   //Enable the auto reload feature, Enable the Timer
                    result = PASS;
                }
                else
                {
                    SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);
                }
            }
    }

    return(result);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Read the duration of the Capture signal
 *
 * @param timer_parameter
 * @return
 */
uint32 Timer__GetCaptureDuration(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint8 id;
    uint32 dur;

    dur = 0;

    id = GetCatchTimerIndex(timer_parameter);

    if (id != INVALID_MEM_INDEX)
    {
        if (SR_CHECK_LONG(Timer_CatchTimer[id].saved_duration)  == SRDATA_OK)
        {
            dur = Timer_CatchTimer[id].saved_duration;
        }
        else
        {
            SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);
        }
    }

    return(dur);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Read the frequency of the captured signal cycle
 *
 * @param timer_parameter
 * @return
 */
uint32 Timer__GetCaptureFrequcncy(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint32 freq;
    uint16 dur;

    freq = 0;

    dur =Timer__GetCaptureDuration(timer_parameter);

    if (dur != 0)
    {
        freq = TIMER_1US_TICK / dur;
    }

    return(freq);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Read the duty of the captured signal
 *
 * @param timer_parameter
 * @return
 */
uint16 Timer__GetCaptureDuty(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint8 id;
    uint16 duty;
    uint32 dur;

    duty = 0;

    id = GetCatchTimerIndex(timer_parameter);

    if (id != INVALID_MEM_INDEX)
    {
        dur = Timer__GetCaptureDuration(timer_parameter);
        if (dur != 0)
        {
            if(SR_CHECK_SHORT(Timer_CatchTimer[id].saved_pos_dur)  == SRDATA_OK)
            {
                duty = (Timer_CatchTimer[id].saved_pos_dur * 10000)/ dur;
            }
            else
            {
                SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);
            }

        }
    }

    return(duty);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to  start PWM mode
 *
 * @param timer_parameter
 * @param frequency
 * @param duty
 * @return
 */
PASS_FAIL_TYPE Timer__PwmStart(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_PWM_FREQUENCY_TYPE frequency, uint16 duty)
{
    PASS_FAIL_TYPE response = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        TIM_DEF_TYPE *timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;

        if (duty > PWM_MAX_DUTY_CYCLE)
        {
            //The Pwm module shall set the Pwm duty to the maximum if the requested duty is beyond the maximum duty limit.
            duty = PWM_MAX_DUTY_CYCLE;
        }

        //This method sets the frequency for the Timer
        uint32 bus_freq = TimerBusFreq(timer_pt);
        if (frequency < TIMER_FREQUENCY_LOW_LIMIT)
        {   // 1Hz up to 299Hz
            timer_pt->ARR = (uint16)((TIMER_FREQUENCY_LOW_BASE / (uint32)frequency) - 1);
            timer_pt->PSC = (uint16)((bus_freq /  TIMER_FREQUENCY_LOW_BASE)- 1);  //The clock now is 65KHz
        }
        else if (frequency < TIMER_FREQUENCY_MID_LIMIT)
        {  // TIMER_FREQUENCY_LOW_LIMIT up to TIMER_FREQUENCY_MID_LIMIT
            timer_pt->ARR = (uint16)((TIMER_FREQUENCY_MID_BASE / (uint32)frequency)-1);
            timer_pt->PSC = (uint16)((bus_freq /  TIMER_FREQUENCY_MID_BASE) - 1);  //The clock now is 10MHz
        }
        else
        {
            // >= TIMER_FREQUENCY_MID_LIMIT
            timer_pt->ARR = (uint16)(bus_freq / (uint32)frequency) - 1;
            timer_pt->PSC = 0;
        }

        //Up-counter , edge-aligned, clock division = 0 : Tdts = tck_tim
        timer_pt->CR1 = TIM_CR1_CLEAR;
        timer_pt->CNT = TIM_CNT_CLEAR;

        //Configure the duty
        PwmTimSetDuty(timer_parameter, duty);

        //Set the State
        PwmStateConfig(timer_parameter, TIMER_PWM1);
        timer_pt->CR1  |= TIM_CR1_ARPE | TIM_CR1_CEN;   //Enable the auto reload feature, Enable the Timer

        response = PASS;
    }

    return (response);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to set desired duty cycle
 *
 * @param timer_parameter
 * @param duty
 * @return
 */
PASS_FAIL_TYPE Timer__PwmSetDutyCycle(TIMER_PARAMETER_TYPE *timer_parameter, uint16 duty)
{
    PASS_FAIL_TYPE response;

    response = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        if (duty > PWM_MAX_DUTY_CYCLE)
        {
            //The Pwm module shall set the Pwm duty to the maximum if the requested duty is beyond the maximum duty limit.
            duty = PWM_MAX_DUTY_CYCLE;
        }

        PwmTimSetDuty(timer_parameter, duty);   //Two PWM parameters are for PWM polarity configuration & Timer channel configuration respectively

        response = PASS;
    }

    return (response);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to get duty cycle
 *
 * @param timer_parameter
 * @return
 */
uint16 Timer__PwmGetDutyCycle(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint16 duty;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        TIM_DEF_TYPE *timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;
        uint8 ocx = timer_parameter->Channel - 1;

        uint32 ccr = (uint32)timer_pt->CCR[ocx];
        uint32 arr = (uint32)((timer_pt->ARR + 1));
        duty = (ccr * 10000) / arr;
    }

    return (duty);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to get frequency
 * @param tim
 * @param chnl
 * @return
 */
TIMER_PWM_FREQUENCY_TYPE Timer__PwmGetFrequency(TIMER_PARAMETER_TYPE *timer_parameter)
{
    TIMER_PWM_FREQUENCY_TYPE frequency = 0;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        TIM_DEF_TYPE *timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;
        uint32 bus_freq = TimerBusFreq(timer_pt);
        uint32 arr = timer_pt->ARR + 1;
        uint32 psc = timer_pt->PSC + 1;
        frequency = (TIMER_PWM_FREQUENCY_TYPE)(bus_freq /(uint32)(arr * psc));
    }
    return (frequency);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to stop the PWM mode
 * @param timer_parameter
 * @param stop_state
 * @return
 */
BOOL_TYPE Timer__PwmStop(TIMER_PARAMETER_TYPE *timer_parameter, BOOL_TYPE stop_state)
{
    BOOL_TYPE response;
    TIMER_MODE_DEF tri_state;

    response = FALSE;
    tri_state = TIMER_FORCE_INACTIVE;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        if (stop_state == TRUE)
        {
            tri_state = TIMER_FORCE_ACTIVE;
        }

        PwmStateConfig(timer_parameter, tri_state);

        response = TRUE;
    }

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to resume the PWM mode
 *
 * @param timer_parameter
 * @return
 */
PASS_FAIL_TYPE Timer__PwmResume(TIMER_PARAMETER_TYPE *timer_parameter)
{
    PASS_FAIL_TYPE response;

    response = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        PwmStateConfig(timer_parameter, TIMER_PWM1);

        response = PASS;
    }

    return (response);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function initializes the timer
 *
 * @param timer_parameter
 */
static void TimerInit(TIMER_PARAMETER_TYPE *timer_parameter)
{
    switch(timer_parameter->Timer)
    {
        case TIMER1:
            //Enable the clock for TIM1 module
            RCC.APBENR2 |= RCC_APBENR2_TIM1EN;
            break;

        case TIMER2:
            //Enable the clock for TIM2 module
            RCC.APBENR1 |= RCC_APBENR1_TIM2EN;
            break;

        case TIMER3:
            //Enable the clock for TIM3 module
            RCC.APBENR1 |= RCC_APBENR1_TIM3EN;
            break;

        case TIMER4:
            //Enable the clock for TIM3 module
            RCC.APBENR1 |= RCC_APBENR1_TIM4EN;
            break;

        case TIMER14:
            //Enable the clock for TIM14 module
            RCC.APBENR2 |= RCC_APBENR2_TIM14EN;
            break;

        case TIMER15:
            //Enable the clock for TIM15 module
            RCC.APBENR2 |= RCC_APBENR2_TIM15EN;
            break;

        case TIMER16:
            //Enable the clock for TIM16 module
            RCC.APBENR2 |= RCC_APBENR2_TIM16EN;
            break;

        case TIMER17:
            //Enable the clock for TIM17 module
            RCC.APBENR2 |= RCC_APBENR2_TIM17EN;
            break;

        default:
            break;
    }
    if (Gpio__Validate(timer_parameter->Port, timer_parameter->Pin) == PASS)
    {
        GPIO__PIN_COFIG_AF(timer_parameter->Port, timer_parameter->Pin, timer_parameter->Pin_ReMap);
        Gpio__PinConfig(timer_parameter->Port, timer_parameter->Pin, SPECIAL_FUNCTION_PUSH_PULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Validates the requested timer and channels
 * @param tim
 * @param chnl
 * @return
 */
static PASS_FAIL_TYPE TimerValidate(TIMER_TYPE tim, TIMER_CHANNEL_TYPE chnl)
{
    TIMERS_INDEX_DEF tim_index;

    tim_index = TIMER_INDEX[tim];

    if ((tim < (sizeof(TIMER_INDEX)/sizeof(TIMERS_INDEX_DEF))) &&
       (chnl <= TIMER_MAX_TIMER_CHNL_INDEX) &&
       (chnl != 0) &&
       (tim_index != TIMER_NULL))
    {
        return(PASS);
    }
    else
    {
        return(FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method configures the off state.
 * @param tim_pt
 * @param chnl
 * @param mode
 */
static void TimerOutputConfig(TIM_DEF_TYPE * timer_pt, TIMER_CHANNEL_TYPE chnl, TIMER_MODE_DEF mode)
{
    switch(chnl)
    {
         case TIMER_CHANNEL1:
            timer_pt->CCMR1 &= ((uint32)~TIM_CCMR1_OC1M);
            timer_pt->CCMR1 |= mode << TIM_CCMR1_OC1M_POS;
            break;

        case TIMER_CHANNEL2:
            timer_pt->CCMR1  &= ((uint32)~TIM_CCMR1_OC2M);
            timer_pt->CCMR1  |= mode << TIM_CCMR1_OC2M_POS;
            break;

        case TIMER_CHANNEL3:
            timer_pt->CCMR2  &= ((uint32)~TIM_CCMR2_OC3M);
            timer_pt->CCMR2  |= mode << TIM_CCMR2_OC3M_POS;
            break;

        case TIMER_CHANNEL4:
            timer_pt->CCMR2  &= ((uint32)~TIM_CCMR2_OC4M);
            timer_pt->CCMR2  |= mode << TIM_CCMR2_OC4M_POS;
            break;
        default:
            break;
    }

    uint8 chnl_position = (chnl - 1) * 4;
    timer_pt->CCER   &= (~((TIM_CCER_CC1E | TIM_CCER_CC1P) << chnl_position));
    timer_pt->CCER   |= (((TIM_CCER_CC1E) << (chnl - 1) * 4));

    //Main output enable
    timer_pt->BDTR   |= TIM_BDTR_MOE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method configures the off state.
 *
 * @param tim_pt
 * @param chnl
 * @param edge
 */
static void TimerInputCaptureConfig(TIM_DEF_TYPE * timer_pt, TIMER_CHANNEL_TYPE chnl, CAPTURE_EDGE_TYPE edge )
{
    switch(chnl)
    {
         case TIMER_CHANNEL1:
             timer_pt->CCMR1 &= ((uint32)~TIM_CCMR1_OC1M);
             timer_pt->CCMR1  |= TIM_CCMR1_CC1S_0;
            break;

        case TIMER_CHANNEL2:
            timer_pt->CCMR1  &= ((uint32)~TIM_CCMR1_OC2M);
            timer_pt->CCMR1  |= TIM_CCMR1_CC2S_0;
            break;

        case TIMER_CHANNEL3:
            timer_pt->CCMR2  &= ((uint32)~TIM_CCMR2_OC3M);
            timer_pt->CCMR2  |= TIM_CCMR2_CC3S_0;
            break;

        case TIMER_CHANNEL4:
            timer_pt->CCMR2  &= ((uint32)~TIM_CCMR2_OC4M);
            timer_pt->CCMR2  |=TIM_CCMR2_CC4S_0;
            break;
        default:
            break;
    }

    uint8 chnl_position = (chnl - 1) * 4;
    timer_pt->CCER   &= (~((TIM_CCER_CC1E | TIM_CCER_CC1P) << chnl_position));
    timer_pt->CCER   |= ((edge | TIM_CCER_CC1E) << chnl_position);

    //Main output enable
    timer_pt->BDTR   |= TIM_BDTR_MOE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function configures the times tick to 1us
 * @param tim_pt
 * @param chnl
 */
static void Timer1usFreeCntr(TIM_DEF_TYPE * tim_pt, TIMER_CHANNEL_TYPE chnl)
{
    tim_pt->ARR = (uint16)(0xFFFF);
    tim_pt->PSC = (uint16)((TimerBusFreq(tim_pt) / TIMER_1US_TICK) - 1);

    //Up-counter , edge-aligned
    tim_pt->CR1 = TIM_CR1_CLEAR;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function returns the bus frequency connecting to the timer
 * @param tim_pt
 * @return
 */
static uint32 TimerBusFreq(TIM_DEF_TYPE * tim_pt)
{
    uint32 bus_freq;
    bus_freq = Micro__GetClock()->PCLK1_Frequency;

    // 1. If the APB prescaler is set to 1, TIMPCLK frequency is equal to PCLK frequency.
    // 2. Otherwise, the TIMPCLK frequency is set to twice the PCLK frequency.
    if ((RCC.CFGR & RCC_CFGR_PPRE) != 0)
    {
        bus_freq = bus_freq * 2;
    }
    return(bus_freq);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Allocate Catch Timer
 * @param tim
 * @param chnl
 * @return
 */
static PASS_FAIL_TYPE AllocateCatchTimer(TIMER_PARAMETER_TYPE *timer_parameter)
{
    PASS_FAIL_TYPE retval;
    uint8 index;

    retval = FAIL;

    if (GetCatchTimerIndex(timer_parameter) == INVALID_MEM_INDEX)
    {
        for(index = 0; index < TIMER_MAX_CAPTURE_CHNL; index++)
        {
            if (SR_CHECK_SHORT(Timer_CatchTimer[index].id)  == SRDATA_OK)
            {
                if (INVALID_MEM_ID == Timer_CatchTimer[index].id)
                {
                    SR_SET_SHORT(Timer_CatchTimer[index].id,TIMERS_CHANNEL(timer_parameter->Timer, timer_parameter->Channel));
                    retval = PASS;
                    break;
                }
            }
            else
            {
                SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);
            }
        }
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Deallocate catch timer
 * @param tim
 * @param chnl
 */
static void DeAllocateCatchTimer(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint8 index;

    index = GetCatchTimerIndex(timer_parameter);

    if (index != INVALID_MEM_INDEX)
    {
        if (SR_CHECK_SHORT(Timer_CatchTimer[index].id)  == SRDATA_OK)
        {
            SR_SET_SHORT(Timer_CatchTimer[index].id,INVALID_MEM_ID);
        }
        else
        {
            SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Get Catch timer index
 * @param tim
 * @param chnl
 * @return
 */
static uint8 GetCatchTimerIndex(TIMER_PARAMETER_TYPE *timer_parameter)
{
    uint8 index;
    uint16 id;

    id = TIMERS_CHANNEL(timer_parameter->Timer, timer_parameter->Channel);

    for(index = 0; index < TIMER_MAX_CAPTURE_CHNL; index++)
    {
        if(SR_CHECK_SHORT(Timer_CatchTimer[index].id)  == SRDATA_OK)
        {
            if (id == Timer_CatchTimer[index].id)
            {
                break;
            }
        }
        else
        {
            SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer_parameter->Timer, timer_parameter->Channel);

        }
    }

    if (index >= TIMER_MAX_CAPTURE_CHNL)
    {
        index = INVALID_MEM_INDEX;
    }

    return(index);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Manages the capture event
 * @param tim
 * @param chnl
 */
BOOL_TYPE Timer_ProcessCapture(TIMER_TYPE timer, TIMER_CHANNEL_TYPE channel)
{
    volatile uint16 irq_src;
    volatile uint8 index;
    uint32 dur;
    uint16 ccr;
    uint8 aux_uint8;
    TIM_DEF_TYPE *timer_pt;
    BOOL_TYPE assert_interrupt;
    TIMER_CAPTURE_CONFIG_TYPE timer_catch_timer_config;
    TIMER_PARAMETER_TYPE timer_parameter;

    assert_interrupt = FALSE;
    timer_parameter.Timer = timer;
    timer_parameter.Channel = channel;

    timer_pt = TIMER_LIST[TIMER_INDEX[timer]].timer;

    index = GetCatchTimerIndex(&timer_parameter);


    if (index != INVALID_MEM_INDEX)
    {
        channel--;

        ccr = timer_pt->CCR[channel];

        if (SR_CHECK_BYTE(Timer_CatchTimer[index].config.Byte)  == SRDATA_OK)
        {
            if (Timer_CatchTimer[index].config.Bit.initial_edge == 0)
            {
                if(SR_CHECK_LONG(Timer_CatchTimer[index].prev_cnt)  == SRDATA_OK)
                {
                    if (ccr < Timer_CatchTimer[index].prev_cnt)
                    {
                        dur = ccr + (TIMER_MAX_COUNT_VALUE - Timer_CatchTimer[index].prev_cnt);
                    }
                    else
                    {
                        dur = ccr - Timer_CatchTimer[index].prev_cnt;
                    }
                }
                else
                {
                    SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer, channel);
                }

                if (timer_pt->CCER & (TIM_CCER_CC1P << (channel * 4)))
                {
                    if(SR_CHECK_LONG(Timer_CatchTimer[index].pos_dur)  == SRDATA_OK)
                    {
                        //Falling edge event
                        SR_SET_LONG(Timer_CatchTimer[index].pos_dur,dur);
                    }
                    else
                    {
                        SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer, channel);
                    }
                }
                else
                {
                    if(SR_CHECK_LONG(Timer_CatchTimer[index].neg_dur)  == SRDATA_OK)
                    {
                        //Rising edge event
                        SR_SET_LONG(Timer_CatchTimer[index].neg_dur,dur);
                    }
                    else
                    {
                        SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, timer, channel);
                    }
                }
            }
            else
            {
                timer_catch_timer_config.Byte = Timer_CatchTimer[index].config.Byte;
                timer_catch_timer_config.Bit.initial_edge = 0;
                SR_SET_BYTE(Timer_CatchTimer[index].config.Byte,timer_catch_timer_config.Byte);
            }

            SR_SET_LONG(Timer_CatchTimer[index].prev_cnt,ccr);

            timer_pt->CCER ^=  (TIM_CCER_CC1P << (channel * 4));

            timer_catch_timer_config.Byte = Timer_CatchTimer[index].config.Byte;
            aux_uint8 = timer_catch_timer_config.Bit.remain_edge - 1;
            timer_catch_timer_config.Bit.remain_edge = aux_uint8;
            SR_SET_BYTE(Timer_CatchTimer[index].config.Byte,timer_catch_timer_config.Byte);

            if (Timer_CatchTimer[index].config.Bit.remain_edge == 0)
            {
                //save the data after the capture in every continuous cycle
                dur                                     = Timer_CatchTimer[index].pos_dur;
                dur                                     +=Timer_CatchTimer[index].neg_dur;

                SR_SET_LONG(Timer_CatchTimer[index].saved_duration,dur);
                dur = Timer_CatchTimer[index].pos_dur;
                SR_SET_SHORT(Timer_CatchTimer[index].saved_pos_dur,dur);

                if (Timer_CatchTimer[index].config.Bit.cycle_mode == 1)
                {
                    timer_catch_timer_config.Byte                =Timer_CatchTimer[index].config.Byte;
                    timer_catch_timer_config.Bit.remain_edge     = 3;
                    timer_catch_timer_config.Bit.initial_edge    = 1;
                    SR_SET_BYTE(Timer_CatchTimer[index].config.Byte,timer_catch_timer_config.Byte);
                }
                else
                {
                    //Disable the interrupt
                    timer_pt->DIER &= (uint16)(~(TIM_DIER_CC1IE << channel));
                }
                assert_interrupt = TRUE;
            }
        }
        else
        {
            SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION,timer,channel);
        }
    }
    else
    {
        //Disable the interrupt
        timer_pt->DIER &= (uint16)(~(TIM_DIER_CC1IE << channel));
    }

    return(assert_interrupt);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method configures the duty.
 *
 * @param timer_parameter
 * @param duty
 */
static void PwmTimSetDuty(TIMER_PARAMETER_TYPE *timer_parameter, uint16 duty)
{
    uint32 aux;
    TIM_DEF_TYPE *timer_pt;
    uint8 ocx;

    ocx = timer_parameter->Channel - 1;
    timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;

    aux = (uint32)timer_pt->ARR + 1;
    aux *= (uint32)duty;
    aux /= 10000;

    timer_pt->CCR[ocx] = aux;          //Set pulse width
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method configures the off state.
 *
 * @param timer_parameter
 * @param tim_state
 */
static void PwmStateConfig(TIMER_PARAMETER_TYPE *timer_parameter, TIMER_MODE_DEF mode)
{
    uint8 ocx = timer_parameter->Channel - 1;
    TIM_DEF_TYPE *timer_pt = TIMER_LIST[TIMER_INDEX[timer_parameter->Timer]].timer;
    uint32 ccer = 0;

    ccer |= ((timer_parameter->Polarity << 1) |  TIM_CCER_CC1E);  //Polarity, output Enable

    timer_pt->CCER   &= (~(ccer << (ocx << 2)));
    timer_pt->CCER   |= (ccer << (ocx << 2));

    switch(timer_parameter->Channel)
    {
         case TIMER_CHANNEL1:
            timer_pt->CCMR1 &= ((uint32)~TIM_CCMR1_OC1M);
            timer_pt->CCMR1 |= mode << TIM_CCMR1_OC1M_POS;
            break;

        case TIMER_CHANNEL2:
            timer_pt->CCMR1  &= ((uint32)~TIM_CCMR1_OC2M);
            timer_pt->CCMR1  |= mode << TIM_CCMR1_OC2M_POS;
            break;

        case TIMER_CHANNEL3:
            timer_pt->CCMR2  &= ((uint32)~TIM_CCMR2_OC3M);
            timer_pt->CCMR2  |= mode << TIM_CCMR2_OC3M_POS;
            break;

        case TIMER_CHANNEL4:
            timer_pt->CCMR2  &= ((uint32)~TIM_CCMR2_OC4M);
            timer_pt->CCMR2  |= mode << TIM_CCMR2_OC4M_POS;
            break;
        default:
            break;
    }

    timer_pt->BDTR |= TIM_BDTR_MOE;
}

static void OutputCompareStart(TIMER_LIST_TYPE *timer_pt, uint8 channel, TIMER_ONCOMPARE_TYPE action, uint32 new_ccr)
{
    TIM_DEF_TYPE *tim_pt = timer_pt->timer;

    tim_pt->CCR[channel-1] = new_ccr;
    tim_pt->DIER        &= (uint16)(~(1 << channel));

    if ((action == TIMER_ONCOMPARE_PIN_HIGH) ||
        (action == TIMER_ONCOMPARE_PIN_HIGH_INTERRUPT))
    {
        TimerOutputConfig(tim_pt, (TIMER_CHANNEL_TYPE) channel, TIMER_ACTIVE_IN_MATCH);

    }
    else if ((action == TIMER_ONCOMPARE_PIN_LOW) ||
             (action == TIMER_ONCOMPARE_PIN_LOW_INTERRUPT))
    {
        TimerOutputConfig(tim_pt, (TIMER_CHANNEL_TYPE) channel, TIMER_INACTIVE_IN_MATCH);
    }
    else
    {
        TimerOutputConfig(tim_pt, (TIMER_CHANNEL_TYPE) channel, TIMER_FROZEN);
    }
    if ((action == TIMER_ONCOMPARE_PIN_HIGH_INTERRUPT) ||
        (action == TIMER_ONCOMPARE_PIN_LOW_INTERRUPT) ||
        (action == TIMER_ONCOMPARE_INTERRUPT))
    {
        tim_pt->DIER |= (uint16)(1 << channel);

        //Enable the Timer Interrupt
        Micro__NVICEnableIRQ(timer_pt->tim_irq_chnl, timer_pt->premp_prior);
    }

    tim_pt->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;   //Enable the auto reload feature, Enable the Timer

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This module configures the Timer modes into desired modes
 * @ brief- Calling function need to ensure the consistency for passing argument(timer_parameter).
 *
 * @param timer_parameter
 * @return
 */
PASS_FAIL_TYPE Timer__ConfigCheck(TIMER_PARAMETER_TYPE *timer_parameter)
{
    PASS_FAIL_TYPE result = FAIL;

    if (TimerValidate(timer_parameter->Timer, timer_parameter->Channel))
    {
        switch(timer_parameter->Mode)
        {
            case TIMER_MODE_PWM:
                if ((Gpio__Validate(timer_parameter->Port, timer_parameter->Pin) == PASS)&&
                    (Gpio__CheckConfig (timer_parameter->Port,timer_parameter->Pin,SPECIAL_FUNCTION_PUSH_PULL,FALSE) == TRUE))
                {
                    result = PASS;
                }
                break;

            case TIMER_MODE_OUTPUT_COMPARE:
                if ((Gpio__Validate(timer_parameter->Port, timer_parameter->Pin) == PASS)&&
                        (Gpio__CheckConfig (timer_parameter->Port,timer_parameter->Pin,SPECIAL_FUNCTION_PUSH_PULL,FALSE) == TRUE))
                {
                    result = PASS;
                }
                break;

            case TIMER_MODE_INPUT_CAPTURE:
                if ((Gpio__Validate(timer_parameter->Port, timer_parameter->Pin) == PASS)&&
                        (Gpio__CheckConfig (timer_parameter->Port,timer_parameter->Pin,SPECIAL_FUNCTION_PUSH_PULL,FALSE) == TRUE))
                {
                    result = PASS;
                }
                break;

            default:
                break;
        }
        if(result == FAIL)
        {
            //generate an exception if the configuration is mismatched
            SRException__Queue(SREXCEPTION_TIMER_CONFIGURATION_CORRUPTION,timer_parameter->Timer , timer_parameter->Mode);
        }
    }
    else
    {
        //generate an exception if the configuration is mismatched
        SRException__Queue(SREXCEPTION_TIMER_CONFIGURATION_CORRUPTION,timer_parameter->Timer , timer_parameter->Mode);
    }
    return (result);
}
