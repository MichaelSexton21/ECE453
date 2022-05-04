/******************************************************************************
 * File Name: main.c
 *
 * Description: This is the source code for the PSoC 6 MCU with BLE
 *
 * Authors: Emily Cebasek, Mya Schmitz, Michael Sexton, Anna Stephan
 *
/***************                                                                                                                                                                                                                               ***************************************************************
 * Header files
 ******************************************************************************/
#include <buzzer_task.h>
#include <led_task.h>
#include <motor_task.h>
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ble_task.h"
#include "status_led_task.h"
#include "uart_debug.h"

//PWM for motors & buzzer

cyhal_pwm_t pwm_obj_motor1;
cyhal_pwm_t pwm_obj_motor2;

cyhal_pwm_t pwm_obj_buzzer;

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
#define TASK_LEDS_PRIORITY    (configMAX_PRIORITIES - 4)
#define TASK_BUZZER_PRIORITY  (configMAX_PRIORITIES - 4)
#define TASK_MOTOR_PRIORITY   (configMAX_PRIORITIES - 4)

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
#define BLE_QUEUE_LENGTH            (10)//u
#define STATUS_LED_QUEUE_LENGTH     (1)//u
#define MOTOR_QUEUE_LENGTH			(1)//u
#define BUZZER_QUEUE_LENGTH			(1)//u

/*******************************************************************************
 * Function Name: Initial Hardware Test
 ********************************************************************************
 * Summary:
 *  This function will turn both wheels on at half speed, turn all RGB on, and turn buzzer on
 *
 *******************************************************************************/
void inital_hardware_test() {
	//cy_rslt_t rslt;

	//rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_red, 0, 1000);
	//rslt = cyhal_pwm_start(&pwm_obj_blue);
	//rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_blue, 100, 1000);
	//rslt = cyhal_pwm_start(&pwm_obj_blue);
	cyhal_gpio_toggle(PIN_LED_RED);
	cyhal_gpio_toggle(PIN_LED_GREEN);
	cyhal_gpio_toggle(PIN_LED_BLUE);

	//Flick LEDs on and off one by one
	for (int i = 0; i < 5; i++) {
		cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_ON);
		vTaskDelay(1000);

		cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_OFF);
		cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_ON);
		vTaskDelay(1000);

		cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_OFF);
		cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_ON);
		vTaskDelay(1000);

		cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_OFF);
	}

	cyhal_gpio_write(PIN_BUZZER, 0U);
	cyhal_pwm_set_duty_cycle(&pwm_obj_buzzer, 50, 2000);

	//Turn buzzer on and off for 5 seconds
	for (int i = 0; i < 3; i++) {
		cyhal_gpio_write(PIN_BUZZER, CYBSP_LED_STATE_OFF);
		vTaskDelay(1000);
	}

	//Make motors go forward at 50% speed
	cyhal_gpio_write(MOTOR_OEn, MOTOR_STATE_ON);
	cyhal_gpio_write(MOTOR1_INB, MOTOR_STATE_ON);
	cyhal_gpio_write(MOTOR2_INB, MOTOR_STATE_ON);

	cyhal_pwm_set_duty_cycle(&pwm_obj_motor1, 50, 1000);
	cyhal_pwm_set_duty_cycle(&pwm_obj_motor2, 50, 1000);
	vTaskDelay(1000);

	//Turn motors back off
	cyhal_gpio_write(MOTOR_OEn, MOTOR_STATE_OFF);
	cyhal_gpio_write(MOTOR1_INA, MOTOR_STATE_OFF);
	cyhal_gpio_write(MOTOR1_INB, MOTOR_STATE_OFF);

}

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
	leds_init();

	/* Create the queues. See the respective data-types for details of queue
	 * contents
	 */
	ble_command_data_q = xQueueCreate(BLE_QUEUE_LENGTH,
			sizeof(ble_command_data_t));
	status_led_data_q = xQueueCreate(STATUS_LED_QUEUE_LENGTH,
			sizeof(status_led_data_t));
	bb8_motor_data_q = xQueueCreate(MOTOR_QUEUE_LENGTH,
			sizeof(bb8_motor_data_t));
	buzzer_data_q = xQueueCreate(BUZZER_QUEUE_LENGTH, sizeof(buzzer_data_t));
	ece453_led_data_q = xQueueCreate(STATUS_LED_QUEUE_LENGTH,
			sizeof(ece453_led_data_t));

	/* Create the user tasks. See the respective Task definition for more
	 * details of these tasks */

	rtos_api_result |= xTaskCreate(task_status_led, TASK_STATUS_LED_NAME,
	TASK_STATUS_LED_STACK_SIZE, NULL,
	TASK_STATUS_LED_PRIORITY, NULL);


	rtos_api_result |= xTaskCreate(task_ble, TASK_BLE_NAME,
	TASK_BLE_STACK_SIZE, NULL,
	TASK_BLE_PRIORITY, NULL);

	rtos_api_result |= xTaskCreate(task_buzzer, "BUZZER TASK", 256,
	NULL, TASK_BUZZER_PRIORITY,
	NULL);

	rtos_api_result |= xTaskCreate(task_ece453_led, "ECE453 LED TASK", 256,
	NULL, TASK_LEDS_PRIORITY,
	NULL);

	rtos_api_result |= xTaskCreate(motor_task, "MOTOR TASK", 256,
	NULL, TASK_MOTOR_PRIORITY,
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

	/* Initial Hardware Test */
	//inital_hardware_test();

	/*So we can keep spinning while the tasks do their thing*/
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
