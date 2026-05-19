/**
 *  @file       
 *
 *  @brief      Standard API for additional OS services - embOS implementation
 *
 *  @details    This module provides a standard interface for some additional OS services without a direct
 *              dependency to a specific OS (like embOS)
 *              Current services provided:
 *              - Thread switch suspend/resume to temporary protect a specific execution
 *                      from preemption by high priority threads - interrupts would not be suspended by this feature
 *              - Mutex (or Resource semaphores) that can be used by defining and using a mutex variable in the module
 *                NOTE:
 *                - Mutex variables have to be initialized at every reset and shall not be saved on NVData sections
 *                - Mutexes cannot be used on any interrupt handler, including 1ms execution
 *
 *                Example:
 *                      OSSERVICE_MUTEX_TYPE Module_Data_Mutex;
 *                      ... <in an Initialize function> ...
 *                      OsServices__MutexInitialize(&Module_Data_Mutex);
 *                      ... <in one of the handlers, to protect execution through mutex>
 *                      OsServices__MutexLock(&Module_Data_Mutex);
 *                          ... do something...
 *                      OsServices__MutexRelease(&Module_Data_Mutex);
 *
 *                      OsServices__MutexTryLock() can be used instead of OsServices__MutexLock() to avoid waiting
 *                      before mutex is released; in this case the function returns FAIL.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2021. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef OS_SERVICES_H_
#define OS_SERVICES_H_

#include "RTOS.h"
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef OS_RSEMA OSSERVICE_MUTEX_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void OsServices__Suspend(void);
void OsServices__Resume(void);

void OsServices__MutexInitialize(OSSERVICE_MUTEX_TYPE *mutex_ptr);
PASS_FAIL_TYPE OsServices__MutexTryLock(OSSERVICE_MUTEX_TYPE *mutex_ptr);
void OsServices__MutexLock(OSSERVICE_MUTEX_TYPE *mutex_ptr);
void OsServices__MutexRelease(OSSERVICE_MUTEX_TYPE *mutex_ptr);
#endif // OS_SERVICES_H_
