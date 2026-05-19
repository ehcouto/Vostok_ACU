/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MCUDebug.h"
#include "Mci.h"
#include "Mcl.h"
#include "Gpio.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#ifndef SIL_MODE
	#define SIL_MODE    DISABLED
#endif

#ifdef TIME_MEASUREMENT
#ifdef UC_STM32G0X_H_
#define TIMER_RESOLUTION_MHZ    64
#endif
#ifdef UC_RX130_H
#define TIMER_RESOLUTION_MHZ    32
#endif

uint16 Measured_Time_US;
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (SIL_MODE == ENABLED)
static uint8 Slow_Handler_Counter;

static uint8 Dbg_Control_Mode;
static uint8 Dbg_MciSlfac_Last_Failure;
static uint8 Dbg_Failure_State;
static uint16 Dbg_Mcl_Main_Counter;
static uint8 Dbg_MciSlfac_Wash_Motor_Type;
static sint16 Dbg_bemf_volt_tmp;
static uint8 My_GetDigital_0;
static uint8 My_GetDigital_1;
static uint8 My_GetDigital_2;
static uint8 My_GetDigital_3;

static sint32 My_GetAnalog_0;
static sint32 My_GetAnalog_1;
static sint32 My_GetAnalog_2;

static uint8 Slow_Handler_Counter;
#endif

#ifdef ENABLE_PILOT_MCI
static uint8 Pilot_Relay_Drive_Status;
static uint32 Pilot_Relay_Timer;
static BOOL_TYPE Pilot_Relay_Request;

static void PilotRelay(void);

//! Pilot relay port/pin defines
#define PILOT_RELAY_PORT  PORTB  //!< Use for toggling pilot relay output
#define PILOT_RELAY_PIN   14     //!< Use for toggling pilot relay output

//! Pilot relay port/pin defines
#define PILOT_DRIVE_PORT  PORTC  //!< Use for toggling pilot relay output
#define PILOT_DRIVE_PIN   2     //!< Use for toggling pilot relay output

//! Pilot relay delay periods defines
#define PILOT_RELAY_ENABLED_TIME        4 // @ 4000 Hz --> 1 ms
#define PILOT_RELAY_DISABLED_TIME     4 // @ 4000 Hz --> 1 ms

static sint16 BD_MotorControl;
#endif

#ifdef MCI_AUTOTESTING
static uint32 My_Mcislfac_Fault;
#endif

#ifdef COMPILE_4_SIMULINK
	#define INITIALIZE_WASH_MOTOR
#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MCUDebug and its variables
 *
 */
void MCUDebug__Initialize(void)
{
#if (MCL_DEBUG_MODE == ENABLED)
#ifdef INITIALIZE_WASH_MOTOR
	Mci__SetWashMotorType(WM_SINGLE_SPEED_SYNCH_SPH_AW);
#endif

#ifdef ENABLE_PILOT_MCI
    Pilot_Relay_Drive_Status = DISABLED;
    Pilot_Relay_Timer = 0;
    Pilot_Relay_Request = FALSE;

    Gpio__PinConfig(PILOT_RELAY_PORT,PILOT_RELAY_PIN,OUTPUT_PUSHPULL);
    Gpio__PinConfig(PILOT_DRIVE_PORT,PILOT_DRIVE_PIN,OUTPUT_PUSHPULL);

    // Default state of the pin
    GPIO__PIN_CLR(PILOT_RELAY_PORT, PILOT_RELAY_PIN); //Pilot Relay OFF
    GPIO__PIN_CLR(PILOT_DRIVE_PORT, PILOT_DRIVE_PIN); //Disable Pilot

    BD_MotorControl = 0;
#endif

#ifdef MCI_AUTOTESTING
    My_Mcislfac_Fault = 0;
#endif

#ifdef TIME_MEASUREMENT
#ifdef UC_STM32G0X_H_
	    TIM_DEF_TYPE * tim;
	    tim = (TIM_DEF_TYPE *) TIM7_BASE;

	    uint32 bus_freq;
	    bus_freq = Micro__GetClock()->PCLK1_Frequency;

	    // 1. If the APB prescaler is set to 1, TIMPCLK frequency is equal to PCLK frequency.
	    // 2. Otherwise, the TIMPCLK frequency is set to twice the PCLK frequency.
	    if ((RCC.CFGR & RCC_CFGR_PPRE) != 0)
	    {
	        bus_freq = bus_freq * 2;
	    }

	    //Enable the clock for TIM7 module
	    RCC.APBENR1 |= RCC_APBENR1_TIM7EN;

	    tim->ARR = (uint16)(0xFFFF);
	    tim->PSC = (uint16)((bus_freq / (TIMER_RESOLUTION_MHZ*1000000)) - 1);


	    //Up-counter , edge-aligned
	    tim->CR1 = (uint16)0x0000; //Clear CR1 Register
	    tim->CR1  |= TIM_CR1_ARPE; //Enable the auto reload feature
	    tim->CR1  |= TIM_CR1_CEN;  //Enable the Timer
#endif
#ifdef UC_RX130_H
	    MSTP_MTU5 = 0;                            /*lint !e923 cast from int to pointer */
	    /*
	     *   PCLK/1   (32Mhz)   ----   0x00
	     *   PCLK/4   (16Mhz)   ----   0x01
	     *   PCLK/16  (2Mhz)    ----   0x02
	     *   PCLK/64  (500khz)  ----   0x03
	     */
	    MTU5.TCRU.BYTE        = 0x00;               //Rising Edge + PCLK (32Mhz) + No Clearing.
	    MTU5.TSTR.BIT.CSTU5   = 1;                  /*lint !e923 cast from int to pointer */
#endif
#endif

#if (SIL_MODE == ENABLED)
	    Slow_Handler_Counter = 125;
#endif
#endif
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      ZC Handler
 *
 */
void MCUDebug_ZeroCross(void)
{

}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Slow Handler
 *
 */
void MCUDebug_SlowHandler(void)
{
#ifdef ENABLE_PILOT_MCI
	if(BD_MotorControl == 1)
	{
		if(MCI_RUN(MOTOR0) == MCI_CMD_ACCEPTED)
		{
			//Pilot_Relay_Request = TRUE;
			BD_MotorControl = -1;
		}
	}

	if(BD_MotorControl == 2)
	{
		if(MCI_STOP(MOTOR0) == MCI_CMD_ACCEPTED)
		{
			//Pilot_Relay_Request = TRUE;
			BD_MotorControl = -2;
		}
	}

	if(BD_MotorControl == 127)
	{
		BD_MotorControl = -127;
		Mci__SetWashMotorType(WM_SINGLE_SPEED_SYNCH_SPH_AW);
	}
#endif
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      MC Handler
 *
 */
void MCUDebug_MCHandler(void)
{
#ifdef ENABLE_PILOT_MCI
	PilotRelay();
#endif

#ifdef MCI_AUTOTESTING
	My_Mcislfac_Fault = Mcl__GetErrorList();
#endif

#if (SIL_MODE == ENABLED)

	Slow_Handler_Counter++;
	if(Slow_Handler_Counter >= 125) //25ms at 5000Hz
	{
		Slow_Handler_Counter = 0;
		Mci__SlowHandler();
	}

	Dbg_Control_Mode = (uint8)Mci__GetAnalog(MOTOR0, MCI_AI_INTERNAL_MCI_STATE);
	Dbg_MciSlfac_Last_Failure = (uint8)Mci__GetErrorList(MOTOR0);
	Dbg_Failure_State = (uint8)Mcl__GetDataSIL(MCL_GD_FAILURE_STATE);
	Dbg_bemf_volt_tmp = (sint16)Mcl__GetDataSIL(MCL_GD_BACK_EMF);
	Dbg_Mcl_Main_Counter = (uint16)Mcl__GetDataSIL(MCL_GD_MAIN_COUNTER);
	Dbg_MciSlfac_Wash_Motor_Type = (uint8)Mcl__GetDataSIL(MCL_GD_MOTOR_TYPE);

	//Digital Channels
	My_GetDigital_0 = Mci__GetDigital(MOTOR0,MCI_DI_MOTOR_STOP);
	My_GetDigital_1 = Mci__GetDigital(MOTOR0,MCI_DI_REACHED_TARGET_SPEED);
	My_GetDigital_2 = Mci__GetDigital(MOTOR0,MCI_DI_IDLE);
	My_GetDigital_3 = Mci__GetDigital(MOTOR0,MCI_DI_MOTOR_RUNNING);

	// Analog Channels
	My_GetAnalog_0 = Mci__GetAnalog(MOTOR0,MCI_AI_CURR_PHASE_P2P);
	My_GetAnalog_1 = Mci__GetAnalog(MOTOR0,MCI_AI_VOLTAGE_PHASE_P2P);
	My_GetAnalog_2 = Mci__GetAnalog(MOTOR0,MCI_AI_LOAD_TORQUE_S16);
#endif
}


#ifdef ENABLE_PILOT_MCI
void PilotRelay(void)
{
    if (Pilot_Relay_Request)
    {
    	GPIO__PIN_SET(PILOT_DRIVE_PORT,PILOT_DRIVE_PIN); //Enable Pilot
        Pilot_Relay_Timer++;

        //Pilot Relay ON
        if (Pilot_Relay_Drive_Status == DISABLED)
        {
            // time to switch ON?
            if (Pilot_Relay_Timer >= PILOT_RELAY_DISABLED_TIME)
            {
                GPIO__PIN_SET(PILOT_RELAY_PORT, PILOT_RELAY_PIN);
                Pilot_Relay_Drive_Status = ENABLED;
                Pilot_Relay_Timer = 0;
            }
        }
        else
        {
            // time to switch OFF?
            if (Pilot_Relay_Timer >= PILOT_RELAY_ENABLED_TIME)
            {
                GPIO__PIN_CLR(PILOT_RELAY_PORT, PILOT_RELAY_PIN);
                Pilot_Relay_Drive_Status = DISABLED;
                Pilot_Relay_Timer = 0;
            }
        }
    }
    else
    {
    	if(Pilot_Relay_Drive_Status == ENABLED)
    	{
    		Pilot_Relay_Timer = 0;
    		GPIO__PIN_CLR(PILOT_RELAY_PORT, PILOT_RELAY_PIN);
    		GPIO__PIN_CLR(PILOT_DRIVE_PORT,PILOT_DRIVE_PIN);
    		Pilot_Relay_Drive_Status = DISABLED;
    	}
    }
}
#endif



//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#ifdef TIME_MEASUREMENT

uint16 Measured_Time_Max;
uint16 Measured_Time_Temp;
uint16 Measured_Time_Counter;

void MCUDebug__MeasTimeSlot(void)
{
#ifdef UC_STM32G0X_H_
	    TIM_DEF_TYPE * tim;
	    tim = (TIM_DEF_TYPE *) TIM7_BASE;

	    uint32 get_time;

	    get_time = (uint32)tim->CNT;

	    Measured_Time_US = (uint16)((get_time)/TIMER_RESOLUTION_MHZ);
	    tim->CNT = 0;
#endif
#ifdef UC_RX130_H
	    uint32 get_time;

	    get_time = (uint32)MTU5.TCNTU;

	    Measured_Time_US = (uint16)((get_time+(TIMER_RESOLUTION_MHZ/2))/TIMER_RESOLUTION_MHZ);
	    MTU5.TCNTU = 0;
#endif
}



void MCUDebug__MeasTimeReset(void)
{
#ifdef UC_STM32G0X_H_
	    TIM_DEF_TYPE * tim;
	    tim = (TIM_DEF_TYPE *) TIM7_BASE;

	    tim->CNT = 0;
#endif

#ifdef UC_RX130_H
	    MTU5.TCNTU = 0;
#endif
}



void MCUDebug__MeasTimeSet(void)
{
#ifdef UC_STM32G0X_H_
	    TIM_DEF_TYPE * tim;
	    tim = (TIM_DEF_TYPE *) TIM7_BASE;

	    uint32 get_time;

	    get_time = (uint32)tim->CNT;

	    Measured_Time_US = (uint16)((uint32)(get_time+32)/TIMER_RESOLUTION_MHZ);
	    tim->CNT = 0;

	    if(Measured_Time_Temp < Measured_Time_US)
	    {
	    	Measured_Time_Temp = Measured_Time_US;
	    }

	    Measured_Time_Counter++;
	    if(Measured_Time_Counter >= 5000)
	    {
	    	Measured_Time_Counter = 0;
	    	Measured_Time_Max = Measured_Time_Temp;
	    	Measured_Time_Temp = 0;
	    }
#endif

#ifdef UC_RX130_H
	    uint32 get_time;

	    get_time = (uint32)MTU5.TCNTU;

	    Measured_Time_US = (uint16)((uint32)(get_time)/TIMER_RESOLUTION_MHZ);
	    MTU5.TCNTU = 0;

	    if(Measured_Time_Temp < Measured_Time_US)
	    {
	    	Measured_Time_Temp = Measured_Time_US;
	    }

	    Measured_Time_Counter++;
	    if(Measured_Time_Counter >= 5000)
	    {
	    	Measured_Time_Counter = 0;
	    	Measured_Time_Max = Measured_Time_Temp;
	    	Measured_Time_Temp = 0;
	    }
#endif
}
#endif
