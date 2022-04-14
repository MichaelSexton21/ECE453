/******************************************************************************
* File Name: battery_task.c
*
* Description: This file contains the task that simulates battery level.
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
*******************************************************************************/
#include "battery_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ble_task.h"
#include "cy_retarget_io.h"


/*******************************************************************************
 * Global constants
 ******************************************************************************/
#define BATTERY_LEVEL_MAX               (100u)
#define BATTERY_LEVEL_MIN               (0u)
#define BATTERY_LEVEL_INCREMENT         (10u)
#define BATTERY_LEVEL_INCREMENT_DELAY   (pdMS_TO_TICKS(2000u))   /* in second */


/*******************************************************************************
* Function Name: task_battery
********************************************************************************
* Summary:
*  Task that simulate battery level. The task increments battery level by
*  (BATTERY_LEVEL_INCREMENT) after (BATTERY_LEVEL_INCREMENT_DELAY)
*
* Parameters:
*  void *param : Task parameter defined during task creation (unused)
*
*******************************************************************************/
void task_battery(void* param)
{
    uint8_t battery_level = 0;

    /* Remove warning for unused parameter */
    (void)param;

    /* Repeatedly running part of the task */
    for(;;)
    {
        battery_level  = battery_level + BATTERY_LEVEL_INCREMENT;

        /* Keep the value in valid range */
        if(battery_level > BATTERY_LEVEL_MAX)
        {
            battery_level = BATTERY_LEVEL_MIN;
        }

        /* Send updated battery level value to BLE Task */
        ble_command_data_t ble_command_data =
        {
            .command = BATTERY_LEVEL_UPDATE,
            .data    = (uint32_t)battery_level
        };

        xQueueSend(ble_command_data_q, &ble_command_data, 0u);

        /* Block the task for 2 second */
        vTaskDelay(BATTERY_LEVEL_INCREMENT_DELAY);
    }
}


/* [] END OF FILE */
