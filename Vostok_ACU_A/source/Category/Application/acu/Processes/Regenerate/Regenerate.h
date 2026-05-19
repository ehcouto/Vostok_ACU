/**
 * @file		Regenerate.h
 * @brief       Public interface to the Regenerate process module.
 *
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_REGENERATE_REGENERATE_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_REGENERATE_REGENERATE_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --

// -- Other Modules --
#include "Process.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Bit indexes in RegenerateAndSaltBitmap
typedef enum BIT_INDEXES_IN_REG_AND_SALT_BITMAP_ENUM
{
	FST_REGENERATION_DONE			= 0,
	BACKRINSE_REQUIRED				= 1,
	SALT_RISING_EDGE				= 2,
	SALT_FALLING_EDGE				= 3
} BIT_INDEXES_IN_REG_AND_SALT_BITMAP_TYPE;

extern uint8 RegenerateAndSaltBitmap;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Regenerate__Initialize(void);
PROCESS_RESULT_TYPE Regenerate__Process(PROCESS_CONTROL_TYPE* control);
uint8 Regenerate__GetNumRegeneration(void);
void Regenerate__SetBitmapBit(uint8 bit_index);
void Regenerate__ClearBitmapBit(uint8 bit_index);
BOOL_TYPE Regenerate__GetBitmapBit(uint8 bit_index);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_REGENERATE_REGENERATE_H_ */
