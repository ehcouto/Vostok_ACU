/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Debug.c 1.7 2015/07/29 13:45:33EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/29 13:45:33EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "Debug.h"

//#include "LedMgr.h"
//#include "HblHmi.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//#define NUM_DEBUG_PINS      12
//const GPIO_PORTBIT_TYPE DEBUG_PN_LIST[NUM_DEBUG_PINS] = {   {PORTC, 6, OUTPUT_PUSHPULL}, // 0
//                                                            {PORTC, 5, OUTPUT_PUSHPULL}, // 1
//                                                            {PORTC, 4, OUTPUT_PUSHPULL}, // 2
//                                                            {PORTB, 7, OUTPUT_PUSHPULL}, // 3
//                                                            {PORTB, 6, OUTPUT_PUSHPULL}, // 4
//                                                            {NULL, 0, OUTPUT_PUSHPULL}, // 5
//                                                            {NULL, 0, OUTPUT_PUSHPULL}, // 6
//                                                            {NULL, 0, OUTPUT_PUSHPULL}, // 7
//                                                            {NULL, 0, OUTPUT_PUSHPULL}, // 8
//                                                            {NULL, 0, OUTPUT_PUSHPULL}, // 9
//                                                            {NULL, 0, OUTPUT_PUSHPULL}, // 10
//                                                            {PORTC, 7, OUTPUT_PUSHPULL} //11
//                                                            };
//

/*
 *
 * J2 - 8   - MISO_1  - PORTC.7
 * J2 - 10  - MOSI_1  - PORTC.6
 * J2 - 12  - SPSCL_1 - PORTC.5
 * J2 - 6   - SS_1    - PORTC.4
 * J2 - 4   - TMR1_CH1- PORTB.7
 * J2 - 2   - TMR1_CH0- PORTB.6
 *
 */


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

////---------------------------------------------------------------------------------------------------------------------
///**
// *  @brief      It Initializes the module Debug and its variables
// *
// */
//void Debug__Initialize(void)
//{
//    uint8 index;
//    for (index =0; index<NUM_DEBUG_PINS ; index ++)
//    {
//        if (DEBUG_PN_LIST[index].port != NULL)
//        {
//            Gpio__PinConfig(DEBUG_PN_LIST[index].port,DEBUG_PN_LIST[index].pin,DEBUG_PN_LIST[index].type);
//        }
//    }
//}
//
//
//void Debug__Set(uint8 id)
//{
//
//    if (id < NUM_DEBUG_PINS)
//    {
//        if (DEBUG_PN_LIST[id].port != NULL)
//        {
//            Gpio__PinWrite(DEBUG_PN_LIST[id].port,DEBUG_PN_LIST[id].pin,1);
//        }
//    }
//}
//
//void Debug__Clear(uint8 id)
//{
//    if (id < NUM_DEBUG_PINS)
//    {
//        if (DEBUG_PN_LIST[id].port != NULL)
//        {
//            Gpio__PinWrite(DEBUG_PN_LIST[id].port,DEBUG_PN_LIST[id].pin,0);
//        }
//    }
//}



/* Change API005LOWLEVEL_DEBUG_TABLE_APP_SPECIFIC for the function name on MAC */

void Debug__Func00(void)
{
/*	HblHmi__StopAllSound();
	LedMgr__SetAllLeds(0,0);
	LedMgr__SetGroup(ON1,100,0);*/
}

void Debug__Func01(void)
{
	/*HblHmi__StopAllSound();
	LedMgr__SetAllLeds(0,0);
	LedMgr__SetGroup(ON2,100,0);*/
}

void Debug__Func02(void)
{
	/*HblHmi__StopAllSound();
	LedMgr__SetAllLeds(100,0);*/
}

void Debug__Func03(void)
{
	/*HblHmi__StopAllSound();
	LedMgr__SetAllLeds(0,0);*/
}

void Debug__Func04(void)
{
	/*HblHmi__StopAllSound();
	HblHmi__PlaySoundSimple(SOUND_INDEX_FVT_3KHZ, SOUND_VOLUME_MAX);*/
}

void Debug__Func05(void)
{
	/*HblHmi__StopAllSound();
	HblHmi__PlaySoundSimple(SOUND_INDEX_FVT_4KHZ, SOUND_VOLUME_MAX);*/
}

void Debug__Func06(void)
{
	/*HblHmi__StopAllSound();
	HblHmi__PlaySoundSimple(SOUND_INDEX_FVT_5KHZ, SOUND_VOLUME_MAX);*/
}

void Debug__Func07(void)
{
	//HblHmi__StopAllSound();
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


