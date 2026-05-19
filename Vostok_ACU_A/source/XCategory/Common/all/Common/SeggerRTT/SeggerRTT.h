/**
 * @brief       Public interface file for the SeggerRTT module.
 *
 * @details     See the @details in the SeggerRTT source file for more details.
 *
 * @copyright Copyright (c) 2022 WhirlpoolCorporation
 */
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

#ifndef SEGGERRTT_H_
#define SEGGERRTT_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

#include "SeggerRTT_prm.h"

#ifdef SEGGER_RTT_MESSAGES_ENABLED

    #include "SEGGER_RTT.h"

    //=====================================================================================================================
    //  --- Public Properties ---------------------------------------------------------------------------------------------
    //=====================================================================================================================

    // Segger RTT has the ability to post messages to a specific terminal.
    typedef uint8 RTT_TERMINAL_TYPE;

    // list of module ids
    typedef enum
    {
        SEGGER_RTT_MODULE_LIST,
        NUM_OF_RTT_MODULES
    } SEGGER_RTT_MODULE_TYPE;

    // configuration that must be defined for each module listed in the SEGGER_RTT_MODULE_TYPE enumeration
    typedef struct
    {
        BOOL_TYPE           Enabled;    // TRUE = module messages will be posted to RTT
        RTT_TERMINAL_TYPE   Terminal;
    } SEGGER_RTT_MODULE_PARAM_TYPE;
    static const SEGGER_RTT_MODULE_PARAM_TYPE SEGGER_RTT_MODULE_PARAM[] =
    {
        SEGGER_RTT_MODULE_PARAM_LIST
    };
    CT_ASSERT(ELEMENTS_IN_ARRAY(SEGGER_RTT_MODULE_PARAM) == NUM_OF_RTT_MODULES);


    //=====================================================================================================================
    //  --- Public Methods ------------------------------------------------------------------------------------------------
    //=====================================================================================================================

    void SeggerRTT__ConfigUpBuffer(uint8 buffer);

    /*
     * @brief   Used to write a message to the RTT.
     *
     * @param   module    the module
     * @param   s           pointer to the string to write
     */
    #define SeggerRTT__WriteTerminal(module, s)\
        do {\
            if (( module < NUM_OF_RTT_MODULES ) &&\
                ( SEGGER_RTT_MODULE_PARAM[module].Enabled == TRUE ))\
            {\
                SEGGER_RTT_SetTerminal(SEGGER_RTT_MODULE_PARAM[module].Terminal);\
                SEGGER_RTT_WriteString(0, s);\
            }\
        } while (0)

    /*
     * @brief   Used to write a message to the RTT.
     *
     * @param   module    the module
     * @param   s           pointer to the string to write
     * @param   arg         the arguments to the string
     */
    #define SeggerRTT__printfTerminal(module, s, arg)\
        do {\
            if (( module < NUM_OF_RTT_MODULES ) &&\
                ( SEGGER_RTT_MODULE_PARAM[module].Enabled == TRUE ))\
            {\
                SEGGER_RTT_SetTerminal(SEGGER_RTT_MODULE_PARAM[module].Terminal);\
                SEGGER_RTT_printf(0, s, arg);\
            }\
        } while (0)


    void SeggerRTT__ConfigureUpBuffer0(void);

#else

    #define SeggerRTT__WriteTerminal(terminal, s)
    #define SeggerRTT__printfTerminal(terminal, s, arg)



#endif

#endif /* SEGGERRTT_H_ */
