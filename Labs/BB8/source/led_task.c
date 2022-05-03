/******************************************************************************
 * File Name: led_task.c
 *
 * Description: This file contains the task that that controls the RGB LEDs
 * on the respective board (via Ethernet connection)
 *
 * Authors: Emily Cebasek, Mya Schmitz, Michael Sexton, Anna Stephan
 *******************************************************************************/

/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "led_task.h"
#include "ble_task.h"
#include "uart_debug.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Queue handle used for commands to Task_StatusLed */
QueueHandle_t ece453_led_data_q;

/*******************************************************************************
 * Function prototype
 ******************************************************************************/

void cycle(int delay, int cycles) {
	const TickType_t xDelay = delay * 1000 / portTICK_PERIOD_MS; // 1000=1second
	int i = 0;
	cycles = cycles * 5;
	for (int i = 0; i < cycles; i++) {
		vTaskDelay(xDelay);
		if (i % 4 == 0) {
			cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_ON);
			cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_OFF);
			cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_OFF);
		} else if (i % 4 == 1) {
			cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_OFF);
			cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_ON);
			cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_OFF);
		} else if (i % 4 == 2) {
			cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_OFF);
			cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_OFF);
			cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_ON);
		} else if (i % 4 == 3) {
			cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_ON);
			cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_ON);
			cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_ON);
		}
	}
}

void leds_init(void) {
	/* Configure RED LED*/
	cyhal_gpio_init(
	PIN_LED_RED,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue

	/* Configure the GREEN LED as an output */
	cyhal_gpio_init(
	PIN_LED_GREEN,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue

	/* Configure the BLUE LED as an output */
	cyhal_gpio_init(
	PIN_LED_BLUE,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue
}

/*******************************************************************************
 * Function Name: vtask_ece453_led
 ********************************************************************************
 * Summary:
 *  Task that controls the status LED.
 *
 * Parameters:
 *  void *param : Task parameter defined during task creation (unused)
 *
 *******************************************************************************/
void task_ece453_led(void *param) {
	/* Variable that stores the data received over queue */
	ece453_led_data_t ece453_led_data;

	/* Variable used to store the return values of RTOS APIs */
	BaseType_t rtos_api_result;

	/* Remove warning for unused parameter */
	(void) param;

	ece453_led_data_t buffer;
	task_print_info("LED_TASK: BEGAN TASK");
	int cycles = 0;
	/* Repeatedly running part of the task */
	int RED_STATUS = 0;
	int BLUE_STATUS = 0;
	int GREEN_STATUS = 0;
	for (;;) {

		task_print_info("LED_TASK: BEGAN LOOP");

    	xQueueReceive(ece453_led_data_q, &buffer, portMAX_DELAY);

    	if(buffer.ece_led == ECE453_LED_RED){
    		RED_STATUS = !RED_STATUS;
    		if(RED_STATUS==0){
    			cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_OFF);
    		}else{
    			cyhal_gpio_write(PIN_LED_RED, CYBSP_LED_STATE_ON);
    		}
    	}
    		else if( buffer.ece_led == ECE453_LED_GREEN){
    		GREEN_STATUS = !GREEN_STATUS;
    		if(GREEN_STATUS==0){
    			cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_OFF);
    		}else{
    			cyhal_gpio_write(PIN_LED_GREEN, CYBSP_LED_STATE_ON);
    		}
    	}
    		else if( buffer.ece_led == ECE453_LED_BLUE){
    		BLUE_STATUS = !BLUE_STATUS;
    		if(BLUE_STATUS==0){
    		    cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_OFF);
    		}else{
    			cyhal_gpio_write(PIN_LED_BLUE, CYBSP_LED_STATE_ON);
    		}

    	}

	}
}


/* [] END OF FILE */

