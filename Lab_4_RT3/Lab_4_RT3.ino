#include <arduinoFFT.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

// Defines for Task RT3
#define LOOP_COUNT    5
arduinoFFT FFT = arduinoFFT();

const uint16_t samples = 128;
const double signalFreq = 1000;
const double samplingFreq = 5000;
const uint8_t amp = 100;
double cycles = (((samples-1) * signalFreq) / samplingFreq);

void TaskRT3p0(void *p);
void TaskRT3p1(void *p);
void TaskRT4(void *p);

double real[samples];
double imag[samples];
double copyData[samples];

static QueueHandle_t fft_queue;
static QueueHandle_t timer_queue;
UBaseType_t uxHighWaterMark;

TaskHandle_t xRT3p0Handle;
TaskHandle_t xRT3p1Handle;
TaskHandle_t xRT4Handle;

void setup() {
  Serial.begin(19200);
  while(!Serial);
  
  xTaskCreate(
    TaskRT3p0
    ,   "RT3p0"
    ,   128
    ,   NULL
    ,   3
    ,   &xRT3p0Handle );

  xTaskCreate(
    TaskRT4
    ,   "RT4"
    ,   200
    ,   NULL
    ,   3
    ,   &xRT4Handle );

  vTaskSuspend(xRT4Handle);
  vTaskStartScheduler();
}

void loop() {
  
}

void TaskRT3p0(void *p){
  for(uint16_t i = 0; i < samples; i++){
    real[i] = random(samples * 5);
    imag[i] = 0;
  }
  
  fft_queue = xQueueCreate(2, sizeof(double));
  
  xTaskCreate(
    TaskRT3p1
    ,   "RT3p1"
    ,   450
    ,   NULL
    ,   2
    ,   &xRT3p1Handle );

  vTaskResume(xRT4Handle);
  vTaskSuspend(xRT3p0Handle);
}

void TaskRT3p1(void *p){
  static int i = 0;
  unsigned long elapsedTime;
  timer_queue = xQueueCreate(2, sizeof(unsigned long));

  for(;;){
    for(int i = 0; i < 5; i++){
      xQueueSendToBack(fft_queue, (void *)real, 100);
      xQueueReceive(timer_queue, &(elapsedTime), 100);
    }
  }
}

void TaskRT4(void *p){
  for(;;){
    xQueueReceive(fft_queue, (void *)real, 100);
    for (uint16_t i = 0; i < samples; i++)
    {
      real[i] = int8_t((amp * (sin((i * (twoPi * cycles)) / samples))) / 2.0);/* Build data with positive and negative values*/
      imag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }
    
    int start = millis();
    for(int i = 0; i < 5; i++){
      FFT.Compute(real, imag, samples, FFT_FORWARD);
    }
    int finished = millis() - start;
    Serial.println(finished);
    xQueueSendToBack(timer_queue, (void *) &finished, 100);
  }
  

}
