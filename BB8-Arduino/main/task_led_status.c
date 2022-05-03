#include <Arduino.h>
#include <task_led_status.h>

void Task_Status_LED(void* param) {
  digitalWrite(LED_STATUS_PIN,HIGH);
  
  while(1) {
  }
}
