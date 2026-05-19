/**
 *  @file       BoardDebugging.h
 *
 *  @brief
 *
 *  @section
 *
 *  @copyright  Copyright 2015 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef BOARDDEBUGGING_H_
    #define BOARDDEBUGGING_H_


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


void BoardDebugging__Initialize(void);
void BoardDebugging__PwmHandler(void);
void BoardDebugging__250usHandler(void);
void BoardDebugging__25msHandler(void);

void BoardDebugging__FastIsrAnalysis(void);
void BoardDebugging__PwmIsrAnalysis(void);


extern void SRMCAtod__SetSyncDelay(int delay_value);

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#endif // BOARDDEBUGGING_H_

