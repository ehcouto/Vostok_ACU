/*
 * McuGetDataMonitor.c
 *
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "McuGetDataMonitor.h"
#if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))
// -- This Module --
#include "API221MotionCtrl.h"
#include "CommunicationMonitor.h"
#elif ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == ENABLED))
#include "MciSensorsDw.h"
#endif

#include "Hbl.h"

#if (API221_CLIENT_FEATURE == ENABLED)
static HBL_PILOT_ENUM_TYPE MGD_Wash_Pump_Pilot_Type;
static HBL_PILOT_ENUM_TYPE MGD_Drain_Pump_Pilot_Type;

static sint16 MGD_Wash_Pump_Speed;
static sint16 MGD_Drain_Pump_Speed;
#endif

#if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))
	static uint16 MGD_Reveal_Node_Status_Old;
	static uint8 MGD_Mcu_Enable_State_Old;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
	void ActivateActualSpeedChannels(void);
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//----------------------------------------------------------------------------
/**
 *  @brief      McuGetDataMonitor__Initialize
 *  @details    Function used to initialize the McuGetDataMonitor.
 *
 */
void McuGetDataMonitor__Initialize(void)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	MGD_Wash_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP));
	MGD_Drain_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

	MGD_Wash_Pump_Speed = 0;
	MGD_Drain_Pump_Speed = 0;
	#endif
	#if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))
		MGD_Reveal_Node_Status_Old = 0;
		MGD_Mcu_Enable_State_Old = UCHAR_MAX;
	#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief      McuGetDataMonitor__Execution
 *  @details    Function used to perform the McuGetDataMonitor.
 *
 */
void McuGetDataMonitor__Execution(void)
{
#if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))
	sint32 temp_data;
	uint16 reveal_node_status = Reveal__GetNodestatus();
	uint8 *mcu_enable_state_ptr;

	mcu_enable_state_ptr = (uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_MCU_ENABLE));

	if (((mcu_enable_state_ptr != NULL) && (*mcu_enable_state_ptr == (uint8)OFF)) ||
		(BIT_TEST(reveal_node_status, WIN_MCU_ADDRESS) == (uint16)0))
	{
		// MCU is off or is not communicating, assume both speeds are 0
		if (MGD_Wash_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			MGD_Wash_Pump_Speed = 0;
		}

		if (MGD_Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			MGD_Drain_Pump_Speed = 0;
		}

		MGD_Reveal_Node_Status_Old = 0;
	}
	else
	{
		if (BIT_TEST(MGD_Reveal_Node_Status_Old, WIN_MCU_ADDRESS) == (uint16)0)
		{
			ActivateActualSpeedChannels();
		}
		MGD_Reveal_Node_Status_Old = reveal_node_status;

		if (MGD_Wash_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			if(API221MotionCtrl__GetData(MOTOR_0,MCI_AI_ACTUAL_SPEED_S16, &temp_data))
			{
				MGD_Wash_Pump_Speed = temp_data >> 16;
			}
		}
		if (MGD_Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			if(API221MotionCtrl__GetData(MOTOR_1,MCI_AI_ACTUAL_SPEED_S16, &temp_data))
			{
				MGD_Drain_Pump_Speed = temp_data >> 16;
			}
		}
	}


	// ENABLE or DISABLE Communication failure with MCU
	if (mcu_enable_state_ptr != NULL)
	{
		if (*mcu_enable_state_ptr != MGD_Mcu_Enable_State_Old)
		{
			if (*mcu_enable_state_ptr == (uint8)OFF)
			{
				CommunicationMonitor__Disable(WIN_MCU_ADDRESS);
			}
			else
			{
				CommunicationMonitor__Enable(WIN_MCU_ADDRESS);
			}
		}

		MGD_Mcu_Enable_State_Old = *mcu_enable_state_ptr;
	}
#elif ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == ENABLED))
	if (MGD_Wash_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		MGD_Wash_Pump_Speed = MciSensorsDw__GetAnalog(SRMOTORSWITCH_LOAD_L0, (MCISENSORSDW_AI_TYPE) MCI_AI_ACTUAL_SPEED_S16) >> 16;
	}
	if (MGD_Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		MGD_Drain_Pump_Speed = MciSensorsDw__GetAnalog(SRMOTORSWITCH_LOAD_L1, (MCISENSORSDW_AI_TYPE) MCI_AI_ACTUAL_SPEED_S16) >> 16;
	}
#endif
}

#if (API221_CLIENT_FEATURE == ENABLED)
//----------------------------------------------------------------------------
/**
 *  @brief      McuGetDataMonitor__GetWashPumpSpeed
 *  @details    Function used to return the MGD_Wash_Pump_Speed.
 *
 */
sint16 McuGetDataMonitor__GetWashPumpSpeed(void)
{
	return MGD_Wash_Pump_Speed;
}

//----------------------------------------------------------------------------
/**
 *  @brief      McuGetDataMonitor__GetDrainPumpSpeed
 *  @details    Function used to return the MGD_Drain_Pump_Speed.
 *
 */
sint16 McuGetDataMonitor__GetDrainPumpSpeed(void)
{
	return MGD_Drain_Pump_Speed;
}
#endif
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
#if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))
//----------------------------------------------------------------------------
/**
 *  @brief      ActivateActualSpeedChannels
 *  @details    Function that active the wash pump and drain pump actual speed channels
 *
 */
void ActivateActualSpeedChannels(void)
{
	uint32 Channel_Bit_Mask = 0;
	BIT_SET(Channel_Bit_Mask,MCI_AI_ACTUAL_SPEED_S16);

	if (MGD_Wash_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		(void)API221MotionCtrl__SetDataPublicationPeriod(MOTOR_0, 1000);
		(void)API221MotionCtrl__AddPeriodicDataChannels(MOTOR_0,  Channel_Bit_Mask);
	}

	if (MGD_Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		(void)API221MotionCtrl__SetDataPublicationPeriod(MOTOR_1, 1000);
		(void)API221MotionCtrl__AddPeriodicDataChannels(MOTOR_1, Channel_Bit_Mask );
	}
}
#endif /*((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))*/


//----------------------------------------------------------------------------
/**
 *  @brief      McuGetDataMonitor__McuReset
 *  @details    Function used in Api009 to catch the McuReset
 *
 */
void McuGetDataMonitor__McuReset(uint8 node, uint8 reason, uint8 reset_mode)
{
#if ((API221_CLIENT_FEATURE == ENABLED) && (API221_SERVER_FEATURE == DISABLED))
	if (node == (uint8)WIN_MCU_ADDRESS)
	{
		ActivateActualSpeedChannels();
	}
#endif
}
