/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "bb8_motor_task.h"
#include "queue.h"
#include "cyhal_hw_types.h"
#include "motor.h"

#define MAX_SPEED 20 //20% is 'speed on'

/* Queue handle used for commands */
QueueHandle_t bb8_motor_data_q;

// current status of INA for motors - gpio ... 1 = forward, 0 = backwards
int INA1 = 1;
int INA2 = 1;

// SPEEDS = PWM (for INB1 and INB2)
int curLeftSpeed = 0;
int curRightSpeed = 0;

void bb8_motor_task(void *param) {

	cy_rslt_t rslt;

	/* Variable that stores the data received over queue */
	bb8_motor_data_t bb8_motor_data;

	/* Remove warning for unused parameter */
	(void) param;

	cyhal_pwm_t pwm_obj_motor1;
	/* Initialize PWM on the supplied pin and assign a new clock */
	rslt = cyhal_pwm_init(&pwm_obj_motor1, MOTOR1_INB, NULL);
	/* Set a duty cycle of 50% and frequency of 1Hz */
	rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor1, 100, 1000);
	/* Start the PWM output */
	rslt = cyhal_pwm_start(&pwm_obj_motor1);

	cyhal_pwm_t pwm_obj_motor2;
	/* Initialize PWM on the supplied pin and assign a new clock */
	rslt = cyhal_pwm_init(&pwm_obj_motor2, MOTOR2_INB, NULL);
	/* Set a duty cycle of 50% and frequency of 1Hz */
	rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor2, 100, 1000);
	/* Start the PWM output */
	rslt = cyhal_pwm_start(&pwm_obj_motor2);

	// Start assuming forward (in case of initial turns)
	bb8_motor_data_t buffer;

	for (;;) {
		/* Block until a command has been received from the BLE task.
		 * The data sent should be recieved on the FreeRTOS queue that
		 * is defined in ece453_led_task.h.  Be sure to examine the
		 * ece453_led_data_t struct when determining what data will be
		 * received from the BLE task.*/

		// if user is not pressing any controls, then gradually slow the robot down
		// rather than abruptly stopping the wheels
		if (buffer.up == 0 && buffer.down == 0 && buffer.left == 0
				&& buffer.right == 0) {
			// prevent speed from going negative
			if (curLeftSpeed >= 2) {
				curLeftSpeed -= 2;
			}

			if (curRightSpeed >= 2) {
				curRightSpeed -= 2;
			}
		}

		xQueueReceive(bb8_motor_data_q, &buffer, portMAX_DELAY);

		// Are we back or forward
		if (buffer.up == 1 && buffer.down == 0) { // forwards
			INA1 = 1;
			INA2 = 1;
			if(curLeftSpeed < MAX_SPEED){
				curLeftSpeed = curLeftSpeed + 2;
			}

			if(curRightSpeed < MAX_SPEED){
				curRightSpeed = curRightSpeed + 2;
			}

		} else if (buffer.down == 1 && buffer.up == 0) { // backwards
			INA1 = 0;
			INA2 = 0;
			curLeftSpeed = MAX_SPEED;
			curRightSpeed = MAX_SPEED;
		}

		// Are we left or right
		if (buffer.left == 1 && buffer.right == 0) { // purely left
		// slowly decrease left wheel to turn left
			if (curLeftSpeed >= 2) {
				curLeftSpeed -= 2;
			}

			if(curRightSpeed < MAX_SPEED){
				curRightSpeed += 2;
			}

		} else if (buffer.right == 1 && buffer.left == 0) { // purely right
		// slowly decrease right wheel to turn right
			if (curRightSpeed >= 2) {
				curRightSpeed -= 2;
			}

			if(curLeftSpeed < MAX_SPEED){
				curLeftSpeed += 2;
			}

		}

		//HAL put GPIO and PWM signals out

		/* Set a duty cycle of 50% and frequency of 1Hz */

		int updatedLeftSpeed = (INA1) ? curLeftSpeed : 100 - curLeftSpeed;
		int updatedRightSpeed = (INA2) ? 100 - curRightSpeed : curRightSpeed;


		rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor1, updatedLeftSpeed, 1000);
		rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor2, updatedRightSpeed, 1000);

		cyhal_gpio_write(MOTOR1_INA, INA1);
		cyhal_gpio_write(MOTOR2_INA, ~INA2);


		/* Cases:
		 * UP ONLY: L and R FORWARDS
		 * DOWN ONLY: L and R BACKWARDS
		 * LEFT ONLY: R ON, L OFF
		 * RIGHT ONLY: R OFF, L ON
		 * UP/LEFT:
		 * DOWN/LEFT:
		 * UP/RIGHT:
		 * DOWN/RIGHT:
		 * UP/DOWN: NOTHING
		 * LEFT/RIGHT: NOTHING
		 */

	}
}
