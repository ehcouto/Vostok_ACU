/**
 *  @file
 *
 *  @brief  Module used to discover the accelerometer that is in the product.
 *          Discovery is based on the WHO_AM_I register (or similar register).
 *          The module cycles through a list of devices, given a register to read and expected device ID.
 *          If the device ID matches the expected value, the module execution is complete.
 *          If the device ID does not match the expected value, the module attempts to read the next device ID in the list.
 *            **NOTE** If no known device ID is read, this module will continue in a loop until it finds a known device ID.
 *
 *  @details
     @startuml{AccelDiscover_sm.png}
        title AccelDiscover State-Machine
        ' Suppress transition checks

        [*] --> INTERFACE_MODE

        INTERFACE_MODE: entry / StartTimer() \n \t SetInterfaceMode()
        INTERFACE_MODE : do / WaitForTimer
        INTERFACE_MODE -->  BOOT_MODE_LOW : evTimerExpired

        BOOT_MODE_LOW : entry / StartTimer() \n \t SetBootModeLow()
        BOOT_MODE_LOW : do / WaitForTimer
        BOOT_MODE_LOW -->  BOOT_MODE_HIGH : evTimerExpired

        BOOT_MODE_HIGH : entry / StartTimer() \n \t SetBootModeHigh()
        BOOT_MODE_HIGH : do / WaitForTimer
        BOOT_MODE_HIGH -->  BOOT_DELAY : evTimerExpired

        BOOT_DELAY : entry / AccelHwInit()
        BOOT_DELAY : do / WaitForBootComplete()
        BOOT_DELAY --> IDENTIFY : evBootComplete

        IDENTIFY : entry / StartTimer()\n\t AccelReadIdentity()
        IDENTIFY : do / WaitForIdentity()
        IDENTIFY --> BOOT_DELAY : evTimeout
        IDENTIFY --> BOOT_DELAY : evWrongIdentity
        IDENTIFY --> Complete : evIdentityReceived

        ' The purpose of the COMPLETE state is to have a status to report that the device has been discovered

    @enduml
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#include "AccelDiscover.h"
#include "FXLS8974.h"   // Required for access to pin configuration in prm.h file
#include "Gpio.h"
#include "I2c.h"
#include "Spi.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Accel Discover State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Accel Discover state machine.
typedef enum ACCEL_DISCOVER_STATE_ENUM
{
    STATE_ACCEL_DISCOVER_NONE           = 0,

    STATE_ACCEL_DISCOVER_INTERFACE_MODE = 1,        //!< (initial_state)
    STATE_ACCEL_DISCOVER_BOOT_MODE_LOW  = 2,
    STATE_ACCEL_DISCOVER_BOOT_MODE_HIGH = 3,
    STATE_ACCEL_DISCOVER_BOOT_DELAY     = 4,
    STATE_ACCEL_DISCOVER_IDENTIFY       = 5,
    STATE_ACCEL_DISCOVER_COMPLETE       = 6,

    STATE_ACCEL_DISCOVER_END            = 7
} ACCEL_DISCOVER_STATE_TYPE;

//! The state variable for the Accel Discover state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static ACCEL_DISCOVER_STATE_TYPE Accel_Discover_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void AccelHwInit(void);
static void AccelReadIdentity(void);
static void SetBootModeHigh(void);
static void SetBootModeLow(void);
static void SetInterfaceMode(void);
static void StartTimer(void);
static void WaitForBootComplete(void);
static void WaitForIdentity(void);
static void WaitForTimer(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void AccelDiscoverExecute(void);
static void AccelDiscoverInitialize(void);
static void EventBootComplete(void);
static void EventIdentityReceived(void);
static void EventTimeout(void);
static void EventTimerExpired(void);
static void EventWrongIdentity(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! The time required for the Accelerometer to initialize itself.
//! NOTE: This value is based on the call rate of LISxDH__Background()
#define ACCEL_TIME_BOOT_UP          20U

//! The timeout after which the module decides that the I2C bus will never become available.
//! NOTE: This value is based on the call rate of LISxDH__Background()
#define I2C_TIMEOUT_READY           100U // 100 * 200us = 20ms

//! Number of accelerometers that can be discovered by this module
#define NUM_DEVICES 2U

//! The command sent to the accelerometer to request the device identification data
#if (SPI_ENABLED == 1)
// RW bit shall be 1 for a read command; MS bit shall be 0 to read only 1 register
static const uint8 ACCEL_IDENTIFY_REGISTERS[NUM_DEVICES] =
{
    0x93,   // NXP FXLS8974
    0x8F    // ST LISxDH
};
#else
//! WARNING! The order of content in this list must match the order in ACCEL_IDENTIFY_REGISTERS (this could be refactored to remove this dependency)
static const uint8 ACCEL_IDENTIFY_REGISTERS[NUM_DEVICES] =
{
    0x13,   // NXP FXLS8974
    0x0F    // ST LISxDH
};
#endif

//! List of expected device IDs
//! WARNING! The order of content in this list must match the order in ACCEL_IDENTIFY_REGISTERS (this could be refactored to remove this dependency)
static const uint8 ACCEL_DEVICE_IDS[NUM_DEVICES] =
{
    0x86,   // NXP FXLS8974
    0x33    // ST LISxDH
};

//! Module data
static struct
{
    uint8 Address;          // I2C address (if applicable)
    uint8 Device_Index;     // Tracks the device when attempting communication; 0: NXP FXLS8974, 1: ST LISxDH
    uint8 Id;               // Unique identification number of the device
    uint8 Port;             // I2C_ENUM_TYPE or SPI_ENUM_DEF
    uint8 Timer;
}AccelDiscover;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initializes the module variables
 */
void AccelDiscover__Initialize(uint8 port, uint8 address)
{
    AccelDiscover.Address = address;
    AccelDiscover.Port = port;

    // Chip Select pin
    Gpio__PinConfig(SPI_CS_PORT, SPI_CS_PIN, OUTPUT_PUSHPULL);
#if (SPI_ENABLED == 1)
    // Set Chip Select pin low for SPI communication
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);
#else
    // Set Chip Select pin high for I2C communication
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);
#endif

    AccelDiscoverInitialize();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 */
void AccelDiscover__Execute(void)
{
    // Update the local time keeping variable.
    AccelDiscover.Timer++;

    // Run the state machine.
    AccelDiscoverExecute();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Returns the unique accelerometer device ID
 *  @retval 0x33(51d) : ST LISxDH
 *  @retval 0x86(134d): NXP FXLS8974
 */
uint8 AccelDiscover__GetId(void)
{
    return (AccelDiscover.Id);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Returns the completion of the auto discovery
 *  @return TRUE if a known accelerometer has been discovered, else FALSE
 */
BOOL_TYPE AccelDiscover__IsComplete(void)
{
    return((Accel_Discover_State == STATE_ACCEL_DISCOVER_COMPLETE) ? TRUE : FALSE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the microcontroller peripherals used by the module
 */
static void AccelHwInit(void)
{
#if (SPI_ENABLED == 1)

    Spi__Initialize((SPI_ENUM_DEF)AccelDiscover.Port);

    if (AccelDiscover.Device_Index == 0U)    // NXP
    {
        Spi__SetConfiguration((SPI_ENUM_DEF)AccelDiscover.Port, SPI_POLARITY_SCK_IDLE_0, SPI_EDGE_FIRST, SPI_ODER_MSB_FIRST);
    }
    else    // ST
    {
        Spi__SetConfiguration((SPI_ENUM_DEF)AccelDiscover.Port, SPI_POLARITY_SCK_IDLE_1, SPI_EDGE_SECOND, SPI_ODER_MSB_FIRST);
    }

    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high (reset CS)
#else
    // Setup the I2C peripheral.
    // NOTE: The I2C module includes the I2C R/W bit as part of the address.
    I2c__Initialize((I2C_ENUM_TYPE)AccelDiscover.Port, (I2C_SPEED_TYPE)I2C_400KHZ, I2C_ADDR_7BITS, (AccelDiscover.Address << 1));
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a command to the accelerometer to read the WHO_AM_I register that identifies the device
 */
static void AccelReadIdentity(void)
{
#if (SPI_ENABLED == 1)
    uint8 buffer[3];
    buffer[0] = ACCEL_IDENTIFY_REGISTERS[AccelDiscover.Device_Index];
    buffer[1] = 0xFF;                                     // Unused byte to read the byte of register data from the accelerometer
    buffer[2] = 0xFF;                                     // Unused byte to read the byte of register data from the accelerometer
    Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, FALSE);       // Pull chip select low during transmission

    uint8 num_write_bytes = 2;   // Default value satisfies LISxDH accelerometer
    if (AccelDiscover.Device_Index == 0U)    // NXP
    {
        num_write_bytes = 3;
    }
    (void)Spi__Write((SPI_ENUM_DEF)AccelDiscover.Port, buffer, num_write_bytes);
#else
    uint8 buffer[1];
    buffer[0] = ACCEL_IDENTIFY_REGISTERS[AccelDiscover.Device_Index];

    I2c__WriteThenRead((I2C_ENUM_TYPE)AccelDiscover.Port, buffer, 1, 1);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Pin setting required for FXLS8974 accelerometer
 */
static void SetBootModeHigh(void)
{
    Gpio__PinWrite(BOOT_MODE_PORT, BOOT_MODE_PIN, TRUE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Pin setting required for FXLS8974 accelerometer
 */
static void SetBootModeLow(void)
{
    Gpio__PinConfig(BOOT_MODE_PORT, BOOT_MODE_PIN, OUTPUT_PUSHPULL);
    Gpio__PinWrite(BOOT_MODE_PORT, BOOT_MODE_PIN, FALSE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Pin setting required for FXLS8974 accelerometer to use I2C/SPI communication
 */
static void SetInterfaceMode(void)
{
    Gpio__PinConfig(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, OUTPUT_PUSHPULL);
#if (SPI_ENABLED == 1)
    Gpio__PinWrite(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, TRUE);                 // Pin high for SPI communication
#else
    Gpio__PinWrite(INTERFACE_SELECT_PORT, INTERFACE_SELECT_PIN, FALSE);                // Pin low for I2C communication
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start the timer to measure a potential timeout.
 */
static void StartTimer(void)
{
    AccelDiscover.Timer = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForBootComplete(void)
{
    if (AccelDiscover.Timer > ACCEL_TIME_BOOT_UP)
    {
        EventBootComplete();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForIdentity(void)
{
    // Check if the accelerometer data has been received.
#if (SPI_ENABLED == 1)
    if (Spi__GetStatus((SPI_ENUM_DEF)AccelDiscover.Port, SPI_STATUS_STATE) == (uint16)SPI_STATE_IDLE)    // Bus is ready when there are 0 bytes to be processed (read or write)
    {
        Gpio__PinWrite(SPI_CS_PORT, SPI_CS_PIN, TRUE);           // Pull chip select high now that transmission is complete
#else
    if (I2c__GetStatus((I2C_ENUM_TYPE)AccelDiscover.Port, I2C_STATUS_STATE) == I2C_STATE_IDLE)
    {
#endif
        // Read the data that was received
        uint8 data[3];
#if (SPI_ENABLED == 1)
        uint8 num_read_bytes = 2;               // Default value satisfies LISxDH accelerometer
        if (AccelDiscover.Device_Index == 0U)    // NXP
        {
            num_read_bytes = 3;
        }

        if (Spi__Read((SPI_ENUM_DEF)AccelDiscover.Port, (void*)data, num_read_bytes))
#else
        if (I2c__Read((I2C_ENUM_TYPE)AccelDiscover.Port, (void*)data, sizeof(data)))
#endif
        {
            // Check if the device ID received matches the expected ID
#if (SPI_ENABLED == 1)
            uint8 data_byte_position = 1;           // Default value satisfies LISxDH accelerometer
            if (AccelDiscover.Device_Index == 0U)   // NXP
            {
                data_byte_position = 2;
            }

            if (data[data_byte_position] == ACCEL_DEVICE_IDS[AccelDiscover.Device_Index])
#else
            if (data[0] == ACCEL_DEVICE_IDS[AccelDiscover.Device_Index])
#endif
            {
#if (SPI_ENABLED == 1)
                AccelDiscover.Id = data[data_byte_position];
#else
                AccelDiscover.Id = data[0];
#endif
                EventIdentityReceived();
            }
            else
            {
                // Try the next device
                AccelDiscover.Device_Index++;
                if (AccelDiscover.Device_Index >= NUM_DEVICES)
                {
                    AccelDiscover.Device_Index = 0;
                }
                EventWrongIdentity();
            }
        }
    }

    if (AccelDiscover.Timer > I2C_TIMEOUT_READY)
    {
        EventTimeout();

        // Try the next device
        AccelDiscover.Device_Index++;
        if (AccelDiscover.Device_Index >= NUM_DEVICES)
        {
            AccelDiscover.Device_Index = 0;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForTimer(void)
{
    if (AccelDiscover.Timer > 100U)
    {
        EventTimerExpired();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Accel Discover State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Accel Discover state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AccelDiscoverExecute(void)
{
    switch(Accel_Discover_State)
    {
        case STATE_ACCEL_DISCOVER_INTERFACE_MODE:
        case STATE_ACCEL_DISCOVER_BOOT_MODE_LOW:
        case STATE_ACCEL_DISCOVER_BOOT_MODE_HIGH:
            WaitForTimer();                         // Do function.
            break;

        case STATE_ACCEL_DISCOVER_BOOT_DELAY:
            WaitForBootComplete();                  // Do function.
            break;

        case STATE_ACCEL_DISCOVER_IDENTIFY:
            WaitForIdentity();                      // Do function.
            break;

        default:
            // There is nothing to do for all other values of Accel_Discover_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Accel Discover state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AccelDiscoverInitialize(void)
{
    Accel_Discover_State = STATE_ACCEL_DISCOVER_INTERFACE_MODE;
    StartTimer();                                   // Entry Function.
    SetInterfaceMode();                             // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Boot Complete Event for the Accel Discover state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBootComplete(void)
{
    switch (Accel_Discover_State)
    {
        case STATE_ACCEL_DISCOVER_BOOT_DELAY:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_IDENTIFY;
            StartTimer();                           // Entry Function.
            AccelReadIdentity();                    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Discover_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Identity Received Event for the Accel Discover state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventIdentityReceived(void)
{
    switch (Accel_Discover_State)
    {
        case STATE_ACCEL_DISCOVER_IDENTIFY:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_COMPLETE;
            break;

        default:
            // Event is ignored for all other values of Accel_Discover_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timeout Event for the Accel Discover state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimeout(void)
{
    switch (Accel_Discover_State)
    {
        case STATE_ACCEL_DISCOVER_IDENTIFY:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_BOOT_DELAY;
            AccelHwInit();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Discover_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timer Expired Event for the Accel Discover state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimerExpired(void)
{
    switch (Accel_Discover_State)
    {
        case STATE_ACCEL_DISCOVER_INTERFACE_MODE:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_BOOT_MODE_LOW;
            StartTimer();                           // Entry Function.
            SetBootModeLow();                       // Entry Function.
            break;

        case STATE_ACCEL_DISCOVER_BOOT_MODE_LOW:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_BOOT_MODE_HIGH;
            StartTimer();                           // Entry Function.
            SetBootModeHigh();                      // Entry Function.
            break;

        case STATE_ACCEL_DISCOVER_BOOT_MODE_HIGH:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_BOOT_DELAY;
            AccelHwInit();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Discover_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Wrong Identity Event for the Accel Discover state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventWrongIdentity(void)
{
    switch (Accel_Discover_State)
    {
        case STATE_ACCEL_DISCOVER_IDENTIFY:
            Accel_Discover_State = STATE_ACCEL_DISCOVER_BOOT_DELAY;
            AccelHwInit();                          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Discover_State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif // (HBL_LLI_NUM_ACCELEROMETER > 0)
