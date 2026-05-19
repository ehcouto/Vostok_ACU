/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      Thre's a default for each register which is applied to the register when Gpio__Initialize and
 *  Gpio__RefreshConfig are called.
 *  \ If the DEFAULT register is not defined it the specific register is not affected.
 *  \ If the DEFAULT register is defined and the MASK is not defined for that specific port the register will be
 *  full overwrite by the default.
 *  \ If the DEFAULT register is defined and the MASK is also defined for that specific port, the register will be
 *  affected by the DEFAULT value only on the bits set to 1 in the MASK define.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef GPIO_PRM_H_
	#define GPIO_PRM_H_

//=====================================================================================================================
//-------------------------------------- GPIO configurable macros -----------------------------------------------------
//=====================================================================================================================
/**
 * Examples -
 *  1. Configure the PORTA,pin15 in OUTPUT_PUSHPULL mode
 *  	#define PORTA_MODER_MASK          0xC0000000
 *  	#define PORTA_DEFAULT_MODER       0x80000000
 *
 *  	#define PORTA_OTYPER_MASK         0x00008000
 *  	#define PORTA_DEFAULT_OTYPER      0x00000000
 *
 *  	#define PORTA_OSPEEDR_MASK        0xC0000000
 *  	#define PORTA_DEFAULT_OSPEEDR     0xC0000000
 *
 *  2. Configure the PORTB,pin0, pin7 in INPUT_PULLUP mode
 *  	#define PORTB_MODER_MASK          0x0000C003
 *  	#define PORTB_DEFAULT_MODER       0x00000000
 *
 *  	#define PORTB_PUPDR_MASK          0x0000C003
 *  	#define PORTB_DEFAULT_PUPDR       0x00008001
 *
 *  3. Set the PORTA, pin0, pin15
 *      #define PORTA_BSRR_MASK         0x00008001  Please refer the 'Port bit set/reset register(GPIOx_BSRR)'
 *      #define PORTA_DEFAULT_BSRR      0x00008001
 *
 *  4. Clear the PORTA, pin0, pin15
 *      #define PORTA_BSRR_MASK         0x80010000
 *      #define PORTA_DEFAULT_BSRR      0x80010000
 *
 *  Note: 1. If the DEFAULT register is not defined, the specific register is not affected.
 *        2. If the DEFAULT register is defined and the MASK is not defined for that specific port the register
 *           will be fully overwritten by the default.
 *        3. If the DEFAULT register is defined and MASK is also defined for that specific port,
 *           the register will be affected by the DEFAULT value only on the bits set to 1 in the MASK define.
 */
/**
 * GPIO port mode register (GPIOx_MODER)  (x =A..F)
 *
 * 				Bits 2y+1:2y MODERy[1:0]: Port x configuration bits (y = 0..15)
 *				These bits are written by software to configure the I/O mode.
 *					00: Input mode (reset state)
 *					01: General purpose output mode
 *					10: Alternate function mode
 *					11: Analog mode
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|
 *  |MODER15[1:0]|MODER14[1:0]|MODER13[1:0]|MODER12[1:0]|MODER11[1:0]|MODER10[1:0]|MODER9[1:0] |MODER8[1:0] |
 *  |------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |------|-----|------|-----|-----|------|-----|------|-----|------|-----|------|------|-----|------|-----|
 *  |MODER7[1:0] |MODER6[1:0] |MODER5[1:0] |MODER4[1:0] |MODER3[1:0] |MODER2[1:0] |MODER1[1:0] |MODER0[1:0] |
 *  |------|-----|------|-----|-----|------|-----|------|-----|------|------|-----|------|-----|------|-----|
 *
 *
 * GPIO port output type register (GPIOx_OTYPER) (x = A..F)
 * 				Bits 31:16 Reserved, must be kept at reset value.
 *				Bits 15:0 OTy: Port x configuration bits (y = 0..15)
 *				These bits are written by software to configure the I/O output type.
 *						0: Output push-pull (reset state)
 *						1: Output open-drain
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |   NA| NA  |NA   |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |OT15 |OT14 |OT13 |OT12 |OT11 |OT10 |OT9  |OT8  |OT7  |OT6  |OT5  |OT4  |OT3  |OT2  |OT1  |OT0  |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *
 * GPIO port output speed register (GPIOx_OSPEEDR) (x = A..F)
 * 				Bits 2y+1:2y OSPEEDRy[1:0]: Port x configuration bits (y = 0..15)
 *        		These bits are written by software to configure the I/O output speed.
 *					x0: Low speed
 *					01: Medium speed
 *					11: High speed
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|
 *  |OSPEEDR15[1:0]|OSPEEDR14[1:0]|OSPEEDR13[1:0]|OSPEEDR12[1:0]|OSPEEDR11[1:0]|OSPEEDR10[1:0]|OSPEEDR9[1:0] |OSPEEDR8[1:0] |
 *  |------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|
 *  |OSPEEDR7[1:0] |OSPEEDR6[1:0] |OSPEEDR5[1:0] |OSPEEDR4[1:0] |OSPEEDR3[1:0] |OSPEEDR2[1:0] |OSPEEDR1[1:0] |OSPEEDR0[1:0] |
 *  |------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|------|-------|
 *
 *
 * GPIO port pull-up/pull-down register (GPIOx_PUPDR) (x = A..F)
 *
 *				Bits 2y+1:2y PUPDRy[1:0]: Port x configuration bits (y = 0..15)
 *				These bits are written by software to configure the I/O pull-up or pull-down
 *						00: No pull-up, pull-down
 *						01: Pull-up
 *						10: Pull-down
 *						11: Reserved
*    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|
 *  |PUPDR15[1:0]|PUPDR14[1:0]|PUPDR13[1:0]|PUPDR12[1:0]|PUPDR11[1:0]|PUPDR10[1:0]|PUPDR9[1:0] |PUPDR8[1:0] |
 *  |------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|------|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |------|-----|------|-----|-----|------|-----|------|-----|------|-----|------|------|-----|------|-----|
 *  |PUPDR7[1:0] |PUPDR6[1:0] |PUPDR5[1:0] |PUPDR4[1:0] |PUPDR3[1:0] |PUPDR2[1:0] |PUPDR1[1:0] |PUPDR0[1:0] |
 *  |------|-----|------|-----|-----|------|-----|------|-----|------|------|-----|------|-----|------|-----|
 *
 *
 * GPIO port bit set/reset register (GPIOx_BSRR) (x = A..F)
 *
 *				Bits 31:16 BRy: Port x reset bit y (y = 0..15)
 *				These bits are write-only. A read to these bits returns the value 0x0000.
 *					0: No action on the corresponding ODRx bit
 *					1: Resets the corresponding ODRx bit
 *
 *				Bits 15:0 BSy: Port x set bit y (y= 0..15)
 *				These bits are write-only. A read to these bits returns the value 0x0000.
 *					0: No action on the corresponding ODRx bit
 *					1: Sets the corresponding ODRx bit
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |BR15 |BR14 |BR13 |BR12 |BR11 |BR10 |BR9  |BR8  |BR7  |BR6  |BR5  |BR4  |BR3  |BR2  |BR1  |BR0  |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *     15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |BS15 |BS14 |BS13 |BS12 |BS11 |BS10 |BS9  |BS8  |BS7  |BS6  |BS5  |BS4  |BS3  |BS2  |BS1  |BS0  |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 */
//=====================================================================================================================
// Caution !! PTA13(SWD_IO) pins are used by the debuggers. They shall not be touched for no reason. Should be marked as '0' If you touch it your board is gone
// Caution  !! PTA14(SWD_CLK) pin is used by the debugger. They shall not be touched for no reason. Should be marked as '0' If you touch it you loose your board
//PORTA -----------------------
//#define PORTA_MODER_MASK          0xC3FFFFFF
//#define PORTA_DEFAULT_MODER       0x41050000       // 0100 0001 0000 0101 0000 0000 0000 0000 NC - 8,9,12,15

//#define PORTA_OTYPER_MASK         0xFFFF9FFF
//#define PORTA_DEFAULT_OTYPER      0x00000000        //

//#define PORTA_PUPDR_MASK          0xC3FFFFFF
//#define PORTA_DEFAULT_PUPDR       0x00000000

//#define PORTA_OSPEEDR_MASK        0xC3FFFFFF
//#define PORTA_DEFAULT_OSPEEDR     0xFFFFFFFF

//#define PORTA_BSRR_MASK           0x9FFF9FFF
//#define PORTA_DEFAULT_BSRR        0x93000000		//1001 0011 0000 0000 0000 0000 0000 0000 0000
//
////PORTB -----------------------
#define PORTB_MODER_MASK            0x000C0000		// Set Input Mode for pin PB9 (Float Switch), to be read in Low Power mode
#define PORTB_DEFAULT_MODER         0x00000000

//#define PORTB_OTYPER_MASK         0xFFFFFFFF
//#define PORTB_DEFAULT_OTYPER      0x00000000

//#define PORTB_PUPDR_MASK          0xFFFFFFFF
//#define PORTB_DEFAULT_PUPDR       0x00000000

//#define PORTB_OSPEEDR_MASK        0xFFFFFFFF
//#define PORTB_DEFAULT_OSPEEDR     0xFFFFFFFF

//#define PORTB_BSRR_MASK           0xFFFFFFFF
//#define PORTB_DEFAULT_BSRR        0xF3080000		//	1111 0011 0000 1000 0000 0000 0000 0000
//
////PORTC -----------------------
//#define PORTC_MODER_MASK          0xFFFFFFFF
//#define PORTC_DEFAULT_MODER       0x54000000		// 0101 0100 0000 0000 0000 0000 0000 => NC  - 13,14,15

//#define PORTC_OTYPER_MASK         0xFFFFFFFF
//#define PORTC_DEFAULT_OTYPER      0x00000000

//#define PORTC_PUPDR_MASK          0xFFFFFFFF
//#define PORTC_DEFAULT_PUPDR       0x00000000

//#define PORTC_OSPEEDR_MASK        0xFFFFFFFF
//#define PORTC_DEFAULT_OSPEEDR     0xFFFFFFFF

//#define PORTC_BSRR_MASK           0xFFFFFFFF
//#define PORTC_DEFAULT_BSRR        0xE0000000		//	1110 0000 0000 0000 0000 0000 0000 0000

//PORTD -----------------------
#define PORTD_MODER_MASK            0x00003300		// Set Input Mode for pin PD4 (Door Switch) and PD6 (Win Rx), to be read in Low Power mode
#define PORTD_DEFAULT_MODER         0x00000000

//#define PORTD_OTYPER_MASK         0xFFFFFFFF
//#define PORTD_DEFAULT_OTYPER      0x00000000

//#define PORTD_PUPDR_MASK          0xFFFFFFFF
//#define PORTD_DEFAULT_PUPDR       0x00000000

//#define PORTD_OSPEEDR_MASK        0xFFFFFFFF
//#define PORTD_DEFAULT_OSPEEDR     0xFFFFFFFF

//#define PORTD_BSRR_MASK           0xFFFFFFFF
//#define PORTD_DEFAULT_BSRR        0x0

//PORTE -----------------------
//#define PORTE_MODER_MASK          0xFFFFFFFF
//#define PORTE_DEFAULT_MODER       0x00000000
//
//#define PORTE_OTYPER_MASK         0xFFFFFFFF
//#define PORTE_DEFAULT_OTYPER      0x00000000
//
//#define PORTE_PUPDR_MASK          0xFFFFFFFF
//#define PORTE_DEFAULT_PUPDR       0x00000000
//
//#define PORTE_OSPEEDR_MASK        0xFFFFFFFF
//#define PORTE_DEFAULT_OSPEEDR     0xFFFFFFFF
//
//#define PORTE_BSRR_MASK           0xFFFFFFFF
//#define PORTE_DEFAULT_BSRR        0x0

//PORTF -----------------------
//#define PORTF_MODER_MASK           0xFFFFFFFF
//#define PORTF_DEFAULT_MODER        0x00000000

//#define PORTF_OTYPER_MASK          0xFFFFFFFF
//#define PORTF_DEFAULT_OTYPER       0x00000000

//#define PORTF_PUPDR_MASK           0xFFFFFFFF
//#define PORTF_DEFAULT_PUPDR        0x00000000

//#define PORTF_OSPEEDR_MASK         0xFFFFFFFF
//#define PORTF_DEFAULT_OSPEEDR      0xFFFFFFFF

//#define PORTF_BSRR_MASK            0xFFFFFFFF
//#define PORTF_DEFAULT_BSRR         0x0

//This directive controls the code that protects some operations about being interrupted.
//IT accepts the options ENABLED / DISABLED
#define GPIO_PROTECTION_FEATURE   ENABLED

#endif // GPIO_PRM_H_


