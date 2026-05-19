/**
 *  @file       TriacMci.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SOURCE_CATEGORY_HBL_HBLDISH_ACULOADHANDLER_TRIACMCI_TRIACMCI_H_
#define SOURCE_CATEGORY_HBL_HBLDISH_ACULOADHANDLER_TRIACMCI_TRIACMCI_H_

#include "SystemConfig.h"

#ifndef HBL_LOAD_NUM_TRIACMCI
    #define HBL_LOAD_NUM_TRIACMCI 0
#endif

#if (HBL_LOAD_NUM_TRIACMCI > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct {
    uint8                               Status;
} TRIACMCI_STATUS_TYPE;

#define TRIACMCI_REGISTER                   TriacMci__Initialize,            \
                                            TriacMci__Allocate,              \
                                            TriacMci__InitializeInstance,    \
                                            TriacMci__Request,               \
                                            TriacMci__Process,               \
                                            TriacMci__GetRequested,          \
                                            TriacMci__GetProcessed,          \
                                            TriacMci__AsyncProcess,          \
                                            TriacMci__GetStatus,             \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TriacMci__Initialize(void);
PASS_FAIL_TYPE TriacMci__InitializeInstance(HBL_LOAD_TYPE * load);
uint8 TriacMci__Allocate(void);
PASS_FAIL_TYPE TriacMci__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE TriacMci__Process(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * TriacMci__GetRequested(HBL_LOAD_TYPE * load);
void * TriacMci__GetProcessed(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
PASS_FAIL_TYPE TriacMci__AsyncProcess(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * TriacMci__GetStatus(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);

#endif

#endif // SOURCE_CATEGORY_HBL_HBLDISH_ACULOADHANDLER_TRIACMCI_TRIACMCI_H_


