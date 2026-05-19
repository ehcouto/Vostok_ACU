/**
 * @file
 * @brief       Public interface to the code generated OWIVoltageRegulatedOutput module.
 *
 * @details     Refer to OWIVoltageRegulatedOutput.c source file for more detailed information.
 *
 * $Header: $
 *
 * @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef OWI_VOLTAGE_REGULATED_OUTPUT_H_
#define OWI_VOLTAGE_REGULATED_OUTPUT_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

#include "SystemConfig.h"

// -- This Module --

// -- Other Modules --



//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --
#define VRO_EXECUTION_TIME 		100  	//!< Execution time definition for the algorithm. Used in the delay timer in state machine
#define VRO_DELAY_TIME          2000    //!< Delay in ms after binary search is over
#define VRO_READ_TIME_BASE_MULT 10      //!< Multiplicator for read time
#define VRO_VOLT_SP_BASE_MULT	10		//!< Multiplicator for SetPoint
#define VRO_VOLT_TOL_BASE_MULT  100     //!< Multiplicator for voltage tolerance when converting from uint8 to float32
#define VRO_VOLT_BASE_MULT 		1000	//!< Multiplicator for voltage reading when converting from uint8 to float32
#define VRO_COMP_EXP_BASE_MULT	1000	//!< Multiplicator for compensation exponential
#define VRO_AVG_COEFF_BASE_MULT	1000	//!< Multiplicator for average coefficient
#define VRO_MAX_DC_HI_RES 		10000	//!< Maximum DutyCycle when OWI Pilot is High Resolution PWM
#define VRO_MAX_DC_PWM 			32768	//!< Maximum DutyCycle when OWI Pilot is Nucleus PWM


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void OWIVoltageRegulatedOutput__Initialize(void);
void OWIVoltageRegulatedOutput__Execution(void);
uint8 OWIVoltageRegulatedOutput__GetSequenceId(void);
void OWIVoltageRegulatedOutput__RequestReset(uint8 setpoint_index);
void OWIVoltageRegulatedOutput__RequestStop(void);
void OWIVoltageRegulatedOutput__RequestKeepRunning(void);
void OWIVoltageRegulatedOutput__RequestResetIfDisabled(void);
void OWIVoltageRegulatedOutput__ResetSetpoint(void);

#endif      /* OWI_VOLTAGE_REGULATED_OUTPUT_H_ */
