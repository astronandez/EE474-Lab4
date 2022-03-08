#include <Arduino_FreeRTOS.h>

// Defines for Task RT1
#define LED_EXTERN_PIN      47
#define LED_REG_BIT         0X04
#define LED_ON_TIME         100
#define LED_OFF_TIME        200

void TaskRT1(void *p);

void setup() {
  DDRL |= LED_REG_BIT;
  
  xTaskCreate(
    TaskRT1
    ,   "Blink"
    ,   128
    ,   NULL
    ,   2
    ,   NULL );

  vTaskStartScheduler();
}

void loop() {

}

void TaskRT1(void *p){
  for(;;){
    digitalWrite(LED_EXTERN_PIN, HIGH);
    vTaskDelay(LED_ON_TIME / portTICK_PERIOD_MS);
    digitalWrite(LED_EXTERN_PIN, LOW);
    vTaskDelay(LED_OFF_TIME / portTICK_PERIOD_MS);
  }
}
