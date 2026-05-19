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
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "LowPower.h" 

#include "GPio.h"
#include "Hbl.h"
#include "Mode.h"
#include "Timer.h"
#include "uc.h"
#include "VirtualPin.h"
#include "Win.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define LOWPOWER_MAIN_THREAD_LONG_INTERVAL		50
#define LOWPOWER_VALID_WAKEUP_TIME				(100/LOWPOWER_MAIN_THREAD_LONG_INTERVAL)
#define TIM_SR_CCIF_FOR_ALL_CHANNELS    		(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF)

//!  The index of the flow meter pin in the VIRTUALPIN_TABLE
#define FLOW_METER_VIRTUAL_PIN_INDEX	49
//!  The door switch port
#define DOOR_SWITCH_PORT				(GPIO_PORT_TYPE)PORTD
//!  The door switch pin
#define DOOR_SWITCH_PIN					4
//!  The float switch port
#define FLOAT_SWITCH_PORT				(GPIO_PORT_TYPE)PORTB
//!  The float switch pin
#define FLOAT_SWITCH_PIN				9

static uint8 LowPower_Timeout;
static uint8 LowPower_Timeout_Float_Switch;
static uint8 LowPower_Timeout_Door_Open;
static BOOL_TYPE LowPower_Flow_Meter_Pulse;
static BOOL_TYPE LP_Door_Closed_Old;
static BOOL_TYPE LP_Door_Open_Wake_Up_Enable;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE GetWinWakeUp(void);
static BOOL_TYPE GetFloatSwitchWakeUp(void);
static BOOL_TYPE GetDoorOpenWakeUp(void);
static BOOL_TYPE GetFlowMeterWakeUp(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module LowPower and its variables
 *
 */
void LowPower__Initialize(void)
{
	LowPower_Timeout = 0;
	LowPower_Timeout_Float_Switch = 0;
	LowPower_Timeout_Door_Open = 0;

	LP_Door_Closed_Old = FALSE;
	LP_Door_Open_Wake_Up_Enable = FALSE;

	LowPower_Flow_Meter_Pulse = FALSE;
	//Flow Meter Timer Interrup
	if (Mode__GetMode() == MODE_LOWPOWER)
	{
		PASS_FAIL_TYPE ret_val = FAIL;
		TIMER_POLARITY_TYPE polarity = TIMER_POLARITY_ACTIVE_LOW;
		TIMER_CAPTURE_EDGE_TYPE edge = TIMER_CAPTUREON_FALLING_EDGE;
		TIMER_PARAMETER_TYPE    timer_parameter;

		timer_parameter.Timer      = (TIMER_TYPE)VIRTUALPIN_TABLE[FLOW_METER_VIRTUAL_PIN_INDEX].device;
		timer_parameter.Channel    = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[FLOW_METER_VIRTUAL_PIN_INDEX].param0;
		timer_parameter.Mode       = TIMER_MODE_INPUT_CAPTURE;
		timer_parameter.Port       = VIRTUALPIN_TABLE[FLOW_METER_VIRTUAL_PIN_INDEX].port;
		timer_parameter.Pin        = VIRTUALPIN_TABLE[FLOW_METER_VIRTUAL_PIN_INDEX].pin;
		timer_parameter.Pin_ReMap  = VIRTUALPIN_TABLE[FLOW_METER_VIRTUAL_PIN_INDEX].pin_remap;
		timer_parameter.Polarity   = polarity;

		ret_val = Timer__Config(&timer_parameter);
		ret_val &= Timer__CaptureStart(&timer_parameter, TIMER_CAPTURE_PULSE, edge);
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It manage the LowPower WakeUp mode
 *
 * @return
 */
BOOL_TYPE LowPower__IsWakeupValid(void)
{
    BOOL_TYPE result;

    result = FALSE;

    if ((GetWinWakeUp() == TRUE) ||
    	(GetFloatSwitchWakeUp() == TRUE) ||
		(GetDoorOpenWakeUp() == TRUE) ||
		(GetFlowMeterWakeUp() == TRUE))
    {
    	result = TRUE;
    }

	return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer16 - IRQ
 *  @details - This API manages the Timer16 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER16 ------ CHANNEL1  :PORTB.PIN8,PORTA.PIN6
 *              If the Input captured interrupt is configured for PORTB.PIN8,PORTA.PIN6 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM16_IRQHANDLER                IntMgr__Timer16IrqHandler    //This handles the TIMER16 Capture/Compare interrupt
 */
void LowPower__Timer16IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_16.SR & TIM_16.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_16.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_16.CCMR2 << 16) | (TIM_16.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_16.DIER &= (uint16)(~(1 << chnl));
#if ((HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0) || (HBLACU_ZEROCROSS_FEATURE == ENABLED))
                if (Mode__GetMode() == MODE_NORMAL)
                {
                	Hbl__OCIntHandler(TIMER16, (TIMER_CHANNEL_TYPE)chnl);
                }
#endif
            }
            else
            {
                if (Timer_ProcessCapture(TIMER16, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                	if (Mode__GetMode() == MODE_LOWPOWER)
                	{
                    	LowPower_Flow_Meter_Pulse = TRUE;
                	}
#if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
                	else if (Mode__GetMode() == MODE_NORMAL)
                	{
                		Hbl__ICIntHandler(TIMER16, (TIMER_CHANNEL_TYPE)chnl);
                	}
                	else
                	{
                		// Do nothing
                	}
#endif
                }
            }
        }
        irq_src = irq_src >> 1;
    }
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It manage the LowPower WakeUp by WIN
 *
 * @return
 */
static BOOL_TYPE GetWinWakeUp(void)
{
	if (Win__GetBusStatus(WIN_BUS_STATUS_RX_PIN) == 0)
	{
		LowPower_Timeout++;
	}
	else
	{
		if (LowPower_Timeout != 0)
		{
			LowPower_Timeout--;
		}
	}

	if (LowPower_Timeout > LOWPOWER_VALID_WAKEUP_TIME)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}


/**
 *  @brief      It manage the LowPower WakeUp by FloatSwitch
 *
 * @return
 */
static BOOL_TYPE GetFloatSwitchWakeUp(void)
{
	if (Gpio__PinRead(FLOAT_SWITCH_PORT, FLOAT_SWITCH_PIN) == TRUE)
	{
		LowPower_Timeout_Float_Switch ++;
	}
	else
	{
		if (LowPower_Timeout_Float_Switch != 0)
		{
			LowPower_Timeout_Float_Switch--;
		}
	}

	if (LowPower_Timeout_Float_Switch > LOWPOWER_VALID_WAKEUP_TIME)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}


/**
 *  @brief      It manage the LowPower WakeUp by Door Open
 *
 * @return
 */
static BOOL_TYPE GetDoorOpenWakeUp(void)
{
	BOOL_TYPE is_door_closed = Gpio__PinRead(DOOR_SWITCH_PORT, DOOR_SWITCH_PIN);

	if ((is_door_closed == FALSE) && (LP_Door_Closed_Old == TRUE))
	{
		// Transition Door Closed -> Door Open
		LP_Door_Open_Wake_Up_Enable = TRUE;
	}

	if (LP_Door_Open_Wake_Up_Enable == TRUE)
	{
		if (is_door_closed == FALSE)
		{
			// Door Open
			LowPower_Timeout_Door_Open++;
		}
		else
		{
			// Door Closed
			if (LowPower_Timeout_Door_Open != 0)
			{
				LowPower_Timeout_Door_Open--;
			}
		}
	}

	LP_Door_Closed_Old = is_door_closed;

	if (LowPower_Timeout_Door_Open > LOWPOWER_VALID_WAKEUP_TIME)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}


/**
 *  @brief      It manage the LowPower WakeUp by Flow Meter
 *
 * @return
 */
static BOOL_TYPE GetFlowMeterWakeUp(void)
{
	if (LowPower_Flow_Meter_Pulse == TRUE)
	{
		LowPower_Flow_Meter_Pulse = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}


