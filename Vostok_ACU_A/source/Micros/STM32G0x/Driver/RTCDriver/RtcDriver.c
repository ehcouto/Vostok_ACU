/**
 *  @file
 *
 *  @brief      Real Time Clock Driver for STM32F0x using Epoch method
 *              By default uses the LSI clock and Alarm feature
 *
 *  @details    This module depends on ExtInterrupt module for interrupt generation
 *
 *  
 *
 *  @copyright  Copyright 2013-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "RtcDriver.h"
#include "uc.h"
#include "Micro.h"
#include "RtcDriver_prv.h"
#include "ExtInterrupt.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#ifndef RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS
    #define RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS         0
#else
    #if (RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS > 60)
        #error : RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS shall be within 60 seconds
    #endif
#endif

#ifndef DEFAULT_CALIBRATION_VALUE
    #define DEFAULT_CALIBRATION_VALUE           65
#endif

//LSE clock value
#ifndef LSE_VALUE
    #define LSE_VALUE       32768
#endif

//Lsi clock is fixed to 32KHz
#define LSI_VALUE                   32000

#define RTC_CALIBRATION_CLEAR_MASK  0xFF80
#define RTC_CALIBRATION_LIMIT_MASK  0x007F


#ifndef RTC_INTERRUPT_PREEMPTION_PRIORITY
    #define RTC_INTERRUPT_PREEMPTION_PRIORITY   2
#elif (RTC_INTERRUPT_PREEMPTION_PRIORITY > 3)
    #error:"RTC_INTERRUPT_PREEMPTION_PRIORITY value should be 0 to 3, 0 is for high priority and 3 is for low priority"
#endif


#define EXTI_21_CONNECTED_TO_RTC_ALARM_EVENT    21

#define RTC_WRITE_PROTECTION_DISABLED()         { RTC.WPR = 0xCA; RTC.WPR = 0x53; }
#define RTC_WRITE_PROTECTION_ENABLED()          { RTC.WPR = 0xFF;}
#define RTC_INIT_MASK                           ((uint32)0xFFFFFFFF)

#define INITMODE_TIMEOUT                        ((uint32) 0x00004000)
#define SYNCHRO_TIMEOUT                         ((uint32) 0x00008000)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static PASS_FAIL_TYPE RtcEnterInitMode(void);
static void RtcExitInitMode(void);
static PASS_FAIL_TYPE RtcWaitForSynchro(void);
static void RtcSetTime(uint8 hours, uint8 minutes, uint8 seconds);
static uint8 RtcByteToBcd2(uint8 Value);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initializes the module RtcDriver and its variables including RTC Registers.
 *
 */
void RtcDriver__Initialize(void)
{
	volatile uint32 synchrocounter = 0;
	uint32 synchrostatus = 0x00;

	//Enable clock for the power
	RCC.APBENR1 |= RCC_APBENR1_PWREN;

	// Enable the Backup Domain Access
	PWR.CR1 |= PWR_CR1_DBP;

	//Enable the LSI clock
	RCC.CSR |= RCC_CSR_LSION;

	while ((RCC.CSR & RCC_CSR_LSIRDY) == FALSE);

	// LSI as RTC Clock
	RCC.BDCR |= RCC_BDCR_RTCSEL_LSI;

	// Enable the RTC Clock
	RCC.BDCR |= RCC_BDCR_RTCEN;

	RCC.APBENR1 |= RCC_APBENR1_RTCAPBEN;

	#if (RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS > 0)
		ExtInterrupt__Initialize();
		//EXTI line 17 is connected to the RTC Alarm event
		ExtInterrupt__Config(EXTINT_CH21, PORT_NULL, 0, 0, EXTINT_TRG_RISE);

		//Configure the RTC interrupt
		Micro__NVICEnableIRQ(RTC_TAMP_IRQ_CHANNEL, RTC_INTERRUPT_PREEMPTION_PRIORITY);
	#endif


	// Disable the write protection for RTC registers
	RTC_WRITE_PROTECTION_DISABLED();

	// Set Initialization mode
	if (RtcEnterInitMode() == FAIL)
	{

	}
	else
	{
		//LSI = 32000
		// ck_apre = 32000/(127+1) = 250Hz
		// ck_spre = 250Hz/(249+1) = 1Hz

		#define PREDIV_ASYNC_PREDIV     (0x007F0000)
		#define PREDIV_SYNC_PREDIV      (0x000000F9)

		RTC.PRER = (uint32)(PREDIV_SYNC_PREDIV);
		RTC.PRER |= (uint32)(PREDIV_ASYNC_PREDIV);

		// MSK4: Alarm A date mask, Date/day don’t care in Alarm A comparison,
		// MSK3: Alarm A hours mask, Hours don’t care in Alarm A comparison
		// MSK2: Alarm A minutes mask, Minutes don’t care in Alarm A comparison
		// No Masking for the Seconds
		//Set the seconds in BCD format
		RTC.ALRMAR = RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | (RtcByteToBcd2(RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS) & (RTC_ALRMAR_SU | RTC_ALRMAR_ST));

		/* Configure the Interrupts in the RTC_CR register */
		RTC.CR = (uint32)(RTC_CR_ALRAIE);


		//Enable the Alarm
		RTC.CR |= RTC_CR_ALRAE;

		//Clear the Alarm Flag
		RTC.SCR = (RTC_SCR_CITSF | RTC_SCR_CTSOVF | RTC_SCR_CTSF | RTC_SCR_CWUTF | RTC_SCR_CALRBF | RTC_SCR_CALRAF);

		//Clear the Timer
		RTC.TR = 0;

		/* Exit Initialization mode */
		RtcExitInitMode();

		/* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
		if ((RTC.CR & RTC_CR_BYPSHAD) == 0)
		{
			// Clear RSF flag
			RTC.ICSR &= (~(uint32)RTC_ICSR_RSF);

			// Wait the registers to be synchronized
			do
			{
				synchrostatus = RTC.ICSR & RTC_ICSR_RSF;
				synchrocounter++;
			} while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));
		}
	}

	// Enable the write protection for RTC registers
	RTC_WRITE_PROTECTION_ENABLED();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set the calibration value for the RTC.
 *
 * @param calib
 */
void RtcDriver__SetCalibration (uint32 calib)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the calibration value for the RTC.
 *
 * @return
 */
uint32 RtcDriver__GetCalibration(void)
{
    return (0);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Handler is used to read the Epoch count update by polling method
 *  @details   This function should be called periodically in a less than 1 sec slot
 *
 */
void RtcDriver__Handler(void)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Epoch_Time.
 *
 *  @return     uint32 Epoch_Time.
 */
uint32 RtcDriver__GetEpochTime(void)
{
    return (0);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set Epoch_Time to epoch_time. Also set RTC peripheral register values to epoch_time.
 *
 *  @details    Only set if valid from MINIMUM_START_DATE through MAXIMUM_START_DATE.
 *
 *  @param     uint32 epoch_time - seconds since 1/1/1970 00:00:00
 */
void RtcDriver__SetEpochTime(uint32 epoch_time)
{
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles the RTC interrupt. Interrupt generated based on the RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS
 *    @details  This needs to be configured in the InterruptVector_prm.h file
 *
 *              #define RTC_IRQ()                       RtcDriver__Interrupt()
 */
void RtcDriver__Interrupt(void)
{
    //Clear the Alarm Flag
    RTC.SCR = (RTC_SCR_CITSF | RTC_SCR_CTSOVF | RTC_SCR_CTSF | RTC_SCR_CWUTF | RTC_SCR_CALRBF | RTC_SCR_CALRAF);

    RtcSetTime(0,0,0);

    #if (RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS > 0)
        //Call-back function every time the RTC interrupt occurs
        RTCDRIVER_INTERRUPT_HANDLER();
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Enters the RTC Initialization mode.
  * @note   The RTC Initialization mode is write protected, use the
  *         RTC_WriteProtectionCmd(DISABLE) before calling this function.
  * @param  None
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: RTC is in Init mode
  *          - ERROR: RTC is not in Init mode
  */
PASS_FAIL_TYPE RtcEnterInitMode(void)
{
  volatile uint32 initcounter = 0x00;
  PASS_FAIL_TYPE status = FAIL;
  uint32 initstatus = 0x00;

  // Check if the Initialization mode is set
  if ((RTC.ICSR & RTC_ICSR_INITF) == (uint32)0)
  {
    // Set the Initialization mode
    RTC.ICSR = (uint32)RTC_INIT_MASK;

    // Wait till RTC is in INIT state and if Time out is reached exit
    do
    {
      initstatus = RTC.ICSR & RTC_ICSR_INITF;
      initcounter++;
    } while((initcounter != INITMODE_TIMEOUT) && (initstatus == 0x00));

    if ((RTC.ICSR & RTC_ICSR_INITF) != 0)
    {
        status = PASS;
    }
    else
    {
        status = FAIL;
    }
  }
  else
  {
      status = PASS;
  }

  return (status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Exits the RTC Initialization mode.
  * @note   When the initialization sequence is complete, the calendar restarts
  *         counting after 4 RTCCLK cycles.
  * @note   The RTC Initialization mode is write protected, use the
  *         RTC_WriteProtectionCmd(DISABLE) before calling this function.
  * @param  None
  * @retval None
  */
static void RtcExitInitMode(void)
{
  // Exit Initialization mode
  RTC.ICSR &= (uint32)~RTC_ICSR_INIT;
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Waits until the RTC Time and Date registers (RTC_TR and RTC_DR) are
  *         synchronized with RTC APB clock.
  * @note   The RTC Resynchronization mode is write protected, use the
  *         RTC_WriteProtectionCmd(DISABLE) before calling this function.
  * @note   To read the calendar through the shadow registers after Calendar
  *         initialization, calendar update or after wakeup from low power modes
  *         the software must first clear the RSF flag.
  *         The software must then wait until it is set again before reading
  *         the calendar, which means that the calendar registers have been
  *         correctly copied into the RTC_TR and RTC_DR shadow registers.
  * @param  None
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: RTC registers are synchronised
  *          - ERROR: RTC registers are not synchronised
  */
static PASS_FAIL_TYPE RtcWaitForSynchro(void)
{
  volatile uint32 synchrocounter = 0;
  PASS_FAIL_TYPE status = FAIL;
  uint32 synchrostatus = 0x00;

  if ((RTC.CR & RTC_CR_BYPSHAD) != 0)
  {
    // Bypass shadow mode
    status = PASS;
  }
  else
  {
    // Disable the write protection for RTC registers
    RTC_WRITE_PROTECTION_DISABLED();

    // Clear RSF flag
    RTC.ICSR &= (~(uint32)RTC_ICSR_RSF);

    // Wait the registers to be synchronized
    do
    {
      synchrostatus = RTC.ICSR & RTC_ICSR_RSF;
      synchrocounter++;
    } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));

    if ((RTC.ICSR & RTC_ICSR_RSF) != 0)
    {
      status = PASS;
    }
    else
    {
      status = FAIL;
    }

    // Enable the write protection for RTC registers
    RTC_WRITE_PROTECTION_ENABLED();
  }

  return (status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param hours
 * @param minutes
 * @param seconds
 */
static void RtcSetTime(uint8 hours, uint8 minutes, uint8 seconds)
{
    // Disable the write protection for RTC registers
    RTC_WRITE_PROTECTION_DISABLED();

    // Set Initialization mode
    if (RtcEnterInitMode() == FAIL)
    {
    }
    else
    {
        RTC.TR = 0;

        // Exit Initialization mode
        RtcExitInitMode();

        // If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
        if ((RTC.CR & RTC_CR_BYPSHAD) == 0)
        {
            RtcWaitForSynchro();
        }
    }

    // Enable the write protection for RTC registers
    RTC_WRITE_PROTECTION_ENABLED();
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted.
  * @retval Converted byte
  */
static uint8 RtcByteToBcd2(uint8 Value)
{
  uint8 bcdhigh = 0;

  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }

  return((uint8)(bcdhigh << 4) | Value);
}
