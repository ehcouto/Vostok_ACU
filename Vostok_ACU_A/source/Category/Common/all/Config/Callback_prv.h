/**
 *  @file
 *  @brief      Private parameters for the Callback module.
 *
 *  $Header: Callback_prv.h 1.1 2015/07/17 14:47:06EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2013 - $Date: 2015/07/17 14:47:06EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef CALLBACK_PRV_H_
#define CALLBACK_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//! Debug macro called when an attempt is made to register a handler that is already registered.
//! @param handler = The handler that is already registered.
#define HANDLER_IS_ALREADY_REGISTERED(handler)

//! Debug macro called when an attempt is made to unregister a handler that is already unregistered.
//! @param handler = The handler that is already unregistered.
#define HANDLER_IS_ALREADY_UNREGISTERED(handler)

//! Debug macro called when there is not enough space to register a handler.
//! @param control = The control structure that ran out of space for registering handlers.
#define NO_SPACE_TO_REGISTER_HANDLER(control)

//! Debug macro called when a attempt to register a NULL handler is made.
#define ATTEMPT_TO_REGISTER_NULL_HANDLER()

//! Debug macro called when a attempt to unregister a NULL handler is made.
#define ATTEMPT_TO_UNREGISTER_NULL_HANDLER()

//! Debug macro called when a NULL control structure is referenced.
#define CALLBACK_CONTROL_IS_NULL()

//! Debug macro called when a corrupted control structure is detected.
//! @param control = The control structure that is corrupted.
#define CALLBACK_REGISTRY_CORRUPTED(control)

#endif  // CALLBACK_PRV_H_
