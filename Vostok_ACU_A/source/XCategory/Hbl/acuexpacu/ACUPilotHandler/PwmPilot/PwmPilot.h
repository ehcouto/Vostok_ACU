/**
 *  @file       
 *
 *  @brief      Public interface file for the PwmPilot module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef PWM_PILOT_H
	#define PWM_PILOT_H

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_PWM_OUTPUT
    #define HBL_PILOT_NUM_PWM_OUTPUT 0
#endif

#if (HBL_PILOT_NUM_PWM_OUTPUT > 0)

#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/*
 *  When a timer from the Timer module is not available, the PwmPilot module is capable of emulating a pwm signal by
 *  bit banging the i/o pin.
 *
 *  In the VirtualPin_prv.h, the following shall be configured to enable the emulate pwm signal.
 *
 *      VIRTUALPIN_TYPE.device  = PWM_PILOT_EMULATE_PWM_PROCESS
 *      VIRTUALPIN_TYPE.param0  = PWM_PILOT_EMULATE_PWM_PROCESS
 */
#define PWM_PILOT_EMULATE_PWM_PROCESS   0xFFU


#define PWMPILOT_REGISTER \
    /* HBL__PILOTINITIALIZE Initialize                  */  PwmPilot__Initialize, \
    /* HBL__PILOTALLOCATE Allocate                      */  PwmPilot__Allocate, \
    /* HBL__PILOTINITIALIZEINSTANCE InitializeInstance  */  PwmPilot__InitializeInstance, \
    /* HBL__PILOTREQUEST Request                        */  PwmPilot__Request, \
    /* HBL__PILOTPROCESS Process                        */  PwmPilot__Process, \
    /* HBL__PILOTGET GetRequested                       */  PwmPilot__GetRequested, \
    /* HBL__PILOTGET GetProcessed                       */  PwmPilot__GetProcessed, \
    /* HBL__PILOTZCPROCESS ZcProcess                    */  NULL, \
    /* HBL__PILOTASYNCPROCESS AsynchProcess             */  NULL, \
    /* HBL__PILOTOCPROCESS OCProcess                    */  NULL, \
    /* HBL__PILOT1MS OneMsHandler                       */  PwmPilot__OneMsHandler, \
    /* HBL__PILOTGETSTATUS GetStatus                    */  PwmPilot__GetStatus, \
    /* uint8 Parameter_Size                             */  sizeof(uint8)

PACKED typedef struct
{
	HBL_LOAD_STATUS_TYPE Hbl_Status;
	uint8 Current;				//!< When driving Duty this holds the value to be set to the PWM.
} PWMPILOT_STATUS_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void 			PwmPilot__Initialize			(void);
uint8 			PwmPilot__Allocate              (void);
PASS_FAIL_TYPE 	PwmPilot__InitializeInstance    (HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE 	PwmPilot__Request               (HBL_LOAD_TYPE * load, void * data);
PASS_FAIL_TYPE 	PwmPilot__Process               (HBL_LOAD_TYPE * load);
void * 			PwmPilot__GetRequested          (HBL_LOAD_TYPE * load);
void * 			PwmPilot__GetProcessed          (HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE 	PwmPilot__OneMsHandler          (HBL_LOAD_TYPE * load);
void * 			PwmPilot__GetStatus             (HBL_LOAD_TYPE * load);

#endif // HBL_PILOT_NUM_PWM_OUTPUT

#endif // PWM_PILOT_H


