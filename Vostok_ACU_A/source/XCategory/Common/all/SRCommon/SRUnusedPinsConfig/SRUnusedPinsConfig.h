/**
 *  @file
 *  @defgroup    CLASS_B
 *  @brief            Interface to the Public methods of SRUnusedPinsConfig module.
 *
 *  @details         It includes the Apis used in the SRUnusedPinsConfig module.
 *                      Refer to the SRUnusedPinsConfig.c source file for more detailed information.
 *
 *  @copyright    Copyright 2022 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRUNUSEDPINSCONFIG_H
	#define SRUNUSEDPINSCONFIG_H


#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRUnusedPinsConfig__Initialize(void);
void SRUnusedPinsConfig__Handler5ms(void);
void SRUnusedPinsConfig__Monitor(void);

#endif  // #if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)

#endif // SRUNUSEDPINSCONFIG_H


