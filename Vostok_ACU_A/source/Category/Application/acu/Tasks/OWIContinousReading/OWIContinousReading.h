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
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_OWICONTINOUSREADING_OWICONTINOUSREADING_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_OWICONTINOUSREADING_OWICONTINOUSREADING_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
ACTIVE_TYPE OWIContinousReading__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE OWIContinousReading__Task(uint8 compartment);
void OWIContinousReading__NVDataRestore(uint8 compartment);
void OWIContinousReading__Terminate(uint8 compartment);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_OWICONTINOUSREADING_OWICONTINOUSREADING_H_


