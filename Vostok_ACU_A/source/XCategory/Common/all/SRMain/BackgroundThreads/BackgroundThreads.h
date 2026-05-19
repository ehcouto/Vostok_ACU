/**
 *  @file       
 *
 *  @brief      Public header for the BackgroundThreads feature
 *
 *  @details    This module creates a pool of Background threads depending by the items present in the
 *              BACKGROUNDTHREAD_STACK_LIST configuration, that contain a list of stack size for each thread.
 *              BACKGROUND_MAX_STACK_SIZE macro shall be defined to be at least equal to the sum of all stacks
 *              listed in BACKGROUNDTHREAD_STACK_LIST.
 *
 *              ** If BACKGROUND_MAX_STACK_SIZE cannot fit all thread stacks defined, no Background threads
 *              ** will be created.
 *
 *              ** Stack RAM occupation will be incremented by (number of threads + 1)*4 bytes to add
 *              ** stack check keywords for each thread and at the end of the entire area.
 *
 *              -  Any module can request to run a Background function using BackgroundThreads__Run(&FunctionName, minStack)
 *              -  Any module can also request to reserve a Thread to run a specific Background function
 *                 using BackgroundThreads__ReserveThread(&FunctionName, minStack) and first free thread
 *                 with the minimum stack size higher than the minStack value.
 *                 Each function can be reserved on one thread only.
 *              -  If a module wants to free a reserved Thread it can call BackgroundThreads__ReleaseThread(&FunctionName)
 *              -  BackgroundThreads__IsRunning(&FunctionName) will return if a function is running
 *              -  BackgroundThreads__GetElapsedTime(&FunctionName) will return time since it started, 0 otherwise
 *
 *  @copyright  Copyright 2021 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef BACKGROUNDTHREADS_H_
#define BACKGROUNDTHREADS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void BackgroundThreads__Initialize(void);
void BackgroundThreads__Handler(void);

PASS_FAIL_TYPE BackgroundThreads__ReserveThread(FUNCTION_TYPE function_ptr, uint16 stack_required);
PASS_FAIL_TYPE BackgroundThreads__Run(FUNCTION_TYPE function_ptr, uint16 stack_required);
void BackgroundThreads__ReleaseThread(FUNCTION_TYPE function_ptr);
BOOL_TYPE BackgroundThreads__IsRunning(FUNCTION_TYPE function_ptr);
uint32 BackgroundThreads__GetElapsedTime(FUNCTION_TYPE function_ptr);
#endif // BACKGROUNDTHREADS_H_
