/**
 * @file
 * @brief       Low-level control to acquire XYZ acceleration data from an accelerometer.
 *
 * @details     This module will work for both the LIS2DH and LIS3DH accelerometer from ST.
 *              There are some features of the LIS3DH that are not supported
 *              (Ex. ADC output registers)
 *
    @startuml{LISXDH_sm.png}
        title LISXDH State-Machine
        ' Suppress transition checks

        [*] --> BootDelay

        BootDelay : entry / AccelHwInit()
        BootDelay : do / WaitForBootComplete()
        BootDelay --> LowPowerMode : evBootComplete

        LowPowerMode : entry / AccelDisable()
        LowPowerMode : do / CheckForEnable()
        LowPowerMode --> Restart : evEnable\n/ LISxDH__FlushData()

        Restart : entry / StartTimer()\n\t AccelHwInit()\n\t AccelEnable()
        Restart : do / WaitForI2cReady()
        Restart --> HighResMode : evI2cReady
        Restart --> LowPowerMode : evTimeout\n[IsDisableRequested()]
        Restart --> Restart : evTimeout

        HighResMode : entry / StartTimer()
        HighResMode : do / WaitForDataReady()
        HighResMode --> ReadData : evDataReady
        HighResMode -u-> LowPowerMode : evDisable
        HighResMode -r-> Restart : evTimeout

        ReadData : entry / AccelReadOneShot()
        ReadData : do / WaitForData()
        ReadData --> LowPowerMode : evDataReady\n[IsDisableRequested()]
        ReadData --> ReadData : evDataReady\n/ StartTimer()
        ReadData -u-> ReadTemp : evReadComplete \n[ReadTemp]
        ReadData -u-> HighResMode : evReadComplete
        ReadData --> AppBufferFull : evAppBufferFull
        ReadData --> Restart : evTimeout

        ReadTemp : entry / StartTimer()\n\t AccelReadTemp()
        ReadTemp : do / WaitForTempData()
        ReadTemp -u-> HighResMode : evReadComplete
        ReadTemp --> Restart : evTimeout

        AppBufferFull : entry / ReportTooMuchData()
        AppBufferFull : do / CheckAppBuffer()
        AppBufferFull -r-> HighResMode : evBufferHasSpace
        AppBufferFull -u-> LowPowerMode : evDisable
    @enduml
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#include "LISxDH.h"
#include "LISxDH_prv.h"

// -- Other Modules --
#include "Log.h"
#include "Micro.h"
#if (SPI_ENABLED == 1)
#include "Gpio.h"
#include "Spi.h"
#endif
#include <string.h>

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Lisxdh State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Lisxdh state machine.
typedef enum LISXDH_STATE_ENUM
{
    STATE_LISXDH_NONE            = 0,

    STATE_LISXDH_BOOT_DELAY      = 1,               //!< (initial_state)
    STATE_LISXDH_LOW_POWER_MODE  = 2,
    STATE_LISXDH_RESTART         = 3,
    STATE_LISXDH_HIGH_RES_MODE   = 4,
    STATE_LISXDH_READ_DATA       = 5,
    STATE_LISXDH_READ_TEMP       = 6,
    STATE_LISXDH_APP_BUFFER_FULL = 7,

    STATE_LISXDH_END             = 8
} LISXDH_STATE_TYPE;

//! The state variable for the Lisxdh state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static LISXDH_STATE_TYPE Lisxdh_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void AccelDisable(void);
static void AccelEnable(void);
static void AccelHwInit(void);
static void AccelReadOneShot(void);
static void AccelReadTemp(void);
static void CheckAppBuffer(void);
static void CheckForEnable(void);
static BOOL_TYPE IsDisableRequested(void);
static BOOL_TYPE ReadTemp(void);
static void ReportTooMuchData(void);
static void StartTimer(void);
static void WaitForBootComplete(void);
static void WaitForData(void);
static void WaitForDataReady(void);
static void WaitForI2cReady(void);
static void WaitForTempData(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventAppBufferFull(void);
static void EventBootComplete(void);
static void EventBufferHasSpace(void);
static void EventDataReady(void);
static void EventDisable(void);
static void EventEnable(void);
static void EventI2cReady(void);
static void EventReadComplete(void);
static void EventTimeout(void);
static void LisxdhExecute(void);
static void LisxdhInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

#if (SPI_ENABLED == 1)
//! RW bit of the Spi command used to read a register.
//! For write commands, the bit is 0.
#define RW_READ                     0x80U

//! MS bit of the Spi command used to auto increment the register address in multiple read/write commands.
//! To read/write to a single register, the bit is 0.
#define MS_MULTIPLE                 0x40U

#endif

//! A bit mask used to request that the Accelerometer auto-increment the sub-address when reading
//! and writing multiple bytes. Refer to section 6.1.1 of the LIS3DH datasheet.
#define ACCEL_MULTIPLE_BYTES        0x80U

//! The first Accelerometer register written that controls configuration (temperature reading enabled).
#define ACCEL_REG_TEMP_CFG          0x1FU

//! The first Accelerometer register written that controls configuration (temperature reading disabled).
#define ACCEL_REG_CTRL_1            0x20U

//! The first Accelerometer register read to get the temperature data (there are 2 adjacent registers that must be read)
#define ACCEL_REG_OUT_TEMP_L        0x0CU

//! The Accelerometer register to get the device identification data
#define ACCEL_REG_WHO_AM_I          0x0FU

//! The first Accelerometer register read to get the XYZ acceleration data.
#define ACCEL_REG_OUT_X_L           0x28U

//! The first Accelerometer register read to get the Status and XYZ acceleration data.
#define ACCEL_REG_STATUS            0x27U

//! The time required for the Accelerometer to initialize itself.
//! NOTE: This value is based on the call rate of LISxDH__Background()
#define ACCEL_TIME_BOOT_UP          25U  // 25 * 200us = 5ms

//! The timeout after which the module decides that no XYZ data will ever be received.
//! NOTE: This value is based on the call rate of LISxDH__Background()
#define ACCEL_TIMEOUT_NO_DATA       100U // 100 * 200us = 20ms

//! The timeout after which the module decides that the I2C bus will never become available.
//! NOTE: This value is based on the call rate of LISxDH__Background()
#define I2C_TIMEOUT_READY           100U // 100 * 200us = 20ms

//! The name of this module for use by the Log module.
#define MODULE_NAME                 MODULE_LISxDH


// -- Private Macro Declarations --

// -- Private Enumerated Constant Declarations --

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_LISxDH_LOG_MESSAGE_ID_ENUM
{
    I2C_APPEARS_STUCK   = 1, //!< I2C_APPEARS_STUCK
    ACCEL_NOT_RESPONDING,    //!< ACCEL_NOT_RESPONDING
    NO_DATA_TIMEOUT,         //!< NO_DATA_TIMEOUT
    TOO_MUCH_DATA,           //!< TOO_MUCH_DATA
    DETECTED_OVERRUN,        //!< DETECTED_OVERRUN
    INVALID_AXIS_MAPPING,    //!< INVALID_AXIS_MAPPING
    TRACE_STATE         = 128//!< TRACE_STATE
} MODULE_LISxDH_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! The data structure read from the Accelerometer that includes XYZ acceleration data.
#if (SPI_ENABLED == 1)
typedef PACKED struct ACCEL_READ_REGISTERS_STRUCT
{
    uint8 Junk;                 // Return data from the byte of command data (can be ignored)
    uint8 Available     : 4;
    uint8 Overrun       : 4;
    sint16 Points[3];
} ACCEL_READ_REGISTERS_TYPE;
#else // I2C communication
typedef PACKED struct ACCEL_READ_REGISTERS_STRUCT
{
    uint8 Available     : 4;
    uint8 Overrun       : 4;
    sint16 Points[3];
} ACCEL_READ_REGISTERS_TYPE;
#endif

// The temperature sensor can be used to measure temperature variations. The output value represents a difference with respect to an unspecified reference value.
// Calibration is required to determine the reference temperature.
#if (SPI_ENABLED == 1)
typedef struct ACCEL_READ_TEMPERATURE_STRUCT
{
    uint8 Junk;                 // Return data from the byte of command data (can be ignored)
    uint8 Temp_LSB;             // Temperature data is left justified, two's complement. It is 10 bits unless LPen bit of CTRL_REG1 is 1 (Low Power Enabled).
    uint8 Temp_MSB;
} ACCEL_READ_TEMPERATURE_TYPE;
#else // I2C communication
typedef struct ACCEL_READ_TEMPERATURE_STRUCT
{
    uint8 Temp_LSB;             // Temperature data is left justified. It is 10 bits unless LPen bit of CTRL_REG1 is 1 (Low Power Enabled).
    uint8 Temp_MSB;
} ACCEL_READ_TEMPERATURE_TYPE;
#endif

//! The variables used by this module.
typedef struct LISxDH_VARS_STRUCT
{
    #if (ACCEL_COMM_TEST == ENABLED)
        //! A count of the number of failures that have been detected since reset.
        uint32 Failure_Indicator_Counter;

        //! A count of the number of accelerometer points that have been successfully read since reset.
        uint32 XYZ_Point_Counter;

        //! A count that measures time in increments of 250 microseconds.
        //! It can measure up to about 12.4 days then it will restart at 0.
        //! It can be used to verify that the slot is running correctly.
        uint32 Reference_Counter;
    #endif

    //! The array of points that have been read from the Accelerometer.
    //! The array is a circular buffer.
    sint16 Points[LISXDH_MAX_POINTS][3];

    #if (ACCEL_COMM_TEST == ENABLED)
        //! A timer used to measure the time that the comm failure indicator is set.
        uint16 Failure_Indicator_Timer;
    #endif

    //! A timer used to measure time and create timeout events.
    uint16 Timer;

    //! The temperature sensor can be used to measure temperature variations. The output value represents a difference with respect to an unspecified reference value.
    //! Calibration is required to determine the reference temperature.
    sint8  Temperature_Delta;

    //! The current requested mode of the module.
    LISXDH_MODE_TYPE Mode_Requested;

    //! The currently active mode of the module.
    LISXDH_MODE_TYPE Mode_Processed;

    //! An index into the Points[] array for the next point that will be read by the application.
    uint8 Read;

    //! The number of points in the Points[] array that the application has not yet read.
    uint8 Number_Of_Points;

    //! Counter used to read the temperature sensor only every 1 second
    uint8 Temperature_Read_Delay_Count;

    //! Amount to de-scale the resulting acceleration values to keep the values in units of 1mg/digit
    uint8 Scale_Factor;

    //! Counts the number of communication failures before publishing a fault over API 18
    uint8 Failure_Counter;

    LISXDH_SF_PARAM_TYPE * SF_Param_Ptr;
} LISxDH_VARS_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

//! The command sent to the accelerometer to request data starting at register ACCEL_STATUS_REG.
#if (SPI_ENABLED == 1)
static const uint8 ACCEL_CMD_DISABLE[] =
{
    ACCEL_REG_CTRL_1,                               //! Register address for the first register required for SPI commands
    0x00                                            //! CTRL_REG1: Put accelerometer in low power mode
};
#else
static const uint8 ACCEL_CMD_DISABLE[] =
{
    0x00                                            //! CTRL_REG1: Put accelerometer in low power mode
};
#endif

//! An array of the 4-byte command sequences that can be sent to the accelerometer for the Self Test feature.
#if (SPI_ENABLED == 1)
static const uint8 ACCEL_CMD_SELF_TEST_ENABLE[][6] =
{
    // LISXDH_MODE_ST_REF: Activate self-test reference
    {
        MS_MULTIPLE | ACCEL_REG_TEMP_CFG,           //!< Register address for the first register required for SPI commands
        0x00,                                       //!< TEMP_CFG_REG: Temperature reading disabled
        0x77,                                       //!< CTRL_REG1: 400Hz sampling on all axes
        0x00,                                       //!< CTRL_REG2: Disable hardware high-pass filter
        0x10,                                       //!< CTRL_REG3: INT1 pin set when XYZ data is ready
        0xA8                                        //!< CTRL_REG4: Block data update, +/-8g, 12-bit, no self-test
    },
    // LISXDH_MODE_ST_0: Activate self-test 0
    {
        MS_MULTIPLE | ACCEL_REG_TEMP_CFG,           //!< Register address for the first register required for SPI commands
        0x00,                                       //!< TEMP_CFG_REG: Temperature reading disabled
        0x77,                                       //!< CTRL_REG1: 400Hz sampling on all axes
        0x00,                                       //!< CTRL_REG2: Disable hardware high-pass filter
        0x10,                                       //!< CTRL_REG3: INT1 pin set when XYZ data is ready
        0xAA                                        //!< CTRL_REG4: Block data update, +/-8g, 12-bit, self-test 0
    },
    // LISXDH_MODE_ST_1: Activate self-test 1
    {
        MS_MULTIPLE | ACCEL_REG_TEMP_CFG,           //!< Register address for the first register required for SPI commands
        0x00,                                       //!< TEMP_CFG_REG: Temperature reading disabled
        0x77,                                       //!< CTRL_REG1: 400Hz sampling on all axes
        0x00,                                       //!< CTRL_REG2: Disable hardware high-pass filter
        0x10,                                       //!< CTRL_REG3: INT1 pin set when XYZ data is ready
        0xAC                                        //!< CTRL_REG4: Block data update, +/-8g, 12-bit, self-test 1
    }
};
#else
static const uint8 ACCEL_CMD_SELF_TEST_ENABLE[][4] =
{
    // LISXDH_MODE_ST_REF: Activate self-test reference
    {
        0x77,                                       //!< CTRL_REG1: 400Hz sampling on all axes
        0x00,                                       //!< CTRL_REG2: Disable hardware high-pass filter
        0x10,                                       //!< CTRL_REG3: INT1 pin set when XYZ data is ready
        0xA8                                        //!< CTRL_REG4: Block data update, +/-8g, 12-bit, no self-test
    },
    // LISXDH_MODE_ST_0: Activate self-test 0
    {
        0x77,                                       //!< CTRL_REG1: 400Hz sampling on all axes
        0x00,                                       //!< CTRL_REG2: Disable hardware high-pass filter
        0x10,                                       //!< CTRL_REG3: INT1 pin set when XYZ data is ready
        0xAA                                        //!< CTRL_REG4: Block data update, +/-8g, 12-bit, self-test 0
    },
    // LISXDH_MODE_ST_1: Activate self-test 1
    {
        0x77,                                       //!< CTRL_REG1: 400Hz sampling on all axes
        0x00,                                       //!< CTRL_REG2: Disable hardware high-pass filter
        0x10,                                       //!< CTRL_REG3: INT1 pin set when XYZ data is ready
        0xAC                                        //!< CTRL_REG4: Block data update, +/-8g, 12-bit, self-test 1
    }
};
#endif

//! The command sent to the accelerometer to request data starting at register ACCEL_STATUS_REG.
#if (SPI_ENABLED == 1)
// RW bit shall be 1 for a read command; MS bit shall be 1 to auto-increment the register address
static const uint8 ACCEL_CMD_READ[] =
{
    RW_READ | MS_MULTIPLE | ACCEL_REG_STATUS,
    0x00,   /* Dummy byte for reading the status register */
    0x00,   /* Dummy byte for reading the X_Low  register */
    0x00,   /* Dummy byte for reading the X_High register */
    0x00,   /* Dummy byte for reading the Y_Low  register */
    0x00,   /* Dummy byte for reading the Y_High register */
    0x00,   /* Dummy byte for reading the Z_Low  register */
    0x00    /* Dummy byte for reading the Z_High register */
};
#else
static const uint8 ACCEL_CMD_READ[] =
{
    ACCEL_MULTIPLE_BYTES | ACCEL_REG_STATUS
};
#endif

//! The command sent to the accelerometer to request the device temperature data (must read both temperature registers; only High register contains useful data)
#if (SPI_ENABLED == 1)
// RW bit shall be 1 for a read command; MS bit shall be 1 to auto-increment the register address
static const uint8 ACCEL_CMD_READ_TEMPERATURE[] =
{
    RW_READ | MS_MULTIPLE | ACCEL_REG_OUT_TEMP_L,
    0x00,   /* Dummy byte for reading the TEMP_L register */
    0x00    /* Dummy byte for reading the TEMP_H register */
};
#else
static const uint8 ACCEL_CMD_READ_TEMPERATURE[] =
{
    ACCEL_MULTIPLE_BYTES | ACCEL_REG_OUT_TEMP_L
};
#endif


// -- Private Variable Definitions --

//! The variables used by this module.
static LISxDH_VARS_TYPE LISxDH;

static LISXDH_SF_PARAM_TYPE ACCELEROMETERLLI_DEFAULT_SF_PARAM = {0, ACCELEROMETER_DEVICE_LISxDH, 0, I2C0, 2, 0, 0x18, I2C_400KHZ, 0, 0, 0, 1, 1, 1, 1, 9, 0, 0, 1, 1, 1, 2, 1, 0};

//  --- Private Function Prototypes -----------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the Accelerometer variables.
 */
void LISxDH__Initialize(LISXDH_SF_PARAM_TYPE * param_ptr)
{
#if ACCELEROMETER_ENABLED == 1
    // Clear all module RAM variables.
    memset(&LISxDH, 0, sizeof(LISxDH));

    #if (ACCEL_COMM_TEST == ENABLED)
        // Clear the failure indicator.
        ACCEL_FAILURE_INDICATOR_OFF();
        ACCEL_FAILURE_INDICATOR_CONFIG();
        LISxDH.Mode_Requested = LISXDH_MODE_400HZ;
    #endif

    if (param_ptr == NULL)
    {
        LISxDH.SF_Param_Ptr = &ACCELEROMETERLLI_DEFAULT_SF_PARAM;
    }
    else
    {
        LISxDH.SF_Param_Ptr = param_ptr;
    }

    // Configure the chip select pin
    Gpio__PinConfig(SPI_CS_PORT, SPI_CS_PIN, OUTPUT_PUSHPULL);
#if (SPI_ENABLED == 0)
    // For I2C, the chip select line must be tied high (per the datasheet)
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);
#endif

    // Start the state machine.
    LisxdhInitialize();
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Execute the background function for the LISxDH module.
 */
void LISxDH__Background(void)
{
#if ACCELEROMETER_ENABLED == 1
    // Update the local time keeping variable.
    LISxDH.Timer++;

    // Run the state machine.
    LisxdhExecute();

    #if (ACCEL_COMM_TEST == ENABLED)
        if (LISxDH.Failure_Indicator_Timer > 0)
        {
            LISxDH.Failure_Indicator_Timer--;
            ACCEL_FAILURE_INDICATOR_ON();
        }
        else
        {
            ACCEL_FAILURE_INDICATOR_OFF();
        }

        LISxDH.Reference_Counter++;
    #endif
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that the accelerometer be disabled.
 */
void LISxDH__Disable(void)
{
#if ((ACCELEROMETER_ENABLED == 1) && (ACCEL_COMM_TEST == DISABLED))
    LISxDH.Mode_Requested = LISXDH_MODE_DISABLED;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that the accelerometer be enabled.
 * @param mode = The type of operation that is desired.
 */
void LISxDH__Enable(LISXDH_MODE_TYPE mode)
{
#if ACCELEROMETER_ENABLED == 1
    // Save the requested state. The state machine will react if necessary.
    if (mode < LISXDH_MODE_COUNT)
    {
        LISxDH.Mode_Requested = mode;
    }
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Flush any data currently in the Points[] queue.
 */
void LISxDH__FlushData(void)
{
#if ACCELEROMETER_ENABLED == 1
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();       // Save and disable interrupts
    uint8 read_index = LISxDH.Read + LISxDH.Number_Of_Points;
    if (read_index >= LISXDH_MAX_POINTS)
    {
        read_index -= LISXDH_MAX_POINTS;
    }
    LISxDH.Read = read_index;
    LISxDH.Number_Of_Points = 0;
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();    // Restore interrupts
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the status of the LISxDH module.
 * @return The status of the LISxDH module.
 */
ACCELEROMETER_STATUS_TYPE LISxDH__GetStatus(void)
{
#if ACCELEROMETER_ENABLED == 1
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_BOOT_DELAY:
        case STATE_LISXDH_RESTART:
            return ACCELEROMETER_INITIALIZING;

        case STATE_LISXDH_LOW_POWER_MODE:
            return ACCELEROMETER_DISABLED;

        case STATE_LISXDH_APP_BUFFER_FULL:
            return ACCELEROMETER_BUFFER_FULL;

        case STATE_LISXDH_HIGH_RES_MODE:
        case STATE_LISXDH_READ_DATA:
        case STATE_LISXDH_READ_TEMP:
            return ACCELEROMETER_ACQUIRING;

        case STATE_LISXDH_NONE:
        default:
            return ACCELEROMETER_UNKNOWN;
    }
#else
    return ACCELEROMETER_DISABLED;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @return  Temperature variation from a reference temperature in Celsius
 */
sint8 LISxDH__GetTemperatureDelta(void)
{
    return(LISxDH.Temperature_Delta);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read a single XYZ acceleration vector from the Accelerometer module's buffer.
 * @param vector = Pointer to an array of three signed 16-bit values for the XYZ data.
 *                 Each point is stored as a signed Q4.8 fixed-point value.
 * @return TRUE if vector was updated with a single XYZ point. FALSE if the buffer is empty.
 */
BOOL_TYPE LISxDH__Read(sint16* vector)
{
    BOOL_TYPE retval = FALSE;
#if ACCELEROMETER_ENABLED == 1
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    if (LISxDH.Number_Of_Points > 0U)
    {
        memcpy(vector, LISxDH.Points[LISxDH.Read], 6);

        // Adjust the read pointer and the number of accelerometer points.
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();       // Save and disable interrupts
        LISxDH.Read++;
        if (LISxDH.Read >= LISXDH_MAX_POINTS)
        {
            LISxDH.Read = 0;
        }
        LISxDH.Number_Of_Points--;
        //Restore Interrupts
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();    // Restore interrupts
        retval = TRUE;
    }
#endif
    return(retval);
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send configuration data to the accelerometer to disable it.
 */
static void AccelDisable(void)
{
    if (LISxDH.Mode_Requested == LISXDH_MODE_DISABLED)
    {
        // Send disable command to the accelerometer.
#if (SPI_ENABLED == 1)
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);     // Pull chip select low during transmission
        (void)Spi__Write((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)ACCEL_CMD_DISABLE, sizeof(ACCEL_CMD_DISABLE));
#else
        I2c__Write((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, ACCEL_MULTIPLE_BYTES | ACCEL_REG_CTRL_1,
                   (void*)ACCEL_CMD_DISABLE, sizeof(ACCEL_CMD_DISABLE));
#endif
        LISxDH.Mode_Processed = LISXDH_MODE_DISABLED;
    }
    LISxDH.Number_Of_Points = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send configuration data to the accelerometer to enable it.
 */
static void AccelEnable(void)
{
    if (LISxDH.Mode_Requested == LISXDH_MODE_NORMAL)
    {
        #if (SPI_ENABLED == 1)
            Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission
        #endif

        // Send enable commands to the accelerometer.
        StartTimer();

        // Determine the value to de-scale the reading so that the units are always 1mg/digit

        const uint8 SCALE_FACTOR[] = {1, 2, 4, 8};       /* Scale = 0 (2g), Scale = 1 (4g), Scale = 2 (8g), Scale = 3 (16g) */
        const uint8 RESOLUTION_FACTOR[] = {16, 4, 1};    /* Res = 0 (8 bits), Res = 1 (10 bits), Res = 2 (12 bits) */
        LISxDH.Scale_Factor = SCALE_FACTOR[LISxDH.SF_Param_Ptr->Scale] * RESOLUTION_FACTOR[LISxDH.SF_Param_Ptr->Resolution];

        LISXDH_SF_PARAM_TYPE * ptr;                 // Create local variable to make code more readable
        ptr = LISxDH.SF_Param_Ptr;
        uint8 enable_command[6] = {0,0,0,0,0,0};    // 6 registers: TEMP_CFG_REG + CTRL_REG1 to CTRL_REG5

        // Set configuration for TEMP_CFG_REG
        enable_command[0] = 0xC0;                   // Enable Temperature reading

        // Set configuration for CTRL_REG1
        enable_command[1] = ((uint8)ptr->Data_Rate << 4) | ((uint8)ptr->Z_Axis_Enabled << 2) | ((uint8)ptr->Y_Axis_Enabled << 1) | ((uint8)ptr->X_Axis_Enabled);

        // Set configuration for CTRL_REG3
        if ((uint8)ptr->Trigger_Source == 0U)       // INT1
        {
            enable_command[3] = 0x10;               // Use Data Ready 1 interrupt
        }

        // Set configuration for CTRL_REG4
        enable_command[4] = ((uint8)ptr->Block_Update << 7U) | ((uint8)ptr->Big_Endian << 6U) | ((uint8)ptr->Scale << 4U);

        if (ptr->Resolution == 2U)
        {
            enable_command[4] |= (1U << 3U);          // Set High Resolution bit
        }

        // Set configuration for CTRL_REG5
        if (ptr->Fifo_Mode == 1U)
        {
            enable_command[5] = (1U << 6U);
        }

        // NOTE: The FIFO_CTRL_REG (2Eh) register is not configured
        // Default behavior will be Bypass mode for the FIFO and a trigger event using INT1
        // FM1-FM0 bits should be set based on the Fifo_Mode parameter
        // TR bit should be set based on the Trigger_Source parameter

#if (SPI_ENABLED == 1)
        // Need to add the register address to the data for SPI as there is no argument in Spi__Write that automatically appends this
        uint8 spi_command[7];
        spi_command[0] = MS_MULTIPLE | ACCEL_REG_TEMP_CFG;
        for (uint8 index = 0; index < 6U; index++)
        {
            spi_command[index + 1U] = enable_command[index];
        }
        (void)Spi__Write((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)spi_command, sizeof(spi_command));
#else
        I2c__Write((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, ACCEL_MULTIPLE_BYTES | ACCEL_REG_TEMP_CFG,
                   (void*)enable_command, sizeof(enable_command));
#endif
        LISxDH.Mode_Processed = LISxDH.Mode_Requested;
    }
    else if ((LISxDH.Mode_Requested == LISXDH_MODE_ST_REF) ||
             (LISxDH.Mode_Requested == LISXDH_MODE_ST_0) ||
             (LISxDH.Mode_Requested == LISXDH_MODE_ST_1))
    {
        // Send enable commands to the accelerometer.
        StartTimer();
        uint8 index = (uint8)LISxDH.Mode_Requested - 2U; // Adjust index into ACCEL_CMD_SELF_TEST_ENABLE table based on LISXDH_MODE_TYPE

        // No scaling during self test
        LISxDH.Scale_Factor = 1;

#if (SPI_ENABLED == 1)
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);     // Pull chip select low during transmission
        (void)Spi__Write((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)ACCEL_CMD_SELF_TEST_ENABLE[index], sizeof(ACCEL_CMD_SELF_TEST_ENABLE[index]));
#else
        I2c__Write((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, ACCEL_MULTIPLE_BYTES | ACCEL_REG_CTRL_1,
                   (void*)ACCEL_CMD_SELF_TEST_ENABLE[index], sizeof(ACCEL_CMD_SELF_TEST_ENABLE[index]));
#endif
        LISxDH.Mode_Processed = LISxDH.Mode_Requested;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the microcontroller peripherals used by the Accelerometer module.
 */
static void AccelHwInit(void)
{
#if (SPI_ENABLED == 1)
    Spi__Initialize((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port);
    Spi__SetConfiguration((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, SPI_POLARITY_SCK_IDLE_1, SPI_EDGE_SECOND, SPI_ODER_MSB_FIRST);
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high (reset CS)
#else
    // Setup the I2C peripheral.
    // NOTE: The I2C module includes the I2C R/W bit as part of the address.
    I2c__Initialize((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, (I2C_SPEED_TYPE)LISxDH.SF_Param_Ptr->I2C_Speed, I2C_ADDR_7BITS, (LISxDH.SF_Param_Ptr->I2C_Address << 1));

#endif

    // Perform any necessary custom configuration.
    LISXDH_CUSTOM_CONFIGURATION();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a command to the accelerometer to read a single XYZ data point.
 */
static void AccelReadOneShot(void)
{
#if (SPI_ENABLED == 1)

    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission
    (void)Spi__Write((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)ACCEL_CMD_READ, sizeof(ACCEL_CMD_READ));
#else
    I2c__WriteThenRead((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port,
                       (void*)ACCEL_CMD_READ, sizeof(ACCEL_CMD_READ),
                       sizeof(ACCEL_READ_REGISTERS_TYPE));
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void AccelReadTemp(void)
{
#if (SPI_ENABLED == 1)
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission
    (void)Spi__Write((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)ACCEL_CMD_READ_TEMPERATURE, sizeof(ACCEL_CMD_READ_TEMPERATURE));
#else
    I2c__WriteThenRead((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port,
                       (void*)ACCEL_CMD_READ_TEMPERATURE, sizeof(ACCEL_CMD_READ_TEMPERATURE),
                       sizeof(ACCEL_READ_TEMPERATURE_TYPE));
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine if the application buffer is full.
 */
static void CheckAppBuffer(void)
{
    if (LISxDH.Mode_Processed != LISxDH.Mode_Requested)
    {
        EventDisable();
    }
    else if (LISxDH.Number_Of_Points < LISXDH_MAX_POINTS)
    {
        EventBufferHasSpace();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Enable the accelerometer if requested by the application.
 */
static void CheckForEnable(void)
{
    if (LISxDH.Mode_Requested != LISXDH_MODE_DISABLED)
    {
#if (SPI_ENABLED == 1)
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high to prepare for future messages
#endif
        EventEnable();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine if a request has been made to disable the module.
 * @return TRUE if a request has been made to disable the module; otherwise FALSE.
 */
static BOOL_TYPE IsDisableRequested(void)
{
    return (BOOL_TYPE)(LISxDH.Mode_Requested == LISXDH_MODE_DISABLED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @return
 */
static BOOL_TYPE ReadTemp(void)
{
    LISxDH.Temperature_Read_Delay_Count++;

    if (LISxDH.Temperature_Read_Delay_Count >= 5U)
    {
        LISxDH.Temperature_Read_Delay_Count = 0;
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a log message to indicate that at least one XYZ data point was lost.
 * This message will occur when the data is received faster than the application can process it.
 */
static void ReportTooMuchData(void)
{
    LOG_ADD_EXCEPTION(TOO_MUCH_DATA, LISXDH_MAX_POINTS);
    ACCELEROMETER_DATA_OVERFLOW();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start the timer to measure a potential timeout.
 */
static void StartTimer(void)
{
    LISxDH.Timer = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait the necessary boot up time for the accelerometer hardware.
 */
static void WaitForBootComplete(void)
{
    if (LISxDH.Timer > ACCEL_TIME_BOOT_UP)
    {
        EventBootComplete();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the read process to complete on the I2C bus and then capture the acceleration data.
 */
static void WaitForData(void)
{
    // Check if the accelerometer data has been received.
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high now that transmission is complete
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
    {
#endif
        // Verify that there is space remaining in the application buffer for the new data.
        #if (ACCEL_COMM_TEST == ENABLED)
            if (LISxDH.Number_Of_Points >= LISXDH_MAX_POINTS)
            {
                LISxDH.Number_Of_Points = 0;
            }
        #endif
        if (LISxDH.Number_Of_Points < LISXDH_MAX_POINTS)
        {
            // Read the data that was received
            ACCEL_READ_REGISTERS_TYPE data;
#if (SPI_ENABLED == 1)
            (void)Spi__Read((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)&data, sizeof(ACCEL_READ_REGISTERS_TYPE));
#else
            if (I2c__Read((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, (void*)&data, sizeof(ACCEL_READ_REGISTERS_TYPE)) == TRUE)
#endif
            {
                if (data.Overrun != 0U)
                {
                    LOG_ADD_EXCEPTION(DETECTED_OVERRUN, COMBINE_NIBBLES(data.Overrun, data.Available));
                    ACCELEROMETER_DATA_OVERWRITTEN();
                }

                // Move the write_index to the next point in the circular buffer.
                uint8 write_index = LISxDH.Read + LISxDH.Number_Of_Points;
                if (write_index >= LISXDH_MAX_POINTS)
                {
                    write_index -= LISXDH_MAX_POINTS;
                }

                // Copy the acceleration measurements for the application to use
                // Transform the position of the axis data and the sign to match the orientation of the PCB
                LISxDH.Points[write_index][TRANSFORM_AXIS(MAP_AXIS_X)] = (TRANSFORM_MULTIPLIER(MAP_AXIS_X) * (data.Points[0] >> 4)) * LISxDH.Scale_Factor;
                LISxDH.Points[write_index][TRANSFORM_AXIS(MAP_AXIS_Y)] = (TRANSFORM_MULTIPLIER(MAP_AXIS_Y) * (data.Points[1] >> 4)) * LISxDH.Scale_Factor;
                LISxDH.Points[write_index][TRANSFORM_AXIS(MAP_AXIS_Z)] = (TRANSFORM_MULTIPLIER(MAP_AXIS_Z) * (data.Points[2] >> 4)) * LISxDH.Scale_Factor;
                LISxDH.Number_Of_Points++;

                #if (ACCEL_COMM_TEST == ENABLED)
                    LISxDH.XYZ_Point_Counter++;
                #endif

                LISxDH.Failure_Counter = 0;     // Reset failure counter
            }

            // Check if there is new data available from the accelerometer.
            if (LISXDH_IS_DATA_READY())
            {
                EventDataReady();
                return;
            }
            else
            {
                EventReadComplete();
                return;
            }
        }
        else
        {
            // There is no space remaining in the application buffer.
            EventAppBufferFull();
            return;
        }
    }

    if (LISxDH.Timer > I2C_TIMEOUT_READY)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            LISxDH.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            LISxDH.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(ACCEL_NOT_RESPONDING, 0);
        LISxDH.Failure_Counter++;
        if (LISxDH.Failure_Counter >= LISXDH_MAX_FAILURES)
        {
            LISxDH.Failure_Counter = 0;
            ACCELEROMETER_NOT_RESPONDING();
        }
        EventTimeout();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the accelerometer to indicate that at least one XYZ data point is ready.
 */
static void WaitForDataReady(void)
{
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
#endif
    {
        if (LISxDH.Mode_Processed != LISxDH.Mode_Requested)
        {
            EventDisable();
            return;
        }
        else if (LISXDH_IS_DATA_READY())
        {
            EventDataReady();
            return;
        }
    }

    if (LISxDH.Timer > ACCEL_TIMEOUT_NO_DATA)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            LISxDH.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            LISxDH.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(NO_DATA_TIMEOUT, 0);
        LISxDH.Failure_Counter++;
        if (LISxDH.Failure_Counter >= LISXDH_MAX_FAILURES)
        {
            LISxDH.Failure_Counter = 0;
            ACCELEROMETER_OLD_DATA();
        }
        EventTimeout();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the I2C bus to become idle.
 */
static void WaitForI2cReady(void)
{
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
    {        
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);       // Pull chip select high after transmission
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
    {
#endif

        EventI2cReady();
    }
    else if (LISxDH.Timer > I2C_TIMEOUT_READY)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            LISxDH.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            LISxDH.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(I2C_APPEARS_STUCK, Lisxdh_State);
        LISxDH.Failure_Counter++;
        if (LISxDH.Failure_Counter >= LISXDH_MAX_FAILURES)
        {
            LISxDH.Failure_Counter = 0;
            ACCELEROMETER_NOT_RESPONDING();
        }
        EventTimeout();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the read process to complete on the I2C bus and then capture the temperature data.
 */
static void WaitForTempData(void)
{
    // Check if the temperature data has been received.
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high now that transmission is complete
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
    {
#endif
        // Read the temperature data that was received from the I2C module.
        ACCEL_READ_TEMPERATURE_TYPE data;
#if (SPI_ENABLED == 1)
        (void)Spi__Read((SPI_ENUM_DEF)LISxDH.SF_Param_Ptr->Port, (void*)&data, sizeof(ACCEL_CMD_READ_TEMPERATURE));
#else
        if (I2c__Read((I2C_ENUM_TYPE)LISxDH.SF_Param_Ptr->Port, (void*)&data, sizeof(data)) == TRUE)
#endif
        {
            // Only need to use the data from the MSB register
            // Data size is 10 bits, but at 8 bit resolution 1 digit = 1 degree C so the data is divided by 4 (2^10 bits/ 2^8 bits) to get it into units of Celsius
            LISxDH.Temperature_Delta = (sint8)(data.Temp_MSB);

            LISxDH.Failure_Counter = 0;     // Reset failure counter

            EventReadComplete();
        }
    }

    if (LISxDH.Timer > (uint16)I2C_TIMEOUT_READY)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            LISxDH.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            LISxDH.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(ACCEL_NOT_RESPONDING, 1);
        LISxDH.Failure_Counter++;
        if (LISxDH.Failure_Counter >= LISXDH_MAX_FAILURES)
        {
            LISxDH.Failure_Counter = 0;
            ACCELEROMETER_NOT_RESPONDING();
        }
        EventTimeout();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Lisxdh State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the App Buffer Full Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventAppBufferFull(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_READ_DATA:
            Lisxdh_State = STATE_LISXDH_APP_BUFFER_FULL;
            ReportTooMuchData();                    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Boot Complete Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBootComplete(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_BOOT_DELAY:
            Lisxdh_State = STATE_LISXDH_LOW_POWER_MODE;
            AccelDisable();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Buffer Has Space Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBufferHasSpace(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_APP_BUFFER_FULL:
            Lisxdh_State = STATE_LISXDH_HIGH_RES_MODE;
            StartTimer();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Data Ready Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDataReady(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_HIGH_RES_MODE:
            Lisxdh_State = STATE_LISXDH_READ_DATA;
            AccelReadOneShot();                     // Entry Function.
            break;

        case STATE_LISXDH_READ_DATA:
            if (IsDisableRequested() == TRUE)
            {
                Lisxdh_State = STATE_LISXDH_LOW_POWER_MODE;
                AccelDisable();                     // Entry Function.
            }
            else
            {
                StartTimer();                       // Transition action function.
                Lisxdh_State = STATE_LISXDH_READ_DATA;
                AccelReadOneShot();                 // Entry Function.
            }
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Disable Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDisable(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_HIGH_RES_MODE:
        case STATE_LISXDH_APP_BUFFER_FULL:
            Lisxdh_State = STATE_LISXDH_LOW_POWER_MODE;
            AccelDisable();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Enable Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventEnable(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_LOW_POWER_MODE:
            LISxDH__FlushData();                    // Transition action function.
            Lisxdh_State = STATE_LISXDH_RESTART;
            StartTimer();                           // Entry Function.
            AccelHwInit();                          // Entry Function.
            AccelEnable();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the I2c Ready Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventI2cReady(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_RESTART:
            Lisxdh_State = STATE_LISXDH_HIGH_RES_MODE;
            StartTimer();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Read Complete Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReadComplete(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_READ_DATA:
            if (ReadTemp() == TRUE)
            {
                Lisxdh_State = STATE_LISXDH_READ_TEMP;
                StartTimer();                       // Entry Function.
                AccelReadTemp();                    // Entry Function.
            }
            else
            {
                Lisxdh_State = STATE_LISXDH_HIGH_RES_MODE;
                StartTimer();                       // Entry Function.
            }
            break;

        case STATE_LISXDH_READ_TEMP:
            Lisxdh_State = STATE_LISXDH_HIGH_RES_MODE;
            StartTimer();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timeout Event for the Lisxdh state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimeout(void)
{
    switch (Lisxdh_State)
    {
        case STATE_LISXDH_RESTART:
            if (IsDisableRequested() == TRUE)
            {
                Lisxdh_State = STATE_LISXDH_LOW_POWER_MODE;
                AccelDisable();                     // Entry Function.
            }
            else
            {
                Lisxdh_State = STATE_LISXDH_RESTART;
                StartTimer();                       // Entry Function.
                AccelHwInit();                      // Entry Function.
                AccelEnable();                      // Entry Function.
            }
            break;

        case STATE_LISXDH_HIGH_RES_MODE:
        case STATE_LISXDH_READ_DATA:
        case STATE_LISXDH_READ_TEMP:
            Lisxdh_State = STATE_LISXDH_RESTART;
            StartTimer();                           // Entry Function.
            AccelHwInit();                          // Entry Function.
            AccelEnable();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Lisxdh state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LisxdhExecute(void)
{
    switch(Lisxdh_State)
    {
        case STATE_LISXDH_BOOT_DELAY:
            WaitForBootComplete();                  // Do function.
            break;

        case STATE_LISXDH_LOW_POWER_MODE:
            CheckForEnable();                       // Do function.
            break;

        case STATE_LISXDH_RESTART:
            WaitForI2cReady();                      // Do function.
            break;

        case STATE_LISXDH_HIGH_RES_MODE:
            WaitForDataReady();                     // Do function.
            break;

        case STATE_LISXDH_READ_DATA:
            WaitForData();                          // Do function.
            break;

        case STATE_LISXDH_READ_TEMP:
            WaitForTempData();                      // Do function.
            break;

        case STATE_LISXDH_APP_BUFFER_FULL:
            CheckAppBuffer();                       // Do function.
            break;

        default:
            // There is nothing to do for all other values of Lisxdh_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Lisxdh state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LisxdhInitialize(void)
{
    Lisxdh_State = STATE_LISXDH_BOOT_DELAY;
    AccelHwInit();                                  // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif // (HBL_LLI_NUM_ACCELEROMETER > 0)
