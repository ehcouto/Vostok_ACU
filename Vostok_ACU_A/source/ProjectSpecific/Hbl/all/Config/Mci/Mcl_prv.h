/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_PRV_H_
#define MCL_PRV_H_

#include "Mci_prm.h"
#include "MclParams_prm.h"
#include "IIRFilter.h"
#include "MathCalc.h"
#include "MclConfig.h"
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

//! MCI error definitions
typedef enum
{
    NO_FAILURE = 0,
    UNABLE_TO_DETECT_FIRST_ROTATION,            //!< After applying first and second and again fist measurement pulses we
                                                //!< was not able to detect rotor polarity. Detected based on peak current comparison.
    UNEXPECTED_MOTOR_STOP,                      //!< Motor is stopped while it is supposed to be running.
                                                //!< Detect based on direct bemf signal check.
    UNABLE_TO_CLOSE_LOOP,                       //!< We are not able to switch from open to close loop.
                                                //!< Detected based on series of consequent correct triac triggers.
    UNDER_CURRENT,                              //!< Motor current is below minimum limit.
    OVER_CURRENT,                               //!< Motor current is above maximum limit.

	// << Recovery Routine >>
	UNDER_VOLTAGE,                              //!< Line voltage is below minimum limit.
    UNDER_VOLTAGE_SHORT_CIRCUIT,                //!< Line voltage is below minimum limit in short mode.
    ZERO_CROSS_NOT_DETECTED,                    //!< Zero cross was not detected and timer is overflow
    POWER_DROP                                  //!< Power drop detected
} MCI_ERROR_TYPE;


// Acceletation for instant motor off
#define MCI_ACC_EXECUTE_IMMEDIATELY     32767


//  ---- Compilation switches -----------------------------------------------------------------------------------------
//! Enables Mci debug
#define MCISLFAC_DEBUG                              DISABLED    // ENABLED/DISABLED
//! Enables Mci debug
#define MCISLFAC_ADC_ERROR_SIM                      DISABLED    // ENABLED/DISABLED
//! Enables Vwi debug
#define MCISLFAC_VWI_DEBUG                          ENABLED     // ENABLED/DISABLED


// --- Hardware pin configuration
//! Gpio pin for triac triggering supply AC voltage to the motor winding
#define MOTOR_GPIO_PORT                             (PORTF)
#define MOTOR_GPIO_PIN                              (0U)
#define MOTOR_TRIAC_ON_STATE                        (TRUE)

//! Voltage zerocross input capture channel
#define AC_ZC_SIGNAL_CHANNEL                        (IC03)
#define TRIAC_FIRE_CHANNEL                          (IC00)
#define READ_TIMER_AND_RESET                        (IC15)


// features
//! Fast handler call freq
#ifdef COMPILE_4_SIMULINK
	#define MCISLFAC_FAST_HANDLER_FREQUENCY_HZ          (5000U) //Used for SIL
#endif
//! HW timer frequency
#define MCISLFAC_TIMER_FREQ_HZ                      (6400000U)
//! Converts frequency in Hz to HW timer ticks
#define CONVERT_FREQ_TO_TIMER_COUNT(x)              (uint16)((MCISLFAC_TIMER_FREQ_HZ/2)/(x))
//! HW Timer ticks value for semi period at 60Hz
#define MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ   (CONVERT_FREQ_TO_TIMER_COUNT(60))
//! Time from previous event when next C03 will not be generated
#define MCISLFAC_KEEP_OUT_WINDOW                    (uint16)(0.00625 * (float)MCISLFAC_TIMER_FREQ_HZ)
//! Converts seconds time main timer ticks
#define MCISLFAC_SECONDS_TO_TIMER_TICK(t, f)           (uint16)((t) * f)
//! Converts mili seconds time main timer ticks
#define MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(t, f)      (uint16)((((t) * f) + 500) / 1000)
//! Converts micro seconds time main timer ticks
#define MCISLFAC_MICRO_SECONDS_TO_TIMER_TICK(t, f)     (uint16)((((t) * f) + 500000) / 1000000)
//! Converts percent of semi period at 60Hz to HW timer ticks
#define MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(p)     (uint16)((((p) * MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ) + 50) / 100)
//! Converts fraction percent of semi period at 60Hz to HW timer ticks. 1 is 0.01%.
#define MCISLFAC_FRAQ_PERCENT_OF_SEMI_PERIOD_60HZ(p) (uint16)((((p) * MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ) + 5000) / 10000)
//! Converts Adc value to observer format
#define CONVERT_TO_OBSERVER_FORMAT(x)               (uint16)(x + 32000)
//! Converts from HW timer tick to PI format
#define CONVERT_TO_PI_FORMAT(x)                     (sint16)((((uint32)(x) * (uint32)(1<<15)) + (uint32)(MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ/2)) / (uint32)MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ)
//! Converts from PI format to HW timer tick
#define CONVERT_FROM_PI_FORMAT(x)                   (uint16)((((uint32)(x) * (uint32)MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ) + (uint32)(1<<14)) / ((uint16)(1<<15)))

//! Fixed point arithmetic divider for 3F16
#define DIV_3F16                                    (65536)
//! Fixed point arithmetic divider for 6F13
#define DIV_6F13                                    (8192)
//! Fixed point arithmetic divider for 16F16
#define DIV_16F16                                   (65536)
//! Converts float to fixed point 3F16
#define FLOAT_TO_3F16(flp)                          (uint32)(((float)flp * (float)DIV_3F16) + 0.5)
//! Converts float to fixed point 6F13
#define FLOAT_TO_6F13(flp)                          (uint32)(((float)flp * (float)DIV_6F13) + 0.5)
//! Converts float to fixed point 16F16
#define FLOAT_TO_16F16(flp)                         (uint32)(((float)flp * (float)DIV_16F16) + 0.5)

//! Peak detect redefinition
#define PEAK_MCI_VOLT_CHANNEL                       PEAK_MCI_CHANNEL_VOLT
#define PEAK_MCI_CURR_CHANNEL                       PEAK_MCI_CHANNEL_CURR

#define WAIT_TIME_STOP_CHECK_MS                     1000   //! Wait time at stop check to allow the rotor stops completely. MCI will not allow motor starts within this time.

#define MCL__ZEROCROSS_HANDLER_BEGIN_CALLBACK()     //TimerInterruptExt__ResetPerInt();

#define STOP_CHECK_EXIT_THRESHOLD                   60  //!> Threshold to leave the stop check routine
#endif // MCL_PRV_H_
