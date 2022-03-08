#include <arduinoFFT.h>
#include <Arduino_FreeRTOS.h>

// Defines for Task RT3


void TaskRT3(void *p);

void setup() {
  
  xTaskCreate(
    TaskRT3
    ,   "FFT"
    ,   128
    ,   NULL
    ,   1
    ,   NULL );

  vTaskStartScheduler();
}

void loop() {

}
