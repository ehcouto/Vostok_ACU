/**
 *  @file       
 *
 *  @brief This module implements the Sw05 test cases
 *
 *  @details   These API's are called from
 *              API         - API004,
 *              OPCODE      - API004_CMD_EXECUTE_FIXED_ARGS_FUNCTIONS
 *              Payload[0]  - 255
 *
 *
 * Format of the Sw05 protocol as below -
 * Api        Opcode                                       Payload[0]               Payload[1]          Payload[2]                         Payload[3]       Payload[4]      ...
 * API004(4)  API004_CMD_EXECUTE_FIXED_ARGS_FUNCTIONS(3)   SW05SoftwareTest(255)    SW05_TEST_ID_TYPE   SW05_TEST_ID_TYPE_PARAM_SIZE(n)    PARAM_VALUE[0]   PARAM_VALUE[1]  ...
 *
 *
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2023-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SW05SOFTWARETEST_H_
#define SW05SOFTWARETEST_H_

#include "SystemConfig.h"

#ifndef SW05_SOFTWARE_TESTS_FEATURE
    #define SW05_SOFTWARE_TESTS_FEATURE     DISABLED    //By default disable the SW05 tests
#endif


#define SW05_TEST_VERSION   1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    SW05_TEST_ID_ALL = 0,
    SW05_TEST_ID_FORCE_WATCHDOG,
    SW05_TEST_ID_ECHO_WIN_DATA,
    SW05_TEST_ID_SETTINGFILE_STATS,
    SW05_TEST_ID_MICRO_CLOCK_INFO,
    SW05_TEST_ID_GETSET_APPLIANCE_AND_STATISTIC_NVDATA,
    SW05_TEST_ID_GETSET_STATIC_NVDATA,
    SW05_TEST_ID_INTERRUPTS_ENABLED,

    SW05_TEST_ID_TASK_TIME_STATS,
    SW05_TEST_ID_SRTASKMONITOR_STATS,
    SW05_TEST_ID_STACK_STATS,
    SW05_TEST_ID_REVEAL_STATS,
    SW05_TEST_ID_SRISRMONITOR_STATS,

    SW05_TEST_ID_SRCLOCKMONITOR_STATS,
    SW05_NUM_CROSS_CATEGORY_TESTS,




    SW05_TEST_ID_CATEGORY = 128,
    SW05_TEST_NONE_INVALID = 255
}SW05_TEST_ID_TYPE;




//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#if (SW05_SOFTWARE_TESTS_FEATURE == ENABLED)
    void SW05SoftwareTest__Initialize(void);
    void SW05SoftwareTest__Execute(uint8 source_node_id, uint8 buffer_length, uint8 *buffer_ptr);
#else
    #define SW05SoftwareTest__Initialize()
    #define SW05SoftwareTest__Execute(source_node_id, buffer_length, buffer_ptr)
#endif

#endif // SW05SOFTWARETEST_H_


