#include <buzzer_task.h>
#include "uart_debug.h"

/*****************************************************
 * Function Name: buzzer_init
 ******************************************************
 * Summary:
 *
 * Initializes the IO pins used to control the Buzzer
 *
 *****************************************************/
/* Queue handle used for commands to Task_StatusLed */
QueueHandle_t buzzer_data_q;

void task_buzzer(void *param) {

#if 1
	task_print_info("BUZZER_TASK: BEGAN TASK");
	vTaskDelay(1000);
	cy_rslt_t rslt;
	cyhal_pwm_t pwm_obj_buzzer;

	(void) param;

	BaseType_t rtos_api_result;

	int BUZZER_DUTY_CYCLE = 50;
	int BUZZER_FREQUENCY = 2000;
	rslt = cyhal_pwm_init(&pwm_obj_buzzer, PIN_BUZZER, NULL);
	rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_buzzer, BUZZER_DUTY_CYCLE,
			BUZZER_FREQUENCY);
	rslt = cyhal_pwm_start(&pwm_obj_buzzer);

	buzzer_data_t buffer;

	int BUZZER_STATUS = 0;
	for (;;) {
		xQueueReceive(buzzer_data_q, &buffer, portMAX_DELAY);
		BUZZER_STATUS = !BUZZER_STATUS;

		if(BUZZER_STATUS==0){
			rslt = cyhal_pwm_start(&pwm_obj_buzzer);
		}else{
			rslt = cyhal_pwm_stop(&pwm_obj_buzzer);
		}

	}
#else
	for (;;) {
		task_print_info("BUZZER_TASK: BEGAN LOOP");
		vTaskDelay(1000);
	}
#endif

}

