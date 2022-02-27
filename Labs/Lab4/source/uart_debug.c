/******************************************************************************
* File Name: uart_debug.c
*
* Description: This file contains the task that is used for thread-safe UART
*              based debug.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
 * Include header files
 ******************************************************************************/
#include <stdarg.h>
#include "cycfg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart_debug.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
/* Queue length of message queue used for debug message printing */
#define DEBUG_QUEUE_SIZE        (16u)

/* Maximum allowed length of debug message */
#define DEBUG_MESSAGE_MAX_LEN   (100u)

#if (DEBUG_ENABLE)

/* Queue handle for debug message Queue */
QueueHandle_t debug_message_q;

/* Structure for storing debug message data */
typedef struct
{
    const char* str_ptr;
    debug_message_type_t message_type;
} debug_message_data_t;


/*******************************************************************************
* Function Name: task_debug
********************************************************************************
* Summary:
*  Task that prints debug messages.
*
* Parameters:
*  void *param : Task parameter defined during task creation (unused)
*
*******************************************************************************/
void task_debug(void* param)
{
    debug_message_data_t message_data;

    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtos_api_result;

    /* Remove warning for unused parameter */
    (void) param;

    /* Repeatedly running part of the task */
    for(;;)
    {
        rtos_api_result = xQueueReceive(debug_message_q, &message_data,
                                      portMAX_DELAY);

        if(rtos_api_result == pdPASS)
        {
            switch(message_data.message_type)
            {
                case none:
                {
                    printf("%s", (char *)message_data.str_ptr);
                    break;
                }
                case info:
                {
                    printf("[Info]    : %s\r\n", (char *)message_data.str_ptr);
                    break;
                }
                case warning:
                {
                    printf("[Warning] : %s\r\n", (char *)message_data.str_ptr);
                    break;
                }
                case error:
                {
                    printf("[Error]   : %s\r\n", (char *)message_data.str_ptr);
                    break;
                }
                default:
                {
                    break;
                }
            }

            /* free the message buffer allocated by the message sender */
            vPortFree((char*)message_data.str_ptr);
        }
        /* Task has timed out and received no commands during an interval of
         * portMAXDELAY ticks
         */
        else
        {
        }
    }
}


/*******************************************************************************
* Function Name: task_debug_printf
********************************************************************************
* Summary:
*  This function sends messages to the debug Queue.
*
*******************************************************************************/
void task_debug_printf(debug_message_type_t message_type, char* str_ptr, ...)
{
    debug_message_data_t message_data;
    char* message_buffer;
    char* task_name;
    uint32_t length = 0;
    va_list args;

    /* Allocate the message buffer */
    message_buffer = pvPortMalloc(DEBUG_MESSAGE_MAX_LEN);

    if(message_buffer)
    {
        va_start(args, str_ptr);
        if(message_type != none)
        {
            task_name = pcTaskGetName(xTaskGetCurrentTaskHandle());
            length = snprintf(message_buffer, DEBUG_MESSAGE_MAX_LEN, "%-16s : ",
                task_name);
        }

        vsnprintf((message_buffer + length), (DEBUG_MESSAGE_MAX_LEN - length),
                str_ptr, args);

        va_end(args);

        message_data.message_type = message_type;
        message_data.str_ptr = message_buffer;

        /* The receiver task is responsible to free the memory from here on */
        if(pdPASS != xQueueSendToBack(debug_message_q, &message_data, 0u))
        {
            /* Failed to send the message into the queue */
            vPortFree(message_buffer);
        }
    }
    else
    {
        /* pvPortMalloc failed. Handle error */
    }
}


/*******************************************************************************
* Function Name: task_debug_init
********************************************************************************
* Summary:
*  Initializes the underlying Task and Queue used for printing debug
*  messages.
*
*******************************************************************************/
void task_debug_init(void)
{
    debug_message_q = xQueueCreate(DEBUG_QUEUE_SIZE,
                                   sizeof(debug_message_data_t));

    xTaskCreate(task_debug, "Debug Task", configMINIMAL_STACK_SIZE,
                NULL, (tskIDLE_PRIORITY + 1u), NULL);
};


#endif /* DEBUG_ENABLE */
