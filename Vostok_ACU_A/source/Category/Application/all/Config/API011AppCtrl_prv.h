/**
 *  @file       
 *
 *  @brief      Private parameters for the API011AppCtrl module.
 *
 *  @copyright  Copyright 2015-$Date: 2017/01/17 13:44:31EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API011APPCTRL_PRV_H_
#define API011APPCTRL_PRV_H_


//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define API011APPCTRL__GET_PRODUCT_TYPE()  0

/**
 * @brief   Handles project specific behavior when the feedback message with the API011APPCTRL_OPCODE_ERROR opcode is received.
 * @param   node: The source of the message
 * @param   error: see API011APPCTRL_ERROR_TYPE
 */
#define API011APPCTRL__RECEIVED_ERROR(node,error)

//! Macro called when the feedback message with the accepted Regulations is received from the ACU
//! This macro is only used and relevant when SYSTEM_HMI_HANDLING is ENABLED.
#define API011APPCTRL_REGULATION_RX_NOTIFICATION()

#endif // API011APPCTRL_PRV_H_
