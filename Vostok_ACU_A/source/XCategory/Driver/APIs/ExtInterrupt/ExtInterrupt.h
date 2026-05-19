/**
 *  @file       
 *
 *  @brief      This file defines the ExtInterrupt API accessible to user
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: ExtInterrupt.h 1.5 2015/06/22 14:55:22EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/22 14:55:22EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef EXTINTERRUPT_H_
#define EXTINTERRUPT_H_
#include "C_Extensions.h"
#include "ExtInterrupt_prm.h"
#include "ExtInterrupt_defs.h"
#include "Gpio.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    EXTINT_TRG_DISABLE,
    EXTINT_TRG_RISE,
    EXTINT_TRG_FALL,
    EXTINT_TRG_BOTH
}EXTINT_TRG_TYPE;

typedef enum
{
	EXTINT_CH0,
	EXTINT_CH1,
	EXTINT_CH2,
	EXTINT_CH3,
	EXTINT_CH4,
	EXTINT_CH5,
	EXTINT_CH6,
	EXTINT_CH7,
	EXTINT_CH8,
	EXTINT_CH9,
	EXTINT_CH10,
	EXTINT_CH11,
	EXTINT_CH12,
	EXTINT_CH13,
	EXTINT_CH14,
	EXTINT_CH15,
	EXTINT_CH16,
	EXTINT_CH17,
	EXTINT_CH18,
	EXTINT_CH19,
	EXTINT_CH20,
	EXTINT_CH21,
	EXTINT_CH22,
	EXTINT_CH23,
	EXTINT_CH24,
	EXTINT_CH25,
	EXTINT_CH26,
	EXTINT_CH27,
	EXTINT_CH28,
	EXTINT_CH29,
	EXTINT_CH30,
	EXTINT_CH31,
	EXTINT_CH32,
	EXTINT_CH33,
	EXTINT_CH34,
	EXTINT_CH35,
	EXTINT_CH36,
	EXTINT_CH37,
	EXTINT_CH38,
	EXTINT_CH39,
	EXTINT_CH40,
	EXTINT_CH41,
	EXTINT_CH42,
	EXTINT_CH43,
	EXTINT_CH44,
	EXTINT_CH45,
	EXTINT_CH46,
	EXTINT_CH47,
	EXTINT_CH48,
	EXTINT_CH49,
	EXTINT_CH50,
	EXTINT_CH51,
	EXTINT_CH52,
	EXTINT_CH53,
	EXTINT_CH54,
	EXTINT_CH55,
	EXTINT_CH56,
	EXTINT_CH57,
	EXTINT_CH58,
	EXTINT_CH59,
	EXTINT_CH60,
	EXTINT_CH61,
	EXTINT_CH62,
	EXTINT_CH63,
	EXTINT_CH64,
	EXTINT_CH65,
	EXTINT_CH66,
	EXTINT_CH67,
	EXTINT_CH68,
	EXTINT_CH69,
	EXTINT_CH70,
	EXTINT_CH71,
	EXTINT_CH72,
	EXTINT_CH73,
	EXTINT_CH74,
	EXTINT_CH75,
	EXTINT_CH76,
	EXTINT_CH77,
	EXTINT_CH78,
	EXTINT_CH79,
	EXTINT_CH80,
	EXTINT_CH81,
	EXTINT_CH82,
	EXTINT_CH83,
	EXTINT_CH84,
	EXTINT_CH85,
	EXTINT_CH86,
	EXTINT_CH87,
	EXTINT_CH88,
	EXTINT_CH89,
	EXTINT_CH90,
	EXTINT_CH91,
	EXTINT_CH92,
	EXTINT_CH93,
	EXTINT_CH94,
	EXTINT_CH95,
	EXTINT_CH96,
	EXTINT_CH97,
	EXTINT_CH98,
	EXTINT_CH99,
	EXTINT_CH100,
	EXTINT_CH101,
	EXTINT_CH102,
	EXTINT_CH103,
	EXTINT_CH104,
	EXTINT_CH105,
	EXTINT_CH106,
	EXTINT_CH107,
	EXTINT_CH108,
	EXTINT_CH109,
	EXTINT_CH110,
	EXTINT_CH111,
	EXTINT_CH112,
	EXTINT_CH113,
	EXTINT_CH114,
	EXTINT_CH115,
	EXTINT_CH116,
	EXTINT_CH117,
	EXTINT_CH118,
	EXTINT_CH119,
	EXTINT_CH120,
	EXTINT_CH121,
	EXTINT_CH122,
	EXTINT_CH123,
	EXTINT_CH124,
	EXTINT_CH125,
	EXTINT_CH126,
	EXTINT_CH127,
	EXTINT_CH128,
	EXTINT_CH129,
	EXTINT_CH130,
	EXTINT_CH131,
	EXTINT_CH132,
	EXTINT_CH133,
	EXTINT_CH134,
	EXTINT_CH135,
	EXTINT_CH136,
	EXTINT_CH137,
	EXTINT_CH138,
	EXTINT_CH139,
	EXTINT_CH140,
	EXTINT_CH141,
	EXTINT_CH142,
	EXTINT_CH143,
	EXTINT_CH144,
	EXTINT_CH145,
	EXTINT_CH146,
	EXTINT_CH147,
	EXTINT_CH148,
	EXTINT_CH149,
	EXTINT_CH150,
	EXTINT_CH151,
	EXTINT_CH152,
	EXTINT_CH153,
	EXTINT_CH154,
	EXTINT_CH155,
	EXTINT_CH156,
	EXTINT_CH157,
	EXTINT_CH158,
	EXTINT_CH159,
	EXTINT_CH160,
	EXTINT_NUM_CHANNELS
}EXTINT_CHNL_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ExtInterrupt__Initialize(void);
PASS_FAIL_TYPE ExtInterrupt__Config(EXTINT_CHNL_TYPE chnl, GPIO_PORT_TYPE port, uint8 pin, uint8 remap, EXTINT_TRG_TYPE trig_mode);


#endif // EXTINTERRUPT_H_


