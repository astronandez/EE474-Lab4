#include <Arduino_FreeRTOS.h>

// Defines for Task RT1
#define LED_EXTERN_PIN      47
#define LED_REG_BIT         0X04
#define LED_ON_TIME         100
#define LED_OFF_TIME        200

void TaskRT1(void *p);

/**
 * @brief Initial code run before beginning the scheduler
 * 
 * Sets up internal Registers to have expected behavior and initializes some global values used by the system.
 * Additionally, we create our the tasks for our scheduler through this function.
 * 
 */
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

/**
 * @brief Executes Task RT1, LED on for 100 ms and off for 200 ms
 * 
 * This function utilizes void *p to be called when state of the task is
 * ready and there are no tasks of a higher priority in the ready state
 * 
 * @param void *p is a pointer needed by FreeRTOS to call the function
 */
void TaskRT1(void *p){
  for(;;){
    digitalWrite(LED_EXTERN_PIN, HIGH);
    vTaskDelay(LED_ON_TIME / portTICK_PERIOD_MS);
    digitalWrite(LED_EXTERN_PIN, LOW);
    vTaskDelay(LED_OFF_TIME / portTICK_PERIOD_MS);
  }
}
