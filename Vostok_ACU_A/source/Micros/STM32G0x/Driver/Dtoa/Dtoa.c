/**
 *  @file       
 *
 *  @brief      The DtoA GDM for STM32G0x is a module that implements the
 *              DtoA Standard API (Application programming interface for the DtoA Channels) for the STM32G0x family of microcontrollers
 *
 *
 *  @details    The two 12-bit buffered DAC channels can be used to convert digital signals into analog
 *              voltage signal outputs.
 *
 *              DtoA channels are available in the below port-pins of STM32G0x -
 *               <pre>
 *               --------------------------------------
 *              | DTOA Channel   |  Port    |   Pin    |
 *              |----------------|----------|----------|
 *              |    DTOA_CH1    |   PORTA  |    4     |
 *              |----------------|----------|----------|
 *              |    DTOA_CH2    |   PORTA  |    5     |
 *               -------------------------------------- </pre>
 *
 *       The analog output voltage on each DAC channel pin is determined by the following equation:
 *
 *       DAC_OUTx = VREF+ * DOR / 4095
 *
 *       with  DOR is the Data Output Register
 *       VEF+ is the input voltage reference (refer to the device data sheet)
 *
 *       e.g. To set DAC_OUT1 to 0.7V, use
 *       Dtoa__SetValue(DTOA_CH1, 868);
 *       Assuming that VREF+ = 3.3, DAC_OUT1 = (3.3 * 868) / 4095 = 0.7V
 *
 *
 *       Supports only 12-bit and 8-bit resolution
 *       Continuous conversion feature is not implemented
 *
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
#include "uc.h"
#include "Dtoa.h" 
#include "Gpio.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DTOA_CHANNEL_1_PORT             PORTA
#define DTOA_CHANNEL_1_PORTBIT          4

#define DTOA_CHANNEL_2_PORT             PORTA
#define DTOA_CHANNEL_2_PORTBIT          5

#define DMA_CR_CHNL1_CLEAR              0xFFFF0000
#define DMA_CR_CHNL2_CLEAR              0x0000FFFF

#ifndef DTOA_CHANNEL_RESOLUTION_HARDCODED
    #define DTOA_CHANNEL_RESOLUTION_HARDCODED       0
#elif ((DTOA_CHANNEL_RESOLUTION_HARDCODED != 0) && (DTOA_CHANNEL_RESOLUTION_HARDCODED != 8) && (DTOA_CHANNEL_RESOLUTION_HARDCODED != 12))
    #error:"STM32G0x micro-controller supports only 8 or 12 bit DTOA resolution";
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module Dtoa and its variables
 *
 */
void Dtoa__Initialize(void)
{
    #if ((DTOA_CH1_FEATURE == ENABLED) || (DTOA_CH2_FEATURE == ENABLED))
        //Reset the DAC engine
        RCC.APBRSTR1 |= RCC_APBRSTR1_DAC1RST;
        RCC.APBRSTR1 &= (uint32)(~RCC_APBRSTR1_DAC1RST);

        //Enable the DAC device in the APB1 bus
        RCC.APBENR1 |= RCC_APBENR1_DAC1EN;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      This method disables the Dtoa Channel
 * @param      channel - Dtoa Channel
 */
void Dtoa__Disable(DTOA_CHANNEL_DEF channel)
{
    #if ((DTOA_CH1_FEATURE == ENABLED) || (DTOA_CH2_FEATURE == ENABLED))

        #if (DTOA_CH2_FEATURE == ENABLED)
            #if (DTOA_CH1_FEATURE == ENABLED)
            if (channel == DTOA_CH2)
            #endif
            {
                DAC.SR |= DAC_SR_DMAUDR2;

                //Disable the Dtoa Channel
                DAC.CR &= DMA_CR_CHNL2_CLEAR;
            }
        #endif

        #if (DTOA_CH1_FEATURE == ENABLED) && (DTOA_CH2_FEATURE == ENABLED)
            else
        #endif
        #if (DTOA_CH1_FEATURE == ENABLED)
            {
                DAC.SR |= DAC_SR_DMAUDR1;

                //Enable the Dtoa Channel and Enable the Output buffer which is used to reduce the output impedance
                DAC.CR &= DMA_CR_CHNL1_CLEAR;
            }

        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   - This method converts the Digital to analog value on the specified channel
 * @details - Continuous Conversion -
 *                  DTOA_CONTINUOUS_CONVERSION_FEATURE shall be enabled
 *
 *            Single Conversion -
 *                  If the application is going to use only the sngle conversion then DTOA_CONTINUOUS_CONVERSION_FEATURE shall be disabled
 *                  for optimization
 *
 * @param channel - DTOA_CHANNEL_DEF
 * @param resolution - DTOA_CHANNEL_RESOLUTION_DEF
 * @param sampling_hz - sampling rate
 * @param source_address
 * @param size
 * @return
 *  @retval PASS - If the conversion requested is processed
 *  @retval FAIL - If the conversion requested is failed to process
 */
PASS_FAIL_TYPE Dtoa__Convert(DTOA_CHANNEL_DEF channel, DTOA_CHANNEL_RESOLUTION_DEF resolution, uint16 sampling_hz, uint32 source_address, uint32 size)
{
    #if ((DTOA_CH1_FEATURE == ENABLED) || (DTOA_CH2_FEATURE == ENABLED))

        PASS_FAIL_TYPE response;
        uint32 volatile *dac_dhr_ptr;

        response = FAIL;

        if (source_address != 0)
        {
            #if (DTOA_CH2_FEATURE == ENABLED)
                #if (DTOA_CH1_FEATURE == ENABLED)
                if (channel == DTOA_CH2)
                #endif
                {
                    //Configure the Dtoa Channel in Analog mode
                    Gpio__PinConfig(DTOA_CHANNEL_2_PORT, DTOA_CHANNEL_2_PORTBIT, INPUT_ANALOG);

                    //clear the DMA under run
                    DAC.SR |= DAC_SR_DMAUDR2;

                    #if (DTOA_CHANNEL_RESOLUTION_HARDCODED == 0)
                        if (resolution == DTOA_RESOLUTION_12BITS)
                        {
                            dac_dhr_ptr = &DAC.DHR12R2;

                        }
                        else
                        {
                            dac_dhr_ptr = &DAC.DHR8R2;

                        }
                    #else
                        #if (DTOA_CHANNEL_RESOLUTION_HARDCODED == 12)
                            dac_dhr_ptr = &DAC.DHR12R2;
                        #else
                            dac_dhr_ptr = &DAC.DHR8R2;
                        #endif
                    #endif

                    if (size == 1)
                    {
                        //000: DAC channel2 is connected to external pin with Buffer enabled
                        //Enable the Output buffer which is used to reduce the output impedance
                        DAC.MCR &= ~DAC_MCR_MODE2;

                        (*(uint32 *)dac_dhr_ptr) = (*(uint32 *)source_address);

                        //Enable the Dtoa Channel
                        DAC.CR |= DAC_CR_EN2;

                        response = PASS;
                    }
                }
            #endif

            #if (DTOA_CH1_FEATURE == ENABLED) && (DTOA_CH2_FEATURE == ENABLED)
                else
            #endif
            #if (DTOA_CH1_FEATURE == ENABLED)
            {
                {
                    //Configure the Dtoa Channel in Analog mode
                    Gpio__PinConfig(DTOA_CHANNEL_1_PORT, DTOA_CHANNEL_1_PORTBIT, INPUT_ANALOG);

                    //clear the DMA under run
                    DAC.SR |= DAC_SR_DMAUDR1;

                    #if (DTOA_CHANNEL_RESOLUTION_HARDCODED == 0)
                        if (resolution == DTOA_RESOLUTION_12BITS)
                        {
                            dac_dhr_ptr = &DAC.DHR12R1;
                        }
                        else
                        {
                            dac_dhr_ptr = &DAC.DHR8R1;
                        }
                    #else
                        #if (DTOA_CHANNEL_RESOLUTION_HARDCODED == 12)
                            dac_dhr_ptr = &DAC.DHR12R1;
                        #else
                            dac_dhr_ptr = &DAC.DHR8R1;
                        #endif
                    #endif

                    if (size == 1)
                    {
                        //000: DAC channel1 is connected to external pin with Buffer enabled
                        //Enable the Output buffer which is used to reduce the output impedance
                        DAC.MCR &= ~DAC_MCR_MODE1;

                        (*(uint32 *)dac_dhr_ptr) = (*(uint32 *)source_address);

                        //Enable the Dtoa Channel and Enable the Output buffer which is used to reduce the output impedance
                        DAC.CR |= DAC_CR_EN1;

                        response = PASS;
                    }
                }
            }
            #endif
        }
        return (response);
    #else
        return (FAIL);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @details -
 * @param channel
 * @return
 */
BOOL_TYPE Dtoa__IsFree(DTOA_CHANNEL_DEF channel)
{
    #if ((DTOA_CH1_FEATURE == ENABLED) || (DTOA_CH2_FEATURE == ENABLED))
            return(TRUE);
    #else
        return(FALSE);
    #endif
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
