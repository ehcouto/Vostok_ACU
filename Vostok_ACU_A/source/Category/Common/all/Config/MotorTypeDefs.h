/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef MOTORTYPEDEFS_H_
#define MOTORTYPEDEFS_H_

typedef enum
{
    UNUSED_0                            = 0,     // Not Used 0
    WM_VARIAB_SPEED_3PH                 = 1,     // VSM Motor
    WM_SINGLE_SPEED_ASYNCH_SPH_PSC      = 2,     // RIF motor
    UNUSED_3                            = 3,     // Not Used 3
    UNUSED_4                            = 4,     // Not Used 4
    WM_SINGLE_SPEED_SYNCH_SPH_PW        = 5,     // Single Phase Permanent Magnet Synchronous Motor Permanent Wash - GWS
    WM_SINGLE_SPEED_SYNCH_SPH_AW        = 6      // Single Phase Permanent Magnet Synchronous Motor Alternating Wash - GWS - (Default)
}APL_WASH_MOTOR_TYPE;


#endif /* MOTORTYPEDEFS_H_ */
