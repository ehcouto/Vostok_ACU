/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      Contains controller specific definition
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef ATOD_DEFS_H_
#define ATOD_DEFS_H_

//=====================================================================================================================
/**
 *    @brief  - Each channel is programmable for sampling time
 *    @details- The total conversion time is calculated as follows:
 *              Tconv = Sampling time + 12.5 cycles
 */
//=====================================================================================================================
typedef enum
{
    SAMPLE_TIME_1_5_CYCLE = 0,  //1.5 ADC Clock Cycle
    SAMPLE_TIME_3_5_CYCLE,      //3.5 ADC Clock Cycle
    SAMPLE_TIME_7_5_CYCLE,      //7.5 ADC Clock Cycle
    SAMPLE_TIME_12_5_CYCLE,     //12.5 ADC Clock Cycle
    SAMPLE_TIME_19_5_CYCLE,     //19.5 ADC Clock Cycle
    SAMPLE_TIME_39_5_CYCLE,     //39.5 ADC Clock Cycle
    SAMPLE_TIME_79_5_CYCLE,     //79.5 ADC Clock Cycle
    SAMPLE_TIME_160_5_CYCLE,    //160.5 ADC Clock Cycle
}ATOD_SAMPLE_TIME_DEF;

#endif // ATOD_DEFS_H_


