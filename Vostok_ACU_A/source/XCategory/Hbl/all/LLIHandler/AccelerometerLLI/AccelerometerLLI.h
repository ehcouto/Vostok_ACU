/**
 *  @file       
 *
 *  @brief      Public interface for the I2CAccelerometerLLI module.
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ACCELEROMETERLLI_H_
#define ACCELEROMETERLLI_H_

#ifndef HBL_LLI_NUM_ACCELEROMETER
    #define HBL_LLI_NUM_ACCELEROMETER 0
#endif

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#include "HblDefs.h"
#endif
#include "AccelerometerLLI_Defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Size of the accelerometer data buffer
#define ACCELEROMETER_BUFFER_SIZE    60

//! Structure for accelerometer axis data
//! This structure is available even if there is no accelerometer LLI so that modules using it will still compile
typedef struct
{
    sint16 X_Axis;
    sint16 Y_Axis;
    sint16 Z_Axis;
    uint32 Timestamp;           // Time (us) when the data was written
}ACCELEROMETER_AXIS_TYPE;

//! Public data for the Accelerometer LLI
//! NOTE: This structure is intentionally not forcibly packed but is intended not to have any extra padding bytes
//! Id, Status, Self Test Status, Self Test Errors and Raw data should come first in the structure so they fit in the first 8 bytes read by external tools
typedef struct
{
    BOOL_TYPE                  Self_Test_Completed;                 // Status of the self test of the accelerometer (TRUE: Complete, FALSE: Incomplete)
    uint8                      Self_Test_Errors;                    // Number of times the self-test has failed sequentially
    uint8                      Id;                                  // Unique identifier for the accelerometer
    ACCELEROMETER_STATUS_TYPE  Status;                              // Status of the accelerometer
    ACCELEROMETER_AXIS_TYPE    Raw[ACCELEROMETER_BUFFER_SIZE];      // Raw accelerometer data in mg
    /* Entries after this point are not intended to be read by external tools so the order does not matter */
    ACCELEROMETER_DEVICE_TYPE  Device;                              // Device that has been identified
    uint16                     Output_Data_Rate;                    // Output data rate in Hz
    uint8                      Last_Written_Index;                  // Index where the last accelerometer data was written
}ACCELEROMETER_DATA_TYPE;


#if (HBL_LLI_NUM_ACCELEROMETER > 0)

#define ACCELEROMETER_LLI_REGISTER  AccelerometerLLI__Initialize,            \
                                    AccelerometerLLI__Allocate,              \
                                    AccelerometerLLI__InitializeInstance,    \
                                    AccelerometerLLI__Get,                   \
                                    AccelerometerLLI__AsynchProcess,         \
                                    NULL,  /* No Peak Process required  */      \
                                    NULL,  /* No Input Capture required */      \
                                    NULL,  /* No 1ms Process required   */      \
                                    10     /* Id (1 byte), Status (1 byte), Self Test Status (1 byte), Self_Test_Errors (1 byte), Raw X (2 bytes), Raw Y (2 bytes), Raw Z (2 bytes) */

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void AccelerometerLLI__Initialize(void);
uint8 AccelerometerLLI__Allocate(void);
PASS_FAIL_TYPE AccelerometerLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * AccelerometerLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE AccelerometerLLI__AsynchProcess(HBL_LLI_TYPE * input);

// Non-standard functions for LLI
void AccelerometerLLI__FastProcess(void);
void AccelerometerLLI__Enable(void);
void AccelerometerLLI__Disable(void);
void AccelerometerLLI__RunSelfTest(void);
void AccelerometerLLI__GetDataPointer(ACCELEROMETER_AXIS_TYPE * ptr);

#endif //(HBL_LLI_NUM_ACCELEROMETER > 0)

#endif // I2CACCELEROMETERLLI_H_
