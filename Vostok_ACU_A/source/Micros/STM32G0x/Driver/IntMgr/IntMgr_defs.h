/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef INTMGR_DEFS_H_
#define INTMGR_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void IntMgr__ExtInterruptExt(void);

void IntMgr__Timer1IrqHandler(void);
void IntMgr__Timer2IrqHandler(void);
void IntMgr__Timer3_4IrqHandler(void);
void IntMgr__Timer14IrqHandler(void);
void IntMgr__Timer15IrqHandler(void);
void IntMgr__Timer16IrqHandler(void);
void IntMgr__Timer17IrqHandler(void);

#endif // INTMGR_DEFS_H_


