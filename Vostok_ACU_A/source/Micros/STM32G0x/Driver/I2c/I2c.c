/**
 *  @file
 *
 *  @brief    This module implements I2c driver for STM32F0x series of micro-controllers
 *
 *  @details  This module provides the service to the application such as initialization of I2c devices i.e. I2c1 & I2c2,
 *            transmission, reception of serial data via I2C bus
 *
 *            Note -
 *            I2C1 supports Slave mode
 *            This was designed just to support the Hardware in loop testing only, projects shall not use this configurations
 *            By default I2C1 works as Master mode
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
#include "I2c.h"
#include "Gpio.h"
#include "Micro.h"
#include <string.h>

#ifndef I2C1_SLAVE_FEATURE
    #define I2C1_SLAVE_FEATURE                    DISABLED
#endif

#if (I2C1_SLAVE_FEATURE == ENABLED)
    #warning :"SLAVE mode is used only for testing purpose";

    #ifndef I2C_1_SLAVE_MODE_NEW_DATA_RECEIVED
        #define I2C_1_SLAVE_MODE_NEW_DATA_RECEIVED()
    #endif

    #ifndef I2c_1_SLAVE_DATA_TRANSMISSION_COMPLETED
        #define I2c_1_SLAVE_DATA_TRANSMISSION_COMPLETED()
    #endif

    static uint8 I2c1_Slave_TxSize;
#endif

#if (I2C3_FEATURE == ENABLED)
    #error :"I2C3 was not able to test during development as its unavailibility, contact developer email: hanumanth_b_dasar@whirlpoll.com";
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef I2C1_DIGITAL_NOISE_FILTER
    #define I2C1_DIGITAL_NOISE_FILTER    2
#elif (I2C1_DIGITAL_NOISE_FILTER > 16)
    #error:"I2C1_DIGITAL_NOISE_FILTER value should be not more than 16"
#endif

#ifndef I2C2_DIGITAL_NOISE_FILTER
    #define I2C2_DIGITAL_NOISE_FILTER    2
#elif (I2C2_DIGITAL_NOISE_FILTER > 16)
    #error:"I2C2_DIGITAL_NOISE_FILTER value should be not more than 16"
#endif

#ifndef I2C3_DIGITAL_NOISE_FILTER
    #define I2C3_DIGITAL_NOISE_FILTER    2
#elif (I2C3_DIGITAL_NOISE_FILTER > 16)
    #error:"I2C3_DIGITAL_NOISE_FILTER value should be not more than 16"
#endif

//I2c Clock Calculation
#ifndef I2C_BASE_FREQUENCY_AT_50KHZ_100KHZ_SPEED
    #define I2C_BASE_FREQUENCY_AT_50KHZ_100KHZ_SPEED        4000000  //clock period = 250ns
#endif

#ifndef I2C_BASE_FREQUENCY_AT_400KHZ_SPEED
    #define I2C_BASE_FREQUENCY_AT_400KHZ_SPEED              8000000  //clock period = 125ns
#endif

#ifndef I2C_SCLL_FREQUENCY_AT_50KHZ
    #define I2C_SCLL_FREQUENCY_AT_50KHZ                     100000
#endif

#ifndef I2C_SCLH_FREQUENCY_AT_50KHZ
    #define I2C_SCLH_FREQUENCY_AT_50KHZ                     110000
#endif

#ifndef I2C_SCLL_FREQUENCY_AT_100KHZ
    #define I2C_SCLL_FREQUENCY_AT_100KHZ                    200000
#endif

#ifndef I2C_SCLH_FREQUENCY_AT_100KHZ
    #define I2C_SCLH_FREQUENCY_AT_100KHZ                    250000
#endif

#ifndef I2C_SCLL_FREQUENCY_AT_400KHZ
    #define I2C_SCLL_FREQUENCY_AT_400KHZ                    800000
#endif

#ifndef I2C_SCLH_FREQUENCY_AT_400KHZ
    #define I2C_SCLH_FREQUENCY_AT_400KHZ                    2000000
#endif


#ifndef I2C_SDADEL_AT_50_100KHZ
    #define I2C_SDADEL_AT_50_100KHZ                         ((uint32)0x00020000)
#endif

#ifndef I2C_SCLDEL_AT_50_100KHZ
    #define I2C_SCLDEL_AT_50_100KHZ                         ((uint32)0x00400000)
#endif

#ifndef I2C_SDADEL_AT_400KHZ
    #define I2C_SDADEL_AT_400KHZ                            ((uint32)0x00020000)
#endif

#ifndef I2C_SCLDEL_AT_400KHZ
    #define I2C_SCLDEL_AT_400KHZ                            ((uint32)0x00300000)
#endif

#ifndef I2C1_INTERRUPT_PREEMPTION_PRIORITY
    #define I2C1_INTERRUPT_PREEMPTION_PRIORITY    3
#elif (I2C1_INTERRUPT_PREEMPTION_PRIORITY > 3)
    #error :"I2C1_INTERRUPT_PREEMPTION_PRIORITY should be 0 to 3.....0 for High priority and 3 for low priority"
#endif

#ifndef I2C2_INTERRUPT_PREEMPTION_PRIORITY
    #define I2C2_INTERRUPT_PREEMPTION_PRIORITY    3
#elif (I2C2_INTERRUPT_PREEMPTION_PRIORITY > 3)
    #error :"I2C2_INTERRUPT_PREEMPTION_PRIORITY should be 0 to 3.....0 for High priority and 3 for low priority"
#endif

#ifndef I2C3_INTERRUPT_PREEMPTION_PRIORITY
    #define I2C3_INTERRUPT_PREEMPTION_PRIORITY    3
#elif (I2C3_INTERRUPT_PREEMPTION_PRIORITY > 3)
    #error :"I2C3_INTERRUPT_PREEMPTION_PRIORITY should be 0 to 3.....0 for High priority and 3 for low priority"
#endif

#if (I2C1_FEATURE == ENABLED)
    static I2C_ERROR_TYPE I2c1_Ack_Error;
    static uint8 I2c1_Buffer_In[I2C1_BUFFER_IN_SIZE];
    static uint8 I2c1_Buffer_Out[I2C1_BUFFER_OUT_SIZE + 1];
    static uint16 I2c1_TxPointer;
    static uint16 I2c1_RxPointer;
#endif

#if (I2C2_FEATURE == ENABLED)
    static I2C_ERROR_TYPE I2c2_Ack_Error;
    static uint8 I2c2_Buffer_In[I2C2_BUFFER_IN_SIZE];
    static uint8 I2c2_Buffer_Out[I2C2_BUFFER_OUT_SIZE + 1];
    static uint16 I2c2_TxPointer;
    static uint16 I2c2_RxPointer;
#endif

#if (I2C3_FEATURE == ENABLED)
    static I2C_ERROR_TYPE I2c3_Ack_Error;
    static uint8 I2c3_Buffer_In[I2C3_BUFFER_IN_SIZE];
    static uint8 I2c3_Buffer_Out[I2C3_BUFFER_IN_SIZE + 1];
    static uint16 I2c3_TxPointer;
    static uint16 I2c3_RxPointer;
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#define I2C1_START()    {I2C_1.CR2 &= (uint32)(~I2C_CR2_STOP); I2C_1.CR2 |= I2C_CR2_START;}
#define I2C1_STOP()     {I2C_1.CR2 = I2C_CR2_STOP;}

#define I2C2_START()    {I2C_2.CR2 &= (uint32)(~I2C_CR2_STOP); I2C_2.CR2 |= I2C_CR2_START;}
#define I2C2_STOP()     {I2C_2.CR2 = I2C_CR2_STOP;}

#define I2C3_START()    {I2C_3.CR2 &= (uint32)(~I2C_CR2_STOP); I2C_3.CR2 |= I2C_CR2_START;}
#define I2C3_STOP()     {I2C_3.CR2 = I2C_CR2_STOP;}

/**
 * @brief  Configures the specified port-pin into the Alternate function.
 */
#define GPIO__PIN_COFIG_AF(PORT, PORTBIT, AF)       {(*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] = (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] | ((uint32)(AF) << ((uint32)((uint32)PORTBIT & (uint32)0x07) << 2));}

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It initializes a specific I2C module i.e. I2c1 or I2c2
 *
 *    @details  This method first reset I2C module to ensure there is no activity on the I2C bus prior to its initialization,
 *              then it enables the particular I2C module and configures it with user defined speed, also defines which type
 *              of addressing scheme is used i.e. 7-bit or 10-bit addressing
 *    @param    i2c       : I2C module i.e. I2c1 or I2c2 device
 *    @param    speed     : defines the i2c bus speed
 *    @param    addr_type : defines the slave addressing scheme types used 7-bits or 10-bits
 *    @param    i2c_Addr  : defines the slave address used during I2C communication
 *    @return   BOOL_TYPE
 *      @retval   TRUE (1) : It means I2C module is successfully initialized
 *      @retval   FALSE(0) : It means I2C module initialization is unsuccessful
 *
 *    @image html I2c__Initialize.jpg
 */
BOOL_TYPE I2c__Initialize(I2C_ENUM_TYPE i2c,I2C_SPEED_TYPE speed, I2C_ADRESS_TYPE addr_type, uint8 i2c_addr)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        BOOL_TYPE response;
        uint32 apb_clock;
        uint8 prescaler;
        uint8 scll;
        uint8 sclh;
        uint32 sdadel_scl_del;

        response = FALSE;
        prescaler = 0;

        I2c__ResetBus(i2c);                                     //I2C bus is reset i.e. SCL is made low-high & SDA is checked for high

        apb_clock = Micro__GetClock()->SYSCLK_Frequency;        //Peripheral clock is supplied by ABP module

        //Clock configuration
        switch (speed)
        {
            case I2C_50KHZ:
                prescaler = apb_clock / I2C_BASE_FREQUENCY_AT_50KHZ_100KHZ_SPEED;

                scll = (apb_clock/prescaler)/I2C_SCLL_FREQUENCY_AT_50KHZ;
                sclh = (apb_clock/prescaler)/I2C_SCLH_FREQUENCY_AT_50KHZ;
                sdadel_scl_del = (I2C_SDADEL_AT_50_100KHZ | I2C_SCLDEL_AT_50_100KHZ);
                break;

            case I2C_400KHZ:
                prescaler = apb_clock / I2C_BASE_FREQUENCY_AT_400KHZ_SPEED;

                scll = (apb_clock/prescaler)/I2C_SCLL_FREQUENCY_AT_400KHZ;
                sclh = (apb_clock/prescaler)/I2C_SCLH_FREQUENCY_AT_400KHZ;
                sdadel_scl_del = (I2C_SDADEL_AT_400KHZ | I2C_SCLDEL_AT_400KHZ);
                break;

            case I2C_100KHZ:
            default:
                prescaler = apb_clock / I2C_BASE_FREQUENCY_AT_50KHZ_100KHZ_SPEED;

                scll = (apb_clock/prescaler)/I2C_SCLL_FREQUENCY_AT_100KHZ;
                sclh = (apb_clock/prescaler)/I2C_SCLH_FREQUENCY_AT_100KHZ;
                sdadel_scl_del = (I2C_SDADEL_AT_50_100KHZ | I2C_SCLDEL_AT_50_100KHZ);
                break;
        }

        prescaler--;
        scll--;
        sclh--;

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                I2c1_Ack_Error = I2C_ERROR_NONE;

                //System clock (SYSCLK) selected as I2c1 clock
                RCC.CCIPR &= ~RCC_CCIPR_I2C1SEL;
                RCC.CCIPR |= LL_RCC_I2C1_CLKSOURCE_SYSCLK;

                //Enable the clock for I2c1 module
                RCC.APBENR1 |= RCC_APBENR1_I2C1EN;

                //Reset the I2c1
                RCC.APBRSTR1 |= RCC_APBENR1_I2C1EN;
                RCC.APBRSTR1 &= (uint32)(~RCC_APBENR1_I2C1EN);

                I2c__Disable(i2c);                                      //I2C module is disabled

                GPIO__PIN_COFIG_AF(I2C_1_SDA_PORT, I2C_1_SDA_PORTBIT, I2C_1_SDA__AF);
                GPIO__PIN_COFIG_SF_OPENDRAIN(I2C_1_SDA_PORT, I2C_1_SDA_PORTBIT);

                GPIO__PIN_COFIG_AF(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT, I2C_1_SCL__AF);
                GPIO__PIN_COFIG_SF_OPENDRAIN(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT);

                I2C_1.TIMINGR = (((prescaler & 0xf) << 28) | sdadel_scl_del | ((sclh & 0xff) << 8) | (scll & 0xff));

                //Configure the Digital filter
                I2C_1.CR1 |= (uint32)((I2C1_DIGITAL_NOISE_FILTER << 8) & I2C_CR1_DFN);

                #if (I2C1_SLAVE_FEATURE == DISABLED)
                    //Configure the Device address
                    I2C_1.CR2 |= (uint32)i2c_addr;
                #else
                    //Slave Byte Control
                    I2C_1.CR1 |= I2C_CR1_SBC;

                    I2C_1.OAR1 = (i2c_addr & 0xFE) | I2C_OAR1_OA1EN;
                #endif

                //Set the interrupt
                Micro__NVICEnableIRQ (I2C1_IRQ_CHANNEL, I2C1_INTERRUPT_PREEMPTION_PRIORITY);

                response = TRUE;
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                I2c2_Ack_Error = I2C_ERROR_NONE;

                //System clock (SYSCLK) selected as I2c1 clock
                RCC.CCIPR &= ~RCC_CCIPR_I2C2SEL;
                RCC.CCIPR |= LL_RCC_I2C2_CLKSOURCE_SYSCLK;

                //Enable the clock for I2c1 module
                RCC.APBENR1 |= RCC_APBENR1_I2C2EN;

                //Reset the I2c1
                RCC.APBRSTR1 |= RCC_APBENR1_I2C2EN;
                RCC.APBRSTR1 &= (uint32)(~RCC_APBENR1_I2C2EN);

                I2c__Disable(i2c);                                      //I2C module is disabled

                GPIO__PIN_COFIG_AF(I2C_2_SDA_PORT, I2C_2_SDA_PORTBIT, I2C_2_SDA__AF);
                GPIO__PIN_COFIG_SF_OPENDRAIN(I2C_2_SDA_PORT, I2C_2_SDA_PORTBIT);

                GPIO__PIN_COFIG_AF(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT, I2C_2_SCL__AF);
                GPIO__PIN_COFIG_SF_OPENDRAIN(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT);

                I2C_2.TIMINGR = (((prescaler & 0xf) << 28) | sdadel_scl_del | ((sclh & 0xff) << 8) | (scll & 0xff));

                //Configure the Digital filter
                I2C_2.CR1 |= (uint32)((I2C2_DIGITAL_NOISE_FILTER << 8) & I2C_CR1_DFN);

                //Configure the Device address
                I2C_2.CR2 |= (uint32)i2c_addr;

                //Set the interrupt
                Micro__NVICEnableIRQ (I2C2_3_IRQ_CHANNEL, I2C2_3_INTERRUPT_PREEMPTION_PRIORITY);

                response = TRUE;
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                I2c3_Ack_Error = I2C_ERROR_NONE;

                //Enable the clock for I2c1 module
                RCC.APBENR1 |= RCC_APBENR1_I2C3EN;

                //Reset the I2c1
                RCC.APBRSTR1 |= RCC_APBENR1_I2C3EN;
                RCC.APBRSTR1 &= (uint32)(~RCC_APBENR1_I2C3EN);

                I2c__Disable(i2c);                                      //I2C module is disabled

                GPIO__PIN_COFIG_AF(I2C_3_SDA_PORT, I2C_3_SDA_PORTBIT, I2C_3_SDA__AF);
                GPIO__PIN_COFIG_SF_OPENDRAIN(I2C_3_SDA_PORT, I2C_3_SDA_PORTBIT);

                GPIO__PIN_COFIG_AF(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT, I2C_3_SCL__AF);
                GPIO__PIN_COFIG_SF_OPENDRAIN(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT);

                I2C_3.TIMINGR = (((prescaler & 0xf) << 28) | sdadel_scl_del | ((sclh & 0xff) << 8) | (scll & 0xff));

                //Configure the Digital filter
                I2C_3.CR1 |= (uint32)((I2C3_DIGITAL_NOISE_FILTER << 8) & I2C_CR1_DFN);

                //Configure the Device address
                I2C_3.CR2 |= (uint32)i2c_addr;

                //Set the interrupt
                Micro__NVICEnableIRQ (I2C2_3_IRQ_CHANNEL, I2C2_3_INTERRUPT_PREEMPTION_PRIORITY);

                response = TRUE;
            }
        #endif

        I2c__Enable(i2c);

        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It clear the state machines which are meant for I2C communication
 *    @details  A software reset can be performed by disabling the I2c peripheral for at least 3 APB clock cycles and
 *              then turning it Enabled
 *    @param    i2c : I2c module i.e. I2c1 or I2c2 device
 *
 *    @image html I2c__Clear.jpg
 */
void I2c__Clear(I2C_ENUM_TYPE i2c)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                // A software reset can be performed by disabling the I2c peripheral for at least 3 APB clock cycles and
                // then turning it Enabled
                // Turn-off the I2c1
                I2C_1.CR1 &= (uint32)(~I2C_CR1_PE);

                //Clear the NBYTES
                I2C_1.CR2 &= (uint32)(~I2C_CR2_NBYTES);

                I2c1_TxPointer = 0;
                I2c1_RxPointer = 0;
                I2c1_Ack_Error = I2C_ERROR_NONE;

                //Enable the I2c2
                I2C_1.CR1 |= I2C_CR1_PE;
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                // A software reset can be performed by disabling the I2c peripheral for at least 3 APB clock cycles and
                // then turning it Enabled
                // Turn-off the I2c1
                I2C_2.CR1 &= (uint32)(~I2C_CR1_PE);

                //Clear the NBYTES
                I2C_2.CR2 &= (uint32)(~I2C_CR2_NBYTES);

                I2c2_TxPointer = 0;
                I2c2_RxPointer = 0;
                I2c2_Ack_Error = I2C_ERROR_NONE;

                //Enable the I2c2
                I2C_2.CR1 |= I2C_CR1_PE;
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                // A software reset can be performed by disabling the I2c peripheral for at least 3 APB clock cycles and
                // then turning it Enabled
                // Turn-off the I2c1
                I2C_3.CR1 &= (uint32)(~I2C_CR1_PE);

                //Clear the NBYTES
                I2C_3.CR2 &= (uint32)(~I2C_CR2_NBYTES);

                I2c3_TxPointer = 0;
                I2c3_RxPointer = 0;
                I2c3_Ack_Error = I2C_ERROR_NONE;

                //Enable the I2c3
                I2C_3.CR1 |= I2C_CR1_PE;
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It is used to reset I2C bus
 *
 *    @details  This method performs the I2C bus reset operation in two cases such as prior to I2C module initialization
 *              and in case any bus error occurs during I2C bus communication
 *    @param    i2c : I2C modules i.e. I2c1 or I2c2 device
 *    @return   BOOL_TYPE
 *      @retval   TRUE (1) : It indicates I2C bus is reset operation is successful
 *      @retval   FALSE(0) : It indicates I2C bus is reset operation is unsuccessful
 *    @image html I2c__ResetBus.jpg
 */
BOOL_TYPE I2c__ResetBus(I2C_ENUM_TYPE i2c)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))
        BOOL_TYPE response;
        uint8 count;
        uint8 aux;

        response = FALSE;

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                Gpio__PinWrite(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT, TRUE);

                //Configure the SDA as INPUT mode and SCL into OOUTPUT mode
                Gpio__PinConfig(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT, OUTPUT_OPEN_DRAIN);
                Gpio__PinConfig(I2C_1_SDA_PORT, I2C_1_SDA_PORTBIT, INPUT_FLOAT);

                //Read the SDA status
                aux = Gpio__PinRead(I2C_1_SDA_PORT, I2C_1_SDA_PORTBIT);

                count = 0;

                //If the SDA is hold by the slave then it must be released by mimicking the I2c clock
                //reading SDA status
                while((aux == FALSE) &&
                      (count < 10))
                {
                    //Send a clock pulse to the slave to release the SDA line
                    Gpio__PinWrite(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT, FALSE);
                    Micro__DelayNumNops(10);
                    Gpio__PinWrite(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT, TRUE);
                    Micro__DelayNumNops(10);

                    //read the status of the SDA line
                    aux = Gpio__PinRead(I2C_1_SDA_PORT, I2C_1_SDA_PORTBIT);
                    count++;
                }

                response = (BOOL_TYPE)aux;

                I2c1_Ack_Error = I2C_ERROR_NONE;

                if (aux == FALSE)
                {
                    //If the SDA line is still read as low then log the BUS_LOCK error
                    I2c1_Ack_Error = I2C_ERROR_BUS_LOCKED;
                }

                Gpio__PinConfig(I2C_1_SCL_PORT, I2C_1_SCL_PORTBIT, SPECIAL_FUNCTION_OPEN_DRAIN);   //Configure SCL line for I2C1
                Gpio__PinConfig(I2C_1_SDA_PORT, I2C_1_SDA_PORTBIT, SPECIAL_FUNCTION_OPEN_DRAIN);   //Configure SDA line for I2C1
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                Gpio__PinWrite(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT, TRUE);

                //Configure the SDA as INPUT mode and SCL into OOUTPUT mode
                Gpio__PinConfig(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT, OUTPUT_OPEN_DRAIN);
                Gpio__PinConfig(I2C_2_SDA_PORT, I2C_2_SDA_PORTBIT, INPUT_FLOAT);


                //Read the SDA status
                aux = Gpio__PinRead(I2C_2_SDA_PORT, I2C_2_SDA_PORTBIT);

                count = 0;

                //If the SDA is hold by the slave then it must be released by mimicking the I2c clock
                //reading SDA status
                while((aux == FALSE) &&
                      (count < 10))
                {
                    //Send a clock pulse to the slave to release the SDA line
                    Gpio__PinWrite(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT, FALSE);
                    Micro__DelayNumNops(10);
                    Gpio__PinWrite(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT, TRUE);
                    Micro__DelayNumNops(10);

                    //read the status of the SDA line
                    aux = Gpio__PinRead(I2C_2_SDA_PORT, I2C_2_SDA_PORTBIT);
                    count++;
                }

                response = (BOOL_TYPE)aux;

                I2c2_Ack_Error = I2C_ERROR_NONE;

                if (aux == FALSE)
                {
                    //If the SDA line is still read as low then log the BUS_LOCK error
                    I2c2_Ack_Error = I2C_ERROR_BUS_LOCKED;
                }

                Gpio__PinConfig(I2C_2_SCL_PORT, I2C_2_SCL_PORTBIT, SPECIAL_FUNCTION_OPEN_DRAIN);   //Configure SCL line for I2C2
                Gpio__PinConfig(I2C_2_SDA_PORT, I2C_2_SDA_PORTBIT, SPECIAL_FUNCTION_OPEN_DRAIN);   //Configure SDA line for I2C2
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                Gpio__PinWrite(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT, TRUE);

                //Configure the SDA as INPUT mode and SCL into OOUTPUT mode
                Gpio__PinConfig(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT, OUTPUT_OPEN_DRAIN);
                Gpio__PinConfig(I2C_3_SDA_PORT, I2C_3_SDA_PORTBIT, INPUT_FLOAT);


                //Read the SDA status
                aux = Gpio__PinRead(I2C_3_SDA_PORT, I2C_3_SDA_PORTBIT);

                count = 0;

                //If the SDA is hold by the slave then it must be released by mimicking the I2c clock
                //reading SDA status
                while((aux == FALSE) &&
                      (count < 10))
                {
                    //Send a clock pulse to the slave to release the SDA line
                    Gpio__PinWrite(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT, FALSE);
                    Micro__DelayNumNops(10);
                    Gpio__PinWrite(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT, TRUE);
                    Micro__DelayNumNops(10);

                    //read the status of the SDA line
                    aux = Gpio__PinRead(I2C_3_SDA_PORT, I2C_3_SDA_PORTBIT);
                    count++;
                }

                response = (BOOL_TYPE)aux;

                I2c3_Ack_Error = I2C_ERROR_NONE;

                if (aux == FALSE)
                {
                    //If the SDA line is still read as low then log the BUS_LOCK error
                    I2c3_Ack_Error = I2C_ERROR_BUS_LOCKED;
                }

                Gpio__PinConfig(I2C_3_SCL_PORT, I2C_3_SCL_PORTBIT, SPECIAL_FUNCTION_OPEN_DRAIN);   //Configure SCL line for I2C3
                Gpio__PinConfig(I2C_3_SDA_PORT, I2C_3_SDA_PORTBIT, SPECIAL_FUNCTION_OPEN_DRAIN);   //Configure SDA line for I2C3
            }
        #endif

        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It enables a specific I2c device & I2c related interrupts
 *
 *    @details  This method is used to enable I2c device and I2c related interrupts such as event interrupt, data buffer
 *              empty interrupt & bus error interrupt
 *    @param    i2c : I2c modules i.e. I2c1 or I2c2 device
 *    @image html I2c__Enable.jpg
 */
void I2c__Enable(I2C_ENUM_TYPE i2c)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))
        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                #if (I2C1_SLAVE_FEATURE == DISABLED)
                    I2C_1.CR1 |= I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_ERRIE;                       //I2c1 device is enabled
                #else
                    I2C_1.CR1 |= I2C_CR1_PE | I2C_CR1_ERRIE | I2C_CR1_NACKIE | I2C_CR1_ADDRIE | I2C_CR1_STOPIE;
                #endif
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                I2C_2.CR1 |= I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_ERRIE;                       //I2c1 device is enabled
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                I2C_3.CR1 |= I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_ERRIE;                       //I2c1 device is enabled
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It disables a specific I2c device & event interrupt related to it
 *
 *    @details  This method is used to disable I2c device & event interrupt related to it
 *    @param    i2c : I2c modules i.e. I2c1 or I2c2 device
 *    @image html I2c__Disable.jpg
 */
void I2c__Disable(I2C_ENUM_TYPE i2c)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                //Disable the I2c Peripheral and its events
                I2C_1.CR1 &= ~(I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE | I2C_CR1_ERRIE);
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                //Disable the I2c Peripheral and its events
                I2C_2.CR1 &= ~(I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE | I2C_CR1_ERRIE);
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                //Disable the I2c Peripheral and its events
                I2C_3.CR1 &= ~(I2C_CR1_PE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE | I2C_CR1_ERRIE);
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It performs I2C write operation for I2C1 & I2C2 modules of this microcontroller
 *
 *    @details  This method is used to perform write operation with device connected to I2C bus
 *    @param    i2c              : I2C modules i.e. I2c1 or I2c2 devices
 *    @param    internal_address : Defines the slave address in which data will be written
 *    @param    buf              : It points to buffer which contains the data to be written
 *    @param    size             : It defines the amount of bytes should be written
 *    @return   BOOL_TYPE
 *      @retval   TRUE(1)  = It means write operation with the devices connected to I2C bus is successful
 *      @retval   FALSE(0) = It means write operation with the devices connected to I2C bus is unsuccessful
 *    @image html I2c__Write.jpg
 */
BOOL_TYPE I2c__Write(I2C_ENUM_TYPE i2c, uint8 internal_address, uint8 * buf,uint16 size)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        BOOL_TYPE response;

        response = FALSE;

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                if ((I2C_1.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2C1_BUFFER_OUT_SIZE)                                                       //Checking for mentioned size not exceeding buffer size
                    {
                        #if (I2C1_SLAVE_FEATURE == DISABLED)
                            I2c__Disable(i2c);
                            I2c1_TxPointer = 0;

                            //Increment the size to accommodate the internal_address
                            size++;

                            I2c1_Buffer_Out[0] = internal_address;

                            #if (I2C_BY_REFERENCE == ENABLED)
                                I2c1_Buffer_Out = buf;
                            #else
                                memcpy((I2c1_Buffer_Out + 1), buf, size);
                            #endif

                            //Set the number of bytes to be transfered, which is limited to max of 255 bytes
                            //Master write Transfer, Enable Automatic End mode,
                            I2C_1.CR2 &= (uint32)(~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN));
                            I2C_1.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND);


                            I2c__Enable(i2c);                                                                   //I2C module is enabled
                            I2C1_START();                                                                       //Configuring I2c1 as master & generating start signal

                            response = TRUE;
                            I2c1_Ack_Error = I2C_ERROR_NONE;
                        #else

                            I2c1_TxPointer = 0;

                            #if (I2C_BY_REFERENCE == ENABLED)
                                I2c1_Buffer_Out = buf;
                            #else
                                memcpy(I2c1_Buffer_Out, buf, size);
                            #endif
                            I2c1_Slave_TxSize = size;

                            I2C_1.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES));

                            response = TRUE;
                            I2c1_Ack_Error = I2C_ERROR_NONE;
                        #endif
                        }
                    }
                }

        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                if ((I2C_2.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2C2_BUFFER_OUT_SIZE)                                                       //Checking for mentioned size not exceeding buffer size
                    {
                        I2c__Disable(i2c);
                        I2c2_TxPointer = 0;

                        //Increment the size to accommodate the internal_address
                        size++;

                        I2c2_Buffer_Out[0] = internal_address;

                        #if (I2C_BY_REFERENCE == ENABLED)
                            I2c2_Buffer_Out = buf;
                        #else
                            memcpy((I2c2_Buffer_Out + 1), buf, size);
                        #endif

                        //Set the number of bytes to be transfered, which is limited to max of 255 bytes
                        //Master write Transfer, Enable Automatic End mode,
                        I2C_2.CR2 &= (uint32)(~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN));
                        I2C_2.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND);


                        I2c__Enable(i2c);                                                                   //I2C module is enabled
                        I2C2_START();                                                                       //Configuring I2c1 as master & generating start signal

                        response = TRUE;
                        I2c2_Ack_Error = I2C_ERROR_NONE;
                    }
                }
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                if ((I2C_3.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2C3_BUFFER_OUT_SIZE)                                                       //Checking for mentioned size not exceeding buffer size
                    {
                        I2c__Disable(i2c);
                        I2c3_TxPointer = 0;

                        //Increment the size to accommodate the internal_address
                        size++;

                        I2c3_Buffer_Out[0] = internal_address;

                        #if (I2C_BY_REFERENCE == ENABLED)
                            I2c3_Buffer_Out = buf;
                        #else
                            memcpy((I2c3_Buffer_Out + 1), buf, size);
                        #endif

                        //Set the number of bytes to be transfered, which is limited to max of 255 bytes
                        //Master write Transfer, Enable Automatic End mode,
                        I2C_3.CR2 &= (uint32)(~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN));
                        I2C_3.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND);


                        I2c__Enable(i2c);                                                                   //I2C module is enabled
                        I2C3_START();                                                                       //Configuring I2c1 as master & generating start signal

                        response = TRUE;
                        I2c3_Ack_Error = I2C_ERROR_NONE;
                    }
                }
            }
        #endif
        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It is used to write on I2c bus in blocking way
 *
 *    @details  This method is used to perform I2c write operation in blocking way i.e. it initiates the data transfer
 *              and waits till the data transfer is complete
 *    @param    i2c              : I2C modules i.e. I2c1 or I2c2 device
 *    @param    internal_address : Defines the slave address in which data will be written
 *    @param    buf              : It points to the buffer which contains the data to be written
 *    @param    size             : It defines the amount of bytes should be written
 *    @return   BOOL_TYPE
 *      @retval   TRUE(1)  = It means write operation with the devices connected to I2C bus in blocking way is successful
 *      @retval   FALSE(0) = It means write operation with the devices connected to I2C bus in blocking way is unsuccessful
 *    @image html I2c__BlockingWrite.jpg
 */
BOOL_TYPE I2c__BlockingWrite(I2C_ENUM_TYPE i2c,uint8 internal_address, uint8 * buf,uint16 size)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        BOOL_TYPE response;
        I2C_STATE_TYPE state;

        response = I2c__Write(i2c, internal_address, buf, size);

        if (response == TRUE)
        {
            state = I2C_STATE_BUSY;

            while (state == I2C_STATE_BUSY)
            {
                state  = (I2C_STATE_TYPE)I2c__GetStatus(i2c, I2C_STATUS_STATE);
            }

            if (I2c__GetStatus(i2c, I2C_STATUS_ERROR) == I2C_ERROR_NONE)
            {
                response = TRUE;
            }
            else
            {
                response = FALSE;
            }
        }
        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It performs I2c read operation
 *    @details  This method is used to read a byte of data from data register, for this to happen I2c__RequestRead()
 *              operation needs to be performed prior this operation
 *    @param    i2c     : I2C modules i.e. I2c1 or I2c2 device
 *    @param    buf     : It's a pointer to the Buffer where the read data will be stored
 *    @param    size    : It defines the amount of bytes will be read
 *    @return   BOOL_TYPE
 *      @retval   TRUE(1)  = If requested operation is successful
 *      @retval   FALSE(0) = If requested operation is unsuccessful
 *    @image html I2c__Read.jpg
 */
BOOL_TYPE I2c__Read(I2C_ENUM_TYPE i2c, uint8 * buf,uint16 size)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        BOOL_TYPE response;

        response = FALSE;

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                if ((I2C_1.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2c1_RxPointer)
                    {
                        memcpy(buf, I2c1_Buffer_In, I2c1_RxPointer);
                        I2c1_RxPointer = 0;
                        response = TRUE;;
                    }
                }
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                if ((I2C_2.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2c2_RxPointer)
                    {
                        memcpy(buf, I2c2_Buffer_In, I2c2_RxPointer);
                        I2c2_RxPointer = 0;
                        response = TRUE;;
                    }
                }
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                if ((I2C_3.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2c3_RxPointer)
                    {
                        memcpy(buf, I2c3_Buffer_In, I2c3_RxPointer);
                        I2c3_RxPointer = 0;
                        response = TRUE;;
                    }
                }
            }
        #endif

        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It performs request for read operation
 *
 *    @details  This method is used to perform actual read operation it copies data from data register and stores in
 *              data buffer from where upon performing read operation data is read. So it means before performing read
 *              operation I2c__RequestRead() needs to be called
 *    @param    i2c: device enumerator
 *    @param    buf: It's a pointer to the Buffer where the read data will be stored.
 *    @param    size: It defines the amount of bytes will be read
 *    @return   BOOL_TYPE
 *      @retval   TRUE(1)  = It means requested operation is successfully executed
 *      @retval   FALSE(0) = It means requested operation is not successfully executed
 */
BOOL_TYPE I2c__RequestRead(I2C_ENUM_TYPE i2c,uint16 size)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))
        BOOL_TYPE response;

        response = FALSE;

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                if ((I2C_1.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2C1_BUFFER_IN_SIZE)
                    {
                        I2c1_RxPointer = 0;

                        if(size != 0)
                        {
                            I2c__Disable(i2c);

                            //Set the number of bytes to be transfered, which is limited to max of 255 bytes
                            //Master Read mode, Enable Automatic End mode,
                            I2C_1.CR2 &= (uint32)(~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN));
                            I2C_1.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);

                            I2c__Enable(i2c);                                                                   //I2C module is enabled
                            I2C1_START();                                                                       //Configuring I2c1 as master & generating start signal
                        }

                        response = TRUE;
                        I2c1_Ack_Error = I2C_ERROR_NONE;
                    }
                }
            }
        #endif


        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                if ((I2C_2.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2C2_BUFFER_IN_SIZE)
                    {
                        I2c2_RxPointer = 0;

                        if(size != 0)
                        {
                            I2c__Disable(i2c);

                            //Set the number of bytes to be transfered, which is limited to max of 255 bytes
                            //Master Read mode, Enable Automatic End mode,
                            I2C_2.CR2 &= (uint32)(~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN));
                            I2C_2.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);

                            I2c__Enable(i2c);                                                                   //I2C module is enabled
                            I2C2_START();                                                                       //Configuring I2c1 as master & generating start signal
                        }

                        response = TRUE;
                        I2c2_Ack_Error = I2C_ERROR_NONE;
                    }
                }
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                if ((I2C_3.ISR & I2C_ISR_BUSY) == FALSE)
                {
                    if (size <= I2C3_BUFFER_IN_SIZE)
                    {
                        I2c3_RxPointer = 0;

                        if(size != 0)
                        {
                            I2c__Disable(i2c);

                            //Set the number of bytes to be transfered, which is limited to max of 255 bytes
                            //Master Read mode, Enable Automatic End mode,
                            I2C_3.CR2 &= (uint32)(~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN));
                            I2C_3.CR2 |= (uint32)((((size & 0xff) << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);

                            I2c__Enable(i2c);                                                                   //I2C module is enabled
                            I2C3_START();                                                                       //Configuring I2c1 as master & generating start signal
                        }

                        response = TRUE;
                        I2c3_Ack_Error = I2C_ERROR_NONE;
                    }
                }
            }
        #endif

        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
*     @brief    It performs read operation in blocking way
*
*     @details  This method reads data from a I2c device in a blocking way (function just returns when the process is finished).
*               The programmer may have to be careful with the amount of bytes requested as well as the time required to read it.
*               It performs read operation till it is completed and returns false response if bus communication occurs
*     @param    i2c     : device enumerator
*     @param    buf    : It's a pointer to the Buffer where the read data will be stored.
*     @param    size    : It defines the amount of bytes will be read
*     @return   BOOL_TYPE
*      @retval    TRUE(1)  = It means requested operation is successful
*      @retval    FALSE(0) = It means requested operation is not successful
 *    @image html I2c__BlockingRead.jpg
*/
BOOL_TYPE I2c__BlockingRead(I2C_ENUM_TYPE i2c, uint8 * buf, uint16 size)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        BOOL_TYPE response;
        I2C_STATE_TYPE state;

        response =  I2c__RequestRead(i2c, size);

        if (response == TRUE)
        {
            state = I2C_STATE_BUSY;

            while (state  == I2C_STATE_BUSY)
            {
                state  = (I2C_STATE_TYPE)I2c__GetStatus(i2c, I2C_STATUS_STATE);
            }

            if (I2c__GetStatus(i2c, I2C_STATUS_ERROR) == I2C_ERROR_NONE)
            {
                response = I2c__Read(i2c,buf,size);
            }
            else
            {
                response = FALSE;
            }
        }
        return (response);

    #else
        return (FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns the current status I2C communication service
 *
 *    @details  This method is used to provide status of I2C communication service i.e. I2C_STATE_IDLE in case no activity
 *              on the I2C bus, I2C_STATE_BUSY some activity on the bus, I2C_STATE_ERROR error occurred on the I2C bus
 *    @param    i2c : I2C module or device
 *    @return   uint8
 *      @retval   (I2C_STATE_IDLE, I2C_STATE_BUSY or I2C_STATE_ERROR)
 */
uint16 I2c__GetStatus(I2C_ENUM_TYPE i2c,I2C_STATUS_TYPE index)
{
    #if ((I2C1_FEATURE == ENABLED) || (I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))

        uint8 response;

        response = 0;

        #if (I2C1_FEATURE == ENABLED)
            if (i2c == I2C1)
            {
                switch (index)
                {
                    case I2C_STATUS_STATE:
                        if ((I2C_1.ISR & I2C_ISR_BUSY) != FALSE)
                        {
                            response = I2C_STATE_BUSY;
                        }
                        break;

                    case I2C_STATUS_BUFFER_IN:
                        if ((I2C_1.ISR & I2C_ISR_BUSY) == FALSE)
                        {
                            response = I2c1_RxPointer;
                        }
                        break;

                    case I2C_STATUS_ERROR:
                        if (((I2C_1.ISR & I2C_ISR_ARLO) != FALSE) ||
                        ((I2c1_Ack_Error == I2C_ERROR_BUS_LOCKED)))
                        {
                            //Set the error flag
                            response = I2C_ERROR_BUS_LOCKED;
                        }
                        else if ((I2C_1.ISR & ( I2C_ISR_NACKF | I2C_ISR_BERR)) != FALSE)
                        {
                            response = I2C_ERROR_ACK;
                        }

                        //Clear the error to accommodate the new errors
                        I2C_1.ICR = (I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF);

                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (I2C2_FEATURE == ENABLED)
            if (i2c == I2C2)
            {
                switch (index)
                {
                    case I2C_STATUS_STATE:
                        if ((I2C_2.ISR & I2C_ISR_BUSY) != FALSE)
                        {
                            response = I2C_STATE_BUSY;
                        }
                        break;

                    case I2C_STATUS_BUFFER_IN:
                        if ((I2C_2.ISR & I2C_ISR_BUSY) == FALSE)
                        {
                            response = I2c2_RxPointer;
                        }
                        break;

                    case I2C_STATUS_ERROR:
                        if (((I2C_2.ISR & I2C_ISR_ARLO) != FALSE) ||
                        ((I2c2_Ack_Error == I2C_ERROR_BUS_LOCKED)))
                        {
                            //Set the error flag
                            response = I2C_ERROR_BUS_LOCKED;
                        }
                        else if ((I2C_2.ISR & ( I2C_ISR_NACKF | I2C_ISR_BERR)) != FALSE)
                        {
                            response = I2C_ERROR_ACK;
                        }

                        //Clear the error to accommodate the new errors
                        I2C_2.ICR = (I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF);

                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            if (i2c == I2C3)
            {
                switch (index)
                {
                    case I2C_STATUS_STATE:
                        if ((I2C_3.ISR & I2C_ISR_BUSY) != FALSE)
                        {
                            response = I2C_STATE_BUSY;
                        }
                        break;

                    case I2C_STATUS_BUFFER_IN:
                        if ((I2C_3.ISR & I2C_ISR_BUSY) == FALSE)
                        {
                            response = I2c3_RxPointer;
                        }
                        break;

                    case I2C_STATUS_ERROR:
                        if (((I2C_3.ISR & I2C_ISR_ARLO) != FALSE) ||
                        ((I2c3_Ack_Error == I2C_ERROR_BUS_LOCKED)))
                        {
                            //Set the error flag
                            response = I2C_ERROR_BUS_LOCKED;
                        }
                        else if ((I2C_3.ISR & ( I2C_ISR_NACKF | I2C_ISR_BERR)) != FALSE)
                        {
                            response = I2C_ERROR_ACK;
                        }

                        //Clear the error to accommodate the new errors
                        I2C_3.ICR = (I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF);

                        break;

                    default:
                        break;
                }
            }
        #endif

        return (response);

    #else
        return (FALSE);
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It defines ISR for I2C interrupt
 *
 *    @details  This method defines ISR for I2C interrupt whenever data byte is available in I2C_DR register & any event
 *              occurs during I2C bus communication
 */
void I2c1__InterruptHandler(void)
{
    #if (I2C1_SLAVE_FEATURE == DISABLED)
        #if (I2C1_FEATURE == ENABLED)
            //Handle Error Events
            if (((I2C_1.ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT | I2C_ISR_NACKF)) != FALSE))
            {
                if ((I2C_1.ISR & I2C_ISR_ARLO) != FALSE)
                {
                    //Set the error flag
                    I2c1_Ack_Error = I2C_ERROR_BUS_LOCKED;
                }
                else
                {
                    I2c1_Ack_Error = I2C_ERROR_ACK;
                }

                //Clear the error to accommodate the new errors
                I2C_1.ICR = (I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF);
            }
            else
            {
                //Handle Write data
                if (I2C_1.ISR & I2C_ISR_TXIS)
                {
                    I2C_1.TXDR = I2c1_Buffer_Out[I2c1_TxPointer];
                    I2c1_TxPointer++;
                }
                //Handle Read data
                else if (I2C_1.ISR & I2C_ISR_RXNE)
                {
                    I2c1_Buffer_In[I2c1_RxPointer] = I2C_1.RXDR;
                    I2c1_RxPointer++;
                }
            }
        #else
        #endif
    #else
        //Slave mode

            //Address matched
            if ((I2C_1.ISR & I2C_ISR_ADDR) != FALSE)
            {
                //Clear NACK
                I2C_1.CR2 &= ~I2C_CR2_NACK;

                if (I2C_1.ISR & I2C_ISR_DIR)
                {
                    if (I2c1_Slave_TxSize != 0)
                    {
                        //Clear the index to accommodate the new write
                        I2c1_TxPointer = 0;

                        //Read transfer, Slave enters transmitter mode
                        I2C_1.CR1 |= I2C_CR1_TXIE | I2C_CR1_NACKIE;
                    }
                    else
                    {
                        //No Data to send
                        I2C_1.CR2 |= I2C_CR2_NACK;
                    }
                }
                else
                {
                    //Clear the index to accommodate the new read
                    I2c1_RxPointer = 0;

                    I2C_1.CR1 |= I2C_CR1_RXIE | I2C_CR1_NACKIE;

                    //Write transfer, Slave enters receiver mode
                    I2C_1.CR2 |= I2C1_BUFFER_IN_SIZE << I2C_CR2_NBYTES_POS;
                }

                //Clear the error to accommodate the new errors
                I2C_1.ICR = (I2C_ICR_ADDRCF);
            }
            //I2c1_Slave_TxSize

            if ((I2C_1.ISR & I2C_ISR_RXNE) != FALSE)
            {
                I2c1_Buffer_In[I2c1_RxPointer] = I2C_1.RXDR;
                I2c1_RxPointer++;

                if (I2c1_RxPointer > I2C1_BUFFER_IN_SIZE)
                {
                    I2C_1.CR2 |= I2C_CR2_NACK;
                }
            }


            if ((I2C_1.ISR & I2C_ISR_TXIS) != FALSE)
            {
                I2C_1.TXDR = I2c1_Buffer_Out[I2c1_TxPointer];
                I2c1_TxPointer++;

                if (I2c1_TxPointer > I2c1_Slave_TxSize)
                {
                    I2C_1.CR2 |= I2C_CR2_NACK;
                }
            }

            if ((I2C_1.ISR & I2C_ISR_STOPF) != FALSE)
            {
                if ((I2C_1.ISR & I2C_ISR_DIR) != FALSE)
                {
                    //Transmission completed
                    I2c_1_SLAVE_DATA_TRANSMISSION_COMPLETED();

                }
                else
                {
                    if (I2c1_RxPointer != 0)
                    {
                        //Reception completed
                        I2C_1_SLAVE_MODE_NEW_DATA_RECEIVED();
                    }

                }
                //Clear the error to accommodate the new errors
                I2C_1.ICR = I2C_ICR_STOPCF;
            }

            if ((I2C_1.ISR & I2C_ISR_NACKF) != FALSE)
            {
                //Clear the error to accommodate the new errors
                I2C_1.ICR = I2C_ICR_NACKCF;
            }
    #endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It defines ISR for I2C interrupt
 *
 *    @details  This method defines ISR for I2C interrupt whenever data byte is available in I2C_DR register & any event
 *              occurs during I2C bus communication
 */
void I2c2_3__InterruptHandler(void)
{
    #if ((I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))
        #if (I2C2_FEATURE == ENABLED)
            //Handle Error Events
            if (((I2C_2.ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT | I2C_ISR_NACKF)) != FALSE))
            {
                if ((I2C_2.ISR & I2C_ISR_ARLO) != FALSE)
                {
                    //Set the error flag
                    I2c2_Ack_Error = I2C_ERROR_BUS_LOCKED;
                }
                else
                {
                    I2c2_Ack_Error = I2C_ERROR_ACK;
                }

                //Clear the error to accommodate the new errors
                I2C_2.ICR = (I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF);
            }
            else
            {
                //Handle Write data
                if (I2C_2.ISR & I2C_ISR_TXIS)
                {
                    I2C_2.TXDR = I2c2_Buffer_Out[I2c2_TxPointer];
                    I2c2_TxPointer++;
                }
                //Handle Read data
                else if (I2C_2.ISR & I2C_ISR_RXNE)
                {
                    I2c2_Buffer_In[I2c2_RxPointer] = I2C_2.RXDR;
                    I2c2_RxPointer++;
                }
            }
        #endif

        #if (I2C3_FEATURE == ENABLED)
            //Handle Error Events
            if (((I2C_3.ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT | I2C_ISR_NACKF)) != FALSE))
            {
                if ((I2C_3.ISR & I2C_ISR_ARLO) != FALSE)
                {
                    //Set the error flag
                    I2c3_Ack_Error = I2C_ERROR_BUS_LOCKED;
                }
                else
                {
                    I2c3_Ack_Error = I2C_ERROR_ACK;
                }

                //Clear the error to accommodate the new errors
                I2C_3.ICR = (I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF);

            }
            else
            {
                //Handle Write data
                if (I2C_3.ISR & I2C_ISR_TXIS)
                {
                    I2C_3.TXDR = I2c3_Buffer_Out[I2c3_TxPointer];
                    I2c3_TxPointer++;
                }
                //Handle Read data
                else if (I2C_3.ISR & I2C_ISR_RXNE)
                {
                    I2c3_Buffer_In[I2c3_RxPointer] = I2C_3.RXDR;
                    I2c3_RxPointer++;
                }
            }
        #endif

    #else
    #endif

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
