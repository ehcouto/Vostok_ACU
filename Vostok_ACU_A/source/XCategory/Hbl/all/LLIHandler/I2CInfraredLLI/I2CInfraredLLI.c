/**
 * @file
 * @brief       Low-Level Input for the Melexis MLX90621 16x4 infrared array sensor.
 *
 * @details     This LLI initializes and reads the MLX90621 16x4 infrared array sensor.
 *              For details of the implementation, refer to the MLX90621 data sheet.
 *
 *              This LLI performs the following actions:
 *                  - Read LLI configuration from the setting file
 *                  - Initialization of the MLX90621
 *                  - Read the MLX90621 EEPROM data
 *                  - Write oscillator trimming value
 *                  - Write device configuration value
 *                  - Read the raw (uncompensated) 16x4 infrared array data
 *
    @startuml
        title IR LLI State Machine
        ' Use state = I2c_Infrared_Lli.State
        [*] --> SetupEepromRead

        SetupEepromRead : do / ConfigureI2CForEeprom()
        SetupEepromRead --> ReadEeprom : evProceed
        SetupEepromRead -> [*] : evFail

        ReadEeprom : entry / StartTimer()\n\t RequestEepromRead()
        ReadEeprom : do / DoEepromRead()
        ReadEeprom --> ReadEeprom : evReadNextSection
        ReadEeprom --> SetupEepromRead : evFail
        ReadEeprom --> SetupTrimValueWrite : evReadComplete

        SetupTrimValueWrite : do / ConfigureI2CForRam()
        SetupTrimValueWrite --> WriteTrimValue : evProceed
        SetupTrimValueWrite --> SetupTrimValueWrite : evFail

        WriteTrimValue : entry / StartTimer()\n\t WriteOscillatorTrim()
        WriteTrimValue : do / WaitForI2cIdle()
        WriteTrimValue --> ReadTrimValue : evWriteComplete
        WriteTrimValue --> SetupTrimValueWrite : evFail

        ReadTrimValue : entry / StartTimer()\n\t RequestOscTrimRead()
        ReadTrimValue : do / DoOscTrimRead()
        ReadTrimValue --> SetupConfigValueWrite : evReadComplete
        ReadTrimValue --> SetupTrimValueWrite : evFail

        SetupConfigValueWrite : do / ConfigureI2CForRam()
        SetupConfigValueWrite --> WriteConfigValue : evProceed
        SetupConfigValueWrite --> SetupConfigValueWrite : evFail

        WriteConfigValue : entry / StartTimer()\n\t WriteConfiguration()
        WriteConfigValue : do / WaitForI2cIdle()
        WriteConfigValue --> ReadConfigValue : evWriteComplete
        WriteConfigValue --> SetupConfigValueWrite : evFail

        ReadConfigValue : entry / StartTimer()\n\t RequestStatusRead()
        ReadConfigValue : do / DoConfigRead()
        ReadConfigValue --> WaitForNextStep : evReadComplete
        ReadConfigValue --> SetupConfigValueWrite : evFail

        WaitForNextStep : do / WaitForNextStep()
        WaitForNextStep --> StartStep : evStart

        StartStep : entry / StartAcquisition()
        StartStep : do / WaitForI2cIdle()
        StartStep --> ReadStatus : evWriteComplete
        StartStep --> SetupTrimValueWrite : evFail

        ReadStatus : entry / RequestStatusRead()
        ReadStatus : do / DoStatusRead()
        ReadStatus --> SetupEepromRead : evPowerOnReset
        ReadStatus --> ReadStatus : evReadComplete
        ReadStatus --> ReadPixels : evDataReady
        ReadStatus --> SetupTrimValueWrite : evFail

        ReadPixels : entry / RequestPixelsRead()
        ReadPixels : do / DoPixelsRead()
        ReadPixels --> DataReady : evReadComplete
        ReadPixels --> SetupTrimValueWrite : evFail

        DataReady : entry / SetDataReadyFlag()
        'DataReady : do / EventGIRead()
        DataReady --> WaitForNextStep : evGIRead
    @enduml
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "I2CInfraredLLI.h"

#if (HBL_LLI_NUM_I2C_INFRARED_SENSOR > 0)
CT_ASSERT(HBL_LLI_NUM_I2C_INFRARED_SENSOR == 1);
// -- Other Modules --
#include "I2c.h"
#include "Millis.h"
#include <string.h>


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Ir Lli State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Ir Lli state machine.
typedef enum IR_LLI_STATE_ENUM
{
    STATE_IR_LLI_NONE                     = 0,

    STATE_IR_LLI_SETUP_EEPROM_READ        = 1,      //!< (initial_state)
    STATE_IR_LLI_READ_EEPROM              = 2,
    STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE   = 3,
    STATE_IR_LLI_WRITE_TRIM_VALUE         = 4,
    STATE_IR_LLI_READ_TRIM_VALUE          = 5,
    STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE = 6,
    STATE_IR_LLI_WRITE_CONFIG_VALUE       = 7,
    STATE_IR_LLI_READ_CONFIG_VALUE        = 8,
    STATE_IR_LLI_WAIT_FOR_NEXT_STEP       = 9,
    STATE_IR_LLI_START_STEP               = 10,
    STATE_IR_LLI_READ_STATUS              = 11,
    STATE_IR_LLI_READ_PIXELS              = 12,
    STATE_IR_LLI_DATA_READY               = 13,

    STATE_IR_LLI_END                      = 14
} IR_LLI_STATE_TYPE;

//! The transition variable for the Ir Lli state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Ir_Lli_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void ConfigureI2CForEeprom(void);
static void ConfigureI2CForRam(void);
static void DoConfigRead(void);
static void DoEepromRead(void);
static void DoOscTrimRead(void);
static void DoPixelsRead(void);
static void DoStatusRead(void);
static void RequestEepromRead(void);
static void RequestOscTrimRead(void);
static void RequestPixelsRead(void);
static void RequestStatusRead(void);
static void SetDataReadyFlag(void);
static void StartAcquisition(void);
static void StartTimer(void);
static void WaitForI2cIdle(void);
static void WaitForNextStep(void);
static void WriteConfiguration(void);
static void WriteOscillatorTrim(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDataReady(void);
static void EventFail(void);
static void EventGIRead(void);
static void EventPowerOnReset(void);
static void EventProceed(void);
static void EventReadComplete(void);
static void EventReadNextSection(void);
static void EventStart(void);
static void EventWriteComplete(void);
static void IrLliExecute(void);
static void IrLliInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

#define CONFIG_VERIFIABLE_FIELDS_MASK 0x5CFF        //!< Bitmask of verifiable bits in the configuration register.
#define I2C_NOT_IDLE_MS_TIMEOUT 8                   //!< The maximum milliseconds for writing a message on the I2C bus.
#define MLX_EEPROM_DEVICE_ADDRESS (2 * 0x50)        //!< The I2C slave address for the Melexis 90621 device EEPROM.
#define MLX_RAM_DEVICE_ADDRESS (2 * 0x60)           //!< The I2C slave address for the Melexis 90621 device RAM.
#define MODULE_NAME MODULE_I2C_INFRARED_LLI         //!< The name of this module for use by the Log module.
#define RAM_OSC_TRIM_READ_ADDRESS 0x93              //!< Address of the sensor's oscillator trim register.
//! The number of bytes read when reading the oscillator trim register.
#define RAM_OSC_TRIM_READ_SIZE (sizeof(I2c_Infrared_Lli.Config.Osc_Trim))
#define RAM_PIXEL_READ_ADDRESS 0                    //!< Address of the sensor's pixel data registers.
//! The number of bytes read when reading the IR pixel data.
#define RAM_PIXEL_READ_SIZE (sizeof(I2c_Infrared_Lli.Sensor.Pixel) + sizeof(I2c_Infrared_Lli.Sensor.PTAT) + sizeof(I2c_Infrared_Lli.Sensor.V_CP))
#define RAM_STATUS_READ_ADDRESS 0x92                //!< Address of the sensor's status register.
//! The number of bytes read when reading the configuration or status register.
#define RAM_STATUS_READ_SIZE (sizeof(MELEXIS_90621_CONFIG_REG_STRUCT_TYPE))


// -- Private Enumerated Constant Declarations --

//! Refresh rates for the IR sensor.
typedef enum IR_REFRESH_RATE_ENUM
{
    IR_REFRESH_512_HZ               = 5,
    IR_REFRESH_256_HZ               = 6,
    IR_REFRESH_128_HZ               = 7,
    IR_REFRESH_64_HZ                = 8,
    IR_REFRESH_32_HZ                = 9,
    IR_REFRESH_16_HZ                = 10,
    IR_REFRESH_8_HZ                 = 11,
    IR_REFRESH_4_HZ                 = 12,
    IR_REFRESH_2_HZ                 = 13,
    IR_REFRESH_1_HZ                 = 14,
    IR_REFRESH_HALF_HZ              = 15
} IR_REFRESH_RATE_TYPE;

//! Commands accepted by the Melexis 90621 sensor.
typedef enum MELEXIS_CMD_ENUM
{
    MLX_CMD_READ_EEPROM         = 0,
    MLX_CMD_START_STEP_LO_BYTE  = 1,
    MLX_CMD_READ_RAM            = 2,
    MLX_CMD_WRITE_CONFIG        = 3,
    MLX_CMD_WRITE_OSC_TRIM      = 4,
    MLX_CMD_START_STEP_HI_BYTE  = 8
} MELEXIS_CMD_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_I2C_INFRARED_LLI_LOG_MESSAGE_ID_ENUM
{
    ERROR_COMMUNICATION  = 1,
    ERROR_DATA_STUCK,
} MODULE_I2C_INFRARED_LLI_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! A data structure to organize memory transfer instructions.
typedef struct I2C_MEMORY_TRANSFER_STRUCT
{
    void* Destination;
    uint8 Read_Address;
    uint8 Read_Size;
} I2C_MEMORY_TRANSFER_TYPE;

//! The configuration data structure of the Melexis 90621 Sensor.
//! These values are stored in EEPROM at 0xF5 to 0xF7.
typedef /*NOT PACKED*/ struct MELEXIS_90621_CONFIG_REG_STRUCT
{
    uint16 IR_Refresh_Rate      : 4;
    uint16 Bit_Resolution       : 2;
    uint16 Step_Mode            : 1;
    uint16 Sleep_Mode           : 1;
    uint16 Reserved1            : 1;
    uint16 Measuring            : 1;
    uint16 Configuration_Loaded : 1;
    uint16 I2C_400KHz_Limit     : 1;
    uint16 EEPROM_Disabled      : 1;
    uint16 Reserved2            : 3;
} MELEXIS_90621_CONFIG_REG_STRUCT_TYPE;

//! A union that holds the configuration data as fields and as a word.
typedef union MELEXIS_90621_CONFIG_REG_UNION
{
    uint16 Whole;
    MELEXIS_90621_CONFIG_REG_STRUCT_TYPE Field;
} MELEXIS_90621_CONFIG_REG_TYPE;

//! A data structure that holds the Melexis 90621 sensor configuration.
typedef struct MELEXIS_90621_CONFIG_STRUCT
{
    MELEXIS_90621_CONFIG_REG_TYPE Cfg_Reg;
    uint16 Osc_Trim;
} MELEXIS_90621_CONFIG_TYPE;

//! The setting file configuration data structure used by the I2CInfraredLLI module.
typedef struct SF_I2C_INFRARED_LLI_STRUCT
{
    uint16 Step_Period_Milliseconds;
    I2C_ENUM_TYPE I2c_Channel;
    uint8 IR_Refresh_Rate       : 4;
    uint8 Bit_Resolution        : 2;
} SF_I2C_INFRARED_LLI_TYPE;

//! The main structure for variables used by the I2CInfraredLLI module.
typedef struct I2C_INFRARED_LLI_DATA_STRUCT
{
    //! EEPROM data from Melexis 90621 sensors. Must be first in data structure to preserve memory alignment.
    MELEXIS_90621_EEPROM_TYPE Eeprom;

    //! The setting file configuration for the LLI.
    const SF_I2C_INFRARED_LLI_TYPE* Sf;

    //! Data from Melexis 90621 sensors.
    MELEXIS_90621_DATA_TYPE Sensor;

    //! The configuration data read from the Melexis 90621 Sensor EEPROM.
    MELEXIS_90621_CONFIG_TYPE Config;

    //! Timer to measure times spent in states.
    uint16 Start_Milliseconds;

    //! The number of sensors allocated for this module.
    uint8 Allocation;

    //! Event flag that indicates that new data is ready.
    //! Event is synchronized to the I2CInfraredLLI__AsynchProcess() function.
    BOOL_TYPE Data_Ready;

    //! The state of the state machine.
    IR_LLI_STATE_TYPE State;

    //! The EEPROM read transfer step being taken in the I2C_READ_INSTRUCTIONS[] array.
    uint8 Transfer_Step;
} I2C_INFRARED_LLI_DATA_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Variable Definitions --

//! The data used by this module.
static I2C_INFRARED_LLI_DATA_TYPE I2c_Infrared_Lli;


// -- Private Flash-Based Constant Definitions --

//! Default hard-coded configuration to use before GESE is ready.
static const SF_I2C_INFRARED_LLI_TYPE I2C_INFRARED_LLI_HARD_CODED_CONFIG =
{
    500,                                            // Step_Period_Milliseconds
    I2C1,                                           // I2c_Channel
    IR_REFRESH_4_HZ,                                // IR_Refresh_Rate
    IR_RESOLUTION_18_BIT,                           // Bit_Resolution
};

//! A series of read instructions to read the Melexis 90621 sensor EEPROM.
//! This strategy allows the unused areas of the EEPROM to not consume RAM.
static const I2C_MEMORY_TRANSFER_TYPE I2C_READ_INSTRUCTIONS[] =
{
//  Destination                                     Read Address,   Read Size,
    &I2c_Infrared_Lli.Eeprom.Ai_Pixel_Offset[0],    0x00,           0x80,
    &I2c_Infrared_Lli.Eeprom.Sensitivity[0],        0x80,           0x40,
    &I2c_Infrared_Lli.Eeprom.A_Common_Offset,       0xD0,           0x18,
    &I2c_Infrared_Lli.Config,                       0xF5,           0x03
};


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static uint16 GetMilliseconds(void);
static BOOL_TYPE IsI2cIdle(void);
static BOOL_TYPE ReadMelexisValues(uint8 address, uint8 size);
static BOOL_TYPE WriteMelexisValue(MELEXIS_CMD_TYPE command, uint16 value, uint8 check);


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the I2CInfraredLLI module's variables.
 */
void I2CInfraredLLI__Initialize(void)
{
    memset(&I2c_Infrared_Lli, 0, sizeof(I2c_Infrared_Lli));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Allocate memory out of the pre-allocated RAM for the next LLI of this type.
 * @return The index to the new LLIs allocated memory.
 *         Returns HBL_INDEX_INVALID if there is no more memory reserved.
 */
uint8 I2CInfraredLLI__Allocate(void)
{
    uint8 index = HBL_INDEX_INVALID;
    if (I2c_Infrared_Lli.Allocation < HBL_LLI_NUM_I2C_INFRARED_SENSOR)
    {
        index = I2c_Infrared_Lli.Allocation;
        I2c_Infrared_Lli.Allocation++;
    }
    return index;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize an instance of the LLI by reading and updating HBL's data structure.
 * @param input = Pointer to the HBL data structure that references the LLI instance.
 * @return PASS if the initialization was successful; otherwise FAIL.
 */
PASS_FAIL_TYPE I2CInfraredLLI__InitializeInstance(HBL_LLI_TYPE* input)
{
    PASS_FAIL_TYPE result = FAIL;
    if (input != NULL)
    {
        if ((input->Parameter_Ptr.Data != NULL) && (input->Parameter_Ptr.Length > 2))
        {
            I2c_Infrared_Lli.Sf = (SF_I2C_INFRARED_LLI_TYPE*)(void*)&input->Parameter_Ptr.Data[2];
        }
        else
        {
            I2c_Infrared_Lli.Sf = &I2C_INFRARED_LLI_HARD_CODED_CONFIG;
        }
        IrLliInitialize();
        if (I2c_Infrared_Lli.State != STATE_IR_LLI_NONE)
        {
            I2c_Infrared_Lli.Sensor.Eeprom = &I2c_Infrared_Lli.Eeprom;
            I2c_Infrared_Lli.Sensor.LLI_Status = I2C_IR_LLI_INITIALIZING;
            result = PASS;
        }
    }
    return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the current data of the referenced LLI.
 * @param input = Pointer to the HBL data structure that references the LLI instance.
 * @return Pointer to the current data of the referenced LLI.
 */
void* I2CInfraredLLI__Get(HBL_LLI_TYPE* input)
{
    void* data = NULL;
    if ((input != NULL) &&
        (input->Data_Index == 0))
    {
        data = (void*)&I2c_Infrared_Lli.Sensor;
        if (I2c_Infrared_Lli.Sensor.LLI_Status == I2C_IR_LLI_DATA_READY)
        {
            EventGIRead();
        }
    }
    return data;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Background function for the referenced LLI.
 * Called every 5ms.
 * @param input = Pointer to the HBL data structure that references the LLI instance.
 * @return Returns TRUE if new data is available. Otherwise FALSE.
 */
BOOL_TYPE I2CInfraredLLI__AsynchProcess(HBL_LLI_TYPE* input)
{
    if (input->Data_Index < I2c_Infrared_Lli.Allocation)
    {
        IrLliExecute();
        switch (I2c_Infrared_Lli.State)
        {
            case STATE_IR_LLI_SETUP_EEPROM_READ:
            case STATE_IR_LLI_READ_EEPROM:
            case STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE:
            case STATE_IR_LLI_WRITE_TRIM_VALUE:
            case STATE_IR_LLI_READ_TRIM_VALUE:
            case STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE:
            case STATE_IR_LLI_WRITE_CONFIG_VALUE:
            case STATE_IR_LLI_READ_CONFIG_VALUE:
                I2c_Infrared_Lli.Sensor.LLI_Status = I2C_IR_LLI_INITIALIZING;
                break;

            case STATE_IR_LLI_WAIT_FOR_NEXT_STEP:
                I2c_Infrared_Lli.Sensor.LLI_Status = I2C_IR_LLI_WAITING_FOR_DATA;
                break;

            case STATE_IR_LLI_START_STEP:
            case STATE_IR_LLI_READ_STATUS:
            case STATE_IR_LLI_READ_PIXELS:
                I2c_Infrared_Lli.Sensor.LLI_Status = I2C_IR_LLI_ACQUIRING_DATA;
                break;

            case STATE_IR_LLI_DATA_READY:
                I2c_Infrared_Lli.Sensor.LLI_Status = I2C_IR_LLI_DATA_READY;
                break;

            default:
                I2c_Infrared_Lli.Sensor.LLI_Status = I2C_IR_LLI_ERROR;
                break;
        }
    }

    BOOL_TYPE data_ready = I2c_Infrared_Lli.Data_Ready;
    I2c_Infrared_Lli.Data_Ready = FALSE;
    return data_ready;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of milliseconds since the StartTimer() function was called.
 * @return The number of milliseconds since the StartTimer() function was called.
 */
static uint16 GetMilliseconds(void)
{
    return (uint16)(Millis__GetFreeRunningCounter() - I2c_Infrared_Lli.Start_Milliseconds);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine whether the I2C bus is idle and ready for the next message.
 * @return TRUE if the I2C bus is idle and ready for the next message; otherwise FALSE.
 */
static BOOL_TYPE IsI2cIdle(void)
{
    return (BOOL_TYPE)(I2c__GetStatus(I2c_Infrared_Lli.Sf->I2c_Channel, I2C_STATUS_STATE) == I2C_STATE_IDLE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read RAM data from the Melexis 90621 sensor.
 * @param address = The address of the first value to read.
 * @param size = The number of bytes to read. Must be an even number.
 * @return TRUE if request was successful.
 */
static BOOL_TYPE ReadMelexisValues(uint8 address, uint8 size)
{
    // Write buffer consists of address, step size, and number of values to read.
    uint8 write_buffer[] = { MLX_CMD_READ_RAM, address, ((size > 2) ? 1 : 0), size / sizeof(uint16) };
    return I2c__WriteThenRead(I2c_Infrared_Lli.Sf->I2c_Channel, write_buffer, sizeof(write_buffer), size);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Write a checked 16-bit value to a Melexis 90621 sensor RAM address.
 * The value is sent with redundant data to reduce the likelihood of communication failures.
 * @param command = The command to send to the Melexis 90621 sensor.
 * @param value = The 16-bit value to write.
 * @param check = Redundant data generator for better communication robustness.
 * @return TRUE if the message was successfully queued.
 */
static BOOL_TYPE WriteMelexisValue(MELEXIS_CMD_TYPE command, uint16 value, uint8 check)
{
    uint8 write_buffer[] =
    {
        (uint8)(LOBYTE(value) - check),
        (uint8)(LOBYTE(value)),
        (uint8)(HIBYTE(value) - check),
        (uint8)(HIBYTE(value))
    };
    return I2c__Write(I2c_Infrared_Lli.Sf->I2c_Channel, command, write_buffer, sizeof(write_buffer));
}


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Configure the I2C bus to access the slave address of the Melexis 90621 EEPROM device.
 */
static void ConfigureI2CForEeprom(void)
{
    // Setup the I2C peripheral to read the EEPROM device slave address.
    // NOTE: The I2C module includes the I2C R/W bit as part of the address.
    if (I2c__Initialize(I2c_Infrared_Lli.Sf->I2c_Channel, I2C_400KHZ, I2C_ADDR_7BITS, MLX_EEPROM_DEVICE_ADDRESS) == TRUE)
    {
        EventProceed();
    }
    else
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Configure the I2C bus to access the slave address of the Melexis 90621 RAM device.
 */
static void ConfigureI2CForRam(void)
{
    // Setup the I2C peripheral to write the Melexis 90621 RAM slave address.
    // NOTE: The I2C module includes the I2C R/W bit as part of the address.
    if (I2c__Initialize(I2c_Infrared_Lli.Sf->I2c_Channel, I2C_400KHZ, I2C_ADDR_7BITS, MLX_RAM_DEVICE_ADDRESS) == TRUE)
    {
        EventProceed();
    }
    else
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the configuration register and confirm that it was written correctly.
 * This function reads and processes the received data.
 * It assumes that the read request was already made.
 */
static void DoConfigRead(void)
{
    I2c_Infrared_Lli.Sensor.Resolution = IR_RESOLUTION_15_BIT;
    if (IsI2cIdle() == TRUE)
    {
        uint16 value;
        if (I2c__Read(I2c_Infrared_Lli.Sf->I2c_Channel, (void*)&value, sizeof(value)) == TRUE)
        {
            value &= CONFIG_VERIFIABLE_FIELDS_MASK;
            if (value == (I2c_Infrared_Lli.Config.Cfg_Reg.Whole & CONFIG_VERIFIABLE_FIELDS_MASK))
            {
                I2c_Infrared_Lli.Sensor.Resolution = (IR_BIT_RESOLUTION_TYPE)I2c_Infrared_Lli.Config.Cfg_Reg.Field.Bit_Resolution;
                EventReadComplete();
            }
            else
            {
                EventFail();
            }
        }
        else
        {
            EventFail();
        }
    }
    else if (GetMilliseconds() >= I2C_NOT_IDLE_MS_TIMEOUT)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Copy the next block of data received on I2C into the RAM copy of the Melexis 90621 Sensor EEPROM.
 * This function reads and processes the received data.
 * It assumes that the read request was already made.
 */
static void DoEepromRead(void)
{
    if (IsI2cIdle() == TRUE)
    {
        // Get the pointer to the RAM copy of the Melexis 90621 EEPROM.
        uint8* destination = I2C_READ_INSTRUCTIONS[I2c_Infrared_Lli.Transfer_Step].Destination;
        uint8 read_size = I2C_READ_INSTRUCTIONS[I2c_Infrared_Lli.Transfer_Step].Read_Size;
        if (I2c__Read(I2c_Infrared_Lli.Sf->I2c_Channel, destination, read_size) == TRUE)
        {
            I2c_Infrared_Lli.Transfer_Step++;
            if (I2c_Infrared_Lli.Transfer_Step >= ELEMENTS_IN_ARRAY(I2C_READ_INSTRUCTIONS))
            {
                EventReadComplete();
            }
            else
            {
                EventReadNextSection();
            }
        }
        else
        {
            EventFail();
        }
    }
    else if (GetMilliseconds() >= I2C_NOT_IDLE_MS_TIMEOUT)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the oscillator trim value and confirm that it was written correctly.
 * This function reads and processes the received data.
 * It assumes that the read request was already made.
 */
static void DoOscTrimRead(void)
{
    if (IsI2cIdle() == TRUE)
    {
        uint16 value = 0;
        if (I2c__Read(I2c_Infrared_Lli.Sf->I2c_Channel, (void*)&value, sizeof(value)) == TRUE)
        {
            if (value == I2c_Infrared_Lli.Config.Osc_Trim)
            {
                EventReadComplete();
            }
            else
            {
                EventFail();
            }
        }
        else
        {
            EventFail();
        }
    }
    else if (GetMilliseconds() >= I2C_NOT_IDLE_MS_TIMEOUT)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the pixel data from the Melexis 90621 sensor.
 * This function reads and processes the received data.
 * It assumes that the read request was already made.
 */
static void DoPixelsRead(void)
{
    if (IsI2cIdle() == TRUE)
    {
        if (I2c__Read(I2c_Infrared_Lli.Sf->I2c_Channel, (void*)I2c_Infrared_Lli.Sensor.Pixel, RAM_PIXEL_READ_SIZE) == TRUE)
        {
            EventReadComplete();
        }
        else
        {
            EventFail();
        }
    }
    else if (GetMilliseconds() >= I2c_Infrared_Lli.Sf->Step_Period_Milliseconds)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the status from the Melexis 90621 sensor.
 * This function reads and processes the received data.
 * It assumes that the read request was already made.
 */
static void DoStatusRead(void)
{
    if (IsI2cIdle() == TRUE)
    {
        // Get the status register from the Melexis 90621 sensor.
        if (I2c__Read(I2c_Infrared_Lli.Sf->I2c_Channel,
                      (void*)(&I2c_Infrared_Lli.Config.Cfg_Reg.Whole),
                      sizeof(I2c_Infrared_Lli.Config.Cfg_Reg.Whole)) == TRUE)
        {
            if (I2c_Infrared_Lli.Config.Cfg_Reg.Field.Configuration_Loaded == FALSE)
            {
                // The Melexis 90621 sensor experienced a reset.
                EventPowerOnReset();
            }
            else if (I2c_Infrared_Lli.Config.Cfg_Reg.Field.Measuring == FALSE)
            {
                // The Melexis 90621 sensor finished acquiring pixel data.
                EventDataReady();
            }
            else
            {
                // The Melexis 90621 sensor is still acquiring data.
                EventReadComplete();
            }
        }
        else
        {
            EventFail();
        }
    }
    else if (GetMilliseconds() >= I2c_Infrared_Lli.Sf->Step_Period_Milliseconds)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start reading the next block of EEPROM data on the I2C bus from the Melexis 90621 Sensor.
 */
static void RequestEepromRead(void)
{
    uint8 address = I2C_READ_INSTRUCTIONS[I2c_Infrared_Lli.Transfer_Step].Read_Address;
    uint8 size = I2C_READ_INSTRUCTIONS[I2c_Infrared_Lli.Transfer_Step].Read_Size;
    if (I2c__WriteThenRead(I2c_Infrared_Lli.Sf->I2c_Channel, &address, sizeof(address), size) == FALSE)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start reading the oscillator trim value from the Melexis 90621 Sensor.
 */
static void RequestOscTrimRead(void)
{
    if (ReadMelexisValues(RAM_OSC_TRIM_READ_ADDRESS, RAM_OSC_TRIM_READ_SIZE) == FALSE)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start reading the pixel data from the Melexis 90621 sensor.
 */
static void RequestPixelsRead(void)
{
    if (ReadMelexisValues(RAM_PIXEL_READ_ADDRESS, RAM_PIXEL_READ_SIZE) == FALSE)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start reading the status of the Melexis 90621 sensor.
 */
static void RequestStatusRead(void)
{
    if (ReadMelexisValues(RAM_STATUS_READ_ADDRESS, RAM_STATUS_READ_SIZE) == FALSE)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the data ready flag to let HBL know that new data is ready.
 * It will be cleared in the DO function.
 */
static void SetDataReadyFlag(void)
{
    I2c_Infrared_Lli.Data_Ready = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send the start command to start a new acquisition.
 */
static void StartAcquisition(void)
{
    uint8 data = MLX_CMD_START_STEP_HI_BYTE;
    if (I2c__Write(I2c_Infrared_Lli.Sf->I2c_Channel, MLX_CMD_START_STEP_LO_BYTE, &data, sizeof(data)) == FALSE)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start the timer.
 * Elapsed time is available from the GetMilliseconds() function.
 */
static void StartTimer(void)
{
    I2c_Infrared_Lli.Start_Milliseconds = (uint16)Millis__GetFreeRunningCounter();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for the I2C bus to become idle with a short timeout.
 * Timeout is based on the expected I2c access time.
 */
static void WaitForI2cIdle(void)
{
    if (IsI2cIdle() == TRUE)
    {
        EventWriteComplete();
    }
    else if (GetMilliseconds() >= I2C_NOT_IDLE_MS_TIMEOUT)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for time to start the next acquisition step.
 * The method used will run at the specified requested refresh rate or as fast as possible if the
 * requested refresh rate cannot be maintained.
 */
static void WaitForNextStep(void)
{
    uint16 elapsed = GetMilliseconds();
    uint16 step_ms = I2c_Infrared_Lli.Sf->Step_Period_Milliseconds;
    if (elapsed >= step_ms)
    {
        if (elapsed < 2 * step_ms)
        {
            I2c_Infrared_Lli.Start_Milliseconds += step_ms;
        }
        else
        {
            // Accommodate unrecoverable slip.
            StartTimer();
        }
        EventStart();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Write the configuration value to the Melexis 90621 Sensor.
 */
static void WriteConfiguration(void)
{
    // Write the trim value to the Melexis 90621 Sensor.
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.IR_Refresh_Rate = I2c_Infrared_Lli.Sf->IR_Refresh_Rate;
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.Bit_Resolution = I2c_Infrared_Lli.Sf->Bit_Resolution;
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.Step_Mode = TRUE;
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.Sleep_Mode = FALSE;
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.Configuration_Loaded = TRUE;
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.I2C_400KHz_Limit = TRUE;
    I2c_Infrared_Lli.Config.Cfg_Reg.Field.EEPROM_Disabled = FALSE;
    // 0x55 is the check byte used to send redundant data for error checking.
    if (WriteMelexisValue(MLX_CMD_WRITE_CONFIG, I2c_Infrared_Lli.Config.Cfg_Reg.Whole, 0x55) == FALSE)
    {
        EventFail();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Write the oscillator trim value to the Melexis 90621 Sensor.
 */
static void WriteOscillatorTrim(void)
{
    // Write the trim value to the Melexis 90621 Sensor.
    //  0xAA is the check byte used to send redundant data for error checking.
    if (WriteMelexisValue(MLX_CMD_WRITE_OSC_TRIM, I2c_Infrared_Lli.Config.Osc_Trim, 0xAA) == FALSE)
    {
        EventFail();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Ir Lli State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Data Ready Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDataReady(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_READ_STATUS:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_PIXELS;
            RequestPixelsRead();                    // Entry Function.
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fail Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFail(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_SETUP_EEPROM_READ:
            I2c_Infrared_Lli.State = STATE_IR_LLI_NONE; // Terminate Ir Lli state machine execution.
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_READ_EEPROM:
            I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_EEPROM_READ;
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE:
        case STATE_IR_LLI_WRITE_TRIM_VALUE:
        case STATE_IR_LLI_READ_TRIM_VALUE:
        case STATE_IR_LLI_START_STEP:
        case STATE_IR_LLI_READ_STATUS:
        case STATE_IR_LLI_READ_PIXELS:
            I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE;
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE:
        case STATE_IR_LLI_WRITE_CONFIG_VALUE:
        case STATE_IR_LLI_READ_CONFIG_VALUE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE;
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the GI Read Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventGIRead(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_DATA_READY:
            I2c_Infrared_Lli.State = STATE_IR_LLI_WAIT_FOR_NEXT_STEP;
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Power On Reset Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPowerOnReset(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_READ_STATUS:
            I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_EEPROM_READ;
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Proceed Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventProceed(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_SETUP_EEPROM_READ:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_EEPROM;
            Ir_Lli_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Ir_Lli_Transitioned == FALSE)
            {
                RequestEepromRead();                // Entry Function.
            }
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_WRITE_TRIM_VALUE;
            Ir_Lli_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Ir_Lli_Transitioned == FALSE)
            {
                WriteOscillatorTrim();              // Entry Function.
            }
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_WRITE_CONFIG_VALUE;
            Ir_Lli_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Ir_Lli_Transitioned == FALSE)
            {
                WriteConfiguration();               // Entry Function.
            }
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Read Complete Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReadComplete(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_READ_EEPROM:
            I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE;
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_READ_TRIM_VALUE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE;
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_READ_CONFIG_VALUE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_WAIT_FOR_NEXT_STEP;
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_READ_STATUS:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_STATUS;
            RequestStatusRead();                    // Entry Function.
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_READ_PIXELS:
            I2c_Infrared_Lli.State = STATE_IR_LLI_DATA_READY;
            SetDataReadyFlag();                     // Entry Function.
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Read Next Section Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventReadNextSection(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_READ_EEPROM:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_EEPROM;
            Ir_Lli_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Ir_Lli_Transitioned == FALSE)
            {
                RequestEepromRead();                // Entry Function.
            }
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStart(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_WAIT_FOR_NEXT_STEP:
            I2c_Infrared_Lli.State = STATE_IR_LLI_START_STEP;
            StartAcquisition();                     // Entry Function.
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Write Complete Event for the Ir Lli state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventWriteComplete(void)
{
    switch (I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_WRITE_TRIM_VALUE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_TRIM_VALUE;
            Ir_Lli_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Ir_Lli_Transitioned == FALSE)
            {
                RequestOscTrimRead();               // Entry Function.
            }
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_WRITE_CONFIG_VALUE:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_CONFIG_VALUE;
            Ir_Lli_Transitioned = FALSE;
            StartTimer();                           // Entry Function.
            if (Ir_Lli_Transitioned == FALSE)
            {
                RequestStatusRead();                // Entry Function.
            }
            Ir_Lli_Transitioned = TRUE;
            break;

        case STATE_IR_LLI_START_STEP:
            I2c_Infrared_Lli.State = STATE_IR_LLI_READ_STATUS;
            RequestStatusRead();                    // Entry Function.
            Ir_Lli_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Ir Lli state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void IrLliExecute(void)
{
    Ir_Lli_Transitioned = FALSE;
    switch(I2c_Infrared_Lli.State)
    {
        case STATE_IR_LLI_SETUP_EEPROM_READ:
            ConfigureI2CForEeprom();                // Do function.
            break;

        case STATE_IR_LLI_READ_EEPROM:
            DoEepromRead();                         // Do function.
            break;

        case STATE_IR_LLI_SETUP_TRIM_VALUE_WRITE:
        case STATE_IR_LLI_SETUP_CONFIG_VALUE_WRITE:
            ConfigureI2CForRam();                   // Do function.
            break;

        case STATE_IR_LLI_WRITE_TRIM_VALUE:
        case STATE_IR_LLI_WRITE_CONFIG_VALUE:
        case STATE_IR_LLI_START_STEP:
            WaitForI2cIdle();                       // Do function.
            break;

        case STATE_IR_LLI_READ_TRIM_VALUE:
            DoOscTrimRead();                        // Do function.
            break;

        case STATE_IR_LLI_READ_CONFIG_VALUE:
            DoConfigRead();                         // Do function.
            break;

        case STATE_IR_LLI_WAIT_FOR_NEXT_STEP:
            WaitForNextStep();                      // Do function.
            break;

        case STATE_IR_LLI_READ_STATUS:
            DoStatusRead();                         // Do function.
            break;

        case STATE_IR_LLI_READ_PIXELS:
            DoPixelsRead();                         // Do function.
            break;

        default:
            // There is nothing to do for all other values of I2c_Infrared_Lli.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Ir Lli state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void IrLliInitialize(void)
{
    I2c_Infrared_Lli.State = STATE_IR_LLI_SETUP_EEPROM_READ;
    Ir_Lli_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif      // #if (HBL_LLI_NUM_I2C_INFRARED_SENSOR > 0)
