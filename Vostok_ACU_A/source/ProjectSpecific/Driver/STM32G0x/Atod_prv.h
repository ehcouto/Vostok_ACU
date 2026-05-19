/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      Contains Class B related configuration parameters for the SRAtod GDM
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef ATOD_PRV_H_
	#define ATOD_PRV_H_

//=====================================================================================================================
//-------------------------------------- CLASS B Configuration Parameter-----------------------------------------------
//=====================================================================================================================

#define ATOD_SR_FEATURE_TEST        						DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the reference voltage supplied for the ADC Engine.
 *    @details- Define the exact voltage (in terms of millisecond) provided for the Microcontroller.
 *              The supply voltage for the STM32F0x controller varies from 2.0v to 3.6v.
 *
 *
 *    @param  - 2000L to 3600L
 *
 *    @note -   //If the reference voltage supplied for the ADC Engine is 3.0 then-
 *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS          3000L"
 *
 *              //If the reference voltage supplied for the ADC Engine is 2.0 then-
 *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS          2000L"
 *
 *              //If the reference voltage supplied for the ADC Engine is 3.6 then-
 *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS          3600L"
 */
//=====================================================================================================================
#define ATOD_ENGINE_REF_IN_MILLIVOLTS                        3300L

//=====================================================================================================================
/**
 *    @brief  - Defines the output voltage range of the “Internal Power Block” in terms of millivolt.
 *    @details- SRAtod module reads the “Internal Power Block” outputs periodically to make sure the functionality
 *              of the ADC and ADC multiplexer are working fine.
 *              The Internal power block output is connected to the ATOD_CH17.
 *              Its available only on ADC1 engine.
 *              It uses the 10-bit resolution.
 *
 *<pre>
 *      The output of the Internal power block varies as given below -
 *      ----------------------------------------------------------------------
 *      |  Conditions      |      Min       |     Typ        |      Max       |
 *      |in Degree celsius | in milliVolt   | in milliVolt   | in milliVolt   |
 *      |------------------|----------------|----------------|----------------|
 *      | -40 < T < +105   |    1160        |    1200        |    1250        |
 *      |------------------|----------------|----------------|----------------|
 *      | -40 < T < +85    |    1160        |    1200        |    1240        |
 *       --------------------------------------------------------------------- </pre>
 *
 *    @param  - For ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS - 1060 to 1160
 *              For ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS - 1260 to 1360
 *
 *              The Internal Power block requires constant voltage supply.
 *              If the supply voltage fluctuates then user has to take care of the ranges by adjusting
 *              the above macro reference value. For better understanding please refer the below examples
 *
 *    @note -   For the constant external supply voltage of 3.3v the macro value should be-
 *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS                        3300L"
 *              "#define ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS                  1160"
 *              "#define ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS                  1260"
 *
 *              For the constant external supply voltage of 2.0v the macro value should be-
 *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS                        2000L"
 *              "#define ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS                  1160"
 *              "#define ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS                  1260"
 */
//=====================================================================================================================
#define ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS               1060
#define ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS               1360


#endif // ATOD_PRV_H_
