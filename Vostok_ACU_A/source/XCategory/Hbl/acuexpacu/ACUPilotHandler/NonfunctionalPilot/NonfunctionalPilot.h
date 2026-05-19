/**
 *  @file
 *
 *  @brief      Public interface for NonfunctionalPilot module.
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef NONFUNCTIONALPILOT_H_
#define NONFUNCTIONALPILOT_H_

#ifndef HBL_PILOT_NUM_NONFUNCTIONAL
    #define HBL_PILOT_NUM_NONFUNCTIONAL 0
#endif

#if (HBL_PILOT_NUM_NONFUNCTIONAL > 0)
//  --- Include Files -------------------------------------------------------------------------------------------------
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! The Pilot function register for HBL use.
#define NONFUNCTIONAL_PILOT_REGISTER    NonfunctionalPilot__Initialize,           \
                                        NonfunctionalPilot__Allocate,             \
                                        NonfunctionalPilot__InitializeInstance,   \
                                        NonfunctionalPilot__Request,              \
                                        NonfunctionalPilot__Process,              \
                                        NonfunctionalPilot__GetRequested,         \
                                        NonfunctionalPilot__GetProcessed,         \
                                        NULL, /* ZcProcess */                     \
                                        NULL, /* AsynchProcess */                 \
                                        NULL, /* OCProcess */                     \
                                        NULL, /* OneMsHandler */                  \
                                        NULL, /* GetStatus */                     \
                                        1     /* Parameter Size */

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            NonfunctionalPilot__Initialize            (void);
uint8           NonfunctionalPilot__Allocate              (void);
PASS_FAIL_TYPE  NonfunctionalPilot__InitializeInstance    (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  NonfunctionalPilot__Request               (HBL_LOAD_TYPE *load, void *data);
PASS_FAIL_TYPE  NonfunctionalPilot__Process               (HBL_LOAD_TYPE *load);
void *          NonfunctionalPilot__GetRequested          (HBL_LOAD_TYPE *load);
void *          NonfunctionalPilot__GetProcessed          (HBL_LOAD_TYPE *load);

#endif  // (HBL_PILOT_NUM_NONFUNCTIONAL > 0)
#endif  // NONFUNCTIONALPILOT_H_
