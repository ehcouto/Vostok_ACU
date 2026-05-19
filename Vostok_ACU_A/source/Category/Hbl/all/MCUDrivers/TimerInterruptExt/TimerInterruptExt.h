/**
 *  @file
 *  @brief      Public interface to ${file_base} module.
 *
 *  $Revision: 1.4 $
 *
 *  Copyright 2011 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef TIMERINTERRUPTEXT_H_
#define TIMERINTERRUPTEXT_H_

//  --- Include Files -------------------------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================
extern void TimerInterruptExt__SetupPerInt(unsigned short freq, unsigned char preemption_priority, unsigned char sub_priority);
extern void TimerInterruptExt__ClearPerIntF(void);
extern void TimerInterruptExt__StartPerInt(void);
extern void TimerInterruptExt__StopPerInt(void);
extern void TimerInterruptExt__ResetPerInt(void);

extern void TimerInterruptExt__SetupEventInt(unsigned char channel, unsigned char event);
extern void TimerInterruptExt__ClearEvtIntF(unsigned char channel);

#endif  // TIMERINTERRUPTEXT_H_
