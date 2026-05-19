/**
 *  @file       AbsoluteHumidityLLI.c
 *
 *  @brief      Module for AbsoluteHumidityLLI readings
 *
 *  @details    This module perform reading of Absolute humidity and perform some filters.
 *
 *              Measuring principle:
 *
 *              The absolute humidity sensor consists of two thermistors which are coupled in series,  when LLI enable pin is high, it will be fed with a
 *              current of approximately 30 mA, which heat them to a working temperature of around 150C.
 *
 *              These two thermistors are mounted in two separate cavities, and connected to a measuring bridge. One cavity is hermetically sealed, while
 *              the other is open to a surrounding air. Humid air going past this cavity will cool the thermistor, causing its resistance to increase and
 *              changing the bridge balance. The bridge has a feedback output connected to LLI feedback pin so that LLI can detect the bridge balance,and
 *              a pulse input connected to  LLI PWM pin so that  LLI can adjust the PWM output to pulse the bridge's resistor  and compensate the voltage
 *              difference to get the bridge balance.
 *
 *              Constantly adjusting PWM output,so that the bridge is always in balance,and the duty can be used to reflect the change of air humidity.
 *
      @startuml{AbsoluteHumidityLLI_SM.png}
        title AbsoluteHumidityLLI State Machine

        ' Use State = Absolute_Humidity_LLI_Data[LLI_Index].State
        [*] --> HEATING

        ' HEATING State - request LLI heating state on entry and wait for event in do
            HEATING: entry /\t CounterInitialize() \n\t\t ErrorStatusInitialize() \n\t\t AbsoluteHumidityHeatingOn() \n\t\t AbsoluteHumidityPWMStartForHeating()
            HEATING: do /\t\t MonitorHeating()
            HEATING --> BALANCING: evBalancing

        ' BALANCING State - request LLI balancing state on entry and wait for event in do
            BALANCING: entry /\t CounterInitialize() \n\t\t AbsoluteHumidityPWMStartForBalancing()
            BALANCING: do /\t\t ExecuteSuccessiveApproximation()
            BALANCING --> REGULATING: evRegulating

        ' REGULATING State - request LLI regulating state on entry and wait for event in do
            REGULATING: entry / CounterInitialize()
            REGULATING: do /\t ExecuteFineTuning() \n\t MonitorUnbalanced()
            REGULATING --> BALANCED: evBalanced
            REGULATING --> BALANCING: evBalancing

        ' BALANCED State - request LLI balanced state on entry and wait for event in do
            BALANCED: entry / CounterInitialize()
            BALANCED: do /\t ExecuteFineTuning() \n\t MonitorBalanced()
            BALANCED --> BALANCING: evBalancing

    @enduml
 *
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------

//lint -e766 Suppress Info: header not used in module 'VirtualGI.c'
#include "SystemConfig.h"

#if (HBL_LLI_NUM_ABSOLUTEHUMIDITY > 0)
#include "AbsoluteHumidityLLI.h"
#include "Hbl.h"
#include "Log.h"
#include "Gpio.h"
#include "Timer.h"
#include "VirtualPin.h"
#include "RTOS.h"

#include <string.h>
#include <limits.h>

//  --- Private Declarations ------------------------------------------------------------------------------------------

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Absolute Humidity LLI State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Absolute Humidity LLI state machine.
typedef enum ABSOLUTE_HUMIDITY_LLI_STATE_ENUM
{
    STATE_ABSOLUTE_HUMIDITY_LLI_NONE       = 0,

    STATE_ABSOLUTE_HUMIDITY_LLI_HEATING    = 1,     //!< (initial_state)
    STATE_ABSOLUTE_HUMIDITY_LLI_BALANCING  = 2,
    STATE_ABSOLUTE_HUMIDITY_LLI_REGULATING = 3,
    STATE_ABSOLUTE_HUMIDITY_LLI_BALANCED   = 4,

    STATE_ABSOLUTE_HUMIDITY_LLI_END        = 5
} ABSOLUTE_HUMIDITY_LLI_STATE_TYPE;

//! The transition variable for the Absolute Humidity LLI state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Absolute_Humidity_LLI_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void AbsoluteHumidityHeatingOn(void);
static void AbsoluteHumidityPWMStartForBalancing(void);
static void AbsoluteHumidityPWMStartForHeating(void);
static void CounterInitialize(void);
static void ErrorStatusInitialize(void);
static void ExecuteFineTuning(void);
static void ExecuteSuccessiveApproximation(void);
static void MonitorBalanced(void);
static void MonitorHeating(void);
static void MonitorUnbalanced(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void AbsoluteHumidityLLIExecute(void);
static void AbsoluteHumidityLLIInitialize(void);
static void EventBalanced(void);
static void EventBalancing(void);
static void EventRegulating(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//  --- Private Definitions -------------------------------------------------------------------------------------------
#define ABSOLUTE_HUMIDITY_PWM_FREQUENCY         4000
#define ABSOLUTE_HUMIDITY_PWM_DUTY_FOR_HEATING   312    // 3.12% - for 14 bits PWM, the value is 0x200
#define ABSOLUTE_HUMIDITY_PWM_MIN_DUTY            39    // 0.39% - for 14 bits PWM, the value is 0x40
#define ABSOLUTE_HUMIDITY_PWM_MID_DUTY          5000    //50.00% - for 14 bits PWM, the value is 0x2000
#define ABSOLUTE_HUMIDITY_PWM_MAX_DUTY          9922    //99.22% - for 14 bits PWM, the value is 0x3F80
#define ABSOLUTE_HUMIDITY_PWM_LOW_DUTY          1875    //18.75% - for 14 bits PWM, the value is 0xC00
#define ABSOLUTE_HUMIDITY_PWM_HI_DUTY           9374    //93.74% - for 14 bits PWM, the value is 0x3BFF

#define ABSOLUTE_HUMIDITY_HEATING_TIMEOUT       600     //60s
#define ABSOLUTE_HUMIDITY_REGULATING_DELAY      20      //2s
#define ABSOLUTE_HUMIDITY_ERROR_DELAY           10      //1s

#define ABSOLUTE_HUMIDITY_FILTER_B1             35
#define ABSOLUTE_HUMIDITY_FILTER_A2_MINUS       186

#define ABSOLUTE_HUMIDITY_FB_HIGH_THRESHOLD     3
#define ABSOLUTE_HUMIDITY_FB_LOW_THRESHOLD      -3
#define ABSOLUTE_HUMIDITY_HEATING_END_THRESHOLD 3

#define ENABLE_PIN                              0
#define PWM_PIN                                 1
#define FEEDBACK_PIN                            2

PACKED typedef struct
{
    HBL_LLI_TYPE *LLI;
    TIMER_PARAMETER_TYPE Timer_Parameter;
    uint8 Enable;
    ABSOLUTE_HUMIDITY_LLI_STATE_TYPE State;
    ABSOLUTE_HUMIDITY_LLI_GET_DATA_TYPE Get_Data;
    uint16 Test_Duty;
    uint16 Output_Duty;
    uint32 Filter_Delay;
    uint16 Filtered_PWM;
    sint8 Feedback_Status;
    uint8 Error_Status;
    uint8 Error_Counter;
    uint16 Delay_Counter;
    uint8 Asynch_Process_Counter;
    uint8 Heating_Counter;
    sint8 Direction_Counter;
} ABSOLUTE_HUMIDITY_LLI_DATA_TYPE;

//! Tracks the number of absolute humidity LLI allocations.
static uint8 Absolute_Humidity_LLI_Allocation;

//! LLI index for current running absolute humidity state machine.
static uint8 LLI_Index;

//! Absolute humidity data array. One entry per absolute humidity instance.
static ABSOLUTE_HUMIDITY_LLI_DATA_TYPE Absolute_Humidity_LLI_Data[HBL_LLI_NUM_ABSOLUTEHUMIDITY];

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_ABSOLUTE_HUMIDITY_LLI

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_ABSOLUTE_HUMIDITY_LLI_LOG_MESSAGE_ID_ENUM
{
    CONFIGURE_ENABLE_PIN_FAILED                 = 1,
    CONFIGURE_FEEDBACK_PIN_FAILED               = 2,
    CONFIGURE_PWM_PIN_FAILED                    = 3,
    CONFIGURE_PWM_FAILED                        = 4,
    HUMIDITY_SENSOR_ERROR_PRESENT_IN_UNBALANCED = 5,
    HUMIDITY_SENSOR_ERROR_PRESENT_IN_BALANCED   = 6,
    HUMIDITY_SENSOR_ERROR_CANCEL                = 7,
} MODULE_ABSOLUTE_HUMIDITY_LLI_LOG_MESSAGE_ID_TYPE;

//  --- Private Function Prototypes -----------------------------------------------------------------------------------
static void AbsoluteHumidityPWMSetDuty(uint16 duty);
static uint16 AbsoluteHumidityPWMGetDuty(void);
static BOOL_TYPE AbsoluteHumidityGetFeedback(void);
static uint16 AbsoluteHumidityFilter(uint16 sample);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Initializes the AbsoluteHumidityLLI module and its variables
 */
void AbsoluteHumidityLLI__Initialize(void)
{
    Absolute_Humidity_LLI_Allocation = 0;
    memset(&Absolute_Humidity_LLI_Data,0,sizeof(Absolute_Humidity_LLI_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Checks if there is free space and reserve a new instance
 * @return	index of AbsoluteHumidityLLI instance, 0xFF if all HBL_LLI_NUM_ABSOLUTEHUMIDITY are already allocated
 */
uint8 AbsoluteHumidityLLI__Allocate(void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    if(Absolute_Humidity_LLI_Allocation < HBL_LLI_NUM_ABSOLUTEHUMIDITY)
    {
        retval = Absolute_Humidity_LLI_Allocation;
        Absolute_Humidity_LLI_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Initializes a new instance of AbsoluteHumidityLLI
 * @param input		pointer to HBL_LLI_TYPE structure reserved for that instance
 * @param status
 */
PASS_FAIL_TYPE AbsoluteHumidityLLI__InitializeInstance(HBL_LLI_TYPE *input)
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 table_index;

    if(input->Data_Index < Absolute_Humidity_LLI_Allocation)
    {
        LLI_Index = input->Data_Index;
        Absolute_Humidity_LLI_Data[LLI_Index].LLI = input;

        //! Initialize AH enable pin
        table_index = Absolute_Humidity_LLI_Data[LLI_Index].LLI->Pin_Ptr[ENABLE_PIN];
        if(VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
        {
            Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, FALSE);
            Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);

            //! Initialize AH feedback pin
            table_index = Absolute_Humidity_LLI_Data[LLI_Index].LLI->Pin_Ptr[FEEDBACK_PIN];
            if(VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
            {
                Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);

                //! Initialize AH pwm pin
                table_index = Absolute_Humidity_LLI_Data[LLI_Index].LLI->Pin_Ptr[PWM_PIN];
                if(VIRTUALPIN_TABLE[table_index].port != PORT_NULL)
                {
                    Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, FALSE);
                    Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);

                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Timer         = (TIMER_TYPE)VIRTUALPIN_TABLE[table_index].device;
                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Channel       = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[table_index].param0;
                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Mode          = TIMER_MODE_PWM;
                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Port          = VIRTUALPIN_TABLE[table_index].port;
                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Pin           = VIRTUALPIN_TABLE[table_index].pin;
                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Pin_ReMap     = VIRTUALPIN_TABLE[table_index].pin_remap;
                    Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter.Polarity      = TIMER_POLARITY_ACTIVE_HIGH;

                    if(Timer__Config(&Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter) == PASS)
                    {
                        //! Initialize Absolute humidity LLI state machine
                        AbsoluteHumidityLLIInitialize();

                        //! Enable Absolute humidity LLI
                        Absolute_Humidity_LLI_Data[LLI_Index].Enable = TRUE;

                        retval = PASS;
                    }
                    else
                    {
                        LOG_ADD_EXCEPTION(CONFIGURE_PWM_FAILED, LLI_Index);
                    }
                }
                else
                {
                    LOG_ADD_EXCEPTION(CONFIGURE_FEEDBACK_PIN_FAILED, LLI_Index);
                }
            }
            else
            {
                LOG_ADD_EXCEPTION(CONFIGURE_FEEDBACK_PIN_FAILED, LLI_Index);
            }
        }
        else
        {
            LOG_ADD_EXCEPTION(CONFIGURE_ENABLE_PIN_FAILED, LLI_Index);
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get function to provide Data of an AbsoluteHumidityLLI instance
 *
 * @param   input   pointer to HBL_LLI_TYPE structure instance
 *
 * @return	pointer to a ABSOLUTE_HUMIDITY_LLI_DATA_TYPE which contains the specified input state and pwm count
 * @return  NULL is the input parameter is invalid
 */
void * AbsoluteHumidityLLI__Get(HBL_LLI_TYPE * input)
{
    if(input->Data_Index < Absolute_Humidity_LLI_Allocation)
    {
        LLI_Index = input->Data_Index;

        if(Absolute_Humidity_LLI_Data[LLI_Index].Enable == TRUE)
        {
            if(Absolute_Humidity_LLI_Data[LLI_Index].Error_Status == TRUE)
            {
                Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.State = ABSOLUTE_HUMIDITY_LLI_STATE_ERROR;
            }
            else
            {
                switch(Absolute_Humidity_LLI_Data[LLI_Index].State)
                {
                    case STATE_ABSOLUTE_HUMIDITY_LLI_HEATING:
                        Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.State = ABSOLUTE_HUMIDITY_LLI_STATE_HEATING;
                        break;

                    case STATE_ABSOLUTE_HUMIDITY_LLI_BALANCING:
                        Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.State = ABSOLUTE_HUMIDITY_LLI_STATE_BALANCING;
                        break;

                    case STATE_ABSOLUTE_HUMIDITY_LLI_REGULATING:
                        Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.State = ABSOLUTE_HUMIDITY_LLI_STATE_REGULATING;
                        break;

                    case STATE_ABSOLUTE_HUMIDITY_LLI_BALANCED:
                        Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.State = ABSOLUTE_HUMIDITY_LLI_STATE_BALANCED;
                        break;

                    default:
                        Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.State = ABSOLUTE_HUMIDITY_LLI_STATE_OFF;
                        break;
                }
            }

            //! Disable the OS scheduler
            OS_EnterRegion();

            Absolute_Humidity_LLI_Data[LLI_Index].Get_Data.Value = Absolute_Humidity_LLI_Data[LLI_Index].Filtered_PWM;

            //! Enabled the OS scheduler
            OS_LeaveRegion();

            return (&Absolute_Humidity_LLI_Data[LLI_Index].Get_Data);
        }
        else
        {
            return (NULL);
        }
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	AsynchProcess called at every 5ms to generate Absolute humidity 100ms handler
 * @param   input   pointer to HBL_LLI_TYPE structure instance
 * @param   status
 * @return	PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE AbsoluteHumidityLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if(input->Data_Index < Absolute_Humidity_LLI_Allocation)
    {
        LLI_Index = input->Data_Index;

        if(Absolute_Humidity_LLI_Data[LLI_Index].Enable == TRUE)
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Asynch_Process_Counter++;
            if(Absolute_Humidity_LLI_Data[LLI_Index].Asynch_Process_Counter >= 20)
            {
                Absolute_Humidity_LLI_Data[LLI_Index].Asynch_Process_Counter = 0;

                AbsoluteHumidityLLIExecute();

                //! Disable the OS scheduler
                OS_EnterRegion();

                Absolute_Humidity_LLI_Data[LLI_Index].Filtered_PWM = AbsoluteHumidityFilter(AbsoluteHumidityPWMGetDuty());

                //! Enabled the OS scheduler
                OS_LeaveRegion();

                retval = TRUE;
            }
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read on ZC Peak angle (90, 270)
 * @param   input   pointer to HBL_LLI_TYPE structure instance
 * @param   status
 * @param   peak_polarity   TRUE for 90, FALSE for 270
 * @return  TRUE if a new sample is ready, FALSE otherwise
 */
BOOL_TYPE AbsoluteHumidityLLI__PeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity)
{
    BOOL_TYPE retval = FALSE;

    if(input->Data_Index < Absolute_Humidity_LLI_Allocation)
    {
        //! Sample the feedback at ZC peak angle 270 only
        if(peak_polarity == FALSE)
        {
            //! Save LLI_Index before changing it in peak process
            uint8 LLI_Index_Bak = LLI_Index;

            LLI_Index = input->Data_Index;

            if(Absolute_Humidity_LLI_Data[LLI_Index].Enable == TRUE)
            {
                if(AbsoluteHumidityGetFeedback() == TRUE)
                {
                    if(Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status >= ABSOLUTE_HUMIDITY_FB_HIGH_THRESHOLD)
                    {
                        Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status = ABSOLUTE_HUMIDITY_FB_HIGH_THRESHOLD;
                    }
                    else
                    {
                        Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status++;
                    }
                }
                else
                {
                    if(Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status <= ABSOLUTE_HUMIDITY_FB_LOW_THRESHOLD)
                    {
                        Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status = ABSOLUTE_HUMIDITY_FB_LOW_THRESHOLD;
                    }
                    else
                    {
                        Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status--;
                    }
                }

                retval = TRUE;
            }

            //! Restore LLI_Index to previous value before exiting peak process
            LLI_Index = LLI_Index_Bak;
        }
    }

    return (retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize the error status before a new measurement.
 */
static void ErrorStatusInitialize(void)
{
    Absolute_Humidity_LLI_Data[LLI_Index].Error_Status = FALSE;
    Absolute_Humidity_LLI_Data[LLI_Index].Error_Counter = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize the counters when state is changed.
 */
static void CounterInitialize(void)
{
    Absolute_Humidity_LLI_Data[LLI_Index].Delay_Counter = 0;
    Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Execute successive approximation to get the measuring bridge approximated balance,
 *          then trigger regulating event.
 */
static void ExecuteSuccessiveApproximation(void)
{
    uint16 duty;

    Absolute_Humidity_LLI_Data[LLI_Index].Test_Duty >>= 1;

    if(Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status > 0)
    {
        duty = AbsoluteHumidityPWMGetDuty() - Absolute_Humidity_LLI_Data[LLI_Index].Test_Duty;
    }
    else
    {
        duty = AbsoluteHumidityPWMGetDuty() + Absolute_Humidity_LLI_Data[LLI_Index].Test_Duty;
    }

    AbsoluteHumidityPWMSetDuty(duty);

    Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status = 0;

   if(Absolute_Humidity_LLI_Data[LLI_Index].Test_Duty == 1)
   {
       EventRegulating();
   }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Execute Fine turning to get the measuring bridge balance.
 */
static void ExecuteFineTuning(void)
{
    uint16 duty;

    if(Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status > 0)
    {
        duty = AbsoluteHumidityPWMGetDuty() - 1;

        if(Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter > 0)
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter = 0;
        }
        else if(Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter > SCHAR_MIN)
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter--;
        }
    }
    else
    {
        duty = AbsoluteHumidityPWMGetDuty() + 1;

        if(Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter < 0)
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter = 0;
        }
        else if(Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter < SCHAR_MAX)
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter++;
        }
    }

    AbsoluteHumidityPWMSetDuty(duty);

    Absolute_Humidity_LLI_Data[LLI_Index].Feedback_Status = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Heat absolute humidity sensor until FB signal become low level or timeout, then trigger balancing event.
 *
 */
static void MonitorHeating(void)
{
    Absolute_Humidity_LLI_Data[LLI_Index].Delay_Counter++;
    if(Absolute_Humidity_LLI_Data[LLI_Index].Delay_Counter >  ABSOLUTE_HUMIDITY_HEATING_TIMEOUT)
    {
        EventBalancing();
    }
    else
    {
        if(AbsoluteHumidityGetFeedback() == FALSE)
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Heating_Counter++;
            if(Absolute_Humidity_LLI_Data[LLI_Index].Heating_Counter > ABSOLUTE_HUMIDITY_HEATING_END_THRESHOLD)
            {
                EventBalancing();
            }
        }
        else
        {
            Absolute_Humidity_LLI_Data[LLI_Index].Heating_Counter = 0;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Monitor unbalanced after entering regulating state for 2s, if PWM is out of range or PWM continue
 *          increasing or decreasing by 10, trigger balancing event; Otherwise, trigger balanced event.
 *
 */
static void MonitorUnbalanced(void)
{
    Absolute_Humidity_LLI_Data[LLI_Index].Delay_Counter++;
     if(Absolute_Humidity_LLI_Data[LLI_Index].Delay_Counter > ABSOLUTE_HUMIDITY_REGULATING_DELAY)
     {
         if((AbsoluteHumidityPWMGetDuty() < ABSOLUTE_HUMIDITY_PWM_LOW_DUTY) || (AbsoluteHumidityPWMGetDuty() > ABSOLUTE_HUMIDITY_PWM_HI_DUTY))
         {
             if(Absolute_Humidity_LLI_Data[LLI_Index].Error_Counter >= ABSOLUTE_HUMIDITY_ERROR_DELAY)
             {
                 Absolute_Humidity_LLI_Data[LLI_Index].Error_Counter = ABSOLUTE_HUMIDITY_ERROR_DELAY;
                 if(Absolute_Humidity_LLI_Data[LLI_Index].Error_Status == FALSE)
                 {
                     Absolute_Humidity_LLI_Data[LLI_Index].Error_Status = TRUE;
                     LOG_ADD_EXCEPTION(HUMIDITY_SENSOR_ERROR_PRESENT_IN_UNBALANCED, LLI_Index);
                 }
             }
             else
             {
                 Absolute_Humidity_LLI_Data[LLI_Index].Error_Counter++;
             }
         }
         else
         {
             if(Absolute_Humidity_LLI_Data[LLI_Index].Error_Status == TRUE)
             {
                 Absolute_Humidity_LLI_Data[LLI_Index].Error_Status = FALSE;
                 LOG_ADD_EXCEPTION(HUMIDITY_SENSOR_ERROR_CANCEL, LLI_Index);
             }
             Absolute_Humidity_LLI_Data[LLI_Index].Error_Counter = 0;
         }

         if(((Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter < -10) || (Absolute_Humidity_LLI_Data[LLI_Index].Direction_Counter > 10)) || (Absolute_Humidity_LLI_Data[LLI_Index].Error_Status == TRUE))
         {
             EventBalancing();
         }
         else
         {
             EventBalanced();
         }
     }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Monitor balanced in balancing state, if PWM is out of range, then trigger balancing event to re-balance
 *          the measuring bridge.
 */
static void MonitorBalanced(void)
{
    if((AbsoluteHumidityPWMGetDuty() < ABSOLUTE_HUMIDITY_PWM_LOW_DUTY) || (AbsoluteHumidityPWMGetDuty() > ABSOLUTE_HUMIDITY_PWM_HI_DUTY))
    {
        Absolute_Humidity_LLI_Data[LLI_Index].Error_Status = TRUE;
        LOG_ADD_EXCEPTION(HUMIDITY_SENSOR_ERROR_PRESENT_IN_BALANCED, LLI_Index);
        EventBalancing();
    }
}


//----------------------------------------------------------------------------------------------
/**
 *  @brief      It Start heating absolute humidity sensor.
 */
static void AbsoluteHumidityHeatingOn(void)
{
    uint8 table_index = Absolute_Humidity_LLI_Data[LLI_Index].LLI->Pin_Ptr[ENABLE_PIN];

    Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, TRUE);

    Absolute_Humidity_LLI_Data[LLI_Index].Heating_Counter = 0;
}


//----------------------------------------------------------------------------------------------
/**
 *  @brief      It Start absolute humidity PWM with specified duty for heating state.
 */
static void AbsoluteHumidityPWMStartForHeating(void)
{
    (void)Timer__PwmStart(&Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter, ABSOLUTE_HUMIDITY_PWM_FREQUENCY, ABSOLUTE_HUMIDITY_PWM_DUTY_FOR_HEATING);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Start absolute humidity PWM with 50% duty for balancing state.
 */
static void AbsoluteHumidityPWMStartForBalancing(void)
{
    Absolute_Humidity_LLI_Data[LLI_Index].Test_Duty = ABSOLUTE_HUMIDITY_PWM_MID_DUTY;
    (void)Timer__PwmSetDutyCycle(&Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter, ABSOLUTE_HUMIDITY_PWM_MID_DUTY);
}


//----------------------------------------------------------------------------------------------
/**
 *  @brief      It set up the duty of humidity PWM.
 */
static void AbsoluteHumidityPWMSetDuty(uint16 duty)
{
    if(duty > ABSOLUTE_HUMIDITY_PWM_MIN_DUTY && duty < ABSOLUTE_HUMIDITY_PWM_MAX_DUTY)
    {
        Absolute_Humidity_LLI_Data[LLI_Index].Output_Duty = duty;
        (void)Timer__PwmSetDutyCycle(&Absolute_Humidity_LLI_Data[LLI_Index].Timer_Parameter, duty);
    }
}


//----------------------------------------------------------------------------------------------
/**
 *  @brief      It return the duty of humidity PWM.
 */
static uint16 AbsoluteHumidityPWMGetDuty(void)
{
    return Absolute_Humidity_LLI_Data[LLI_Index].Output_Duty;
}


//----------------------------------------------------------------------------------------------
/**
 *  @brief      It return absolute humidity feedback pin status.
 */
static BOOL_TYPE AbsoluteHumidityGetFeedback(void)
{
    uint8 table_index = Absolute_Humidity_LLI_Data[LLI_Index].LLI->Pin_Ptr[FEEDBACK_PIN];

    BOOL_TYPE pin_state = Gpio__PinRead(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin);

    return pin_state;
}


//----------------------------------------------------------------------------------------------
/**
 *  @brief      It filter the duty value and return filtered PWM value.
 *
        Humidity signal filter. Filters 14-bit signal from PWM Data Buffer.

        1:st degree butterworth lowpass canonical IIR-filter,
        with Fc = 0.1*Fn.  (Fc= Frequency cutoff, Fn = nyquist)

        Filter parameters:
        a1=1
        a2=-0.7265
        b1=0.1367
        b2=0.1367

        a2-b2 normalized to (1(decimal)=256) gives: -a2=186
                                                    b1=b2=35.

        All calculations is 32-bit. No truncation except for last division with 256.

        Canonical structure:

        x(n) >--------------------------
                        | b2            | b1
                        |      _        |
                        + --->|T| ----> + --------------> y(n)
                        |      -             |
                        | -a2                |
                        <--------------------

        y(n) = b1*x(n) + b2*x(n-1) - a2*y(n-1)
 */
static uint16 AbsoluteHumidityFilter(uint16 sample)
{
    uint32 newSample, tempValue;

    // Conver duty (0~10000) to 14 bit pwm (0~16383)
    tempValue = (uint32)sample*(uint32)16383;
    tempValue /= 10000;
    sample = (uint16)tempValue;

    // Calculate new filter output value
    newSample = (uint32)sample*(uint32)ABSOLUTE_HUMIDITY_FILTER_B1;    // newSample = 256*b1*x(n)
    tempValue = newSample;
    newSample += (uint32)(Absolute_Humidity_LLI_Data[LLI_Index].Filter_Delay); // newSample += 256*b2*x(n-1) + 256*(-a2)*y(n-1) => 256y(n)

    // calculate new filter delay tap value
    Absolute_Humidity_LLI_Data[LLI_Index].Filter_Delay = tempValue; // Filter_Delay = 256*b2*x(n)
    Absolute_Humidity_LLI_Data[LLI_Index].Filter_Delay += ((((uint32)(newSample))*(uint32)ABSOLUTE_HUMIDITY_FILTER_A2_MINUS) >> 8); // Filter_Delay += 256*(-a2) * 256*y(n) / 256 => 256*b2*x(n) + 256*(-a2)*y(n)

    // Return new filtered value
    return((uint16)(newSample >>= 8)); // newSample/256 => y(n)
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Absolute Humidity LLI State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Absolute Humidity LLI state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AbsoluteHumidityLLIExecute(void)
{
    Absolute_Humidity_LLI_Transitioned = FALSE;
    switch(Absolute_Humidity_LLI_Data[LLI_Index].State)
    {
        case STATE_ABSOLUTE_HUMIDITY_LLI_HEATING:
            MonitorHeating();                       // Do function.
            break;

        case STATE_ABSOLUTE_HUMIDITY_LLI_BALANCING:
            ExecuteSuccessiveApproximation();       // Do function.
            break;

        case STATE_ABSOLUTE_HUMIDITY_LLI_REGULATING:
            ExecuteFineTuning();                    // Do function.
            if (Absolute_Humidity_LLI_Transitioned == FALSE)
            {
                MonitorUnbalanced();                // Do function.
            }
            break;

        case STATE_ABSOLUTE_HUMIDITY_LLI_BALANCED:
            ExecuteFineTuning();                    // Do function.
            if (Absolute_Humidity_LLI_Transitioned == FALSE)
            {
                MonitorBalanced();                  // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of Absolute_Humidity_LLI_Data[LLI_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Absolute Humidity LLI state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AbsoluteHumidityLLIInitialize(void)
{
    Absolute_Humidity_LLI_Data[LLI_Index].State = STATE_ABSOLUTE_HUMIDITY_LLI_HEATING;
    Absolute_Humidity_LLI_Transitioned = FALSE;
    CounterInitialize();                            // Entry Function.
    if (Absolute_Humidity_LLI_Transitioned == FALSE)
    {
        ErrorStatusInitialize();                    // Entry Function.
        if (Absolute_Humidity_LLI_Transitioned == FALSE)
        {
            AbsoluteHumidityHeatingOn();            // Entry Function.
            if (Absolute_Humidity_LLI_Transitioned == FALSE)
            {
                AbsoluteHumidityPWMStartForHeating(); // Entry Function.
            }
        }
    }
    Absolute_Humidity_LLI_Transitioned = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Balanced Event for the Absolute Humidity LLI state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBalanced(void)
{
    switch (Absolute_Humidity_LLI_Data[LLI_Index].State)
    {
        case STATE_ABSOLUTE_HUMIDITY_LLI_REGULATING:
            Absolute_Humidity_LLI_Data[LLI_Index].State = STATE_ABSOLUTE_HUMIDITY_LLI_BALANCED;
            CounterInitialize();                    // Entry Function.
            Absolute_Humidity_LLI_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Absolute_Humidity_LLI_Data[LLI_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Balancing Event for the Absolute Humidity LLI state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBalancing(void)
{
    switch (Absolute_Humidity_LLI_Data[LLI_Index].State)
    {
        case STATE_ABSOLUTE_HUMIDITY_LLI_HEATING:
        case STATE_ABSOLUTE_HUMIDITY_LLI_REGULATING:
        case STATE_ABSOLUTE_HUMIDITY_LLI_BALANCED:
            Absolute_Humidity_LLI_Data[LLI_Index].State = STATE_ABSOLUTE_HUMIDITY_LLI_BALANCING;
            Absolute_Humidity_LLI_Transitioned = FALSE;
            CounterInitialize();                    // Entry Function.
            if (Absolute_Humidity_LLI_Transitioned == FALSE)
            {
                AbsoluteHumidityPWMStartForBalancing(); // Entry Function.
            }
            Absolute_Humidity_LLI_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Absolute_Humidity_LLI_Data[LLI_Index].State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Regulating Event for the Absolute Humidity LLI state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRegulating(void)
{
    switch (Absolute_Humidity_LLI_Data[LLI_Index].State)
    {
        case STATE_ABSOLUTE_HUMIDITY_LLI_BALANCING:
            Absolute_Humidity_LLI_Data[LLI_Index].State = STATE_ABSOLUTE_HUMIDITY_LLI_REGULATING;
            CounterInitialize();                    // Entry Function.
            Absolute_Humidity_LLI_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Absolute_Humidity_LLI_Data[LLI_Index].State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif
