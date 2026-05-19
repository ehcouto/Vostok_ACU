/**
 *  @file       
 *
 *  @brief      Low-level control to acquire XYZ acceleration data from an accelerometer.
 *
 *  @details    This module works with the FXL29874CF accelerometer from NXP.
 *
 *  @section    Applicable_Documents
 *                  FXLS8974CF Datasheet (Rev 1.3)
 *
    @startuml{FXLS8974_sm.png}
        title FXLS8974 State-Machine
        ' Suppress transition checks

        [*] --> BootDelay

        BootDelay : entry / AccelHwInit()
        BootDelay : do / WaitForBootComplete()
        BootDelay --> LowPowerMode : evBootComplete

        LowPowerMode : entry / AccelDisable()
        LowPowerMode : do / CheckForEnable()
        LowPowerMode --> Restart : evEnable\n/ FXLS8974__FlushData()

        Restart : entry / StartTimer()\n\t AccelHwInit()\n\t AccelEnable()
        Restart : do / WaitForI2cReady()
        Restart --> SelfTest : evI2cReady [SelfTestEnabled]
        Restart --> Activate : evI2cReady
        Restart --> LowPowerMode : evTimeout\n[IsDisableRequested()]
        Restart --> Restart : evTimeout

        SelfTest : entry / ConfigSelfTest()
        SelfTest : do / WaitForI2cReady()
        SelfTest --> Activate : evI2cReady

        Activate : entry / StartTimer()\n\t AccelActivate()
        Activate : do / WaitForI2cReady()
        Activate --> HighResMode : evI2cReady

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

 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#include "FXLS8974.h"
#include "FXLS8974_prv.h"

// -- Other Modules --
#include "Gpio.h"
#include "Log.h"
#if (SPI_ENABLED == 1)
#include "Spi.h"
#endif
#include <math.h>
#include "Micro_Defs.h"
#include <string.h>

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Fxls8974 State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Fxls8974 state machine.
typedef enum FXLS8974_STATE_ENUM
{
    STATE_FXLS8974_NONE            = 0,

    STATE_FXLS8974_BOOT_DELAY      = 1,             //!< (initial_state)
    STATE_FXLS8974_LOW_POWER_MODE  = 2,
    STATE_FXLS8974_RESTART         = 3,
    STATE_FXLS8974_SELF_TEST       = 4,
    STATE_FXLS8974_ACTIVATE        = 5,
    STATE_FXLS8974_HIGH_RES_MODE   = 6,
    STATE_FXLS8974_READ_DATA       = 7,
    STATE_FXLS8974_READ_TEMP       = 8,
    STATE_FXLS8974_APP_BUFFER_FULL = 9,

    STATE_FXLS8974_END             = 10
} FXLS8974_STATE_TYPE;

//! The state variable for the Fxls8974 state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static FXLS8974_STATE_TYPE Fxls8974_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void AccelActivate(void);
static void AccelDisable(void);
static void AccelEnable(void);
static void AccelHwInit(void);
static void AccelReadOneShot(void);
static void AccelReadTemp(void);
static void CheckAppBuffer(void);
static void CheckForEnable(void);
static void ConfigSelfTest(void);
static BOOL_TYPE IsDisableRequested(void);
static BOOL_TYPE ReadTemp(void);
static void ReportTooMuchData(void);
static BOOL_TYPE SelfTestEnabled(void);
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
static void Fxls8974Execute(void);
static void Fxls8974Initialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//  --- Private Declarations ------------------------------------------------------------------------------------------

// -- Private Constant Declarations --

#if (SPI_ENABLED == 1)

//! RW bit of the Spi command used to read a register.
//! For write commands, the bit is 0.
#define RW_READ                     0x80U

#endif

//! The first Accelerometer register written that controls configuration
#define ACCEL_REG_SENS_CONFIG_1     0x15U

//! The Accelerometer register read to get the temperature data
#define ACCEL_REG_OUT_TEMP          0x01U

//! The Accelerometer register to get the device identification data
#define ACCEL_REG_WHO_AM_I          0x13U

//! The first Accelerometer register read to get the XYZ acceleration data.
#define ACCEL_REG_OUT_X_L           0x04U

//! The Accelerometer register to set the decimation factor for the self test feature
#define ACCEL_REG_SELF_TEST_CFG_2   0x38U

//! The time required for the Accelerometer to initialize itself.
//! Each count represents 250 microseconds.
#define ACCEL_TIME_BOOT_UP          25U  // 25 * 200us = 5ms

//! The timeout after which the module decides that no XYZ data will ever be received.
//! NOTE: This value is based on the call rate of FXLS8974__Background()
#define ACCEL_TIMEOUT_NO_DATA       100U // 100 * 200us = 20ms

//! The timeout after which the module decides that no XYZ data will ever be received.
//! NOTE: This value is based on the call rate of FXLS8974__Background()
#define ACCEL_TIMEOUT_SELF_TEST_NO_DATA 2000U    // 2000 * 200us = 400ms

//! The timeout after which the module decides that the I2C bus will never become available.
//! NOTE: This value is based on the call rate of FXLS8974__Background()
#define I2C_TIMEOUT_READY           100U // 100 * 200us = 20ms

//! The name of this module for use by the Log module.
#define MODULE_NAME                 MODULE_FXLS8974

// -- Private Macro Declarations --

// -- Private Enumerated Constant Declarations --

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_FXLS8974_LOG_MESSAGE_ID_ENUM
{
    I2C_APPEARS_STUCK   = 1, //!< I2C_APPEARS_STUCK
    ACCEL_NOT_RESPONDING,    //!< ACCEL_NOT_RESPONDING
    NO_DATA_TIMEOUT,         //!< NO_DATA_TIMEOUT
    TOO_MUCH_DATA,           //!< TOO_MUCH_DATA
    DETECTED_OVERRUN,        //!< DETECTED_OVERRUN
    INVALID_AXIS_MAPPING,    //!< INVALID_AXIS_MAPPING
    TRACE_STATE         = 128//!< TRACE_STATE
} MODULE_FXLS8974_LOG_MESSAGE_ID_TYPE;

// -- Private Structure Type Declarations --

//! The data structure read from the Accelerometer that includes XYZ acceleration data.
#if (SPI_ENABLED == 1)
typedef PACKED struct ACCEL_READ_REGISTERS_STRUCT
{
    uint16 Junk;               // 2 dummy bytes(can be ignored)
    sint16 Points[3];
} ACCEL_READ_REGISTERS_TYPE;
#else // I2C communication
typedef PACKED struct ACCEL_READ_REGISTERS_STRUCT
{
    sint16 Points[3];
} ACCEL_READ_REGISTERS_TYPE;

#endif

// The temperature sensor can be used to measure temperature variations. The output value represents a difference with respect to an unspecified reference value.
// Calibration is required to determine the reference temperature.
#if (SPI_ENABLED == 1)
typedef struct ACCEL_READ_TEMPERATURE_STRUCT
{
    uint16 Junk;                // 2 dummy bytes(can be ignored)
    sint8  Value;
} ACCEL_READ_TEMPERATURE_TYPE;
#else // I2C communication
typedef struct ACCEL_READ_TEMPERATURE_STRUCT
{
    sint8  Value;
} ACCEL_READ_TEMPERATURE_TYPE;
#endif

//! The variables used by this module.
typedef struct FXLS8974_VARS_STRUCT
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
    sint16 Points[FXLS8974_MAX_POINTS][3];

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
    FXLS8974_MODE_TYPE Mode_Requested;

    //! The currently active mode of the module.
    FXLS8974_MODE_TYPE Mode_Processed;

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

    FXLS8974_SF_PARAM_TYPE * SF_Param_Ptr;
} FXLS8974_VARS_TYPE;

//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --


//! The command sent to the accelerometer to request data starting at register ACCEL_STATUS_REG.
#if (SPI_ENABLED == 1)
static const uint8 ACCEL_CMD_DISABLE[] =
{
    ACCEL_REG_SENS_CONFIG_1,                        //! Register address for the first register required for SPI commands
    0xFF,                                           //! Dummy byte required after the register address
    0x00                                            //! SENS_CONFIG1: Put accelerometer in standby mode
};
#else
static const uint8 ACCEL_CMD_DISABLE[] =
{
    0x00                                            //! SENS_CONFIG1: Put accelerometer in standby mode
};
#endif

//! An array of the command sequences that can be sent to the accelerometer for the Self Test feature.
#if (SPI_ENABLED == 1)
static const uint8 ACCEL_CMD_SELF_TEST_ENABLE[][15] =
{
    // FXLS8974_MODE_POSITIVE_X
    {
        ACCEL_REG_SENS_CONFIG_1,    // Register address for the first register required for SPI commands
        0x00,                       // Dummy byte required after the register address
        0x26,   // SENS_CONFIG1:  ST X Axis, ST_POL Positive, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_NEGATIVE_X
    {
        ACCEL_REG_SENS_CONFIG_1,    // Register address for the first register required for SPI commands
        0x00,                       // Dummy byte required after the register address
        0x36,   // SENS_CONFIG1:  ST X Axis, ST_POL Negative, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_POSITIVE_Y
    {
        ACCEL_REG_SENS_CONFIG_1,    // Register address for the first register required for SPI commands
        0x00,                       // Dummy byte required after the register address
        0x46,   // SENS_CONFIG1:  ST Y Axis, ST_POL Positive, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_NEGATIVE_Y
    {
        ACCEL_REG_SENS_CONFIG_1,    // Register address for the first register required for SPI commands
        0x00,                       // Dummy byte required after the register address
        0x56,   // SENS_CONFIG1:  ST Y Axis, ST_POL Negative, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_POSITIVE_Z
    {
        ACCEL_REG_SENS_CONFIG_1,    // Register address for the first register required for SPI commands
        0x00,                       // Dummy byte required after the register address
        0x66,   // SENS_CONFIG1:  ST Z Axis, ST_POL Positive, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_NEGATIVE_Z
    {
        ACCEL_REG_SENS_CONFIG_1,    // Register address for the first register required for SPI commands
        0x00,                       // Dummy byte required after the register address
        0x76,   // SENS_CONFIG1:  ST Z Axis, ST_POL Negative, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    }
};
#else
static const uint8 ACCEL_CMD_SELF_TEST_ENABLE[][13] =
{
    // FXLS8974_MODE_POSITIVE_X
    {
        0x26,   // SENS_CONFIG1:  ST X Axis, ST_POL Positive, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_NEGATIVE_X
    {
        0x36,   // SENS_CONFIG1:  ST X Axis, ST_POL Negative, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_POSITIVE_Y
    {
        0x46,   // SENS_CONFIG1:  ST Y Axis, ST_POL Positive, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_NEGATIVE_Y
    {
        0x56,   // SENS_CONFIG1:  ST Y Axis, ST_POL Negative, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_POSITIVE_Z
    {
        0x66,   // SENS_CONFIG1:  ST Z Axis, ST_POL Positive, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    },
    // FXLS8974_MODE_NEGATIVE_Z
    {
        0x76,   // SENS_CONFIG1:  ST Z Axis, ST_POL Negative, SPI 4-wire, 16g Range, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
        0x50,   // SENS_CONFIG2:  High performance mode, little endian, Normal Read mode
        0x55,   // SENS_CONFIG3   100Hz ODR (first sample will be valid)
        0x01,   // SENS_CONFIG4:  INT_POL sets interrupt events as active high
        0x00,   // SENS_CONFIG5:  Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        0x00,   // WAKE_IDLE_LSB  (not used)
        0x00,   // WAKE_IDLE_MSB  (not used)
        0x00,   // SLEEP_IDLE_LSB (not used)
        0x00,   // SLEEP_IDLE_MSB (not used)
        0x00,   // ASLP_COUNT_LSB (not used)
        0x00,   // ASLP_COUNT_MSB (not used)
        0x82,   // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        0x7F    // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin
    }
};
#endif

//! The command sent to the accelerometer to request x,y,z data
#if (SPI_ENABLED == 1)
// RW bit shall be 1 for a read command
static const uint8 ACCEL_CMD_READ[] =
{
    RW_READ | ACCEL_REG_OUT_X_L,
    0x00,   /* Dummy byte required after the register address */
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
    ACCEL_REG_OUT_X_L
};
#endif

//! The command sent to the accelerometer to request the device temperature data (must read both temperature registers; only High register contains useful data)
#if (SPI_ENABLED == 1)
// RW bit shall be 1 for a read command; MS bit shall be 1 to auto-increment the register address
static const uint8 ACCEL_CMD_READ_TEMPERATURE[] =
{
    RW_READ | ACCEL_REG_OUT_TEMP,
    0x00,  /* Dummy byte required after the register address */
    0x00   /* Dummy byte for reading the TEMP register */
};
#else
static const uint8 ACCEL_CMD_READ_TEMPERATURE[] =
{
    ACCEL_REG_OUT_TEMP
};
#endif


//! Table to conver the setting file output data rate to the value to set the ODR in the SENS_CONFIG3 register
static const uint8 CONVERT_ODR[10] = {
                                         15, /*0 = Standby Mode*/
                                         11, /*1 = 1.5Hz*/
                                         8,  /*2 = 12.5Hz*/
                                         7,  /*3 = 25Hz*/
                                         6,  /*4 = 50Hz*/
                                         5,  /*5 = 100Hz*/
                                         4,  /*6 = 200Hz*/
                                         3,  /*7 = 400Hz*/
                                         1,  /*8 = 1600Hz*/
                                         0   /*9 = 3200Hz*/
                                     };

// -- Private Variable Definitions --

//! The variables used by this module.
static FXLS8974_VARS_TYPE Fxls8974;

static FXLS8974_SF_PARAM_TYPE ACCELEROMETERLLI_DEFAULT_SF_PARAM = {0, ACCELEROMETER_DEVICE_FXLS8974, 0, I2C0, 2, 0, 0x19, I2C_400KHZ, 2, 0, 0, 1, 1, 1, 1, 9, 0, 0, 1, 1, 1, 2, 1, 0};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initializes the module variables
 *
 */
void FXLS8974__Initialize(FXLS8974_SF_PARAM_TYPE * param_ptr)
{
    // Clear all module RAM variables.
    memset(&Fxls8974, 0, sizeof(Fxls8974));

    if (param_ptr == NULL)
    {
        Fxls8974.SF_Param_Ptr = &ACCELEROMETERLLI_DEFAULT_SF_PARAM;
    }
    else
    {
        Fxls8974.SF_Param_Ptr = param_ptr;
    }

#if (SPI_ENABLED == 1)
    Gpio__PinConfig(SPI_CS_PORT, SPI_CS_PIN, OUTPUT_PUSHPULL);
#endif

    // Start the state machine.
    Fxls8974Initialize();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Execute the background function for the module.
 */
void FXLS8974__Background(void)
{
    // Update the local time keeping variable.
    Fxls8974.Timer++;

    // Run the state machine.
    Fxls8974Execute();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that the accelerometer be disabled.
 */
void FXLS8974__Disable(void)
{
    Fxls8974.Mode_Requested = FXLS8974_MODE_DISABLED;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that the accelerometer be enabled.
 * @param mode = The type of operation that is desired.
 */
void FXLS8974__Enable(FXLS8974_MODE_TYPE mode)
{
    // Save the requested state. The state machine will react if necessary.
    if (mode < FXLS8974_MODE_COUNT)
    {
        Fxls8974.Mode_Requested = mode;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Flush any data currently in the Points[] queue.
 */
void FXLS8974__FlushData(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();       // Save and disable interrupts
    uint8 read_index = Fxls8974.Read + Fxls8974.Number_Of_Points;
    if (read_index >= FXLS8974_MAX_POINTS)
    {
        read_index -= FXLS8974_MAX_POINTS;
    }
    Fxls8974.Read = read_index;
    Fxls8974.Number_Of_Points = 0;
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();    // Restore interrupts
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the status of the module.
 * @return The status of the module.
 */
ACCELEROMETER_STATUS_TYPE FXLS8974__GetStatus(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_BOOT_DELAY:
        case STATE_FXLS8974_RESTART:
            return ACCELEROMETER_INITIALIZING;

        case STATE_FXLS8974_LOW_POWER_MODE:
            return ACCELEROMETER_DISABLED;

        case STATE_FXLS8974_APP_BUFFER_FULL:
            return ACCELEROMETER_BUFFER_FULL;

        case STATE_FXLS8974_HIGH_RES_MODE:
        case STATE_FXLS8974_READ_DATA:
        case STATE_FXLS8974_READ_TEMP:
            return ACCELEROMETER_ACQUIRING;

        case STATE_FXLS8974_NONE:
        default:
            return ACCELEROMETER_UNKNOWN;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @return  Temperature variation from a reference temperature in Celsius
 */
sint8 FXLS8974__GetTemperatureDelta(void)
{
    return(Fxls8974.Temperature_Delta);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read a single XYZ acceleration vector from the Accelerometer module's buffer.
 * @param vector = Pointer to an array of three signed 16-bit values for the XYZ data.
 *                 Each point is stored as a signed Q4.8 fixed-point value.
 * @return TRUE if vector was updated with a single XYZ point. FALSE if the buffer is empty.
 */
BOOL_TYPE FXLS8974__Read(sint16* vector)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    if (Fxls8974.Number_Of_Points > 0U)
    {
        memcpy(vector, Fxls8974.Points[Fxls8974.Read], 6);

        // Adjust the read pointer and the number of accelerometer points.
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();       // Save and disable interrupts
        Fxls8974.Read++;
        if (Fxls8974.Read >= FXLS8974_MAX_POINTS)
        {
            Fxls8974.Read = 0;
        }
        Fxls8974.Number_Of_Points--;
        //Restore Interrupts
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();    // Restore interrupts

        return TRUE;
    }
    return FALSE;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  This function only sets the "Active" bit in the SENS_CONFIG_1 register since all the other configuration is already
 *  done (Registers must be configured while the "Active" bit is 0 when the accelerometer is in Standby)
 */
static void AccelActivate(void)
{
    FXLS8974_SF_PARAM_TYPE * ptr = Fxls8974.SF_Param_Ptr;       // Create local variable to make code more readable
    uint8 command[1] = {0};                                     // registers: SENS_CONFIG1 to SENS_CONFIG4

    // Set configuration for SENS_CONFIG1
    if (SelfTestEnabled())
    {
        // Self Test mode
#if (SPI_ENABLED == 1)
        command[0] = ACCEL_CMD_SELF_TEST_ENABLE[(uint8)Fxls8974.Mode_Processed - 2U][2];   // Use the value for SENS_CONFIG1 from the table
#else
        command[0] = ACCEL_CMD_SELF_TEST_ENABLE[(uint8)Fxls8974.Mode_Processed - 2U][0];   // Use the value for SENS_CONFIG1 from the table
#endif
        BIT_SET(command[0], 0);                                                    // Set the ACTIVE bit to place the device in Active mode
    }
    else
    {
        // Normal mode
        command[0] = 0x01U | ((uint8)ptr->Scale << 1U);  //ST disabled, SPI 4-wire, 2/4/8/16g Scale, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)
    }

#if (SPI_ENABLED == 1)
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);     // Pull chip select low during transmission
    // Need to add the register address to the data for SPI as there is no argument in Spi__Write that automatically appends this
    uint8 spi_command[3];
    spi_command[0] = ACCEL_REG_SENS_CONFIG_1;
    spi_command[1] = 0xFF;                              // Dummy byte required after the register address
    spi_command[2] = command[0];
    (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)spi_command, sizeof(spi_command));
#else
        I2c__Write((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, ACCEL_REG_SENS_CONFIG_1,
                   (void*)command, sizeof(command));
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send configuration data to the accelerometer to disable it.
 */
static void AccelDisable(void)
{
    if (Fxls8974.Mode_Requested == FXLS8974_MODE_DISABLED)
    {
        // Send disable command to the accelerometer.
#if (SPI_ENABLED == 1)
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);     // Pull chip select low during transmission
        (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)ACCEL_CMD_DISABLE, sizeof(ACCEL_CMD_DISABLE));
#else
        I2c__Write((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, ACCEL_REG_SENS_CONFIG_1,
                   (void*)ACCEL_CMD_DISABLE, sizeof(ACCEL_CMD_DISABLE));
#endif
        Fxls8974.Mode_Processed = FXLS8974_MODE_DISABLED;
    }
    Fxls8974.Number_Of_Points = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send configuration data to the accelerometer to enable it.
 */
static void AccelEnable(void)
{
    if (Fxls8974.Mode_Requested == FXLS8974_MODE_NORMAL)
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission

        // Send enable commands to the accelerometer.
        StartTimer();

        // Determine the value to de-scale the reading so that the units are always 1mg/digit
        Fxls8974.Scale_Factor = (uint8)(pow(2, Fxls8974.SF_Param_Ptr->Scale));

        FXLS8974_SF_PARAM_TYPE * ptr = Fxls8974.SF_Param_Ptr;       // Create local variable to make code more readable
        uint8 enable_command[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};            // registers: SENS_CONFIG1 to SENS_CONFIG4

        // Set configuration for SENS_CONFIG1
        enable_command[0] = ((uint8)ptr->Scale << 1U);              //Self test disabled, SPI 4-wire, 2/4/8/16g Scale, Standby mode (REGISTERS CAN ONLY BE MODIFIED IN STANDBY MODE)

        // Set configuration for SENS_CONFIG2
        enable_command[1] = ((uint8)ptr->Big_Endian << 3U);

        if (ptr->Resolution == 2U)
        {
            enable_command[1] |= (1U << 6U);                        // Set WAKE to "High performance mode" (affects possible ODRs)
        }

        // Set configuration for SENS_CONFIG3
        uint8 output_data_rate = CONVERT_ODR[ptr->Data_Rate];
        /* TEMPORARY */
        // TODO: Update setting file so the ODR is 1600 instead of 3200; No temperature reading occurs at 3200Hz
        output_data_rate = 1U; // 1 corresponds to 1600Hz

        enable_command[2] = (output_data_rate << 4U) | output_data_rate; // Set WAKE_ODR and SLEEP_ODR to same data rate

        // Set configuration for SENS_CONFIG4
        enable_command[3] = 0x01;                   // INT_POL sets interrupt events as active high
//        enable_command[3] = (ptr->Block_Update << 7);

        if (ptr->Fifo_Mode == 1U)
        {
//            enable_command[4] = (1 << 6);
        }

        enable_command[4]  = 0x00;  // SENS_CONFIG5: Vector Magnitude calculation disabled, x/y/z axis auto-increment address enabled, hibernate disabled
        enable_command[5]  = 0x00;  // WAKE_IDLE_LSB (not used)
        enable_command[6]  = 0x00;  // WAKE_IDLE_MSB (not used)
        enable_command[7]  = 0x00;  // SLEEP_IDLE_LSB (not used)
        enable_command[8]  = 0x00;  // SLEEP_IDLE_MSB (not used)
        enable_command[9]  = 0x00;  // ASLP_COUNT_LSB (not used)
        enable_command[10] = 0x00;  // ASLP_COUNT_MSB (not used)
        enable_command[11] = 0x82;  // INT_EN: Data-ready interrupt output enabled, Boot Interrupt Disabled
        enable_command[12] = 0x7F;  // INT_PIN_SEL: DRDY_INT2 data-ready interrupt routing to INT1 pin


#if (SPI_ENABLED == 1)
        // Need to add the register address to the data for SPI as there is no argument in Spi__Write that automatically appends this
        uint8 spi_command[15];
        spi_command[0] = ACCEL_REG_SENS_CONFIG_1;
        spi_command[1] = 0x00;  // Dummy byte required after the register address
        for (uint8 index = 0; index < 13U; index++)
        {
            spi_command[index + 2U] = enable_command[index];
        }
        (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)spi_command, sizeof(spi_command));
#else
        I2c__Write((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, ACCEL_REG_SENS_CONFIG_1,
                   (void*)enable_command, sizeof(enable_command));
#endif
        Fxls8974.Mode_Processed = Fxls8974.Mode_Requested;
    }
    else if ((Fxls8974.Mode_Requested == FXLS8974_MODE_ST_POSITIVE_X) ||
             (Fxls8974.Mode_Requested == FXLS8974_MODE_ST_POSITIVE_Y) ||
             (Fxls8974.Mode_Requested == FXLS8974_MODE_ST_POSITIVE_Z) ||
             (Fxls8974.Mode_Requested == FXLS8974_MODE_ST_NEGATIVE_X) ||
             (Fxls8974.Mode_Requested == FXLS8974_MODE_ST_NEGATIVE_Y) ||
             (Fxls8974.Mode_Requested == FXLS8974_MODE_ST_NEGATIVE_Z))
    {

        // Send enable commands to the accelerometer.
        StartTimer();
        uint8 index = (uint8)Fxls8974.Mode_Requested - 2U; // Adjust index into ACCEL_CMD_SELF_TEST_ENABLE table based on FXLS8974_MODE_TYPE

        // No scaling during self test
        Fxls8974.Scale_Factor = 1;

#if (SPI_ENABLED == 1)
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission
        (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)ACCEL_CMD_SELF_TEST_ENABLE[index], sizeof(ACCEL_CMD_SELF_TEST_ENABLE[index]));
#else
        I2c__Write((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, ACCEL_REG_SENS_CONFIG_1,
                   (void*)ACCEL_CMD_SELF_TEST_ENABLE[index], sizeof(ACCEL_CMD_SELF_TEST_ENABLE[index]));
#endif
        Fxls8974.Mode_Processed = Fxls8974.Mode_Requested;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the microcontroller peripherals used by the Accelerometer module.
 */
static void AccelHwInit(void)
{
#if (SPI_ENABLED == 1)
    // Set the INTF_SEL pin high for SPI
    Gpio__PinConfig(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, OUTPUT_PUSHPULL);
    Gpio__PinWrite(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, TRUE);

    // Set the BT_MODE pin for Boot Mode Selection (Low: Default mode; High: Motion Detection mode)
    Gpio__PinConfig(BOOT_MODE_PORT, BOOT_MODE_PIN, OUTPUT_PUSHPULL);
    Gpio__PinWrite(BOOT_MODE_PORT, BOOT_MODE_PIN, FALSE);

    Spi__Initialize((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port);
    Spi__SetConfiguration((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, SPI_POLARITY_SCK_IDLE_0, SPI_EDGE_FIRST, SPI_ODER_MSB_FIRST);
#else
    // Set the INTF_SEL pin low for I2C
    Gpio__PinConfig(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, OUTPUT_PUSHPULL);
    Gpio__PinWrite(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, FALSE);

    // Set the BT_MODE pin for Boot Mode Selection (Low: Default mode; High: Motion Detection mode)
    Gpio__PinConfig(BOOT_MODE_PORT, BOOT_MODE_PIN, OUTPUT_PUSHPULL);
    Gpio__PinWrite(BOOT_MODE_PORT, BOOT_MODE_PIN, FALSE);

    // Setup the I2C peripheral.
    // NOTE: The I2C module includes the I2C R/W bit as part of the address.
    I2c__Initialize((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, (I2C_SPEED_TYPE)Fxls8974.SF_Param_Ptr->I2C_Speed, I2C_ADDR_7BITS, (Fxls8974.SF_Param_Ptr->I2C_Address << 1));

#endif

    // Perform any necessary custom configuration.
    FXLS8974_CUSTOM_CONFIGURATION();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a command to the accelerometer to read a single XYZ data point.
 */
static void AccelReadOneShot(void)
{
#if (SPI_ENABLED == 1)
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission
    (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)ACCEL_CMD_READ, sizeof(ACCEL_CMD_READ));
#else
    I2c__WriteThenRead((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port,
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
    (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)ACCEL_CMD_READ_TEMPERATURE, sizeof(ACCEL_CMD_READ_TEMPERATURE));
#else
    I2c__WriteThenRead((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port,
                       (void*)ACCEL_CMD_READ_TEMPERATURE, sizeof(ACCEL_CMD_READ_TEMPERATURE), 1);   // Temperature data is only one byte on the FXLS8974
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine if the application buffer is full.
 */
static void CheckAppBuffer(void)
{
    if (Fxls8974.Mode_Processed != Fxls8974.Mode_Requested)
    {
        EventDisable();
    }
    else if (Fxls8974.Number_Of_Points < FXLS8974_MAX_POINTS)
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
    if (Fxls8974.Mode_Requested != FXLS8974_MODE_DISABLED)
    {
        EventEnable();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  State machine function to configure the SLF_TEST_CONFIG_2 register. All other register settings are handled
 *  in the ACCEL_CMD_SELF_TEST_ENABLE table.
 */
static void ConfigSelfTest(void)
{
    uint8 command[3];

#if (SPI_ENABLED == 1)
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission

    // Need to add the register address to the data for SPI as there is no argument in Spi__Write that automatically appends this
    command[0] = ACCEL_REG_SELF_TEST_CFG_2;
    command[1] = 0xFF;  // Dummy byte required after the register address
    command[2] = 0x08;  // Self Test measurement phase decimation factor for 256 samples over 80ms at 12.5 ODR
    (void)Spi__Write((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)command, sizeof(command));
#else
    command[0] = 0x08;  // Self Test measurement phase decimation factor for 256 samples over 80ms at 12.5 ODR
    I2c__Write((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, ACCEL_REG_SELF_TEST_CFG_2, (void*)command, 1);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine if a request has been made to disable the module.
 * @return TRUE if a request has been made to disable the module; otherwise FALSE.
 */
static BOOL_TYPE IsDisableRequested(void)
{
    return (BOOL_TYPE)(Fxls8974.Mode_Requested == FXLS8974_MODE_DISABLED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @return
 */
static BOOL_TYPE ReadTemp(void)
{
    Fxls8974.Temperature_Read_Delay_Count++;

    if (Fxls8974.Temperature_Read_Delay_Count >= 5U)
    {
        Fxls8974.Temperature_Read_Delay_Count = 0;
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
    LOG_ADD_EXCEPTION(TOO_MUCH_DATA, FXLS8974_MAX_POINTS);
    ACCELEROMETER_DATA_OVERFLOW();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start the timer to measure a potential timeout.
 */
static void StartTimer(void)
{
    Fxls8974.Timer = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait the necessary boot up time for the accelerometer hardware.
 */
static void WaitForBootComplete(void)
{
    if (Fxls8974.Timer > ACCEL_TIME_BOOT_UP)
    {
        EventBootComplete();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the read process to complete on the I2C bus.
 */
static void WaitForData(void)
{
    // Check if the accelerometer data has been received.
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high now that transmission is complete
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
    {
#endif
        // Verify that there is space remaining in the application buffer for the new data.
        #if (ACCEL_COMM_TEST == ENABLED)
            if (Fxls8974.Number_Of_Points >= FXLS8974_MAX_POINTS)
            {
                Fxls8974.Number_Of_Points = 0;
            }
        #endif
        if (Fxls8974.Number_Of_Points < FXLS8974_MAX_POINTS)
        {
            // Read the data that was received from the I2C module.
            ACCEL_READ_REGISTERS_TYPE data;
#if (SPI_ENABLED == 1)
            (void)Spi__Read((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)&data, sizeof(ACCEL_READ_REGISTERS_TYPE));
#else
            if (I2c__Read((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, (void*)&data, sizeof(ACCEL_READ_REGISTERS_TYPE)) == TRUE)
#endif
            {
                //TODO: This content has to be modified to read the proper register from the NXP accelerometer
//                if (data.Overrun != 0)
//                {
//                    LOG_ADD_EXCEPTION(DETECTED_OVERRUN, COMBINE_NIBBLES(data.Overrun, data.Available));
//                    ACCELEROMETER_DATA_OVERWRITTEN();
//                }

                // Move the write_index to the next point in the circular buffer.
                uint8 write_index = Fxls8974.Read + Fxls8974.Number_Of_Points;
                if (write_index >= FXLS8974_MAX_POINTS)
                {
                    write_index -= FXLS8974_MAX_POINTS;
                }

                // Copy the acceleration measurements for the application to use.
                Fxls8974.Points[write_index][TRANSFORM_AXIS(MAP_AXIS_X)] = TRANSFORM_MULTIPLIER(MAP_AXIS_X) * data.Points[0] * Fxls8974.Scale_Factor;
                Fxls8974.Points[write_index][TRANSFORM_AXIS(MAP_AXIS_Y)] = TRANSFORM_MULTIPLIER(MAP_AXIS_Y) * data.Points[1] * Fxls8974.Scale_Factor;
                Fxls8974.Points[write_index][TRANSFORM_AXIS(MAP_AXIS_Z)] = TRANSFORM_MULTIPLIER(MAP_AXIS_Z) * data.Points[2] * Fxls8974.Scale_Factor;

                Fxls8974.Number_Of_Points++;
                #if (ACCEL_COMM_TEST == ENABLED)
                    Fxls8974.XYZ_Point_Counter++;
                #endif

                Fxls8974.Failure_Counter = 0;     // Reset failure counter
            }

            // Check if there is new data available from the accelerometer.
            if (FXLS8974_IS_DATA_READY())
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

    if (Fxls8974.Timer > I2C_TIMEOUT_READY)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            Fxls8974.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            Fxls8974.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(ACCEL_NOT_RESPONDING, 0);
        Fxls8974.Failure_Counter++;
        if (Fxls8974.Failure_Counter >= FXLS8974_MAX_FAILURES)
        {
            Fxls8974.Failure_Counter = 0;
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
    if (Spi__GetStatus((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
#endif
    {
        if (Fxls8974.Mode_Processed != Fxls8974.Mode_Requested)
        {
            EventDisable();
            return;
        }
        else if (FXLS8974_IS_DATA_READY())
        {
            EventDataReady();
            return;
        }
    }

    if (Fxls8974.Timer > ACCEL_TIMEOUT_NO_DATA)
    {
        if ((SelfTestEnabled() == FALSE) ||                     // If self test is not running, this is a failure
            (Fxls8974.Timer > ACCEL_TIMEOUT_SELF_TEST_NO_DATA)) // If self test is running, it must exceed time based on ODR from SELF_TEST_CONFIG_2
        {
            #if (ACCEL_COMM_TEST == ENABLED)
                Fxls8974.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
                Fxls8974.Failure_Indicator_Counter++;
            #endif
            LOG_ADD_EXCEPTION(NO_DATA_TIMEOUT, 0);
            Fxls8974.Failure_Counter++;
            if (Fxls8974.Failure_Counter >= FXLS8974_MAX_FAILURES)
            {
                Fxls8974.Failure_Counter = 0;
                ACCELEROMETER_OLD_DATA();
            }
            EventTimeout();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the I2C bus to become idle.
 */
static void WaitForI2cReady(void)
{
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
#endif
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);       // Pull chip select high after transmission
        EventI2cReady();
    }
    else if (Fxls8974.Timer > I2C_TIMEOUT_READY)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            Fxls8974.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            Fxls8974.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(I2C_APPEARS_STUCK, Fxls8974_State);
        Fxls8974.Failure_Counter++;
        if (Fxls8974.Failure_Counter >= FXLS8974_MAX_FAILURES)
        {
            Fxls8974.Failure_Counter = 0;
            ACCELEROMETER_NOT_RESPONDING();
        }
        EventTimeout();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForTempData(void)
{
    // Check if the temperature data has been received.
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high now that transmission is complete
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
    {
#endif
        // Read the temperature data that was received from the I2C module.
        ACCEL_READ_TEMPERATURE_TYPE data;
#if (SPI_ENABLED == 1)
        (void)Spi__Read((SPI_ENUM_DEF)Fxls8974.SF_Param_Ptr->Port, (void*)&data, sizeof(ACCEL_CMD_READ_TEMPERATURE));
#else
        if (I2c__Read((I2C_ENUM_TYPE)Fxls8974.SF_Param_Ptr->Port, (void*)&data, 1) == TRUE)
#endif
        {
            Fxls8974.Temperature_Delta = data.Value;

            Fxls8974.Failure_Counter = 0;     // Reset failure counter

            EventReadComplete();
        }
    }

    if (Fxls8974.Timer > I2C_TIMEOUT_READY)
    {
        #if (ACCEL_COMM_TEST == ENABLED)
            Fxls8974.Failure_Indicator_Timer = ACCEL_FAILURE_INDICATOR_TIME;
            Fxls8974.Failure_Indicator_Counter++;
        #endif
        LOG_ADD_EXCEPTION(ACCEL_NOT_RESPONDING, 0);
        Fxls8974.Failure_Counter++;
        if (Fxls8974.Failure_Counter >= FXLS8974_MAX_FAILURES)
        {
            Fxls8974.Failure_Counter = 0;
            ACCELEROMETER_NOT_RESPONDING();
        }
        EventTimeout();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  State machine guard function to determine if self test feature is enabled
 * @return
 */
static BOOL_TYPE SelfTestEnabled(void)
{
    BOOL_TYPE is_self_test_enabled = FALSE;

    if ((Fxls8974.Mode_Processed == FXLS8974_MODE_ST_POSITIVE_X) ||
        (Fxls8974.Mode_Processed == FXLS8974_MODE_ST_POSITIVE_Y) ||
        (Fxls8974.Mode_Processed == FXLS8974_MODE_ST_POSITIVE_Z) ||
        (Fxls8974.Mode_Processed == FXLS8974_MODE_ST_NEGATIVE_X) ||
        (Fxls8974.Mode_Processed == FXLS8974_MODE_ST_NEGATIVE_Y) ||
        (Fxls8974.Mode_Processed == FXLS8974_MODE_ST_NEGATIVE_Z))
    {
        is_self_test_enabled = TRUE;
    }

    return(is_self_test_enabled);
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Fxls8974 State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the App Buffer Full Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventAppBufferFull(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_READ_DATA:
            Fxls8974_State = STATE_FXLS8974_APP_BUFFER_FULL;
            ReportTooMuchData();                    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Boot Complete Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBootComplete(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_BOOT_DELAY:
            Fxls8974_State = STATE_FXLS8974_LOW_POWER_MODE;
            AccelDisable();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Buffer Has Space Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBufferHasSpace(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_APP_BUFFER_FULL:
            Fxls8974_State = STATE_FXLS8974_HIGH_RES_MODE;
            StartTimer();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Data Ready Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDataReady(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_HIGH_RES_MODE:
            Fxls8974_State = STATE_FXLS8974_READ_DATA;
            AccelReadOneShot();                     // Entry Function.
            break;

        case STATE_FXLS8974_READ_DATA:
            if (IsDisableRequested() == TRUE)
            {
                Fxls8974_State = STATE_FXLS8974_LOW_POWER_MODE;
                AccelDisable();                     // Entry Function.
            }
            else
            {
                StartTimer();                       // Transition action function.
                Fxls8974_State = STATE_FXLS8974_READ_DATA;
                AccelReadOneShot();                 // Entry Function.
            }
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Disable Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDisable(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_HIGH_RES_MODE:
        case STATE_FXLS8974_APP_BUFFER_FULL:
            Fxls8974_State = STATE_FXLS8974_LOW_POWER_MODE;
            AccelDisable();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Enable Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventEnable(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_LOW_POWER_MODE:
            FXLS8974__FlushData();                  // Transition action function.
            Fxls8974_State = STATE_FXLS8974_RESTART;
            StartTimer();                           // Entry Function.
            AccelHwInit();                          // Entry Function.
            AccelEnable();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the I2c Ready Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventI2cReady(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_RESTART:
            if (SelfTestEnabled() == TRUE)
            {
                Fxls8974_State = STATE_FXLS8974_SELF_TEST;
                ConfigSelfTest();                   // Entry Function.
            }
            else
            {
                Fxls8974_State = STATE_FXLS8974_ACTIVATE;
                StartTimer();                       // Entry Function.
                AccelActivate();                    // Entry Function.
            }
            break;

        case STATE_FXLS8974_SELF_TEST:
            Fxls8974_State = STATE_FXLS8974_ACTIVATE;
            StartTimer();                           // Entry Function.
            AccelActivate();                        // Entry Function.
            break;

        case STATE_FXLS8974_ACTIVATE:
            Fxls8974_State = STATE_FXLS8974_HIGH_RES_MODE;
            StartTimer();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Read Complete Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReadComplete(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_READ_DATA:
            if (ReadTemp() == TRUE)
            {
                Fxls8974_State = STATE_FXLS8974_READ_TEMP;
                StartTimer();                       // Entry Function.
                AccelReadTemp();                    // Entry Function.
            }
            else
            {
                Fxls8974_State = STATE_FXLS8974_HIGH_RES_MODE;
                StartTimer();                       // Entry Function.
            }
            break;

        case STATE_FXLS8974_READ_TEMP:
            Fxls8974_State = STATE_FXLS8974_HIGH_RES_MODE;
            StartTimer();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timeout Event for the Fxls8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimeout(void)
{
    switch (Fxls8974_State)
    {
        case STATE_FXLS8974_RESTART:
            if (IsDisableRequested() == TRUE)
            {
                Fxls8974_State = STATE_FXLS8974_LOW_POWER_MODE;
                AccelDisable();                     // Entry Function.
            }
            else
            {
                Fxls8974_State = STATE_FXLS8974_RESTART;
                StartTimer();                       // Entry Function.
                AccelHwInit();                      // Entry Function.
                AccelEnable();                      // Entry Function.
            }
            break;

        case STATE_FXLS8974_HIGH_RES_MODE:
        case STATE_FXLS8974_READ_DATA:
        case STATE_FXLS8974_READ_TEMP:
            Fxls8974_State = STATE_FXLS8974_RESTART;
            StartTimer();                           // Entry Function.
            AccelHwInit();                          // Entry Function.
            AccelEnable();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Fxls8974 state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void Fxls8974Execute(void)
{
    switch(Fxls8974_State)
    {
        case STATE_FXLS8974_BOOT_DELAY:
            WaitForBootComplete();                  // Do function.
            break;

        case STATE_FXLS8974_LOW_POWER_MODE:
            CheckForEnable();                       // Do function.
            break;

        case STATE_FXLS8974_RESTART:
        case STATE_FXLS8974_SELF_TEST:
        case STATE_FXLS8974_ACTIVATE:
            WaitForI2cReady();                      // Do function.
            break;

        case STATE_FXLS8974_HIGH_RES_MODE:
            WaitForDataReady();                     // Do function.
            break;

        case STATE_FXLS8974_READ_DATA:
            WaitForData();                          // Do function.
            break;

        case STATE_FXLS8974_READ_TEMP:
            WaitForTempData();                      // Do function.
            break;

        case STATE_FXLS8974_APP_BUFFER_FULL:
            CheckAppBuffer();                       // Do function.
            break;

        default:
            // There is nothing to do for all other values of Fxls8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Fxls8974 state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void Fxls8974Initialize(void)
{
    Fxls8974_State = STATE_FXLS8974_BOOT_DELAY;
    AccelHwInit();                                  // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif // (HBL_LLI_NUM_ACCELEROMETER > 0)
