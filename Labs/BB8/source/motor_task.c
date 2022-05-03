/******************************************************************************
 * File Name: motor_task.c
 *
 * Description: This file contains the task awaiting user input for motor direction
 *
 * Authors: Emily Cebasek, Mya Schmitz, Michael Sexton, Anna Stephan
 *******************************************************************************/


/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "motor_task.h"
#include "uart_debug.h"
#include "queue.h"
#include "cyhal_hw_types.h"
#include "led_task.h"

#define uxTopUsedPriority = configMAX_PRIORITIES-1

#define MAX_SPEED 20 //80% is 'speed on'. if not touching

/* Queue handle used for commands */
QueueHandle_t bb8_motor_data_q;

// current status of INA for motors - gpio ... 1 = forward, 0 = backwards
int INA1 = 0;
int INA2 = 0;

// SPEEDS = PWM (for INB1 and INB2)
int curLeftSpeed = 0;
int curRightSpeed = 0;


void gpio_motor_init(void) {


	/* MOTOR 1 -- IN1A (GPIO) + IN1B (PWM) */
	cyhal_gpio_init(
			MOTOR1_INA,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue


	cyhal_gpio_init(
			MOTOR1_ENCODER_A,                // Pin
			CYHAL_GPIO_DIR_INPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue

	cyhal_gpio_init(
			MOTOR1_ENCODER_B,                // Pin
			CYHAL_GPIO_DIR_INPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);

	/* MOTOR 2 -- IN1A (GPIO) + IN1B (PWM) */
	cyhal_gpio_init(
			MOTOR2_INA,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue


	cyhal_gpio_init(
	MOTOR2_ENCODER_A,                // Pin
			CYHAL_GPIO_DIR_INPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);				        // InitialValue

	cyhal_gpio_init(
	MOTOR2_ENCODER_B,                // Pin
			CYHAL_GPIO_DIR_INPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);

	/* MOTOR OE Enable */
	cyhal_gpio_init(
	MOTOR_OEn,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			false);
}

void motor_task(void *param) {

	cyhal_pwm_t pwm_obj_motor1;
	cyhal_pwm_t pwm_obj_motor2;
	cy_rslt_t rslt;

	/* Remove warning for unused parameter */
	(void) param;

	task_print_info("MOTOR_TASK: BEGAN TASK");

	/* Initialize Motors - each has one GPIO (As) and one PWM (Bs) */


			gpio_motor_init();

			cyhal_gpio_write(MOTOR_OEn, 0);
			cyhal_gpio_write(MOTOR1_INA, 0);

			cyhal_gpio_write(MOTOR2_INA, 0);
			task_print_info("MOTOR_TASK: INIT");


			/* Write to motor 1 and motor 2 PWM 100% */
			rslt = cyhal_pwm_init(&pwm_obj_motor1, MOTOR1_INB, NULL);
			rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor1, 99,
						1000);
			rslt = cyhal_pwm_start(&pwm_obj_motor1);

			rslt = cyhal_pwm_init(&pwm_obj_motor2, MOTOR2_INB, NULL);
			rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor2, 99,
						1000);
			rslt = cyhal_pwm_start(&pwm_obj_motor2);


	/* Variable that stores the data received over queue */
	bb8_motor_data_t buffer;

	for (;;) {

		xQueueReceive(bb8_motor_data_q, &buffer, portMAX_DELAY);



		// Can only be one of 4 states
		if (buffer.direction == FORWARD) { // forwards
			INA1 = 1;
			if (curLeftSpeed < MAX_SPEED) {
				curLeftSpeed = curLeftSpeed + 2;
			}

			if (curRightSpeed < MAX_SPEED) {
				curRightSpeed = curRightSpeed + 2;
			}

		} else if (buffer.direction == REVERSE) { // backwards
			INA1 = 0;
			curLeftSpeed = MAX_SPEED;
			curRightSpeed = MAX_SPEED;
		}

		// Are we left or right
		else if (buffer.direction == LEFT) { // purely left
		// slowly decrease left wheel to turn left
			if (curLeftSpeed >= 2) {
				curLeftSpeed -= 2;
			}

			if (curRightSpeed < MAX_SPEED) {
				curRightSpeed += 2;
			}

		} else if (buffer.direction == RIGHT) { // purely right
		// slowly decrease right wheel to turn right
			if (curRightSpeed >= 2) {
				curRightSpeed -= 2;
			}

			if (curLeftSpeed < MAX_SPEED) {
				curLeftSpeed += 2;
			}

		}
		else{
			// prevent speed from going negative
			if (curLeftSpeed >= 2) {
				curLeftSpeed -= 2;
			}

			if (curRightSpeed >= 2) {
				curRightSpeed -= 2;
			}
		}

		//HAL put GPIO and PWM signals out

		/* Set a duty cycle of 50% and frequency of 1Hz */

//		int updatedLeftSpeed = (INA1) ? curLeftSpeed : 100 - curLeftSpeed;
//		int updatedRightSpeed = (INA2) ? 100 - curRightSpeed : curRightSpeed;
//
//		rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor1, updatedLeftSpeed, 1000);
//		rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_motor2, updatedRightSpeed, 1000);
//
//		cyhal_gpio_write(MOTOR1_INA, INA1);
//		cyhal_gpio_write(MOTOR2_INA, ~INA2);

	}

}

