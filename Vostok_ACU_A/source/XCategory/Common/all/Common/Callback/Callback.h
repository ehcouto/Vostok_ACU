/**
 *  @brief      Public interface to the Callback module.
 *
 *  @details    See the @details in the Callback.c file.
 *
 *  @copyright  Copyright (c) 2013, 2022 WhirlpoolCorporation
 */
#ifndef CALLBACK_H_
#define CALLBACK_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! The data type used to hold callback events managed by the Callback module.
typedef uint8 CALLBACK_EVENT_TYPE;

//! A function pointer prototype matched by Observer functions that are used as callback handlers.
typedef void (*CALLBACK_HANDLER_TYPE)(CALLBACK_EVENT_TYPE event);

//! Control structure used by the callback module for it's normal operations.
typedef const struct CALLBACK_CONTROL_STRUCT
{
    uint8 * const Registry;
    uint8   const Registry_Size;
} CALLBACK_CONTROL_TYPE;

#define CALLBACK_REGISTRY(callback_control_name)   callback_control_name##_Registry

//! macro used to define a control structure for a Subject module. 1 byte checksum is stored at end of handlers
//! @param callback_control_name = Name of subject's callback control structure.
//! @param registry_size = Number of observers allowed to attach to subject for callback.

#define DEFINE_CALLBACK_CONTROL_STRUCTURE(callback_control_name, registry_size)  \
static uint8 CALLBACK_REGISTRY(callback_control_name)[(registry_size * sizeof(CALLBACK_HANDLER_TYPE)) + 1U]; \
static CALLBACK_CONTROL_TYPE callback_control_name = {CALLBACK_REGISTRY(callback_control_name), registry_size}

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Callback__Initialize (CALLBACK_CONTROL_TYPE const * control);
void Callback__Register   (CALLBACK_CONTROL_TYPE const * control, CALLBACK_HANDLER_TYPE handler);
void Callback__Unregister (CALLBACK_CONTROL_TYPE const * control, CALLBACK_HANDLER_TYPE handler);
void Callback__Notify     (CALLBACK_CONTROL_TYPE const * control, CALLBACK_EVENT_TYPE event);

#endif  // CALLBACK_H_

