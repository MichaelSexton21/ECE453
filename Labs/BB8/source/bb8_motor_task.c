/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "bb8_motor_task.h"
#include "lvhb.h"

#define MAX_SPEED 20 //20% is 'speed on'

/* Queue handle used for commands */
QueueHandle_t bb8_motor_data_q;

void task_bb8_motor(void *param){

	cy_rslt_t rslt;

  /* Variable that stores the data received over queue */
  bb8_motor_data_t bb8_motor_data;

  /* Remove warning for unused parameter */
  (void)param ;

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

  //Start assuming forward (in case of inital turns)
	bb8_motor_data_t buffer;
  LVHB_SetDirection(drvConfig, true, lvhbBridge1);
  LVHB_SetDirection(drvConfig, true, lvhbBridge2);

	for(;;)
    {        
        /* Block until a command has been received from the BLE task.
         * The data sent should be recieved on the FreeRTOS queue that
         * is defined in ece453_led_task.h.  Be sure to examine the 
         * ece453_led_data_t struct when determining what data will be
         * received from the BLE task.*/

    	xQueueReceive(ece453_led_data_q, &buffer, portMAX_DELAY);

      int curLeftSpeed = 0;
      int curRightSpeed = 0;

      // Are we back or forward
    	if(buffer.up == 1 && buffer.down == 0){
        LVHB_SetDirection(drvConfig, true, lvhbBridge1);
        LVHB_SetDirection(drvConfig, true, lvhbBridge2);
        curLeftSpeed = MAX_SPEED;
        curRightSpeed = MAX_SPEED;
    	}
    	else if(buffer.down == 1 && buffer.up == 0){
    		LVHB_SetDirection(drvConfig, false, lvhbBridge1);
        LVHB_SetDirection(drvConfig, false, lvhbBridge2);
        curLeftSpeed = MAX_SPEED;
        curRightSpeed = MAX_SPEED;
    	}

      //Are we left or right
    	if(buffer.left == 1 && buffer.right == 0){
        curRightSpeed = curRightSpeed + 20;
    	}
    	else if(buffer.right == 1 && buffer.left == 0){
        curLeftSpeed = curLeftSpeed + 20;
    	}

      LVHB_RotateProportional(drvConfig, curLeftSpeed, lvhbBridge1);
      LVHB_RotateProportional(drvConfig, curRightSpeed, lvhbBridge2);

      /* Cases:
       * UP ONLY: up and down FORWARDS
       * DOWN ONLY: up and down BACKWARDS
       * LEFT ONLY: right wheel ON, left wheel OFF
       * RIGHT ONLY: right wheel OFF, left wheel ON
       * UP/LEFT: 
       * DOWN/LEFT: 
       * UP/RIGHT: 
       * DOWN/RIGHT: 
       * UP/DOWN: NOTHING
       * LEFT/RIGHT: NOTHING
       */ 



        
    }



}