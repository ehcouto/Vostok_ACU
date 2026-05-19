/**
 *  @file
 *
 *  @brief      Public interface for the Magnetron Pilot.
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MAGNETRONPILOT_H
	#define MAGNETRONPILOT_H

#ifndef HBL_PILOT_NUM_MAGNETRON
    #define HBL_PILOT_NUM_MAGNETRON    0
#endif

#if (HBL_PILOT_NUM_MAGNETRON > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Function pointer register for pilot methods.
#define MAGNETRON_PILOT_REGISTER    MagnetronPilot__Initialize,           \
                                    MagnetronPilot__Allocate,             \
                                    MagnetronPilot__InitializeInstance,   \
                                    MagnetronPilot__Request,              \
                                    MagnetronPilot__Process,              \
                                    MagnetronPilot__GetRequested,         \
                                    MagnetronPilot__GetProcessed,         \
                                    NULL,                                 \
                                    MagnetronPilot__AsynchHandler,        \
                                    NULL,                                 \
                                    NULL,                                 \
                                    MagnetronPilot__GetStatus,            \
                                    4

//! Magnetron pilot request enumeration type definition.
typedef enum MAGNETRON_REQUEST_ENUM
{
    MAGNETRON_PILOT_REQUEST_OFF     = 0,
    MAGNETRON_PILOT_REQUEST_ON      = 1,
    MAGNETRON_PILOT_REQUEST_TURBO   = 2
} MAGNETRON_PILOT_REQUEST_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            MagnetronPilot__Initialize          (void);
PASS_FAIL_TYPE  MagnetronPilot__InitializeInstance  (HBL_LOAD_TYPE *load);
uint8           MagnetronPilot__Allocate            (void);
PASS_FAIL_TYPE  MagnetronPilot__Request             (HBL_LOAD_TYPE *load, void *data);
PASS_FAIL_TYPE  MagnetronPilot__Process             (HBL_LOAD_TYPE *load);
void *          MagnetronPilot__GetRequested        (HBL_LOAD_TYPE *load);
void *          MagnetronPilot__GetProcessed        (HBL_LOAD_TYPE *load);
void *          MagnetronPilot__GetStatus           (HBL_LOAD_TYPE *load);
void            MagnetronPilot__200usHandler        (void);
PASS_FAIL_TYPE  MagnetronPilot__AsynchHandler       (HBL_LOAD_TYPE * load);

#endif //(HBL_PILOT_NUM_MAGNETRON > 0)

#endif // MAGNETRONPILOT_H


