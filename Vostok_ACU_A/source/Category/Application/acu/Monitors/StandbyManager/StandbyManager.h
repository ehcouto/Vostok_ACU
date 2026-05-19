/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2018-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef STANDBYMANAGER_H_
#define STANDBYMANAGER_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! time out for attempting further standby activation
#define STANDBY_ATTEMPT_TIMEOUT   2000
#define STANDBY_ATTEMPT_DEFAULT_VALUE   3

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void StandbyManager__Initialize(void);
void StandbyManager__Execute(void);
BOOL_TYPE StandbyManager__GetPermission(void);
uint8 StandbyManager__GetStatus(void);
BOOL_TYPE StandbyManager__GetActivateStandby(void);
void StandbyManager__ResetActivateStandby(void);
BOOL_TYPE StandbyManager__StopProcedure(void);
#endif // STANDBYMANAGER_H_


