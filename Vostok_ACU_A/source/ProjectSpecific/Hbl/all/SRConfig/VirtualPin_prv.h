/**
 *  @file       VirtualPin_prv.h
 *
 *  @brief      Private declarations for the VirtualPin module.
 *
 *	@defgroup	CLASS_B
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/02 08:59:52EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VIRTUALPIN_PRV_H_
#define VIRTUALPIN_PRV_H_

#include "SystemConfig.h"
#include "Gpio.h"
#include "Atod.h"
#include "Timer.h"
#include "ExtInterrupt.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! project specific pcb pin configuration.
#define VIRTUALPIN_LIST \
{ \
    /* port      pin   pin_config                   device              pin_remap   param0                         param1                  param2      GESE Virtual Pin Number - 1 based index */ \
    { PORTC      ,11  ,INPUT_FLOAT                  ,(uint8)EXTINT_CH11 ,0         ,(uint8)TIMER1                  ,(uint8)TIMER_CHANNEL4  ,0},     /*  1 VZX */ \
    { PORTC      ,12  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /*  2 FILL */   \
    { PORTC      ,13  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /*  3 DIVERTER */   \
    { PORTC      ,14  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /*  4 DISPENSER */   \
    { PORTC      ,15  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /*  5 DRAIN */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /*  6 VBAT */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /*  7 VREF+ */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /*  8 VDD/VDDA */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /*  9 VSS/VSSA */ \
    { PORTF      ,0   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 10 WASH GWS TRIAC */   \
    { PORTF      ,1   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 11 WASH */   \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 12 NRST */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 13 DEV TX */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 14 DEV RX */ \
	{ PORTC      ,2   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 15 PILOT DIRECT */   \
    { PORTC      ,3   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 16 FACTORY SERVICE */ \
	{ PORTA      ,0   ,INPUT_ANALOG                 ,(uint8)ATOD_CH0    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 17 LVD */  \
	{ PORTA      ,1   ,INPUT_ANALOG                 ,(uint8)ATOD_CH1    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 18 RA EMPTY SIG */  \
	{ PORTA      ,2   ,INPUT_ANALOG                 ,(uint8)ATOD_CH2    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 19 NTC1 */  \
	{ PORTA      ,3   ,INPUT_ANALOG                 ,(uint8)ATOD_CH3    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 20 DC FAN I SENSE */  \
	{ PORTA      ,4   ,INPUT_ANALOG                 ,(uint8)ATOD_CH4    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 21 V DIAGNOSTIC */  \
	{ PORTA      ,5   ,INPUT_ANALOG                 ,(uint8)ATOD_CH5    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 22 I DIAGNOSTIC */  \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 23 WASH I */ \
	{ PORTA      ,7   ,INPUT_ANALOG                 ,(uint8)ATOD_CH7    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 24 AC LINE */  \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 25 WASH U */ \
	{ PORTC      ,5   ,INPUT_ANALOG                 ,(uint8)ATOD_CH18   ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 26 HEATER FB */  \
	{ PORTB      ,0   ,INPUT_ANALOG                 ,(uint8)ATOD_CH8    ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 27 HEATER CURRENT */  \
	{ PORTB      ,1   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 28 HEATER L */   \
	{ PORTB      ,2   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 29 HEATER N */   \
	{ PORTB      ,10  ,INPUT_ANALOG                 ,(uint8)ATOD_CH11   ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 30 HW TIMER SENSE */  \
	{ PORTB      ,11  ,INPUT_ANALOG                 ,(uint8)ATOD_CH15   ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 31 DIV POS SIG */  \
	{ PORTB      ,12  ,INPUT_ANALOG                 ,(uint8)ATOD_CH16   ,0         ,(uint8)ATOD_RESOLUTION_10BITS  ,0                      ,0},     /* 32 OPT SIG */  \
	{ PORTB      ,13  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 33 HW TIMER DRIVE */   \
	{ PORTB      ,14  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 34 PILOT CHARGE */   \
	{ PORTB      ,15  ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 35 BULK 1 */   \
	{ PORTA      ,8   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 36 BULK 2 */   \
	{ PORTA      ,9   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 37 MSA SENS SIG */   \
	{ PORTC      ,6   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 38 Unused */  \
	{ PORTC      ,7   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 39 Unused */  \
	{ PORTD      ,8   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 40 Unused */  \
	{ PORTD      ,9   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 41 Unused */  \
	{ PORTA      ,10  ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 42 unused */  \
	{ PORTA      ,11  ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 43 Unused */  \
	{ PORTA      ,12  ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 44 Unused */  \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 45 SWDIO */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 46 SWCLK */ \
    { PORTA      ,15  ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 47 Unused */ \
    { PORTC      ,8   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 48 Unused */ \
    { PORTC      ,9   ,OUTPUT_PUSHPULL              ,(uint8)TIMER3      ,GPIO_AF_1 ,(uint8)TIMER_CHANNEL4          ,0                      ,0},     /* 49 TURB_PWM */ \
    { PORTD      ,0   ,INPUT_FLOAT                  ,(uint8)TIMER16     ,GPIO_AF_2 ,(uint8)TIMER_CHANNEL1          ,0                      ,0},     /* 50 FLOW METER */  \
	{ PORTD      ,1   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 51 DIV SALT SENS VCC */   \
	{ PORTD      ,2   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 52 DC FAN */   \
    { PORTD      ,3   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 53 DOS */ \
    { PORTD      ,4   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 54 DOOR SWITCH */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 55 WIN TX */ \
    { PORT_NULL  ,0   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 56 WIN RX */ \
    { PORTB      ,3   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 57 WIN 12.7V SWITCHED */ \
    { PORTB      ,4   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 58 Unused */ \
	{ PORTB      ,5   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 59 DIAGNOSTIC LED */   \
	{ PORTB      ,6   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 60 VENT REGEN VALVE */   \
    { PORTB      ,7   ,OUTPUT_PUSHPULL              ,(uint8)TIMER4      ,GPIO_AF_9 ,(uint8)TIMER_CHANNEL2          ,0                      ,0},     /* 61 LITT */ \
	{ PORTB      ,8   ,OUTPUT_PUSHPULL              ,0                  ,0         ,0                              ,0                      ,0},     /* 62 RA EMPTY SENS DRIVE */   \
    { PORTB      ,9   ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 63 FLOAT SWITCH */ \
    { PORTC      ,10  ,INPUT_FLOAT                  ,0                  ,0         ,0                              ,0                      ,0},     /* 64 SALT SENS SIG */ \
}

#endif // VIRTUALPIN_PRV_H_
