/**
 *  @file       
 *
 *  @brief      Public interface file for the PwmHiResPilot module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef PWMHIRES_PILOT_H
	#define PWMHIRES_PILOT_H

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_PWM_HI_RES_OUTPUT
    #define HBL_PILOT_NUM_PWM_HI_RES_OUTPUT 0
#endif

#if (HBL_PILOT_NUM_PWM_HI_RES_OUTPUT > 0)

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
 *      VIRTUALPIN_TYPE.device  = PWM_HI_RES_PILOT_EMULATE_PWM_PROCESS
 *      VIRTUALPIN_TYPE.param0  = PWM_HI_RES_PILOT_EMULATE_PWM_PROCESS
 */
#define PWM_HI_RES_PILOT_EMULATE_PWM_PROCESS   0xFFU


#define PWMHIRESPILOT_REGISTER \
    /* HBL__PILOTINITIALIZE Initialize                  */  PwmHiResPilot__Initialize, \
    /* HBL__PILOTALLOCATE Allocate                      */  PwmHiResPilot__Allocate, \
    /* HBL__PILOTINITIALIZEINSTANCE InitializeInstance  */  PwmHiResPilot__InitializeInstance, \
    /* HBL__PILOTREQUEST Request                        */  PwmHiResPilot__Request, \
    /* HBL__PILOTPROCESS Process                        */  PwmHiResPilot__Process, \
    /* HBL__PILOTGET GetRequested                       */  PwmHiResPilot__GetRequested, \
    /* HBL__PILOTGET GetProcessed                       */  PwmHiResPilot__GetProcessed, \
    /* HBL__PILOTZCPROCESS ZcProcess                    */  NULL, \
    /* HBL__PILOTASYNCPROCESS AsynchProcess             */  NULL, \
    /* HBL__PILOTOCPROCESS OCProcess                    */  NULL, \
    /* HBL__PILOT1MS OneMsHandler                       */  PwmHiResPilot__OneMsHandler, \
    /* HBL__PILOTGETSTATUS GetStatus                    */  PwmHiResPilot__GetStatus, \
    /* uint8 Parameter_Size                             */  sizeof(uint16)


PACKED typedef struct
{
	HBL_LOAD_STATUS_TYPE Hbl_Status;
	uint16 Current;				//!< When driving Duty this holds the value to be set to the PWM.
} PWMHIRESPILOT_STATUS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void 			PwmHiResPilot__Initialize			(void);
uint8 			PwmHiResPilot__Allocate              (void);
PASS_FAIL_TYPE 	PwmHiResPilot__InitializeInstance    (HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE 	PwmHiResPilot__Request               (HBL_LOAD_TYPE * load, void * data);
PASS_FAIL_TYPE 	PwmHiResPilot__Process               (HBL_LOAD_TYPE * load);
void * 			PwmHiResPilot__GetRequested          (HBL_LOAD_TYPE * load);
void * 			PwmHiResPilot__GetProcessed          (HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE 	PwmHiResPilot__OneMsHandler          (HBL_LOAD_TYPE * load);
void * 			PwmHiResPilot__GetStatus             (HBL_LOAD_TYPE * load);

#endif // HBL_PILOT_NUM_PWM_HI_RES_OUTPUT

#endif // PWMHIRES_PILOT_H


