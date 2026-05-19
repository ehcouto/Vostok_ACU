/**
 *  @file
 *  @defgroup    CLASS_B
 *  @brief            This module implements the interface to configuration, refresh and status
 *                          check of Unused pins, with ClassB requirements.
 *
 *  @details         Handles a configuration, refresh and status check of Unused pins.
 *                      It's adding Class B requirement.
 *                      SRUnusedPinsConfig__Handler5ms running from Asynch task checks current
 *                      status & configuration of pins. If any pin status configuration mismatched
 *                      it will trigger SR Exception. Execution of SRUnusedPinsConfig__Handler5ms
 *                      is monitored by SRUnusedPinsConfig__Monitor running at 100mS. If
 *                      execution of SRUnusedPinsConfig__Handler5ms goes beyond expected
 *                      limit it will trigger SR Exception.
 *
 *  @copyright    Copyright 2022 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------

#include "SRSystemConfig.h"

#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)

// -- This Module --
#include "SRUnusedPinsConfig.h"
#include "SRUnusedPinsConfig_prv.h"

// -- Other Modules --
#include "Gpio.h"
#include "SRData.h"
#include "SRException.h"
#include "SRTaskSequence.h"
#include "VirtualPin.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- Default defines ---------------------------------------------------------------

#ifndef SR_UNUSED_PINS_CONFIG_COUNT_ERROR_IN_PERCENTAGE
    #define SR_UNUSED_PINS_CONFIG_COUNT_ERROR_IN_PERCENTAGE                    (10)
#endif

// This option shows time by which SRUnusedpinsConfig Handler runs, Its in milliseconds.
// #define SR_UNUSED_PINS_CONFIG_HANDLER_TIME 5 is 5mS
#define SR_UNUSED_PINS_CONFIG_HANDLER_TIME                                                 (5)

//---------------------------------------------------------------------------------------------------------------------
// its used to generate exception in case handler gets missed by certain percentage of count
// Mentioned in SRUnusedpinsConfig_prv.h file SR_UNUSED_PINS_CONFIG_COUNT_ERROR_IN_PERCENTAGE
// SRUnusedPinsConfig__Monitor used to check SRUnusedPinsConfig__Handler5ms is not getting missed
// and executing correct number of times.
//---------------------------------------------------------------------------------------------------------------------

// This option shows time by which SRUnusedpinsConfig monitor runs, Its in milliseconds.

#define SR_UNUSED_PINS_CONFIG_MONITOR_HANDLER_TIME                               (100)


// below Macros mainly used to calculate number if times Handler (5mS) will get executed before next
// run of SRUnusedPinsConfig__Monitor.

#define SR_UNUSED_PINS_CONFIG_MONITOR_ABSOLUTE_COUNT    \
                    (SR_UNUSED_PINS_CONFIG_MONITOR_HANDLER_TIME / SR_UNUSED_PINS_CONFIG_HANDLER_TIME)

// This Macro calculates error count from percentage

#define SR_UNUSED_PINS_CONFIG_MONITOR_ERROR_COUNT          \
                    (( SR_UNUSED_PINS_CONFIG_MONITOR_ABSOLUTE_COUNT * SR_UNUSED_PINS_CONFIG_COUNT_ERROR_IN_PERCENTAGE ) / 100 )

// This Macro calculates max allowed count of SRUnusedPinsConfig__Handler5ms execution before next
// SRUnusedPinsConfig__Monitor run / Max number of allowed times pins have been checked for configuration

#define SR_UNUSED_PINS_CONFIG_HANDLER_MAX_COUNT                       \
                    ( SR_UNUSED_PINS_CONFIG_MONITOR_ABSOLUTE_COUNT + SR_UNUSED_PINS_CONFIG_MONITOR_ERROR_COUNT )

// This Macro calculates min allowed count of SRUnusedpinsConfig SRUnusedPinsConfig__Handler5ms execution before next
// SRUnusedPinsConfig__Monitor run / Min number of allowed times pins have been checked for configuration

#define SR_UNUSED_PINS_CONFIG_HANDLER_MIN_COUNT                        \
                    ( SR_UNUSED_PINS_CONFIG_MONITOR_ABSOLUTE_COUNT -  SR_UNUSED_PINS_CONFIG_MONITOR_ERROR_COUNT )


// typedef struct, format of Unused Pins parameters
typedef struct SR_UNUSED_PINS_PARAM_STRUCT
{
    uint8   Virtual_Pin_Index;              /*  pin index from  VIRTUALPIN_LIST */
    uint8   Gpio_Type:7;                        /* Enum of GPIO_PIN_MODE_TYPE */
    uint8   Is_High:1;                            /* in status  should be high or low ( only if Pins are output ) */
} SR_UNUSED_PINS_PARAM_TYPE;

//! definition of the unused pins configuration.
static const SR_UNUSED_PINS_PARAM_TYPE SR_UNUSED_PINS_TABLE[] = SR_UNUSED_PINS_CONFIG_LIST;

//-------------------------------------- Variables ---------------------------------------------------------------
//! Keeps track of how many times pins have been checked for Configuration.
static uint8 Unused_Pins_Config_Count;
//! Flag to skip first time SRUnusedPinsConfig__Monitor.
static uint8 Skip_First_Monitor;

//-------------------------------------- inverted Variables ---------------------------------------------------------------
//! Inverted memory for the Class B variable,  Keeps track of how many times pins have been checked
//!  for Configuration.
static uint8 N_Unused_Pins_Config_Count;
//! Inverted memory for the Class B variable,  Flag to skip first time SRUnusedPinsConfig__Monitor.
static uint8 N_Skip_First_Monitor;


//! Macro to calculate number of pin entries in SR_UNUSED_PINS_TABLE
#define NUMBER_OF_UNUSED_PINS_TO_CONFIG  ELEMENTS_IN_ARRAY(SR_UNUSED_PINS_TABLE)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void DoUnusedPinsUpdate( uint8 unused_pins_table_index );
static void ConfigUnusedPins( void );



//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRUnusedPinsConfig and its variables.
 *                    It also initializes / configures Unused pin status.
 *
 */
void SRUnusedPinsConfig__Initialize(void)
{
    //Init variable and flags
    SR_SET_BYTE(Unused_Pins_Config_Count, 0);
    SR_SET_BYTE(Skip_First_Monitor, 1);

    //Configure and set Unused pin status in initialization. Pins not verified here as first time configuration.
    ConfigUnusedPins();

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_SRCHECKUNUSEDPINS);
    #endif
}


/**
 *  @brief      It validates configuration, data integrity of pins and reconfigure unused pins
 *                    This should run at 5mS handler
 *
 */
void SRUnusedPinsConfig__Handler5ms( void )
{
    uint8 table_index;

    // Check configuration and status of each pin entry  in SR_UNUSED_PINS_TABLE
    for( table_index = 0; table_index < NUMBER_OF_UNUSED_PINS_TO_CONFIG ; table_index++  )
    {
        // Check Virtual Pin Index is within limit
        SR_UNUSED_PINS_PARAM_TYPE *cur_param_ptr =
                    (SR_UNUSED_PINS_PARAM_TYPE *)&SR_UNUSED_PINS_TABLE[ table_index ];
       if(cur_param_ptr->Virtual_Pin_Index < VirtualPin__GetNumOf())
       {
           // Get Port and Pin from Virtual Pin Table
           VIRTUALPIN_TYPE *pin_ptr = (void *) &VIRTUALPIN_TABLE[cur_param_ptr->Virtual_Pin_Index];

           // Check pin configuration & status of pins mentioned in SR_UNUSED_PINS_TABLE.
           // If Configuration or status is not changed reconfigure & refresh pin.
           // If there is difference in actual and expected Pins configuration or status of Pins, Gpio module
           // will generate Exception
           if(TRUE == Gpio__CheckConfig(pin_ptr->port, pin_ptr->pin,
                           (GPIO_PIN_MODE_TYPE)cur_param_ptr->Gpio_Type, (BOOL_TYPE)cur_param_ptr->Is_High))
           {
               // Update the pins.
               DoUnusedPinsUpdate( table_index );
           }
       }
    }

    // Check data integrity & Increment count for number of times pins gets configured.
    SR_CHECK_BYTE( Unused_Pins_Config_Count );
    SR_SET_BYTE( Unused_Pins_Config_Count, Unused_Pins_Config_Count + 1 );
    //If data overflow generate exception
    if( 0xFF == Unused_Pins_Config_Count )
    {
        SRException__Queue( SREXCEPTION_ASSYNCHRONOUS_TASK_OVERRUN, (uint32)Unused_Pins_Config_Count, 0 );
    }
}


/**
 *  @brief      Monitors @100mS
 *                    It checks count of number of time pins got refreshed, tracks SRUnusedPinsConfig__Handler5ms
 *                    with 100mS SRUnusedPinsConfig__Monitor.
 *                    If Unused_Pins_Config_Count goes beyond error count this method will generate exception
 *
 */
void SRUnusedPinsConfig__Monitor( void )
{
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRCHECKUNUSEDPINS_ENTER);
#endif

    // Check data Integrity
    SR_CHECK_BYTE( Skip_First_Monitor );
    // skip first time as Async task and App mgr task are not in sync which leads to
    // error in Unused_Pins_Config_Count
    if(0 == Skip_First_Monitor)
    {
        SR_CHECK_BYTE( Unused_Pins_Config_Count );
        // Validate number of times pins refreshed is within limits if not generate exception
        if( ( SR_UNUSED_PINS_CONFIG_HANDLER_MAX_COUNT < Unused_Pins_Config_Count ) ||
        ( SR_UNUSED_PINS_CONFIG_HANDLER_MIN_COUNT > Unused_Pins_Config_Count ) )
        {
            SRException__Queue( SREXCEPTION_ASSYNCHRONOUS_TASK_OVERRUN, (uint32)Unused_Pins_Config_Count, 0 );
        }
    }

    // Clear Flag and Count
    SR_SET_BYTE( Unused_Pins_Config_Count, 0 );
    SR_SET_BYTE( Skip_First_Monitor, 0 );

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRCHECKUNUSEDPINS_EXIT);
#endif
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the output pin based on the requested value and change configuration
 * @param unused_pins_table_index = SR_UNUSED_PINS_TABLE Table index
 * @return None
 */
static void DoUnusedPinsUpdate( uint8 unused_pins_table_index )
{
    // Validate Virtual Pin Index is within VIRTUALPIN_TABLE limits.
    SR_UNUSED_PINS_PARAM_TYPE *cur_param_ptr =
                (SR_UNUSED_PINS_PARAM_TYPE *)&SR_UNUSED_PINS_TABLE[ unused_pins_table_index ];
    if(cur_param_ptr->Virtual_Pin_Index < VirtualPin__GetNumOf())
    {
        VIRTUALPIN_TYPE *pin_ptr = (void *) &VIRTUALPIN_TABLE[cur_param_ptr->Virtual_Pin_Index];

        // Check an entry where Port is not set
        if(pin_ptr->port != PORT_NULL)
        {
            // Set Pin value if an Output Type is set
            if((cur_param_ptr->Gpio_Type == OUTPUT_PUSHPULL) ||
                (cur_param_ptr->Gpio_Type == OUTPUT_OPEN_DRAIN) ||
                (cur_param_ptr->Gpio_Type == SPECIAL_FUNCTION_PUSH_PULL) ||
                (cur_param_ptr->Gpio_Type == SPECIAL_FUNCTION_OPEN_DRAIN))
            {
                Gpio__PinWrite(pin_ptr->port, pin_ptr->pin, (BOOL_TYPE)cur_param_ptr->Is_High);
            }
            // Configure Pin
            Gpio__PinConfig(pin_ptr->port, pin_ptr->pin, (GPIO_PIN_MODE_TYPE)cur_param_ptr->Gpio_Type);
        }
    }
}


/**
 * Update the output pin based on the requested value and change configuration
 * This method doesn't validate previous pin configuration or data.
 * It mainly used in Initialization Method or first time pin configuration.
 * @param None
 * @return None
 */
static void ConfigUnusedPins( void )
{
    uint8 table_index;

    // Check configuration and status of each pin entry  in SR_UNUSED_PINS_TABLE
    for( table_index = 0; table_index < NUMBER_OF_UNUSED_PINS_TO_CONFIG ; table_index++  )
    {
        // Update the pins.
        DoUnusedPinsUpdate( table_index );
    }
}

#endif  // #if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)
