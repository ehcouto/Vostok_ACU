/**
 *  @file       BoardDebugging.c
 *
 *  @brief      Debugging features
 *
 *  @details
 *
 *  @section
 *
 *  @copyright  Copyright 2013-$Date: 2016/01/11 12:30:13CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------

#include "BoardDebugging.h"
#include "BoardDebugging_prv.h"
#include "mci.h"

#ifndef BD_RESET
    #define BD_RESET                 DISABLED
#endif

#ifndef BD_MCI_CMD
    #define BD_MCI_CMD               DISABLED
#endif

#ifndef BD_ADC_NOISE_ASSESSMENT
    #define BD_ADC_NOISE_ASSESSMENT  DISABLED
#endif

#ifndef BD_PWM_ACCESS
    #define BD_PWM_ACCESS            DISABLED
#endif

#ifndef BD_ADC_SYNCH_ADJUST
    #define BD_ADC_SYNCH_ADJUST      DISABLED
#endif

#ifndef DEBUG_MASTERCOMMANDER
    #define DEBUG_MASTERCOMMANDER    DISABLED
#endif

#ifndef BD_LED
    #define BD_LED                   DISABLED
#endif

#ifndef BD_INTERRUPT_ANALYSIS
    #define BD_INTERRUPT_ANALYSIS    DISABLED
#endif

#if (BD_LED == ENABLED)
    #define LED_INIT_COUNTER         (uint16)(LED_INIT_COUNTER_MS / 25)
    #define LED_TIMER                (uint16)(LED_TIMER_MS / 25)
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (DEBUG_MASTERCOMMANDER == ENABLED)
    #include "MasterCommander.h"
#endif


#if (BD_MCI_CMD == ENABLED)
    typedef enum
    {
        SELECT_STOP,
        SELECT_SPEED,
        SELECT_MANUAL,
        SELECT_PWM,
        SELECT_DISABLED,
        SELECT_PLTSPECIFIC,
    } SELECT_SPD_MANUAL_TYPE;

    static sint8 BD_Update_Cmd;

    static SELECT_SPD_MANUAL_TYPE BD_Select_Method;

    static MOTOR_ENUM_TYPE BD_Motor;

    static sint16 BD_Target_Speed;
    static sint16 BD_Target_Accel;
    static sint16 BD_Target_Stop;
    static MCI_INJECTION_TYPE BD_Manual_Method;
    static sint32 BD_Level_x32;
    static sint32 BD_Level_Rate_x32;
    static sint32 BD_Param_x32;
    static sint32 BD_Param_Rate_x32;

    #ifdef BD_PLT_SPECIFIC
    static uint8 BD_Plt_Specific_Buffer_Size;
    static uint8 BD_Plt_Specific_Buffer[10];
    #endif
    #ifndef BD_CUSTOM_CMD
        // define here macros for standard MCI commands
        #define BD_RUN(motor_index, target_speed, target_accel)                                              Mci__Run(motor_index, target_speed, target_accel)
        #define BD_MANUAL_INJ(motor_index, method, level_x32, level_rate_x32, param_x32, param_rate_x32)     Mci__Manual_Injection(motor_index, method, level_x32, level_rate_x32, param_x32, param_rate_x32)
        #define BD_STOP(motor_index, target_stop)                                                            Mci__Stop(motor_index, target_stop)
        // otherwise macros definition will be in prv file (for instance for Dishwasher application)

    #endif
#endif

#if (BD_RESET == ENABLED)
    #include "Micro.h"
    static uint8   BD_Reset = 255;  //  force reset from Board Debugging
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////// --- ADC NOISE ASSESMENT --/////////////////////////////////////////
#if (BD_ADC_NOISE_ASSESSMENT == ENABLED)
    #include "MotorSafetyMgr.h"
    #include "MathCalc.h"
    #include "Mcl.h"

// structure declaration
    typedef struct{
        sint16 Signal_K_1;
        sint16 Signal_K_2;
        uint16 Noise_Cnt;
        uint16 Noise_Ampl_Max;
    } NOISE_SIGNAL_TYPE;

    static NOISE_SIGNAL_TYPE Current_A;
    static NOISE_SIGNAL_TYPE Current_B;
    static NOISE_SIGNAL_TYPE Current_C;
    static NOISE_SIGNAL_TYPE DcBus;

    static uint16 Current_Noise_Thr;
    static uint16 DcBus_Noise_Thr;
    static uint16 Noise_Measure_Window;
    static uint16 Noise_Measure_Window_Cnt;

#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////// --- PWM SIGNALS DEBUG ---//////////////////////////////////////////


#if (BD_PWM_ACCESS == ENABLED)
    #include "MclBasicTypes.h"
    #include "MotorSafetyMgr.h"
    #include "Mcl.h"
    static uint8 Pwm_Turn_On_Flag;
    static ABC_COOR_SYST_TYPE Pwm_Duty_Hw_Dbg;

    extern MCI_CONTROL_STATE_TYPE Mci_Control_State;           //!< Define the states used in this controller

#endif


#if (BD_ADC_SYNCH_ADJUST == ENABLED)
    static uint8  Adc_Conv_Synch_Adjust;
    static volatile sint16 Adc_Conv_Synch_Value;
#endif

#if (BD_LED == ENABLED)
	#include "Gpio.h"
    static BOOL_TYPE Is_Led_Blinking;
    static uint16 Led_Counter;
#endif


#if (BD_INTERRUPT_ANALYSIS == ENABLED)
    // CPU processing time and overload
    static volatile float32 BD_Fast_Isr_Ovl_Perc;
    static volatile uint16  BD_Fast_Isr_Max_us;
    static volatile uint16  BD_Fast_Isr_Duration_us;
    static volatile float32 BD_Pwm_Isr_Ovl_Perc;
    static volatile float32 BD_Pwm_Isr_Max_us;
    static volatile float32 BD_Pwm_Isr_Duration_us;
#endif
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------




//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

#if (BD_ADC_NOISE_ASSESSMENT == ENABLED)
static void AdcNoiseInit();
static void AdcNoiseHandler();
static void AdcNoiseMeasure(sint16 signal, NOISE_SIGNAL_TYPE* noise_signal, uint16 noise_thr);
#endif



//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module BoardDebugging and its variables
 *
 *  @details    The selected debugging features are initialized
 *
 */
void BoardDebugging__Initialize(void)
{

#if (DEBUG_MASTERCOMMANDER == ENABLED)
    MasterCommander__Initialize();
#endif

#if (BD_ADC_NOISE_ASSESSMENT == ENABLED)
    AdcNoiseInit();
#endif

#if (BD_RESET == ENABLED)
    BD_Reset = 255;
#endif

#if (BD_MCI_CMD == ENABLED)

    BD_Select_Method = SELECT_SPEED;
    BD_Update_Cmd = 0;

    BD_Motor = BD_MOTOR_INIT;

    BD_Target_Speed = BD_SPEED_INIT;
    BD_Target_Accel = BD_ACCEL_INIT;
    BD_Target_Stop  = BD_STOP_INIT;

    BD_Manual_Method  = BD_MANUAL_METHOD_INIT;
    BD_Level_x32      = (sint32)(BD_LEVEL_INIT      * 32.0f);
    BD_Level_Rate_x32 = (sint32)(BD_LEVEL_RATE_INIT * 32.0f);
    BD_Param_x32      = (sint32)(BD_PARAM_INIT      * 32.0f);
    BD_Param_Rate_x32 = (sint32)(BD_PARAM_RATE_INIT * 32.0f);

#endif

#if (BD_PWM_ACCESS == ENABLED)
    Pwm_Duty_Hw_Dbg.A = 16384;
    Pwm_Duty_Hw_Dbg.B = 16384;
    Pwm_Duty_Hw_Dbg.C = 16384;
#endif

#if (BD_LED == ENABLED)
    Is_Led_Blinking = FALSE;
    Led_Counter = LED_INIT_COUNTER;
    Gpio__PinConfig(LED_PORT,LED_PIN,OUTPUT_PUSHPULL);
    GPIO__PIN_SET(LED_PORT,LED_PIN);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Pwm handler for MasterCommander. Rate: every 62.5 us
 *
 */


void BoardDebugging__PwmHandler(void)
{
#if (BD_ADC_NOISE_ASSESSMENT == ENABLED)
    // Adc noise calculation
    // make sure this is executed after offset calculation
    if (MotorSafetyMgr__IsInIdle() || (Mci__GetAnalog(MOTOR0, MCI_AI_INTERNAL_MCI_STATE) >= 3)) // >= MCI_IDLE
    {
        AdcNoiseHandler();
    }
#endif


#if (BD_PWM_ACCESS == ENABLED)
    if(Pwm_Turn_On_Flag && (MotorSafetyMgr__DoesClassAHaveAccessToPwm() == TRUE))
    {
        if (BD_Update_Cmd != 0)
        {
            // Apply the PWM
            MotorSafetyMgr__UpdatePwm(Pwm_Duty_Hw_Dbg.A, Pwm_Duty_Hw_Dbg.B, Pwm_Duty_Hw_Dbg.C);

            BD_Update_Cmd = 0;
        }
    }
#endif


#if (BD_ADC_SYNCH_ADJUST == ENABLED)
    if (Adc_Conv_Synch_Adjust != 0)
    {
        SRMCAtod__SetSyncDelay(Adc_Conv_Synch_Value);
        Adc_Conv_Synch_Adjust  = 0;
    }
#endif

#if (DEBUG_MASTERCOMMANDER == ENABLED)
    MasterCommander__PwmHandler();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Board Debugging handler for 250 us
 *
 */
void BoardDebugging__250usHandler(void)
{
#if (DEBUG_MASTERCOMMANDER == ENABLED)
    MasterCommander__FastHandler();
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Board Debugging handler for 25 ms
 *
 */
#pragma optimize = none
void BoardDebugging__25msHandler(void)
{
#if (BD_RESET == ENABLED)
    if (BD_Reset != 255)
    {
        Micro__ForceReset((MICRO_RESET_MODE_TYPE)BD_Reset);
    }
#endif

#if (BD_PWM_ACCESS == ENABLED)
    if ((BD_Select_Method == SELECT_PWM) && (BD_Update_Cmd != 0))
    {
        if (Pwm_Turn_On_Flag == 0)
        {
            Pwm_Turn_On_Flag = 1;
            MotorSafetyMgr__Prepare2StartMotor();
            // put MCI in debug state
            Mci_Control_State = MCI_DEBUGGING;
            Mcl__SetDigital(MCL_WRITE_INJECTION_DC_VOLTAGE);
        }
    }
    if((BD_Select_Method == SELECT_STOP) && Pwm_Turn_On_Flag == 1)
    {
        Pwm_Turn_On_Flag = 0;
        MotorSafetyMgr__Prepare2StopMotor();
        BD_Select_Method = SELECT_DISABLED;
        Mci_Control_State = MCI_IDLE;
    }
#endif

#if (BD_MCI_CMD == ENABLED)
    if((BD_Select_Method == SELECT_SPEED) && (BD_Update_Cmd != 0))
    {
        if(BD_Target_Speed != 0)
        {
            BD_Update_Cmd = BD_RUN(BD_Motor, BD_Target_Speed, BD_Target_Accel);
        }
        else
        {
            BD_Update_Cmd = BD_STOP(BD_Motor, BD_Target_Accel);
        }
    }
    else if((BD_Select_Method == SELECT_MANUAL) && (BD_Update_Cmd != 0))
    {
        BD_Update_Cmd = BD_MANUAL_INJ(BD_Motor, BD_Manual_Method, BD_Level_x32, BD_Level_Rate_x32, BD_Param_x32, BD_Param_Rate_x32);
    }
    else if(BD_Select_Method == SELECT_STOP)
    {
        BD_STOP(BD_Motor, BD_Target_Stop);
        BD_Select_Method = SELECT_DISABLED;
    }
    else if(BD_Select_Method == SELECT_DISABLED)
    {
        BD_Update_Cmd = 0;
    }
#ifdef BD_PLT_SPECIFIC
    else if ((BD_Select_Method == SELECT_PLTSPECIFIC) && (BD_Update_Cmd != 0))
    {
        BD_Update_Cmd = 0;
        BD_PLT_SPECIFIC(BD_Motor, BD_Plt_Specific_Buffer);
    }
#endif

    if (BD_Update_Cmd > 0)
    {
        BD_Update_Cmd = 0;
    }
#endif


#if (BD_ADC_NOISE_ASSESSMENT == ENABLED)
    Noise_Measure_Window_Cnt++;
    if(Noise_Measure_Window_Cnt == Noise_Measure_Window)
    {
        Noise_Measure_Window_Cnt = 0;
        Current_A.Noise_Cnt = 0;
        Current_B.Noise_Cnt = 0;
        Current_C.Noise_Cnt = 0;
        DcBus.Noise_Cnt     = 0;
    }
#endif

#if (BD_LED == ENABLED)
    if(Is_Led_Blinking == FALSE)
    {
        GPIO__PIN_SET(LED_PORT,LED_PIN);
        Led_Counter--;
        if(!Led_Counter)
        {
            Led_Counter = LED_TIMER;
            Is_Led_Blinking = TRUE;
        }
    }
    else
    {
        Led_Counter--;
        if(!Led_Counter)
        {
            GPIO__PIN_TOGGLE(LED_PORT,LED_PIN);
            Led_Counter = LED_TIMER;
            Is_Led_Blinking = TRUE;
        }
    }
#endif
}

#if (BD_INTERRUPT_ANALYSIS == ENABLED)
/*
 *   BD_Get_Fast_Duration_us(), BD_Fast_Overload(), BD_Get_PWM_Duration_us(), BD_PWM_Overload() are
 *   private functions. Implementation is micro specific, so they are placed in the BoardDebugging_prv.h file
 *   with the pragma inline
 */

void BoardDebugging__FastIsrAnalysis(void)
{
    uint16 temp;
    temp = BD_Get_Fast_Duration_us();
    BD_Fast_Isr_Duration_us = temp;

    if (temp > BD_Fast_Isr_Max_us)
    {   // update the max counter
        BD_Fast_Isr_Max_us = BD_Fast_Isr_Duration_us;
    }
    // CPU overload
    BD_Fast_Overload(BD_Fast_Isr_Ovl_Perc);
}

void BoardDebugging__PwmIsrAnalysis(void)
{
    float32 temp;
    temp = BD_Get_PWM_Duration_us();
    BD_Pwm_Isr_Duration_us = temp;
    if (temp > BD_Pwm_Isr_Max_us)
    {   // update the max counter
        BD_Pwm_Isr_Max_us = BD_Pwm_Isr_Duration_us;
    }

    // CPU overload
    BD_PWM_Overload(BD_Pwm_Isr_Ovl_Perc);
}
#endif



//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (BD_ADC_NOISE_ASSESSMENT == ENABLED)
static void AdcNoiseInit()
{
    //Adc Noise counters initialization
    Current_A.Signal_K_1     = 0;
    Current_B.Signal_K_1     = 0;
    Current_C.Signal_K_1     = 0;
    DcBus.Signal_K_1         = 0;

    Current_A.Signal_K_2     = 0;
    Current_B.Signal_K_2     = 0;
    Current_C.Signal_K_2     = 0;
    DcBus.Signal_K_2         = 0;


    Current_A.Noise_Ampl_Max = 0;
    Current_B.Noise_Ampl_Max = 0;
    Current_C.Noise_Ampl_Max = 0;
    DcBus.Noise_Ampl_Max     = 0;

    Current_A.Noise_Cnt      = 0;
    Current_B.Noise_Cnt      = 0;
    Current_C.Noise_Cnt      = 0;
    DcBus.Noise_Cnt          = 0;

    Current_Noise_Thr = BD_CURRENT_NOISE_THR;
    DcBus_Noise_Thr = BD_DC_BUS_NOISE_THR;

    Noise_Measure_Window = BD_NOISE_WINDOW;
    Noise_Measure_Window_Cnt = 0;

}

static void AdcNoiseHandler()
{
    sint16 current_a;
    sint16 current_b;
    sint16 current_c;
    sint16 dc_bus;

    // convert floating point mcl currents and DCbus voltage back to 1.15 notation
    // (to be used, for instance, in case of third current reconstruction)
    current_a = (sint16) BD_CURRENT(Mcl_IO.Is_ABC.A);
    current_b = (sint16) BD_CURRENT(Mcl_IO.Is_ABC.B);
    current_c = (sint16) BD_CURRENT(Mcl_IO.Is_ABC.C);
    dc_bus    = (sint16) BD_VOLTAGE(Mcl_IO.Vdc);


    //--------------- Motor Current Adc Noise measurement -----------------------
    AdcNoiseMeasure(current_a, &Current_A, Current_Noise_Thr);
    AdcNoiseMeasure(current_b, &Current_B, Current_Noise_Thr);
    AdcNoiseMeasure(current_c, &Current_C, Current_Noise_Thr);

    //--------------- Dc Bus  Noise measurement -------------------------
    AdcNoiseMeasure(dc_bus, &DcBus, DcBus_Noise_Thr);

}


static void AdcNoiseMeasure(sint16 signal, NOISE_SIGNAL_TYPE* noise_signal, uint16 noise_thr)
{
    sint16 signal_mean;
    sint16 signal_delta;


    signal_mean = (signal>>1) + (noise_signal->Signal_K_2 >>1);

    signal_delta = MATHCALC__ABS(noise_signal->Signal_K_1 - signal_mean);

    if(signal_delta > noise_thr)
    {
        if(noise_signal->Noise_Ampl_Max < signal_delta)
        {
            noise_signal->Noise_Ampl_Max = signal_delta;
        }

        noise_signal->Noise_Cnt++;
    }

    noise_signal->Signal_K_2 = noise_signal->Signal_K_1;
    noise_signal->Signal_K_1 = signal;

}


#endif
