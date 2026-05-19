/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      The Atod GDM for STM32G0x is a module that implements the
 *              Atod standard API (Application Programming interface for the Atod module) for the STM32G0x family
 *              of microcontrollers. This module provides the interface to the application to read the Digital value
 *              from the Analog signal using ADC Engine.
 *  @details    In addition to regular Atod GDM module, Atod contains all necessary mechanisms to Ensure fully safe
 *              reading of digital values from the analog signal and
 *              meets requirements from Normative IEC 60730-1 (annex H.11).
 *
 *              The STM32G0x implements up to 18 A/D channels. It has 16 External channels and 3 internal channels
 *              ATOD_CH12, ATOD_CH13 & ATOD_CH14 are used as internal channels.
 * <pre>
 *               Following table contains the ADC Channel, Port, Pin associated with ADC module for STM32F0x
 *                --------------------------------------
 *               | ATOD Channel   |  Port    |   Pin    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH0    |   PORTA  |    0     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH1    |   PORTA  |    1     |
 *               | ---------------|----------|----------|
 *               |    ATOD_CH2    |   PORTA  |    2     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH3    |   PORTA  |    3     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH4    |   PORTA  |    4     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH5    |   PORTA  |    5     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH6    |   PORTA  |    6     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH7    |   PORTA  |    7     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH8    |   PORTB  |    0     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH9    |   PORTB  |    1     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH10   |   PORTB  |    2     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH11   |   PORTB  |    10    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH12   |  Internal Channel   |
 *               |                | (Temperature Sensor)|
 *               |----------------|----------|----------|
 *               |    ATOD_CH13   |  Internal Channel   |
 *               |                |     (Vrefint)       |
 *               |----------------|----------|----------|
 *               |    ATOD_CH14   |  Monitoring Vbat    |
 *               |                |     (Vbat/3)        |
 *               |----------------|----------|----------|
 *               |    ATOD_CH15   |   PORTB  |    11    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH16   |   PORTB  |    12    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH17   |   PORTC  |    4     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH18   |   PORTC  |    5     |
 *                -------------------------------------- </pre>
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
#include "SRSystemConfig.h"
#include "uc.h"
#include "Micro.h"
#include "Atod.h"

#include "Atod_defs.h"
#include "Atod_prv.h"



#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

#if (CLASSB_FEATURE == ENABLED)
    #ifndef ATOD_SR_FEATURE_TEST
        #define ATOD_SR_FEATURE_TEST                               ENABLED
    #endif
#else
    #ifndef ATOD_SR_FEATURE_TEST
        #define ATOD_SR_FEATURE_TEST                               DISABLED
    #endif
#endif

//Class B related modules
#if (ATOD_SR_FEATURE_TEST == ENABLED)
    #include "SRException.h"
    #include "SRData.h"
#endif

// Atod Channel check max ADC delta threshold
#ifndef ATOD_CHANNEL_CHECK_MAX_DELTA_ADC
    #define ATOD_CHANNEL_CHECK_MAX_DELTA_ADC   100
#endif

// Pre/Discharge delay for Atod Channel check - unit is NOPs
#ifndef ATOD_CHARGE_DELAY
    #define ATOD_CHARGE_DELAY           0
#endif

#if (ATOD_CHARGE_DELAY < 0)
    #error: "ATOD_CHARGE_DELAY must be equal (for no delay) or higher than 0."
#endif

#if (ATOD_CHARGE_DELAY > 15)
    #error: "ATOD_CHARGE_DELAY must be lower than 15."
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//=====================================================================================================================
/**
 *    @brief  - Maximum de-bounce counter value for invalid reference voltage detection.
 *    @details- When the ADC get stuck then the de-bounce counter starts incrementing and
 *              whenever the counter reaches to the above macro value it throws an SRATOD_STUCK_EXCEPTION.
 *
 *              If the Atod__EngineClassBCheck() function is called from Applience thread which runs at 100ms then
 *              module will throw an SRATOD_STUCK_EXCEPTION exception after 100 * MAX_INTERNAL_REF_DEBOUNCE time if the Atod engine got stuck
 *
 *    @param  - 1/2/3/4
 *
 *    @note -   "#define MAX_INTERNAL_REF_DEBOUNCE          5"
 *              "#define MAX_INTERNAL_REF_DEBOUNCE          10"
 */
//=====================================================================================================================
#ifndef MAX_INTERNAL_REF_DEBOUNCE
    #define MAX_INTERNAL_REF_DEBOUNCE                       5
#endif

//=====================================================================================================================
//Macro Defines
//=====================================================================================================================
#define MAX_ADC_EXTERN_CHANNEL              16
#define MAX_ADC_INTERNAL_CHANNEL            3
#define MAX_ADC_CHANNEL                     (MAX_ADC_EXTERN_CHANNEL + MAX_ADC_INTERNAL_CHANNEL)

/* Fixed timeout value for ADC calibration.                                   */
/* Values defined to be higher than worst cases: maximum ratio between ADC    */
/* and CPU clock frequencies.                                                 */
/* Example of profile low frequency : ADC frequency at 31.25kHz (ADC clock    */
/* source PLL 8MHz, ADC clock prescaler 256), CPU frequency 52MHz.            */
/* Calibration time max = 116 / fADC (refer to datasheet)                     */
/*                      = 193 024 CPU cycles                                  */
#define ADC_CALIBRATION_TIMEOUT             (193024UL)   /*!< ADC calibration time-out value (unit: CPU cycles) */

#define CLEAR_IER_REG                       ((uint32)0x00000000)

#define CLEAR_ADC_CR_REG                    ((uint32)0x00000000)

//! Atod uses the ADC1 engine because the "Internal Power Block(Vrefint)" and Temperature sensor are available with ADC1 engine only
#define  ADC_ENG                            ADC1

//! Internal channel Vrefint
//#define ADC_VREF_CHANNEL                        ATOD_CH13
#define ADC_VREF_CHANNEL                    ATOD_CH13

//! Number of bits used by the A/D converter when measuring the Internal channel Vrefint
#define AD_CONVERTER_BITS                   10

//=====================================================================================================================
/**
 *    @brief  - Defines the channel by channel Programmable sampling time.
 *    @details- Each channel can be sampled with a different sample time.\n
 *              The recommended sampling time for the temperature sensor is 17.1us
 *
 *              Default Sampling time set to SAMPLE_TIME_39_5_CYCLE for more accuracy in atod count.
 *
 *    @param  - SAMPLE_TIME_1_5_CYCLE = 0,  //1.5 ADC Clock Cycle
 *              SAMPLE_TIME_3_5_CYCLE,      //3.5 ADC Clock Cycle
 *              SAMPLE_TIME_7_5_CYCLE,      //7.5 ADC Clock Cycle
 *              SAMPLE_TIME_12_5_CYCLE,     //12.5 ADC Clock Cycle
 *              SAMPLE_TIME_19_5_CYCLE,     //19.5 ADC Clock Cycle
 *              SAMPLE_TIME_39_5_CYCLE,     //39.5 ADC Clock Cycle
 *              SAMPLE_TIME_79_5_CYCLE,     //79.5 ADC Clock Cycle
 *              SAMPLE_TIME_160_5_CYCLE,    //160.5 ADC Clock Cycle
 *
 */
#ifndef ATOD_CH0_SAMPLE_TIME
    #define ATOD_CH0_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH1_SAMPLE_TIME
    #define ATOD_CH1_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH2_SAMPLE_TIME
    #define ATOD_CH2_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH3_SAMPLE_TIME
    #define ATOD_CH3_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH4_SAMPLE_TIME
    #define ATOD_CH4_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH5_SAMPLE_TIME
    #define ATOD_CH5_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH6_SAMPLE_TIME
    #define ATOD_CH6_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH7_SAMPLE_TIME
    #define ATOD_CH7_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH8_SAMPLE_TIME
    #define ATOD_CH8_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH9_SAMPLE_TIME
    #define ATOD_CH9_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH10_SAMPLE_TIME
    #define ATOD_CH10_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH11_SAMPLE_TIME
    #define ATOD_CH11_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH12_SAMPLE_TIME
    #define ATOD_CH12_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH13_SAMPLE_TIME
    #define ATOD_CH13_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH14_SAMPLE_TIME
    #define ATOD_CH14_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH15_SAMPLE_TIME
    #define ATOD_CH15_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH16_SAMPLE_TIME
    #define ATOD_CH16_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH17_SAMPLE_TIME
    #define ATOD_CH17_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifndef ATOD_CH18_SAMPLE_TIME
    #define ATOD_CH18_SAMPLE_TIME                 SAMPLE_TIME_39_5_CYCLE
#endif

#ifdef ATOD_HARDCODED_RESOLUTION
    #if (ATOD_HARDCODED_RESOLUTION == 4)
        #define INTERNAL_REFERENCE_CH_RESOLUTION    >>6
    #elif (ATOD_HARDCODED_RESOLUTION == 8)
        #define INTERNAL_REFERENCE_CH_RESOLUTION    >>2
        result = result>>4;
    #elif (ATOD_HARDCODED_RESOLUTION == 10)
        #define INTERNAL_REFERENCE_CH_RESOLUTION    >>0
        result = result>>2;
    #elif (ATOD_HARDCODED_RESOLUTION == 12)
        #define INTERNAL_REFERENCE_CH_RESOLUTION    <<2
    #elif (ATOD_HARDCODED_RESOLUTION == 14)
        #define INTERNAL_REFERENCE_CH_RESOLUTION    <<4
    #elif (ATOD_HARDCODED_RESOLUTION == 16)
        #define INTERNAL_REFERENCE_CH_RESOLUTION    <<6
    #endif
#else
    #define INTERNAL_REFERENCE_CH_RESOLUTION    >>0
#endif

//! Macro for converting millivolts into ADC count
#define ATOD_INTERNAL_REF_MIN_COUNT             (uint16)(((((uint32)ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS)<<AD_CONVERTER_BITS)/ATOD_ENGINE_REF_IN_MILLIVOLTS) INTERNAL_REFERENCE_CH_RESOLUTION)
#define ATOD_INTERNAL_REF_MAX_COUNT             (uint16)(((((uint32)ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS)<<AD_CONVERTER_BITS)/ATOD_ENGINE_REF_IN_MILLIVOLTS) INTERNAL_REFERENCE_CH_RESOLUTION)

//=====================================================================================================================
//Compiler directive
//=====================================================================================================================
#ifndef ATOD_ENGINE_REF_IN_MILLIVOLTS
    #error: "ATOD_ENGINE_REF_IN_MILLIVOLTS must be defined in the Atod_prv.h file."
#else
    #if((ATOD_ENGINE_REF_IN_MILLIVOLTS < 2000) ||(ATOD_ENGINE_REF_IN_MILLIVOLTS > 3600))
        #error: "ATOD_ENGINE_REF_IN_MILLIVOLTS value should be between 2000(2V) and 3600(3.6V) in the Atod_prv.h file."
    #endif
#endif

#ifndef ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS
    #error: "ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS must be defined in the Atod_prv.h file."
#else
    #if((ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS < 500) || (ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS > 2000))
        #error: "ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS value should be between 500(0.5V) and 2000(2V) in the Atod_prv.h file."
    #endif
#endif

#ifndef ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS
    #error: "ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS must be defined in the Atod_prv.h file."
#else
    #if((ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS < 500) || (ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS > 2000))
        #error: "ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS value should be between 500(0.5V) and 2000(2V) in the Atod_prv.h file."
    #endif
#endif

#ifndef MAX_INTERNAL_REF_DEBOUNCE
    #define MAX_INTERNAL_REF_DEBOUNCE           5
#else
    #if(MAX_INTERNAL_REF_DEBOUNCE>255)
        #error: "MAX_INTERNAL_REF_DEBOUNCE count should not exceed more than 255 in the Atod_prm.h file."
    #endif
#endif

//=====================================================================================================================
//Variable section
//=====================================================================================================================
//! Structure containing the ADC read value, mode and resolution for each channel
PACKED typedef struct
{
    uint16 value;

    #ifndef ATOD_HARDCODED_RESOLUTION
        ATOD_CHANNEL_RESOLUTION_DEF resolution;
    #endif
} ATOD_SYSTEM_DEF;

//! Structure containing the ADC channel port pin
PACKED typedef struct
{
    volatile GPIO_DEF * port_ptr;
    GPIO_PORT_TYPE port_label;
    uint8 pin;
} PORT_STRUCT_DEF;

//! Port configuration for each A/D channel
const PORT_STRUCT_DEF ATOD_PORT[MAX_ADC_CHANNEL] =
{
    {GPIOA, PORTA, 0},      //CH0
    {GPIOA, PORTA, 1},      //CH1
    {GPIOA, PORTA, 2},      //CH2
    {GPIOA, PORTA, 3},      //CH3
    {GPIOA, PORTA, 4},      //CH4
    {GPIOA, PORTA, 5},      //CH5
    {GPIOA, PORTA, 6},      //CH6
    {GPIOA, PORTA, 7},      //CH7
    {GPIOB, PORTB, 0},      //CH8
    {GPIOB, PORTB, 1},      //CH9
    {GPIOB, PORTB, 2},      //CH10
    {GPIOB, PORTB, 10},     //CH11

    {GPIOA, PORT_NULL, 0},  //CH12 - Internal Temperature sensor
    {GPIOA, PORT_NULL, 0},  //CH13 - Internal Vrefint
    {GPIOA, PORT_NULL, 0},  //CH14 - Internal Vbat

    {GPIOB, PORTB, 11},     //CH15
    {GPIOB, PORTB, 12},     //CH16
    {GPIOC, PORTC, 4},      //CH17
    {GPIOC, PORTC, 5}};     //CH18

//! Sample time channel table
const uint8 SAMPLE_TIME_TABLE[MAX_ADC_CHANNEL]  = { ATOD_CH0_SAMPLE_TIME,
                                                    ATOD_CH1_SAMPLE_TIME,
                                                    ATOD_CH2_SAMPLE_TIME,
                                                    ATOD_CH3_SAMPLE_TIME,
                                                    ATOD_CH4_SAMPLE_TIME,
                                                    ATOD_CH5_SAMPLE_TIME,
                                                    ATOD_CH6_SAMPLE_TIME,
                                                    ATOD_CH7_SAMPLE_TIME,
                                                    ATOD_CH8_SAMPLE_TIME,
                                                    ATOD_CH9_SAMPLE_TIME,
                                                    ATOD_CH10_SAMPLE_TIME,
                                                    ATOD_CH11_SAMPLE_TIME,
                                                    ATOD_CH12_SAMPLE_TIME,
                                                    ATOD_CH13_SAMPLE_TIME,
                                                    ATOD_CH14_SAMPLE_TIME,
                                                    ATOD_CH15_SAMPLE_TIME,
                                                    ATOD_CH16_SAMPLE_TIME,
                                                    ATOD_CH17_SAMPLE_TIME,
                                                    ATOD_CH18_SAMPLE_TIME};



//! Class B Data section
#if (ATOD_SR_FEATURE_TEST == ENABLED)
    static uint8 Atod_Exception_Debounce_Count;
    static uint8 Atod_Channel_Debounce_Count[MAX_ADC_CHANNEL];
    static uint8 N_Atod_Exception_Debounce_Count;
    static uint8 N_Atod_Channel_Debounce_Count[MAX_ADC_CHANNEL];
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the ADC Engine.
 *    @details  This method initializes the Atod module variable and resets the entire ADC related variable.
 *              This method is called in the Initialization_Tasks (SRMain_prm.h),
 *              should be called after SRGpio__Initialize().
 *              This must be called before calling any other Atod API’s.
 */
void Atod__EngineInitialize(void)
{
    #if (ATOD_SR_FEATURE_TEST == ENABLED)
        SR_SET_BYTE(Atod_Exception_Debounce_Count, 0);
        SRData__Fill(&Atod_Channel_Debounce_Count, &N_Atod_Channel_Debounce_Count, 0, sizeof(Atod_Channel_Debounce_Count));
    #endif

    uint32 calibrationcounter = 0;
    uint32 calibrationstatus = 0;

    //===============================================================================================================
    /**
     * Clock Configuration
     */

    //Reset the ADC1 engine
    RCC.APBRSTR2 |= RCC_APBRSTR2_ADCRST;
    RCC.APBRSTR2 &= (uint32)(~RCC_APBRSTR2_ADCRST);

    //Enable the ADC device in the APB2 bus
    RCC.APBENR2 |= RCC_APBENR2_ADCEN;

    //System Clock as ADC Clock source
    RCC.CCIPR &= ~RCC_CCIPR_ADCSEL;

    //Maxumim ADC clock shall be not more then 35MHz
    // SYSTEM_CLK(64MHz)/2 to comply
    ADC1_COMMON.CCR = ADC_CLOCK_ASYNC_DIV2;

    //Configure the ADC Clock mode
    //CKMODE[]:00 ADCCLK (Asynchronous clock mode), generated at product level
    ADC_ENG.CFGR2 = 0;

    // Set the ADDIS to Disable the ADC peripheral only if its enabled
    if ((ADC_ENG.CR & ADC_CR_ADEN) != 0)
    {
        ADC_ENG.CR = (uint32)ADC_CR_ADDIS;
    }

    while((ADC_ENG.CR & ADC_CR_ADEN) != 0);


    //===============================================================================================================
    /**
     * Voltage stabilization
     */
    SERVICE_WATCHDOG();

    ADC_ENG.CR |= (uint32)ADC_CR_ADVREGEN;

    /* Delay for ADC stabilization time */
    /* Wait loop initialization and execution */
    /* Note: Variable divided by 2 to compensate partially              */
    /*       CPU processing cycles, scaling in us split to not          */
    /*       exceed 32 bits register capacity and handle low frequency. */
    uint32 wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US / 10UL) * (SYSTEM_CLOCK_64MHZ / (100000UL * 2UL)));
    while (wait_loop_index != 0UL)
    {
      wait_loop_index--;
    }


    //===============================================================================================================
    /**
     * Calibration
     */
    SERVICE_WATCHDOG();

    calibrationcounter = 0;

    // Set the ADC calibration
    ADC_ENG.CR |= (uint32)ADC_CR_ADCAL;

    // Wait until ADC calibration is completed
    do
    {
        calibrationstatus = ADC_ENG.CR & ADC_CR_ADCAL;
        calibrationcounter++;
    } while ((calibrationcounter != ADC_CALIBRATION_TIMEOUT) &&
             (calibrationstatus != 0x00));


    //===============================================================================================================
    /**
     * ADC Enable, configuration
     */

    //Enable the ADC Engine
    ADC_ENG.CR |= ADC_CR_ADEN;

    //Disable all the ADC related Interrupts
    ADC_ENG.IER = CLEAR_IER_REG;

    //SCANDIR, RES[1:0] = 12bit, ALIGN = Right, EXTSEL[2:0], EXTEN[1:0] = Software trigger and CONT = disable
    ADC_ENG.CFGR1 = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method disables the ADC Engine
 *    @details  This could help in saving the power during "low power mode".
 */
void Atod__EngineDisable(void)
{
    //Disables the ADC device from the APB2 bus
    RCC.APBENR2 &= (uint32)(~RCC_APBENR2_ADCEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes a specific channel of the A/D converter
 *    @details  This method configures the specific A/D channel to "INPUT_ANALOG" mode, assigns the resolution to the
 *              requested A/D channel and by default initializes each channel in "ONE SHOT" mode
 *    @param    channel : Defines the specific A/D channel
 *    @param    resolution : Define the A/D resolution
 *    @return   ATOD_RETURN_DEF
 *      @retval ATOD_RETURN_FAIL(0)   : Channel is failed to initialize
 *      @retval ATOD_RETURN_SUCCESS(1): Channel is successfully initialized
 */

uint16 Atod__Convert(ATOD_CHANNEL_DEF channel,ATOD_CHANNEL_RESOLUTION_DEF resolution)
{
    uint16 response = ATOD_INVALID_CONVERSION;
    uint32 analogconfig = 0;
    uint32 wait_loop_index = 0;

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    if (channel <= MAX_ADC_CHANNEL)
    {
        switch(channel)
        {
            case ATOD_CH12:
                //This enables the Internal Temperature sensor
                ADC1_COMMON.CCR |= ADC_CCR_TSEN;

                wait_loop_index = (((LL_ADC_DELAY_TEMPSENSOR_STAB_US / 10UL) * (SYSTEM_CLOCK_64MHZ / (100000UL * 2UL))) + 1UL);
                while (wait_loop_index != 0UL)
                {
                  wait_loop_index--;
                }

                break;

            case ATOD_CH13:
                //This enables the Internal Vrefint
                ADC1_COMMON.CCR |= ADC_CCR_VREFEN;

                wait_loop_index = (((LL_ADC_DELAY_VREFINT_STAB_US / 10UL) * (SYSTEM_CLOCK_64MHZ / (100000UL * 2UL))) + 1UL);
                while (wait_loop_index != 0UL)
                {
                  wait_loop_index--;
                }

                break;

            case ATOD_CH14:
                //This enables the Internal Vbat
                ADC1_COMMON.CCR |= ADC_CCR_VBATEN;

                wait_loop_index = (((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US / 10UL) * (SYSTEM_CLOCK_64MHZ / (100000UL * 2UL))) + 1UL);
                while (wait_loop_index != 0UL)
                {
                  wait_loop_index--;
                }

                break;

            default:
                // write to register_buff configuration bits on their correct positions

                // If ATOD_SR_FEATURE_TEST is DISABLED, configure current Analog channel as Analog input,
                // otherwise it will be configured as digital input, since it doesn't affect analog reading
                // and allows use of pull up/pull down for Channel check
                analogconfig = (((uint32)3) << (ATOD_PORT[channel].pin << 1));
                (ATOD_PORT[channel].port_ptr)->MODER  &= (uint32)(~analogconfig);
                #if (ATOD_SR_FEATURE_TEST == DISABLED)
                    (ATOD_PORT[channel].port_ptr)->MODER |= analogconfig;
                    (ATOD_PORT[channel].port_ptr)->PUPDR &= (uint32)(~analogconfig);
                #endif
                break;
        }


        if (FALSE != (RCC.APBENR2 & RCC_APBENR2_ADCEN))
        {
            #if (ATOD_SR_FEATURE_TEST == ENABLED)
                /**
                 * Atod Engine and Channel MUX Test
                 */
                BOOL_TYPE exception = FALSE;

                switch(channel)
                {
                    case ATOD_CH12:
                        if ((ADC1_COMMON.CCR & ADC_CCR_TSEN) == FALSE)
                        {
                            exception = TRUE;
                        }
                        break;

                    case ATOD_CH13:
                        if ((ADC1_COMMON.CCR & ADC_CCR_VREFEN) == FALSE)
                        {
                            exception = TRUE;
                        }
                        break;

                    case ATOD_CH14:
                        if ((ADC1_COMMON.CCR & ADC_CCR_VBATEN) == FALSE)
                        {
                            exception = TRUE;
                        }
                        break;

                    default:
                        //The specific Atod channel should be configured as digital input
                        if (((ATOD_PORT[channel].port_ptr)->MODER & (((uint32)3) << ((ATOD_PORT[channel].pin) * 2))) != 0)
                        {
                            exception = TRUE;
                        }
                        break;
                }

                if (exception == TRUE)
                {
                    //generate an exception if the ADC gegisters are corrupted
                    SRException__Queue(SREXCEPTION_ATOD_CONFIGURATION_CORRUPTION, channel, 0);
                }
            #endif

            //Save and Disable the Interrupts
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            //Stop the Ongoing ADC conversion
            ADC_ENG.CR |= ADC_CR_ADSTP;

            while((ADC_ENG.CR & ADC_CR_ADSTART) != FALSE);

            //Configure the Sampling time
            ADC_ENG.SMPR = (uint32)SAMPLE_TIME_TABLE[channel];;

            //Enable the Discontinuous mode
            ADC_ENG.CFGR1 |= ADC_CFGR1_DISCEN;
            ADC_ENG.CFGR1 &= (uint32)(~ADC_CFGR1_CONT);

            // Configure the ADC Channel
            ADC_ENG.CHSELR = (uint32)(1 << channel);

            //Start the ADC conversion
            ADC_ENG.CR |= ADC_CR_ADSTART;

            while ((ADC_ENG.ISR & ADC_ISR_EOC) == FALSE);            // wait for the end of the conversion

            response = (uint16)ADC_ENG.DR;

            //Restore Interrupts
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }

        switch (resolution)
        {
            case ATOD_RESOLUTION_04BITS:
                response = response >> 8;
                break;

            case ATOD_RESOLUTION_08BITS:
                response = response >> 4;
                break;

            case ATOD_RESOLUTION_10BITS:
                response = response >> 2;
                break;

            case ATOD_RESOLUTION_14BITS:
                response = response << 2;
                break;

            case ATOD_RESOLUTION_16BITS:
                response = response << 4;
                break;

            default:
                break;
        }
    }

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles internal voltage reference in order to check A/D accuracy and proper multiplexing.
 *    @details  On every call it verifies the ADCx Engine by reading the Internal power block.
 *              This method should be called periodically or as per application requirement to ensure the ADCx Engine is working.
 *    @retval PASS : ADC value read from the "Internal Power Block" is in range
 *    @retval FAIL : ADC value read from the "Internal Power Block" is out of range
 */
#pragma optimize=none
PASS_FAIL_TYPE Atod__EngineClassBCheck(void)
{
    PASS_FAIL_TYPE retval = PASS;

    #if (ATOD_SR_FEATURE_TEST == ENABLED)

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_ATOD_ENGINECLASSB_CHECK_ENTER);
        #endif

        //-----------------------------------------------------------------------------------------------------------------
        // CLASS B Test : Reading the Internal Reference voltage periodically
        //
        //In order to be sure that a valid analog data has been measured, the Atod__EngineClassBCheck() function measures
        //the "Internal Power Block" as reference. This mechanism checks if ADC has proper accuracy
        //and embedded channel multiplexer works properly
        //-----------------------------------------------------------------------------------------------------------------
        //measure internal reference voltage to detect inaccuracy of ADC conversion
        uint16 internal_ref = Atod__Convert(ADC_VREF_CHANNEL, ATOD_RESOLUTION_10BITS);

        //Check for the range
        if( (internal_ref < ATOD_INTERNAL_REF_MIN_COUNT) || (internal_ref > ATOD_INTERNAL_REF_MAX_COUNT) )
        {
            SR_CHECK_BYTE(Atod_Exception_Debounce_Count);

            SR_SET_BYTE(Atod_Exception_Debounce_Count, Atod_Exception_Debounce_Count + 1);

            if(Atod_Exception_Debounce_Count > MAX_INTERNAL_REF_DEBOUNCE)
            {
                SR_SET_BYTE(Atod_Exception_Debounce_Count, 0);

                //generate an exception if the ADC value read from the "Internal Power Block" is out of range
                SRException__Queue(SREXCEPTION_ATOD_STUCK, internal_ref, Atod_Exception_Debounce_Count);
                retval = FAIL;
            }
        }
        else
        {
            SR_SET_BYTE(Atod_Exception_Debounce_Count, 0);
        }

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_ATOD_ENGINECLASSB_CHECK_EXIT);
        #endif
    #endif

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method perform Channel checks required for Class B
 * @details STM32F0x micro does not support this features
 *
 * @param channel Channel to convert
 * @param port Port used to perform Channel check - Not used
 * @param pin Pin used to perform Channel check - Not used
 * @return PASS if test is good, FAIL if there is any failure
 */

#pragma optimize=none
PASS_FAIL_TYPE Atod__ChannelClassBCheck(ATOD_CHANNEL_DEF channel, GPIO_PORT_TYPE port, uint8 pin)
{
    PASS_FAIL_TYPE retval;
    #if (ATOD_SR_FEATURE_TEST == ENABLED)
         retval = FAIL;

        /* Check channel is configured or not */
        if(channel < MAX_ADC_EXTERN_CHANNEL)
        {
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
            PORT_STRUCT_DEF *atod_ptr = (void *) &ATOD_PORT[channel];
            uint8 atod_port_cfg = ((atod_ptr->port_ptr)->MODER >> ((atod_ptr->pin) * 2)) & 3;
            uint16 atod_discharge_value;
            uint16 atod_precharge_value;
            uint16 atod_delta;

            // Check if Gpio pin associated to Atod channel has been already configured as Digital or Analog reading
            if((atod_port_cfg == 0) ||
                (atod_port_cfg == 3))
            {
                /*   Floating pin detection  */
                retval = PASS;

                // Discharge value check - configure it as pull down / connect to GND
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                GPIO__PIN_COFIG_I_PULLDOWN(atod_ptr->port_label, atod_ptr->pin);
                #if(ATOD_CHARGE_DELAY > 0)
                    Micro__DelayNumNops(ATOD_CHARGE_DELAY);
                #endif
                atod_discharge_value =  Atod__Convert(channel ,ATOD_RESOLUTION_12BITS );
                GPIO__PIN_COFIG_I_FLOAT(atod_ptr->port_label, atod_ptr->pin);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

                // Precharge value check - configure it as pull up / connect to Vcc
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                GPIO__PIN_COFIG_I_PULLUP(atod_ptr->port_label, atod_ptr->pin);
                #if(ATOD_CHARGE_DELAY > 0)
                    Micro__DelayNumNops(ATOD_CHARGE_DELAY);
                #endif
                atod_precharge_value =  Atod__Convert(channel ,ATOD_RESOLUTION_12BITS );
                GPIO__PIN_COFIG_I_FLOAT(atod_ptr->port_label, atod_ptr->pin);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

                if (atod_precharge_value > atod_discharge_value)
                {
                    atod_delta = atod_precharge_value - atod_discharge_value;
                }
                else
                {
                    atod_delta = atod_discharge_value - atod_precharge_value;
                }

                if (atod_delta > ATOD_CHANNEL_CHECK_MAX_DELTA_ADC)
                {
                    SRData__UpdateByte(&Atod_Channel_Debounce_Count[channel], &N_Atod_Channel_Debounce_Count[channel], Atod_Channel_Debounce_Count[channel]+1);
                    if (Atod_Channel_Debounce_Count[channel] >= MAX_INTERNAL_REF_DEBOUNCE)
                    {
                        SRData__UpdateByte(&Atod_Channel_Debounce_Count[channel], &N_Atod_Channel_Debounce_Count[channel], 0);
                        //generate an exception if the ADC value read from the "Internal Power Block" is out of range
                        SRException__Queue(SREXCEPTION_ATOD_STUCK, atod_delta, channel);
                        retval = FAIL;
                    }
                }
                else
                {
                    SRData__UpdateByte(&Atod_Channel_Debounce_Count[channel], &N_Atod_Channel_Debounce_Count[channel], 0);
                }
              }
        }
    #else
        retval = PASS;
    #endif

    return (retval);
}
