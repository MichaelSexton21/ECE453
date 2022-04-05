/******************************************************************************
* File Name: uart_debug.h
*
* Description: This file contains the macros that are used for UART based
*              debug.
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
* Include guard
*******************************************************************************/
#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

/*******************************************************************************
* Header files
*******************************************************************************/
#include "cy_retarget_io.h"

/* (true) enables UART based debug and (false) disables it.
 * Note that enabling debug reduces performance, power efficiency and
 * increases code size.
 */
#define DEBUG_ENABLE    (true)

/* Debug message type */
typedef enum
{
    none = 0,
    info = 1,
    warning = 2,
    error = 3
} debug_message_type_t;

#if (DEBUG_ENABLE)

#define task_print(...)         task_debug_printf(none, __VA_ARGS__)
#define task_print_info(...)    task_debug_printf(info, __VA_ARGS__)
#define task_print_warning(...) task_debug_printf(warning, __VA_ARGS__)
#define task_print_error(...)   task_debug_printf(error, __VA_ARGS__)

/* DebugPrintf is not thread-safe, and should not be used inside task */
#define debug_printf(...)       printf(__VA_ARGS__)

/*******************************************************************************
 * Function prototype
 ******************************************************************************/
void task_debug_printf(debug_message_type_t messageType, char* stringPtr, ...);
void task_debug_init(void);


/* Declaration of empty or default value macros if the debug is not enabled
 * for efficient code generation.
 */
#else

/*******************************************************************************
 * Function prototype
 ******************************************************************************/
#define task_debug_init(void)
#define task_debug_printf(...)
#define task_print(...)
#define task_print_info(...)
#define task_print_warnig(...)
#define task_print_error(...)
#define debug_printf(...)

#endif /* DEBUG_ENABLE */

#endif /* UART_DEBUG_H_ */
