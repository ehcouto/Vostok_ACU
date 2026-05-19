/**
 *  @file       
 *
 *  @brief      Private parameters for the StatusLed module.
 *
 *  @copyright  Copyright 2018-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef STATUSLED_PRV_H_
#define STATUSLED_PRV_H_

#include "VirtualPin.h"
#include "Fault.h"

//=====================================================================================================================
//-------------------------------------- PRIVATE (Extern Variables, Constants & Defines) ------------------------------
//=====================================================================================================================

/*
 *  The index of the status led in the VIRTUALPIN_TABLE.
 *
 *  Macro must be uncommented and the index defined with the VIRTUALPIN_TABLE index of the i/o pin used to control the
 *  status led.
 *
 *  Example:
 *      STATUS_LED_VIRTUAL_PIN_INDEX    27
 */
#define STATUS_LED_VIRTUAL_PIN_INDEX	58


/*
 *  Macro is called the when the STATUS_LED_VIRTUAL_PIN_INDEX is invalid.
 */
#define STATUS_LED_VIRTUAL_PIN_INDEX_INVALID()

/*
 *  Determines if there is a communication fault present
 *  Macro should return TRUE a communication fault is present, else FALSE
 */
#define COMMUNICATION_FAULT_PRESENT()   ((Fault__IsInFaultState(FAULT_ID_ZEROCROSS) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_FILL_VALVE_TRIAC_SHORTED) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_PILOT_RELAY_STUCK_CLOSED) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_DIVERTER_TRIAC_SHORTED) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_PILOT_RELAY_STUCK_OPEN) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_GENERIC_ERROR) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_HARDWARE_OVERCURRENT) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_OVER_VOLTAGE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_UNDER_VOLTAGE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_DRIVER_FAILURE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_INVERTER_OVER_TEMPERATURE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_DM_MCI_GENERIC_ERROR) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_DM_MCI_HARDWARE_OVERCURRENT) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_DM_MCI_OVER_VOLTAGE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_DM_MCI_UNDER_VOLTAGE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_DM_MCI_DRIVER_FAILURE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_DM_MCI_INVERTER_OVER_TEMPERATURE) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_OVERFILL) == TRUE || \
										  Fault__IsInFaultState(FAULT_ID_MCU_NO_MCU_COMMUNICATION) == TRUE) ? TRUE : FALSE)
/*
 *  Determines if the status LED is active high or low
 *  Macro should return ENABLED if the LED is active low, else DISABLED
 */
#define STATUS_LED_ACTIVE_LOW   DISABLED



#endif // STATUSLED_PRV_H_


