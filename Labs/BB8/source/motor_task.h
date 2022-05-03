#ifndef MOTOR_TASK_H__
#define MOTOR_TASK_H__

#include "FreeRTOS.h"
#include "queue.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

extern cyhal_pwm_t pwm_obj_motor1;
extern cyhal_pwm_t pwm_obj_motor2;

/** Pin state for the motor on. */
#ifndef MOTOR_STATE_ON
#define MOTOR_STATE_ON          (0U)
#endif

/** Pin state for the motor off. */
#ifndef MOTOR_STATE_OFF
#define MOTOR_STATE_OFF          (1U)
#endif

//Encoder and two motor input streams
#define MOTOR1_ENCODER_A P5_5
#define MOTOR1_ENCODER_B P5_6
#define MOTOR2_ENCODER_A P10_0
#define MOTOR2_ENCODER_B P10_1

#define MOTOR1_INA P10_2
#define MOTOR1_INB P10_3 // tcpwm0_7 / tcpwm1_23

#define MOTOR2_INA P10_4
#define MOTOR2_INB P10_5 // tcpwm0_0 / tcpwm1_0

#define MOTOR_OEn P10_6
/*******************************************************************************
 * Structure and enumeration
 ******************************************************************************/

/* 1 if user is clicking such dir, 0 if they are not (at time of sample)  */
typedef enum
{
	FORWARD, REVERSE, LEFT, RIGHT, ENCODER1, ENCODER2
}   bb8_motor_t;

typedef struct {
	uint8_t direction;
} bb8_motor_data_t;

/*******************************************************************************
 * Extern variable
 ******************************************************************************/
/* Handle for the Queue that contains Status LED data */
extern QueueHandle_t bb8_motor_data_q;

void gpio_motor_init(void);

/*******************************************************************************
 * Function prototype
 ******************************************************************************/
void motor_task(void *param);



/* [] END OF FILE */
#endif
