/**
 * @file
 * @defgroup   CLASS_B
 * @brief    Contains the controller related structures like clock related structure definition and
 *              Nested vector interrupt structure. This is specific to the STM32F10x controller.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef MICRO_DEFS_H_
	#define MICRO_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    uint32 SYSCLK_Frequency;
    uint32 HCLK_Frequency;
    uint32 PCLK1_Frequency;
}MICRO_CLOCK_DEF;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief The MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL() macro defines a local variable of the type istate_t
 * @details The local variable will be used to keep the interrupt context in a local scope. The call for this macro should be placed
 * only once in the top of the function along with the local variables declaration.
 * @note
 * <pre>
 * void Function (void)
 * {
 *      variables declaration...
 *      <B>MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();</B>
 *
 *      code...
 *      MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
 *      protected code 1...
 *      MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
 *
 *      code...
 *      MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
 *      protected code 2...
 *      MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
 * }
 * </pre>
 */
#define MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL()     __istate_t local_interrupt_context;

/**
 * @brief The MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL macro saves the interrupt context into the local variable and disable interrupts
 * @details This macro requires the macro MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL() to be added in the top of the function in order to
 * declare the local variable used to keep the interrupt context.
 * @note
 * <pre>
 * void Function (void)
 * {
 *      variables declaration...
 *      MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
 *
 *      code...
 *      <B>MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();</B>
 *      protected code 1...
 *      MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
 *
 *      code...
 *      <B>MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();</B>
 *      protected code 2...
 *      MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
 * }
 * </pre>
 */
#define MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL()        {   local_interrupt_context = __get_interrupt_state(); \
                                                        __disable_interrupt();}

/**
 * @brief The MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL() macro restores the I bit in the PSW register so restabilishing the interrupt context
 * @details This macro requires the macro MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL() to be added in the top of the function in order to
 * declare the local variable used to keep the interrupt context.
 * @note
 * <pre>
 * void Function (void)
 * {
 *      variables declaration...
 *      MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
 *
 *      code...
 *      MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
 *      protected code 1...
 *      <B>MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();</B>
 *
 *      code...
 *      MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
 *      protected code 2...
 *      <B>MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();</B>
 * }
 * </pre>
 */
#define MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL()     {   __set_interrupt_state(local_interrupt_context);}

void Micro__NVICEnableIRQ(IRQ_CHANNEL_TYPE irqchnl, uint8 priority);
void Micro__NVICDisableIRQ(IRQ_CHANNEL_TYPE irqchnl);
void Micro__HardwareInit(void);
void Micro__WatchdogInit(void);
uint8 Micro__NumberOfInterruptsDefined(void);

#endif // MICRO_DEFS_H_
