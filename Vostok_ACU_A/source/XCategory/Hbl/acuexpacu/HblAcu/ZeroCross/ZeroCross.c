/**
 *    @file
 *    @brief    It implements the Zero Cross Detection Logic
 *
 *    @details  This module handles zero cross signal and generates events based on the rise, fall or no edge as
 *              "ZC_FALL_EDGE_EVENT", "ZC_RISE_EDGE_EVENT", "ZC_BLACKOUT_EVENT" & ZC_NO_EVENT, it provides the last event
 *              & current state of zero cross signal, it provides duty cycle & frequency of zero cross signal
 *
 *  $Header: ZeroCross.c 1.6 2015/07/24 11:46:15EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/07/24 11:46:15EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "ZeroCross.h"
#include "ZeroCross_prv.h"
#include "Gpio.h"
#include "Log.h"
#include "Micro.h"
#include "Millis.h"
#include "Timer.h"
#if(ZEROCROSS_POLLING_MODE == DISABLED)
    #include "ExtInterrupt.h"
#else
    #include <string.h>
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

// Polling mode feature - check if FAST_HANDLER_FEATURE is enabled to enable it
#if((ZEROCROSS_POLLING_MODE == ENABLED) && (FAST_HANDLER_FEATURE == DISABLED))
    #error "ZEROCROSS_POLLING_MODE cannot be enabled without FAST_HANDLER_FEATURE"
#endif

// Advance pulse default is 0 - no delay
#ifndef     ZC_ADVANCE_PULSE_DELAY
    #define ZC_ADVANCE_PULSE_DELAY      0
#endif

// ZC Invert Input
#ifndef     ZC_INVERT_PIN_VALUE
    #define ZC_INVERT_PIN_VALUE         DISABLED
#endif

// Debounce Cycle
#ifndef     ZC_DEBOUNCE_CYCLE
    #define ZC_DEBOUNCE_CYCLE           4
#endif

// Timeout
#ifndef     ZC_MAX_CYCLE_TIMEOUT
    #define ZC_MAX_CYCLE_TIMEOUT        4
#endif

#if(ZC_DEBOUNCE_CYCLE < 4)
    #error "ZC_DEBOUNCE_CYCLE minimum value accepted is 4"
#endif

#if(ZC_MAX_CYCLE_TIMEOUT < 2)
    #error "ZC_MAX_CYCLE_TIMEOUT minimum value accepted is 2"
#endif

// Minimum frequency allowed
#ifndef ZC_ALLOWED_MIN_FREQUENCY
    #define ZC_ALLOWED_MIN_FREQUENCY    44
#endif

// Maximum frequency allowed
#ifndef ZC_ALLOWED_MAX_FREQUENCY
    #define ZC_ALLOWED_MAX_FREQUENCY    66
#endif

// Max Delta (in percentage) allowed when ZeroCross signal is detected
#ifndef ZC_THRESHOLD_PERCENTAGE
    #define ZC_THRESHOLD_PERCENTAGE     5
#endif

#if((ZC_THRESHOLD_PERCENTAGE < 0) || (ZC_THRESHOLD_PERCENTAGE > 100))
    #error "ZC_THRESHOLD_PERCENTAGE must be in the range [0..100]"
#endif

// Maximum Cycle time (period) allowed in milliseconds
#define MAX_CYCLE_TIME_MS                   ((ZC_CYCLE_USEC(ZC_ALLOWED_MIN_FREQUENCY)+999U)/1000U)

// Frequency threshold to determine Nominal frequency
#define ZC_NOMINAL_FREQ_THRESHOLD       55

// Initial reference frequency
#ifndef ZC_INITIAL_FREQUENCY
    #define ZC_INITIAL_FREQUENCY        ZC_NOMINAL_FREQ_THRESHOLD
#endif

// Gpio Requests feature is disabled by default (from SystemConfig.h)
#ifndef     ZEROCROSS_GPIO_REQUEST_NUMBER
    #define ZEROCROSS_GPIO_REQUEST_NUMBER        0
#endif

#if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
    #include <string.h>
    // Macro used to get the index from the ID
    #define         GPIO_ID_INDEX(x)                ((uint8) (x) & 0xFFU)
#endif

// Angle Event callback function
//
#ifndef ZC_ANGLE_EVENT
    #define ZC_ANGLE_EVENT(event)
#endif

#ifndef ZC_BLACKOUT_CALLBACK
    #define ZC_BLACKOUT_CALLBACK()
#endif

#if (ZEROCROSS_POLLING_MODE == ENABLED)

    /*
     *  Butterworth filter coefficients - 2nd order filter, cutoff range 35..85 Hz
     *  Available for sampling rates 2KHz (500us), 4KHz (250us), 5KHz (200us), 8KHz (125uz), 10KHz (100us)
     *
     *  Filter coefficients can be customized using
     *  - ZC_BUTTERWORTH_NUM_COEFFICIENTS macro for b0, b1, b2, b3, b4
     *  - ZC_BUTTERWORTH_DEN_COEFFICIENTS macro for a1, a2, a3, a4
     *  Each coefficient should be provided using Q notation; use ZC_FILTER_Q() macro to convert a floating point value.
     */
        #ifndef ZC_BUTTERWORTH_NUM_COEFFICIENTS
            #if(FAST_HANDLER_TIMING_USEC == 500)
                #define ZC_BUTTERWORTH_NUM_COEFFICIENTS     ZC_FILTER_Q(0.005542717210281), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(-0.011085434420561), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(0.005542717210281)
            #elif(FAST_HANDLER_TIMING_USEC == 250)
                #define ZC_BUTTERWORTH_NUM_COEFFICIENTS     ZC_FILTER_Q(0.001460316305528), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(-0.002920632611055), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(0.001460316305528)
            #elif(FAST_HANDLER_TIMING_USEC == 200)
                #define ZC_BUTTERWORTH_NUM_COEFFICIENTS     ZC_FILTER_Q(0.000944691843840), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(-0.001889383687680), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(0.000944691843840)
            #elif(FAST_HANDLER_TIMING_USEC == 125)
                #define ZC_BUTTERWORTH_NUM_COEFFICIENTS     ZC_FILTER_Q(0.000375069616297), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(-0.000750139232594), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(0.000375069616297)
            #elif(FAST_HANDLER_TIMING_USEC == 100)
                #define ZC_BUTTERWORTH_NUM_COEFFICIENTS     ZC_FILTER_Q(0.000241359049042), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(-0.000482718098084), \
                                                            ZC_FILTER_Q(0), \
                                                            ZC_FILTER_Q(0.000241359049042)
            #else
                #error "Check FAST_HANDLER_TIMING_USEC or define ZC_BUTTERWORTH_NUM_COEFFICIENTS and ZC_BUTTERWORTH_DEN_COEFFICIENTS with correct values"
            #endif
        #endif

        #ifndef ZC_BUTTERWORTH_DEN_COEFFICIENTS
            #if(FAST_HANDLER_TIMING_USEC == 500)
                #define ZC_BUTTERWORTH_DEN_COEFFICIENTS     ZC_FILTER_Q(-3.72317913929359), \
                                                            ZC_FILTER_Q(5.2537783810854), \
                                                            ZC_FILTER_Q(-3.33063100327823), \
                                                            ZC_FILTER_Q(0.800802646665707)
            #elif(FAST_HANDLER_TIMING_USEC == 250)
                #define ZC_BUTTERWORTH_DEN_COEFFICIENTS     ZC_FILTER_Q(-3.87476071301398), \
                                                            ZC_FILTER_Q(5.64521833446096), \
                                                            ZC_FILTER_Q(-3.66528100374253), \
                                                            ZC_FILTER_Q(0.894874344616636)
            #elif(FAST_HANDLER_TIMING_USEC == 200)
                #define ZC_BUTTERWORTH_DEN_COEFFICIENTS     ZC_FILTER_Q(-3.902010373028500), \
                                                            ZC_FILTER_Q(5.719417102434887), \
                                                            ZC_FILTER_Q(-3.732361455398672), \
                                                            ZC_FILTER_Q(0.914975834801434)
            #elif(FAST_HANDLER_TIMING_USEC == 125)
                #define ZC_BUTTERWORTH_DEN_COEFFICIENTS     ZC_FILTER_Q(-3.94085843232892), \
                                                            ZC_FILTER_Q(5.82779721126903), \
                                                            ZC_FILTER_Q(-3.83291343985996), \
                                                            ZC_FILTER_Q(0.945977936232281)
            #elif(FAST_HANDLER_TIMING_USEC == 100)
                #define ZC_BUTTERWORTH_DEN_COEFFICIENTS     ZC_FILTER_Q(-3.9532553970607), \
                                                            ZC_FILTER_Q(5.86310685760801), \
                                                            ZC_FILTER_Q(-3.86639378799551), \
                                                            ZC_FILTER_Q(0.956543676511203)
            #else
                #error "Check FAST_HANDLER_TIMING_US or define ZC_BUTTERWORTH_NUM_COEFFICIENTS and ZC_BUTTERWORTH_DEN_COEFFICIENTS with correct values"
            #endif
        #endif


    /*
     *  Butterworth Lobe thresholds
     *
     *  Lobe thresholds can be customized using the ZC_LOBE_THRESHOLDS to provide an array of three elements:
     *  - Cycle Time
     *  - Positive lobe threshold
     *  - Negative lobe threshold
     *
     *  Array shall be ordered with cycle time ascending (from minimum to maximum)
     *
     *  Cycle time is in microseconds; use ZC_CYCLE_USEC() macro to convert a frequency to cycle time.
     *  Each coefficient should be provided using Q notation; use ZC_FILTER_Q() macro to convert a floating point value.
     */
        #ifndef ZC_LOBE_THRESHOLDS
            #define ZC_LOBE_THRESHOLDS      { ZC_CYCLE_USEC(66.0), ZC_FILTER_Q(-0.4361233711), ZC_FILTER_Q(0.4181880951) }, \
                                            { ZC_CYCLE_USEC(64.0), ZC_FILTER_Q(-0.3705024719), ZC_FILTER_Q(0.3562669754) }, \
                                            { ZC_CYCLE_USEC(62.0), ZC_FILTER_Q(-0.3178539276), ZC_FILTER_Q(0.3046236038) }, \
                                            { ZC_CYCLE_USEC(60.0), ZC_FILTER_Q(-0.2598671913), ZC_FILTER_Q(0.2455620766) }, \
                                            { ZC_CYCLE_USEC(58.0), ZC_FILTER_Q(-0.1866121292), ZC_FILTER_Q(0.1807813644) }, \
                                            { ZC_CYCLE_USEC(56.0), ZC_FILTER_Q(-0.1192092896), ZC_FILTER_Q(0.1098413467) }, \
                                            { ZC_CYCLE_USEC(54.0), ZC_FILTER_Q(-0.05960464478), ZC_FILTER_Q(0.0241060257) }, \
                                            { ZC_CYCLE_USEC(52.0), ZC_FILTER_Q(0.0262260437), ZC_FILTER_Q(-0.05272912979) }, \
                                            { ZC_CYCLE_USEC(50.0), ZC_FILTER_Q(0.09965896606), ZC_FILTER_Q(-0.1250019073) }, \
                                            { ZC_CYCLE_USEC(48.0), ZC_FILTER_Q(0.1908402443), ZC_FILTER_Q(-0.1909446716) }, \
                                            { ZC_CYCLE_USEC(46.0), ZC_FILTER_Q(0.2813339233), ZC_FILTER_Q(-0.2839126587) }, \
                                            { ZC_CYCLE_USEC(44.0), ZC_FILTER_Q(0.3636412621), ZC_FILTER_Q(-0.353020668) }
        #endif

    #define         ZEROCROSS_UPDATE_SAMPLES        (500000/FAST_HANDLER_TIMING_USEC)

    typedef struct
    {
        uint32 Cycle_Count;
        sint32 Positive_Lobe_Threshold;
        sint32 Negative_Lobe_Threshold;
    } ZEROCROSS_LOBE_THRESHOLDS_TYPE;

    typedef enum ZEROCROSS_LOBE_TRACKING_ENUM
    {
        ZEROCROSS_NO_LOBE_DETECTED = 0,
        ZEROCROSS_NEG_LOBE_FALLING,
        ZEROCROSS_NEG_LOBE_RISING,
        ZEROCROSS_POS_LOBE_RISING,
        ZEROCROSS_POS_LOBE_FALLING
    } ZEROCROSS_LOBE_TRACKING_TYPE;


    static sint32 const ZEROCROSS_FLT_NUM_COEFFICIENTS[5] = { ZC_BUTTERWORTH_NUM_COEFFICIENTS };
    static sint32 const ZEROCROSS_FLT_DEN_COEFFICIENTS[4] = { ZC_BUTTERWORTH_DEN_COEFFICIENTS };
    static ZEROCROSS_LOBE_THRESHOLDS_TYPE const ZEROCROSS_LOBE_THRESHOLDS[] = { ZC_LOBE_THRESHOLDS };
    #define ZEROCROSS_LOBE_ITEMS        (ELEMENTS_IN_ARRAY(ZEROCROSS_LOBE_THRESHOLDS))

    // Butterworth Filter Input buffer
    static uint8 ZeroCross_Filter_In[4];

    // Butterworth Filter Output buffer
    static sint32 ZeroCross_Filter_Out[4];

    // Timer counter in us (using FAST_HANDLER_TIMING resolution) to determine timing references
    static uint16 ZeroCross_Cycle_Timer;

    // Current Positive lobe threshold according to determined cycle time
    static sint32 ZeroCross_Current_Pos_Lobe;

    // Current Negative lobe threshold according to determined cycle time
    static sint32 ZeroCross_Current_Neg_Lobe;

    // Lobe tracking state machine to perform lobe detection algorithm
    static ZEROCROSS_LOBE_TRACKING_TYPE ZeroCross_Lobe_Tracking;
#else

    // Timeout to consider cycle detection lost and try re-triggering it before blackout event
    #define LOST_CYCLE_TIMEOUT_MS               (ZC_CYCLE_USEC(ZC_ALLOWED_MIN_FREQUENCY)*3U/2000U)

    #ifndef ZC_FILTER_INV_ALPHA_SHIFT
        #define     ZC_FILTER_INV_ALPHA_SHIFT                  8
    #endif

    #ifndef ZC_FILTER_CLEAN_INV_ALPHA_SHIFT
        #define     ZC_FILTER_CLEAN_INV_ALPHA_SHIFT            3
    #endif

    // ZC_FILTER_INV_ALPHA_SHIFT should be equal or higher than ZC_FILTER_CLEAN_INV_ALPHA_SHIFT
    CT_ASSERT(ZC_FILTER_INV_ALPHA_SHIFT >= ZC_FILTER_CLEAN_INV_ALPHA_SHIFT);

    #define     INIT_ACCUMULATOR(value)                        ((value) << (uint8) ZC_FILTER_INV_ALPHA_SHIFT)

    // How many signal events occurred between from last Asynch handler execution
    static uint8 Signal_Event_Occurrences;

    // Time between last Zero Angle event detected and Timer counter reference restarted
    static sint16 Zero_Angle_Reference;

    // Compensation time to synchronize internal Angle events with ZeroCross detected ones
    static sint16 ZeroCross_Cycle_Sync;

    // ZeroCross detected Time through Signal Events in microseconds
    // On 0 degree events it's the detected Cycle time (Period)
    // On 180 degree events it's the detected Half cycle time
    static uint16 Detected_Time;

    // Compensation time due to ZeroCross signal duty cycle
    static sint16 Duty_Cycle_Offset;

    // Reference time to calculate phase shift compensation when ZeroCross signal is detected
    static uint16 ZeroCross_Angle_Event_Time;
#endif

// ZeroCross state
static ZEROCROSS_STATE_TYPE ZeroCross_State;

// Current determined Lobe
static ZEROCROSS_LOBE_TYPE ZeroCross_Lobe;

// Last ZeroCross Event occurred
static ZEROCROSS_EVENT_TYPE ZeroCross_Event;

// Gpio Port and pin associated to ZeroCross signal
static GPIO_PORT_TYPE ZeroCross_Gpio_Port;
static uint8 ZeroCross_Gpio_Pin;

// Nominal frequency determined (50 or 60 Hz)
static uint8 Nominal_Frequency;

// Zero Angle events Sequence counter - it increments when ZeroCross is present (detected signal or during detection)
static uint8 Zero_Events_Sequence;

// ZeroCross determined Cycle Time (Period)
static uint16 ZeroCross_Cycle;

// Current Compensation time between Timer counter and determined Zero event
static sint16 ZeroCross_Cycle_Offset;

// Timer counter Id used for timing reference
static uint8 ZeroCross_Timer_Count_Id;

// Debounce counter for ZeroCross detection
static uint8 ZeroCross_Debounce_Counter;

// Timer used for Blackout detection - refreshed every Zero Angle event determined
static MILLIS_TIMER_TYPE ZeroCross_Blackout;

// Timer parameters used for Timer configuration and Output Compare actions
static TIMER_PARAMETER_TYPE ZeroCross_Pin_Timer;


#if(ZC_THRESHOLD_PERCENTAGE > 0)
    static uint16 ZeroCross_Max_Cycle;
    static uint16 ZeroCross_Min_Cycle;
    #define ZEROCROSS_MAX_CYCLE       (ZeroCross_Max_Cycle)
    #define ZEROCROSS_MIN_CYCLE       (ZeroCross_Min_Cycle)
#else
    #define ZEROCROSS_MAX_CYCLE       ((uint16) ZC_CYCLE_USEC(ZC_ALLOWED_MIN_FREQUENCY))
    #define ZEROCROSS_MIN_CYCLE       ((uint16) ZC_CYCLE_USEC(ZC_ALLOWED_MAX_FREQUENCY))
#endif

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_ZEROCROSS

// -- Private Type Declarations --

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_ZEROCROSS_LOG_MESSAGE_ID_ENUM
{
    INVALID_ZEROCROSS_RANGE = 1,
    DEPRECATED_API_ZEROCROSS_EVENTS = 2,
    DEPRECATED_API_ATOD_SERVICE = 3,
} MODULE_ZEROCROSS_LOG_MESSAGE_ID_TYPE;

#if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
    // Gpio configuration
    PACKED typedef struct
    {
        uint16          Gpio_Id;
        GPIO_PORT_TYPE  Port;
        uint8           Pin;
    } ZEROCROSS_GPIO_CONFIG_TYPE;

    static ZEROCROSS_GPIO_CONFIG_TYPE   ZeroCross_Gpio_Pin_Configuration[ZEROCROSS_GPIO_REQUEST_NUMBER];
    static ZEROCROSS_GPIO_REQUEST_TYPE  ZeroCross_Gpio_Requests[ZEROCROSS_GPIO_REQUEST_NUMBER];
    static ZEROCROSS_GPIO_REQUEST_TYPE  ZeroCross_Gpio_Processing[ZEROCROSS_GPIO_REQUEST_NUMBER];
    static ZEROCROSS_LOBE_TYPE ZeroCross_Last_Lobe;

    static void GpioRequestInitialize(void);
#endif

static void ResetValidRange(void);
static void UpdateValidRange(void);
static void UpdateNominalFrequency(void);

#if (ZEROCROSS_POLLING_MODE == ENABLED)
    static void ZeroCrossButterworthFilter(void);
    static ZEROCROSS_EVENT_TYPE ZeroCrossLobeDetection(void);
    static void UpdateLobeThresholds(void);
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

/**
 * @brief     It initializes the ZeroCross module and variables associated with it
 * @details - It Configures the external interrupt connected to the zero cross hardware and initializes the timer module for generating the angle event
 *
 */
void ZeroCross__Initialize(void)
{
    ZeroCross_Pin_Timer.Timer = TIMER0;
    ZeroCross_Pin_Timer.Channel = TIMER_CHANNEL0;
    ZeroCross_Pin_Timer.Mode = TIMER_MODE_DISABLE;
    ZeroCross_Pin_Timer.Port = PORT_NULL;
    ZeroCross_Pin_Timer.Pin = 0U;
    ZeroCross_Pin_Timer.Pin_ReMap = 0U;
    ZeroCross_Pin_Timer.Polarity = TIMER_POLARITY_ACTIVE_HIGH;
    ZeroCross_Gpio_Port = PORT_NULL;
    ZeroCross_Gpio_Pin = 0U;
    ZeroCross_Timer_Count_Id = TIMER_INVALID_COUNTER_ID;
    ZeroCross_Debounce_Counter = (uint8) ZC_DEBOUNCE_CYCLE;
    ZeroCross_State = ZEROCROSS_INITIALIZE;
    ZeroCross_Lobe = ZEROCROSS_UNKNOWN_LOBE;
    ZeroCross_Event = ZEROCROSS_NO_EVENT;
    ZeroCross_Cycle = (uint16) ZC_CYCLE_USEC(ZC_INITIAL_FREQUENCY);
    Zero_Events_Sequence = 0U;
    ZeroCross_Cycle_Offset = 0;
    Millis__Start(&ZeroCross_Blackout);
    ResetValidRange();
    if(Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        Nominal_Frequency = 0U;
    }

    #if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
        // Initialize Gpio request feature
        GpioRequestInitialize();
        ZeroCross_Last_Lobe = ZEROCROSS_UNKNOWN_LOBE;
    #endif

    #if (ZEROCROSS_POLLING_MODE == ENABLED)
        ZeroCross_Current_Pos_Lobe = 0;
        ZeroCross_Current_Neg_Lobe = 0;
        ZeroCross_Cycle_Timer = 0U;
        ZeroCross_Lobe_Tracking = ZEROCROSS_NO_LOBE_DETECTED;
        memset(ZeroCross_Filter_In, 0, sizeof(ZeroCross_Filter_In));
        memset(ZeroCross_Filter_Out, 0, sizeof(ZeroCross_Filter_Out));
    #else
        Signal_Event_Occurrences = 0U;
        ZeroCross_Angle_Event_Time = 0U;
        Zero_Angle_Reference = 0;
        Duty_Cycle_Offset = 0;
        Detected_Time = 0U;
        ZeroCross_Cycle_Sync = 0;
    #endif
}


/**
 * @brief     It configures the ZeroCross module and variables associated with it
 * @details - It Configures the external interrupt connected to the ZeroCross hardware and initializes the timer module for generating the angle event
 * @param ext_chnl
 * @param ext_port
 * @param ext_pin
 * @param ext_remap
 * @param tim
 * @param tim_chnl
 * @return
 */
PASS_FAIL_TYPE ZeroCross__Config(EXTINT_CHNL_TYPE ext_chnl, GPIO_PORT_TYPE ext_port, uint8 ext_pin, uint8 ext_remap,
    TIMER_TYPE timer, TIMER_CHANNEL_TYPE timer_channel)
{
    PASS_FAIL_TYPE status = FAIL;

    ZeroCross_Pin_Timer.Timer = timer;
    ZeroCross_Pin_Timer.Channel = timer_channel;
    ZeroCross_Pin_Timer.Mode = TIMER_MODE_OUTPUT_COMPARE;

    Gpio__PinConfig(ext_port, ext_pin, INPUT_FLOAT);
    if(Timer__Config(&ZeroCross_Pin_Timer) != FAIL)
    {
        ZeroCross_Timer_Count_Id = Timer__StartCount(&ZeroCross_Pin_Timer);
        ZeroCross_Gpio_Port = ext_port;
        ZeroCross_Gpio_Pin = ext_pin;
        #if (ZEROCROSS_POLLING_MODE == DISABLED)
            (void) ExtInterrupt__Config(ext_chnl, ZeroCross_Gpio_Port, ZeroCross_Gpio_Pin,
                ext_remap, EXTINT_TRG_BOTH);
            (void) Timer__OutputCompareStart(&ZeroCross_Pin_Timer, TIMER_ONCOMPARE_INTERRUPT,(uint32) ZeroCross_Cycle / 4U);
        #endif
        status = PASS;
    }
    return status;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API handles the ZeroCross signal event
 * @details - On ZeroCross event ExtInterrupt module generates the interrupt
 */
void ZeroCross__SignalEvent(void)
{
    #if (ZEROCROSS_POLLING_MODE == DISABLED)
    static uint32 Detected_Time_Accumulator;

        if(ZeroCross_Gpio_Port != PORT_NULL)
        {
            sint32 new_cycle_timer = (sint32) Timer__GetCount(ZeroCross_Timer_Count_Id) + Zero_Angle_Reference;
            if(Signal_Event_Occurrences == 0U)
            {
                Detected_Time_Accumulator = INIT_ACCUMULATOR((uint32) new_cycle_timer);
                Detected_Time = (uint16) new_cycle_timer;
            }
            else
            {
                Detected_Time_Accumulator += (uint32) new_cycle_timer;
                Detected_Time_Accumulator -= Detected_Time;
                Detected_Time = (uint16) (Detected_Time_Accumulator >> ZC_FILTER_INV_ALPHA_SHIFT);
            }
            Signal_Event_Occurrences++;
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -     ZeroCross Asynchronous handler
 *
 * @details -   This function is called in Hbl__AsyncHandler
 *              - It checks timeout and restart Zc_Debounce_Cycle when ZeroCross signal is lost
 *              - It performs ZeroCross Signal detection checks if ExtInterrupt is enabled
 *
 * @return  current ZeroCross event, if ZeroCross has not been configured at all
 *
 */
ZEROCROSS_EVENT_TYPE ZeroCross__AsyncHandler(void)
{
    ZEROCROSS_EVENT_TYPE event = ZEROCROSS_NO_EVENT;

    if(ZeroCross_Gpio_Port != PORT_NULL)
    {
        uint32 blackout_ms = Millis__GetElapsed(&ZeroCross_Blackout);

        #if (ZEROCROSS_POLLING_MODE == DISABLED)
            if(Signal_Event_Occurrences != 0U)
            {
                MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
                uint16 elapsed_time;
                ZEROCROSS_LOBE_TYPE current_lobe;

                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                elapsed_time = Detected_Time;
                current_lobe = ZeroCross_Lobe;
                if(ZeroCross_State != ZEROCROSS_DETECTED_SIGNAL)
                {
                    uint8 pin = (uint8) ZeroCross__GetPinValue();
                    #if(ZC_INVERT_PIN_VALUE == ENABLED)
                        pin ^= 0x01U;
                    #endif
                    current_lobe = (pin == (uint8) TRUE) ? ZEROCROSS_POS_LOBE : ZEROCROSS_NEG_LOBE;
                }
                Detected_Time = 0U;
                Signal_Event_Occurrences = 0U;
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

                /* Blackout condition o no ZeroCross event for more than one cycle time and half related to
                 * the maximum cycle time allowed (LOST_CYCLE_TIMEOUT_MS)
                 */
                if((ZeroCross_State == ZEROCROSS_INITIALIZE) ||
                    (ZeroCross_State == ZEROCROSS_BLACKOUT) ||
                    (blackout_ms > LOST_CYCLE_TIMEOUT_MS))
                {
                    if(current_lobe == ZEROCROSS_POS_LOBE)
                    {
                        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                        ZeroCross_Cycle_Sync = 0;
                        // Update ZeroCross Event reference from last event detected and restart ZeroCross timer
                        Zero_Angle_Reference = (sint16) Timer__ReStartCount(ZeroCross_Timer_Count_Id);
                        Zero_Angle_Reference -= (sint16) elapsed_time;
                        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                        if((ZeroCross_State == ZEROCROSS_INITIALIZE) ||
                            (ZeroCross_State == ZEROCROSS_BLACKOUT))
                        {
                            Millis__Start(&ZeroCross_Blackout);
                            blackout_ms = 0;
                            ZeroCross_State = ZEROCROSS_DETECTING;
                        }
                    }
                }
                else
                {
                    // Detected Positive Lobe time to evaluate compensation due to a ZeroCross signal with duty cycle different than 50%
                    static uint16 ZeroCross_Detected_Positive_Lobe_Time;

                    if(current_lobe == ZEROCROSS_NEG_LOBE)
                    {
                        static uint32 ZeroCross_Detected_Positive_Lobe_Time_Accumulator;

                        if(ZeroCross_Debounce_Counter >= (uint8) ZC_DEBOUNCE_CYCLE)
                        {
                            ZeroCross_Detected_Positive_Lobe_Time_Accumulator = INIT_ACCUMULATOR((uint32) elapsed_time);
                            ZeroCross_Detected_Positive_Lobe_Time = elapsed_time;
                        }
                        else if(elapsed_time < ZC_CYCLE_USEC(ZC_ALLOWED_MAX_FREQUENCY))
                        {
                            uint16 half_timer = elapsed_time;
                            half_timer += (uint16) Duty_Cycle_Offset;
                            half_timer += (uint16) Duty_Cycle_Offset;

                            if((half_timer >= (ZEROCROSS_MIN_CYCLE/2U))
                                && (half_timer <= (ZEROCROSS_MAX_CYCLE/2U)))
                            {
                                sint32 new_accumulator = elapsed_time;
                                new_accumulator -= (sint32) ZeroCross_Detected_Positive_Lobe_Time;
                                new_accumulator *= (sint32) (1U << ((uint8) (ZC_FILTER_INV_ALPHA_SHIFT - ZC_FILTER_CLEAN_INV_ALPHA_SHIFT)));
                                new_accumulator += (sint32) ZeroCross_Detected_Positive_Lobe_Time_Accumulator;

                                ZeroCross_Detected_Positive_Lobe_Time_Accumulator = (uint32) new_accumulator;
                                ZeroCross_Detected_Positive_Lobe_Time = (uint16) (ZeroCross_Detected_Positive_Lobe_Time_Accumulator >> ZC_FILTER_INV_ALPHA_SHIFT);
                            }
                            else
                            {
                                ZeroCross_Detected_Positive_Lobe_Time_Accumulator += elapsed_time;
                                ZeroCross_Detected_Positive_Lobe_Time_Accumulator -= ZeroCross_Detected_Positive_Lobe_Time;
                                ZeroCross_Detected_Positive_Lobe_Time = (uint16) (ZeroCross_Detected_Positive_Lobe_Time_Accumulator >> ZC_FILTER_INV_ALPHA_SHIFT);
                            }
                        }
                    }

                    if((elapsed_time >= (uint16) ZEROCROSS_MIN_CYCLE)
                        && (elapsed_time <= (uint16) ZEROCROSS_MAX_CYCLE))
                    {
                        static uint32 ZeroCross_Cycle_Accumulator;

                        // Update determined ZeroCross cycle
                        if(ZeroCross_Debounce_Counter >= (uint8) ZC_DEBOUNCE_CYCLE)
                        {
                            ZeroCross_Cycle_Accumulator = INIT_ACCUMULATOR((uint32) elapsed_time);
                            ZeroCross_Cycle = elapsed_time;
                            ZeroCross_Debounce_Counter--;
                        }
                        else
                        {
                            sint32 new_value = (sint32) elapsed_time;
                            new_value -= (sint32) ZeroCross_Cycle;
                            new_value *= (sint32) (1U << (uint8) (ZC_FILTER_INV_ALPHA_SHIFT - ZC_FILTER_CLEAN_INV_ALPHA_SHIFT));
                            new_value += (sint32) ZeroCross_Cycle_Accumulator;

                            ZeroCross_Cycle_Accumulator = (uint32) new_value;
                            ZeroCross_Cycle = (uint16) (ZeroCross_Cycle_Accumulator >> ZC_FILTER_INV_ALPHA_SHIFT);
                        }

                        // Update Duty cycle offset
                        if(ZeroCross_Detected_Positive_Lobe_Time > 0U)
                        {
                            sint32 duty_offset;
                            #if(ZC_INVERT_PIN_VALUE == ENABLED)
                                duty_offset = (sint32) ZeroCross_Detected_Positive_Lobe_Time;
                                duty_offset -= (sint32) ZeroCross_Cycle;
                            #else
                                duty_offset = -((sint32) ZeroCross_Detected_Positive_Lobe_Time);
                            #endif
                            duty_offset *= 2;
                            duty_offset += (sint32) ZeroCross_Cycle;
                            duty_offset /= 4;
                            Duty_Cycle_Offset = (sint16) duty_offset;
                        }

                        // Determine the Cycle offset needed to keep correct synchronization with detected signal
                        // Performed only when the ZeroCross Cycle Sync is not in progress on the Angle Event
                        if(ZeroCross_Cycle_Sync == 0)
                        {
                            sint32 new_offset = (sint32) ZeroCross_Angle_Event_Time;
                            new_offset %= (sint32) ZeroCross_Cycle;

                            if((2*new_offset) > (sint32) ZeroCross_Cycle)
                            {
                                new_offset -= (sint32) ZeroCross_Cycle;
                            }
                            #if(ZC_INVERT_PIN_VALUE == ENABLED)
                                new_offset -= Duty_Cycle_Offset;
                            #else
                                new_offset += Duty_Cycle_Offset;
                            #endif
                            new_offset -= ZC_ADVANCE_PULSE_DELAY;
                            ZeroCross_Cycle_Offset = (sint16) new_offset;
                        }
                        else
                        {
                            ZeroCross_Cycle_Offset = 0;
                        }
                        Millis__Start(&ZeroCross_Blackout);
                        blackout_ms = 0;

                        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                        Zero_Angle_Reference += (sint16) Timer__ReStartCount(ZeroCross_Timer_Count_Id);
                        Zero_Angle_Reference -= (sint16) elapsed_time;
                        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                    }
                }
            }
        #endif

        if(ZeroCross_State != ZEROCROSS_BLACKOUT)
        {
            // Determine round up blackout timeout reference
            uint32 blackout_reference = ZeroCross_Cycle;
            blackout_reference *= (uint8) ZC_MAX_CYCLE_TIMEOUT;
            blackout_reference += 999U;
            blackout_reference /= 1000U;

            if(blackout_ms > blackout_reference)
            {
                ZeroCross_Lobe = ZEROCROSS_UNKNOWN_LOBE;
                ZeroCross_State = ZEROCROSS_BLACKOUT;
                ZeroCross_Debounce_Counter = (uint8) ZC_DEBOUNCE_CYCLE;
                Millis__Pause(&ZeroCross_Blackout);
                ResetValidRange();
                #if (ZEROCROSS_POLLING_MODE == ENABLED)
                    ZeroCross_Lobe_Tracking = ZEROCROSS_NO_LOBE_DETECTED;
                #else
                    Signal_Event_Occurrences = 0U;
                    ZeroCross_Angle_Event_Time = 0U;
                    Zero_Angle_Reference = 0;
                    Duty_Cycle_Offset = 0;
                #endif
                ZC_BLACKOUT_CALLBACK();
            }
        }
    }
    else
    {
        // If ZeroCross is not properly configured, ZeroCross simulates angle events.
        switch(ZeroCross_Event)
        {
            case ZEROCROSS_ANGLE_0_EVENT:
                event = ZEROCROSS_ANGLE_90_EVENT;
                break;
            case ZEROCROSS_ANGLE_90_EVENT:
                event = ZEROCROSS_ANGLE_180_EVENT;
                break;
            case ZEROCROSS_ANGLE_180_EVENT:
                event = ZEROCROSS_ANGLE_270_EVENT;
                break;
            case ZEROCROSS_ANGLE_270_EVENT:
            default:
                event = ZEROCROSS_ANGLE_0_EVENT;
                break;
        }
        ZeroCross_Event = event;
    }
    return(event);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -     ZeroCross Fast Handler
 *
 * @details -   This function is called in the Fast Handler execution:
 *              - ZeroCross Polling mode algorithm, if enabled
 *              - Gpio Serivce routine, if enabled
 *
 * @return  current ZeroCross event
 */

ZEROCROSS_EVENT_TYPE ZeroCross__FastHandler(void)
{
    ZEROCROSS_EVENT_TYPE event = ZEROCROSS_NO_EVENT;
    #if (((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0)) \
        || (ZEROCROSS_POLLING_MODE == ENABLED))

        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

        if(ZeroCross_Gpio_Port != PORT_NULL)
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            #if (ZEROCROSS_POLLING_MODE == ENABLED)
                ZeroCrossButterworthFilter();
                UpdateLobeThresholds();
                event = ZeroCrossLobeDetection();
            #endif
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    #endif
    return (event);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API handles the ZeroCross Angle event
 *
 * @details - Hbl OC Handler calls this function on output compare in ExtInterrupt mode
 * @return  current ZeroCross event
 */

ZEROCROSS_EVENT_TYPE ZeroCross__AngleEvent(void)
{
    ZEROCROSS_EVENT_TYPE event = ZEROCROSS_NO_EVENT;

    #if (ZEROCROSS_POLLING_MODE == DISABLED)
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        sint16 offset_threshold = (sint16) ZeroCross_Cycle / 4;
        sint16 offset = 0;

        if(ZeroCross_Gpio_Port != PORT_NULL)
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            switch(ZeroCross_Event)
            {
                case ZEROCROSS_ANGLE_0_EVENT:
                    event = ZEROCROSS_ANGLE_90_EVENT;
                    ZeroCross_Lobe = ZEROCROSS_POS_LOBE;
                    break;
                case ZEROCROSS_ANGLE_90_EVENT:
                    event = ZEROCROSS_ANGLE_180_EVENT;
                    ZeroCross_Lobe = ZEROCROSS_NEG_LOBE;
                    break;
                case ZEROCROSS_ANGLE_180_EVENT:
                    event = ZEROCROSS_ANGLE_270_EVENT;
                    ZeroCross_Lobe = ZEROCROSS_NEG_LOBE;
                    break;
                case ZEROCROSS_ANGLE_270_EVENT:
                default:
                    event = ZEROCROSS_ANGLE_0_EVENT;
                    if(ZeroCross_Debounce_Counter < (uint8) ZC_DEBOUNCE_CYCLE)
                    {
                        sint32 new_cycle_sync = 0;
                        uint32 delta_time = Millis__GetElapsed(&ZeroCross_Blackout);
                        sint32 new_angle_event_time = (sint32) Timer__GetCount(ZeroCross_Timer_Count_Id);
                        new_angle_event_time += (sint32) Zero_Angle_Reference;

                        // Set the current Angle event time - used as timing reference
                        if(new_angle_event_time < ((sint32) ZeroCross_Cycle / 4))
                        {
                            new_angle_event_time += (sint32) ZeroCross_Cycle;
                        }
                        ZeroCross_Angle_Event_Time = (uint16) new_angle_event_time;

                        // ZeroCross Cycle Sync - determine the distance between current Angle 0 event and
                        // the detected one from Signal events, then calculate the compensation value to
                        // synchronize Angle events.
                        // Performed only if last event detect happened within the maximum cycle time allowed.
                        if((ZeroCross_Debounce_Counter > 0U) &&
                            (delta_time < MAX_CYCLE_TIME_MS))
                        {
                            ZeroCross_Debounce_Counter--;
                            if(ZeroCross_Angle_Event_Time >= (ZeroCross_Cycle / 4U))
                            {
                                new_cycle_sync = (sint32) ZeroCross_Angle_Event_Time;
                                if(ZeroCross_Angle_Event_Time >= (ZeroCross_Cycle / 2U))
                                {
                                    new_cycle_sync -= (sint32) ZeroCross_Cycle;
                                }
                                new_cycle_sync -= ZC_ADVANCE_PULSE_DELAY;
                                #if(ZC_INVERT_PIN_VALUE == ENABLED)
                                    new_cycle_sync -= (sint32) Duty_Cycle_Offset;
                                #else
                                    new_cycle_sync += (sint32) Duty_Cycle_Offset;
                                #endif
                                new_cycle_sync /= 4;
                            }
                        }


                        // Debounce completed - update state, ZeroCross valid range and nominal frequency
                        if(ZeroCross_Debounce_Counter == 0U)
                        {
                            UpdateNominalFrequency();
                            UpdateValidRange();
                            if(ZeroCross_State != ZEROCROSS_DETECTED_SIGNAL)
                            {
                                ZeroCross_State = ZEROCROSS_DETECTED_SIGNAL;
                            }
                        }

                        ZeroCross_Cycle_Sync = (sint16) new_cycle_sync;
                        Zero_Events_Sequence++;
                    }
                    ZeroCross_Lobe = ZEROCROSS_POS_LOBE;
                    break;
            }

            // Cycle offset evaluated from Asynch handler and needs to be applied
            // It will be limited to the maximum distance allowed by the valid range
            // and offset decremented to be applied on the next angle event
            if(ZeroCross_Cycle_Offset != 0)
            {
                offset = ZeroCross_Cycle_Offset;
                if(ZeroCross_Cycle_Offset < 0)
                {
                    offset = -offset;
                }
                offset_threshold = (sint16) ZeroCross_Max_Cycle;
                offset_threshold -= (sint16) ZeroCross_Min_Cycle;
                offset_threshold /= 2;
                if(offset > offset_threshold)
                {
                    offset = offset_threshold;
                }
                if(ZeroCross_Cycle_Offset < 0)
                {
                    offset = -offset;
                }
                ZeroCross_Cycle_Offset -= offset;
            }

            ZeroCross_Event = event;
            offset_threshold = (sint16) ZeroCross_Cycle / 4;
            offset = offset_threshold - offset;
            offset -= ZeroCross_Cycle_Sync;
            (void) Timer__OutputCompareStartFromLast(&ZeroCross_Pin_Timer, TIMER_ONCOMPARE_INTERRUPT, (uint32) ((sint32) offset));
            ZC_ANGLE_EVENT(event);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    #endif
    return (event);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the nominal frequency of ZeroCross signal
 *
 *    @details  This method returns the nominal frequency of ZeroCross signal
 *    @return   Nominal frequency of ZeroCross signal being received - 50 or 60
 */
uint8 ZeroCross__GetFrequency(void)
{
    uint8 freq = 0U;

    if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        freq = Nominal_Frequency;
    }
    return(freq);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the determined (filtered) ZeroCross cycle count or period in microseconds
 *
 *    @details  This method returns the determined cycle_count of ZeroCross signal
 *    @return   determined ZeroCross period in microseconds
 */
uint32 ZeroCross__GetCycleCount(void)
{
    uint32 cycle_count = 0U;
    if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        cycle_count = ZeroCross_Cycle;
    }
    return(cycle_count);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the current angle of ZeroCross signal
 *
 *    @details  This function returns the current angle of ZeroCross signal
 *    @return   Current angle of zero cross signal
 */
uint16 ZeroCross__GetAngle(void)
{
    sint32 angle = 0;
    if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        angle = (sint32) ZeroCross__GetMicroSecondsInLobe();
        angle *= 360;
        angle /= (sint32) ZeroCross_Cycle;
        // At this point angle shall not be higher than 180
        if(angle > 180)
        {
            angle = 180;
        }
        if(ZeroCross_Lobe == ZEROCROSS_NEG_LOBE)
        {
            angle += 180;
        }
    }
    return((uint16) angle);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the current ZeroCross sequence counter at every determined ZeroCross 360 event
 *
 *    @details  This returns the current ZeroCross sequence counter at every determined ZeroCross 360 event
 *    @return   Current zero cross sequence counter
 */
uint8 ZeroCross__GetCycleEventsSequence(void)
{
    return(Zero_Events_Sequence);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the current ZeroCross lobe
 *
 *    @details  This function returns the current lobe of ZeroCross signal
 *    @return   Current lobe of zero cross signal
 */
ZEROCROSS_LOBE_TYPE ZeroCross__GetLobe(void)
{
    ZEROCROSS_LOBE_TYPE lobe = ZEROCROSS_UNKNOWN_LOBE;
    if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        lobe = ZeroCross_Lobe;
    }
    return(lobe);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the current ZeroCross state
 *
 *    @details  This function returns the current ZeroCross detection state.
 *    @return   Current ZeroCross detection state
 */
ZEROCROSS_STATE_TYPE ZeroCross__GetState(void)
{
    return(ZeroCross_State);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the current ZeroCross pin value
 *
 *    @details  This function returns the current ZeroCross pin value.
 *    @return   Current ZeroCross pin value
 */
BOOL_TYPE ZeroCross__GetPinValue(void)
{
    return(Gpio__PinRead(ZeroCross_Gpio_Port, ZeroCross_Gpio_Pin));
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the time from last determined Lobe transition event happened
 *
 *    @details  This function returns the time (in microseconds) from last determined Lobe transition event happened
 *              Maximum value provided will be ZeroCross_Cycle/2
 *
 *    @return   elapsed time in microseconds
 */
uint16 ZeroCross__GetMicroSecondsInLobe(void)
{
    uint16 microseconds = 0U;
    if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        sint32 new_microseconds = ((sint32) Timer__GetCount(ZeroCross_Timer_Count_Id) - ZeroCross_Cycle_Offset);
        sint32 half_cycle = ((sint32) ZeroCross_Cycle) / 2;

        #if (ZEROCROSS_POLLING_MODE == DISABLED)
            new_microseconds += Zero_Angle_Reference;
            // Timer__ReStartCount() has not been called yet, so it may exceed ZeroCross_Cycle
            if(new_microseconds > ZeroCross_Cycle)
            {
                new_microseconds %= ZeroCross_Cycle;
            }
            if(ZeroCross_Lobe == ZEROCROSS_NEG_LOBE)
            {
                // Counter restarts on 0 angle event only
                new_microseconds -= half_cycle;
            }
        #endif
        if(new_microseconds > 0)
        {
            // Limit to ZeroCross_Cycle/2
            if(new_microseconds >= half_cycle)
            {
                new_microseconds = half_cycle;
            }
            microseconds = (uint16) new_microseconds;
        }
    }
    return((uint16) microseconds);
}

//=====================================================================================================================
//------------------------------------ Gpio Activity Feature Functions ------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Function to register a Gpio pin for ZeroCross synchronized activation
 *
 * @param port  Gpio port
 * @param pin   Gpio pin
 * @return  gpio_id is the Gpio identifier to be used on ZeroCross Gpio functions as reference
 *                  ZEROCROSS_ID_INVALID value returned if there are more than ZEROCROSS_GPIO_REQUEST_NUMBER registered
 */
uint16 ZeroCross__RegisterGpioActivity(GPIO_PORT_TYPE port, uint8 pin)
{
    uint16 gpio_id = ZEROCROSS_ID_INVALID;
    #if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
        BOOL_TYPE end_loop = FALSE;
        BOOL_TYPE already_registered = FALSE;

        //Handle the Duplicate requests
        for(uint8 gpio_index = 0U; gpio_index < ((uint8) ZEROCROSS_GPIO_REQUEST_NUMBER); gpio_index++)
        {
            ZEROCROSS_GPIO_CONFIG_TYPE *cfg_ptr = &ZeroCross_Gpio_Pin_Configuration[gpio_index];

            if (cfg_ptr->Gpio_Id == ZEROCROSS_ID_INVALID)
            {
                uint8 high_id = (uint8) (Millis__GetFreeRunningCounter() & 0xFFU);
                gpio_id = (uint16) gpio_index | (((uint16) high_id) << 8);
                end_loop = TRUE;
            }
            else if ((port == cfg_ptr->Port) &&
                    (pin == cfg_ptr->Pin))
            {
                already_registered = TRUE;
                end_loop = TRUE;
            }

            if(end_loop)
            {
                break;
            }
        }

        //
        if ((already_registered == FALSE) &&
            (gpio_id != ZEROCROSS_ID_INVALID))
        {
            uint8 index = GPIO_ID_INDEX(gpio_id);
            ZEROCROSS_GPIO_CONFIG_TYPE *cfg_ptr = &ZeroCross_Gpio_Pin_Configuration[index];

            cfg_ptr->Gpio_Id = gpio_id;
            cfg_ptr->Port = port;
            cfg_ptr->Pin = pin;
        }
    #endif

    return(gpio_id);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Function that releases a Gpio pin registered for ZeroCross synchronized activation
 *
 * @param gpio_id   Gpio identifier to be used on ZeroCross Gpio functions as reference
 *
 */
PASS_FAIL_TYPE ZeroCross__ReleaseGpioActivity(uint16 gpio_id)
{
    PASS_FAIL_TYPE result = FAIL;
    #if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
        uint8 index = GPIO_ID_INDEX(gpio_id);
        if ((index < (uint8) ZEROCROSS_GPIO_REQUEST_NUMBER) &&
            (ZeroCross_Gpio_Pin_Configuration[index].Gpio_Id == gpio_id))
        {
            ZeroCross_Gpio_Pin_Configuration[index].Gpio_Id = ZEROCROSS_ID_INVALID;
            result = PASS;
        }
    #endif
    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Function to schedule activation/deactivation of a Gpio pin
 *
 * @param gpio_activity Pointer to a structure with Gpio ID, pin activation value and activation/deactivation angles
 * @return  TRUE if request has been accepted, FALSE otherwise
 *
 */
BOOL_TYPE ZeroCross__RequestGpioActivity(ZEROCROSS_GPIO_REQUEST_TYPE *gpio_activity)
{
    BOOL_TYPE request_accepted = FALSE;
    #if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
        uint8 index = GPIO_ID_INDEX(gpio_activity->Gpio_Id);
        if ((index < (uint8) ZEROCROSS_GPIO_REQUEST_NUMBER) &&
            (ZeroCross_Gpio_Pin_Configuration[index].Gpio_Id == gpio_activity->Gpio_Id))
        {
            ZEROCROSS_GPIO_REQUEST_TYPE *req_ptr = &ZeroCross_Gpio_Requests[index];
            memcpy(req_ptr, gpio_activity, sizeof(ZEROCROSS_GPIO_REQUEST_TYPE));
            request_accepted = TRUE;
        }
    #endif
    return(request_accepted);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Execute the GPIO request service
 *
 */
void ZeroCross__GpioRequestHandler(void)
{
    #if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
    //Zero Cross Event happens
    if (ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        uint16 current_half_angle = ZeroCross__GetAngle();
        if (current_half_angle >= (uint16) ZEROCROSS_HALF_WAVE_ANGLE)
        {
            current_half_angle -= (uint16) ZEROCROSS_HALF_WAVE_ANGLE;
        }

        // When 0 and 180 degree events happen, update all the Gpio requests
        if(ZeroCross_Last_Lobe != ZeroCross_Lobe)
        {
            for (uint8 gpio_req_index = 0U; gpio_req_index < ((uint8) ZEROCROSS_GPIO_REQUEST_NUMBER); gpio_req_index++)
            {
                ZEROCROSS_GPIO_REQUEST_TYPE *processing_ptr = &ZeroCross_Gpio_Processing[gpio_req_index];
                ZEROCROSS_GPIO_REQUEST_TYPE *req_ptr = &ZeroCross_Gpio_Requests[gpio_req_index];
                if (req_ptr->Gpio_Id == ZEROCROSS_ID_INVALID)
                {
                    processing_ptr->Gpio_Id = ZEROCROSS_ID_INVALID;
                }
                else
                {
                    //copy the requests from request to process buffer
                    memcpy((void *)processing_ptr, (void *)req_ptr, sizeof(ZEROCROSS_GPIO_REQUEST_TYPE));
                    req_ptr->Gpio_Id = ZEROCROSS_ID_INVALID;
                    req_ptr->Active_Degree = 0U;
                    req_ptr->Deactive_Degree = 0U;
                    req_ptr->Active_value = FALSE;
                }
            }
            ZeroCross_Last_Lobe = ZeroCross_Lobe;
        }

        for (uint8 gpio_req_index = 0U; gpio_req_index < ((uint8) ZEROCROSS_GPIO_REQUEST_NUMBER); gpio_req_index++)
        {
            ZEROCROSS_GPIO_CONFIG_TYPE *cfg_ptr = &ZeroCross_Gpio_Pin_Configuration[gpio_req_index];
            if(cfg_ptr->Gpio_Id != ZEROCROSS_ID_INVALID)
            {
                ZEROCROSS_GPIO_REQUEST_TYPE *processing_ptr = &ZeroCross_Gpio_Processing[gpio_req_index];

                if (processing_ptr->Gpio_Id == cfg_ptr->Gpio_Id)
                {
                    BOOL_TYPE gpio_value = processing_ptr->Active_value;

                    if ((current_half_angle < processing_ptr->Active_Degree) ||
                    (current_half_angle >= processing_ptr->Deactive_Degree))
                    {
                        gpio_value = gpio_value ? FALSE : TRUE;
                    }

                    Gpio__PinWrite(cfg_ptr->Port, cfg_ptr->Pin, gpio_value);
                }
            }
        }
    }
    #endif
}

//=====================================================================================================================
//-------------------------------------- Deprecated Functions ---------------------------------------------------------
//=====================================================================================================================

/**
 *    @brief    DEPRECATED - It was intended to return last ZeroCross event occurred
 *              ZeroCross__GetLobe() can be used instead to determine current lobe
 *              ZeroCross__GetState() can be used instead to determine if ZeroCross signal is present or not
 *
 *    @return   Last ZeroCross event
 *
 */
ZEROCROSS_EVENT_TYPE ZeroCross__GetLastEvent(void)
{
    ZEROCROSS_EVENT_TYPE event;
    LOG_ADD_EXCEPTION(DEPRECATED_API_ZEROCROSS_EVENTS, 0xFF);
    if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
    {
        event = ZeroCross_Event;
    }
    else if(ZeroCross_State == ZEROCROSS_BLACKOUT)
    {
        event = ZEROCROSS_BLACKOUT_EVENT;
    }
    else
    {
        event = ZEROCROSS_NO_EVENT;
    }

    return(event);
}

/**
 *    @brief    DEPRECATED - It was intended to return time (in us) from last event happened
 *              ZeroCross__GetMicroSecondsInLobe() can be used instead to determine time since last lobe transition
 *                  it provides the same result after 0 and 180 angle events - and before respectively 90 and 270 ones.
 *
 */
uint16 ZeroCross__GetTimeFromLastEvent(void)
{
    LOG_ADD_EXCEPTION(DEPRECATED_API_ZEROCROSS_EVENTS, 0xFE);
    return(0U);
}

//=====================================================================================================================
//----------- Atod sampling Deprecated Functions - Entire feature has been removed from ZeroCross module --------------
//=====================================================================================================================


uint16 ZeroCross__RegisterAtodActivity(ATOD_CHANNEL_DEF channel, ATOD_CHANNEL_RESOLUTION_DEF resolution)
{
    LOG_ADD_EXCEPTION(DEPRECATED_API_ATOD_SERVICE, channel);
    return(ZEROCROSS_ID_INVALID);
}

PASS_FAIL_TYPE ZeroCross__ReleaseAtodActivity(uint16 atod_id)
{
    LOG_ADD_EXCEPTION(DEPRECATED_API_ATOD_SERVICE, 0xFF);
    return(FAIL);
}

BOOL_TYPE ZeroCross__IsAtodReadOverrun(uint16 atod_id)
{
    LOG_ADD_EXCEPTION(DEPRECATED_API_ATOD_SERVICE, 0xFE);
    return(FALSE);
}

uint16 ZeroCross__CopyAtodSamples(uint16 atod_id, uint16 *buffer, uint16 max_items)
{
    LOG_ADD_EXCEPTION(DEPRECATED_API_ATOD_SERVICE, 0xFD);
    return(0U);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief       Function that Updates Nominal Frequency from current Cycle time
 *
 * @details     If no Nominal frequency was detected, it uses ZC_NOMINAL_FREQ_THRESHOLD (55 Hz) as separator between 50 and 60 Hz
 *              Transition between 50 and 60 Hz will happen if current frequency goes above 56 Hz
 *              Transition between 60 and 50 Hz will happen if current frequency goes below 54 Hz
 *
 */
static void UpdateNominalFrequency(void)
{
    // Update ZeroCross nominal frequency with an hysteresis if already determined
    if(Nominal_Frequency != 0U)
    {
        if(ZeroCross_Cycle > ZC_CYCLE_USEC(ZC_NOMINAL_FREQ_THRESHOLD-1))
        {
            Nominal_Frequency = 50U;
        }
        else if(ZeroCross_Cycle < ZC_CYCLE_USEC(ZC_NOMINAL_FREQ_THRESHOLD+1))
        {
            Nominal_Frequency = 60U;
        }
    }
    // Use ZC_NOMINAL_FREQ_THRESHOLD reference on the first run
    else if(ZeroCross_Cycle < ZC_CYCLE_USEC(ZC_NOMINAL_FREQ_THRESHOLD))
    {
        Nominal_Frequency = 60U;
    }
    else
    {
        Nominal_Frequency = 50U;
    }
}

/**
 * @brief       Reset valid range to the full Min-Max range (e.g. 45-65 Hz) if ZC_THRESHOLD_PERCENTAGE is not 0
 *
 */
static void ResetValidRange(void)
{
    #if(ZC_THRESHOLD_PERCENTAGE > 0)
        ZeroCross_Max_Cycle = (uint16) ZC_CYCLE_USEC(ZC_ALLOWED_MIN_FREQUENCY);
        ZeroCross_Min_Cycle = (uint16) ZC_CYCLE_USEC(ZC_ALLOWED_MAX_FREQUENCY);
    #endif
}

/**
 * @brief       Update valid range (Min-Max) applying ZC_THRESHOLD_PERCENTAGE to the determined Cycle time,
 *                  if ZC_THRESHOLD_PERCENTAGE is not 0
 *
 */
static void UpdateValidRange(void)
{
#if(ZC_THRESHOLD_PERCENTAGE > 0)
    ZeroCross_Max_Cycle = (uint16) (((uint32) ZeroCross_Cycle * (100U + (uint8) ZC_THRESHOLD_PERCENTAGE)) / 100U);
    ZeroCross_Min_Cycle = (uint16) (((uint32) ZeroCross_Cycle * (100U - (uint8) ZC_THRESHOLD_PERCENTAGE)) / 100U);
#endif
}


//----------------------------------- ZeroCross Polling Mode private functions ----------------------------------------

#if (ZEROCROSS_POLLING_MODE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Butterworth filter implementation
 *
 */
static void ZeroCrossButterworthFilter(void)
{
    sint64 divider;
    sint32 accumulator;
    uint8 pin_value = (uint8) ZeroCross__GetPinValue();

    #if(ZC_INVERT_PIN_VALUE == ENABLED)
        pin_value ^= 0x01U;
    #endif
    accumulator = ZEROCROSS_FLT_NUM_COEFFICIENTS[0] * (sint32) pin_value;
    accumulator += ZEROCROSS_FLT_NUM_COEFFICIENTS[1] * (sint32) ZeroCross_Filter_In[0];
    accumulator += ZEROCROSS_FLT_NUM_COEFFICIENTS[2] * (sint32) ZeroCross_Filter_In[1];
    accumulator += ZEROCROSS_FLT_NUM_COEFFICIENTS[3] * (sint32) ZeroCross_Filter_In[2];
    accumulator += ZEROCROSS_FLT_NUM_COEFFICIENTS[4] * (sint32) ZeroCross_Filter_In[3];
    divider = (sint64) ZEROCROSS_FLT_DEN_COEFFICIENTS[0] * (sint64) ZeroCross_Filter_Out[0];
    divider += (sint64) ZEROCROSS_FLT_DEN_COEFFICIENTS[1] * (sint64) ZeroCross_Filter_Out[1];
    divider += (sint64) ZEROCROSS_FLT_DEN_COEFFICIENTS[2] * (sint64) ZeroCross_Filter_Out[2];
    divider += (sint64) ZEROCROSS_FLT_DEN_COEFFICIENTS[3] * (sint64) ZeroCross_Filter_Out[3];
    /*
     * Right shift of a signed integer on this formula is required for performance needs - it's faster than divide operation:
     * - Sum of the entire "divider" formula (sum of ZEROCROSS_FLT_DEN_COEFFICIENTS[x] * ZeroCross_Filter_Out[x] then right shift)
     *   fits the sint32 type
     * - Both IAR and GCC compilers perform sign bit extension on right shift operation.
     */
    //lint -e704 -e9027
    divider >>= ((uint8) ZC_Q_DECIMALS);
    //lint +e704 +e9027
    accumulator -= (sint32) divider;

    // Rolls the input buffer
    ZeroCross_Filter_In[3] = ZeroCross_Filter_In[2];
    ZeroCross_Filter_In[2] = ZeroCross_Filter_In[1];
    ZeroCross_Filter_In[1] = ZeroCross_Filter_In[0];
    ZeroCross_Filter_In[0] = pin_value;
    // Rolls the output buffer
    ZeroCross_Filter_Out[3] = ZeroCross_Filter_Out[2];
    ZeroCross_Filter_Out[2] = ZeroCross_Filter_Out[1];
    ZeroCross_Filter_Out[1] = ZeroCross_Filter_Out[0];
    ZeroCross_Filter_Out[0] = accumulator;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Function to perform Lobe detection and generate ZeroCross angle events
 *
 */
static ZEROCROSS_EVENT_TYPE ZeroCrossLobeDetection(void)
{
    uint16 new_cycle_counter = ZeroCross_Cycle_Timer + (uint16) FAST_HANDLER_TIMING_USEC;
    ZEROCROSS_EVENT_TYPE event = ZEROCROSS_NO_EVENT;

    if((ZeroCross_State == ZEROCROSS_DETECTING) ||
        (ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL))
    {
        switch(ZeroCross_Lobe_Tracking)
        {
            case ZEROCROSS_POS_LOBE_FALLING:
                if(ZeroCross_Filter_Out[0] <= ZeroCross_Current_Pos_Lobe)
                {
                    ZeroCross_Lobe_Tracking = (ZeroCross_Current_Pos_Lobe >= ZeroCross_Current_Neg_Lobe) ?
                        ZEROCROSS_NEG_LOBE_RISING : ZEROCROSS_POS_LOBE_RISING;
                }
                break;

            // 180° Determined angle event
            case ZEROCROSS_NEG_LOBE_FALLING:
                if(ZeroCross_Filter_Out[0] <= ZeroCross_Current_Neg_Lobe)
                {
                    if(ZeroCross_Event != ZEROCROSS_ANGLE_180_EVENT)
                    {
                        (void) Timer__ReStartCount(ZeroCross_Timer_Count_Id);
                        ZeroCross_Cycle_Offset = ZC_ADVANCE_PULSE_DELAY;
                        event = ZEROCROSS_ANGLE_180_EVENT;
                    }
                    else
                    {
                        ZeroCross_Cycle_Offset = (((sint16) ZeroCross_Cycle / 2) + ZC_ADVANCE_PULSE_DELAY)
                            - (sint16) new_cycle_counter;
                    }
                    new_cycle_counter = (ZeroCross_Cycle / 2U) + (uint16) ZC_ADVANCE_PULSE_DELAY;
                    ZeroCross_Lobe_Tracking = (ZeroCross_Current_Pos_Lobe >= ZeroCross_Current_Neg_Lobe) ?
                        ZEROCROSS_NEG_LOBE_RISING : ZEROCROSS_POS_LOBE_FALLING;
                }
                break;

            case ZEROCROSS_NEG_LOBE_RISING:
                if(ZeroCross_Filter_Out[0] >= ZeroCross_Current_Neg_Lobe)
                {
                    ZeroCross_Lobe_Tracking = (ZeroCross_Current_Pos_Lobe >= ZeroCross_Current_Neg_Lobe) ?
                        ZEROCROSS_POS_LOBE_RISING : ZEROCROSS_NEG_LOBE_FALLING;
                }
                break;

            // 0° Determined angle event
            case ZEROCROSS_POS_LOBE_RISING:
            default:
                if(ZeroCross_Filter_Out[0] >= ZeroCross_Current_Pos_Lobe)
                {
                    if(ZeroCross_Event != ZEROCROSS_ANGLE_0_EVENT)
                    {
                        (void) Timer__ReStartCount(ZeroCross_Timer_Count_Id);
                        ZeroCross_Cycle_Offset = ZC_ADVANCE_PULSE_DELAY;
                        event = ZEROCROSS_ANGLE_0_EVENT;
                    }
                    else
                    {
                        ZeroCross_Cycle_Offset = (sint16) new_cycle_counter;
                        ZeroCross_Cycle_Offset %= (sint16) ZeroCross_Cycle;
                        ZeroCross_Cycle_Offset += ZC_ADVANCE_PULSE_DELAY;
                    }
                    /* Update ZeroCross_State to ZEROCROSS_DETECTED_SIGNAL when Debounce counter goes to 0
                     * This ensure that State happens on ZEROCROSS_ANGLE_0_EVENT
                     */
                    if((ZeroCross_State != ZEROCROSS_DETECTED_SIGNAL) &&
                        (ZeroCross_Debounce_Counter == 0U))
                    {
                        ZeroCross_State = ZEROCROSS_DETECTED_SIGNAL;
                    }
                    ZeroCross_Lobe_Tracking = (ZeroCross_Current_Pos_Lobe >= ZeroCross_Current_Neg_Lobe) ?
                        ZEROCROSS_POS_LOBE_FALLING : ZEROCROSS_NEG_LOBE_RISING;
                    Zero_Events_Sequence++;
                    Millis__Start(&ZeroCross_Blackout);
                    new_cycle_counter = (uint16) ZC_ADVANCE_PULSE_DELAY;
                }
                break;
        }
    }

    // Generate angle events if no events happened from Lobe detection
    if(event == ZEROCROSS_NO_EVENT)
    {
        if((new_cycle_counter >= ZeroCross_Cycle) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_0_EVENT))
        {
            (void) Timer__ReStartCount(ZeroCross_Timer_Count_Id);
            ZeroCross_Cycle_Offset = 0;
            event = ZEROCROSS_ANGLE_0_EVENT;
            new_cycle_counter %= ZeroCross_Cycle;
        }
        else if((new_cycle_counter >= (ZeroCross_Cycle / 2U)) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_180_EVENT) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_270_EVENT))
        {
            (void) Timer__ReStartCount(ZeroCross_Timer_Count_Id);
            ZeroCross_Cycle_Offset = 0;
            event = ZEROCROSS_ANGLE_180_EVENT;
        }

        if((new_cycle_counter >= (ZeroCross_Cycle - (ZeroCross_Cycle / 4U))) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_270_EVENT))
        {
            (void) Timer__GetCount(ZeroCross_Timer_Count_Id);
            event = ZEROCROSS_ANGLE_270_EVENT;
        }
        else if((new_cycle_counter >= (ZeroCross_Cycle / 4U)) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_90_EVENT) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_180_EVENT) &&
            (ZeroCross_Event != ZEROCROSS_ANGLE_270_EVENT))
        {
            (void) Timer__GetCount(ZeroCross_Timer_Count_Id);
            event = ZEROCROSS_ANGLE_90_EVENT;
        }
    }

    if(event != ZEROCROSS_NO_EVENT)
    {
        ZeroCross_Event = event;
        if(ZeroCross_State == ZEROCROSS_DETECTED_SIGNAL)
        {
            if(event == ZEROCROSS_ANGLE_0_EVENT)
            {
                ZeroCross_Lobe = ZEROCROSS_POS_LOBE;
            }
            else if(event == ZEROCROSS_ANGLE_180_EVENT)
            {
                ZeroCross_Lobe = ZEROCROSS_NEG_LOBE;
            }
        }
        ZC_ANGLE_EVENT(event);
    }
    ZeroCross_Cycle_Timer = new_cycle_counter;
    return(event);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Function that updates Lobe thresholds used for Lobe detection algorithm
 *
 */
static void UpdateLobeThresholds(void)
{
    static uint8 ZeroCross_Cycle_Counter;
    static uint16 ZeroCross_Samples_Counter;
    static uint16 ZeroCross_Cycles_Sample_Counter;
    BOOL_TYPE update_lobe_thresholds = FALSE;

    if(ZeroCross_Debounce_Counter >= (uint8) ZC_DEBOUNCE_CYCLE)
    {
        ZeroCross_Samples_Counter = 0U;
        ZeroCross_Cycle_Counter = 0U;
    }
    else
    {
        ZeroCross_Samples_Counter++;
        ZeroCross_Cycle_Counter++;
    }
    // Detect Cycle time when Filtered signal crosses the 0 value when the signal raises
    if((ZeroCross_Filter_Out[0] >= 0) &&
        (ZeroCross_Filter_Out[1] < 0))
    {
        uint16 new_cycle;

        // First event to detect ZeroCross signal
        if(ZeroCross_Debounce_Counter >= (uint8) ZC_DEBOUNCE_CYCLE)
        {
            Millis__Start(&ZeroCross_Blackout);
            ZeroCross_Cycles_Sample_Counter = 0U;
            ZeroCross_State = ZEROCROSS_DETECTING;
            ZeroCross_Debounce_Counter--;
        }

        new_cycle = ZeroCross_Cycle_Counter * (uint16) FAST_HANDLER_TIMING_USEC;

        if((new_cycle >= ZEROCROSS_MIN_CYCLE) &&
            (new_cycle <= ZEROCROSS_MAX_CYCLE))
        {
            if(ZeroCross_Debounce_Counter > 0U)
            {
                ZeroCross_Cycles_Sample_Counter++;
                ZeroCross_Debounce_Counter--;
                ZeroCross_Cycle = new_cycle;
                if(ZeroCross_Debounce_Counter == 0U)
                {
                    Nominal_Frequency = (ZeroCross_Cycle < ZC_CYCLE_USEC(ZC_NOMINAL_FREQ_THRESHOLD)) ? 60U : 50U;
                    // Set State to Signal detected if ZEROCROSS_ANGLE_0_EVENT event already happened
                    if((ZeroCross_State != ZEROCROSS_DETECTED_SIGNAL) &&
                        (ZeroCross_Event == ZEROCROSS_ANGLE_0_EVENT))
                    {
                        ZeroCross_State = ZEROCROSS_DETECTED_SIGNAL;
                    }
                }
                update_lobe_thresholds = TRUE;
            }
            else
            {
                // Update ZeroCross cycle every second
                ZeroCross_Cycles_Sample_Counter++;
                if(ZeroCross_Samples_Counter >= (uint16) ZEROCROSS_UPDATE_SAMPLES)
                {
                    ZeroCross_Cycle = (uint16) ((ZeroCross_Samples_Counter * ((uint16) FAST_HANDLER_TIMING_USEC))
                        / ZeroCross_Cycles_Sample_Counter);
                    ZeroCross_Cycles_Sample_Counter = 0U;
                    ZeroCross_Samples_Counter = 0U;
                    update_lobe_thresholds = TRUE;
                    UpdateNominalFrequency();
                }
                UpdateValidRange();
            }
        }
        else
        {
            ZeroCross_Cycles_Sample_Counter = 0U;
            ZeroCross_Samples_Counter = 0U;
        }
        ZeroCross_Cycle_Counter = 0U;
    }

    if(update_lobe_thresholds)
    {
        if(ZeroCross_Cycle <= (uint16) ZEROCROSS_LOBE_THRESHOLDS[0].Cycle_Count)
        {
            ZeroCross_Current_Pos_Lobe = ZEROCROSS_LOBE_THRESHOLDS[0].Positive_Lobe_Threshold;
            ZeroCross_Current_Neg_Lobe = ZEROCROSS_LOBE_THRESHOLDS[0].Negative_Lobe_Threshold;
        }
        else if(ZeroCross_Cycle >= (uint16) ZEROCROSS_LOBE_THRESHOLDS[ZEROCROSS_LOBE_ITEMS - 1U].Cycle_Count)
        {
            ZeroCross_Current_Pos_Lobe = ZEROCROSS_LOBE_THRESHOLDS[ZEROCROSS_LOBE_ITEMS - 1U].Positive_Lobe_Threshold;
            ZeroCross_Current_Neg_Lobe = ZEROCROSS_LOBE_THRESHOLDS[ZEROCROSS_LOBE_ITEMS - 1U].Negative_Lobe_Threshold;
        }
        else
        {
            BOOL_TYPE exit_loop = FALSE;

            for(uint8 i=0; i < (ZEROCROSS_LOBE_ITEMS - 1U); i++)
            {
                if(ZeroCross_Cycle == (uint16) ZEROCROSS_LOBE_THRESHOLDS[i].Cycle_Count)
                {
                    ZeroCross_Current_Pos_Lobe = ZEROCROSS_LOBE_THRESHOLDS[i].Positive_Lobe_Threshold;
                    ZeroCross_Current_Neg_Lobe = ZEROCROSS_LOBE_THRESHOLDS[i].Negative_Lobe_Threshold;
                    exit_loop = TRUE;
                }
                else if((uint32) ZeroCross_Cycle < ZEROCROSS_LOBE_THRESHOLDS[i+1U].Cycle_Count)
                {
                    sint32 delta_in_table = (sint32) ZEROCROSS_LOBE_THRESHOLDS[i+1U].Cycle_Count -
                        (sint32) ZEROCROSS_LOBE_THRESHOLDS[i].Cycle_Count;
                    sint32 delta_in = (sint32) ZeroCross_Cycle - (sint32) ZEROCROSS_LOBE_THRESHOLDS[i].Cycle_Count;
                    sint32 delta_out;
                    sint32 value;

                    // Update Positive Lobe
                    value = ZEROCROSS_LOBE_THRESHOLDS[i].Positive_Lobe_Threshold;
                    delta_out = ZEROCROSS_LOBE_THRESHOLDS[i+1U].Positive_Lobe_Threshold -
                        ZEROCROSS_LOBE_THRESHOLDS[i].Positive_Lobe_Threshold;
                    value += (sint32) (((sint64) delta_out * delta_in) / delta_in_table);
                    ZeroCross_Current_Pos_Lobe = value;
                    // Update Negative Lobe
                    value = ZEROCROSS_LOBE_THRESHOLDS[i].Negative_Lobe_Threshold;
                    delta_out = ZEROCROSS_LOBE_THRESHOLDS[i+1U].Negative_Lobe_Threshold -
                        ZEROCROSS_LOBE_THRESHOLDS[i].Negative_Lobe_Threshold;
                    value += (sint32) (((sint64) delta_out * delta_in) / delta_in_table);
                    ZeroCross_Current_Neg_Lobe = value;
                    exit_loop = TRUE;
                }

                if(exit_loop)
                {
                    break;
                }
            }
        }
    }
}

#endif

//----------------------------------- ZeroCross Gpio Request service private functions --------------------------------

#if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Initialize the GPIO request service
 *
 */
static void GpioRequestInitialize(void)
{
    for(uint8 gpio_index = 0U; gpio_index < ((uint8) ZEROCROSS_GPIO_REQUEST_NUMBER); gpio_index++)
    {
        ZeroCross_Gpio_Pin_Configuration[gpio_index].Gpio_Id = ZEROCROSS_ID_INVALID;
        ZeroCross_Gpio_Pin_Configuration[gpio_index].Port = PORT_NULL;
        ZeroCross_Gpio_Pin_Configuration[gpio_index].Pin = 0U;

        ZeroCross_Gpio_Requests[gpio_index].Gpio_Id = ZEROCROSS_ID_INVALID;
        ZeroCross_Gpio_Requests[gpio_index].Active_Degree = 0U;
        ZeroCross_Gpio_Requests[gpio_index].Deactive_Degree = 0U;
        ZeroCross_Gpio_Requests[gpio_index].Active_value = FALSE;

        ZeroCross_Gpio_Processing[gpio_index].Gpio_Id = ZEROCROSS_ID_INVALID;
        ZeroCross_Gpio_Processing[gpio_index].Active_Degree = 0U;
        ZeroCross_Gpio_Processing[gpio_index].Deactive_Degree = 0U;
        ZeroCross_Gpio_Processing[gpio_index].Active_value = FALSE;
    }
}

#endif
