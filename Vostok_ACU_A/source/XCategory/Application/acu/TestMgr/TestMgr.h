/**
 *  @file
 *
 *  @brief      Public Interface for Test Manager module.
 *
 *  @copyright  Copyright 2015-2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TESTMGR_H_
#define TESTMGR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Test Manager Cycle API enumeration.
//! Note: Enumeration is used to look up test cycle name ID.
typedef enum
{
    TESTMGR_CYCLE_EOL_0 = 0,
    TESTMGR_CYCLE_EOL_1,
    TESTMGR_CYCLE_EOL_2,
    TESTMGR_CYCLE_SERVICE_0,
    TESTMGR_CYCLE_SERVICE_1,
    TESTMGR_DISENGAGE   = 240,  // Disengage any active cycle in compartment 0 including EOL and Service.
    TESTMGR_CYCLE_NONE  = 255   // Do not engage any special cycle. Allows Supervisor to see change in regulation.
                                // HMI should set back to TESTMGR_CYCLE_NONE after test cycle is engaged/disengaged.
} TESTMGR_CYCLE_TYPE;

//! Test Manager Phase enumeration. Used to set phase and/or step from external control (PC).
typedef enum
{
    TESTMGR_PHASE_0 = 0,
    TESTMGR_PHASE_1,
    TESTMGR_PHASE_2,
    TESTMGR_PHASE_3,
    TESTMGR_PHASE_4,
    TESTMGR_PHASE_5,
    TESTMGR_PHASE_6,
    TESTMGR_PHASE_7,
    TESTMGR_PHASE_8,
    TESTMGR_PHASE_9,
    TESTMGR_PHASE_10,
    TESTMGR_PHASE_11,
    TESTMGR_PHASE_12,
    TESTMGR_PHASE_13,
    TESTMGR_PHASE_14,
    TESTMGR_PHASE_15,
    TESTMGR_PHASE_16,
    TESTMGR_PHASE_17,
    TESTMGR_PHASE_18,
    TESTMGR_PHASE_19,
    TESTMGR_PHASE_20,
    // 21 to 128...

    TESTMGR_PHASE_NONE    = 0xFD,
    TESTMGR_PHASE_REGRESS = 0xFE,
    TESTMGR_PHASE_ADVANCE = 0xFF
} TESTMGR_PHASE_TYPE;



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TestMgr__Initialize(void);
void TestMgr__Handler(void);
PASS_FAIL_TYPE TestMgr__EngageTest(uint8 compartment, TESTMGR_CYCLE_TYPE test);
PASS_FAIL_TYPE TestMgr__SetPhase(uint8 compartment, TESTMGR_PHASE_TYPE phase);
PASS_FAIL_TYPE TestMgr__SetStep(uint8 compartment, TESTMGR_PHASE_TYPE step);
void TestMgr__SetStorage(BOOL_TYPE state);

#endif // TESTMGR_H_


