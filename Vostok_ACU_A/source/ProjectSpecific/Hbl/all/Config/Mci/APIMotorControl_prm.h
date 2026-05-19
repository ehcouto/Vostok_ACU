
/**
 *  @file       APIMotorControl_prm.h
 *
 *  @brief      Parameters used in the APIMotorControl. Such parameters shall be defined according to the Mci API
 *              implementation. For instance, the number of connected motors to the Mci API has to be defined
 *              here as well.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: APIMotorControl_prm.h 1.8 2016/01/22 12:03:56BRST ANDRE PETRONILHO (PETROA1) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2016/01/22 12:03:56BRST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef APIMOTORCONTROL_PRM_H_
    #define APIMOTORCONTROL_PRM_H_


//-------------------------------------- Include Files ----------------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- PUBLIC (Constants & Defines) -------------------------------------------------
//=====================================================================================================================


#define API220_ENABLE_MCI_CALLS                     //! Allow commands from WIN Network to Mci.

#define API220_MAX_DI_CHECK_COUNT           6       //!< Amount of bytes to be check at each loop
#define MAX_DO_WRITE_COUNT                  4       //!< Amount of bytes to be written at each loop
#define API220_NR_OF_CONNECTED_MOTORS       1       //!< Number of connected motor that this API shall control

#define API220_FVT_ONLY
#define API220_DEBUG_MODE                   DISABLED//!< Enable debug mode (not intended for production!!)
#define API220_THERMALMODEL_DEBUG			DISABLED//!< Enable ThermalModel debug mode (not intended for production!!)

#if (API220_DEBUG_MODE == ENABLED)
void Mci__Emulate_Error(uint8 fault);
#endif

#endif // APIMOTORCONTROL_PRM_H_

