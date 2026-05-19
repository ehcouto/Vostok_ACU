/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This module implements the standard API (Application Programming Interface) for
 *              STM32G0x Micro module.
 *
 *  @details    This module provides the interface to the application to configure the controller and to perform the
 *              CLASSB safety relevant controller tests.
 *
 *              Configuring the Controller such as-
 *              Initializing microcontroller bus clock, configuring clock for microcontroller,
 *              Configures the controller to work either in slow, normal or sleep mode of operation,
 *              servicing Window and Independent watchdog,
 *              enabling/disabling or store/restoring of the interrupts,
 *              Providing FLASH size ,RAM size and ID of microcontroller,
 *              provision to detect Low voltage.
 *
 *              CLASS B safety-relevant controller test during boot as well as run time such as -
 *              CPU Register Test,
 *              Watchdog test,
 *              RAM Test(at boot time),
 *              Flash test
 *              stack test
 *
 * @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
  */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "uc.h"
#include "Micro.h"
#include "Micro_prv.h"
#include "SRException.h"
#include "Crc8.h"
#include <string.h>

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

#if (MICRO_SR_FEATURE_TEST == ENABLED)
    #include "SRData.h"
#endif


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (MICRO_SR_FEATURE_TEST == ENABLED)
    #if (MICRO_WATCHDOG_FEATURE == DISABLED)
        #error"MICRO_WATCHDOG_FEATURE shall be enabled if the MICRO_SR_FEATURE_TEST is enabled"
    #endif
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief   -This define enables or disables the debug mode.
 *
 *    @param  - ENABLED: Stops TIM1,TIM2,TIM3,TIM6,TIM14,TIM15,TIM16,TIM17,WWDG,IWDG when the micro enter in halt (debug)
 *              DISABLED: Keep the peripherals running when the micro is in HALT
 *
 *    @note    -"#define MICRO_DEBUG_MODE          ENABLED" //Debug mode ON
 *              "#define MICRO_DEBUG_MODE          DISABLED" //Debug mode OFF
 */
#define MICRO_DEBUG_MODE                                    ENABLED

#define OPTION_BYTE_START_ADDRESS           0x1FFF787F
#define OPTION_BYTE_END_ADDRESS             0x1FFF7800



// Interrupt Priorities are WORD accessible only under ARMv6M
// The following MACROS handle generation of the register offset and byte masks
#define IRQ_BIT_SHIFT(IRQn)              ( (((uint32)(IRQn)       )    &  0x03) * 8 )
#define IRQ_SHP_IDX(IRQn)                (((((uint32)(IRQn) & 0x0F)-8) >>    2)     )
#define IRQ_IP_IDX(IRQn)                 (  ((uint32)(IRQn)            >>    2)     )

extern uint32 __ICFEDIT_INTERRUPT_TABLE_start__;
#define INTERRUPT_TABLE_START_ADDRESS           ((uint32)&__ICFEDIT_INTERRUPT_TABLE_start__)
//=====================================================================================================================
//Compiler directive
//=====================================================================================================================
extern uint32 RAM_TOTAL_SIZE;

#define RAM_SIZE         ((uint32)(&RAM_TOTAL_SIZE))        // Flash lower boundary.

// By Default LSI clock is always ON.
// During debugging it is observed that the LSI clock gets ready within 13 count value.
#define LSISTARTUP_TIMEOUT                  1000
#define HSISTARTUP_TIMEOUT                  1000
#define PLLSTARTUP_TIMEOUT                  1000
#define FLASHLATENCY_TIMEOUT                1000

#define MICRO_FLASH_SIZE()                  ((uint32)FLASH_SIZE)
#define MICRO_RAM_SIZE()                    ((uint32)RAM_SIZE)
#define MICRO_CPU_ID()                      ((MICRO_ID_TYPE * )(&MICRO_CPU_IDENTIFICATION))





// Debug mode
#ifndef MICRO_DEBUG_MODE
    #define DBGMCU_APB1_FZ_STOP   0x00000000
    #define DBGMCU_APB2_FZ__STOP  0x00000000
#else
    #if (MICRO_DEBUG_MODE == ENABLED)
        // Halts the TIM2,TIM3,TIM6,TIM14,WWDG,IWDG
        #ifndef DBGMCU_APB1_FZ_STOP
            #define DBGMCU_APB1_FZ_STOP   ( DBG_APB_FZ1_DBG_TIM2_STOP | \
                                            DBG_APB_FZ1_DBG_TIM3_STOP | \
                                            DBG_APB_FZ1_DBG_TIM4_STOP | \
                                            DBG_APB_FZ1_DBG_TIM6_STOP | \
                                            DBG_APB_FZ1_DBG_TIM7_STOP | \
                                            DBG_APB_FZ1_DBG_RTC_STOP  | \
                                            DBG_APB_FZ1_DBG_WWDG_STOP | \
                                            DBG_APB_FZ1_DBG_IWDG_STOP | \
                                            DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP | \
                                            DBG_APB_FZ1_DBG_I2C2_SMBUS_TIMEOUT_STOP | \
                                            DBG_APB_FZ1_DBG_LPTIM2_STOP | \
                                            DBG_APB_FZ1_DBG_LPTIM1_STOP )
        #endif


        // Halts the TIM1,TIM15,TIM16,TIM17
        #ifndef DBGMCU_APB2_FZ__STOP
            #define DBGMCU_APB2_FZ__STOP   (DBG_APB_FZ2_DBG_TIM1_STOP  | \
                                            DBG_APB_FZ2_DBG_TIM14_STOP | \
                                            DBG_APB_FZ2_DBG_TIM15_STOP | \
                                            DBG_APB_FZ2_DBG_TIM16_STOP | \
                                            DBG_APB_FZ2_DBG_TIM17_STOP )
        #endif
    #else
        #define DBGMCU_APB1_FZ_STOP   0x00000000
        #define DBGMCU_APB2_FZ__STOP  0x00000000
    #endif
#endif

// Independent watchdog
#ifndef MICRO_WATCHDOG_FEATURE
    #define MICRO_WATCHDOG_FEATURE                  ENABLED
#endif

#if (MICRO_WATCHDOG_FEATURE == ENABLED)
    #if ((MICRO_WATCHDOG_TIMEOUT_IN_MS < 100) || (MICRO_WATCHDOG_TIMEOUT_IN_MS > 1000))
        #error:"MICRO_WATCHDOG_TIMEOUT_IN_MS shall be set between 100ms to 1000ms only"
    #endif
#endif

// Low voltage detector
#ifndef MICRO_LOW_VOLTAGE_MONITOR_FEATURE
    #define MICRO_LOW_VOLTAGE_MONITOR_FEATURE        DISABLED
#endif

#if (MICRO_LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)
    #ifndef MICRO_LOW_VOLTAGE_LEVEL
        #error:"MICRO_LOW_VOLTAGE_LEVEL macro must be defined in the Micro_prm.h file"
    #else
        #if MICRO_LOW_VOLTAGE_LEVEL > 6
            #error:"MICRO_LOW_VOLTAGE_LEVEL macro value must be within 0 to 6  and should be defined in the Micro_prm.h file"
        #endif
    #endif

    // PVD(Programmable Voltage detector) interrupt priority
    #ifndef PVD_PREEMPTION_PRIORITY
        #define PVD_PREEMPTION_PRIORITY     3  //The PVD(Programmable Voltage detector) is in interrupt group 0 having highest priority
    #elif (PVD_PREEMPTION_PRIORITY > 3)
        #error:"PVD_PREEMPTION_PRIORITY macro value must be either 0,1,2,3"
    #endif
#endif

typedef enum
{
    SYSTEM_CLOCK_NORMAL_SPEED,
    SYSTEM_CLOCK_LOW_SPEED_2MHZ,
    SYSTEM_CLOCK_LOW_SPEED_4MHZ,
}SYSTEM_CLOCK_SPEED;

//=====================================================================================================================
//Variable section
//=====================================================================================================================
static const MICRO_ID_TYPE MICRO_CPU_IDENTIFICATION = {(uint8 *)UID_BASE, 16};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void MicroInitClk(SYSTEM_CLOCK_SPEED speed);
static void MicroGetClocksStatus(void);
static MICRO_LAST_RESET_TYPE MicroReadActualReset(void);

#if (MICRO_SR_FEATURE_TEST == ENABLED)
    static void MicroBootFlashTest(void);
    static void MicroBootWatchdogTest(void);
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * List all the generic structures, variable, private function prototype
 */
#ifndef MICRO_SRPOWERMGR_FEATURE
    #define MICRO_SRPOWERMGR_FEATURE    DISABLED
#endif

#if (MICRO_SRPOWERMGR_FEATURE == ENABLED)
    #include "SRPowerMgr.h"
#endif

#define MICRO_RESETINFO_STRING_SIZE             44

PACKED typedef struct
{
    uint8 Header[MICRO_RESETINFO_STRING_SIZE];

    //The Reset member shall be always after the Header memeber
    MICRO_LAST_RESET_TYPE Reset;
    MICRO_LAST_RESET_TYPE Actual_Reset;
    MICRO_RESET_MODE_TYPE Mode;
    MICRO_WARM_RESET_ACTION_TYPE Warm_Reset_Actions;
    uint8 Loop_Reset_Count;
    uint8 Warm_Reset_Count;
    //This variable shall always be at the end of the structure
    uint8 Crc8_Data_Integrator;
}MICRO_RESET_INFO_HEADER_TYPE;

//! Holds the Clock status
static MICRO_CLOCK_DEF  Micro_ClocksStatus;

const uint8 MICRO_RESETINFO_HEADER[MICRO_RESETINFO_STRING_SIZE] =      {"Dream is not the thing you see in sleep but"};
const uint8 MICRO_RESETINFO_FOOTER[MICRO_RESETINFO_STRING_SIZE] =      {"it is that thing that doesn't let you sleep"};

#pragma location = ".RESET_INFO_HEADER"
static NO_INIT MICRO_RESET_INFO_HEADER_TYPE Micro_ResetInfo;

#pragma location = ".RESET_INFO_FOOTER"
static NO_INIT uint8 Micro_ResetInfo_Footer[MICRO_RESETINFO_STRING_SIZE];

//=====================================================================================================================
//! ClassB Related
//=====================================================================================================================

#define MICRO_RESETINFO_CLASSB_DATA_FROM      ((uint8 *)&Micro_ResetInfo.Reset)
#define MICRO_RESETINFO_CLASSB_DATA_SIZE      (sizeof(Micro_ResetInfo) - MICRO_RESETINFO_STRING_SIZE - 1)       // -1 to exclude the Crc location

//! Stack test patterns
#define RAM_TEST_PATTERN                    0x55U
#define RAM_TEST_PATTERN_INV                0xAAU
#define RAM_TEST_BKD                        0x00U
#define RAM_TEST_BKD_INV                    0xFFU

//"Whirlpool100Year"
#define STACK_TEST_PATTERN1                 0x72696857U
#define STACK_TEST_PATTERN2                 0x6F6F706CU
#define STACK_TEST_PATTERN3                 0x3030316CU
#define STACK_TEST_PATTERN4                 0x72616559U

//"EES Organization"
#define STACK_TEST_PATTERN5                 0x20534545U
#define STACK_TEST_PATTERN6                 0x6167724FU
#define STACK_TEST_PATTERN7                 0x617A696EU
#define STACK_TEST_PATTERN8                 0x6E6F6974U

#ifndef MICRO_CALLBACK_BEFORE_WATCHDOG_INIT
    #define MICRO_CALLBACK_BEFORE_WATCHDOG_INIT()
#endif

#define MICRO_BOOT_SEQUENCE_1               3U
#define MICRO_BOOT_SEQUENCE_2               5U
#define MICRO_BOOT_SEQUENCE_3               11U
#define MICRO_BOOT_RAM_TEST_FLAG            0x5A000000U
#define MICRO_BOOT_RAM_TEST_MASK            0xFF000000U
#define MICRO_BOOT_RAM_TEST_SEQUENCE_MASK   0x000000FFU

#define RUN_TIME_RAM_TEST_TRUE              0x000000A5U
#define RUN_TIME_RAM_TEST_PROP_MASK         0x000000FFU

#ifndef MICRO_WARM_RESET_COUNTER_LIMIT
    #define MICRO_WARM_RESET_COUNTER_LIMIT  20
#endif

// Below symbols are exported from the linker file (config.icf)

#pragma section = "STACK_UNDERFLOW_SIGNATURE"
#pragma section = "STACK_OVERFLOW_SIGNATURE"
#define MICRO_STACK_TEST_START_ADDRESS         ((uint32)__section_begin("STACK_OVERFLOW_SIGNATURE"))
#define MICRO_STACK_TEST_END_ADDRESS           ((uint32)__section_begin("STACK_UNDERFLOW_SIGNATURE"))

extern uint32  __ICFEDIT_region_RAM_start__;
extern uint32  __ICFEDIT_region_RAM_end__;
#define MICRO_RAM_START_ADDRESS         ((uint32)&__ICFEDIT_region_RAM_start__)
#define MICRO_RAM_END_ADDRESS           ((uint32)&__ICFEDIT_region_RAM_end__)

extern uint32  MICRO_FLASH_START;
extern uint32  MICRO_FLASH_END;
#define MICRO_FLASH_START_ADDRESS   ((uint32 *)&MICRO_FLASH_START)
#define MICRO_FLASH_END_ADDRESS     ((uint32 *)&MICRO_FLASH_END)

#if (MICRO_SR_FEATURE_TEST == ENABLED)
    /**
     * RAM Periodic Test
     */
    static uint8 *RAM_Test_Pointer;
    static uint8 *N_RAM_Test_Pointer;

    /**
     * Flash Periodic Test
     */
    #include "Crc16.h"

    // symbol MICRO_FLASH_CRC created by the IELF tool, this holds the calculated CRC value on the flash memory.
    extern uint16 MICRO_FLASH_CRC;

    static uint16 Micro_Flash_Crc;
    static uint32 * Micro_Flash_Crc_Pt;

    #define MICRO_CRC_INITIAL           0x0000
#endif

static void MicroResetHandling(void);
static void MicroClearResetData(void);
static void MicroSetRamSignature(void);
static BOOL_TYPE MicroIsResetSignatureValid(void);
static void MicroValidateResetInfoDataCrc(void);
static void MicroUpdateResetInfoDataCrc(void);

#if (MICRO_SR_FEATURE_TEST == ENABLED)
    static void MicroBootFlashTest(void);
    static void MicroFailSafe(void);
#endif



//End of Generic Section ---------------------------------------------------------------------------------------------

// After boot test __iar_program_start which calls the void main(void)
extern void __iar_program_start( void );

//Create a lib_init() function, for the C initialization. This function will copy the initial values for
//static and global initialized variables from ROM to RAM, and initialize zero-initialized data to 0
#ifndef RAM_DATA_INITIALIZE
    extern void __iar_data_init3(void);
    #define RAM_DATA_INITIALIZE()   __iar_data_init3()
#endif

extern void main(void);

#define MICRO_BOOTTEST_WDTINIT()                      Micro__WatchdogInit()
#define MICRO_BOOTTEST_MICROHWINIT()                  Micro__HardwareInit()
#define MICRO_BOOTTEST_RAMTEST()                      __BootTimeRamTest()
#define MICRO_BOOTTEST_REGTEST()                      __StartupRegisterTest()
#define MICRO_BOOTTEST_FLASHTEST()                    MicroBootFlashTest()
#define MICRO_BOOTTEST_WDTEST()                       MicroBootWatchdogTest()
#define MICRO_BOOTTEST_CLKINIT()                      MicroInitClk(SYSTEM_CLOCK_NORMAL_SPEED)
#define MICRO_RUNTIME_REGISTER_TEST()                 __RunRegisterTest()

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Micro module
 *    @details  This method is used to initialize clock, Nested Vectored Interrupt (NVIC) and
 *              window watchdog configuration. It should be called prior to any other application method is called
 *              since it initializes micro module.
 *
 *              The Gpio module must be re-initialized if the Micro__Initialize() is called in the middle
 *              of the Application code.
 */
void Micro__Initialize(void)
{
	#if (MICRO_SR_FEATURE_TEST == ENABLED)
		uint32 * pt_stack_test;
	#endif

    // Upon reset, the UCPD CCx lines present a pull-down resistor that can be disabled by
    // setting the UCPDx_STROBE bit of the SYSCFG_CFGR1 register
    SYSCFG.CFGR1 = SYSCFG_CFGR1_UCPD1_STROBE | SYSCFG_CFGR1_UCPD2_STROBE;

    // This method read the current system clock configuration and update Micro_ClocksStatus
    MicroGetClocksStatus();

    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        //Initialize the flash periodic test variables.
        Micro_Flash_Crc = MICRO_CRC_INITIAL;
        Micro_Flash_Crc_Pt = MICRO_FLASH_START_ADDRESS;

        //Initialize the Ram periodic test variables.
        SRData__UpdateLong((void *)&RAM_Test_Pointer, (void *)&N_RAM_Test_Pointer, MICRO_RAM_START_ADDRESS);
        
        //Write the signatures over the stack top and bottom to detect
        pt_stack_test = (uint32 *) MICRO_STACK_TEST_END_ADDRESS;
        pt_stack_test[3] = STACK_TEST_PATTERN4;
        pt_stack_test[2] = STACK_TEST_PATTERN3;
        pt_stack_test[1] = STACK_TEST_PATTERN2;
        pt_stack_test[0] = STACK_TEST_PATTERN1;

        pt_stack_test = (uint32 *) MICRO_STACK_TEST_START_ADDRESS;
        pt_stack_test[0] = STACK_TEST_PATTERN5;
        pt_stack_test[1] = STACK_TEST_PATTERN6;
        pt_stack_test[2] = STACK_TEST_PATTERN7;
        pt_stack_test[3] = STACK_TEST_PATTERN8;
    #endif

    DISABLE_INTERRUPTS();

    //refresh the wathcdog
    Micro__ServiceWatchdog();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method makes the controller to work at low speed
 *    @details  The clock speed in the low power mode is depend on the clock source
 *
 *    Note - 1. The low speed clock should not be set less than 4MHz to run the Wide and time dependent Application.
 *           2. For the time precision application its recommend to use the External clock source during the low speed.
 */
void Micro__SetLowSpeedMode(void)
{
    MicroInitClk(SYSTEM_CLOCK_LOW_SPEED_2MHZ);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method configured the Low Voltage Detection if the feature is available.
 * @image    html SRMicro_LVD.jpg
 */
void Micro__ConfigureLvd(void)
{
    #if (MICRO_LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)

        //Enable the clock for Power control module
        RCC.APBENR1 |= RCC_APBENR1_PWREN;

        SYSCFG.CFGR2 &= ~SYSCFG_CFGR2_PVDL;

        //Enable the voltage detector
        PWR.CR2 |= PWR_CR2_PVDE | (MICRO_LOW_VOLTAGE_LEVEL << PWR_CR2_PVDFT_POS) | (MICRO_LOW_VOLTAGE_LEVEL << PWR_CR2_PVDRT_POS);

        // Connect the Alternate function device to the bus
        RCC.APBENR2 |= RCC_APBENR2_SYSCFGEN;

        //Enable the External mask register for External Line 16
        EXTI.IMR1 |= EXTI_IMR1_IM16;

        //Configure the interrupt for rise time
        EXTI.RTSR1 |= EXTI_RTSR1_RT16;

        Micro__NVICEnableIRQ(PVD_IRQ_CHANNEL, PVD_PREEMPTION_PRIORITY);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method should be called from the Low Voltage detection interrupt handle which is application specific.
 *        This function only clear the register related to the interruption if need.
 *        This is not the handler for the interrupt, this code should be managed by the Application specific code which knows
 *        what must be done during the LVD ISR.
 */
void Micro__ClearLvdIsrRegisters(void)
{
    // Connect the Alternate function device to the bus
    RCC.APBENR2 |= RCC_APBENR2_SYSCFGEN;

    //Clear pending interrupts
    EXTI.RPR1 |= EXTI_RPR1_RPIF16;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the current status of the LVD.
 *
 *    @return   MICRO_LVD_STATUS_TYPE :
 *               MICRO_LVD_DISABLED              - If the LVD is disabled
 *               MICRO_LVD_ENABLED_NOT_DETECTING - The LVD is enabled and low voltage threshold condition not happened
 *               MICRO_LVD_ENABLED_DETECTING     - The LVD is enabled and low voltage threshold condition is occured
 */
MICRO_LVD_STATUS_TYPE  Micro__GetLvdStatus(void)
{
    MICRO_LVD_STATUS_TYPE lvdreturn;

    lvdreturn = MICRO_LVD_DISABLED;

    #if (MICRO_LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)
        if((PWR.SR2 & PWR_SR2_PVDO) != 0)
        {
            lvdreturn = MICRO_LVD_ENABLED_DETECTING;
        }
        else
        {
            lvdreturn = MICRO_LVD_ENABLED_NOT_DETECTING;
        }
    #endif

    return (lvdreturn);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This function enables and sets priority for a device specific interrupt in the NVIC interrupt controller.
 *    @details
 *    @param    irqchnl     : Defines the IRQ Channel number should be from the "IRQ_CHANNEL_TYPE" enum list.
 *    @param    priority    : Should range from 0 to 3.
 *                            0 is the Highest priority and 3 is the lowest priority.
 *
 *    Note: The priority cannot be set for every core interrupt.
 */
void Micro__NVICEnableIRQ(IRQ_CHANNEL_TYPE irqchnl, uint8 priority)
{
    if(irqchnl < 0)
    {
        SCB.SHP[IRQ_SHP_IDX(irqchnl)] = (SCB.SHP[IRQ_SHP_IDX(irqchnl)] & ~(0xFF << IRQ_BIT_SHIFT(irqchnl))) |
        (((priority << (8 - NVIC_PRIO_BITS)) & 0xFF) << IRQ_BIT_SHIFT(irqchnl));
    }
    else
    {
        NVIC.Priority[IRQ_IP_IDX(irqchnl)] = (NVIC.Priority[IRQ_IP_IDX(irqchnl)] & ~(0xFF << IRQ_BIT_SHIFT(irqchnl))) |
        (((priority << (8 - NVIC_PRIO_BITS)) & 0xFF) << IRQ_BIT_SHIFT(irqchnl));

        NVIC.Enable[0] = (1 << ((uint32)(irqchnl) & 0x1F));
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Disable External Interrupt
 *    @details  This function disables a device specific interrupt in the NVIC interrupt controller.
 *              The interrupt number cannot be a negative value.
 *
 *    @param    irqchnl     : Defines the IRQ Channel number should be from the "IRQ_CHANNEL_TYPE" enum list.
 */
void Micro__NVICDisableIRQ(IRQ_CHANNEL_TYPE irqchnl)
{
    if(irqchnl > 0)
    {
        NVIC.Disable[0] = (1 << ((uint32)(irqchnl) & 0x1F));
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * List all the generic API's used across different micro
 */
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs the Boot test during the system boot.
 *    @details  This method is the first function to be executed in the system and from this function the code
 *              goes to the main function.
 *
 *              This method is responsible for:
 *              CPU Register Boot Test,
 *              Watchdog Boot test,
 *              RAM MarchC test,
 *              Flash Boot test
 *              and writing stack test pattern at the boundaries of stack memory.
 *              After this it calls the system startup routine.
 *
 *              Time Calculation:
 *              Micro__BootTest takes 136ms to 142ms time period for the below parameters-
 *              System clock                - 48MHz, external clock
 *              RAM memory range            - 0x20000000 to 0x20001FFF
 *              Flash memory range          - 0x080000C0 to 0x0800FFFF
 *
 *              Note-1. The Independent watchdog refresh time should be big enough to perform the boot test
 *                   2. The boot test should not be performed when the application is running.
 */
#pragma optimize=none
INTERRUPT_DIRECTIVE void Micro__BootTest(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
    // Byte 3 - RAM March C running keyword
    // Byte 2 - save_reset_status
    // Byte 1 - save_reset_sync
    // Byte 0 - counter
        register uint32 save_reset_value;
        register uint32 tmp_data;
        register uint32 counter;
        register uint32 mask;
    #endif

    MICRO_BOOTTEST_MICROHWINIT();
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : Full register test
        //
        //Its mandatory to test all the CPU register test
        //-----------------------------------------------------------------------------------------------------------------
        if (FALSE == MICRO_BOOTTEST_REGTEST())
        {
            MicroFailSafe();
        }
    #endif

    //This function realizes the reset mode
    MicroResetHandling();
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        save_reset_value = 1U;
    #endif

    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        RAM_DATA_INITIALIZE();
    }

    #if (MICRO_SRPOWERMGR_FEATURE == ENABLED)
        SRPowerMgr__Initialize();
    #endif

    MICRO_CALLBACK_BEFORE_WATCHDOG_INIT();

    //Initialize the watchdog
    MICRO_BOOTTEST_WDTINIT();
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        save_reset_value++;
    #endif

    //Initializes the system clock
    MICRO_BOOTTEST_CLKINIT();

    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        save_reset_value++;
        MicroValidateResetInfoDataCrc();
        if ((Micro_ResetInfo.Warm_Reset_Actions & MICRO_WARM_RESET_ACTION_SKIP_CLASSB_TEST) == 0)
        {

            if (save_reset_value != MICRO_BOOT_SEQUENCE_1)
            {
              MicroFailSafe();
            }

            //Skip storing the reset reason as this is the intended reset for test
            Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST);

            MICRO_BOOTTEST_WDTEST();
            save_reset_value++;
            SERVICE_WATCHDOG();

            MICRO_BOOTTEST_FLASHTEST();
            save_reset_value++;
            SERVICE_WATCHDOG();

            //Skip the ClassB test during warm reset
            Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_SKIP_CLASSB_TEST);
        }
        else
        {
            save_reset_value += 2U;
        }

        if (save_reset_value != MICRO_BOOT_SEQUENCE_2)
        {
          MicroFailSafe();
        }
    #endif

    MicroValidateResetInfoDataCrc();
    if (Micro_ResetInfo.Mode == MICRO_RESET_MODE_COLD)
    {
        MicroValidateResetInfoDataCrc();

        #if (MICRO_SR_FEATURE_TEST == ENABLED)
            //Save reset status on Auto register as March C test will screw the all ram memory content
            save_reset_value   |=   (*(uint8 *)(&Micro_ResetInfo.Loop_Reset_Count)) << 8U;
            save_reset_value   |=   ((*(uint8 *)(&Micro_ResetInfo.Reset)) << 16U);

            //-----------------------------------------------------------------------------------------------------------------
            // CLASSB Test : RAM MARCH C Test and Bit coupling test
            //
            // This test is used to validate the volatile memory by using the MARCH C test and bit coupling test
            //-----------------------------------------------------------------------------------------------------------------
            SERVICE_WATCHDOG();

            save_reset_value |= MICRO_BOOT_RAM_TEST_FLAG;

            //=>W0
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                *((uint8 *)counter) = RAM_TEST_BKD;
            }
            save_reset_value++;
            //=>(R0, W1)
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                if (*((uint8 *)counter) != RAM_TEST_BKD)
                {
                    save_reset_value &= ~MICRO_BOOT_RAM_TEST_FLAG;
                }
                *((uint8 *)counter) = RAM_TEST_BKD_INV;
            }
            save_reset_value++;
            SERVICE_WATCHDOG();

            //=>(R1, W0)
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                if (*((uint8 *)counter) != RAM_TEST_BKD_INV)
                {
                    save_reset_value &= ~MICRO_BOOT_RAM_TEST_FLAG;
                }
                *((uint8 *)counter) = RAM_TEST_BKD;
            }
            save_reset_value++;
            //<=(R0, W1)
            for (counter = MICRO_RAM_END_ADDRESS; counter >= MICRO_RAM_START_ADDRESS; counter--)
            {
                if (*((uint8 *)counter) != RAM_TEST_BKD)
                {
                    save_reset_value &= ~MICRO_BOOT_RAM_TEST_FLAG;
                }
                *((uint8 *)counter) = RAM_TEST_BKD_INV;
            }
            save_reset_value++;

            SERVICE_WATCHDOG();

            //<=(R1, W0)
            for (counter = MICRO_RAM_END_ADDRESS; counter >= MICRO_RAM_START_ADDRESS; counter--)
            {
                if (*((uint8 *)counter) != RAM_TEST_BKD_INV)
                {
                    save_reset_value &= ~MICRO_BOOT_RAM_TEST_FLAG;
                }
                *((uint8 *)counter) = RAM_TEST_BKD;
            }
            save_reset_value++;

            //=>R0
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                if (*((uint8 *)counter) != RAM_TEST_BKD)
                {
                    save_reset_value &= ~MICRO_BOOT_RAM_TEST_FLAG;
                }
            }
            save_reset_value++;
            if ((save_reset_value & MICRO_BOOT_RAM_TEST_MASK) != MICRO_BOOT_RAM_TEST_FLAG)
            {
                //If the test fails
                MicroFailSafe();
            }

            SERVICE_WATCHDOG();

            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter += 4)
            {
                tmp_data = *((uint32 *) counter);
                SERVICE_WATCHDOG();

                for (mask = 1; mask > 0; mask = (mask << 1))
                {
                    *((uint32 *) counter) = mask;
                    if (*((uint32 *) counter) != mask)
                    {
                        MicroFailSafe();

                    }
                }
                *((uint32 *) counter) = tmp_data;
            }

            //refresh the watch dog
            SERVICE_WATCHDOG();
            if ((save_reset_value & MICRO_BOOT_RAM_TEST_SEQUENCE_MASK) != MICRO_BOOT_SEQUENCE_3)
            {
                MicroFailSafe();
            }

            //restore the reset status
            MicroClearResetData();
            (*(uint8 *)(&Micro_ResetInfo.Reset))        = (save_reset_value >> 16) & 0xFFU;
            (*(uint8 *)(&Micro_ResetInfo.Loop_Reset_Count)) = (save_reset_value >> 8) & 0xFFU;
            Micro_ResetInfo.Actual_Reset.WATCHDOG           = TRUE;

            MicroUpdateResetInfoDataCrc();

            Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_SKIP_CLASSB_TEST);
        #endif

        // Go to compiler startup
        __iar_program_start();
    }
    else
    {
        main();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs volatile memory (CPU register, Stack) test periodically.
 *    @details  This method is responsible for:
 *              CPU Registers Test,
 *              Stack memory complete test
 *              Stack overflow and underflow test.
 *
 *              MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST configuration macro defined in the SRMicro_prv.h file
 *              Defines how many bytes will be processed during each interaction of the Continuous stack memory Test.
 *
 *
 *              The time taken for complete stack memory test is calculated below-
 *              FULL RAM TEST (in ms) = (Stack Size/ MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST) *
 *                                       Call rate(of Micro__RamTest())
 *
 *             For example-
 *              Stack memory  size is 2048bytes,  number of bytes processed in each call are 4 and the call rate is 100ms then,
 *              FULL  Stack memory(in ms) =  (2048/4)*100 ms
 *                                        =  51200 ms
 *
 *              Along with this it provides the flow control log and exception control for each tests.
 *
 *              This API is been called from ApplienceMgr Thread at every 100ms
 */
#pragma optimize=none
void Micro__SRRamTest(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        //Used to hold the data of a RAM memory location on which the RAM test is conducted
        //Byte3 - ram_next_location_data
        //Byte2 - ram_current_location_data
        //Byte1 - ram_previous_location_data
        //Byte 0 - result - test result
	    register uint32 ram_test_data;

        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        uint32 bytes_processed;

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_MICRO_SRRAMTEST_ENTER);
        #endif

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : CPU run time Test
        //
        //Tests the R0-R7 registers
        //If any error is found an exception will be thrown.
        //-----------------------------------------------------------------------------------------------------------------
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        if (FALSE == MICRO_RUNTIME_REGISTER_TEST())
        {
            //If the test fails throw test fail exception
            SRException__Queue(SREXCEPTION_MICRO_CPU_REGISTER_TEST,0,0);
        }

        //Restore Interrupts
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : RAM periodic Test
        //
        // This test is used to validate the volatile memory
        //-----------------------------------------------------------------------------------------------------------------

        //Refresh the watchdog
        Micro__ServiceWatchdog();

        ram_test_data       = RUN_TIME_RAM_TEST_TRUE;

        // Check if ram pointer is not corrupted
        for(bytes_processed = 0U; bytes_processed < (uint32) MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST; bytes_processed++)
        {
            //Used to point the Ram location and its adjacent location  on which DC coupling RAM test is performed
            register uint8 *test_ptr;
            register uint8 *next_location_ptr;
            register uint8 *previous_location_ptr;

            (void) SRData__CheckLong((void *) &RAM_Test_Pointer,(void *) &N_RAM_Test_Pointer);

            //assign the ram location to be tested to the register variable
            test_ptr = RAM_Test_Pointer;
            next_location_ptr = test_ptr+1;
            previous_location_ptr = (test_ptr - 1U);

            //Check for the RAM boundaries
            if ( test_ptr == (void *)MICRO_RAM_START_ADDRESS)
            {
                next_location_ptr = (uint8 *)(test_ptr + 1U);
                previous_location_ptr = (uint8 *)(test_ptr + 2U);
            }

            if ( test_ptr == (void *)MICRO_RAM_END_ADDRESS)
            {
                next_location_ptr = (uint8 *)(test_ptr - 2U);
                previous_location_ptr = (uint8 *)(test_ptr - 1U);
            }

            // Check RAM boundaries for No Test area: end address looks like start of RAM area
            if (test_ptr == RAM_NOTEST_AREA_END_ADDRESS)
            {
                next_location_ptr = (uint8 *)(test_ptr + 1U);
                previous_location_ptr = (uint8 *)(test_ptr + 2U);
            }

            // Check RAM boundaries for No Test area: address before start address looks like end of RAM area
            if ( (test_ptr + 1) == RAM_NOTEST_AREA_START_ADDRESS)
            {
                next_location_ptr = (uint8 *)(test_ptr - 2U);
                previous_location_ptr = (uint8 *)(test_ptr - 1U);
            }

            //Disable the interrupt during test iteration
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            //store the data value into registers on which RAM test is done
            ram_test_data = (((uint8) *next_location_ptr) << 24U) | (((uint8) *test_ptr) << 16U)  | (((uint8) *previous_location_ptr) << 8U) | ((uint8) ram_test_data);

            //========================================================================
            // Actual engine of calculation

            //Write the test location with 0x55 value and the neighbors by 0
            *(test_ptr) = RAM_TEST_PATTERN;
            *(next_location_ptr) = RAM_TEST_BKD;
            *(previous_location_ptr) = RAM_TEST_BKD;

            //Check if the Ram test location is changed because of the neighborhood pattern
            if (*(test_ptr) != RAM_TEST_PATTERN)
            {
                ram_test_data  &= ~RUN_TIME_RAM_TEST_TRUE;
            }

            //change the neighborhood location value to 0xff and do not change the Ram test value
            *(next_location_ptr) = RAM_TEST_BKD_INV;
            *(previous_location_ptr) = RAM_TEST_BKD_INV;

            //Check if the Ram test location is changed because of the neighborhood pattern
            if (*(test_ptr) != RAM_TEST_PATTERN)
            {
                ram_test_data  &= ~RUN_TIME_RAM_TEST_TRUE;
            }

            //Write the test location with 0xAA value and the neighbors by 0
            *(test_ptr) = RAM_TEST_PATTERN_INV;
            *(next_location_ptr) = RAM_TEST_BKD;
            *(previous_location_ptr) = RAM_TEST_BKD;

            //Check if the Ram test location is changed because of the neighborhood pattern
            if (*(test_ptr) != RAM_TEST_PATTERN_INV)
            {
                ram_test_data  &= ~RUN_TIME_RAM_TEST_TRUE;
            }

            //change the neighborhood location value to 0xff and do not change the Ram test value
            *(next_location_ptr) = RAM_TEST_BKD_INV;
            *(previous_location_ptr) = RAM_TEST_BKD_INV;

            //Check if the Ram test location is changed because of the neighborhood pattern
            if (*(test_ptr) != RAM_TEST_PATTERN_INV)
            {
                ram_test_data  &= ~RUN_TIME_RAM_TEST_TRUE;
            }

            //Restore the value
            *next_location_ptr = (ram_test_data >> 24) & 0xFF;
            *previous_location_ptr = (ram_test_data >> 8) & 0xFF;
            *test_ptr = (ram_test_data >> 16) & 0xFF;;

            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

           //Is the RAM test reached the end of the RAM memory
            if ( test_ptr == (uint8 *)MICRO_RAM_END_ADDRESS)
            {
                test_ptr = (uint8 *)MICRO_RAM_START_ADDRESS;
            }
            else
            {
                //or else go to the next memory location
                test_ptr++;
            }
            // If the RAM No Test area is enabled, skip the section RAM_NOTEST_AREA
            if(test_ptr == RAM_NOTEST_AREA_START_ADDRESS)
            {
                test_ptr = RAM_NOTEST_AREA_END_ADDRESS;
            }
            //Restore the RAM test pointer
            SRData__UpdateLong((void *) &RAM_Test_Pointer,(void *) &N_RAM_Test_Pointer,(uint32) test_ptr);
        } //End of for loop for RAM_CONTINUOUS_TEST

        if ((ram_test_data & RUN_TIME_RAM_TEST_PROP_MASK) != RUN_TIME_RAM_TEST_TRUE)
        {
          //If the test fails throw test fail exception
         SRException__Queue(SREXCEPTION_MICRO_RAM_INTEGRITY_TEST,0,0);
        }

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : Stack Underflow and overflow test
        //
        //This is used to validate the Stack operation
        //-----------------------------------------------------------------------------------------------------------------

        #define SREXCEPTION_DATA1               ((uint32)1)     //1 represents that the exception is from Micro module
        #define SREXCEPTION_DATA0_OVERFLOW      ((uint32)0)     //0 represents that the exception is because of the stack overflow
        #define SREXCEPTION_DATA0_UNDERFLOW     ((uint32)1)     //1 represents that the exception is because of the stack underflow

        //check Stack underflow pattern
        if ((((uint32*)MICRO_STACK_TEST_END_ADDRESS)[0] != STACK_TEST_PATTERN1) ||
            (((uint32*)MICRO_STACK_TEST_END_ADDRESS)[1] != STACK_TEST_PATTERN2) ||
            (((uint32*)MICRO_STACK_TEST_END_ADDRESS)[2] != STACK_TEST_PATTERN3) ||
            (((uint32*)MICRO_STACK_TEST_END_ADDRESS)[3] != STACK_TEST_PATTERN4))
        {
            SRException__Queue(SREXCEPTION_STACK_OVERFLOW, SREXCEPTION_DATA1, SREXCEPTION_DATA0_OVERFLOW);
        }

        //check Stack overflow pattern
        if ((((uint32*)MICRO_STACK_TEST_START_ADDRESS)[0] != STACK_TEST_PATTERN5) ||
            (((uint32*)MICRO_STACK_TEST_START_ADDRESS)[1] != STACK_TEST_PATTERN6) ||
            (((uint32*)MICRO_STACK_TEST_START_ADDRESS)[2] != STACK_TEST_PATTERN7) ||
            (((uint32*)MICRO_STACK_TEST_START_ADDRESS)[3] != STACK_TEST_PATTERN8))
        {
            SRException__Queue(SREXCEPTION_STACK_OVERFLOW, SREXCEPTION_DATA1, SREXCEPTION_DATA0_UNDERFLOW);
        }

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_MICRO_SRRAMTEST_EXIT);
        #endif

    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs non-volatile memory (FLASH) test periodically.
 *    @details  This method is responsible for checking the FLASH memory consistency.
 *              Run time Flash Test which calculates CRC couple bytes each time and keeps on continuing until it
 *              reaches the FLASH memory end and compare the calculated CRC against a CRC value stored
 *              in FLASH memory.
 *
 *              INBUILT_FLASH_CRC_CALCULATOR macro defines the CRC calculation engine either Inbuilt or External.
 *
 *              MICRO_NUM_WORD_FOR_FLASH_CONTINUOUS_TEST configuration macro defined in the SRMicro_prv.h file
 *              Defines how many words will be processed each interaction of the Continuous Flash Test.
 *
 *              The time taken for complete FLASH test is calculated below-
 *              For the inbuilt CRC tool(IELF) -
 *              FULL FLASH TEST (in ms) =
 *                 (((FLASH Size in words)-4)/ MICRO_NUM_WORD_FOR_FLASH_CONTINUOUS_TEST) * Call rate(of Micro__FlashTest())
 *
 *              If the external CRC tool(IELF) is used-
 *              FULL FLASH TEST (in ms) =
 *                  (((FLASH Size in bytes)-4)/ MICRO_NUM_WORD_FOR_FLASH_CONTINUOUS_TEST) * Call rate(of Micro__FlashTest())
 *
 *              In the calculation FLASH Size-4 because the last 4- bytes of the flash memory is used to
 *              hold the calculated CRC value on which the Flash test is not performed.
 *
 *
 *              For example
 *              Flash size is 32768 in words,  number of bytes processed in each call are 4 and the call rate is 5ms
 *              then time taken by the inbuilt CRC calculator is-
 *              FULL FLASH TEST (in ms) =  ((32768-4) /4)  * 5 ms
 *                                      = 2047.75ms
 *
 *              Along with this it provides the flow control log and exception control for each tests.
 *              Note- Should be called at every 5ms or 25ms slot.
 */
#pragma optimize=none
void Micro__SRFlashTest(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        uint16 count;

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_MICRO_SRFLASHTEST_ENTER);
        #endif

        for (count=0; count< MICRO_NUM_WORD_FOR_FLASH_CONTINUOUS_TEST;count++)
        {
            //Calculate the CRC16 CCIT
            Micro_Flash_Crc = Crc16__ConstRangeProcess((uint8 *)Micro_Flash_Crc_Pt,Micro_Flash_Crc,4);

            Micro_Flash_Crc_Pt++;

            //If the test reached the end of memory
            if (Micro_Flash_Crc_Pt >= (uint32 *)MICRO_FLASH_END_ADDRESS)
            {
                //compare the calculated CRC against a CRC value stored in FLASH memory by the IELF tool.
                if ((uint16)MICRO_FLASH_CRC !=  Micro_Flash_Crc)
                {
                    //If the test fails throw test fail exception
                    SRException__Queue(SREXCEPTION_MICRO_FLASH_INTEGRITY_TEST, (uint32)Micro_Flash_Crc, (uint32)MICRO_FLASH_CRC);
                }

                Micro_Flash_Crc = MICRO_CRC_INITIAL;
                Micro_Flash_Crc_Pt = (uint32 *)MICRO_FLASH_START_ADDRESS;
            }
        }

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_MICRO_SRFLASHTEST_EXIT);
        #endif

    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method makes the controller to work at Normal speed or resumes the controller from Low Speed mode
 *    @details  This method provides a way to switch from slow to normal mode of operation
 */
void Micro__SetNormalSpeedMode(void)
{
    MicroInitClk(SYSTEM_CLOCK_NORMAL_SPEED);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method puts the micro in WAIT mode
 *    @details  The CPU clock is stopped, reducing the power consumption of the microcontroller.
 *              When an internal or external interrupt request occurs,
 *              the CPU wakes-up from Wait mode and resumes processing
 */
void Micro__GoSleep(void)
{
    Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);
    Micro__ForceReset(MICRO_RESET_MODE_WARM);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method refreshes the watchdog
 * @details This method should be called periodically to refresh the WWDG (window watchdog)
 *          and/or IWDG (Independent watchdog) depending on the configuration.
 *
 *          Can be used to resolve processor malfunctions due to hardware or software failures.
 *          Watchdog should be refreshed at every 5ms
 */
void Micro__ServiceWatchdog(void)
{
    #if (MICRO_WATCHDOG_FEATURE == ENABLED)
        SERVICE_WATCHDOG();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It reset the micro when a not handled interruption occurs
 *    @details  This method is used to reset the micro when an interrupt appears which was not handled
 */
INTERRUPT_DIRECTIVE void Micro__Trap(void)
{
    SYSTEM_RESET();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides information about the clocks
 *    @details  This method provides information about clocks such as
 *              System clock frequency (SYSCLK_Frequency),
 *              AHB Clock frequency (HCLK_Frequency),
 *              APB1 Clock frequency( PCLK1_Frequency),
 *              APB2 Clock frequency( PCLK2_Frequency),
 *              And HSE(High speed External) initialization status.
 *
 *    @return   MICRO_CLOCK_DEF* : pointer to the clock registers structure
 */
MICRO_CLOCK_DEF * Micro__GetClock(void)
{
    return (&Micro_ClocksStatus);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method adds the delay to the execution
 *    @details  Adds the delay by executing the NOP(No Operation)
 *    @param    delay : Defines the number times the NOP opcode is executed
 */
void Micro__DelayNumNops(uint16 delay)
{
    uint16 count;

    for(count =0; count < delay; count++)
    {
        NOP();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides a reason for last system reset.
 *    @details  This method is used to provides information about the source from where reset to system occurs
 *    @return   MICRO_LAST_RESET_TYPE* : pointer to system reset status register structure
 *                                      Reset by Debugger,
 *                                      Reset by Low Voltage Detector (LVD),
 *                                      Reset by Clock failure,
 *                                      Reset by EMC,
 *                                      Reset by illegal opcode execution,
 *                                      Reset by watchdog,
 *                                      Reset by External,
 *                                      And Power on reset.
 */
MICRO_LAST_RESET_TYPE * Micro__GetLastResetstatus(void)
{
    MicroValidateResetInfoDataCrc();
    return (&(Micro_ResetInfo.Reset));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API assert the reset
 *
 * @param reset_mode
 *
 */
void Micro__ForceReset(MICRO_RESET_MODE_TYPE reset_mode)
{
    if (reset_mode == MICRO_RESET_MODE_WARM)
    {
        //warm reset
        SYSTEM_RESET();
    }
    else
    {
        //Cold reset
        //Corrupt the signature
        Micro_ResetInfo.Header[0] = 0;
        Micro_ResetInfo_Footer[0] = 0;

        SYSTEM_RESET();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API reads the reset mode
 * @return MICRO_RESET_MODE_TYPE:
 *
 */
MICRO_RESET_MODE_TYPE Micro__GetResetMode(void)
{
    MicroValidateResetInfoDataCrc();
    return(Micro_ResetInfo.Mode);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to Enable the warm reset action if the next reset is warn reset type
 * @param warm_reset_action
 */
void Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_TYPE warm_reset_action)
{
    MicroSetRamSignature();
    MicroValidateResetInfoDataCrc();
    Micro_ResetInfo.Warm_Reset_Actions |= warm_reset_action;
    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to Disable the warm reset action if the next reset is warn reset type
 * @param warm_reset_action
 */
void Micro__DisableWarmResetAction(MICRO_WARM_RESET_ACTION_TYPE warm_reset_action)
{
    MicroSetRamSignature();
    MicroValidateResetInfoDataCrc();
    Micro_ResetInfo.Warm_Reset_Actions &= ~warm_reset_action;
    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to read the warm reset actions set
 * @return MICRO_WARM_RESET_ACTION_TYPE
 */
MICRO_WARM_RESET_ACTION_TYPE Micro__GetWarmResetAction(void)
{
    if ((MicroIsResetSignatureValid() == TRUE) &&
    	(Micro_ResetInfo.Crc8_Data_Integrator == Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE)))
    {
        return(Micro_ResetInfo.Warm_Reset_Actions);
    }
    else
    {
        return((MICRO_WARM_RESET_ACTION_TYPE)0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Returns the Warm Reset Count
 * @details - We keep the count on how many times the Warm reset type occurred in the micro ranging from 0 to 20.
 * 			  After 20th Warm reset type we are forcing the cold reset which does clears the RAM memory
 *
 * @return uint8 : Returns the Current Warm reset counts
 *  @retval
 */
uint8 Micro__GetWarmResetCount(void)
{
    if ((MicroIsResetSignatureValid() == TRUE) &&
        (Micro_ResetInfo.Crc8_Data_Integrator == Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE))) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        return(Micro_ResetInfo.Warm_Reset_Count);
    }
    else
    {
        return(0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Returns the Loop Reset Count
 * @details - We keep the count on how many times the Loop reset type occurred in the micro after a POR or a CRC check failure.
 *
 * @return uint8 : Returns the Current Loop reset counter
 *  @retval
 */
uint8 Micro__GetLoopResetCount(void)
{
    if (Micro_ResetInfo.Crc8_Data_Integrator == Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE)) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        return(Micro_ResetInfo.Loop_Reset_Count);
    }
    else
    {
        return(0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Reset the Loop Reset Count in a valid Micro_ResetInfo structure.
 *
 */
void Micro__ResetLoopResetCount(void)
{
    if ((Micro_ResetInfo.Loop_Reset_Count != 0) &&
        (Micro_ResetInfo.Crc8_Data_Integrator == Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE))) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        Micro_ResetInfo.Loop_Reset_Count = 0;
        MicroUpdateResetInfoDataCrc();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method returns the Flash size of microcontroller
 *    @return   uint16 : Returns the flash size
 */
uint32 Micro__GetFlashSize(void)
{
    return (MICRO_FLASH_SIZE());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method returns the RAM size of microcontroller.
 *    @return   uint32 : It returns the ram size.
 */
uint32 Micro__GetRamSize(void)
{
    return (MICRO_RAM_SIZE());
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method returns the revision of the microcontrollers core.
 *  @return     uint8 : Revision of the Microcontroller Core
 */
MICRO_ID_TYPE * Micro__GetCoreID(void)
{
    return (MICRO_CPU_ID());
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes system clock.
 *    @details  It uses the PLL as the system clock using HSI clock running the System clock at maximum of 64MHz
 *              This internal function is called from the Micro__Initialize() API.
 */
static void MicroInitClk(SYSTEM_CLOCK_SPEED speed)
{
    uint16 timeout = 0;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //Save and Disable the Interrupts
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    RCC.CFGR    = RCC_CFGR_INIT_CONFIG; //Switch System Clock to HSI default, Sets AHB and APB Prescaler to DIV 1
    RCC.CR      = RCC_CR_INIT_CONFIG;   //HSI ON by default, DISABLE PLL
    RCC.PLLCFGR = RCC_PLLCFGR_INIT_CONFIG;

    // Disable all interrupts
    RCC.CIER = 0x00000000u;
    RCC.CICR = RCC_CICR_CLEAR_ALL_INTERRUPTS;

    if (speed == SYSTEM_CLOCK_NORMAL_SPEED)
    {
        /**
         * PLL Configuration
         */
        //Turn off the PLL
        RCC.CR &= ((uint32)~RCC_CR_PLLON);

        //Latency 3 wait cycles for system clock <= 64MHz
        FLASH.ACR |=  FLASH_LATENCY_2;

        // Check that the new number of wait states is taken into account to access the Flash
        // memory by polling the FLASH_ACR register
        timeout = 0;
        while ((FLASH.ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_1)
        {
            timeout++;

            if (timeout >= FLASHLATENCY_TIMEOUT)
            {
                SYSTEM_RESET();
            }
        }


        /**
         *
         * fPLLIN = 16MHz
         * N = 64
         * M = 4
         *
         * fVCO = fPLLIN � (N / M)
         * fVCO = 16MHz x (64/4) = 256MHz
         *
         * fPLLP = fVCO / P
         * P = 4
         * fPLLP = 256MHz / 4 = 64MHz
         *
         *
         * fPLLQ = fVCO / Q
         * Q = 4
         * fPLLQ = 256MHz / 2 = 64MHz
         *
         *
         * fPLLR = fVCO / R
         * R = 4
         * fPLLR = 256MHz / 4 = 64MHz
         *
         */
        #define PLL_CONFIG_OUTPUT_64MHZ             RCC_PLLCFGR_PLLSRC_HSI      |   \
                                                    RCC_PLLCFGR_PLLREN          |   \
                                                    RCC_PLLM_DIV4               |   \
                                                    (64 << RCC_PLLCFGR_PLLN_POS)|   \
                                                    RCC_PLLP_DIV4               |   \
                                                    RCC_PLLQ_DIV4               |   \
                                                    RCC_PLLR_DIV4

        RCC.PLLCFGR = PLL_CONFIG_OUTPUT_64MHZ;

        //Turn the PLL on
        RCC.CR |= RCC_CR_PLLON;

        // Wait till PLL is ready
        timeout = 0;
        while((RCC.CR & RCC_CR_PLLRDY) == 0)
        {
            timeout++;

            if (timeout >= PLLSTARTUP_TIMEOUT)
            {
                SYSTEM_RESET();
            }
        }

        // Select PLL as system clock source
        RCC.CFGR |= (uint32)RCC_CFGR_SW_PLL;

        // Wait till PLL is used as system clock source
        timeout = 0;

        while ((RCC.CFGR & (uint32)RCC_CFGR_SWS) != (uint32)RCC_CFGR_SWS_PLL)
        {
            timeout++;

            if (timeout >= PLLSTARTUP_TIMEOUT)
            {
                SYSTEM_RESET();
            }
        }
    }
    else    //SYSTEM_CLOCK_LOW_SPEED_2MHZ
    {
        RCC.CR |= RCC_HSI_DIV8;
    }

    // This method read the current system clock configuration and update Micro_ClocksStatus
    MicroGetClocksStatus();

    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Update the clock registers with the current clock info.
 *    @details  Updates the Micro_ClocksStatus register.
 */
static void MicroGetClocksStatus(void)
{
    if (((RCC.CFGR & RCC_CFGR_SWS) == (uint32)RCC_CFGR_SWS_PLL) && ((RCC.PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSI))
    {
        //PLL is used as System Clock source and HSI16 is used as PLL clock source

        uint32 pllm = ((RCC.PLLCFGR & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_POS) + 1U ;
        uint32 pllvco = (HSI_CLOCK_FREQUENCY / pllm) * ((RCC.PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_POS) ;
        uint32 pllr = (((RCC.PLLCFGR & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_POS) + 1U);

        Micro_ClocksStatus.SYSCLK_Frequency = pllvco / pllr;
    }
    else if ((RCC.CFGR & RCC_CFGR_SWS) == (uint32)RCC_CFGR_SWS_HSI)
    {
        Micro_ClocksStatus.SYSCLK_Frequency = HSI_CLOCK_FREQUENCY / (1 << ((RCC.CR & RCC_CR_HSIDIV_MSK) >> RCC_CR_HSIDIV_POS));
    }
    else
    {
        Micro_ClocksStatus.SYSCLK_Frequency     = 0;
    }

    // Compute HCLK, PCLK clocks frequencies
    // Get HCLK prescaler

    uint8 AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    uint8 APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

    Micro_ClocksStatus.HCLK_Frequency = Micro_ClocksStatus.SYSCLK_Frequency;
    uint32 tmp = AHBPrescTable[((RCC.CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_POS)];
    Micro_ClocksStatus.HCLK_Frequency >>= tmp;

    // Get PCLK1 prescaler
    Micro_ClocksStatus.PCLK1_Frequency = Micro_ClocksStatus.HCLK_Frequency;
    tmp = APBPrescTable[((RCC.CFGR & RCC_CFGR_PPRE) >> RCC_CFGR_PPRE_POS)];
    Micro_ClocksStatus.PCLK1_Frequency >>= tmp;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function initializes the micro hardware
 */
void Micro__HardwareInit(void)
{
    uint8 index;

    SYSTICK.CTRL &= ~SYSTICK_CTRL_ENABLE_MSK;   //Disable Systick clock
    DISABLE_INTERRUPTS();

    //Reset all the peripheral modules
    RCC.AHBRSTR  = RCC_AHBRSTR_FORCE_RESET;              // Reset AHB Peripheral except Flash
    RCC.APBRSTR1 = RCC_APBRSTR1_FORCE_RESET;             // Reset APB1 Peripheral
    RCC.APBRSTR2 = RCC_APBRSTR2_FORCE_RESET;             // Reset APB2 Peripheral
    RCC.IOPRSTR  = RCC_IOPRSTR_FORCE_RESET;              // Reset I/O port

    RCC.AHBRSTR  = RCC_AHBRSTR_RELEASE_RESET;              // Disable Reseting AHB Peripheral
    RCC.APBRSTR1 = RCC_APBRSTR1_RELEASE_RESET;             // Disable Reseting APB1 Peripheral
    RCC.APBRSTR2 = RCC_APBRSTR2_RELEASE_RESET;             // Disable Reseting APB2 Peripheral
    RCC.IOPRSTR  = RCC_IOPRSTR_RELEASE_RESET;              // Disable Reseting I/O port

    //Disable all the peripherals
    RCC.AHBENR  = RCC_AHBPERIPH_CLK_DISABLE;               // Disable AHB Peripheral Clock except Flash
    RCC.APBENR1 = RCC_APBENR1_CLK_DISABLE;                 // Disable APB2 Peripheral Clock
    RCC.APBENR2 = RCC_APBENR2_CLK_DISABLE;                 // Disable APB1 Peripheral Clock
    RCC.IOPENR  = RCC_IOPENR_CLK_DISABLE;                  // Disable APB1 Peripheral Clock

    // Initialize the interrupt vectors
    NVIC.Disable[0] = NVIC_DISABLE_ALL_INTERRUPTS;
    NVIC.Clear[0]   = NVIC_CLEAR_ALL_INTERRUPTS;

    //Initializes all priorities to 0
    for(index = 0; index <= NVIC_INTERRUPT_PRIORITY_ARRAY; index++)
    {
        NVIC.Priority[index] = NVIC_INTERRUPT_PRIORITY_CLEAR;
    }

    //Set the Vector Table base address
    SCB.VTOR = INTERRUPT_TABLE_START_ADDRESS;


    // Disable all interrupts
    RCC.CICR = RCC_CICR_CLEAR_ALL_INTERRUPTS;

    /**
     * Disable CPU Prefetch
     * Enable CPU Instruction cache
     * Important - Enabling the pre fetch buffer throws an HARDFAULT reset. Confirmed with ST and the issue is still unknown for ST as well
     */
    FLASH.ACR = FLASH_ACR_ICEN | FLASH_ACR_DBG_SWEN | FLASH_LATENCY_2;

    #if (MICRO_DEBUG_MODE == ENABLED)
        //Enable the clock for Debug control module
        RCC.APBENR1 |= RCC_APBENR1_DBGEN;

        //This will keeps the debugger connected eventhough we execute the WFI command in OS__Idle()
        DBG.CR = DBG_CR_DBG_STOP_MSK | DBG_CR_DBG_STANDBY_MSK;

        // Configure the Debug register, it sets which peripheral will be stopped in the halt mode
        // Halts the TIM2,TIM3,TIM6,TIM14,WWDG,IWDG
        DBG.APBFZ1 = DBGMCU_APB1_FZ_STOP;

        // Halts the TIM1,TIM15,TIM16,TIM17
        DBG.APBFZ2 = DBGMCU_APB2_FZ__STOP;
    #endif

    //PWR block configuration
    //Voltage scaling range selection is set to RANGE 1
    PWR.CR1 = PWR_CR1_VOS_0;

    // Upon reset, the UCPD CCx lines present a pull-down resistor that can be disabled by
    // setting the UCPDx_STROBE bit of the SYSCFG_CFGR1 register
    SYSCFG.CFGR1 = SYSCFG_CFGR1_UCPD1_STROBE | SYSCFG_CFGR1_UCPD2_STROBE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function tests the watchdog during boot time
 */
#if (MICRO_SR_FEATURE_TEST == ENABLED)
#pragma optimize=none
static void MicroBootWatchdogTest(void)
{
    //-----------------------------------------------------------------------------------------------------------------
    // CLASSB Test : Watchdog Test
    //
    //Its mandatory to use the Independent watchdog for the CLASSB test
    //-----------------------------------------------------------------------------------------------------------------
    if (Micro_ResetInfo.Actual_Reset.WATCHDOG == FALSE)
    {
        while (1);
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function initializes the watchdog with the user defined timeout
 *
 * @details LSI_CLOCK_FREQUENCY is 32K. Using /32 as prescaler sets the clock to the IWDG at 1KHz = 1ms per cycle
 */
void Micro__WatchdogInit(void)
{
    #if (MICRO_WATCHDOG_FEATURE == ENABLED)
        uint16 timeout;

        timeout = 0;

        RCC.CSR |= (uint32)(RCC_CSR_LSION);                     // LSI on

        while((RCC.CSR & RCC_CSR_LSIRDY) == FALSE)              // Wait till LSI is ready
        {
            timeout++;

            if (timeout >= LSISTARTUP_TIMEOUT)
            {
                SYSTEM_RESET();
            }
        }

        //Configure IWDG
        IWDG.KR     = IWDG_WRITE_ACCESS_ENABLE;             // Enable write access to IWDG_PR and IWDG_RLR registers
        IWDG.PR     = IWDG_PR_PR_0 | IWDG_PR_PR_1;          // Set prescaler value as /32
        IWDG.RLR    = MICRO_WATCHDOG_TIMEOUT_IN_MS;         // Set IWDG Reload value
        IWDG.KR     = IWDG_RELOAD;                          // Reload IWDG counter
        IWDG.KR     = IWDG_ENABLE;                          // Enable IWDG (LSI automatically enabled by HW)
        IWDG.KR     = IWDG_WRITE_ACCESS_DISABLE;            // Disable write access to IWDG_PR and IWDG_RLR registers
        Micro__ServiceWatchdog();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -Reads the actual reset value from the micro and clears it
 * @return
 */
static MICRO_LAST_RESET_TYPE MicroReadActualReset(void)
{
    MICRO_LAST_RESET_TYPE reset;

    *((uint8 *)(&reset)) = 0;

    //Independent watchdog or Window watchdog reset flag
    if ((RCC.CSR & (RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF)) != FALSE)
    {
        reset.WATCHDOG = TRUE;
    }

    //Low-power reset flag
    if ((RCC.CSR & RCC_CSR_LPWRRSTF) != FALSE)
    {
        reset.LVD = TRUE;
    }

    //Software reset flag or Option byte loader reset flag
    if ((RCC.CSR & (RCC_CSR_SFTRSTF | RCC_CSR_OBLRSTF)) != FALSE)
    {
        // Software reset is reported as illegal opcode reset
        reset.ILLEGAL_OP = TRUE;
    }

    //POR/PDR reset flag
    if ((RCC.CSR & RCC_CSR_PWRRSTF) != FALSE)
    {
        reset.POR = TRUE;
    }

    // IWDG reset, WWDG reset, Power reset, Software reset and Low power management reset asserts 20us of low signal on the
    // External reset pin which makes external reset flag to set on all other type of resets.
    // Actual External reset can only be differentiated only if no other reset occurs other than external reset.
    if (((RCC.CSR & RCC_CSR_PINRSTF) != FALSE) &&
        (*((uint8 *)(&reset)) == 0))
    {
        reset.EXTERNAL = TRUE;
    }

    RCC.CSR |= RCC_CSR_RMVF;
    RCC.CSR &= (uint32)(~RCC_CSR_RMVF);

    return(reset);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * List all the generic static functions used
 */
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This is the routine where the application will end up if a test fails during the boot time and perform the system reset.
 *    @details  This routine been called if any one of the below boot test fails-
 *              CPU Register test,
 *              MARCH C Ram test,
 *              FLASH CRC test
 *
 */
#if (MICRO_SR_FEATURE_TEST == ENABLED)
static void MicroFailSafe(void)
{
    SYSTEM_RESET();
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function performs the flash boot test
 */
#if (MICRO_SR_FEATURE_TEST == ENABLED)
#pragma optimize=none
static void MicroBootFlashTest(void)
{
    //Maximum 12 variables can be assigned as register type. More than that could cause problem.
    //-----------------------------------------------------------------------------------------------------------------
    // CLASSB Test : Flash CRC test
    //
    //This test is used to validate the non-volatile memory consistency
    //-----------------------------------------------------------------------------------------------------------------
    Micro_Flash_Crc = MICRO_CRC_INITIAL;

    // for each word in the block
    for ( Micro_Flash_Crc_Pt = MICRO_FLASH_START_ADDRESS; Micro_Flash_Crc_Pt <= MICRO_FLASH_END_ADDRESS; Micro_Flash_Crc_Pt++ )
    {
        //Calculates the CRC over a range of memory
        Micro_Flash_Crc = Crc16__RangeProcess((uint8 *)Micro_Flash_Crc_Pt,Micro_Flash_Crc,4);

        //Refresh the independent watchdog
        SERVICE_WATCHDOG();
    }

    //compare the calculated CRC against a CRC value stored in FLASH memory by the IELF tool.
    if ((uint16)MICRO_FLASH_CRC !=  Micro_Flash_Crc)
    {
        MicroFailSafe();
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This method handles the reset and realizes which reset method to be apply
 */
static void MicroResetHandling(void)
{
    uint8 loop_reset_counter;
    BOOL_TYPE is_watchdog_test;
    BOOL_TYPE is_signature_valid;
    MICRO_RESET_INFO_HEADER_TYPE local_reset_info;

    loop_reset_counter = 0;
    is_watchdog_test = FALSE;
    is_signature_valid = MicroIsResetSignatureValid();

    // If CRC value is valid, keep the Cold Reset counter
    if (Micro_ResetInfo.Crc8_Data_Integrator == Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE)) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        loop_reset_counter = Micro_ResetInfo.Loop_Reset_Count;
    }
    else
    {
        is_signature_valid = FALSE;
    }

    //Is Signature not present
    if (is_signature_valid == FALSE)
    {
        MicroClearResetData();
    }

    //Validate and store the Micro_ResetInfo value into stack variable
    MicroValidateResetInfoDataCrc();
    memcpy(&local_reset_info, &Micro_ResetInfo, sizeof(Micro_ResetInfo));

    //Read the actual reset register content
    local_reset_info.Actual_Reset = MicroReadActualReset();

    //do not save the reset type if the previous reset was intentional. Intentional reset was done in watch dog testing
    if ((local_reset_info.Warm_Reset_Actions & MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST) != MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST)
    {
        local_reset_info.Reset = local_reset_info.Actual_Reset;
    }
    else
    {
        //clear the watch dog test
        local_reset_info.Warm_Reset_Actions &= ~MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST;
        is_watchdog_test = TRUE;
    }

    //What is the reset type?Cold or Warm
    if ((local_reset_info.Reset.POR          == 1) ||
        (local_reset_info.Reset.LVD          == 1) ||
        (is_signature_valid == FALSE) ||
        (is_watchdog_test == TRUE) ||
        (Micro_ResetInfo.Warm_Reset_Count >= MICRO_WARM_RESET_COUNTER_LIMIT))
    {
        local_reset_info.Mode = MICRO_RESET_MODE_COLD;
        local_reset_info.Warm_Reset_Count = 0;
        if ((local_reset_info.Reset.POR          == 1) ||
            (local_reset_info.Reset.LVD          == 1))
        {
            loop_reset_counter = 0;
        }
        else if((Micro_ResetInfo.Warm_Reset_Count < MICRO_WARM_RESET_COUNTER_LIMIT) &&
            (loop_reset_counter < 0xFF) &&
            (is_watchdog_test == FALSE))
        {
            loop_reset_counter++;
        }
        local_reset_info.Loop_Reset_Count = loop_reset_counter;
    }
    else
    {
        local_reset_info.Warm_Reset_Count++;
        local_reset_info.Mode = MICRO_RESET_MODE_WARM;
    }

    //Restore and update the Crc of the Micro_ResetInfo
    memcpy(&Micro_ResetInfo, &local_reset_info, sizeof(Micro_ResetInfo));
    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function clears the reset data
 */
static void MicroClearResetData(void)
{
    MicroSetRamSignature();

    (*(uint8 *)&(Micro_ResetInfo.Reset))        = 0;
    (*(uint8 *)&(Micro_ResetInfo.Actual_Reset)) = 0;
    Micro_ResetInfo.Mode                        = MICRO_RESET_MODE_COLD;
    Micro_ResetInfo.Warm_Reset_Actions          = (MICRO_WARM_RESET_ACTION_TYPE)0;
    Micro_ResetInfo.Loop_Reset_Count            = 0;
    Micro_ResetInfo.Warm_Reset_Count            = 0;

    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - sets the RAM signature
 */
static void MicroSetRamSignature(void)
{
    uint8 index;

    for (index = 0; index < MICRO_RESETINFO_STRING_SIZE; index++)
    {
        Micro_ResetInfo.Header[index] = MICRO_RESETINFO_HEADER[index];
        Micro_ResetInfo_Footer[index] = MICRO_RESETINFO_FOOTER[index];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function validates the reset signature
 * @return
 */
static BOOL_TYPE MicroIsResetSignatureValid(void)
{
    BOOL_TYPE sign_valid;
    uint8 index;

    sign_valid = TRUE;

    for (index = 0; index < MICRO_RESETINFO_STRING_SIZE; index++)
    {
        if ((Micro_ResetInfo.Header[index] != MICRO_RESETINFO_HEADER[index]) ||
            (Micro_ResetInfo_Footer[index] != MICRO_RESETINFO_FOOTER[index]))
        {
            sign_valid = FALSE;
            break;
        }
    }

    return(sign_valid);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Validates the Data integrity of the Micro_ResetInfo variable
 * @return
 */
static void MicroValidateResetInfoDataCrc(void)
{
    if (Micro_ResetInfo.Crc8_Data_Integrator != Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE)) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFF,0xFF);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Update the Data integrity of the Micro_ResetInfo variable
 * @return
 */
static void MicroUpdateResetInfoDataCrc(void)
{
    Micro_ResetInfo.Crc8_Data_Integrator = Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
}


