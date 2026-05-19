/**
 * @brief   Public parameters for the SeggerRTT module.
 *
 * @details See the @details in SeggerRTT.c for more information.
 *
 * @copyright Copyright (c) 2022 WhirlpoolCorporation
 */

#ifndef SEGGERRTT_PRM_H_
#define SEGGERRTT_PRM_H_

//  --- Include Files -------------------------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

/*
 * Macro is used to enable/disable the SeggerRTT module.
 * Not defining macro SEGGER_RTT_MESSAGES_ENABLED will disable the SeggerRTT module.
 * Defining macro SEGGER_RTT_MESSAGES_ENABLED will enable the SeggerRTT module.
 */
//#define SEGGER_RTT_MESSAGES_ENABLED


#ifdef SEGGER_RTT_MESSAGES_ENABLED

    // list of module ids
    #define SEGGER_RTT_MODULE_LIST\
        RTT_ERROR,\
        RTT_RECEPTACLE_MNGR,\
        RTT_RECEPTACLE_MNGR_FSM,\
        RTT_NFCC,\
        RTT_NFCC_FSM,\
        RTT_NCI,\
        RTT_NCI_FSM,\
        RTT_I2C,\
        RTT_I2C_INTERRUPT

    // define the module messages as enabled/disabled and what RTT Terminal the message will be displayed in
    #define SEGGER_RTT_MODULE_PARAM_LIST\
        /* RTT_ERROR */                 { TRUE, 0 },\
        /* RTT_RECEPTACLE_MNGR */       { TRUE, 1 },\
        /* RTT_RECEPTACLE_MNGR_FSM */   { TRUE, 2 },\
        /* RTT_NFCC */                  { TRUE, 3 },\
        /* RTT_NFCC_FSM */              { TRUE, 4 },\
        /* RTT_NCI */                   { FALSE, 3 },\
        /* RTT_NCI_FSM */               { TRUE, 5 },\
        /* RTT_I2C */                   { FALSE, 0 },\
        /* RTT_I2C_INTERRUPT */         { FALSE, 0 }

#endif

#endif /* SEGGERRTT_PRM_H_ */
