/******************************************************************************
* File Name: status_led_task.c
*
* Description: This file contains the task that that controls the status LEDs.
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
 * Header file includes
 ******************************************************************************/
#include "cybsp.h"
#include "cyhal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "status_led_task.h"
#include "uart_debug.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/
/* LED refresh interval of 500ms is used when continuously toggling a
 * status LED
 */
#define STATUS_LED_TOGGLE_INTERVAL (pdMS_TO_TICKS(500u))
/* LED refresh interval of 4s is used for blinking a status LED once */
#define STATUS_LED_BLINK_INTERVAL  (pdMS_TO_TICKS(4000u))
/* Idle interval is used for static LED states or when no LEDs are ON */
#define STATUS_LED_IDLE_INTERVAL   (portMAX_DELAY)


/* Queue handle used for commands to Task_StatusLed */
QueueHandle_t status_led_data_q;

/* Timer handles used to control LED blink / toggle intervals */
TimerHandle_t status_led_timer_handle;


/*******************************************************************************
 * Function prototype
 ******************************************************************************/
static void status_led_timer_start(void);
static void status_led_timer_update(TickType_t period);


/*******************************************************************************
* Function Name: vtask_status_led
********************************************************************************
* Summary:
*  Task that controls the status LED.
*
* Parameters:
*  void *param : Task parameter defined during task creation (unused)
*
*******************************************************************************/
void task_status_led(void *param)
{
    /* Variable that stores the data received over queue */
    status_led_data_t status_led_data;

    /* Variables used to detect changes to the LED states */
    static status_led_command_t led_state_cur = LED_TURN_OFF;

    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtos_api_result;

    /* Remove warning for unused parameter */
    (void)param ;

    /* Configure GPIO pin for driving LED */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* Start the timers that control LED blink / toggle intervals */
    status_led_timer_start();

    /* Repeatedly running part of the task */
    for(;;)
    {
        /* Block until a command has been received over status_led_data_q */
        rtos_api_result = xQueueReceive(status_led_data_q, &status_led_data,
                                        portMAX_DELAY);
        /* Command has been received from status_led_data_q */
        if(rtos_api_result == pdTRUE)
        {
            /* Take an action based on the command received for user_led LED */
            switch(status_led_data.user_led)
            {
                /* No change to the LED state */
                case LED_NO_CHANGE:
                {
                    break;
                }
                /* Turn ON the LED */
                case LED_TURN_ON:
                {
                    cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
                    /* Set the timer to idle mode */
                    status_led_timer_update(STATUS_LED_IDLE_INTERVAL);
                    led_state_cur = LED_TURN_ON;
                    break;
                }
                /* Turn OFF the LED */
                case LED_TURN_OFF:
                {
                    cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
                    /* Set the timer to idle mode */
                    status_led_timer_update(STATUS_LED_IDLE_INTERVAL);
                    led_state_cur = LED_TURN_OFF;
                    break;
                }
                /* Continuously toggle the LED */
                case LED_TOGGLE_EN:
                {
                    /* Update the timer period to toggle interval */
                    status_led_timer_update(STATUS_LED_TOGGLE_INTERVAL);
                    led_state_cur = LED_TOGGLE_EN;
                    break;
                }
                /* Blink the LED once */
                case LED_BLINK_ONCE:
                {
                    cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
                    /* Update the timer period to blink interval */
                    status_led_timer_update(STATUS_LED_BLINK_INTERVAL);
                    led_state_cur = LED_BLINK_ONCE;
                    break;
                }
                /* Refresh the LED based on the current state */
                case LED_TIMER_EXPIRED:
                {
                    if(led_state_cur == LED_TOGGLE_EN)
                    {
                        /* Toggle the LED */
                        cyhal_gpio_toggle(CYBSP_USER_LED);
                    }
                    else if (led_state_cur == LED_BLINK_ONCE)
                    {
                        /* Turn off the LED */
                        cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
                        /* Set the timer to idle mode */
                        status_led_timer_update(STATUS_LED_IDLE_INTERVAL);
                        led_state_cur = LED_TURN_OFF;
                    }
                    else
                    {
                        task_print_error("Refresh command for LED during"\
                                " invalid state");
                    }
                    break;
                }
                /* Invalid command received */
                default:
                {
                    task_print_error("Invalid  command for LED received. Error"\
                            " Code: %u", status_led_data.user_led);
                    break;
                }
            }
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
* Function Name: status_led_timer_callback
********************************************************************************
* Summary:
*  This function is called when the LED Timer expires
*
* Parameters:
*  TimerHandle_t xTimer :  Current timer value (unused)
*
*******************************************************************************/
static void status_led_timer_callback(TimerHandle_t xTimer)
{
    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtos_api_result;

    /* Remove warning for unused parameter */
    (void)xTimer;

    /* Send command to refresh the LED state */
    status_led_data_t led_refresh_data =
    {
        .user_led     = LED_TIMER_EXPIRED,
    };

    rtos_api_result = xQueueOverwrite(status_led_data_q, &led_refresh_data);

    /* Check if the operation has been successful */
    if(rtos_api_result != pdTRUE)
    {
        task_print_error("Sending data to Status LED queue");
    }
}


/*******************************************************************************
* Function Name: status_led_timer_start
********************************************************************************
* Summary:
*  This function starts the timer that provides timing to LED toggle / blink
*
*******************************************************************************/
static void status_led_timer_start(void)
{
    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtos_api_result;

    /* Create an RTOS timer */
    status_led_timer_handle =  xTimerCreate ("Status LED Timer",
                                             STATUS_LED_IDLE_INTERVAL, pdTRUE,
                                             NULL, status_led_timer_callback);

    /* Make sure that timer handle is valid */
    if (status_led_timer_handle != NULL)
    {
        /* Start the timer */
        rtos_api_result = xTimerStart(status_led_timer_handle, 0u);

        /* Check if the operation has been successful */
        if(rtos_api_result  != pdPASS)
        {
            task_print_error("LED Timer initialization");
        }
    }
    else
    {
        task_print_error("LED Timer creation");
    }
}


/*******************************************************************************
* Function Name: status_led_timer_update
********************************************************************************
* Summary:
*  This function updates the timer period per the parameter
*
* Parameters:
*  TickType_t period : Period of the timer in ticks
*
*******************************************************************************/
static void status_led_timer_update(TickType_t period)
{
    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtos_api_result;

    /* Change the timer period */
    rtos_api_result = xTimerChangePeriod(status_led_timer_handle, period, 0u);

    /* Check if the operation has been successful */
    if(rtos_api_result != pdPASS)
    {
        task_print_error("LED Timer update");
    }
}


/* [] END OF FILE */

