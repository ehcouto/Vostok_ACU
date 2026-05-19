/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      Contains configurable macros for Micro GDM.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef MICRO_PRM_H
	#define MICRO_PRM_H
#include "C_Extensions.h"
#include "uc.h"

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  -  Defines the Enabling or disabling of the Independent watchdog.
 *  @details-  This macro should be Enabled for the production code, can be disabled only for debugging purpose
 *             The Independent watchdog is used in watchdog test in CLASSB software.
 *
 *             Note -The Independent watchdog feature should be enabled if the SRMICRO_WATCHDOG_BOOT_TEST is enabled
 *
 *  @param  - ENABLED(1)    - Which Enables the Independent watchdog
 *            DISABLED(0)   – Which Disables the Independent watchdog
 *
 *  @note    -  "#define MICRO_WATCHDOG_FEATURE    ENABLED"
 *              "#define MICRO_WATCHDOG_FEATURE    DISABLED"
 */
#define MICRO_WATCHDOG_FEATURE              ENABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  - Watchdog timeout value in milli seconds
 *  @details-
 *
 *  @param  - 100-1000
 *
 *  @note    -  "#define MICRO_WATCHDOG_TIMEOUT_IN_MS    500"    WATCHDOG will reset after 500ms
 */
#define MICRO_WATCHDOG_TIMEOUT_IN_MS                500

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  - This macro enables or disables the low Voltage monitor feature.
 *  @details- Its interrupt driven
 *            If MICRO_LOW_VOLTAGE_MONITOR_FEATURE is enabled then user should-
 *            assign the user application function which will be called when the low voltage is detected as follows
 *            in the SRInterrupt_prm.h file-
 *            #define PVD_IRQ()                       APplLvdService()
 *
 *            The moment the low voltage is detected the APplLvdService()(application function) is called and
 *            here the application can decide what action to be taken for the low voltage
 *
 *  @param  - ENABLED(1)   – Enables the low voltage monitor feature
 *            DISABLED(0)  – Disables the low voltage monitor feature
 *
 *  @note    - This enables the low voltage monitor feature
 *             "#define MICRO_LOW_VOLTAGE_MONITOR_FEATURE     ENABLED"
 *
 *             This disables the low voltage monitor feature
 *             "#define MICRO_LOW_VOLTAGE_MONITOR_FEATURE     DISABLED"
 */
#define MICRO_LOW_VOLTAGE_MONITOR_FEATURE                   DISABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief    This macro defines the voltage threshold detected by the power voltage detector.
 *  @details
 *
 *  @param  - 0/1/2/3/4/5/6
 *<pre>
 *      Below table explains Power Voltage detector falling threshold selection
 *       -----------------------------------
 *      | MICRO_LOW_VOLTAGE_LEVEL | Voltage level |
 *      |-------------------|---------------|
 *      |       0           |    2.0V       |
 *      |-------------------|---------------|
 *      |       1           |    2.2V       |
 *      |-------------------|---------------|
 *      |       2           |    2.4V       |
 *      |-------------------|---------------|
 *      |       3           |    2.5V       |
 *      |-------------------|---------------|
 *      |       4           |    2.6V       |
 *      |-------------------|---------------|
 *      |       5           |    2.8V       |
 *      |-------------------|---------------|
 *      |       6           |    2.9V       |
 *       ----------------------------------- </pre>
 *
 *  @note    - "#define MICRO_LOW_VOLTAGE_LEVEL     0"  Threshold Voltage level detected is 2.0V
 *             "#define MICRO_LOW_VOLTAGE_LEVEL     3"  Threshold Voltage level detected is 2.5V
 */
#define MICRO_LOW_VOLTAGE_LEVEL        3

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  This enables the SRPowerMgr module features
 *    @details -
 *
 *  @param  - ENABLED(1)   – Enables the SRPowerMgr initialization
 *            DISABLED(0)  – Disables the SRPowerMgr features
 *
 */
#define MICRO_SRPOWERMGR_FEATURE                        DISABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  This disables clock to Real Time Clock
 *    @details -
 *
 *
 */
#define MICRO_DISABLE_RTC_CLOCK()         { PWR.CR1 |= PWR_CR1_DBP; RCC.BDCR &= ~RCC_BDCR_RTCEN; }

#endif
