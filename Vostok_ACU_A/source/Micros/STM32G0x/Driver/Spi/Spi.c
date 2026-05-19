/**
 *  @file
 *
 *  @brief      Spi GDM module for STM32G0x implements the Spi Standard API (Application Programming Interface).
 *
 *  @details    This module provides the synchronous serial communication services to the application,
 *              such as initialization of Spi module, transmission/reception of byte and multiple bytes
 *              via Spi and status of Spi communication using Dma module
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
#include "Spi.h"
#include "Micro.h"
#include "Gpio.h"
#include <string.h>
#include "Dma.h"


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//Compiler directive
//=====================================================================================================================
#ifndef SPI_1_TX_ONLY
    #define SPI_1_TX_ONLY        DISABLED
#endif

#ifndef SPI_2_TX_ONLY
    #define SPI_2_TX_ONLY        DISABLED
#endif

#ifndef SPI_3_TX_ONLY
    #define SPI_3_TX_ONLY        DISABLED
#endif

#if (SPI_BUFFER_SIZE == 0)
    #error : " SPI_BUFFER_SIZE macro value must be non-zero in the Spi_prm.h file"
#endif

#if (SPI_1_PRESCALER == 0)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_2)
#elif (SPI_1_PRESCALER == 1)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_4)
#elif (SPI_1_PRESCALER == 2)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_8)
#elif (SPI_1_PRESCALER == 3)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_16)
#elif (SPI_1_PRESCALER == 4)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_32)
#elif (SPI_1_PRESCALER == 5)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_64)
#elif (SPI_1_PRESCALER == 6)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_128)
#elif (SPI_1_PRESCALER == 7)
    #define SPI_1_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_256)
#else
    #error : " SPI_1_PRESCALER macro value must be between 0 to 7 in the Spi_prm.h file"
#endif

#if (SPI_2_PRESCALER == 0)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_2)
#elif (SPI_2_PRESCALER == 1)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_4)
#elif (SPI_2_PRESCALER == 2)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_8)
#elif (SPI_2_PRESCALER == 3)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_16)
#elif (SPI_2_PRESCALER == 4)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_32)
#elif (SPI_2_PRESCALER == 5)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_64)
#elif (SPI_2_PRESCALER == 6)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_128)
#elif (SPI_2_PRESCALER == 7)
    #define SPI_2_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_256)
#else
    #error : " SPI_2_PRESCALER macro value must be between 0 to 7 in the Spi_prm.h file"
#endif

#if (SPI_3_PRESCALER == 0)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_2)
#elif (SPI_3_PRESCALER == 1)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_4)
#elif (SPI_3_PRESCALER == 2)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_8)
#elif (SPI_3_PRESCALER == 3)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_16)
#elif (SPI_3_PRESCALER == 4)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_32)
#elif (SPI_3_PRESCALER == 5)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_64)
#elif (SPI_3_PRESCALER == 6)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_128)
#elif (SPI_3_PRESCALER == 7)
    #define SPI_3_CR1_BR         ((uint16)SPI_BAUDRATEPRESCALER_256)
#else
    #error : " SPI_3_PRESCALER macro value must be between 0 to 7 in the Spi_prm.h file"
#endif

#if (SPI_1_POL == 0)
    #define SPI_1_CR1_CPOL       ((uint16)0x0000)
#elif (SPI_1_POL == 1)
    #define SPI_1_CR1_CPOL       ((uint16)SPI_CR1_CPOL)
#else
    #error : " SPI_1_POL macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_2_POL == 0)
    #define SPI_2_CR1_CPOL       ((uint16)0x0000)
#elif (SPI_2_POL == 1)
    #define SPI_2_CR1_CPOL       ((uint16)SPI_CR1_CPOL)
#else
    #error : " SPI_2_POL macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_3_POL == 0)
    #define SPI_3_CR1_CPOL       ((uint16)0x0000)
#elif (SPI_3_POL == 1)
    #define SPI_3_CR1_CPOL       ((uint16)SPI_CR1_CPOL)
#else
    #error : " SPI_3_POL macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_1_CPHA_EDGE == 0)
    #define SPI_1_CR1_CPHA       ((uint16)0x0000)
#elif (SPI_1_CPHA_EDGE == 1)
    #define SPI_1_CR1_CPHA       ((uint16)SPI_CR1_CPHA)
#else
    #error : " SPI_1_CPHA_EDGE macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_2_CPHA_EDGE == 0)
    #define SPI_2_CR1_CPHA       ((uint16)0x0000)
#elif (SPI_2_CPHA_EDGE == 1)
    #define SPI_2_CR1_CPHA       ((uint16)SPI_CR1_CPHA)
#else
    #error : " SPI_2_CPHA_EDGE macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_3_CPHA_EDGE == 0)
    #define SPI_3_CR1_CPHA       ((uint16)0x0000)
#elif (SPI_3_CPHA_EDGE == 1)
    #define SPI_3_CR1_CPHA       ((uint16)SPI_CR1_CPHA)
#else
    #error : " SPI_3_CPHA_EDGE macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_1_LSBFIRST == 0)
    #define SPI_1_CR1_LSBFIRST  ((uint16)0x0000)
#elif (SPI_1_LSBFIRST == 1)
    #define SPI_1_CR1_LSBFIRST  ((uint16)SPI_CR1_LSBFIRST)
#else
    #error : " SPI_1_LSBFIRST macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_2_LSBFIRST == 0)
    #define SPI_2_CR1_LSBFIRST  ((uint16)0x0000)
#elif (SPI_2_LSBFIRST == 1)
    #define SPI_2_CR1_LSBFIRST  ((uint16)SPI_CR1_LSBFIRST)
#else
    #error : " SPI_2_LSBFIRST macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#if (SPI_3_LSBFIRST == 0)
    #define SPI_3_CR1_LSBFIRST  ((uint16)0x0000)
#elif (SPI_3_LSBFIRST == 1)
    #define SPI_3_CR1_LSBFIRST  ((uint16)SPI_CR1_LSBFIRST)
#else
    #error : " SPI_3_LSBFIRST macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

//=====================================================================================================================
// Spi and Dma Configuration
//=====================================================================================================================
#define SPI_1_TX_DMA_ENGINE         DMA_ENGINE1
#define SPI_1_TX_DMA_CHANNEL        DMA_CH2

#define SPI_1_RX_DMA_ENGINE         DMA_ENGINE1
#define SPI_1_RX_DMA_CHANNEL        DMA_CH3

#define SPI_2_TX_DMA_ENGINE         DMA_ENGINE1
#define SPI_2_TX_DMA_CHANNEL        DMA_CH4

#define SPI_2_RX_DMA_ENGINE         DMA_ENGINE1
#define SPI_2_RX_DMA_CHANNEL        DMA_CH5

#define SPI_3_TX_DMA_ENGINE         DMA_ENGINE1
#define SPI_3_TX_DMA_CHANNEL        DMA_CH6

#define SPI_3_RX_DMA_ENGINE         DMA_ENGINE1
#define SPI_3_RX_DMA_CHANNEL        DMA_CH7

#ifndef SPI_1_DMA_PRIORITY
    #define SPI_1_DMA_PRIORITY  DMA_PRIORITY_VERYHIGH
#endif

#ifndef SPI_2_DMA_PRIORITY
    #define SPI_2_DMA_PRIORITY  DMA_PRIORITY_VERYHIGH
#endif

#ifndef SPI_3_DMA_PRIORITY
    #define SPI_3_DMA_PRIORITY  DMA_PRIORITY_VERYHIGH
#endif

//=====================================================================================================================
//Variable section
//=====================================================================================================================
typedef struct
{
    #if (SPI_1_TX_BY_REFERENCE == DISABLED)
        uint8 Tx_Data[SPI_BUFFER_SIZE];
    #endif

    #if (SPI_1_TX_ONLY == DISABLED)
        uint8 Rx_Data[SPI_BUFFER_SIZE];
    #endif

    uint16 Write;

    #if (SPI_1_TX_ONLY == DISABLED)
        uint16 Read;
    #endif
} SPI_1_FIFO_DEF;

typedef struct
{
    #if (SPI_2_TX_BY_REFERENCE == DISABLED)
        uint8 Tx_Data[SPI_BUFFER_SIZE];
    #endif

    #if (SPI_2_TX_ONLY == DISABLED)
        uint8 Rx_Data[SPI_BUFFER_SIZE];
    #endif

    uint16 Write;

    #if (SPI_2_TX_ONLY == DISABLED)
        uint16 Read;
    #endif
} SPI_2_FIFO_DEF;

typedef struct
{
    #if (SPI_3_TX_BY_REFERENCE == DISABLED)
        uint8 Tx_Data[SPI_BUFFER_SIZE];
    #endif

    #if (SPI_3_TX_ONLY == DISABLED)
        uint8 Rx_Data[SPI_BUFFER_SIZE];
    #endif

    uint16 Write;

    #if (SPI_3_TX_ONLY == DISABLED)
        uint16 Read;
    #endif
} SPI_3_FIFO_DEF;

#if (SPI_1_FEATURE == ENABLED)
    #pragma location = "RAM_NOTEST_AREA"
    SPI_1_FIFO_DEF Spi1_Buffer;

    #if (SPI_1_TX_BY_REFERENCE == DISABLED)
        static const DMA_INIT_TYPE SPI_1_TX_DMA_CONFIG =
    #else
        static DMA_INIT_TYPE SPI_1_TX_DMA_CONFIG =
    #endif
                                                            {
                                                                (DMA_ADDRESS_TYPE)(&(SPI_1.DR)),
                                                                #if (SPI_1_TX_BY_REFERENCE == DISABLED)
                                                                    (DMA_ADDRESS_TYPE)(Spi1_Buffer.Tx_Data),
                                                                #else
                                                                    (DMA_ADDRESS_TYPE)(NULL),
                                                                #endif
                                                                DMA_DATA_DIRECTION_MEM_TO_PERIPH,
                                                                0,
                                                                DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_MODE_NORMAL,
                                                                SPI_1_DMA_PRIORITY,
                                                                DMAMUX_REQ_SPI1_TX
                                                            };
    #if (SPI_1_TX_ONLY == DISABLED)
        static const DMA_INIT_TYPE SPI_1_RX_DMA_CONFIG = {   (DMA_ADDRESS_TYPE)(&SPI_1.DR),
                                                                (DMA_ADDRESS_TYPE)(Spi1_Buffer.Rx_Data),
                                                                DMA_DATA_DIRECTION_PERIPH_TO_MEM,
                                                                0,
                                                                DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_MODE_NORMAL,
                                                                SPI_1_DMA_PRIORITY,
                                                                DMAMUX_REQ_SPI1_RX
                                                            };
    #endif
#endif

#if (SPI_2_FEATURE == ENABLED)
    #pragma location = "RAM_NOTEST_AREA"
    SPI_2_FIFO_DEF Spi2_Buffer;

    #if (SPI_2_TX_BY_REFERENCE == DISABLED)
        static const DMA_INIT_TYPE SPI_2_TX_DMA_CONFIG =
    #else
        static DMA_INIT_TYPE SPI_2_TX_DMA_CONFIG =
    #endif
                                                            {
                                                                (DMA_ADDRESS_TYPE)(&(SPI_2.DR)),
                                                                #if (SPI_2_TX_BY_REFERENCE == DISABLED)
                                                                    (DMA_ADDRESS_TYPE)(Spi2_Buffer.Tx_Data),
                                                                #else
                                                                    (DMA_ADDRESS_TYPE)(NULL),
                                                                #endif
                                                                DMA_DATA_DIRECTION_MEM_TO_PERIPH,
                                                                0,
                                                                DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_MODE_NORMAL,
                                                                SPI_2_DMA_PRIORITY,
                                                                DMAMUX_REQ_SPI2_TX
                                                            };
    #if (SPI_2_TX_ONLY == DISABLED)
        static const DMA_INIT_TYPE SPI_2_RX_DMA_CONFIG = {   (DMA_ADDRESS_TYPE)(&SPI_2.DR),
                                                                (DMA_ADDRESS_TYPE)(Spi2_Buffer.Rx_Data),
                                                                DMA_DATA_DIRECTION_PERIPH_TO_MEM,
                                                                0,
                                                                DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_MODE_NORMAL,
                                                                SPI_2_DMA_PRIORITY,
                                                                DMAMUX_REQ_SPI2_RX
                                                            };
    #endif
#endif

#if (SPI_3_FEATURE == ENABLED)
    SPI_3_FIFO_DEF Spi3_Buffer;
    #if (SPI_3_TX_BY_REFERENCE == DISABLED)
        static const DMA_INIT_TYPE SPI_3_TX_DMA_CONFIG =
    #else
        static DMA_INIT_TYPE SPI_3_TX_DMA_CONFIG =
    #endif
                                                            {   (DMA_ADDRESS_TYPE)(&SPI_3.DR),
                                                                #if (SPI_3_TX_BY_REFERENCE == DISABLED)
                                                                    (DMA_ADDRESS_TYPE)(Spi3_Buffer.Tx_Data),
                                                                #else
                                                                    (DMA_ADDRESS_TYPE)(NULL),
                                                                #endif
                                                                DMA_DATA_DIRECTION_MEM_TO_PERIPH,
                                                                0,
                                                                DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_MODE_NORMAL,
                                                                SPI_3_DMA_PRIORITY,
                                                                DMAMUX_REQ_SPI3_TX
                                                            };
    #if (SPI_3_TX_ONLY == DISABLED)
        static const DMA_INIT_TYPE SPI_3_RX_DMA_CONFIG = {   (DMA_ADDRESS_TYPE)(&SPI_3.DR),
                                                                (DMA_ADDRESS_TYPE)(Spi3_Buffer.Rx_Data),
                                                                DMA_DATA_DIRECTION_PERIPH_TO_MEM,
                                                                0,
                                                                DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_DATA_WIDTH_BYTE,
                                                                DMA_MODE_NORMAL,
                                                                SPI_3_DMA_PRIORITY,
                                                                DMAMUX_REQ_SPI3_RX
                                                            };
    #endif
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
/**
 * @brief  Configures the specified port-pin into the Alternate function.
 */
#define GPIO__PIN_COFIG_AF(PORT, PORTBIT, AF)       {(*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] = (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] | ((uint32)(AF) << ((uint32)((uint32)PORTBIT & (uint32)0x07) << 2));}

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes a specific SPI module by reading the Spi Configuration parameters
 *              defined in the Spi_prm.h file.
 *    @details  Before using Spi module this method should be called to initialize specific Spi module.
 *              To make use of this API the following configuration macros must be defined in the Spi_prm.h file-
 *              "#define SPI_x_PRESCALER"\n
 *              "#define SPI_x_LSBFIRST"\n
 *              "#define SPI_x_POL"\n
 *              "#define SPI_x_CPHA_EDGE"
 *    @param    spi : specific SPI module
 *    @image    html    Spi__Initialize.jpg
 */
void Spi__Initialize(SPI_ENUM_DEF spi)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))

        //Initialize the DMA
        Dma__EngineInitialize();

        //Clear the buffers
        Spi__Clear(spi);

        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                //Enable the clock for Spi1 module
                RCC.APBENR2 |= RCC_APBENR2_SPI1EN;

                //Reset the Spi1
                RCC.APBRSTR2 |= RCC_APBRSTR2_SPI1RST;
                RCC.APBRSTR2 &= (uint32)(~RCC_APBRSTR2_SPI1RST);

                #if (SPI_1_TX_ONLY == DISABLED)
                    // 8bits data format
                    // RXNE event is generated if the FIFO level is greater than or equal to 1/4 (8-bit)
                    SPI_1.CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH;

                    // 2-line unidirectional data mode selected
                    // CRC Disable
                    // Spi1 Baud rate, Polarity, Edge, DataFrame Format, Full Duplex, Software slave management
                    // Master Mode Enable, Spi Enabled.
                    SPI_1.CR1 = SPI_1_CR1_BR | SPI_1_CR1_CPOL | SPI_1_CR1_CPHA | SPI_1_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR;

                #else
                    // 8bits data format
                    SPI_1.CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;

                    // 1-line bidirectional data mode selected
                    // Transmit only mode enabled
                    // CRC Disable
                    // Spi1 Baud rate, Polarity, Edge, DataFrame Format, Software slave management
                    // Master Mode Enable, Spi Enabled.
                    SPI_1.CR1 = SPI_1_CR1_BR | SPI_1_CR1_CPOL | SPI_1_CR1_CPHA | SPI_1_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
                #endif

                #if (SPI_1_TX_ONLY == DISABLED)
                    SPI_1.CR2 |= SPI_CR2_LDMARX;
                #endif
                SPI_1.CR2 |= SPI_CR2_LDMATX;
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                //Enable the clock for Spi1 module
                RCC.APBENR1 |= RCC_APBENR1_SPI2EN;

                //Reset the Spi1
                RCC.APBRSTR1 |= RCC_APBRSTR1_SPI2RST;
                RCC.APBRSTR1 &= (uint32)(~RCC_APBRSTR1_SPI2RST);

                #if (SPI_2_TX_ONLY == DISABLED)
                    // 8bits data format
                    // RXNE event is generated if the FIFO level is greater than or equal to 1/4 (8-bit)
                    SPI_2.CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH;

                    // 2-line unidirectional data mode selected
                    // CRC Disable
                    // Spi1 Baud rate, Polarity, Edge, DataFrame Format, Full Duplex, Software slave management
                    // Master Mode Enable, Spi Enabled.
                    SPI_2.CR1 = SPI_2_CR1_BR | SPI_2_CR1_CPOL | SPI_2_CR1_CPHA | SPI_2_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR;

                #else
                    // 8bits data format
                    SPI_2.CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;

                    // 1-line bidirectional data mode selected
                    // Transmit only mode enabled
                    // CRC Disable
                    // Spi1 Baud rate, Polarity, Edge, DataFrame Format, Software slave management
                    // Master Mode Enable, Spi Enabled.
                    SPI_2.CR1 = SPI_2_CR1_BR | SPI_2_CR1_CPOL | SPI_2_CR1_CPHA | SPI_2_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
                #endif

                #if (SPI_2_TX_ONLY == DISABLED)
                    SPI_2.CR2 |= SPI_CR2_LDMARX;
                #endif
                SPI_2.CR2 |= SPI_CR2_LDMATX;
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                //Enable the clock for Spi1 module
                RCC.APBENR1 |= RCC_APBENR1_SPI3EN;

                //Reset the Spi1
                RCC.APBRSTR1 |= RCC_APBRSTR1_SPI3RST;
                RCC.APBRSTR1 &= (uint32)(~RCC_APBRSTR1_SPI3RST);

                #if (SPI_3_TX_ONLY == DISABLED)
                    // 8bits data format
                    // RXNE event is generated if the FIFO level is greater than or equal to 1/4 (8-bit)
                    SPI_3.CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH;

                    // 2-line unidirectional data mode selected
                    // CRC Disable
                    // Spi1 Baud rate, Polarity, Edge, DataFrame Format, Full Duplex, Software slave management
                    // Master Mode Enable, Spi Enabled.
                    SPI_3.CR1 = SPI_3_CR1_BR | SPI_3_CR1_CPOL | SPI_3_CR1_CPHA | SPI_3_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR;
                #else
                    // 8bits data format
                    SPI_3.CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;

                    // 1-line bidirectional data mode selected
                    // Transmit only mode enabled
                    // CRC Disable
                    // Spi1 Baud rate, Polarity, Edge, DataFrame Format, Software slave management
                    // Master Mode Enable, Spi Enabled.
                    SPI_3.CR1 = SPI_3_CR1_BR | SPI_3_CR1_CPOL | SPI_3_CR1_CPHA | SPI_3_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
                #endif

                #if (SPI_3_TX_ONLY == DISABLED)
                    SPI_3.CR2 |= SPI_CR2_LDMARX;
                #endif
                SPI_3.CR2 |= SPI_CR2_LDMATX;
            }
        #endif

        Spi__Enable(spi);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It enables a specific Spi device.
 *    @details  This method should be called after disabling the Spi module.
 *    @param    spi : specific spi device
 */
void Spi__Enable(SPI_ENUM_DEF spi)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))
        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                GPIO__PIN_COFIG_AF(SPI_1_MOSI_PORT, SPI_1_MOSI_PORTBIT, SPI_1_MOSI_AF);
                Gpio__PinConfig(SPI_1_MOSI_PORT, SPI_1_MOSI_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);

                GPIO__PIN_COFIG_AF(SPI_1_SCK_PORT, SPI_1_SCK_PORTBIT, SPI_1_SCK_AF);
                Gpio__PinConfig(SPI_1_SCK_PORT, SPI_1_SCK_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);

                #if (SPI_1_TX_ONLY == DISABLED)
                    GPIO__PIN_COFIG_AF(SPI_1_MISO_PORT, SPI_1_MISO_PORTBIT, SPI_1_MISO_AF);
                    Gpio__PinConfig(SPI_1_MISO_PORT, SPI_1_MISO_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);
                #endif

                SPI_1.CR1 |= SPI_CR1_SPE;    //Enable the Spi1
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                GPIO__PIN_COFIG_AF(SPI_2_MOSI_PORT, SPI_2_MOSI_PORTBIT, SPI_2_MOSI_AF);
                Gpio__PinConfig(SPI_2_MOSI_PORT, SPI_2_MOSI_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);

                GPIO__PIN_COFIG_AF(SPI_2_SCK_PORT, SPI_2_SCK_PORTBIT, SPI_2_SCK_AF);
                Gpio__PinConfig(SPI_2_SCK_PORT, SPI_2_SCK_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);

                #if (SPI_2_TX_ONLY == DISABLED)
                    GPIO__PIN_COFIG_AF(SPI_2_MISO_PORT, SPI_2_MISO_PORTBIT, SPI_2_MISO_AF);
                    Gpio__PinConfig(SPI_2_MISO_PORT, SPI_2_MISO_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);
                #endif

                SPI_2.CR1 |= SPI_CR1_SPE;    //Enable the Spi2
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                GPIO__PIN_COFIG_AF(SPI_3_MOSI_PORT, SPI_3_MOSI_PORTBIT, SPI_3_MOSI_AF);
                Gpio__PinConfig(SPI_3_MOSI_PORT, SPI_3_MOSI_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);

                GPIO__PIN_COFIG_AF(SPI_3_SCK_PORT, SPI_3_SCK_PORTBIT, SPI_3_SCK_AF);
                Gpio__PinConfig(SPI_3_SCK_PORT, SPI_3_SCK_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);

                #if (SPI_3_TX_ONLY == DISABLED)
                    GPIO__PIN_COFIG_AF(SPI_3_MISO_PORT, SPI_3_MISO_PORTBIT, SPI_3_MISO_AF);
                    Gpio__PinConfig(SPI_3_MISO_PORT, SPI_3_MISO_PORTBIT, SPECIAL_FUNCTION_PUSH_PULL);
                #endif

                SPI_3.CR1 |= SPI_CR1_SPE;    //Enable the Spi2
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It disables a specific Spi device.
 *    @details  The Spi must be enabled by calling Spi_Enable() after disable before performing the Spi communication.
 *    @param    spi : specific spi device
 */
void Spi__Disable(SPI_ENUM_DEF spi)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))
        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                SPI_1.CR1 &= (uint16)(~SPI_CR1_SPE);          //Disable the Spi1
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                SPI_2.CR1 &= (uint16)(~SPI_CR1_SPE);          //Disable the Spi1
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                SPI_3.CR1 &= (uint16)(~SPI_CR1_SPE);          //Disable the Spi1
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method clears transmit and receive buffers of a specific Spi.
 *    @param    spi : specific spi device
 *    @image    html    Spi__Clear.jpg
 */
void Spi__Clear(SPI_ENUM_DEF spi)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))
        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                Dma__StopTransfer(SPI_1_TX_DMA_ENGINE, SPI_1_TX_DMA_CHANNEL);

                #if (SPI_1_TX_ONLY == DISABLED)
                    Dma__StopTransfer(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL);
                #endif

                Spi1_Buffer.Write = 0;

                #if (SPI_1_TX_ONLY == DISABLED)
                    Spi1_Buffer.Read = 0;
                #endif
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                Dma__StopTransfer(SPI_2_TX_DMA_ENGINE, SPI_2_TX_DMA_CHANNEL);

                #if (SPI_2_TX_ONLY == DISABLED)
                    Dma__StopTransfer(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL);
                #endif

                Spi2_Buffer.Write = 0;

                #if (SPI_2_TX_ONLY == DISABLED)
                    Spi2_Buffer.Read = 0;
                #endif
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                Dma__StopTransfer(SPI_3_TX_DMA_ENGINE, SPI_3_TX_DMA_CHANNEL);

                #if (SPI_3_TX_ONLY == DISABLED)
                    Dma__StopTransfer(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL);
                #endif

                Spi3_Buffer.Write = 0;

                #if (SPI_3_TX_ONLY == DISABLED)
                    Spi3_Buffer.Read = 0;
                #endif
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes data from "source" (buffer address mentioned by the Application)
 *              to the "destination" (Transmitter FIFO) as per the size mentioned to the specific Spi and also
 *              returns the response for the write operation.
 *    @details  The data in the Transmitter FIFO is sent to other Spi node through the specified Spi COM port
 *              whenever the Transmitter is free. Before writing into the Transmitter FIFO check for the
 *              FIFO availability using Spi__GetStatus() API to avoid the over writing on the existing data.
 *    @param    spi  : specific Spi device
 *    @param    buf  : source buffer from where the data to be sent
 *    @param    size : defines the amount of data to be sent
 *      @return   BOOL_TYPE
 *      @retval   TRUE(1)  : desired request is successfully executed
 *                FALSE(0) : desired request is not successfully executed
 *    @image    html    Spi__Write.jpg
 */
BOOL_TYPE Spi__Write(SPI_ENUM_DEF spi, uint8 * buf, uint16 size)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        BOOL_TYPE response;

        response = FALSE;

        if (size <= SPI_BUFFER_SIZE)
        {
            #if (SPI_1_FEATURE == ENABLED)
                if (spi == SPI1)
                {
                    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

                    //Wait till FTLVL == 0 and busy == 0
                    if (((SPI_1.CR1 & SPI_CR1_SPE) != FALSE) &&
                        (Dma__GetFlagStatus(SPI_1_TX_DMA_ENGINE, SPI_1_TX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING) &&

                        #if (SPI_1_TX_ONLY == DISABLED)
                            (Dma__GetFlagStatus(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING) &&
                        #endif

                        ((SPI_1.SR & SPI_SR_BSY) == FALSE))
                    {
                        //Clear the Buffer index
                        Spi1_Buffer.Write = 0;

                        //Clear the Dma Tx, Rx Bit
                        SPI_1.CR2 &= (uint16)(~( SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN));

                        #if (SPI_1_TX_ONLY == DISABLED)
                            Spi1_Buffer.Read = 0;
                        #endif

                        #if (SPI_1_TX_BY_REFERENCE == DISABLED)
                            memcpy((Spi1_Buffer.Tx_Data),buf, size);
                        #else
                            SPI_1_TX_DMA_CONFIG.DMA_MemBaseAddr = (DMA_ADDRESS_TYPE)buf;
                        #endif

                        Dma__Initialize(SPI_1_TX_DMA_ENGINE, SPI_1_TX_DMA_CHANNEL, (DMA_INIT_TYPE*)(&SPI_1_TX_DMA_CONFIG));

                        #if (SPI_1_TX_ONLY == DISABLED)
                            Dma__Initialize(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL, (DMA_INIT_TYPE*)(&SPI_1_RX_DMA_CONFIG));
                        #endif

                        if (Dma__StartTransfer(SPI_1_TX_DMA_ENGINE, SPI_1_TX_DMA_CHANNEL, size) == TRUE)
                        {
                            #if (SPI_1_TX_ONLY == DISABLED)
                            if (Dma__StartTransfer(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL, size) == TRUE)
                            #endif
                            {
                                #if (SPI_1_TX_ONLY == DISABLED)
                                    SPI_1.CR2 |= ( SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);        //Initiate DMA transfer
                                #else
                                    SPI_1.CR2 |= SPI_CR2_TXDMAEN;        //Initiate DMA transfer
                                #endif

                                Spi1_Buffer.Write = size;

                                response = TRUE;
                            }
                        }

                    }

                    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                }
            #endif


            #if (SPI_2_FEATURE == ENABLED)
                if (spi == SPI2)
                {
                    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

                    //Wait till FTLVL == 0 and busy == 0
                    if (((SPI_2.CR1 & SPI_CR1_SPE) != FALSE) &&
                        (Dma__GetFlagStatus(SPI_2_TX_DMA_ENGINE, SPI_2_TX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING) &&

                        #if (SPI_2_TX_ONLY == DISABLED)
                        (Dma__GetFlagStatus(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING) &&
                        #endif

                        ((SPI_2.SR & SPI_SR_BSY) == FALSE))
                    {
                        //Clear the Buffer index
                        Spi2_Buffer.Write = 0;

                        //Clear the Dma Tx, Rx Bit
                        SPI_2.CR2 &= (uint16)(~( SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN));

                        #if (SPI_2_TX_ONLY == DISABLED)
                            Spi2_Buffer.Read = 0;
                        #endif

                        #if (SPI_2_TX_BY_REFERENCE == DISABLED)
                            memcpy((Spi2_Buffer.Tx_Data),buf, size);
                        #else
                            SPI_2_TX_DMA_CONFIG.DMA_MemBaseAddr = (DMA_ADDRESS_TYPE)buf;
                        #endif

                        Dma__Initialize(SPI_2_TX_DMA_ENGINE, SPI_2_TX_DMA_CHANNEL, (DMA_INIT_TYPE*)(&SPI_2_TX_DMA_CONFIG));

                        #if (SPI_2_TX_ONLY == DISABLED)
                            Dma__Initialize(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL, (DMA_INIT_TYPE*)(&SPI_2_RX_DMA_CONFIG));
                        #endif

                        if (Dma__StartTransfer(SPI_2_TX_DMA_ENGINE, SPI_2_TX_DMA_CHANNEL, size) == TRUE)
                        {
                            #if (SPI_2_TX_ONLY == DISABLED)
                            if (Dma__StartTransfer(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL, size) == TRUE)
                            #endif
                            {
                                #if (SPI_2_TX_ONLY == DISABLED)
                                    SPI_2.CR2 |= ( SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);        //Initiate DMA transfer
                                #else
                                    SPI_2.CR2 |= SPI_CR2_TXDMAEN;        //Initiate DMA transfer
                                #endif
                                Spi2_Buffer.Write = size;

                                response = TRUE;
                            }
                        }
                        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                    }
                }
            #endif

            #if (SPI_3_FEATURE == ENABLED)
                if (spi == SPI3)
                {
                    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

                    //Wait till FTLVL == 0 and busy == 0
                    if (((SPI_3.CR1 & SPI_CR1_SPE) != FALSE) &&
                        (Dma__GetFlagStatus(SPI_3_TX_DMA_ENGINE, SPI_3_TX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING) &&

                        #if (SPI_3_TX_ONLY == DISABLED)
                        (Dma__GetFlagStatus(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING) &&
                        #endif

                        ((SPI_3.SR & SPI_SR_BSY) == FALSE))
                    {
                        //Clear the Buffer index
                        Spi3_Buffer.Write = 0;

                        //Clear the Dma Tx, Rx Bit
                        SPI_1.CR2 &= (uint16)(~( SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN));

                        #if (SPI_3_TX_ONLY == DISABLED)
                            Spi3_Buffer.Read = 0;
                        #endif

                        #if (SPI_3_TX_BY_REFERENCE == DISABLED)
                            memcpy((Spi3_Buffer.Tx_Data),buf, size);
                        #else
                            SPI_3_TX_DMA_CONFIG.DMA_MemBaseAddr = (DMA_ADDRESS_TYPE)buf;
                        #endif

                        Dma__Initialize(SPI_3_TX_DMA_ENGINE, SPI_3_TX_DMA_CHANNEL, (DMA_INIT_TYPE*)(&SPI_3_TX_DMA_CONFIG));

                        #if (SPI_3_TX_ONLY == DISABLED)
                            Dma__Initialize(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL, (DMA_INIT_TYPE*)(&SPI_3_RX_DMA_CONFIG));
                        #endif

                        if (Dma__StartTransfer(SPI_3_TX_DMA_ENGINE, SPI_3_TX_DMA_CHANNEL, size) == TRUE)
                        {
                            #if (SPI_3_TX_ONLY == DISABLED)
                            if (Dma__StartTransfer(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL, size) == TRUE)
                            #endif
                            {
                                #if (SPI_3_TX_ONLY == DISABLED)
                                    SPI_3.CR2 |= ( SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);        //Initiate DMA transfer
                                #else
                                    SPI_3.CR2 |= SPI_CR2_TXDMAEN;        //Initiate DMA transfer
                                #endif
                                Spi3_Buffer.Write = size;

                                response = TRUE;
                            }
                        }
                    }

                    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                }
            #endif

        }

    return (response);

    #else
        return(FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from "receive FIFO" and copies to the "address of the buffer" mentioned by the
 *              Application as per the size and also returns the response for the read operation.
 *    @details  Before reading the Receiver FIFO check for the
 *              data availability in the Receiver FIFO using Spi__GetStatus() API.
 *    @param    spi : specific spi device
 *    @param    buf : Indicates the buffer�s address where the read data is stored
 *    @param    size : defines the amount of data to be read
 *    @return   BOOL_TYPE
 *      @retval TRUE(1)   : desired request is successfully executed
 *      @retval FALSE(0)  : desired request is not successfully executed
 *    @image    html    Spi__Read.jpg
 */
BOOL_TYPE Spi__Read(SPI_ENUM_DEF spi, uint8 * buf, uint16 size)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))
        BOOL_TYPE response;

        response = FALSE;

        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                #if (SPI_1_TX_ONLY == DISABLED)

                    if (Dma__GetFlagStatus(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                    {
                        if (size <= (Spi1_Buffer.Write - Spi1_Buffer.Read))
                        {
                            memcpy(buf, (Spi1_Buffer.Rx_Data + Spi1_Buffer.Read), size);

                            Spi1_Buffer.Read = Spi1_Buffer.Read + size;

                            response = TRUE;
                        }
                    }
                #endif
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                #if (SPI_2_TX_ONLY == DISABLED)
                    if (Dma__GetFlagStatus(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                    {
                        if (size <= (Spi2_Buffer.Write - Spi2_Buffer.Read))
                        {
                            memcpy(buf, (Spi2_Buffer.Rx_Data + Spi2_Buffer.Read), size);

                            Spi2_Buffer.Read = Spi2_Buffer.Read + size;

                            response = TRUE;
                        }
                    }
                #endif
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                #if (SPI_3_TX_ONLY == DISABLED)
                    if (Dma__GetFlagStatus(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                    {
                        if (size <= (Spi3_Buffer.Write - Spi3_Buffer.Read))
                        {
                            memcpy(buf, (Spi3_Buffer.Rx_Data + Spi3_Buffer.Read), size);

                            Spi3_Buffer.Read = Spi3_Buffer.Read + size;

                            response = TRUE;
                        }
                    }
                #endif
            }
        #endif

    return (response);

    #else
        return(FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method gets the status of a specific SPI device.
 *    @details  This method is used to get status from specific SPI module.
 *    @param    spi : specific spi device
 *    @param    index : Index would be SPI_STATUS_STATE or SPI_STATUS_BUFFER_IN
 *    @return   For SPI_STATUS_STATE the return would be-
 *               SPI_STATE_BUSY � The Spi communication is going on
 *               SPI_STATE_IDLE � The Spi is ready to transfer the data
 *               SPI_STATE_ERROR � The Spi module encounter an error
 *               For SPI_STATUS_BUFFER_IN it returns the number of data bytes available to read
 */
uint16 Spi__GetStatus(SPI_ENUM_DEF spi, SPI_STATUS_DEF index)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))

        uint16 response;

        // This variable is used to clear Over run and Under run error by reading the Data and Status register
        uint16 aux;

        response = FALSE;

        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                if (index == SPI_STATUS_STATE)
                {
                    //Over run error, Under run error, Mode fault error
                    if (((SPI_1.SR & (SPI_SR_OVR | SPI_SR_UDR | SPI_SR_MODF) != FALSE) ||
                        (SPI_1.CR1 & SPI_CR1_SPE == FALSE)) &&
                        (Dma__GetFlagStatus(SPI_1_TX_DMA_ENGINE, SPI_1_TX_DMA_CHANNEL) == DMA_TRANSFER_ERROR))
                    {
                        //Over run and Under run error clear sequence
                        aux = SPI_1.DR;
                        aux = SPI_1.SR;

                        response = SPI_STATE_ERROR;
                    }
                    else
                    {
                        #if (SPI_1_TX_ONLY == DISABLED)
                            if (Dma__GetFlagStatus(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                            {
                                response = SPI_STATE_IDLE;
                            }
                            else
                            {
                                response = SPI_STATE_BUSY;
                            }
                        #else
                            response = SPI_STATE_IDLE;
                        #endif
                    }
                }
                else
                {
                    #if (SPI_1_TX_ONLY == DISABLED)
                        if (Dma__GetFlagStatus(SPI_1_RX_DMA_ENGINE, SPI_1_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                        {
                            response = Spi1_Buffer.Write - Spi1_Buffer.Read;
                        }
                    #endif
                }
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                if (index == SPI_STATUS_STATE)
                {
                    //Over run error, Under run error, Mode fault error
                    if (((SPI_2.SR & (SPI_SR_OVR | SPI_SR_UDR | SPI_SR_MODF) != FALSE) ||
                        (SPI_2.CR1 & SPI_CR1_SPE == FALSE)) &&
                        (Dma__GetFlagStatus(SPI_2_TX_DMA_ENGINE, SPI_2_TX_DMA_CHANNEL) == DMA_TRANSFER_ERROR))
                    {
                        //Over run and Under run error clear sequence
                        aux = SPI_2.DR;
                        aux = SPI_2.SR;

                        response = SPI_STATE_ERROR;
                    }
                    else
                    {
                        #if (SPI_2_TX_ONLY == DISABLED)
                            if (Dma__GetFlagStatus(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                            {
                                response = SPI_STATE_IDLE;
                            }
                            else
                            {
                                response = SPI_STATE_BUSY;
                            }
                        #else
                            response = SPI_STATE_IDLE;
                        #endif
                    }
                }
                else
                {
                    #if (SPI_2_TX_ONLY == DISABLED)
                        if (Dma__GetFlagStatus(SPI_2_RX_DMA_ENGINE, SPI_2_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                        {
                            response = Spi2_Buffer.Write - Spi2_Buffer.Read;
                        }
                    #endif
                }
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                if (index == SPI_STATUS_STATE)
                {
                    //Over run error, Under run error, Mode fault error
                    if (((SPI_3.SR & (SPI_SR_OVR | SPI_SR_UDR | SPI_SR_MODF) != FALSE) ||
                        (SPI_3.CR1 & SPI_CR1_SPE == FALSE)) &&
                        (Dma__GetFlagStatus(SPI_3_TX_DMA_ENGINE, SPI_3_TX_DMA_CHANNEL) == DMA_TRANSFER_ERROR))
                    {
                        //Over run and Under run error clear sequence
                        aux = SPI_3.DR;
                        aux = SPI_3.SR;

                        response = SPI_STATE_ERROR;
                    }
                    else
                    {
                        #if (SPI_3_TX_ONLY == DISABLED)
                            if (Dma__GetFlagStatus(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                            {
                                response = SPI_STATE_IDLE;
                            }
                            else
                            {
                                response = SPI_STATE_BUSY;
                            }
                        #else
                            response = SPI_STATE_IDLE;
                        #endif
                    }
                }
                else
                {
                    #if (SPI_3_TX_ONLY == DISABLED)
                        if (Dma__GetFlagStatus(SPI_3_RX_DMA_ENGINE, SPI_3_RX_DMA_CHANNEL) != DMA_TRANSFER_ONGOING)
                        {
                            response = Spi3_Buffer.Write - Spi3_Buffer.Read;
                        }
                    #endif
                }
            }
        #endif

    return (response);

    #else
        return(FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reconfigure a specific Spi for a customised setting
 *
 *  @details    This function changes the configureation of a specific SPI and must called after the Spi has being initialzie
 *              Every time that the Spi is initialized it starts with the default parameters
 *
 *  @param      spi the specific spi device
 *  @param      pol defines the polarity of SCK signal
 *  @param      edge defines the the edge of action for the SCK line
 *  @param      order defines if the MSB bit goes first or the LSB bit goes first
 *  @image    html    Spi__SetConfiguration.jpg
 */
void Spi__SetConfiguration(SPI_ENUM_DEF spi,SPI_POLARITY_TYPE pol,SPI_EDGE_TYPE edge,SPI_ORDER_TYPE order)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))

        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                SPI_1.CR1 &= (uint16)((~SPI_CR1_SPE)&(~SPI_CR1_LSBFIRST)&(~SPI_CR1_CPOL)&(~SPI_CR1_CPHA));  //keep it disable until the end of the configuration

                //Order, Polarity and Edge
                SPI_1.CR1 |= (((order & 0x01 ) << SPI_CR1_LSBFIRST_POS ) | ((pol & 0x01 ) << SPI_CR1_CPOL_POS ) | (edge & 0x01 ));
                SPI_1.CR1 |= SPI_CR1_SPE;
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                SPI_2.CR1 &= (uint16)((~SPI_CR1_SPE)&(~SPI_CR1_LSBFIRST)&(~SPI_CR1_CPOL)&(~SPI_CR1_CPHA));  //keep it disable until the end of the configuration

                //Order, Polarity and Edge
                SPI_2.CR1 |= (((order & 0x01 ) << SPI_CR1_LSBFIRST_POS ) | ((pol & 0x01 ) << SPI_CR1_CPOL_POS ) | (edge & 0x01 ));
                SPI_2.CR1 |= SPI_CR1_SPE;
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                SPI_3.CR1 &= (uint16)((~SPI_CR1_SPE)&(~SPI_CR1_LSBFIRST)&(~SPI_CR1_CPOL)&(~SPI_CR1_CPHA));  //keep it disable until the end of the configuration

                //Order, Polarity and Edge
                SPI_3.CR1 |= (((order & 0x01 ) << SPI_CR1_LSBFIRST_POS ) | ((pol & 0x01 ) << SPI_CR1_CPOL_POS ) | (edge & 0x01 ));
                SPI_3.CR1 |= SPI_CR1_SPE;
            }
        #endif

    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method is used to change the speed of the SPI peripheral in running time.
 *    @param    spi
 *    @param    speed index to the speed : SPI_SPEED_1,SPI_SPEED_2,SPI_SPEED_3,SPI_SPEED_4 or SPI_SPEED_5
 */
void Spi__SetSpeed(SPI_ENUM_DEF spi, SPI_SPEED_TYPE speed)
{
    #if ((SPI_1_FEATURE == ENABLED) || (SPI_2_FEATURE == ENABLED) || (SPI_3_FEATURE == ENABLED))
        #if (SPI_1_FEATURE == ENABLED)
            if (spi == SPI1)
            {
                //clear the baud bits
                SPI_1.CR1 &= (~SPI_CR1_BR);

                switch (speed)
                {
                    case SPI_SPEED_1:
                        SPI_1.CR1 |= (SPI_CR1_BR & (SPI_1_PRESCALER_SPEED1 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_2:
                        SPI_1.CR1 |= (SPI_CR1_BR & (SPI_1_PRESCALER_SPEED2 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_3:
                        SPI_1.CR1 |= (SPI_CR1_BR & (SPI_1_PRESCALER_SPEED3 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_4:
                        SPI_1.CR1 |= (SPI_CR1_BR & (SPI_1_PRESCALER_SPEED4 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_5:
                        SPI_1.CR1 |= (SPI_CR1_BR & (SPI_1_PRESCALER_SPEED5 << SPI_CR1_BR_POS));
                        break;
                    default:
                        //sets to the baud rate set in the Spi__Initialize() API
                        SPI_1.CR1 |= (SPI_CR1_BR & SPI_1_CR1_BR);
                        break;
                }
            }
        #endif

        #if (SPI_2_FEATURE == ENABLED)
            if (spi == SPI2)
            {
                //clear the baud bits
                SPI_2.CR1 &= (~SPI_CR1_BR);

                switch (speed)
                {
                    case SPI_SPEED_1:
                        SPI_2.CR1 |= (SPI_CR1_BR & (SPI_2_PRESCALER_SPEED1 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_2:
                        SPI_2.CR1 |= (SPI_CR1_BR & (SPI_2_PRESCALER_SPEED2 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_3:
                        SPI_2.CR1 |= (SPI_CR1_BR & (SPI_2_PRESCALER_SPEED3 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_4:
                        SPI_2.CR1 |= (SPI_CR1_BR & (SPI_2_PRESCALER_SPEED4 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_5:
                        SPI_2.CR1 |= (SPI_CR1_BR & (SPI_2_PRESCALER_SPEED5 << SPI_CR1_BR_POS));
                        break;
                    default:
                        //sets to the baud rate set in the Spi__Initialize() API
                        SPI_2.CR1 |= (SPI_CR1_BR & SPI_2_CR1_BR);
                        break;
                }
            }
        #endif

        #if (SPI_3_FEATURE == ENABLED)
            if (spi == SPI3)
            {
                //clear the baud bits
                SPI_3.CR1 &= (~SPI_CR1_BR);

                switch (speed)
                {
                    case SPI_SPEED_1:
                        SPI_3.CR1 |= (SPI_CR1_BR & (SPI_3_PRESCALER_SPEED1 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_2:
                        SPI_3.CR1 |= (SPI_CR1_BR & (SPI_3_PRESCALER_SPEED2 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_3:
                        SPI_3.CR1 |= (SPI_CR1_BR & (SPI_3_PRESCALER_SPEED3 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_4:
                        SPI_3.CR1 |= (SPI_CR1_BR & (SPI_3_PRESCALER_SPEED4 << SPI_CR1_BR_POS));
                        break;
                    case SPI_SPEED_5:
                        SPI_3.CR1 |= (SPI_CR1_BR & (SPI_3_PRESCALER_SPEED5 << SPI_CR1_BR_POS));
                        break;
                    default:
                        //sets to the baud rate set in the Spi__Initialize() API
                        SPI_3.CR1 |= (SPI_CR1_BR & SPI_3_CR1_BR);
                        break;
                }
            }
        #endif
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
