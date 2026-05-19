/**
 *  @file       
 *
 *  @brief      Macros for the BoardDebugging module
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef BOARDDEBUGGING_MACROS_H_
#define BOARDDEBUGGING_MACROS_H_

#include "Mci_prm.h"
#include "Gpio.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
// Definition of the available debug ports and pins for this board
#define PORT_DBG_0      PORTE
#define PIN_DBG_0       (17)
#define PORT_DBG_1      PORTE
#define PIN_DBG_1       (25)
#define PORT_DBG_2      PORTE
#define PIN_DBG_2       (19)

// Association of the ports and pins to the desired module
#define PORT_ATOD       PORT_DBG_0
#define PIN_ATOD        PIN_DBG_0
#define PORT_ATOD_A     PORT_DBG_1
#define PIN_ATOD_A      PIN_DBG_1
#define PORT_ATOD_B     PORT_DBG_2
#define PIN_ATOD_B      PIN_DBG_2
#define PORT_WDOG       PORT_DBG_1
#define PIN_WDOG        PIN_DBG_1
#define PORT_TIMER      PORT_DBG_2
#define PIN_TIMER       PIN_DBG_2
#define PORT_UART       PORT_DBG_1
#define PIN_UART        PIN_DBG_1
#define PORT_MAIN_A     PORT_DBG_1
#define PIN_MAIN_A      PIN_DBG_1
#define PORT_MAIN_B     PORT_DBG_2
#define PIN_MAIN_B      PIN_DBG_2
#define PORT_MAIN       PORT_DBG_0
#define PIN_MAIN        PIN_DBG_0
#define PORT_ISR_A      PORT_DBG_1
#define PIN_ISR_A       PIN_DBG_1
#define PORT_ISR_B      PORT_DBG_2
#define PIN_ISR_B       PIN_DBG_2
#define PORT_ISR        PORT_DBG_0
#define PIN_ISR         PIN_DBG_0
#define PORT_IDLE       PORT_DBG_0
#define PIN_IDLE        PIN_DBG_0
#define PORT_INIT       PORT_DBG_0
#define PIN_INIT        PIN_DBG_0
#define PORT_INIT_A     PORT_DBG_1
#define PIN_INIT_A      PIN_DBG_1
#define PORT_INIT_B     PORT_DBG_2
#define PIN_INIT_B      PIN_DBG_2

// Choose the slot to analyze
// This is common to the Main and ISR Vortex loops
#define CPU_LOAD_N_SLOT                    0

// Macros
#if (DEBUG_CPU_LOAD == ENABLED)
// AtoD handler macros
#define CPU_LOAD_ATOD_ISR_BEGIN()          //GPIO__PIN_SET(PORT_ATOD, PIN_ATOD)
#define CPU_LOAD_ATOD_ISR_END()            //GPIO__PIN_CLR(PORT_ATOD, PIN_ATOD)
#define CPU_LOAD_ATOD_ISR_TOGGLE()         //GPIO__PIN_TOGGLE(PORT_ATOD, PIN_ATOD)
// Class A only
#define CPU_LOAD_ATOD_ISR_A_BEGIN()        //GPIO__PIN_SET(PORT_ATOD_A, PIN_ATOD_A)
#define CPU_LOAD_ATOD_ISR_A_END()          //GPIO__PIN_CLR(PORT_ATOD_A, PIN_ATOD_A)
#define CPU_LOAD_ATOD_ISR_A_TOGGLE()       //GPIO__PIN_TOGGLE(PORT_ATOD_A, PIN_ATOD_A)
// Class B only
#define CPU_LOAD_ATOD_ISR_B_BEGIN()        //GPIO__PIN_SET(PORT_ATOD_B, PIN_ATOD_B)
#define CPU_LOAD_ATOD_ISR_B_END()          //GPIO__PIN_CLR(PORT_ATOD_B, PIN_ATOD_B)
#define CPU_LOAD_ATOD_ISR_B_TOGGLE()       //GPIO__PIN_TOGGLE(PORT_ATOD, PIN_ATOD)
// WDT refresh
#define CPU_LOAD_MAIN_WDOG_TOGGLE()        //GPIO__PIN_TOGGLE(PORT_WDOG, PIN_WDOG)
// Vortex ISR macros
#define CPU_LOAD_TIMERISR_BEGIN()          //GPIO__PIN_SET(PORT_TIMER, PIN_TIMER)
#define CPU_LOAD_TIMERISR_END()            //GPIO__PIN_CLR(PORT_TIMER, PIN_TIMER)
#define CPU_LOAD_TIMERISR_TOGGLE()         //GPIO__PIN_TOGGLE(PORT_TIMER, PIN_TIMER)
// Per Slot Class A only
#define CPU_LOAD_ISRSLOT_A_BEGIN()         //GPIO__PIN_SET(PORT_ISR_A, PIN_ISR_A)
#define CPU_LOAD_ISRSLOT_A_END()           //GPIO__PIN_CLR(PORT_ISR_A, PIN_ISR_A)
#define CPU_LOAD_ISRSLOT_A_TOGGLE()        //GPIO__PIN_TOGGLE(PORT_UART, PIN_UART)
// Per Slot Class B only
#define CPU_LOAD_ISRSLOT_B_BEGIN()         //GPIO__PIN_SET(PORT_ISR_B, PIN_ISR_B)
#define CPU_LOAD_ISRSLOT_B_END()           //GPIO__PIN_CLR(PORT_ISR_B, PIN_ISR_B)
#define CPU_LOAD_ISRSLOT_B_TOGGLE()        //GPIO__PIN_TOGGLE(PORT_ISR_B, PIN_ISR_B)
// Per Slot Class A & Class B
#define CPU_LOAD_ISRSLOT_BEGIN()           //GPIO__PIN_SET(PORT_ISR, PIN_ISR)
#define CPU_LOAD_ISRSLOT_END()             //GPIO__PIN_CLR(PORT_ISR, PIN_ISR)
// UART ISR macros
#define CPU_LOAD_UARTISR_BEGIN()           //GPIO__PIN_SET(PORT_UART, PIN_UART)
#define CPU_LOAD_UARTISR_END()             //GPIO__PIN_CLR(PORT_UART, PIN_UART)
#define CPU_LOAD_UARTISR_TOGGLE()          //GPIO__PIN_TOGGLE(PORT_UART, PIN_UART)
// Vortex main loop per slot macros
// Per slot Class A only
#define CPU_LOAD_MAINSLOT_A_BEGIN()        //GPIO__PIN_SET(PORT_MAIN_A, PIN_MAIN_A)
#define CPU_LOAD_MAINSLOT_A_END()          //GPIO__PIN_CLR(PORT_MAIN_A, PIN_MAIN_A)
#define CPU_LOAD_MAINSLOT_A_TOGGLE()       //GPIO__PIN_TOGGLE(PORT_UART, PIN_UART)
// Per slot Class B only
#define CPU_LOAD_MAINSLOT_B_BEGIN()        //GPIO__PIN_SET(PORT_MAIN_B, PIN_MAIN_B)
#define CPU_LOAD_MAINSLOT_B_END()          //GPIO__PIN_CLR(PORT_MAIN_B, PIN_MAIN_B)
#define CPU_LOAD_MAINSLOT_B_TOGGLE()       //GPIO__PIN_TOGGLE(PORT_UART, PIN_UART)
// Per Slot Class A & Class B
#define CPU_LOAD_MAINSLOT_BEGIN()          //GPIO__PIN_SET(PORT_MAIN, PIN_MAIN)
#define CPU_LOAD_MAINSLOT_END()            //GPIO__PIN_CLR(PORT_MAIN, PIN_MAIN)
// Used to measure overall IDLE time
// When the selected pin is high we are idle
#define CPU_LOAD_IDLE_SET()                //GPIO__PIN_SET(PORT_IDLE, PIN_IDLE)
#define CPU_LOAD_IDLE_CLEAR()              //GPIO__PIN_CLR(PORT_IDLE, PIN_IDLE)
// Used for the initialization phase measurement
#define CPU_LOAD_INIT_BEGIN()              GPIO__PIN_SET(PORT_INIT, PIN_INIT)
#define CPU_LOAD_INIT_END()                GPIO__PIN_CLR(PORT_INIT, PIN_INIT)
#define CPU_LOAD_INIT_A_BEGIN()            GPIO__PIN_SET(PORT_INIT_A, PIN_INIT_A)
#define CPU_LOAD_INIT_A_END()              GPIO__PIN_CLR(PORT_INIT_A, PIN_INIT_A)
#define CPU_LOAD_INIT_B_BEGIN()            GPIO__PIN_SET(PORT_INIT_B, PIN_INIT_B)
#define CPU_LOAD_INIT_B_END()              GPIO__PIN_CLR(PORT_INIT_B, PIN_INIT_B)

#else
// AtoD handler macros
#define CPU_LOAD_ATOD_ISR_BEGIN()
#define CPU_LOAD_ATOD_ISR_END()
#define CPU_LOAD_ATOD_ISR_TOGGLE()
// Class A only
#define CPU_LOAD_ATOD_ISR_A_BEGIN()
#define CPU_LOAD_ATOD_ISR_A_END()
#define CPU_LOAD_ATOD_ISR_A_TOGGLE()
// Class B only
#define CPU_LOAD_ATOD_ISR_B_BEGIN()
#define CPU_LOAD_ATOD_ISR_B_END()
#define CPU_LOAD_ATOD_ISR_B_TOGGLE()
// WDT refresh
#define CPU_LOAD_MAIN_WDOG_TOGGLE()
// Vortex ISR macros
#define CPU_LOAD_TIMERISR_BEGIN()
#define CPU_LOAD_TIMERISR_END()
#define CPU_LOAD_TIMERISR_TOGGLE()
// Per Slot Class A only
#define CPU_LOAD_ISRSLOT_A_BEGIN()
#define CPU_LOAD_ISRSLOT_A_END()
#define CPU_LOAD_ISRSLOT_A_TOGGLE()
// Per Slot Class B only
#define CPU_LOAD_ISRSLOT_B_BEGIN()
#define CPU_LOAD_ISRSLOT_B_END()
#define CPU_LOAD_ISRSLOT_B_TOGGLE()
// Per Slot Class A & Class B
#define CPU_LOAD_ISRSLOT_BEGIN()
#define CPU_LOAD_ISRSLOT_END()
// UART ISR macros
#define CPU_LOAD_UARTISR_BEGIN()
#define CPU_LOAD_UARTISR_END()
#define CPU_LOAD_UARTISR_TOGGLE()
// Vortex main loop per slot macros
// Per slot Class A only
#define CPU_LOAD_MAINSLOT_A_BEGIN()
#define CPU_LOAD_MAINSLOT_A_END()
#define CPU_LOAD_MAINSLOT_A_TOGGLE()
// Per slot Class B only
#define CPU_LOAD_MAINSLOT_B_BEGIN()
#define CPU_LOAD_MAINSLOT_B_END()
#define CPU_LOAD_MAINSLOT_B_TOGGLE()
// Per Slot Class A & Class B
#define CPU_LOAD_MAINSLOT_BEGIN()
#define CPU_LOAD_MAINSLOT_END()
// Used to measure overall IDLE time
// When the selected pin is high we are idle
#define CPU_LOAD_IDLE_SET()
#define CPU_LOAD_IDLE_CLEAR()
// Used for the initialization phase measurement
#define CPU_LOAD_INIT_BEGIN()
#define CPU_LOAD_INIT_END()
#define CPU_LOAD_INIT_A_BEGIN()
#define CPU_LOAD_INIT_A_END()
#define CPU_LOAD_INIT_B_BEGIN()
#define CPU_LOAD_INIT_B_END()
#endif
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // BOARDDEBUGGING_MACROS_H_


