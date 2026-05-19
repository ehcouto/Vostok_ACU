/**
 *  @file
 *  @brief      This module configures comparator.
 *
 *  @copyright  Copyright 2016- Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef COMPARATOR_H
	#define COMPARATOR_H
#include "Comparator_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! COMPARATOR_CHANNEL_DEF defines the list of channels the API supports
//! Depending on the implementation not all channels in this list will have support
typedef enum
{
    COMPARATOR_CHANNEL0,
	COMPARATOR_CHANNEL1,
    COMPARATOR_CHANNEL2,
	COMPARATOR_CHANNEL3,
    COMPARATOR_CHANNEL4,
	COMPARATOR_CHANNEL5
}COMPARATOR_CHANNEL_DEF;


//! COMPARATOR_INVERTED_INPUT_DEF defines the possible inverting inputs

typedef enum
{
	COMPARATOR_INVERTED_INPUT0,
	COMPARATOR_INVERTED_INPUT1,
	COMPARATOR_INVERTED_INPUT2,
	COMPARATOR_INVERTED_INPUT3,
	COMPARATOR_INVERTED_INPUT4,
	COMPARATOR_INVERTED_INPUT5,
	COMPARATOR_INVERTED_INPUT6,
	COMPARATOR_INVERTED_INPUT7
} COMPARATOR_INVERTED_INPUT_DEF;

typedef enum
{
	COMPARATOR_NONINVERTED_INPUT0,
	COMPARATOR_NONINVERTED_INPUT1,
	COMPARATOR_NONINVERTED_INPUT2,
	COMPARATOR_NONINVERTED_INPUT3,
	COMPARATOR_NONINVERTED_INPUT4,
	COMPARATOR_NONINVERTED_INPUT5,
	COMPARATOR_NONINVERTED_INPUT6,
	COMPARATOR_NONINVERTED_INPUT7
} COMPARATOR_NONINVERTED_INPUT_DEF;

//! COMPARATOR_MODE_DEF defines the possible modes with which the channels can be configured to desired mode
typedef enum
{
	COMPARATOR_MODE_DISABLE,
	COMPARATOR_MODE_CONTINUOUS,
	COMPARATOR_MODE_SAMPLED_NONFILTERED,
	COMPARATOR_MODE_SAMPLED_FILTERED,
	COMPARATOR_MODE_WINDOWED,
	COMPARATOR_MODE_WINDOWED_RESAMPLED,
	COMPARATOR_MODE_WINDOWED_FILTERED
} COMPARATOR_MODE_DEF;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Comparator__Initialize(void);
PASS_FAIL_TYPE Comparator__Config(COMPARATOR_CHANNEL_DEF channel,COMPARATOR_INVERTED_INPUT_DEF inverted_input,COMPARATOR_NONINVERTED_INPUT_DEF noninverted_input,COMPARATOR_MODE_DEF mode, uint16 cmp_thr);
uint8 Comparator__GetStatus(COMPARATOR_CHANNEL_DEF channel);
void Comparator__SetThr(COMPARATOR_CHANNEL_DEF channel, uint16 cmp_thr);

#endif
