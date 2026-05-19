/**
 *  @file       MoisturePrevention.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_MOISTUREPREVENTION_MOISTUREPREVENTION_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_MOISTUREPREVENTION_MOISTUREPREVENTION_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
ACTIVE_TYPE MoisturePrevention__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
void MoisturePrevention__NVDataRestore(uint8 compartment);
ACTIVE_TYPE MoisturePrevention__Task(uint8 compartment);
void MoisturePrevention__Terminate(uint8 compartment);
void MoisturePrevention__Resume(uint8 compartment);
void MoisturePrevention__Pause(uint8 compartment);

#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_MOISTUREPREVENTION_MOISTUREPREVENTION_H_


