/******************************************************************************
 * File Name: main.c
 *
 * Description: This is the source code for the PSoC 6 MCU with BLE
 *              Connectivity: Battery Level (FreeRTOS) Example for ModusToolbox.
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
 * Header files
 ******************************************************************************/
#include <ece453_led_task.h>
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ble_task.h"
#include "battery_task.h"
#include "status_led_task.h"
#include "uart_debug.h"
#include "ece453_led_task.h"

//Motor Driver Includes
#include "lvhb/lvhb.h"
#include "aml/wait_aml/wait_aml.h"
#include "motor.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/
/* Priority of user tasks in this application. Valid range of task priory is
 * priority is 0 to ( configMAX_PRIORITIES - 1 ), where configMAX_PRIORITIES is
 * defined within FreeRTOSConfig.h. Higher number means higher priority task.
 */
#define TASK_BLE_PRIORITY           (configMAX_PRIORITIES - 1)
#define TASK_BATTERY_PRIORITY       (configMAX_PRIORITIES - 2)
#define TASK_STATUS_LED_PRIORITY    (configMAX_PRIORITIES - 3)

/* Task names used in this application */
#define TASK_BLE_NAME               ("BLE Task")
#define TASK_STATUS_LED_NAME        ("Status LED Task")
#define TASK_BATTERY_NAME           ("Battery Task")

/* Stack sized used for task */
#define TASK_BLE_STACK_SIZE         (1024u)
#define TASK_STATUS_LED_STACK_SIZE  (configMINIMAL_STACK_SIZE)
#define TASK_BATTERY_STACK_SIZE     (configMINIMAL_STACK_SIZE)
#define TASK_LED_STACK_SIZE         (configMINIMAL_STACK_SIZE)

/* Queue lengths of message queues used in this application  */
#define BLE_QUEUE_LENGTH            (10u)
#define STATUS_LED_QUEUE_LENGTH     (1u)

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 *  This is the main function for CM4 CPU. This function creates required queues,
 *  sets up user tasks and then starts the RTOS scheduler.
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main(void) {
	cy_rslt_t result = CY_RSLT_SUCCESS;
	BaseType_t rtos_api_result = pdPASS;

	///////////// Motor interface config //////////////////////////////////////////////////
	lvhb_drv_config_t drvConfig;
	cyhal_pwm_t pwm_obj_motor1;
	cyhal_pwm_t pwm_obj_motor2;

	/* Initialize Motors - each has one GPIO and one PWM */
	/* MOTOR 1 */
	/* IN1A (GPIO) + IN1B (PWM) */
	cyhal_gpio_init(
			MOTOR1_INA,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue
	cyhal_pwm_init(
			&pwm_obj_motor1,				// PWM Object
			MOTOR1_INB,                // Pin
			NULL);						// Clock

	/* MOTOR 2 */
	/* IN1A (GPIO) + IN1B (PWM) */
	cyhal_gpio_init(
			MOTOR2_INA,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue

	cyhal_pwm_init(
			&pwm_obj_motor2,				// PWM Object
			MOTOR2_INB,                // Pin
			NULL);

	/* MOTOR OE Enable */
	cyhal_gpio_init(
			MOTOR_OEn,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			true);

	/* Fill LVHB driver config. */
	LVHB_GetDefaultConfig(&drvConfig, lvhbDeviceMPC17529, lvhbMotorBrushed);

	drvConfig.deviceConfig.brushPwmFrequency = 10000;
	drvConfig.deviceConfig.secondaryBridgeUsed = true;
	drvConfig.deviceConfig.motorType = lvhbMotorBrushed;

	//drvConfig.enPinIndex = MOTOR_OEn; - maybe we don't need this?

	/* Timer settings - #TODO */
	drvConfig.tmrInstance = 0; /* FTM0 */
	drvConfig.tmrLvhbConfig.counterWidth = 16;
	drvConfig.tmrLvhbConfig.prescale = tmrPrescDiv_8;
	drvConfig.tmrLvhbConfig.srcClck_Hz = CLOCK_GetFreq(kCLOCK_BusClk);

	/* Pins */
	drvConfig.inputPins[lvhbBridge1] = lvhbPinsGpioPwm; /* IN1A (GPIO) + IN1B (PWM) */
	drvConfig.inxaPinInstance[lvhbBridge1] = instanceA; /* IN1A - PTA2/FTM0_CH7 */
	drvConfig.inxaPinIndex[lvhbBridge1] = 2U;
	drvConfig.tmrLvhbConfig.inxbChannelNumber[lvhbBridge1] = 1; /* IN1B - PTC2/FTM0_CH1 */

	drvConfig.inputPins[lvhbBridge2] = lvhbPinsGpioPwm; /* IN2A (GPIO) + IN2B (PWM) */
	drvConfig.inxaPinInstance[lvhbBridge2] = instanceC; /* IN2A - PTC12 */
	drvConfig.inxaPinIndex[lvhbBridge2] = 12U;

	drvConfig.inxbPinInstance[lvhbBridge2] = instanceC; /* IN2B - PTC4/FTM0_CH3 */
	drvConfig.inxbPinIndex[lvhbBridge2] = 4U;

	///////////// Motor interface config //////////////////////////////////////////////////


	/* Initialize the device and board peripherals */
	result = cybsp_init();

	/* Board init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	/* Initialize retarget-io to use the debug UART port */
	cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
	CY_RETARGET_IO_BAUDRATE);

	/* Enable global interrupts */
	__enable_irq();

	/* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
	printf("\x1b[2J\x1b[;H");

	printf("******** "
			"PSoC 6 MCU with BLE connectivity Battery Level (FreeRTOS)"
			" ********\r\n\n");

	/* Create a task to do thread-safe debug message printing */
	task_debug_init();

	/* Create the queues. See the respective data-types for details of queue
	 * contents
	 */
	ble_command_data_q = xQueueCreate(BLE_QUEUE_LENGTH,
			sizeof(ble_command_data_t));
	status_led_data_q = xQueueCreate(STATUS_LED_QUEUE_LENGTH,
			sizeof(status_led_data_t));

	ece453_led_data_q = xQueueCreate(1, sizeof(ece453_led_data_t));

	/* Create the user tasks. See the respective Task definition for more
	 * details of these tasks */
	rtos_api_result |= xTaskCreate(task_ble, TASK_BLE_NAME,
	TASK_BLE_STACK_SIZE, NULL,
	TASK_BLE_PRIORITY, NULL);
	rtos_api_result |= xTaskCreate(task_status_led, TASK_STATUS_LED_NAME,
	TASK_STATUS_LED_STACK_SIZE, NULL,
	TASK_STATUS_LED_PRIORITY, NULL);
	rtos_api_result |= xTaskCreate(task_battery, TASK_BATTERY_NAME,
	TASK_BATTERY_STACK_SIZE, NULL,
	TASK_BATTERY_PRIORITY, NULL);

	rtos_api_result |= xTaskCreate(task_ece453_led, "ECE453 LED TASK", 256,
	NULL, 1,
	NULL);

	if (pdPASS == rtos_api_result) {
		/* Start the RTOS scheduler. This function should never return */
		vTaskStartScheduler();

		/* Program should never reach here! */
		debug_printf("[Error]   : FreeRTOS scheduler failed to start \r\n");
	} else {
		debug_printf("[Error]   : FreeRTOS failed to create task \r\n");
	}

	/* Halt the CPU if failed to create task or failed to start scheduler */
	CY_ASSERT(0u);

	for (;;) {
	}
}

/*******************************************************************************
 * Function Name: vApplicationStackOverflowHook
 ********************************************************************************
 * Summary:
 *  This function is called when a stack overflow has been detected by the RTOS
 *
 * Parameters:
 *  TaskHandle_t  : Handle to the task
 *  signed char   : Name of the task
 *
 *******************************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t *pxTask,
		signed char *pcTaskName) {
	/* Remove warning for unused parameters */
	(void) pxTask;

	/* Print the error message with task name if debug is enabled in
	 * uart_debug.h file */
	debug_printf("[Error]   : RTOS - stack overflow in %s \r\n", pcTaskName);

	/* Halt the CPU */
	CY_ASSERT(0);
}

/*******************************************************************************
 * Function Name: void vApplicationMallocFailedHook(void)
 ********************************************************************************
 * Summary:
 *  This function is called when a memory allocation operation by the RTOS
 *  has failed
 *
 *******************************************************************************/
void vApplicationMallocFailedHook(void) {
	/* Print the error message if debug is enabled in uart_debug.h file */
	debug_printf("[Error]   : RTOS - Memory allocation failed \r\n");

	/* Halt the CPU */
	CY_ASSERT(0);
}

/* [] END OF FILE */

