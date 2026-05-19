/**
 *  @file       
 *
 *  @brief      Public interface to ExpansionPilot module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef EXPANSIONPILOT_H_
#define EXPANSIONPILOT_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_EXPANSION
    #define HBL_PILOT_NUM_EXPANSION 0
#endif

#if (HBL_PILOT_NUM_EXPANSION > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Function pointer definition for pilot methods.
#define EXPANSION_PILOT_REGISTER            ExpansionPilot__Initialize,          \
                                            ExpansionPilot__Allocate,            \
                                            ExpansionPilot__InitializeInstance,  \
                                            ExpansionPilot__Request,             \
                                            ExpansionPilot__Process,             \
                                            ExpansionPilot__GetRequested,        \
                                            ExpansionPilot__GetProcessed,        \
                                            NULL,                                \
                                            NULL,                                \
                                            NULL,                                \
                                            NULL,                                \
                                            ExpansionPilot__GetStatus,           \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            ExpansionPilot__Initialize           (void);
uint8           ExpansionPilot__Allocate             (void);
PASS_FAIL_TYPE  ExpansionPilot__InitializeInstance   (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  ExpansionPilot__Request              (HBL_LOAD_TYPE *load,void *data);
PASS_FAIL_TYPE  ExpansionPilot__Process              (HBL_LOAD_TYPE *load);
void *          ExpansionPilot__GetRequested         (HBL_LOAD_TYPE *load);
void *          ExpansionPilot__GetProcessed         (HBL_LOAD_TYPE *load);
void *          ExpansionPilot__GetStatus            (HBL_LOAD_TYPE *load);

#endif

#endif /* EXPANSIONPILOT_H_ */
