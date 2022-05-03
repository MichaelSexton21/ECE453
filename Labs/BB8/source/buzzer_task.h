#ifndef BUZZER_H__
#define BUZZER_H__

#include "cybsp.h"
#include "cyhal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "uart_debug.h"
#include "cy_pdl.h"


extern cyhal_pwm_t pwm_obj_buzzer;

// Buzzer definition main board
#define PIN_BUZZER 		P9_3 // tcpwm0_5 / tcpwm1_21

typedef enum
{
	BUZZER
} buzzer_t;

/* Data-type used for the control of Red and Orange status LEDs  */
typedef struct
{
	buzzer_t buzzer;
	uint8_t pwm;            // Duty cycle of the LED
}   buzzer_data_t;


/*******************************************************************************
 * Extern variable
 ******************************************************************************/
/* Handle for the Queue that contains Status LED data */
extern QueueHandle_t buzzer_data_q;

void task_buzzer(void *param);

#endif
