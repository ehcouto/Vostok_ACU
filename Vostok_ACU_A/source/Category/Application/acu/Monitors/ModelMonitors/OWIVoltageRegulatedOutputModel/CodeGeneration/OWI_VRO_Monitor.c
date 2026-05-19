/*
 * File: OWI_VRO_Monitor.c
 *
 * Code generated for Simulink model 'OWI_VRO_Monitor'.
 *
 * Model version                  : 1.125
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Tue Aug  2 17:21:20 2022
 *
 * Target selection: monitor.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "OWI_VRO_Monitor.h"
#include "OWI_VRO_Monitor_private.h"

/* Named constants for Chart: '<S2>/Binary_Search_Logic' */
#define OWI_VRO_Moni_IN_NO_ACTIVE_CHILD ((uint8)0U)
#define OWI_VRO_Monit_IN_INITIALIZATION ((uint8)3U)
#define OWI_VRO_Monito_IN_BINARY_SEARCH ((uint8)1U)
#define OWI_VRO_Monitor_IN_COMPLETE    ((uint8)2U)
#define OWI_VRO_Monitor_IN_RUN         ((uint8)1U)
#define OWI_VRO_Monitor_IN_SATURATED   ((uint8)4U)
#define OWI_VRO_Monitor_IN_STOPPED     ((uint8)2U)
#define OWI_VRO_Monitor_IN_WAIT        ((uint8)5U)
#define OWI_VRO_Monitor_IN_WAIT_DELAY  ((uint8)1U)
#define OWI_VRO_Monitor_IN_WAIT_START  ((uint8)2U)

/* Block signals (default storage) */
BOWI_VRO_MONITORC_TYPE OWI_VRO_Monitor_b;

/* Block states (default storage) */
DWOWI_VRO_MONITORF_TYPE OWI_VRO_Monitor_dw;

/* Forward declaration for local functions */
static uint16 OWI_VRO_Monitor_Linearization(uint16 dutycycle, uint8
  sf_owivrolinearizationnumitems, const uint16
  sf_owivrolinearizationtableduty[MAX_LINEARIZATION_TABLE], const uint16
  sf_owivrolinearizationtablemult[MAX_LINEARIZATION_TABLE]);
static void OWI_VRO_Monitor_RUN(const float32 *avg_coeff, const float32
  *voltage_reading, const float32 *voltage_setpoint, const float32 *exponent,
  const float32 *voltage_tolerance, const float32 *voltage_error, BOOL_TYPE
  owi_vro_stop, BOOL_TYPE owi_vro_reset, uint8 sf_owivrolinearizationnumitems,
  const uint16 sf_owivrolinearizationtableduty[MAX_LINEARIZATION_TABLE], const
  uint16 sf_owivrolinearizationtablemult[MAX_LINEARIZATION_TABLE], uint16
  *search_dutycycle, uint16 *owi_vro_dutycycle_stable, uint16
  *owi_vro_dutycycle_comp, uint8 *owi_vro_dutycycle_sequence_id,
  OWI_VRO_STATEMACHINE_TYPE *owi_vro_statemachine, float32
  *owi_vro_voltage_stable);

/* Function for Chart: '<S2>/Binary_Search_Logic' */
static uint16 OWI_VRO_Monitor_Linearization(uint16 dutycycle, uint8
  sf_owivrolinearizationnumitems, const uint16
  sf_owivrolinearizationtableduty[MAX_LINEARIZATION_TABLE], const uint16
  sf_owivrolinearizationtablemult[MAX_LINEARIZATION_TABLE])
{
  sint32 mdiff;
  float32 malgdiff;
  uint16 currdefdiff;
  uint8 row;
  BOOL_TYPE guard1 = false;
  row = 1U;
  while ((row < sf_owivrolinearizationnumitems) &&
         (sf_owivrolinearizationtableduty[(sint32)(((sint32)row) - 1)] <
          dutycycle)) {
    row = (uint8)((uint32)(((uint32)row) + 1U));
  }

  guard1 = false;
  if (((sint32)row) == 1) {
    if (((sint32)sf_owivrolinearizationnumitems) == 1) {
      /*  Don't adjust multiplier difference */
      malgdiff = 0.0F;
    } else {
      /*  Calculate the difference between next two values */
      currdefdiff = (uint16)((uint32)(((uint32)sf_owivrolinearizationtableduty[1])
        - ((uint32)sf_owivrolinearizationtableduty[0])));
      guard1 = true;
    }
  } else {
    /*  Calculate the difference between neighbouring values */
    currdefdiff = (uint16)((uint32)(((uint32)sf_owivrolinearizationtableduty
      [(sint32)(((sint32)row) - 1)]) - ((uint32)sf_owivrolinearizationtableduty
      [(sint32)(((sint32)((uint8)((uint32)(((uint32)row) - 1U)))) - 1)])));
    guard1 = true;
  }

  if (guard1) {
    /*  Calculate the difference between table row and algorithm input */
    if (((sint32)row) == 1) {
      /*  Calculate the difference between multipliers in next two rows */
      mdiff = (sint32)fabsf((float32)((sint32)(((sint32)
        sf_owivrolinearizationtablemult[0]) - ((sint32)
        sf_owivrolinearizationtablemult[1]))));
    } else {
      /*  Calculate the difference between multipliers in neighbouring rows */
      mdiff = (sint32)(((sint32)sf_owivrolinearizationtablemult[(sint32)
                        (((sint32)row) - 1)]) - ((sint32)
        sf_owivrolinearizationtablemult[(sint32)(((sint32)((uint8)((uint32)
        (((uint32)row) - 1U)))) - 1)]));
    }

    if (((sint32)currdefdiff) == 0) {
      /*  Don't adjust multiplier difference */
      malgdiff = 0.0F;
    } else {
      /*  Multiplier difference calculated for referenced Duty Cycle and neighbouring row's parameter */
      malgdiff = (((float32)((sint32)(((sint32)((uint16)((uint32)(((uint32)
        sf_owivrolinearizationtableduty[(sint32)(((sint32)row) - 1)]) - ((uint32)
        dutycycle))))) * mdiff))) / ((float32)VRO_VOLT_BASE_MULT)) / ((float32)
        currdefdiff);
    }
  }

  mdiff = (sint32)(((sint32)row) - 1);
  if (sf_owivrolinearizationtableduty[mdiff] < dutycycle) {
    /*  Calculate actual multiplier for value above the highest threshold */
    malgdiff += ((float32)sf_owivrolinearizationtablemult[mdiff]) / ((float32)
      VRO_VOLT_BASE_MULT);
  } else {
    /*  Calculate actual multiplier */
    malgdiff = (((float32)sf_owivrolinearizationtablemult[mdiff]) / ((float32)
      VRO_VOLT_BASE_MULT)) - malgdiff;
  }

  malgdiff *= (float32)dutycycle;
  if (malgdiff > ((float32)VRO_MAX_DC_PWM)) {
    malgdiff = (float32)VRO_MAX_DC_PWM;
  } else {
    /*  Result below max DutyCycle - safe to assign; */
  }

  return (uint16)roundf(malgdiff);
}

/* Function for Chart: '<S2>/Binary_Search_Logic' */
static void OWI_VRO_Monitor_RUN(const float32 *avg_coeff, const float32
  *voltage_reading, const float32 *voltage_setpoint, const float32 *exponent,
  const float32 *voltage_tolerance, const float32 *voltage_error, BOOL_TYPE
  owi_vro_stop, BOOL_TYPE owi_vro_reset, uint8 sf_owivrolinearizationnumitems,
  const uint16 sf_owivrolinearizationtableduty[MAX_LINEARIZATION_TABLE], const
  uint16 sf_owivrolinearizationtablemult[MAX_LINEARIZATION_TABLE], uint16
  *search_dutycycle, uint16 *owi_vro_dutycycle_stable, uint16
  *owi_vro_dutycycle_comp, uint8 *owi_vro_dutycycle_sequence_id,
  OWI_VRO_STATEMACHINE_TYPE *owi_vro_statemachine, float32
  *owi_vro_voltage_stable)
{
  BOOL_TYPE guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/OWI_VRO_Turbidity_Evaluation' */
  /* Chart: '<S2>/Binary_Search_Logic' */
  if (owi_vro_stop || owi_vro_reset) {
    OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Moni_IN_NO_ACTIVE_CHILD;
    OWI_VRO_Monitor_dw.is_run = OWI_VRO_Moni_IN_NO_ACTIVE_CHILD;
    OWI_VRO_Monitor_dw.is_c1_owi_vro_monitor = OWI_VRO_Monitor_IN_STOPPED;
    *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_STOPPED;
    *search_dutycycle = 0U;
  } else {
    guard1 = false;
    switch (OWI_VRO_Monitor_dw.is_run) {
     case OWI_VRO_Monito_IN_BINARY_SEARCH:
      if (((sint32)((uint16)((uint32)(((uint32)OWI_VRO_Monitor_dw.upper) -
              ((uint32)OWI_VRO_Monitor_dw.lower))))) <= 1) {
        if ((*voltage_error) > (*voltage_tolerance)) {
          if (((sint32)OWI_VRO_Monitor_dw.upper) >= ((sint32)roundf(0.95F *
                ((float32)OWI_VRO_Monitor_b.max_dutycycle)))) {
            OWI_VRO_Monitor_dw.saturation = true;
            guard1 = true;
          } else {
            /*  Not saturated, restart search to find a value in tolerance range; */
            OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monit_IN_INITIALIZATION;
            *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_INITIALIZATION;
            OWI_VRO_Monitor_dw.upper = OWI_VRO_Monitor_b.max_dutycycle;
            OWI_VRO_Monitor_dw.lower = 0U;
            *search_dutycycle = (uint16)roundf(((float32)
              OWI_VRO_Monitor_b.max_dutycycle) / 2.0F);
            OWI_VRO_Monitor_dw.besterror = 5.0F;
            OWI_VRO_Monitor_dw.bestdutycycle = *search_dutycycle;
            OWI_VRO_Monitor_dw.saturation = false;
            OWI_VRO_Monitor_dw.startover = false;
          }
        } else {
          /*  Voltage error in tolerance; */
          guard1 = true;
        }
      } else {
        OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monitor_IN_WAIT;
        OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Monitor_IN_WAIT_START;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_WAIT_START;
        OWI_VRO_Monitor_dw.time_count = 0U;
      }
      break;

     case OWI_VRO_Monitor_IN_COMPLETE:
      if (OWI_VRO_Monitor_dw.saturation) {
        OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monitor_IN_SATURATED;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_SATURATED;
      } else {
        /*  Start over after delay */
        OWI_VRO_Monitor_dw.startover = true;
        OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monitor_IN_WAIT;
        OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Monitor_IN_WAIT_START;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_WAIT_START;
        OWI_VRO_Monitor_dw.time_count = 0U;
      }
      break;

     case OWI_VRO_Monit_IN_INITIALIZATION:
      OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monitor_IN_WAIT;
      OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Monitor_IN_WAIT_START;
      *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_WAIT_START;
      OWI_VRO_Monitor_dw.time_count = 0U;
      break;

     case OWI_VRO_Monitor_IN_SATURATED:
      /*  Start over after delay */
      OWI_VRO_Monitor_dw.startover = true;
      OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monitor_IN_WAIT;
      OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Monitor_IN_WAIT_START;
      *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_WAIT_START;
      OWI_VRO_Monitor_dw.time_count = 0U;
      break;

     default:
      /* case IN_WAIT: */
      if ((OWI_VRO_Monitor_dw.startover) && (OWI_VRO_Monitor_dw.time_count >=
           VRO_DELAY_TIME)) {
        OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Moni_IN_NO_ACTIVE_CHILD;
        OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monit_IN_INITIALIZATION;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_INITIALIZATION;
        OWI_VRO_Monitor_dw.upper = OWI_VRO_Monitor_b.max_dutycycle;
        OWI_VRO_Monitor_dw.lower = 0U;
        *search_dutycycle = (uint16)roundf(((float32)
          OWI_VRO_Monitor_b.max_dutycycle) / 2.0F);
        OWI_VRO_Monitor_dw.besterror = 5.0F;
        OWI_VRO_Monitor_dw.bestdutycycle = *search_dutycycle;
        OWI_VRO_Monitor_dw.saturation = false;
        OWI_VRO_Monitor_dw.startover = false;
      } else if ((!OWI_VRO_Monitor_dw.startover) &&
                 (OWI_VRO_Monitor_dw.time_count >=
                  OWI_VRO_Monitor_b.read_time_ms)) {
        OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Moni_IN_NO_ACTIVE_CHILD;
        OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monito_IN_BINARY_SEARCH;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_BINARY_SEARCH;
        if ((*voltage_error) <= OWI_VRO_Monitor_dw.besterror) {
          OWI_VRO_Monitor_dw.bestdutycycle = *search_dutycycle;
          OWI_VRO_Monitor_dw.besterror = *voltage_error;
        } else {
          /*  Not the best error yet; */
        }

        if ((*voltage_reading) > (*voltage_setpoint)) {
          OWI_VRO_Monitor_dw.lower = *search_dutycycle;
        } else {
          /*  OWI_VRO_Voltage_Reading <= Voltage_Set_Point */
          OWI_VRO_Monitor_dw.upper = *search_dutycycle;
        }

        *search_dutycycle = (uint16)roundf(((float32)((uint16)((uint32)(((uint32)
          OWI_VRO_Monitor_dw.upper) + ((uint32)OWI_VRO_Monitor_dw.lower))))) /
          2.0F);
      } else if (((sint32)OWI_VRO_Monitor_dw.is_wait) == 1) {
        OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Monitor_IN_WAIT_DELAY;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_WAIT_DELAY;
        OWI_VRO_Monitor_dw.time_count = (uint16)((uint32)(((uint32)
          OWI_VRO_Monitor_dw.time_count) + ((uint32)VRO_EXECUTION_TIME)));
      } else {
        /* case IN_WAIT_START: */
        OWI_VRO_Monitor_dw.is_wait = OWI_VRO_Monitor_IN_WAIT_DELAY;
        *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_WAIT_DELAY;
        OWI_VRO_Monitor_dw.time_count = (uint16)((uint32)(((uint32)
          OWI_VRO_Monitor_dw.time_count) + ((uint32)VRO_EXECUTION_TIME)));
      }
      break;
    }

    if (guard1) {
      OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monitor_IN_COMPLETE;
      *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_COMPLETE;
      *search_dutycycle = OWI_VRO_Monitor_dw.bestdutycycle;
      *owi_vro_dutycycle_stable = OWI_VRO_Monitor_Linearization
        (OWI_VRO_Monitor_dw.bestdutycycle, sf_owivrolinearizationnumitems,
         sf_owivrolinearizationtableduty, sf_owivrolinearizationtablemult);
      *owi_vro_voltage_stable = (float32)(*voltage_reading);
      if ((*avg_coeff) > 0.0F) {
        /*  Compensate the result if Average Coefficient has been calculated */
        *owi_vro_dutycycle_comp = (uint16)roundf(powf(*avg_coeff, *exponent) *
          ((float32)(*owi_vro_dutycycle_stable)));
      } else {
        /*  Average Coefficient not available; */
      }

      if (((sint32)(*owi_vro_dutycycle_sequence_id)) < 255) {
        /*  Increase the sequence id in order to make it easier to recognize a new sample */
        *owi_vro_dutycycle_sequence_id = (uint8)((uint32)(((uint32)
          (*owi_vro_dutycycle_sequence_id)) + 1U));
      } else {
        /*  Start incrementation over */
        *owi_vro_dutycycle_sequence_id = 1U;
      }
    }
  }

  /* End of Chart: '<S2>/Binary_Search_Logic' */
  /* End of Outputs for SubSystem: '<Root>/OWI_VRO_Turbidity_Evaluation' */
}

/* Output and update for referenced model: 'OWI_VRO_Monitor' */
void OWI_VRO_Monitor__Execute(BOOL_TYPE owi_vro_stop, BOOL_TYPE owi_vro_reset,
  HBL_PILOT_ENUM_TYPE owi_pilot_type, uint16 owi_vro_avg_coeff, uint32
  owi_vro_voltage_reading, uint8 owi_vro_setpoint_index, uint8
  sf_owivrosetpoint1data_voltage_, uint16 sf_owivrosetpoint1data_compensa, uint8
  sf_owivrosetpoint2data_voltage_, uint16 sf_owivrosetpoint2data_compensa, uint8
  sf_owivrosetpoint3data_voltage_, uint16 sf_owivrosetpoint3data_compensa, const
  OWI_VRO_COMMON_SF_PARAMS_TYPE *sf_owivoltageregulatedoutput, uint8
  sf_owivrolinearizationnumitems, const uint16
  sf_owivrolinearizationtableduty[MAX_LINEARIZATION_TABLE], const uint16
  sf_owivrolinearizationtablemult[MAX_LINEARIZATION_TABLE], uint16
  *search_dutycycle, uint16 *owi_vro_dutycycle_stable, uint16
  *owi_vro_dutycycle_comp, uint8 *owi_vro_dutycycle_sequence_id,
  OWI_VRO_STATEMACHINE_TYPE *owi_vro_statemachine, float32
  *owi_vro_voltage_stable)
{
  uint16 Constant2;
  uint8 Voltage_Setpoint;
  float32 avg_coeff;
  float32 voltage_reading;
  float32 voltage_setpoint;
  float32 exponent;
  float32 voltage_tolerance;
  float32 voltage_error;

  /* Outputs for Atomic SubSystem: '<Root>/OWI_VRO_Turbidity_Evaluation' */
  /* Switch: '<S6>/Switch' incorporates:
   *  Constant: '<S6>/Constant'
   *  Constant: '<S6>/Constant1'
   *  Constant: '<S8>/Constant'
   *  RelationalOperator: '<S8>/Compare'
   */
  if (owi_pilot_type == HBL_PILOT_PWM_HI_RES) {
    OWI_VRO_Monitor_b.max_dutycycle = VRO_MAX_DC_HI_RES;
  } else {
    OWI_VRO_Monitor_b.max_dutycycle = VRO_MAX_DC_PWM;
  }

  /* End of Switch: '<S6>/Switch' */

  /* Product: '<S5>/Divide6' incorporates:
   *  Constant: '<S5>/Constant6'
   *  DataTypeConversion: '<S5>/Data Type Conversion6'
   */
  avg_coeff = (float32)(((float32)((float32)owi_vro_avg_coeff)) / ((float32)
    VRO_AVG_COEFF_BASE_MULT));

  /* Product: '<S5>/Divide' incorporates:
   *  Constant: '<S5>/Constant1'
   *  DataTypeConversion: '<S5>/Data Type Conversion'
   */
  voltage_reading = (float32)(((float32)((float32)owi_vro_voltage_reading)) /
    ((float32)VRO_VOLT_BASE_MULT));

  /* MultiPortSwitch generated from: '<S7>/Multiport Switch' */
  switch ((sint32)owi_vro_setpoint_index) {
   case 2:
    Voltage_Setpoint = sf_owivrosetpoint3data_voltage_;
    break;

   case 1:
    Voltage_Setpoint = sf_owivrosetpoint2data_voltage_;
    break;

   default:
    Voltage_Setpoint = sf_owivrosetpoint1data_voltage_;
    break;
  }

  /* Product: '<S5>/Divide3' incorporates:
   *  Constant: '<S5>/Constant3'
   *  DataTypeConversion: '<S5>/Data Type Conversion3'
   */
  voltage_setpoint = (float32)(((float32)((float32)Voltage_Setpoint)) /
    ((float32)VRO_VOLT_SP_BASE_MULT));

  /* Abs: '<S5>/Abs' incorporates:
   *  Sum: '<S5>/Subtract'
   */
  voltage_error = fabsf(voltage_reading - voltage_setpoint);

  /* MultiPortSwitch generated from: '<S7>/Multiport Switch' */
  switch ((sint32)owi_vro_setpoint_index) {
   case 2:
    Constant2 = sf_owivrosetpoint3data_compensa;
    break;

   case 1:
    Constant2 = sf_owivrosetpoint2data_compensa;
    break;

   default:
    Constant2 = sf_owivrosetpoint1data_compensa;
    break;
  }

  /* Product: '<S5>/Divide4' incorporates:
   *  Constant: '<S5>/Constant4'
   *  DataTypeConversion: '<S5>/Data Type Conversion4'
   */
  exponent = (float32)(((float32)((float32)Constant2)) / ((float32)
    VRO_COMP_EXP_BASE_MULT));

  /* Product: '<S5>/Divide1' incorporates:
   *  Constant: '<S5>/Constant'
   *  DataTypeConversion: '<S5>/Data Type Conversion1'
   */
  voltage_tolerance = (float32)(((float32)((float32)
    sf_owivoltageregulatedoutput->Voltage_Tolerance)) / ((float32)
    VRO_VOLT_TOL_BASE_MULT));

  /* DataTypeConversion: '<S5>/Data Type Conversion5' incorporates:
   *  Constant: '<S5>/Constant2'
   *  Product: '<S5>/Divide2'
   */
  OWI_VRO_Monitor_b.read_time_ms = (uint16)((uint32)(((uint32)
    sf_owivoltageregulatedoutput->Read_Time_ms) * ((uint32)
    VRO_READ_TIME_BASE_MULT)));

  /* Chart: '<S2>/Binary_Search_Logic' */
  if (((uint32)OWI_VRO_Monitor_dw.is_active_c1_owi_vro_monitor) == 0U) {
    OWI_VRO_Monitor_dw.is_active_c1_owi_vro_monitor = 1U;
    *owi_vro_dutycycle_sequence_id = 0U;
    OWI_VRO_Monitor_dw.is_c1_owi_vro_monitor = OWI_VRO_Monitor_IN_STOPPED;
    *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_STOPPED;
    *search_dutycycle = 0U;
  } else if (((uint32)OWI_VRO_Monitor_dw.is_c1_owi_vro_monitor) ==
             OWI_VRO_Monitor_IN_RUN) {
    OWI_VRO_Monitor_RUN(&avg_coeff, &voltage_reading, &voltage_setpoint,
                        &exponent, &voltage_tolerance, &voltage_error,
                        owi_vro_stop, owi_vro_reset,
                        sf_owivrolinearizationnumitems,
                        sf_owivrolinearizationtableduty,
                        sf_owivrolinearizationtablemult, search_dutycycle,
                        owi_vro_dutycycle_stable, owi_vro_dutycycle_comp,
                        owi_vro_dutycycle_sequence_id, owi_vro_statemachine,
                        owi_vro_voltage_stable);
  } else {
    /* case IN_STOPPED: */
    if ((!owi_vro_stop) && owi_vro_reset) {
      OWI_VRO_Reset = false;
      OWI_VRO_Monitor_dw.is_c1_owi_vro_monitor = OWI_VRO_Monitor_IN_RUN;
      OWI_VRO_Monitor_dw.is_run = OWI_VRO_Monit_IN_INITIALIZATION;
      *owi_vro_statemachine = OWI_VRO_STATEMACHINE_TYPE_INITIALIZATION;
      OWI_VRO_Monitor_dw.upper = OWI_VRO_Monitor_b.max_dutycycle;
      OWI_VRO_Monitor_dw.lower = 0U;
      *search_dutycycle = (uint16)roundf(((float32)
        OWI_VRO_Monitor_b.max_dutycycle) / 2.0F);
      OWI_VRO_Monitor_dw.besterror = 5.0F;
      OWI_VRO_Monitor_dw.bestdutycycle = *search_dutycycle;
      OWI_VRO_Monitor_dw.saturation = false;
      OWI_VRO_Monitor_dw.startover = false;
    }
  }

  /* End of Chart: '<S2>/Binary_Search_Logic' */
  /* End of Outputs for SubSystem: '<Root>/OWI_VRO_Turbidity_Evaluation' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
