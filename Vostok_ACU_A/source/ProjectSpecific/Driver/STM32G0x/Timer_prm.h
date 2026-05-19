/**
 *  @file       Timer_prm.h
 *
 *  @defgroup   CLASS_B
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
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
#ifndef TIMER_PRM_H_
#define TIMER_PRM_H_


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - This macro Enables or Disables the single Timer modules (TIMER1-TIMER3,TIMER14-TIMER17).
 *    @details- The macro must be Enabled before using in the application.
 *
 *    @param  - ENABLED(1)  - Which Enables the Specified Timer
 *              DISABLED(0) – Which Disables the Specified Timer
 *
 *    @note   - Following are the Examples\n
 *              "#define TIMER0_FEATURE        ENABLED"   This Enables the TIMER0 Feature\n
 *              "#define TIMER0_FEATURE        DISABLED"  This Disables the TIMER0 Feature\n
 *              "#define TIMER1_FEATURE        ENABLED"   This Enables the TIMER1 Feature\n
 *              "#define TIMER1_FEATURE        DISABLED"  This Disables the TIMER1 Feature
 */
#define TIMER1_FEATURE                                        ENABLED
#define TIMER2_FEATURE                                        DISABLED
#define TIMER3_4_FEATURE                                      ENABLED
#define TIMER14_FEATURE                                       ENABLED
#define TIMER15_FEATURE                                       DISABLED
#define TIMER16_FEATURE                                       ENABLED
#define TIMER17_FEATURE                                       DISABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  - Configure the maximum capture channels used.
 * @details- Only TIMER1, TIMER2, TIMER3, TIMER4 can be used as capture channels
 *
 * @param  - 0 -        Disable
 *           1 to 16 -  Total capture channels available in the STM32F10x
 *
 *
 * @note   - Following are the Examples\n
 *           "#define TIMER_MAX_CAPTURE_CHNLS_USED        2"         2 capture channels are used
 *           "#define TIMER_MAX_CAPTURE_CHNLS_USED        0"         Capture feature is disabled
 */
#define TIMER_MAX_CAPTURE_CHNL              2

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  - Configure the maximum counters are used
 * @details-
 *
 * @param  - 0 -        Disable
 *           1 to 255
 *
 *
 * @note   - Following are the Examples\n
 *           "#define TIMER_MAX_COUNTER_USED              2"         2 counters are created
 *           "#define TIMER_MAX_COUNTER_USED              0"         No counters are created
 */
#define TIMER_MAX_COUNTER_USED              2

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  - This macro defines the Timer interrupt group priority.
 * @details- Consists of 4 interrupt groups and each group consists of 4 sub-priorities.
 *           Only the group priority determines preemption of interrupt exceptions.
 *           When the processor is executing an interrupt exception handler, another interrupt with the
 *           same group priority as the interrupt being handled does not preempt the handler.
 *           If multiple pending interrupts have the same group priority, the sub-priority field determines
 *           the order in which they are processed. If multiple pending interrupts have the same group priority
 *           and sub-priority, the interrupt with the lowest IRQ number is processed first.
 *           The lower the Group priority the greater the priority of the corresponding interrupt group.
 *           The lower the sub-priority the greater the priorities of the corresponding interrupt within the group.
 *
 * @param  - 0/1/2/3
 *
 * @note   - Following are the Examples\n
 *           "#define TIMER1_PREEMPTION_PRIORITY 0"   The TIMER1 is in interrupt group 0 having highest priority
 *           "#define TIMER1_PREEMPTION_PRIORITY 1"   The TIMER1 is in interrupt group 1 having priority less than the UART1
 */
#define TIMER1_PREEMPTION_PRIORITY           1
#define TIMER2_PREEMPTION_PRIORITY           2
#define TIMER3_4_PREEMPTION_PRIORITY         2
#define TIMER14_PREEMPTION_PRIORITY          2
#define TIMER15_PREEMPTION_PRIORITY          2
#define TIMER16_PREEMPTION_PRIORITY          2
#define TIMER17_PREEMPTION_PRIORITY          2

#endif // TIMER_PRM_H_
