#include <Arduino_FreeRTOS.h>
#include <croutine.h>
#include <event_groups.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <StackMacros.h>
#include <task.h>
#include <timers.h>

#include "task_motor.h"
#include "task_buzzer.h"
#include "task_led.h"
#include <task_led_status.h>

void setup() {
  /* Setup pins as input/output */
  pinMode(LED_STATUS_PIN, OUTPUT);
  pinMode(MOTOR_1A_PIN, OUTPUT);
  pinMode(MOTOR_1B_PIN, OUTPUT);
  pinMode(MOTOR_2A_PIN, OUTPUT);
  pinMode(MOTOR_2B_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  /* Initialize pins */
  digitalWrite(LED_STATUS_PIN, LOW);
  digitalWrite(MOTOR_1A_PIN, LOW);
  digitalWrite(MOTOR_1B_PIN, LOW);
  digitalWrite(MOTOR_2A_PIN, LOW);
  digitalWrite(MOTOR_2B_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, LOW);

  /* Create RTOS Queues */
  
  

  /* Create RTOS Tasks */
//  xTaskCreate(MyTask_pointer, "task_name", 100, Parameter, Priority, TaskHandle);
//  xTaskCreate(MyTask_pointer, "task_name", 100, Parameter, Priority, TaskHandle);
//  xTaskCreate(MyTask_pointer, "task_name", 100, Parameter, Priority, TaskHandle);
//  xTaskCreate(MyTask_pointer, "task_name", 100, Parameter, Priority, TaskHandle);
  xTaskCreate(Task_Status_LED, TASK_STATUS_LED_NAME, 100, NULL, TASK_STATUS_LED_PRIORITY, NULL);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
