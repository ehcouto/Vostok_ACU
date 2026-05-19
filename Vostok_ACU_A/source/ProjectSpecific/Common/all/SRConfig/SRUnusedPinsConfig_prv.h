/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief           Private (prv) configuration file for SRUnusedpinsConfig module
 *
 *  @details       Refer to the C source file for more detailed information.
 *
 *  @copyright  Copyright 2022 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _SRUNUSEDPINSCONFIG_PRV_H_
#define _SRUNUSEDPINSCONFIG_PRV_H_

#include "VirtualPin.h"

#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------

// Configuration of controller Pins to be used in SRUnusedPinsConfig module
// SR_UNUSED_PINS_CONFIG_LIST must be configured as an array of SR_UNUSED_PINS_PARAM_TYPE structure
// Virtual_Pin_Index - pin index from  VIRTUALPIN_LIST
// Gpio_Type - Enum of GPIO_PIN_MODE_TYPE
// Is_High - Pin status  should be high or low ( only if Pins are output )

//  Virtual_Pin_Index       pin_type                      Is_High
#define SR_UNUSED_PINS_CONFIG_LIST  { \
    {3                              ,OUTPUT_PUSHPULL              ,1},           /* 1               */ \
    {32                            ,OUTPUT_PUSHPULL              ,1} }         /* 2               */


//---------------------------------------------------------------------------------------------------------------------

//! This option allows to configure percentage of error allowed in SRUnusedPinsConfig module Handler execution.
//! This percentage mainly used to calculate Max and Min allowed count for SRUnusedPinsConfig__Handler5mS
//! execution and verification in SRUnusedPinsConfig__Monitor running at 100mS task

#define SR_UNUSED_PINS_CONFIG_COUNT_ERROR_IN_PERCENTAGE                    (10)


#endif  //   (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)

#endif // _SRUNUSEDPINSCONFIG_PRV_H_
