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


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "OsServices.h"
#include <string.h>
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It temporary suspend the OS scheduler
 *
 */
void OsServices__Suspend(void)
{
    OS_EnterRegion();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It resumes the OS scheduler previously suspended
 *  @note
 *
 */
void OsServices__Resume(void)
{
    OS_LeaveRegion();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Mutex Initialization
 *  @detail     This function must be called during initialization, before any use of Mutexes
 *
 *  @param      Pointer to a mutex variable
 */
void OsServices__MutexInitialize(OSSERVICE_MUTEX_TYPE *mutex_ptr)
{
    memset(mutex_ptr, 0, sizeof(OSSERVICE_MUTEX_TYPE));
    OS_CreateRSema(mutex_ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Request to lock a Mutex
 *  @details    This function will request to lock a Mutex: if the Mutex is already in use by another thread
 *              this function doesn't suspend current thread execution but it returns FAIL.
 *
 *  @param      Pointer to a mutex variable to request lock
 *  @return     PASS if Mutex lock succeeded, FAIL if it's in use from another thread
 *
 */
PASS_FAIL_TYPE OsServices__MutexTryLock(OSSERVICE_MUTEX_TYPE *mutex_ptr)
{
    PASS_FAIL_TYPE result = (OS_Request(mutex_ptr) != 0) ? PASS : FAIL;
    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Lock a Mutex or wait until it gets freed
 *  @details    This function will perform a Mutex lock: if the Mutex is already in use by another thread
 *              execution will be suspended until Mutex will be released; embOS shall switch to the thread which
 *              locked the Mutex in order to complete execution and release it.
 *  @note       Check that all call to OsServices__MutexLock() in a thread will be completed
 *              with a call to OsServices__MutexRelease()
 *
 *  @param      mutex_ptr - Pointer to a mutex variable to lock
 */
void OsServices__MutexLock(OSSERVICE_MUTEX_TYPE *mutex_ptr)
{
    (void) OS_Use(mutex_ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Releases a Mutex previously locked
 *  @details    This function will release a Mutex previously locked: if there are other high priority threads
 *              which are locking that Mutex, embOS will switch to them since they were interrupted due to the lock.
 *
 *  @param      mutex_ptr - Pointer to a mutex variable to release
 */
void OsServices__MutexRelease(OSSERVICE_MUTEX_TYPE *mutex_ptr)
{
    OS_Unuse(mutex_ptr);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
