/**
 *  @file
 *
 *  @brief      Private parameters for the Variables module.
 *
 *  @details    Refer to the C source file for more detailed information.
 *
 *      This file specifies the variables that should be accessible to the following tools:
 *          - JumpIf statements in the cycle definition use variables to evaluate their logical conditions.
 *          - Modifiers in the cycle definition use variable values as inputs.
 *          - GINA uses variables (through API010) for data acquisition.
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VARIABLE_PRV_H_
#define VARIABLE_PRV_H_

#include "SystemConfig_prm.h"
#if (SYSTEM_ACU_HANDLING == ENABLED)
#include "CartridgeDetectionMonitor.h"
#include "DishAuxVar.h"
#include "DoorMonitor.h"
#include "FaultStatusMonitor.h"
#include "Fill.h"
#include "HeaterCalcificationDetectionMonitor.h"
#include "HeaterShadowing.h"
#include "OWIVoltageRegulatedOutputModel.h"
#include "OwiVroRead.h"
#include "Regenerate.h"
#include "RinseAidMonitor.h"
#include "SaltSensingMonitor.h"
#include "StabilityControl.h"
#include "TriacVoltageMonitor.h"
#include "VoltageRangeMonitor.h"
#include "Vwi.h"
#endif
#if (SYSTEM_HMI_HANDLING == ENABLED)
#include "AcuVariableExtern.h"
#endif

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

/*
 * Extern the "protected" variables here.
 * List them in alphabetical order so that they are easier to find and to avoid duplicates.
 */

extern uint16 Estimated_Remaining_Time[APPLIANCE_MAX_COMPARTMENTS];
extern uint32 Supervisor_Delay_Time_Remaining[APPLIANCE_MAX_COMPARTMENTS];
extern uint8 SensedDrain_DrainConfirmed;
extern HEATER_SHADOWING_STATUS_TYPE HeaterShadowing_Status;



/**
 * Here you add the address of each variable on this #define.
 * This is the place you add the Variables that aren't Cross Category.
 * Cross-Category and Application Variables declared here will be on the Data_Holder[] array.
 *
 * As you can use the VARIABLE_ID_TYPE to define IDs for the variables you're making accessible through this module.
 * Number of elements here plus the number of Cross-Category Variables must be the same as there so the references will make sense.
 *
 * typedef PACKED struct VARIABLE_STRUCT
 * {
 *     void * Address;      //! Address for the Variable on the memory. You need to cast to the proper type to use.
 *     uint8 Length;        //! Length if the Variable is actually an ARRAY you use this field and TYPE is the TYPE of the variables on the ARRAY. 1 means 1 element
 *     DATATYPE_TYPE Type;  //! Type of the variable DATA_DEFINITION_TYPE.
 *     BOOL_TYPE	 Is_Writable;	//! Indicates we can write to the variable.
 * } VARIABLE_TYPE;
 *
 * The values in this table must match the values in the appropriate tab of the "_Adding a Variable: Variable list" Google Sheet:
 *		https://docs.google.com/a/whirlpool.com/spreadsheets/d/1GVgeE_Q-qhtORBQHp_ASRtQg-N8MoHo8T5vsdFkEvQg/edit?usp=sharing
 *
 *	If you don't have variables to add, comment this out.
 */
#define ACU_CATEGORY_VARIABLE_LIST       /*VARIABLE_USERS_LIST_OF_NON_CROSS_CATEGORY \*/\
    /*  Address                                   Length                         Type                         RW  */ \
      { &Wash_Probe_Value							,1							,DATATYPE_IS_Q15_CELSIUS    ,FALSE  } /* VARIABLE_WATER_TEMPERATURE        	       			*/ \
    , { &Diverter_Position							,1							,DATATYPE_UINT8    	        ,FALSE  } /* VARIABLE_DIVERTER_POSITION        	       			*/ \
    , { &Sanitization_Result						,1							,DATATYPE_UINT8    	        ,FALSE  } /* VARIABLE_SANITIZATION_RESULT      	       			*/ \
	, { &Pulse_Counter_Ref							,1 							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_PULSE_COUNTER_REF        	       			*/ \
 	, { &Water_Counter								,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_WATER_COUNTER            	       			*/ \
    , { &Door_Status								,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_DOOR_STATUS              	       			*/ \
    , { &Water_Present								,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_WATER_PRESENT            	       			*/ \
	, { &Liters_In_Tub								,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_LITERS_IN_TUB            	       			*/ \
	, { &Liters_Current_Fill						,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LITERS_CURRENT_FILL      	       			*/ \
	, { &H2O_Alarm									,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_H2O_ALARM                	       			*/ \
	, { &Previous_HHP_Door_Check					,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_PREVIOUS_HHP_DOOR_CHECK          			*/ \
    , { &OWI_Analog									,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_OWI_ANALOG               	       			*/ \
    , { &Positional_Loads_Status_Bitmap				,1							,DATATYPE_UINT32            ,FALSE  } /* VARIABLE_POSITIONAL_LOADS_BITMAP          			*/ \
    , { &Rinse_Aid_Status							,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_RINSE_AID                	       			*/ \
	, { &Soil_Measure_Recent						,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_RECENT      	       			*/ \
	, { &Soil_Measure_2								,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_1                   			*/ \
	, { &Soil_Measure_3								,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_2           	       			*/ \
	, { &Soil_Measure_4								,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_3         	       			*/ \
	, { &Soil_Measure_5								,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_4        	       			*/ \
	, { &Soil_Measure_6								,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_5           	       			*/ \
	, { &Soil_Measure_Difference_Recent				,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_DIFFERNCE_RECENT    			*/ \
	, { &Soil_Measure_Difference_2					,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_DIFFERNCE_2         			*/ \
	, { &Soil_Measure_Difference_3					,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_SOIL_MEASURE_DIFFERNCE_3         			*/ \
	, { &Estimated_Remaining_Time[0]				,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_ESTIMATED_REMAINING_TIME         			*/ \
	, { &Supervisor_Delay_Time_Remaining[0]			,APPLIANCE_MAX_COMPARTMENTS	,DATATYPE_UINT32            ,FALSE  } /* VARIABLE_SUPERVISOR_DELAY_TIME_REMAINING  			*/ \
    , { &OWI_Calibration_Drive_Duty_Cycle			,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_OWI_CALIBRATION_DRIVE_DUTY_CYCLE 			*/ \
	, { &OWI_Status									,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_OWI_CALIBRATION_STATUS		       		*/ \
	, { &SensedDrain_DrainConfirmed					,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_SENSED_DRAIN_CONFIRMED		   			*/ \
	, { &Sensed_Drain_Status						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_SENSED_DRAIN_STATUS			   			*/ \
	, { &Soil_Level_1								,1							,DATATYPE_UINT8             ,TRUE   } /* SOIL_LEVEL_1							   			*/ \
	, { &Soil_Level_2								,1							,DATATYPE_UINT8             ,TRUE   } /* SOIL_LEVEL_2							   			*/ \
	, { &Life_Test_Mode_Shortcut_Status				,1							,DATATYPE_UINT8             ,FALSE  } /* LIFE_TEST_MODE_SHORTCUT_STATUS		       			*/ \
	, { &Fill_Fault									,1							,DATATYPE_UINT8             ,FALSE  } /* FILL_FAULT							       			*/ \
	, { &OWICalibration_State						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_OWICALIBRATION_STATE			   			*/ \
	, { &OWICalibration_Status						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_OWICALIBRATION_STATUS		       			*/ \
	, { &OWICalibration_Turbidity_Voltage_Reading	,1							,DATATYPE_FLOAT32           ,FALSE  } /* VARIABLE_OWICALIBRATION_TURBIDITY_VOLTAGE_READING	*/ \
	, { &Phase_Repetitions							,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_PHASE_REPETITIONS			       			*/ \
	, { &RinseAid_Dispenser_Use_Cnt					,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_RINSEAID_DISPENSER_USE_COUNTER   			*/ \
	, { &Generic_Bitmap								,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_GENERIC_BITMAP							*/ \
	, { &Fault_Status_Bitmap_0_15					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_FAULT_STATUS_BITMAP_0_15		   			*/ \
    , { &Fault_Status_Bitmap_16_31					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_FAULT_STATUS_BITMAP_16_31	       			*/ \
    , { &Loads_Bitmap_Status_16_31					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LOADS_BITMAP_16_31               			*/ \
	, { &Loads_Bitmap_Status_0_15					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LOADS_BITMAP_0_15                			*/ \
	, { &Do_Factory_Cycle							,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_DO_FACTORY_CYCLE                 			*/ \
	, { &Fault_Status_Bitmap_32_47					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_FAULT_STATUS_BITMAP_32_47	       			*/ \
	, { &HeaterShadowing_Status						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_HEATER_SHADOWING_STATUS          			*/ \
	, { &Salt_Alarm_Led								,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_SALT_ALARM_LED			       			*/ \
	, { &RegenerateAndSaltBitmap					,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_REG_AND_SALT_BITMAP		       			*/ \
	, { &Vwi_Status									,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_VWI_STATUS			       	   			*/ \
	, { &Model_Options[0]							,MODEL_OPTIONS_NUM_VAR		,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_MODEL_OPTIONS			       	   			*/ \
	, { &Fault_Status_Bitmap_48_63					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_FAULT_STATUS_BITMAP_48_63	       			*/ \
	, { &Dish_General_Purpose_Variables[0]			,DISH_GEN_PURP_NUM_VAR		,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_DISH_GENERAL_PURPOSE             			*/ \
	, { &SC_Subcycle_Execution_Counters[0]			,SC_SUBC_EXEC_COUNTERS_NUM	,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_SC_SUBCYCLE_EXECUTION_COUNTERS			*/ \
	, { &Partial_Drain_Status						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_PARTIAL_DRAIN_STATUS			   			*/ \
	, { &Fault_Status_Bitmap_64_79					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_FAULT_STATUS_BITMAP_64_79	       			*/ \
	, { &HCD_Heater_Clicks							,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_HCD_HEATER_CLICKS  		   				*/ \
	, { &HCD_Calcification_Level					,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_HCD_CALCIFICATION_LEVEL		   			*/ \
	, { &HCD_Bitmap.All								,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_HCD_BITMAP					   			*/ \
	, { &HCD_HMI_Data.Flags.All						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_HCD_HMI_DATA_FLAGS						*/ \
	, { &HCD_HMI_Data.Cycles_Remaining				,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_HCD_HMI_DATA_CYCLES_REMAINING				*/ \
	, { &Liters_In_Water_Tank						,1							,DATATYPE_UINT16            ,TRUE   } /* VARIABLE_LITERS_IN_WATER_TANK     	       			*/ \
	, { &Water_Tank_State							,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_WATER_TANK_STATE			       	   		*/ \
	, { &Voltage_Range_Status						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_VOLTAGE_RANGE_STATUS		       	   		*/ \
	, { &OWI_VRO_DutyCycle_Stable					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_OWI_VRO_DUTYCYCLE_STABLE					*/ \
	, { &OWI_VRO_DutyCycle_Comp						,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_OWI_VRO_DUTYCYCLE_COMP					*/ \
	, { &OWI_VRO_OWI_Status							,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_OWI_VRO_CALIBRATION_STATUS		   		*/ \
	, { &Stability_Control_Flags					,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_STABILITY_CONTROL_FLAGS		   		    */ \
	, { &Class_A_Fill_Bitmap						,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_CLASS_A_FILL_BITMAP		       			*/ \
	, { &Tank_State_Drain							,1							,DATATYPE_UINT8             ,TRUE   } /* VARIABLE_TANK_STATE_DRAIN			       	   		*/ \
	, { &Tank_State_Fill							,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_TANK_STATE_FILL			       	   		*/ \
	, { &Liters_In_Tub_Before_Fill					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LITERS_IN_TUB_BEFORE_FILL        			*/ \
	, { &Liters_In_Tank_Before_Fill					,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LITERS_IN_TANK_BEFORE_FILL       			*/ \
	, { &Liters_Unchecked							,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LITERS_UNCHECKED			       			*/ \
	, { &Liters_For_Tank_Full						,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LITERS_FOR_TANK_FULL		       			*/ \
	, { &Liters_For_Tank_Empty						,1							,DATATYPE_UINT16            ,FALSE  } /* VARIABLE_LITERS_FOR_TANK_EMPTY		       			*/ \
	, { &Power_Loss_Occurring						,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_POWER_LOSS_OCCURRING		       			*/ \
	, { &Cartridge_HMI_Data.Flags.All				,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_CARTRIDGE_HMI_DATA_FLAGS					*/ \
	, { &Cartridge_HMI_Data.Percentage_level		,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_CARTRIDGE_HMI_DATA_PERCENTAGE_LEVEL		*/ \
	, { &Enzime_Tag_Operating_Temperature			,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_ENZIME_TAG_OPERATING_TEMPERATURE						*/ \
	, { &Bleach_Tag_Operating_Temperature			,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_BLEACH_TAG_OPERATING_TEMPERATURE						*/ \
	, { &RinseAid_Tag_Operating_Temperature			,1							,DATATYPE_UINT8             ,FALSE  } /* VARIABLE_RINSEAID_TAG_OPERATING_TEMPERATURE					*/

/**
 * This is the translation table for the getters and setters that use Kvp Id to access the variables.
 *
 */
/*#define HMI_CATEGORY_VARIABLE_LIST                                                         \
    { &HMI_Variable.U8_Var               ,1                      ,DATATYPE_UINT8     ,FALSE  }, \
    { &HMI_Variable.U16_Var              ,1                      ,DATATYPE_UINT16    ,TRUE   }, \
    { &HMI_Variable.U32_Var              ,1                      ,DATATYPE_UINT32    ,FALSE  }, \
    { HMI_Variable.U8_Array_Var          ,ELEMENTS_IN_U8_ARRAY   ,DATATYPE_UINT8     ,TRUE   }, \
    { &HMI_Variable.S8_Var               ,1                      ,DATATYPE_SINT8     ,TRUE   }, \
    { &HMI_Variable.S16_Var              ,1                      ,DATATYPE_SINT16    ,FALSE  }, \
    { &HMI_Variable.S32_Var              ,1                      ,DATATYPE_SINT32    ,TRUE   }  \
*/
//=====================================================================================================================
//-------------------------------------- Remaining time Estimation Definitions ----------------------------------------
//=====================================================================================================================



#endif // VARIABLE_PRV_H_
