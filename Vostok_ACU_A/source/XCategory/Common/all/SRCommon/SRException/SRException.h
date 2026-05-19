/**
 *  @file       SRException.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2007.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SREXCEPTION_H_
	#define SREXCEPTION_H_

#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


typedef enum
{
    SREXCEPTION_NONE,
    SREXCEPTION_SRDATA_NOISE,
    SREXCEPTION_SRDATA_BAD_MEMORY,
    SREXCEPTION_SRDATA_NOT_ALLOWED,

	SREXCEPTION_SRTASKMON_ALREADY_ENABLED,
	SREXCEPTION_SRTASKMON_NO_TASKS,
	SREXCEPTION_SRTASKMON_TIMEOUT,
	SREXCEPTION_SRTASKMON_ENTER_OUTOFRANGE,
	SREXCEPTION_SRTASKMON_ENTER_INACTIVE,
	SREXCEPTION_SRTASKMON_ALREADY_ENTER,
	SREXCEPTION_SRTASKMON_EXIT_OUTOFRANGE,
	SREXCEPTION_SRTASKMON_EXIT_INACTIVE,
	SREXCEPTION_SRTASKMON_ALREADY_EXIT,
	SREXCEPTION_SRTASKMON_SUSPEND_OVERRUN,
	SREXCEPTION_SRTASKMON_SUSPEND_TIMEOUT,

	SREXCEPTION_SRTASKSEQ_WRONG_NUMBER,
	SREXCEPTION_SRTASKSEQ_INCOMPLETED,
	SREXCEPTION_SRTASKSEQ_TIMEOUT,

	SREXCEPTION_SRISR_OVERRUN,
	SREXCEPTION_SRISR_SEQUENCE_MISMATCH,
	SREXCEPTION_SRISR_TIMEOUT,

	SREXCEPTION_OS_ERROR,
	SREXCEPTION_STACK_OVERFLOW,

	SREXCEPTION_HBL_DATA_NOT_CONSISTENT,

    SREXCEPTION_APPLICATION_TASK_OVERRUN,
    SREXCEPTION_ASSYNCHRONOUS_TASK_OVERRUN,
    SREXCEPTION_COMMUNICATION_TASK_OVERRUN,

    SREXCEPTION_SRCLOCK_TEST,
    SREXCEPTION_SRCLOCK_OVERRUN,

    SREXCEPTION_PRODUCT_LEVEL_RECOVERABLE,
    SREXCEPTION_PRODUCT_LEVEL_NOT_RECOVERABLE,

    SREXCEPTION_MICRO_RAM_INTEGRITY_TEST,
    SREXCEPTION_MICRO_FLASH_INTEGRITY_TEST,
    SREXCEPTION_MICRO_CPU_REGISTER_TEST,
    SREXCEPTION_ATOD_CONFIGURATION_CORRUPTION,
    SREXCEPTION_ATOD_STUCK,
    SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION,
    SREXCEPTION_GPIO_DATA_CORRUPTION,
    SREXCEPTION_TIMER_DATA_CORRUPTION,
    SREXCEPTION_TIMER_CONFIGURATION_CORRUPTION,

	SREXCEPTION_SRCHECK_DATA_NOT_CONSISTENT,
	SREXCEPTION_SRACTION_DATA_NOT_CONSISTENT,

} SREXCEPTION_EVENT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRException__LogLastException(void);
void SRException__Queue(SREXCEPTION_EVENT_TYPE event,uint32 data1, uint32 data2);



#endif // SREXCEPTION_H_


