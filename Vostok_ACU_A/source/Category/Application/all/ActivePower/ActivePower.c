/**
 *  @file
 *  @brief      Active Power Calculation for RIF
 *
 *  @details    One interface for any motor present in system
 *
 *  @Module_Owner Eduardo H Couto
 *  $Header: ActivePower.c 1.00 - May/2022
 *
 *  @copyright  Copyright 2020 - Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ActivePower.h"
#include "Atod.h"
#include "PeakDetector.h"
#include "ActivePower_prv.h"
#include "MotorTypeDefs.h"
#include "IIRFilter.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint16 ActivePower_Current_Offset;
static uint16 ActivePower_Voltage_Offset;
static sint32 ActivePower_Active_Power_Accumulator;
static uint16 ActivePower_Active_Power;
static uint16 ActivePower_Active_Power_Comp;
static uint16 ActivePower_P2P_Voltage;
static uint16 ActivePower_P2P_Current;
static uint32 ActivePower_IIR_P2P_Voltage;
static uint32 ActivePower_IIR_Curr_Offset;
static uint32 ActivePower_IIR_Volt_Offset;
#if(POWER_COMPENSATION_PARAMS == 2)
static sint32 ActivePower_Active_Power_Accumulator_Max_NoComp;
static sint32 ActivePower_Active_Power_Accumulator_Comp;
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

#define P2P_CURRENT_RELAOD     (uint16)(P2P_CURRENT_MS*ACTIVEPOWER_FAST_HANDLER_FREQUENCY_HZ/1000)    //22ms
#define P2P_VOLTAGE_RELOAD     (uint16)(P2P_VOLTAGE_MS*ACTIVEPOWER_FAST_HANDLER_FREQUENCY_HZ/1000)    //22ms
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MclActivePower and its variables
 *
 */
void ActivePower__Initialize(APL_WASH_MOTOR_TYPE wash_motor)
{
	if(wash_motor == WM_SINGLE_SPEED_ASYNCH_SPH_PSC)
	{
		ActivePower_Voltage_Offset = 0;
		ActivePower_Current_Offset = 0;
		ActivePower_IIR_Volt_Offset = (uint32)(2019 << ACTIVEPOWER_VOLTAGE_OFFSET_ALPHA_BITS);
		ActivePower_IIR_Curr_Offset = (uint32)(1937 << ACTIVEPOWER_CURRENT_OFFSET_ALPHA_BITS);
		ActivePower_Active_Power_Accumulator = 0;
		ActivePower_Active_Power = 0;
		ActivePower_Active_Power_Comp = 0;
		ActivePower_P2P_Voltage = 0;
		ActivePower_P2P_Current = 0;
		ActivePower_IIR_P2P_Voltage = 0;

		#if(POWER_COMPENSATION_PARAMS == 2)
		ActivePower_Active_Power_Accumulator_Max_NoComp = 0;
		ActivePower_Active_Power_Accumulator_Comp = 0;
		#endif

		PeakDetector__Setup(PEAK_MCI_VOLT_CHANNEL, P2P_VOLTAGE_RELOAD);
		PeakDetector__Setup(PEAK_MCI_CURR_CHANNEL, P2P_CURRENT_RELAOD);

		ACTIVEPOWER_INIT_EXT_MODULES();
	}
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Fast Handler
 *
 */
void ActivePower__FastHandler(void)
{
	sint16 raw_volt;
	uint16 raw_curr;
	sint32 power_correction;
	sint32 temp_volt;
	#if(POWER_COMPENSATION_PARAMS == 2)
	sint32 temp_s32;
	#endif

	raw_volt = Atod__Convert(MCI_VOLTAGE_CHANNEL,ATOD_RESOLUTION_12BITS);
	ActivePower_Voltage_Offset = IIRFilterLong(&ActivePower_IIR_Volt_Offset, raw_volt, ACTIVEPOWER_VOLTAGE_OFFSET_ALPHA_BITS);

	raw_curr = Atod__Convert(MCI_CURRENT_CHANNEL,ATOD_RESOLUTION_12BITS);
	ActivePower_Current_Offset = IIRFilterLong(&ActivePower_IIR_Curr_Offset, raw_curr, ACTIVEPOWER_CURRENT_OFFSET_ALPHA_BITS);

	PeakDetector__SetValue(PEAK_MCI_VOLT_CHANNEL,  IIRFilterLong(&ActivePower_IIR_P2P_Voltage, raw_volt, 4));
	PeakDetector__SetValue(PEAK_MCI_CURR_CHANNEL, raw_curr);

	ActivePower_Active_Power_Accumulator += (sint32)(raw_curr - ActivePower_Current_Offset) * (sint32)(raw_volt - ActivePower_Voltage_Offset);

	if(TC_RISING_EDGE_EVENT == ZEROCROSS_FUNCTION_CALLBACK())
	{
		if(ActivePower_Active_Power_Accumulator <= ACCUMULATOR_THR)
		{
			ActivePower_Active_Power = 0;
		}
		else
		{
			ActivePower_Active_Power = (uint16)((ActivePower_Active_Power_Accumulator - ACCUMULATOR_THR) / ACTIVE_POWER_FACTOR);
		}

		//Read P2P Current
		ActivePower_P2P_Current = PeakDetector__GetPeak2Peak(PEAK_MCI_CURR_CHANNEL);
		//Read P2P Voltage
		ActivePower_P2P_Voltage = PeakDetector__GetPeak2Peak(PEAK_MCI_VOLT_CHANNEL);

#if(POWER_COMPENSATION_PARAMS == 0)
		// Parametrization for Yoda2.
		// AP_COMP5 = FLOOR(FLOOR('MciSimple_Active_Power_Median') - 4) -'FLOOR(FLOOR( 1536* (FLOOR('MciSimple_P2P_Voltage_4_Median')-1626) )/10000)
		// '-'FLOOR(FLOOR (176*(FLOOR('MciSimple_P2P_Voltage_4_Median')-1626) *(FLOOR('MciSimple_P2P_Voltage_4_Median')-1626) )/1000000)'
		temp_volt = ActivePower_P2P_Voltage - 1626;

		power_correction = (sint16)((((temp_volt * temp_volt) + ((sint32)873 * temp_volt)) / (sint32)5682) + (sint32)4);

		if(power_correction >= ActivePower_Active_Power)
		{
			ActivePower_Active_Power_Comp = 0;
		}
		else
		{
			ActivePower_Active_Power_Comp = ActivePower_Active_Power - power_correction;
		}
#elif(POWER_COMPENSATION_PARAMS == 1)
		// Parametrization for FINN
        temp_volt = ((sint32)ActivePower_P2P_Voltage)/((sint32)10) - (sint32)120;

        power_correction = (sint16)((( ((sint32)115 * temp_volt) - (temp_volt * temp_volt * (sint32)2)) / (sint32)218) + (sint32)6);

        if((-power_correction) >= ActivePower_Active_Power)
        {
        	ActivePower_Active_Power_Comp = 0;
        }
        else
        {
        	ActivePower_Active_Power_Comp = ActivePower_Active_Power + power_correction;
        }
#elif(POWER_COMPENSATION_PARAMS == 2)
        // Parametrization for Luke/Vostok.

        /* Save Accumulator before compensation */
        ActivePower_Active_Power_Accumulator_Max_NoComp = ActivePower_Active_Power_Accumulator;

        /* Calculate how far the voltage is from Nominal (120V) */
        temp_volt = (sint32)(P2P_VOLTAGE_THR - (sint32)ActivePower_P2P_Voltage);
        /* This is the new voltage compensation equation */
        temp_s32 = (8578)*(temp_volt);
        temp_s32 = temp_s32 - ((4914*(temp_volt)*(temp_volt))>>12);

        power_correction = temp_s32 - ACCUMULATOR_THR;

        /* Adjust power accumulator for the voltage measured */
        ActivePower_Active_Power_Accumulator_Comp = ActivePower_Active_Power_Accumulator + power_correction;

        if(ActivePower_Active_Power_Accumulator_Comp < 0)
        {
        	ActivePower_Active_Power_Comp = 0;
        }
        else
        {
        	ActivePower_Active_Power_Comp = (sint32)ActivePower_Active_Power_Accumulator_Comp / ACTIVE_POWER_FACTOR;
        }
#endif

        //Reset the Accumulator...
		ActivePower_Active_Power_Accumulator = 0;
	}
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return the calculated power data
 *
 */
sint32 ActivePower__GetData(MCI_AI_TYPE read_data)
{
	sint32 ret_val;
	ret_val = -32767; //Default

	if(read_data == MCI_AI_LOAD_TORQUE_S16)
	{
		ret_val = (sint32)ActivePower_Active_Power_Comp;
	}

	if(read_data == MCI_AI_CURR_PHASE_P2P)
	{
		ret_val = (sint32)ActivePower_P2P_Current;
	}

	if(read_data == MCI_AI_SHAFT_POWER_S16)
	{
		ret_val = (sint32)ActivePower_Active_Power;
	}

#if(POWER_COMPENSATION_PARAMS == 2)
	if(read_data == MCI_AI_STATOR_TEMP_S16)
	{
		ret_val = (sint32)ActivePower_Active_Power_Accumulator_Max_NoComp;
	}

	if(read_data == MCI_AI_ROTOR_TEMP_S16)
	{
		ret_val = (sint32)ActivePower_Active_Power_Accumulator_Comp;
	}
#endif

	return(ret_val);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


