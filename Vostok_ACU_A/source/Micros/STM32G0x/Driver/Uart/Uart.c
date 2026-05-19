/**
 *  @file
 *  @brief    Uart GDM module for STM32G0x implements the Uart Standard API (Application Programming Interface).
 *  @details  This module provides the interface to the application such as initialization of Uart,\n
 *              transmission and reception of serial data.
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
#include "Micro.h"
#include "Gpio.h"
#include "Uart.h"


//=====================================================================================================================
//Compiler directive
//=====================================================================================================================
#ifndef UART_POLLING_MODE
    #define UART_POLLING_MODE           DISABLED
#endif

//! Enable the Variables, Constants & Defines only if the either one of the UART is enabled
#ifndef COM1_PREEMPTION_PRIORITY
    #define COM1_PREEMPTION_PRIORITY                0
#elif (COM1_PREEMPTION_PRIORITY > 3)
    #error " COM1_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
#endif

#ifndef COM2_PREEMPTION_PRIORITY
    #define COM2_PREEMPTION_PRIORITY                0
#elif (COM2_PREEMPTION_PRIORITY > 3)
    #error " COM2_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
#endif

#ifndef COM3_PREEMPTION_PRIORITY
    #define COM3_PREEMPTION_PRIORITY                0
#elif (COM3_PREEMPTION_PRIORITY > 3)
    #error " COM3_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
#endif

#ifndef COM4_PREEMPTION_PRIORITY
    #define COM4_PREEMPTION_PRIORITY                0
#elif (COM4_PREEMPTION_PRIORITY > 4)
    #error " COM4_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
#endif

#ifndef COM5_PREEMPTION_PRIORITY
    #define COM5_PREEMPTION_PRIORITY                0
#elif (COM3_PREEMPTION_PRIORITY > 3)
    #error " COM5_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
#endif

#ifndef COM6_PREEMPTION_PRIORITY
    #define COM6_PREEMPTION_PRIORITY                0
#elif (COM6_PREEMPTION_PRIORITY > 4)
    #error " COM6_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define  USART_ICR_CLEAR_ALL_FLAGS              ((uint32)(  USART_ICR_PECF      | \
                                                            USART_ICR_FECF      | \
                                                            USART_ICR_NECF      | \
                                                            USART_ICR_ORECF     | \
                                                            USART_ICR_IDLECF    | \
                                                            USART_ICR_TXFECF    | \
                                                            USART_ICR_TCCF      | \
                                                            USART_ICR_LBDCF     | \
                                                            USART_ICR_CTSCF     | \
                                                            USART_ICR_CMCF      | \
                                                            USART_ICR_WUCF      | \
                                                            USART_ICR_RTOCF ))


#if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
    static const uint32 BDRATE[] = {1200,2400,4800,9600,19200,38400,57600,115200};

    static USART_TYPEDEF * const UART_PTR[] =
    {
        NULL,

        #if (UART1_FEATURE == ENABLED)
            &USART1,
        #else
            NULL,
        #endif

        #if (UART2_FEATURE == ENABLED)
            &USART2,
        #else
            NULL,
        #endif

        #if (UART3_FEATURE == ENABLED)
            &USART3,
        #else
            NULL,
        #endif

        #if (UART4_FEATURE == ENABLED)
            &USART4,
        #else
            NULL,
        #endif

        #if (UART5_FEATURE == ENABLED)
            &USART5,
        #else
            NULL,
        #endif

        #if (UART6_FEATURE == ENABLED)
            &USART6,
        #else
            NULL,
        #endif
     };

    typedef struct
    {
        uint16 parity_err      :1;
        uint16 frame_err       :1;
        uint16 noise_err       :1;
        uint16 overrun_err     :1;
        uint16 incomming_break :1;
        uint16 data            :8;
        uint16 unused12_to_15  :3;
    } DATA_REGISTER_TYPE;

    #if (UART1_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART1_Rx_Buffer[UART1_RX_BUF_SIZE];
        static uint8 UART1_Tx_Buffer[UART1_TX_BUF_SIZE];
        static uint8 UART1_Tx_Write_Index;
        static uint8 UART1_Tx_Read_Index;
        static uint8 UART1_Rx_Write_Index;
        static uint8 UART1_Rx_Read_Index;
        static uint8 UART1_Break;
        static DATA_REGISTER_TYPE UART1_LastData;
    #endif

    #if (UART2_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART2_Rx_Buffer[UART2_RX_BUF_SIZE];
        static uint8 UART2_Tx_Buffer[UART2_TX_BUF_SIZE];
        static uint8 UART2_Tx_Write_Index;
        static uint8 UART2_Tx_Read_Index;
        static uint8 UART2_Rx_Write_Index;
        static uint8 UART2_Rx_Read_Index;
        static uint8 UART2_Break;
        static DATA_REGISTER_TYPE  UART2_LastData;
    #endif

    #if (UART3_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART3_Rx_Buffer[UART3_RX_BUF_SIZE];
        static uint8 UART3_Tx_Buffer[UART3_TX_BUF_SIZE];
        static uint8 UART3_Tx_Write_Index;
        static uint8 UART3_Tx_Read_Index;
        static uint8 UART3_Rx_Write_Index;
        static uint8 UART3_Rx_Read_Index;
        static uint8 UART3_Break;
        static DATA_REGISTER_TYPE UART3_LastData;
    #endif

    #if (UART4_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART4_Rx_Buffer[UART4_RX_BUF_SIZE];
        static uint8 UART4_Tx_Buffer[UART4_TX_BUF_SIZE];
        static uint8 UART4_Tx_Write_Index;
        static uint8 UART4_Tx_Read_Index;
        static uint8 UART4_Rx_Write_Index;
        static uint8 UART4_Rx_Read_Index;
        static uint8 UART4_Break;
        static DATA_REGISTER_TYPE  UART4_LastData;
    #endif

    #if (UART5_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART5_Rx_Buffer[UART5_RX_BUF_SIZE];
        static uint8 UART5_Tx_Buffer[UART5_TX_BUF_SIZE];
        static uint8 UART5_Tx_Write_Index;
        static uint8 UART5_Tx_Read_Index;
        static uint8 UART5_Rx_Write_Index;
        static uint8 UART5_Rx_Read_Index;
        static uint8 UART5_Break;
        static DATA_REGISTER_TYPE  UART5_LastData;
    #endif

    #if (UART6_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART6_Rx_Buffer[UART6_RX_BUF_SIZE];
        static uint8 UART6_Tx_Buffer[UART6_TX_BUF_SIZE];
        static uint8 UART6_Tx_Write_Index;
        static uint8 UART6_Tx_Read_Index;
        static uint8 UART6_Rx_Write_Index;
        static uint8 UART6_Rx_Read_Index;
        static uint8 UART6_Break;
        static DATA_REGISTER_TYPE  UART6_LastData;
    #endif
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  Configures the specified port-pin into the Alternate function.
 */
#define GPIO__PIN_COFIG_AF(PORT, PORTBIT, AF)       {(*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] = (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).AFR[PORTBIT >> 0x03] | ((uint32)(AF) << ((uint32)((uint32)PORTBIT & (uint32)0x07) << 2));}

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes a specific Uart with given configuration such as baud rate (i.e. speed),\n
 *              stop bits and parity. It also initializes the Uart module variables.
 *    @details  Before using Uart module this method should be called to initialize specific Uart.
 *    @param    uart    : Defines the Uart COM port
 *    @param    speed   : Defines the baud rate for Uart
 *    @param    stops   : Defines the stop bit for Uart
 *    @param    parity  : Defines the parity bit for Uart
 *    @return   BOOL_TYPE
 *      @retval TRUE (1)    :  If the Uart is successfully initialized.
 *      @retval FALSE (0)   : Failed to initialize the Uart.
 *
 *    @image    html    Uart__Initialize.jpg
 */
BOOL_TYPE Uart__Initialize(UART_ENUM_DEF uart, UART_SPEED_DEF speed, UART_STOP_DEF stops, UART_PARITY_DEF parity )
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        uint32 tmpreg;
        uint32 divider;
        uint32 apbclock;
        BOOL_TYPE response;
        USART_TYPEDEF * com;

        apbclock = 0;
        divider = 0;
        tmpreg = 0;
        response = FALSE;
        com = NULL;

        if (uart <= COM6 )
        {
            com = UART_PTR[uart];

            if (com != NULL)
            {
                if (speed <=B230400)
                {
                    Uart__Clear(uart);

                    #if (UART1_FEATURE == ENABLED)
                        if (uart == COM1)
                        {
                            //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                            GPIO__PIN_SET(UART_1_TX_PORT, UART_1_TX_PORTBIT)
                            GPIO__PIN_COFIG_O_PUSHPULL(UART_1_TX_PORT, UART_1_TX_PORTBIT);

                            //Enable the clock for Uart module
                            RCC.APBENR2 |= RCC_APBENR2_USART1EN;

                            //Reset the UART
                            RCC.APBRSTR2 |= RCC_APBRSTR2_USART1RST;
                            RCC.APBRSTR2 &= (uint32)(~(uint32)RCC_APBRSTR2_USART1RST);

                            //Set SYSCLK(64MHz) as Clock source
                            if ((RCC.CCIPR & RCC_CCIPR_USART1SEL) != LL_RCC_USART1_CLKSOURCE_SYSCLK)
                            {
                                RCC.CCIPR &= ~RCC_CCIPR_USART1SEL;
                            }

                            RCC.CCIPR |= LL_RCC_USART1_CLKSOURCE_SYSCLK;
                        }
                    #endif

                    #if (UART2_FEATURE == ENABLED)
                        if (uart == COM2)
                        {
                            //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                            GPIO__PIN_SET(UART_2_TX_PORT, UART_2_TX_PORTBIT)
                            GPIO__PIN_COFIG_O_PUSHPULL(UART_2_TX_PORT, UART_2_TX_PORTBIT);

                            //Enable the clock for Uart module
                            RCC.APBENR1 |= RCC_APBENR1_USART2EN;

                            //Reset the UART
                            RCC.APBRSTR1 |= RCC_APBRSTR1_USART2RST;
                            RCC.APBRSTR1 &= (uint32)(~(uint32)RCC_APBRSTR1_USART2RST);

                            //Set SYSCLK(64MHz) as Clock source
                            if ((RCC.CCIPR & RCC_CCIPR_USART2SEL) != LL_RCC_USART2_CLKSOURCE_SYSCLK)
                            {
                                RCC.CCIPR &= ~RCC_CCIPR_USART2SEL;
                            }

                            RCC.CCIPR |= LL_RCC_USART2_CLKSOURCE_SYSCLK;
                        }
                    #endif

                    #if (UART3_FEATURE == ENABLED)
                        if (uart == COM3)
                        {
                            //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                            GPIO__PIN_SET(UART_3_TX_PORT, UART_3_TX_PORTBIT)
                            GPIO__PIN_COFIG_O_PUSHPULL(UART_3_TX_PORT, UART_3_TX_PORTBIT);

                            //Enable the clock for Uart module
                            RCC.APBENR1 |= RCC_APBENR1_USART3EN;

                            //Reset the UART
                            RCC.APBRSTR1 |= RCC_APBRSTR1_USART3RST;
                            RCC.APBRSTR1 &= (uint32)(~(uint32)RCC_APBRSTR1_USART3RST);

                            //Set SYSCLK(64MHz) as Clock source
                            if ((RCC.CCIPR & RCC_CCIPR_USART3SEL) != LL_RCC_USART3_CLKSOURCE_SYSCLK)
                            {
                                RCC.CCIPR &= ~RCC_CCIPR_USART3SEL;
                            }

                            RCC.CCIPR |= LL_RCC_USART3_CLKSOURCE_SYSCLK;
                        }
                    #endif

                    #if (UART4_FEATURE == ENABLED)
                        if (uart == COM4)
                        {
                            //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                            GPIO__PIN_SET(UART_4_TX_PORT, UART_4_TX_PORTBIT)
                            GPIO__PIN_COFIG_O_PUSHPULL(UART_4_TX_PORT, UART_4_TX_PORTBIT);

                            //Enable the clock for Uart module
                            RCC.APBENR1 |= RCC_APBENR1_USART4EN;

                            //Reset the UART
                            RCC.APBRSTR1 |= RCC_APBRSTR1_USART4RST;
                            RCC.APBRSTR1 &= (uint32)(~(uint32)RCC_APBRSTR1_USART4RST);

                            //Set SYSCLK(64MHz) as Clock source
                            if ((RCC.CCIPR & RCC_CCIPR_USART3SEL) != LL_RCC_USART3_CLKSOURCE_SYSCLK)
                            {
                                RCC.CCIPR &= ~RCC_CCIPR_USART3SEL;
                            }

                            RCC.CCIPR |= LL_RCC_USART3_CLKSOURCE_SYSCLK;
                        }
                    #endif

                    #if (UART5_FEATURE == ENABLED)
                        if (uart == COM5)
                        {
                            //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                            GPIO__PIN_SET(UART_5_TX_PORT, UART_5_TX_PORTBIT)
                            GPIO__PIN_COFIG_O_PUSHPULL(UART_5_TX_PORT, UART_5_TX_PORTBIT);

                            //Enable the clock for Uart module
                            RCC.APBENR1 |= RCC_APBENR1_USART5EN;

                            //Reset the UART
                            RCC.APBRSTR1 |= RCC_APBRSTR1_USART5RST;
                            RCC.APBRSTR1 &= (uint32)(~(uint32)RCC_APBRSTR1_USART5RST);

                            //Set SYSCLK(64MHz) as Clock source
                            if ((RCC.CCIPR & RCC_CCIPR_USART3SEL) != LL_RCC_USART3_CLKSOURCE_SYSCLK)
                            {
                                RCC.CCIPR &= ~RCC_CCIPR_USART3SEL;
                            }

                            RCC.CCIPR |= LL_RCC_USART3_CLKSOURCE_SYSCLK;
                        }
                    #endif

                    #if (UART6_FEATURE == ENABLED)
                        if (uart == COM6)
                        {
                            //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                            GPIO__PIN_SET(UART_6_TX_PORT, UART_6_TX_PORTBIT)
                            GPIO__PIN_COFIG_O_PUSHPULL(UART_6_TX_PORT, UART_6_TX_PORTBIT);

                            //Enable the clock for Uart module
                            RCC.APBENR1 |= RCC_APBENR1_USART6EN;

                            //Reset the UART
                            RCC.APBRSTR1 |= RCC_APBRSTR1_USART6RST;
                            RCC.APBRSTR1 &= (uint32)(~(uint32)RCC_APBRSTR1_USART6RST);

                            //Set SYSCLK(64MHz) as Clock source
                            if ((RCC.CCIPR & RCC_CCIPR_USART3SEL) != LL_RCC_USART3_CLKSOURCE_SYSCLK)
                            {
                                RCC.CCIPR &= ~RCC_CCIPR_USART3SEL;
                            }

                            RCC.CCIPR |= LL_RCC_USART3_CLKSOURCE_SYSCLK;
                        }
                    #endif

                    // Disable USART
                    com->CR1 = 0;

                    //Prescaler is set not to divide

                    //---------------------------- USART CR2 Configuration ----------------------
                    // Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit ------------
                    // Set STOP[13:12] bits according to USART_StopBits value
                    // Stop Bits
                    if (stops == UART_STOP_BIT_2)
                    {
                        com->CR2 = (uint32)UART_STOPBITS_2; // 1 Stop Bit by default
                    }


                    //---------------------------- USART CR1 Configuration -----------------------
                    // Configure the USART Word Length, Parity and mode -----------------------
                    // start bit, 8 Data bits, n Stop bit
                    // FIFO disabled
                    // Set PCE(Parity Control) and PS(Parity selection) as per the request
                    // Over sampling by 16 method
                    if (parity == UART_PARITY_EVEN)
                    {
                        com->CR1 = UART_CR1_PARITY_EVEN;
                    }
                    else if (parity == UART_PARITY_ODD)
                    {
                        com->CR1 = UART_CR1_PARITY_ODD;
                    }


                    //---------------------------- USART BRR Configuration -----------------------
                    // Configure the USART Baud Rate ---------------------------------------------
                    apbclock = Micro__GetClock()->PCLK1_Frequency;

                    // (divider * 10) computing in case Oversampling mode is 16 Samples
                    divider = (uint32)((apbclock) / (BDRATE[speed]));
                    tmpreg  = (uint32)((apbclock) % (BDRATE[speed]));

                    // round the divider : if fractional part i greater than 0.5 increment divider
                    if (tmpreg >=  (BDRATE[speed]) / 2)
                    {
                        divider++;
                    }

                    // Write to USART BRR
                    com->BRR = (uint16)divider;

                    #if (UART_POLLING_MODE == DISABLED)
                        //Set Priority for UART
                        #if (UART1_FEATURE == ENABLED)
                            if (uart == COM1)
                            {
                                Micro__NVICEnableIRQ (USART1_IRQ_CHANNEL, COM1_PREEMPTION_PRIORITY);
                            }
                        #endif

                        #if (UART2_FEATURE == ENABLED)
                            if (uart == COM2)
                            {
                                Micro__NVICEnableIRQ (USART2_IRQ_CHANNEL, COM2_PREEMPTION_PRIORITY);
                            }
                        #endif

                        #if ((UART3_FEATURE == ENABLED) || (UART4_FEATURE == ENABLED) || (UART5_FEATURE == ENABLED) || (UART6_FEATURE == ENABLED))
                            if ((COM3 <= uart) || (uart <= COM6))
                            {
                                Micro__NVICEnableIRQ (USART3_4_5_6_LPUART1_IRQ_CHANNEL, COM3_PREEMPTION_PRIORITY);
                            }
                        #endif
                    #endif

                    // Enable the device
                    Uart__Enable(uart, UART_OPTION_ALL);

                    response = TRUE;
                }
            }
        }
    #else
        uint8 response;
        response = FALSE;
    #endif

    return response;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method clears transmit and receive FIFO buffers of a specific uart.
 *
 *  @param      uart: uart enumerator
 *    @image    html    Uart__Clear.jpg
 */
void Uart__Clear(UART_ENUM_DEF uart)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                UART1_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART1_Tx_Write_Index = 0;
                UART1_Tx_Read_Index = 0;
                UART1_Rx_Write_Index = 0;
                UART1_Rx_Read_Index = 0;
                *((uint16 *)(&UART1_LastData)) = 0;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                UART2_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART2_Tx_Write_Index = 0;
                UART2_Tx_Read_Index = 0;
                UART2_Rx_Write_Index = 0;
                UART2_Rx_Read_Index = 0;
                *((uint16 *)(&UART2_LastData)) = 0;
            }
        #endif

        #if (UART3_FEATURE == ENABLED)
            if (uart == COM3)
            {
                UART3_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART3_Tx_Write_Index = 0;
                UART3_Tx_Read_Index = 0;
                UART3_Rx_Write_Index = 0;
                UART3_Rx_Read_Index = 0;
                *((uint16 *)(&UART3_LastData)) = 0;
            }
        #endif

        #if (UART4_FEATURE == ENABLED)
            if (uart == COM4)
            {
                UART4_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART4_Tx_Write_Index = 0;
                UART4_Tx_Read_Index = 0;
                UART4_Rx_Write_Index = 0;
                UART4_Rx_Read_Index = 0;
                *((uint16 *)(&UART4_LastData)) = 0;
            }
        #endif

        #if (UART5_FEATURE == ENABLED)
            if (uart == COM5)
            {
                UART5_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART5_Tx_Write_Index = 0;
                UART5_Tx_Read_Index = 0;
                UART5_Rx_Write_Index = 0;
                UART5_Rx_Read_Index = 0;
                *((uint16 *)(&UART5_LastData)) = 0;
            }

        #if (UART6_FEATURE == ENABLED)
            if (uart == COM6)
            {
                UART6_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART6_Tx_Write_Index = 0;
                UART6_Tx_Read_Index = 0;
                UART6_Rx_Write_Index = 0;
                UART6_Rx_Read_Index = 0;
                *((uint16 *)(&UART6_LastData)) = 0;
            }
        #endif

    #endif
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method clears the transmit FIFO buffers of a specific Uart.
 * @param uart    : Defines the Uart COM port
 */
void Uart__ClearTx(UART_ENUM_DEF uart)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART1_Tx_Write_Index = 0;
                UART1_Tx_Read_Index = 0;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART2_Tx_Write_Index = 0;
                UART2_Tx_Read_Index = 0;
            }
        #endif

        #if (UART3_FEATURE == ENABLED)
            if (uart == COM3)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART3_Tx_Write_Index = 0;
                UART3_Tx_Read_Index = 0;
            }
        #endif

        #if (UART4_FEATURE == ENABLED)
            if (uart == COM4)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART4_Tx_Write_Index = 0;
                UART4_Tx_Read_Index = 0;
            }
        #endif

        #if (UART5_FEATURE == ENABLED)
            if (uart == COM5)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART5_Tx_Write_Index = 0;
                UART5_Tx_Read_Index = 0;
            }
        #endif

        #if (UART6_FEATURE == ENABLED)
            if (uart == COM6)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART6_Tx_Write_Index = 0;
                UART6_Tx_Read_Index = 0;
            }
        #endif

        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method enables transmission, reception or both depending on option for
 *              specific uart.
 *    @details  This method enables an option (UART_OPTION_ALL,UART_OPTION_TX_EN,UART_OPTION_RX_EN)
 *              for a specific Uart.
 *    @param    uart   : Defines the Specific Uart COM port
 *    @param    option : Defines the option to enable the transmitter, receiver or both of the
 *                       specific Uart.
 *
 *    @image    html    Uart__Enable.jpg
 */
void Uart__Enable(UART_ENUM_DEF uart,UART_ENABLE_OPTIONS_DEF option)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
    USART_TYPEDEF * com;

    if (uart <= COM6 )
    {
        com = UART_PTR[uart];

        if (com != NULL)
        {
            switch (option)
            {
                case UART_OPTION_ALL:
                    com->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_PEIE);
                    com->CR3 = USART_CR3_EIE;

                    #if (UART1_FEATURE == ENABLED)
                        if (uart == COM1)
                        {
                            //Configure the Uart Tx and Rx port-pin
                            //Uart Tx, Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_1_TX_PORT, UART_1_TX_PORTBIT, UART_1_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_1_TX_PORT, UART_1_TX_PORTBIT);

                            GPIO__PIN_COFIG_AF(UART_1_RX_PORT, UART_1_RX_PORTBIT, UART_1_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_1_RX_PORT, UART_1_RX_PORTBIT);
                        }
                    #endif

                    #if (UART2_FEATURE == ENABLED)
                        if (uart == COM2)
                        {
                            //Configure the Uart Tx and Rx port-pin
                            //Uart Tx, Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_2_TX_PORT, UART_2_TX_PORTBIT, UART_2_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_2_TX_PORT, UART_2_TX_PORTBIT);

                            GPIO__PIN_COFIG_AF(UART_2_RX_PORT, UART_2_RX_PORTBIT, UART_2_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_2_RX_PORT, UART_2_RX_PORTBIT);
                        }
                    #endif

                    #if (UART3_FEATURE == ENABLED)
                        if (uart == COM3)
                        {
                            //Configure the Uart Tx and Rx port-pin
                            //Uart Tx, Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_3_TX_PORT, UART_3_TX_PORTBIT, UART_3_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_3_TX_PORT, UART_3_TX_PORTBIT);

                            GPIO__PIN_COFIG_AF(UART_3_RX_PORT, UART_3_RX_PORTBIT, UART_3_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_3_RX_PORT, UART_3_RX_PORTBIT);
                        }
                    #endif

                    #if (UART4_FEATURE == ENABLED)
                        if (uart == COM4)
                        {
                            //Configure the Uart Tx and Rx port-pin
                            //Uart Tx, Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_4_TX_PORT, UART_4_TX_PORTBIT, UART_4_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_4_TX_PORT, UART_4_TX_PORTBIT);

                            GPIO__PIN_COFIG_AF(UART_4_RX_PORT, UART_4_RX_PORTBIT, UART_4_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_4_RX_PORT, UART_4_RX_PORTBIT);
                        }
                    #endif

                    #if (UART5_FEATURE == ENABLED)
                        if (uart == COM5)
                        {
                            //Configure the Uart Tx and Rx port-pin
                            //Uart Tx, Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_5_TX_PORT, UART_5_TX_PORTBIT, UART_5_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_5_TX_PORT, UART_5_TX_PORTBIT);

                            GPIO__PIN_COFIG_AF(UART_5_RX_PORT, UART_5_RX_PORTBIT, UART_5_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_5_RX_PORT, UART_5_RX_PORTBIT);
                        }
                    #endif

                    #if (UART6_FEATURE == ENABLED)
                        if (uart == COM6)
                        {
                            //Configure the Uart Tx and Rx port-pin
                            //Uart Tx, Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_6_TX_PORT, UART_6_TX_PORTBIT, UART_6_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_6_TX_PORT, UART_6_TX_PORTBIT);

                            GPIO__PIN_COFIG_AF(UART_6_RX_PORT, UART_6_RX_PORTBIT, UART_6_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_6_RX_PORT, UART_6_RX_PORTBIT);
                        }
                    #endif
                    break;

                case UART_OPTION_TX_EN:
                    com->CR1 |= (USART_CR1_TE | USART_CR1_TXEIE);
                    com->CR3 = USART_CR3_EIE;

                    #if (UART1_FEATURE == ENABLED)
                        if (uart == COM1)
                        {
                            //Configure the Uart Tx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_1_TX_PORT, UART_1_TX_PORTBIT, UART_1_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_1_TX_PORT, UART_1_TX_PORTBIT);
                        }
                    #endif

                    #if (UART2_FEATURE == ENABLED)
                        if (uart == COM2)
                        {
                            //Configure the Uart Tx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_2_TX_PORT, UART_2_TX_PORTBIT, UART_2_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_2_TX_PORT, UART_2_TX_PORTBIT);
                        }
                    #endif

                    #if (UART3_FEATURE == ENABLED)
                        if (uart == COM3)
                        {
                            //Configure the Uart Tx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_3_TX_PORT, UART_3_TX_PORTBIT, UART_3_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_3_TX_PORT, UART_3_TX_PORTBIT);
                        }
                    #endif

                    #if (UART4_FEATURE == ENABLED)
                        if (uart == COM4)
                        {
                            //Configure the Uart Tx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_4_TX_PORT, UART_4_TX_PORTBIT, UART_4_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_4_TX_PORT, UART_4_TX_PORTBIT);
                        }
                    #endif

                    #if (UART5_FEATURE == ENABLED)
                        if (uart == COM5)
                        {
                            //Configure the Uart Tx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_5_TX_PORT, UART_5_TX_PORTBIT, UART_5_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_5_TX_PORT, UART_5_TX_PORTBIT);
                        }
                    #endif

                    #if (UART6_FEATURE == ENABLED)
                        if (uart == COM6)
                        {
                            //Configure the Uart Tx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_6_TX_PORT, UART_6_TX_PORTBIT, UART_6_TX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_6_TX_PORT, UART_6_TX_PORTBIT);
                        }
                    #endif

                    break;

                case UART_OPTION_RX_EN:
                    com->CR1 |= (USART_CR1_RE | USART_CR1_RXNEIE);
                    com->CR3 = USART_CR3_EIE;

                    #if (UART1_FEATURE == ENABLED)
                        if (uart == COM1)
                        {
                            //Configure the Uart Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_1_RX_PORT, UART_1_RX_PORTBIT, UART_1_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_1_RX_PORT, UART_1_RX_PORTBIT);
                        }
                    #endif

                    #if (UART2_FEATURE == ENABLED)
                        if (uart == COM2)
                        {
                            //Configure the Uart Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_2_RX_PORT, UART_2_RX_PORTBIT, UART_2_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_2_RX_PORT, UART_2_RX_PORTBIT);
                        }
                    #endif

                    #if (UART3_FEATURE == ENABLED)
                        if (uart == COM3)
                        {
                            //Configure the Uart Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_3_RX_PORT, UART_3_RX_PORTBIT, UART_3_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_3_RX_PORT, UART_3_RX_PORTBIT);
                        }
                    #endif

                    #if (UART4_FEATURE == ENABLED)
                        if (uart == COM4)
                        {
                            //Configure the Uart Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_4_RX_PORT, UART_4_RX_PORTBIT, UART_4_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_4_RX_PORT, UART_4_RX_PORTBIT);
                        }
                    #endif

                    #if (UART5_FEATURE == ENABLED)
                        if (uart == COM5)
                        {
                            //Configure the Uart Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_5_RX_PORT, UART_5_RX_PORTBIT, UART_5_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_5_RX_PORT, UART_5_RX_PORTBIT);
                        }
                    #endif

                    #if (UART6_FEATURE == ENABLED)
                        if (uart == COM6)
                        {
                            //Configure the Uart Rx Pin configuration
                            GPIO__PIN_COFIG_AF(UART_6_RX_PORT, UART_6_RX_PORTBIT, UART_6_RX_AF);
                            GPIO__PIN_COFIG_SF_PUSHPULL(UART_6_RX_PORT, UART_6_RX_PORTBIT);
                        }
                    #endif

                    break;


                default:
                    break;
            }
        }
    }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method disables transmission, reception or both depending on option for
 *              specific uart.
 *    @details  This method disables an option (UART_OPTION_ALL,UART_OPTION_TX_EN,UART_OPTION_RX_EN)
 *              for a specific Uart.
 *    @param    uart   : Defines the Specific Uart COM port
 *    @param    option : Defines the option to disable the transmitter, receiver or both of the
 *                       specific Uart.
 *
 *    @image    html    Uart__Disable.jpg
 */
void Uart__Disable(UART_ENUM_DEF uart,UART_ENABLE_OPTIONS_DEF option)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
    USART_TYPEDEF * com;

    if (uart <= COM6 )
    {
        com = UART_PTR[uart];

        if (com != NULL)
        {
            #if (UART1_FEATURE == ENABLED)
                if (uart == COM1)
                {
                    //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                    GPIO__PIN_SET(UART_1_TX_PORT, UART_1_TX_PORTBIT)
                    GPIO__PIN_COFIG_O_PUSHPULL(UART_1_TX_PORT, UART_1_TX_PORTBIT);
                }
            #endif

            #if (UART2_FEATURE == ENABLED)
                if (uart == COM2)
                {
                    //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                    GPIO__PIN_SET(UART_2_TX_PORT, UART_2_TX_PORTBIT)
                    GPIO__PIN_COFIG_O_PUSHPULL(UART_2_TX_PORT, UART_2_TX_PORTBIT);
                }
            #endif

            #if (UART3_FEATURE == ENABLED)
                if (uart == COM3)
                {
                    //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                    GPIO__PIN_SET(UART_3_TX_PORT, UART_3_TX_PORTBIT)
                    GPIO__PIN_COFIG_O_PUSHPULL(UART_3_TX_PORT, UART_3_TX_PORTBIT);
                }
            #endif

            #if (UART4_FEATURE == ENABLED)
                if (uart == COM4)
                {
                    //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                    GPIO__PIN_SET(UART_4_TX_PORT, UART_4_TX_PORTBIT)
                    GPIO__PIN_COFIG_O_PUSHPULL(UART_4_TX_PORT, UART_4_TX_PORTBIT);
                }
            #endif

            #if (UART5_FEATURE == ENABLED)
                if (uart == COM5)
                {
                    //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                    GPIO__PIN_SET(UART_5_TX_PORT, UART_5_TX_PORTBIT)
                    GPIO__PIN_COFIG_O_PUSHPULL(UART_5_TX_PORT, UART_5_TX_PORTBIT);
                }
            #endif

            #if (UART6_FEATURE == ENABLED)
                if (uart == COM6)
                {
                    //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive HIGH on the pin
                    GPIO__PIN_SET(UART_6_TX_PORT, UART_6_TX_PORTBIT)
                    GPIO__PIN_COFIG_O_PUSHPULL(UART_6_TX_PORT, UART_6_TX_PORTBIT);
                }
            #endif

            switch (option)
            {
                case UART_OPTION_ALL:
                    com->CR1 &= (uint32)~((uint32)(USART_CR1_UE | USART_CR1_RE));
                    com->CR3 &= (uint32)~((uint32)USART_CR3_EIE);
                    break;
                case UART_OPTION_TX_EN:
                    com->CR1 &= (uint32)~((uint32)(USART_CR1_TXEIE | USART_CR1_TE));
                    break;
                case UART_OPTION_RX_EN:
                    com->CR1 &= (uint32)~((uint32)USART_CR1_RE);
                    break;
                default:
                    break;
            }
        }
    }
   #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes data from ‘source’ (buffer address mentioned by the Application) to\n
 *              the ‘destination’ (Transmitter FIFO) as per the size mentioned to the specific Uart\n
 *              and also returns the response for the write operation.
 *    @details  The data in the Transmitter FIFO is sent to other Uart node through the\n
 *              specified Uart COM port whenever the Transmitter is free.\n
 *              Before writing into the Transmitter FIFO check for the FIFO availability using Uart__GetStatus() API\n
 *              to avoid the over writing on the existing data.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    buf : Indicates the buffer’s address which contains the data to be sent.
 *    @param    size : Defines the number of bytes to be sent.
 *    @return   BOOL_TYPE
 *      @retval TRUE (1): If the data written in the transmission FIFO is successful.
 *      @retval FALSE (0): If data written in the transmission FIFO is failed.
 *
 *    @image    html    Uart__Write.jpg
 */
BOOL_TYPE Uart__Write(UART_ENUM_DEF uart, uint8 * buf,uint8 size)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        BOOL_TYPE response;
        uint8 count;
        uint8 pt;

        response = FALSE;

        if (Uart__GetStatus(uart, UART_STATUS_TX_AVAILABLE) >= size)
        {
            #if (UART1_FEATURE == ENABLED)
                if (uart == COM1)
                {
                    pt = UART1_Tx_Write_Index;
                    for (count = 0; count < size; count++)
                    {
                        UART1_Tx_Buffer[pt] = *buf;
                        buf++;
                        pt++;
                        if (pt >= UART1_TX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    UART1_Tx_Write_Index = pt;

                    response = TRUE;
                }
            #endif

            #if (UART2_FEATURE == ENABLED)
                if (uart == COM2)
                {
                    pt = UART2_Tx_Write_Index;
                    for (count = 0; count < size; count++)
                    {
                        UART2_Tx_Buffer[pt] = *buf;
                        buf++;
                        pt++;
                        if (pt >= UART2_TX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    UART2_Tx_Write_Index = pt;

                    response = TRUE;
                }
            #endif

            #if (UART3_FEATURE == ENABLED)
                if (uart == COM3)
                {
                    pt = UART3_Tx_Write_Index;
                    for (count = 0; count < size; count++)
                    {
                        UART3_Tx_Buffer[pt] = *buf;
                        buf++;
                        pt++;
                        if (pt >= UART3_TX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    UART3_Tx_Write_Index = pt;

                    response = TRUE;
                }
            #endif

            #if (UART4_FEATURE == ENABLED)
                if (uart == COM4)
                {
                    pt = UART4_Tx_Write_Index;
                    for (count = 0; count < size; count++)
                    {
                        UART4_Tx_Buffer[pt] = *buf;
                        buf++;
                        pt++;
                        if (pt >= UART4_TX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    UART4_Tx_Write_Index = pt;

                    response = TRUE;
                }
            #endif

            #if (UART5_FEATURE == ENABLED)
                if (uart == COM5)
                {
                    pt = UART5_Tx_Write_Index;
                    for (count = 0; count < size; count++)
                    {
                        UART5_Tx_Buffer[pt] = *buf;
                        buf++;
                        pt++;
                        if (pt >= UART5_TX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    UART5_Tx_Write_Index = pt;

                    response = TRUE;
                }
            #endif

            #if (UART6_FEATURE == ENABLED)
                if (uart == COM6)
                {
                    pt = UART6_Tx_Write_Index;
                    for (count = 0; count < size; count++)
                    {
                        UART6_Tx_Buffer[pt] = *buf;
                        buf++;
                        pt++;
                        if (pt >= UART6_TX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    UART6_Tx_Write_Index = pt;

                    response = TRUE;
                }
            #endif

        }
    #else
        uint8 response;
        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from ‘receive FIFO’ and\n
 *              copies to the ‘address of the buffer’ mentioned by the Application as per the size and\n
 *              also returns the response for the read operation.
 *    @details  Before reading the Receiver FIFO check for the data availability in the\n
 *              Receiver FIFO using Uart__GetStatus() API.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    buf : Indicates the buffer’s address
 *    @param    size : Defines the number of bytes to be sent.
 *    @return   BOOL_TYPE
 *      @retval TRUE (1): If data reading from receive FIFO is successful. The amount of bytes
 *                        requested was available to read
 *      @retval FALSE (0): If data reading from receive FIFO is unsuccessful. There was not
 *                         enough data to be read, fewer bytes than the size requested
 *
 *    @image    html    Uart__Read.jpg
 */
BOOL_TYPE Uart__Read(UART_ENUM_DEF uart, uint8 * buf,uint8 size)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        BOOL_TYPE response;
        uint8 count;
        uint8 pt;

        response = TRUE;

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                pt = UART1_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART1_Rx_Write_Index)
                    {
                        *buf = UART1_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART1_LastData)) = *((uint16 *)(&UART1_Rx_Buffer[pt]));

                        buf++;
                        pt++;

                        if (pt >= UART1_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART1_Rx_Read_Index;
                        break;
                    }
                }

                UART1_Rx_Read_Index = pt;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                pt = UART2_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART2_Rx_Write_Index)
                    {
                        *buf = UART2_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART2_LastData)) = *((uint16 *)(&UART2_Rx_Buffer[pt]));
                        buf++;
                        pt++;

                        if (pt >= UART2_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART2_Rx_Read_Index;
                        break;
                    }

                    UART2_Rx_Read_Index = pt;
                }
            }
        #endif

        #if (UART3_FEATURE == ENABLED)
            if (uart == COM3)
            {
                pt = UART3_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART3_Rx_Write_Index)
                    {
                        *buf = UART3_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART3_LastData)) = *((uint16 *)(&UART3_Rx_Buffer[pt]));

                        buf++;
                        pt++;

                        if (pt >= UART3_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART3_Rx_Read_Index;
                        break;
                    }
                }

                UART3_Rx_Read_Index = pt;
            }
        #endif

        #if (UART4_FEATURE == ENABLED)
            if (uart == COM4)
            {
                pt = UART4_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART4_Rx_Write_Index)
                    {
                        *buf = UART4_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART4_LastData)) = *((uint16 *)(&UART4_Rx_Buffer[pt]));
                        buf++;
                        pt++;

                        if (pt >= UART4_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART4_Rx_Read_Index;
                        break;
                    }

                    UART4_Rx_Read_Index = pt;
                }
            }
        #endif

        #if (UART5_FEATURE == ENABLED)
            if (uart == COM5)
            {
                pt = UART5_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART5_Rx_Write_Index)
                    {
                        *buf = UART5_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART5_LastData)) = *((uint16 *)(&UART5_Rx_Buffer[pt]));

                        buf++;
                        pt++;

                        if (pt >= UART5_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART5_Rx_Read_Index;
                        break;
                    }
                }

                UART5_Rx_Read_Index = pt;
            }
        #endif

        #if (UART6_FEATURE == ENABLED)
            if (uart == COM6)
            {
                pt = UART6_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART6_Rx_Write_Index)
                    {
                        *buf = UART6_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART6_LastData)) = *((uint16 *)(&UART6_Rx_Buffer[pt]));
                        buf++;
                        pt++;

                        if (pt >= UART6_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART6_Rx_Read_Index;
                        break;
                    }

                    UART6_Rx_Read_Index = pt;
                }
            }
        #endif

    #else
        uint8 response;
        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes one byte of data into the Uart transmitter FIFO.
 *    @details  The data in the Transmitter FIFO is sent to other Uart node through the specified Uart COM port\n
 *              whenever the Transmitter is free.\n
 *              Before writing into the Transmitter FIFO check for the FIFO availability using Uart__GetStatus() API\n
 *              to avoid the over writing on the existing data.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    data : data to be written into the transmission FIFO.
 *    @return   BOOL_TYPE
 *      @retval TRUE (1): If data writing into transmission FIFO is successful.
 *      @retval FALSE (0): If data writing into transmission FIFO is unsuccessful.
 *
 *    @image    html    Uart__WriteByte.jpg
 */
BOOL_TYPE Uart__WriteByte(UART_ENUM_DEF uart,uint8 data)
{
    BOOL_TYPE response;

    response = FALSE;

    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        response = Uart__Write(uart,&data,1);
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads one byte of data from Uart receiver FIFO.
 *    @details  Before reading the Receiver FIFO check for the data availability in the Receiver FIFO
 *              using Uart__GetStatus() API.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @return   uint8: data read from UART receive FIFO
 *
 *    @image    html    Uart__ReadByte.jpg
 */
uint8 Uart__ReadByte(UART_ENUM_DEF uart)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        uint8 response;

        response = FALSE;

        Uart__Read(uart,&response,1);
    #else
        uint8 response;

        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method starts sending break signal through a specific uart.
 *    @details  Break signal should be sent If the Uart Node requires more time to participate in the Uart communication.
 *              The break signal should be released once the node is ready to participate in the communication.
 *              The Break signal should not be hold for the long time.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @image    html    Uart__SendBreak.jpg
 */
void Uart__SendBreak(UART_ENUM_DEF uart)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        Uart__Disable(uart,UART_OPTION_TX_EN);

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                UART1_Break = TRUE;

                //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
                GPIO__PIN_COFIG_O_PUSHPULL(UART_1_TX_PORT, UART_1_TX_PORTBIT)
                GPIO__PIN_CLR(UART_1_TX_PORT, UART_1_TX_PORTBIT)
            }
            #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                UART2_Break = TRUE;

                //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
                GPIO__PIN_COFIG_O_PUSHPULL(UART_2_TX_PORT, UART_2_TX_PORTBIT)
                GPIO__PIN_CLR(UART_2_TX_PORT, UART_2_TX_PORTBIT)
            }
        #endif

        #if (UART3_FEATURE == ENABLED)
            if (uart == COM3)
            {
                UART3_Break = TRUE;

                //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
                GPIO__PIN_COFIG_O_PUSHPULL(UART_3_TX_PORT, UART_3_TX_PORTBIT)
                GPIO__PIN_CLR(UART_3_TX_PORT, UART_3_TX_PORTBIT)
            }
            #endif

        #if (UART4_FEATURE == ENABLED)
            if (uart == COM4)
            {
                UART4_Break = TRUE;

                //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
                GPIO__PIN_COFIG_O_PUSHPULL(UART_4_TX_PORT, UART_4_TX_PORTBIT)
                GPIO__PIN_CLR(UART_4_TX_PORT, UART_4_TX_PORTBIT)
            }
        #endif

        #if (UART5_FEATURE == ENABLED)
            if (uart == COM5)
            {
                UART5_Break = TRUE;

                //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
                GPIO__PIN_COFIG_O_PUSHPULL(UART_5_TX_PORT, UART_5_TX_PORTBIT)
                GPIO__PIN_CLR(UART_5_TX_PORT, UART_5_TX_PORTBIT)
            }
            #endif

        #if (UART6_FEATURE == ENABLED)
            if (uart == COM6)
            {
                UART6_Break = TRUE;

                //Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
                GPIO__PIN_COFIG_O_PUSHPULL(UART_6_TX_PORT, UART_6_TX_PORTBIT)
                GPIO__PIN_CLR(UART_6_TX_PORT, UART_6_TX_PORTBIT)
            }
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This procedure stops sending a break signal through a specific Uart.
 *    @details  The break signal should be release once the node is ready to participate in the communication.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @image    html    Uart__SendBreak_Verify.jpg
 */
void Uart__StopBreak(UART_ENUM_DEF uart)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                //Configure the Uart Tx port-pin
                //Uart Tx Pin configuration
                GPIO__PIN_COFIG_AF(UART_1_TX_PORT, UART_1_TX_PORTBIT, UART_1_TX_AF);
                GPIO__PIN_COFIG_SF_PUSHPULL(UART_1_TX_PORT, UART_1_TX_PORTBIT);

                UART1_Break = FALSE;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                //Configure the Uart Tx port-pin
                //Uart Tx Pin configuration
                GPIO__PIN_COFIG_AF(UART_2_TX_PORT, UART_2_TX_PORTBIT, UART_2_TX_AF);
                GPIO__PIN_COFIG_SF_PUSHPULL(UART_2_TX_PORT, UART_2_TX_PORTBIT);

                UART2_Break = FALSE;
            }
        #endif

        #if (UART3_FEATURE == ENABLED)
            if (uart == COM3)
            {
                //Configure the Uart Tx port-pin
                //Uart Tx Pin configuration
                GPIO__PIN_COFIG_AF(UART_3_TX_PORT, UART_3_TX_PORTBIT, UART_3_TX_AF);
                GPIO__PIN_COFIG_SF_PUSHPULL(UART_3_TX_PORT, UART_3_TX_PORTBIT);

                UART3_Break = FALSE;
            }
        #endif

        #if (UART4_FEATURE == ENABLED)
            if (uart == COM4)
            {
                //Configure the Uart Tx port-pin
                //Uart Tx Pin configuration
                GPIO__PIN_COFIG_AF(UART_4_TX_PORT, UART_4_TX_PORTBIT, UART_4_TX_AF);
                GPIO__PIN_COFIG_SF_PUSHPULL(UART_4_TX_PORT, UART_4_TX_PORTBIT);

                UART4_Break = FALSE;
            }
        #endif

        #if (UART5_FEATURE == ENABLED)
            if (uart == COM5)
            {
                //Configure the Uart Tx port-pin
                //Uart Tx Pin configuration
                GPIO__PIN_COFIG_AF(UART_5_TX_PORT, UART_5_TX_PORTBIT, UART_5_TX_AF);
                GPIO__PIN_COFIG_SF_PUSHPULL(UART_5_TX_PORT, UART_5_TX_PORTBIT);

                UART5_Break = FALSE;
            }
        #endif

        #if (UART6_FEATURE == ENABLED)
            if (uart == COM6)
            {
                //Configure the Uart Tx port-pin
                //Uart Tx Pin configuration
                GPIO__PIN_COFIG_AF(UART_6_TX_PORT, UART_6_TX_PORTBIT, UART_6_TX_AF);
                GPIO__PIN_COFIG_SF_PUSHPULL(UART_6_TX_PORT, UART_6_TX_PORTBIT);

                UART6_Break = FALSE;
            }
        #endif

        Uart__Enable(uart,UART_OPTION_TX_EN);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method checks the status of the specific Uart.
 *    @details  This method checks the break in, break out signal, transmit and receive FIFO availability for\n
 *              writing and reading, transmit FIFO empty and error status of received frame.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    index : Index enumerator defines which status should be checked for specific Uart.
 *
 *    @return   UART_STATUS_BREAK_IN:\n
 *              Response:   TRUE (1) = If it detects the Break signal sent by other node.\n
 *                          FALSE (0) = No break signal is detected.\n
 *              UART_STATUS_BREAK_OUT:\n
 *              Response:   TRUE (1) = A break signal is sent;\n
 *                          FALSE (0) = No break signal is sent;\n
 *              UART_STATUS_TX_EMPTY:\n
 *              Response:   TRUE (1) = The transmission FIFO is empty;\n
 *                          FALSE (0) = The transmission FIFO is not empty;\n
 *              UART_STATUS_TX_AVAILABLE:\n
 *              Response:   The response is the free space in the Transmission FIFO\n
 *                          If the response is 1 means the only one byte is free in the Tx FIFO,\n
 *                          if response is 10 means only 10 bytes are free in the Tx FIFO.\n
 *              UART_STATUS_RX_AVAILABLE:\n
 *              Response:   The amount of bytes available in the Reception FIFO;\n
 *                          If the response is 1 means the only one byte is available to read the Rx FIFO,\n
 *                          If response is 10 means only 10 bytes are available to read the the Rx FIFO.\n
 *              UART_STATUS_ERROR:\n
 *              Response:   TRUE (1) = Error detected (Over run, Framing, Parity or noise_err error)\n
 *                          FALSE (0) = No error detected.
 *
 */
uint8 Uart__GetStatus(UART_ENUM_DEF uart,UART_STATUS_DEF index)
{
    #if ((UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        uint8 response = 0;

        // UART_Rx_Write_Index and UART_Tx_Read_Index variables values gets updated from the Uart Interrupt which runs at the highest priority in asynchronous way
        // The value shall be copied to the local variable and perform the operation instead of relying on the variable which is changing its value based on the interrupt
        uint8 local_uart_rx_write_index = 0;
        uint8 local_uart_tx_read_index = 0;

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART1_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART1_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART1_Tx_Read_Index == UART1_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART1_Tx_Read_Index;

                        if (local_uart_tx_read_index <= UART1_Tx_Write_Index)
                        {
                            response = (UART1_TX_BUF_SIZE - UART1_Tx_Write_Index) + local_uart_tx_read_index;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART1_Tx_Write_Index;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART1_Rx_Write_Index;


                        if (UART1_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART1_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART1_RX_BUF_SIZE -  UART1_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART1_LastData.overrun_err)||       // Overrun error
                           (UART1_LastData.noise_err)||             // Noisy error
                           (UART1_LastData.frame_err)||         // Frame error
                           (UART1_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_RX_PIN:
                        response = (uint8)Gpio__PinRead(UART_1_RX_PORT, UART_1_RX_PORTBIT);
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART2_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART2_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART2_Tx_Read_Index == UART2_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART2_Tx_Read_Index;


                        if (local_uart_tx_read_index <= UART2_Tx_Write_Index)
                        {
                            response = (UART2_TX_BUF_SIZE - UART2_Tx_Write_Index) + local_uart_tx_read_index;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART2_Tx_Write_Index;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART2_Rx_Write_Index;

                        if (UART2_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART2_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART2_RX_BUF_SIZE -  UART2_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART2_LastData.overrun_err)||       // Overrun error
                           (UART2_LastData.noise_err)||             // Noisy error
                           (UART2_LastData.frame_err)||         // Frame error
                           (UART2_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_RX_PIN:
                        response = (uint8)Gpio__PinRead(UART_2_RX_PORT, UART_2_RX_PORTBIT);
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART3_FEATURE == ENABLED)
            if (uart == COM3)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART3_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART3_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART3_Tx_Read_Index == UART3_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART3_Tx_Read_Index;

                        if (local_uart_tx_read_index <= UART3_Tx_Write_Index)
                        {
                            response = (UART3_TX_BUF_SIZE - UART3_Tx_Write_Index) + local_uart_tx_read_index;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART3_Tx_Write_Index;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART3_Rx_Write_Index;


                        if (UART3_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART3_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART3_RX_BUF_SIZE -  UART3_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART3_LastData.overrun_err)||       // Overrun error
                           (UART3_LastData.noise_err)||             // Noisy error
                           (UART3_LastData.frame_err)||         // Frame error
                           (UART3_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_RX_PIN:
                        response = (uint8)Gpio__PinRead(UART_3_RX_PORT, UART_3_RX_PORTBIT);
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART4_FEATURE == ENABLED)
            if (uart == COM4)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART4_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART4_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART4_Tx_Read_Index == UART4_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART4_Tx_Read_Index;

                        if (local_uart_tx_read_index <= UART4_Tx_Write_Index)
                        {
                            response = (UART4_TX_BUF_SIZE - UART4_Tx_Write_Index) + local_uart_tx_read_index;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART4_Tx_Write_Index;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART4_Rx_Write_Index;

                        if (UART4_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART4_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART4_RX_BUF_SIZE -  UART4_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART4_LastData.overrun_err)||       // Overrun error
                           (UART4_LastData.noise_err)||             // Noisy error
                           (UART4_LastData.frame_err)||         // Frame error
                           (UART4_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_RX_PIN:
                        response = (uint8)Gpio__PinRead(UART_4_RX_PORT, UART_4_RX_PORTBIT);
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART5_FEATURE == ENABLED)
            if (uart == COM5)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART5_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART5_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART5_Tx_Read_Index == UART5_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART5_Tx_Read_Index;

                        if (local_uart_tx_read_index <= UART5_Tx_Write_Index)
                        {
                            response = (UART5_TX_BUF_SIZE - UART5_Tx_Write_Index) + local_uart_tx_read_index;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART5_Tx_Write_Index;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART5_Rx_Write_Index;


                        if (UART5_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART5_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART5_RX_BUF_SIZE -  UART5_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART5_LastData.overrun_err)||       // Overrun error
                           (UART5_LastData.noise_err)||             // Noisy error
                           (UART5_LastData.frame_err)||         // Frame error
                           (UART5_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_RX_PIN:
                        response = (uint8)Gpio__PinRead(UART_5_RX_PORT, UART_5_RX_PORTBIT);
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART6_FEATURE == ENABLED)
            if (uart == COM6)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART6_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART6_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART6_Tx_Read_Index == UART6_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART6_Tx_Read_Index;


                        if (local_uart_tx_read_index <= UART6_Tx_Write_Index)
                        {
                            response = (UART6_TX_BUF_SIZE - UART6_Tx_Write_Index) + local_uart_tx_read_index;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART6_Tx_Write_Index;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART6_Rx_Write_Index;

                        if (UART6_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART6_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART6_RX_BUF_SIZE -  UART6_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART6_LastData.overrun_err)||       // Overrun error
                           (UART6_LastData.noise_err)||             // Noisy error
                           (UART6_LastData.frame_err)||         // Frame error
                           (UART6_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_RX_PIN:
                        response = (uint8)Gpio__PinRead(UART_6_RX_PORT, UART_6_RX_PORTBIT);
                        break;

                    default:
                        break;
                }
            }
        #endif
    #else
        uint8 response;

        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles Uart Data Transmission and also the break signal
 *    @details  The break signal has highest priority then the Data Transmission.\n
 *              If the application requests for the Break signal transmission then handler inserts break signal after
 *              completion of the ongoing transmission\n
 *              and stops the further Data transmission until the Break signal is resumed.\n
 *              This method checks if there is data in transmit FIFO. If data is present then this method\n
 *              transmits the data on Uart.
 *
 *    @image    html    Uart__Handler.jpg
 */
void Uart__Handler(void)
{
    #if (UART1_FEATURE == ENABLED)
            if (UART1_Tx_Write_Index != UART1_Tx_Read_Index)   // Is there something to be sent?
            {
                if (((USART1.CR1 & USART_CR1_TE) == FALSE) ||
                    ((USART1.CR1 & USART_CR1_TXEIE) == FALSE))
                {
                    Uart__Enable(COM1, UART_OPTION_TX_EN);
                }
            }
    #endif

    #if (UART2_FEATURE == ENABLED)
            if (UART2_Tx_Write_Index != UART2_Tx_Read_Index)   // Is there something to be sent?
            {
                if (((USART2.CR1 & USART_CR1_TE) == FALSE) ||
                    ((USART2.CR1 & USART_CR1_TXEIE) == FALSE))
                {
                    Uart__Enable(COM2, UART_OPTION_TX_EN);
                }
            }
    #endif

    #if (UART3_FEATURE == ENABLED)
            if (UART3_Tx_Write_Index != UART3_Tx_Read_Index)   // Is there something to be sent?
            {
                if (((USART3.CR1 & USART_CR1_TE) == FALSE) ||
                    ((USART3.CR1 & USART_CR1_TXEIE) == FALSE))
                {
                    Uart__Enable(COM3, UART_OPTION_TX_EN);
                }
            }
    #endif

    #if (UART4_FEATURE == ENABLED)
            if (UART4_Tx_Write_Index != UART4_Tx_Read_Index)   // Is there something to be sent?
            {
                if (((USART4.CR1 & USART_CR1_TE) == FALSE) ||
                    ((USART4.CR1 & USART_CR1_TXEIE) == FALSE))
                {
                    Uart__Enable(COM4, UART_OPTION_TX_EN);
                }
            }
    #endif

    #if (UART5_FEATURE == ENABLED)
            if (UART5_Tx_Write_Index != UART5_Tx_Read_Index)   // Is there something to be sent?
            {
                if (((USART5.CR1 & USART_CR1_TE) == FALSE) ||
                    ((USART5.CR1 & USART_CR1_TXEIE) == FALSE))
                {
                    Uart__Enable(COM5, UART_OPTION_TX_EN);
                }
            }
    #endif

    #if (UART6_FEATURE == ENABLED)
            if (UART6_Tx_Write_Index != UART6_Tx_Read_Index)   // Is there something to be sent?
            {
                if (((USART6.CR1 & USART_CR1_TE) == FALSE) ||
                    ((USART6.CR1 & USART_CR1_TXEIE) == FALSE))
                {
                    Uart__Enable(COM6, UART_OPTION_TX_EN);
                }
            }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles the Uart1 receive interrupt.
 *    @details  This needs to be configured in the InterruptVector_prm.h file as below-\n
 *              #define USART1_RX_INTERRUPTION      Uart__Uart1RxIsr\n
 *              This ISR is called whenever the data is received in the Uart_DR register.
 */
void Uart__Interrupt1(void)
{
    #if (UART1_FEATURE == ENABLED)
        volatile USART_TYPEDEF status;
        uint8 index;
        volatile uint16 data;

        status.ISR = USART1.ISR;

        //Receive part----------------------------------------------
        if (((status.ISR) & (USART_ISR_RXNE |
                               USART_ISR_FE |
                               USART_ISR_ORE |
                               USART_ISR_NE |
                               USART_ISR_PE)) != FALSE)
        {

            data = (USART1.RDR & (uint16)0x0FF);

            USART1.ICR = USART_ICR_CLEAR_ALL_FLAGS;
            index = UART1_Rx_Write_Index;

            *((uint16 *)(&UART1_Rx_Buffer[index])) = 0;

            *((uint16 *)(&UART1_Rx_Buffer[index])) = (uint16)status.ISR;

            UART1_Rx_Buffer[index].data = data;

            if (((status.ISR & USART_ISR_FE) != FALSE) &&
                (UART1_Rx_Buffer[index].data == 0))
            {
                UART1_Rx_Buffer[index].incomming_break = TRUE;
            }
            else
            {
                UART1_Rx_Buffer[index].incomming_break = FALSE;
            }
            index++;

            if (index >= UART1_RX_BUF_SIZE)
            {
                index = 0;
            }

            UART1_Rx_Write_Index = index;
        }

        //Transmitter---------------------------------------------------------------------------------------------------
        if ((USART1.CR1 & USART_CR1_TXEIE) != FALSE)
        {
            if ((status.ISR & USART_ISR_TXE) != FALSE)
            {
                if (UART1_Tx_Write_Index == UART1_Tx_Read_Index)
                {
                    USART1.CR1 &= (uint32)~((uint32) USART_CR1_TXEIE);
                }
                else
                {
                    USART1.TDR = (UART1_Tx_Buffer[UART1_Tx_Read_Index] & (uint16)0x01FF);

                    UART1_Tx_Read_Index++;
                    if (UART1_Tx_Read_Index >= UART1_TX_BUF_SIZE)
                    {
                        UART1_Tx_Read_Index = 0;
                    }
                }
            }
        }

    #else
        //Enable the clock for Uart module
        RCC.APBENR2 |= RCC_APBENR2_USART1EN;

        //Reset the UART
        RCC.APBRSTR2 |= RCC_APBRSTR2_USART1RST;
        RCC.APBRSTR2 &= (uint32)(~(uint32)RCC_APBRSTR2_USART1RST);

        //Disable the clock for Uart module
        RCC.APBENR2 &= ~RCC_APBENR2_USART1EN;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles the Uart1 receive interrupt.
 *    @details  This needs to be configured in the InterruptVector_prm.h file as below-\n
 *              #define USART1_RX_INTERRUPTION      Uart__Uart1RxIsr\n
 *              This ISR is called whenever the data is received in the Uart_DR register.
 */
void Uart__Interrupt2(void)
{
    #if (UART2_FEATURE == ENABLED)
        volatile USART_TYPEDEF status;
        uint8 index;
        volatile uint16 data;

        status.ISR = USART2.ISR;

        //Receive part----------------------------------------------
        if (((status.ISR) & (USART_ISR_RXNE |
                               USART_ISR_FE |
                               USART_ISR_ORE |
                               USART_ISR_NE |
                               USART_ISR_PE)) != FALSE)
        {

            data = (USART2.RDR & (uint16)0x0FF);

            USART2.ICR = USART_ICR_CLEAR_ALL_FLAGS;
            index = UART2_Rx_Write_Index;

            *((uint16 *)(&UART2_Rx_Buffer[index])) = 0;

            *((uint16 *)(&UART2_Rx_Buffer[index])) = (uint16)status.ISR;

            UART2_Rx_Buffer[index].data = data;

            if (((status.ISR & USART_ISR_FE) != FALSE) &&
                (UART2_Rx_Buffer[index].data == 0))
            {
                UART2_Rx_Buffer[index].incomming_break = TRUE;
            }
            else
            {
                UART2_Rx_Buffer[index].incomming_break = FALSE;
            }
            index++;

            if (index >= UART2_RX_BUF_SIZE)
            {
                index = 0;
            }

            UART2_Rx_Write_Index = index;
        }

        //Transmitter---------------------------------------------------------------------------------------------------
        if ((USART2.CR1 & USART_CR1_TXEIE) != FALSE)
        {
            if ((status.ISR & USART_ISR_TXE) != FALSE)
            {
                if (UART2_Tx_Write_Index == UART2_Tx_Read_Index)
                {
                    USART2.CR1 &= (uint32)~((uint32) USART_CR1_TXEIE);
                }
                else
                {
                    USART2.TDR = (UART2_Tx_Buffer[UART2_Tx_Read_Index] & (uint16)0x01FF);

                    UART2_Tx_Read_Index++;
                    if (UART2_Tx_Read_Index >= UART2_TX_BUF_SIZE)
                    {
                        UART2_Tx_Read_Index = 0;
                    }
                }
            }
        }

    #else
        //Enable the clock for Uart module
        RCC.APBENR1 |= RCC_APBENR1_USART2EN;

        //Reset the UART3
        RCC.APBRSTR1 |= RCC_APBRSTR1_USART2RST;
        RCC.APBRSTR1 &= (uint32)(~(uint32)RCC_APBRSTR1_USART2RST);

        //Disable the clock for Uart module
        RCC.APBENR1 &= ~RCC_APBENR1_USART2EN;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles the Uart1 receive interrupt.
 *    @details  This needs to be configured in the InterruptVector_prm.h file as below-\n
 *              #define USART3_RX_INTERRUPTION      Uart__Uart3RxIsr\n
 *              This ISR is called whenever the data is received in the Uart_DR register.
 */
void Uart__Interrupt3_4_5_6(void)
{
    #if ((UART3_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED)||(UART5_FEATURE == ENABLED)||(UART6_FEATURE == ENABLED))
        volatile USART_TYPEDEF status;
        uint8 index;
        volatile uint16 data;
    #endif

    #if (UART3_FEATURE == ENABLED)
        status.ISR = USART3.ISR;

        //Receive part----------------------------------------------
        if (((status.ISR) & (USART_ISR_RXNE |
                               USART_ISR_FE |
                               USART_ISR_ORE |
                               USART_ISR_NE |
                               USART_ISR_PE)) != FALSE)
        {

            data = (USART3.RDR & (uint16)0x0FF);

            USART3.ICR = USART_ICR_CLEAR_ALL_FLAGS;

            index = UART3_Rx_Write_Index;

            *((uint16 *)(&UART3_Rx_Buffer[index])) = 0;

            *((uint16 *)(&UART3_Rx_Buffer[index])) = (uint16)status.ISR;

            UART3_Rx_Buffer[index].data = data;

            if (((status.ISR & USART_ISR_FE) != FALSE) &&
                (UART3_Rx_Buffer[index].data == 0))
            {
                UART3_Rx_Buffer[index].incomming_break = TRUE;
            }
            else
            {
                UART3_Rx_Buffer[index].incomming_break = FALSE;
            }
            index++;

            if (index >= UART3_RX_BUF_SIZE)
            {
                index = 0;
            }

            UART3_Rx_Write_Index = index;
        }

        //Transmitter---------------------------------------------------------------------------------------------------
        if ((USART3.CR1 & USART_CR1_TXEIE) != FALSE)
        {
            if ((status.ISR & USART_ISR_TXE) != FALSE)
            {
                if (UART3_Tx_Write_Index == UART3_Tx_Read_Index)
                {
                    USART3.CR1 &= (uint32)~((uint32) USART_CR1_TXEIE);
                }
                else
                {
                    USART3.TDR = (UART3_Tx_Buffer[UART3_Tx_Read_Index] & (uint16)0x01FF);

                    UART3_Tx_Read_Index++;
                    if (UART3_Tx_Read_Index >= UART3_TX_BUF_SIZE)
                    {
                        UART3_Tx_Read_Index = 0;
                    }
                }
            }
        }
    #endif

    #if (UART4_FEATURE == ENABLED)

        status.ISR = USART4.ISR;

        //Receive part----------------------------------------------
        if (((status.ISR) & (USART_ISR_RXNE |
                               USART_ISR_FE |
                               USART_ISR_ORE |
                               USART_ISR_NE |
                               USART_ISR_PE)) != FALSE)
        {

            data = (USART4.RDR & (uint16)0x0FF);

            USART4.ICR = USART_ICR_CLEAR_ALL_FLAGS;

            index = UART4_Rx_Write_Index;

            *((uint16 *)(&UART4_Rx_Buffer[index])) = 0;

            *((uint16 *)(&UART4_Rx_Buffer[index])) = (uint16)status.ISR;

            UART4_Rx_Buffer[index].data = data;

            if (((status.ISR & USART_ISR_FE) != FALSE) &&
                (UART4_Rx_Buffer[index].data == 0))
            {
                UART4_Rx_Buffer[index].incomming_break = TRUE;
            }
            else
            {
                UART4_Rx_Buffer[index].incomming_break = FALSE;
            }
            index++;

            if (index >= UART4_RX_BUF_SIZE)
            {
                index = 0;
            }

            UART4_Rx_Write_Index = index;
        }

        //Transmitter---------------------------------------------------------------------------------------------------
        if ((USART4.CR1 & USART_CR1_TXEIE) != FALSE)
        {
            if ((status.ISR & USART_ISR_TXE) != FALSE)
            {
                if (UART4_Tx_Write_Index == UART4_Tx_Read_Index)
                {
                    USART4.CR1 &= (uint32)~((uint32) USART_CR1_TXEIE);
                }
                else
                {
                    USART4.TDR = (UART4_Tx_Buffer[UART4_Tx_Read_Index] & (uint16)0x01FF);

                    UART4_Tx_Read_Index++;
                    if (UART4_Tx_Read_Index >= UART4_TX_BUF_SIZE)
                    {
                        UART4_Tx_Read_Index = 0;
                    }
                }
            }
        }
    #endif

    #if (UART5_FEATURE == ENABLED)
        status.ISR = USART5.ISR;

        //Receive part----------------------------------------------
        if (((status.ISR) & (USART_ISR_RXNE |
                               USART_ISR_FE |
                               USART_ISR_ORE |
                               USART_ISR_NE |
                               USART_ISR_PE)) != FALSE)
        {

            data = (USART5.RDR & (uint16)0x0FF);

            USART5.ICR = USART_ICR_CLEAR_ALL_FLAGS;

            index = UART5_Rx_Write_Index;

            *((uint16 *)(&UART5_Rx_Buffer[index])) = 0;

            *((uint16 *)(&UART5_Rx_Buffer[index])) = (uint16)status.ISR;

            UART5_Rx_Buffer[index].data = data;

            if (((status.ISR & USART_ISR_FE) != FALSE) &&
                (UART5_Rx_Buffer[index].data == 0))
            {
                UART5_Rx_Buffer[index].incomming_break = TRUE;
            }
            else
            {
                UART5_Rx_Buffer[index].incomming_break = FALSE;
            }
            index++;

            if (index >= UART5_RX_BUF_SIZE)
            {
                index = 0;
            }

            UART5_Rx_Write_Index = index;
        }

        //Transmitter---------------------------------------------------------------------------------------------------
        if ((USART5.CR1 & USART_CR1_TXEIE) != FALSE)
        {
            if ((status.ISR & USART_ISR_TXE) != FALSE)
            {
                if (UART5_Tx_Write_Index == UART5_Tx_Read_Index)
                {
                    USART5.CR1 &= (uint32)~((uint32) USART_CR1_TXEIE);
                }
                else
                {
                    USART5.TDR = (UART5_Tx_Buffer[UART5_Tx_Read_Index] & (uint16)0x01FF);

                    UART5_Tx_Read_Index++;
                    if (UART5_Tx_Read_Index >= UART5_TX_BUF_SIZE)
                    {
                        UART5_Tx_Read_Index = 0;
                    }
                }
            }
        }
    #endif


    #if (UART6_FEATURE == ENABLED)

        status.ISR = USART6.ISR;

        //Receive part----------------------------------------------
        if (((status.ISR) & (USART_ISR_RXNE |
                               USART_ISR_FE |
                               USART_ISR_ORE |
                               USART_ISR_NE |
                               USART_ISR_PE)) != FALSE)
        {

            data = (USART6.RDR & (uint16)0x0FF);

            USART6.ICR = USART_ICR_CLEAR_ALL_FLAGS;

            index = UART6_Rx_Write_Index;

            *((uint16 *)(&UART6_Rx_Buffer[index])) = 0;

            *((uint16 *)(&UART6_Rx_Buffer[index])) = (uint16)status.ISR;

            UART6_Rx_Buffer[index].data = data;

            if (((status.ISR & USART_ISR_FE) != FALSE) &&
                (UART6_Rx_Buffer[index].data == 0))
            {
                UART6_Rx_Buffer[index].incomming_break = TRUE;
            }
            else
            {
                UART6_Rx_Buffer[index].incomming_break = FALSE;
            }
            index++;

            if (index >= UART6_RX_BUF_SIZE)
            {
                index = 0;
            }

            UART6_Rx_Write_Index = index;
        }

        //Transmitter---------------------------------------------------------------------------------------------------
        if ((USART6.CR1 & USART_CR1_TXEIE) != FALSE)
        {
            if ((status.ISR & USART_ISR_TXE) != FALSE)
            {
                if (UART6_Tx_Write_Index == UART6_Tx_Read_Index)
                {
                    USART6.CR1 &= (uint32)~((uint32) USART_CR1_TXEIE);
                }
                else
                {
                    USART6.TDR = (UART6_Tx_Buffer[UART6_Tx_Read_Index] & (uint16)0x01FF);

                    UART6_Tx_Read_Index++;
                    if (UART6_Tx_Read_Index >= UART6_TX_BUF_SIZE)
                    {
                        UART6_Tx_Read_Index = 0;
                    }
                }
            }
        }
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
