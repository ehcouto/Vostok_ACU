/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: EmbOSBasic.h 1.3 2015/07/07 17:54:22EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/07 17:54:22EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SOURCE_SRSOURCE_DRIVER_EMBOS_EMBOSBASIC_H_
#define SOURCE_SRSOURCE_DRIVER_EMBOS_EMBOSBASIC_H_

#include "C_Extensions.h"
#include "RTOS.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


void EmbOSBasic__SystickHandler(void);
void EmbOSBasics__FlushOS(void);
#endif // SOURCE_SRSOURCE_DRIVER_EMBOS_EMBOSBASIC_H_


