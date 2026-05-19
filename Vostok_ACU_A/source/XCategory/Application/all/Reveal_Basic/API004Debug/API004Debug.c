/**
 *  @file       
 *
 *  @brief      Allows to interact with your project allowing to debug your system at different levels
 *
 *  @details    EXECUTE METHOD APIS:
 *              The API004Debug module provides commands to execute global methods of your projects
 *              and allows to receive feedbacks , if any, from the method itself, such as the
 *              return value of the function.
 *              The methods have to be listed in a table described in the prv file and , for each
 *              method, you need to insert a label (a string to mark the method), the pointer to the method
 *              and a Signature value that contains the information of the data types for the input and ouput
 *              of the method.
 *              The Api uses 2 opcodes for this purpose: one to publish the information of all the listed methods
 *              and another one to execute the method with the required input parameter values.
 *              If the executed method has a return value, the value will be published in the feedback packet
 *
 *              To Enable this module you have to use the SystemConfig_prm file since the definition value is also
 *              used by the "Reveal_prv" file to allow to correct use of the parser functionalities
 *  @copyright  Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#if (API004DEBUG_FEATURE == ENABLED)
#include "API004Debug_prv.h"
#include "API004Debug.h"

// -- Other Modules
#include "Utilities.h"
#include <string.h>
#include "Log.h"

#if (SW05_SOFTWARE_TESTS_FEATURE == ENABLED)
    #include "SW05SoftwareTest.h"
#endif
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! maximum number of function input parameters allowed
#define MAX_PARAMETERS_NUMBER   (sizeof(uint32)*2)-1

//!Reveal more message pending value
#define REVEAL_MMP_BIT  ((uint8)0x80)

//!Maximum length of the label describing the method
#define LABEL_MAX_LENGTH    40

//!Value of the wrong index to address the execution of a method
#define WRONG_FUNCTION_INDEX    0xFF

//!Bit mask value to match the data type information of the return value inside the Signature variable
#define RETURN_VALUE_POSITION_MASK  0x0000000F

//! Character to terminate the label string
#define NULL_CHAR 0x00U

//! Function ID is always payload[0]
#define FIXED_ARGS_FUNCTION_ID                 buffer->payload[0]

//! Input parameters starts from payload[1]
#define FIXED_ARGS_INPUT_PARAMETERS            buffer->node_id, (buffer->size -1), &buffer->payload[1]

#ifdef DEBUG_FIXED_ARGS_APIS_LIST
    //! Array containing the Complex Functions info
    static const DEBUG_FIXED_ARGS_FUNCTION_TYPE DEBUG_FIXED_ARGS_FUNCTIONS_LIST[] = DEBUG_FIXED_ARGS_APIS_LIST;

    #define NUMBER_OF_FIXED_ARGS_FUNCTIONS sizeof(DEBUG_FIXED_ARGS_FUNCTIONS_LIST)/sizeof(DEBUG_FIXED_ARGS_FUNCTION_TYPE)
#endif


#ifdef DEBUG_APIS_LIST
//! The data type applied to the function pointer without a return value
typedef void (*DEBUG_FUNCTION_HANDLER_TYPE)();

//! The data type applied to the function pointer with a return value
typedef sint32 (*DEBUG_FUNCTION_WITH_RETURN_HANDLER_TYPE)();

//! Array containing the Functions info
static const DEBUG_FUNCTION_TYPE DEBUG_FUNCTIONS_LIST[] = DEBUG_APIS_LIST;

//!Number of functions in the DEBUG_FUNCTIONS_LIST
#define NUMBER_OF_FUNCTIONS sizeof(DEBUG_FUNCTIONS_LIST)/sizeof(DEBUG_FUNCTION_TYPE)
// The NUMBER_OF_FUNCTIONS must be smaller than 255
CT_ASSERT(NUMBER_OF_FUNCTIONS < 0xFF);
#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

#ifdef DEBUG_APIS_LIST
static REVEAL_RECIPE_STATUS_TYPE ExecuteFunction(sint32 array[],uint8 function_position,uint8 node_id,BOOL_TYPE return_value_function);
static BOOL_TYPE PrintOnReveal(uint8 function_id, char* str, uint32 sign,BOOL_TYPE mmp_enabled, uint8 node_id);
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API004Debug commands sent to this node.
 * @param buffer = Pointer to the received command message.
 * @return REVEAL_RECIPE_STATUS_TYPE that expresses the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API004Debug__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval = NVO_RS_READY;
    #ifdef DEBUG_APIS_LIST
    uint8 index;
    uint8 current_index;
    BOOL_TYPE function_with_return_value;
    sint32 input_parameters_list[MAX_PARAMETERS_NUMBER] = {0};
    DATATYPE_TYPE param_array[MAX_PARAMETERS_NUMBER] ={DATATYPE_INVALID,DATATYPE_INVALID,DATATYPE_INVALID,DATATYPE_INVALID,DATATYPE_INVALID,DATATYPE_INVALID,DATATYPE_INVALID};

    current_index = 0;
    function_with_return_value = TRUE;
    #endif // #ifdef DEBUG_APIS_LIST

    switch (buffer->op_code)
    {
        #ifdef DEBUG_APIS_LIST
        case API004_CMD_EXECUTE_FUNCTION:
            if ( (buffer->payload[0] < NUMBER_OF_FUNCTIONS) &&
                 ((DEBUG_FUNCTIONS_LIST[buffer->payload[0]].Pointer) != NULL)
                )
            {
                /*collect the information about the data type of the input parameter if any*/
                for(index = 0; index < MAX_PARAMETERS_NUMBER ; index++)
                {
                    param_array[index]= (DATATYPE_TYPE)(((DEBUG_FUNCTIONS_LIST[buffer->payload[0]].Signature) & ((uint32)ELEMENT_MASK<<((index+1)* ELEMENT_BITS)))/(uint32)(16<<(index*ELEMENT_BITS)));
                    if(param_array[index]== DATATYPE_INVALID)
                    {
                      break;
                    }
                }

                /*store the value of the input parameters to pass to the function*/
                for(uint8 param_index=0; param_index<index;param_index++)
                {
                    switch(param_array[param_index])
                    {
                        case DATATYPE_UINT8:
                            input_parameters_list[param_index] = buffer->payload[current_index+1];
                            current_index = current_index+sizeof(uint8);
                            break;
                        case DATATYPE_SINT8:
                            input_parameters_list[param_index] = (sint8)(buffer->payload[current_index+1]);
                            current_index = current_index+sizeof(sint8);
                            break;
                        case DATATYPE_UINT16:
                            input_parameters_list[param_index] = Utilities__ConvertArrayTo16bits(&(buffer->payload[current_index+1]));
                            current_index = current_index+sizeof(uint16);
                            break;
                        case DATATYPE_SINT16:
                            input_parameters_list[param_index] = (sint16)(Utilities__ConvertArrayTo16bits(&(buffer->payload[current_index+1])));
                            current_index = current_index+sizeof(sint16);
                            break;
                        case DATATYPE_UINT32:
                        case DATATYPE_SINT32:
                            input_parameters_list[param_index] =Utilities__ConvertArrayTo32bits(&(buffer->payload[current_index+1]));
                            current_index = current_index+sizeof(uint32);
                            break;
                        default:
                            break;
                    }
                }

                if( ((DEBUG_FUNCTIONS_LIST[buffer->payload[0]].Signature) & RETURN_VALUE_POSITION_MASK) == DATATYPE_VOID)
                {
                    function_with_return_value = FALSE;
                }

                retval = ExecuteFunction(input_parameters_list,buffer->payload[0],buffer->node_id,function_with_return_value);
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;

       case API004_CMD_REQUEST_FUNCTIONS_INFO:
            /*scroll the Functions name list to publish the Functions information*/
            for(uint8 function_position=0; function_position<NUMBER_OF_FUNCTIONS;function_position++)
            {
                BOOL_TYPE mmp = ( (function_position == (NUMBER_OF_FUNCTIONS-1)) ? FALSE:TRUE);

                if (PrintOnReveal(function_position,DEBUG_FUNCTIONS_LIST[function_position].Name,DEBUG_FUNCTIONS_LIST[function_position].Signature,mmp,buffer->node_id) == FALSE)
                {
                    retval = NVO_RS_UNSUP_UNAVAILABLE;
                }
            }
           break;
        #endif // #ifdef DEBUG_APIS_LIST

        case API004_CMD_EXECUTE_FIXED_ARGS_FUNCTIONS:
            /**
             * ->payload[0] - Complex function ID,
             *                  0-254 for user configurable
             *                  255 is reserved for SW05SoftwareTest
             *
             * ->payload[1-x] - FIXED_ARGS_INPUT_PARAMETERS
             */
            retval = NVO_RS_UNSUP_UNAVAILABLE;

            #if (SW05_SOFTWARE_TESTS_FEATURE == ENABLED)
                if (FIXED_ARGS_FUNCTION_ID == FIXED_ARGS_FUNCTION_ID_SW05SOFTWARE)
                {
                    SW05SoftwareTest__Execute(FIXED_ARGS_INPUT_PARAMETERS);
                    retval = NVO_RS_READY;
                }
                else
            #endif
                {
                    #ifdef DEBUG_FIXED_ARGS_APIS_LIST
                        if(FIXED_ARGS_FUNCTION_ID < NUMBER_OF_FIXED_ARGS_FUNCTIONS)
                        {
                            ((DEBUG_FIXED_ARGS_FUNCTION_HANDLER_TYPE)DEBUG_FIXED_ARGS_FUNCTIONS_LIST[FIXED_ARGS_FUNCTION_ID].Pointer)(FIXED_ARGS_INPUT_PARAMETERS);
                            retval = NVO_RS_READY;
                        }
                    #endif
                }
           break;

        case API004_CMD_LOG_MESSAGES_SEVERITY_FILTER:
            {
                //If the payload is empty then this opcode will send the current 'severity filter' status without setting it
                if (buffer->size > 0)
                {
                    Log__SetMessageSeverityFilter((LOG_SEVERITY_FILTER_TYPE)buffer->payload[0]);
                }

                REVEAL_COMMAND_TYPE * pt = Reveal__NewFeedback(API004DEBUG_NUM, API004_FBK_LOG_MESSAGES_SEVERITY_FILTER, buffer->node_id, 1);
                if (pt != (REVEAL_COMMAND_TYPE *)NULL)
                {
                    Reveal__AddDataU8(pt,(uint8)Log__GetMessageSeverityFilter());
                    Reveal__QueueCommand(pt);
                }
            }
            break;

        default:
            retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API is used by the 'Complex Execute Functions' to send the feedback message to the 'API004_CMD_EXECUTE_FIXED_ARGS_FUNCTIONS'
 *
 * @param destin_node_id    - A node to who this feedback to be sent
 * @param length            - Number of bytes to send from buffer_ptr
 * @param set_mmp           - Set to 'TRUE' if more data pending or else set to 'FALSE'
 * @param buffer_ptr        - Send data from this buffer pointer
 */
void API004Debug__SendFbkForFixedArgsFunctions(uint8 destin_node_id, uint8 length, BOOL_TYPE set_mmp, uint8 *buffer_ptr)
{
    REVEAL_COMMAND_TYPE *reveal_fbk_ptr = Reveal__NewFeedback(API004DEBUG_NUM, API004_FBK_EXECUTE_FIXED_ARGS_FUNCTIONS, destin_node_id, (REVEAL_PAYLOAD_SIZE - 4));

    if (reveal_fbk_ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        //set the mmp bit if their are more than one message needs to be send
        if(set_mmp != FALSE)
        {
            reveal_fbk_ptr->opcode |= REVEAL_MMP_BIT;
        }

        Reveal__AddDataU8Buffer(reveal_fbk_ptr,buffer_ptr, length);
        Reveal__QueueCommand(reveal_fbk_ptr);
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#ifdef DEBUG_APIS_LIST
/**
 * Call the function to be executed based on the parameters
 * @param element_list: it is the array containing the values of the input parameters to pass to the function
 * @param function_position:index of the function inside the list
 * @param node_id: the value of the node requesting the function execution
 * @param return_value_function: flag informing if the function has a return value or not
 * @return REVEAL_RECIPE_STATUS_TYPE that expresses the result of the parsing.
 */

static REVEAL_RECIPE_STATUS_TYPE ExecuteFunction(sint32 element_list[],uint8 function_position,uint8 node_id,BOOL_TYPE return_value_function)
{
    #define INPUT_PARAMETERS element_list[0],element_list[1],element_list[2],element_list[3],element_list[4],element_list[5],element_list[6]
    REVEAL_RECIPE_STATUS_TYPE retval;
    retval = NVO_RS_READY;

    if (return_value_function == FALSE)
    {
        /*Execute the function with no return value*/
        ((DEBUG_FUNCTION_HANDLER_TYPE)DEBUG_FUNCTIONS_LIST[function_position].Pointer)(INPUT_PARAMETERS);

        /*Publish the feedback with only the function index information*/
        Reveal__QueueMessageU8(API004DEBUG_NUM,API004_FBK_CONFIRM_EXECUTION,node_id,FALSE,FALSE,function_position);
    }
    else
    {
        sint32 return_value;
        uint8 return_value_data_type;
        REVEAL_COMMAND_TYPE* rev_feedback = Reveal__NewFeedback(API004DEBUG_NUM, API004_FBK_CONFIRM_EXECUTION, node_id, sizeof(function_position)+sizeof(return_value));
        /*Execute the function with a return value*/
        return_value = ((DEBUG_FUNCTION_WITH_RETURN_HANDLER_TYPE)DEBUG_FUNCTIONS_LIST[function_position].Pointer)(INPUT_PARAMETERS);
        return_value_data_type = (uint8)(DEBUG_FUNCTIONS_LIST[function_position].Signature & RETURN_VALUE_POSITION_MASK);
        /*Publish the feedback with the return value information*/
        if( (Reveal__AddDataU8(rev_feedback, function_position)== FAIL) ||
            (Reveal__AddDataU32(rev_feedback, return_value)== FAIL) ||
            (Reveal__AddDataU8(rev_feedback, return_value_data_type)== FAIL) ||
            (Reveal__QueueCommand(rev_feedback)== FALSE)
            )
        {
            retval = NVO_RS_BUSY;
        }
    }

    return(retval);
}

/**
 * Publish on the Bus the information about the functions present in the List
 * @param function_id: index of the function inside the list
 * @param str: ASCII characters label describing the function name
 * @param sign: Signature Information of the function
 * @param mmp_enabled: flag used in case of multiple packets; 1 means another packet has to be sent, 0 means that is the last packet
 * @param node_id: the node to address the information
 * @return TRUE if data to publish is consistent, FALSE if not
 */
static BOOL_TYPE PrintOnReveal(uint8 function_id,char* str,uint32 sign,BOOL_TYPE mmp_enabled,uint8 node_id)
{
    uint8 message_size;
    BOOL_TYPE result;

    message_size = 0;
    result = FALSE;

    message_size = strlen(str);
    if (message_size > LABEL_MAX_LENGTH)
    {
        message_size = LABEL_MAX_LENGTH;
    }

    if(message_size > 0)
    {
        REVEAL_COMMAND_TYPE* rev_feedback = Reveal__NewFeedback(API004DEBUG_NUM, API004_FBK_FUNCTIONS_INFO, node_id, sizeof(function_id)+sizeof(sign)+message_size+1);
        if(mmp_enabled == TRUE)  /*if Multi Message Packet is requested, the MMP bit is set*/
        {
            rev_feedback->opcode |= REVEAL_MMP_BIT;
        }

        result = (BOOL_TYPE)((Reveal__AddDataU8(rev_feedback, function_id)== PASS) &&
                             (Reveal__AddDataU32(rev_feedback, sign)== PASS) &&
                             (Reveal__AddDataU8Buffer(rev_feedback, (uint8*)str, message_size)== PASS) &&
                             (Reveal__AddDataU8(rev_feedback, NULL_CHAR)== PASS) &&
                             (Reveal__QueueCommand(rev_feedback)== TRUE));

    }
    return result;
}
#endif// #ifdef DEBUG_APIS_LIST
#endif//#if (API004DEBUG_FEATURE == ENABLED)
