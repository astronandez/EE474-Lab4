#include <arduinoFFT.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

// Defines for Task RT3
#define LOOP_COUNT    5
arduinoFFT FFT = arduinoFFT();

const uint16_t samples = 64;
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
    TaskRT3p1
    ,   "RT3p1"
    ,   200
    ,   NULL
    ,   3
    ,   &xRT3p1Handle );

  xTaskCreate(
    TaskRT4
    ,   "RT4"
    ,   1000
    ,   NULL
    ,   2
    ,   &xRT4Handle );
  vTaskStartScheduler();
}

void loop() {
  
}

void TaskRT3p0(void *p){
  for(uint16_t i = 0; i < samples; i++){
    real[i] = random(samples * 5);
    imag[i] = 0;
  }
  fft_queue = xQueueCreate(samples, sizeof(double));
  xQueueSend(fft_queue, (void *)&real, (TickType_t) 10);
  vTaskDelete(xRT3p0Handle);
}

void TaskRT3p1(void *p){
  static int i = 0;
  timer_queue = xQueueCreate(LOOP_COUNT, sizeof(unsigned long));
  while(i < 5){
    xQueueSendToBack(fft_queue, (void *)&real, (TickType_t) 10);
    unsigned long *elapsedTime;
    while(xQueueReceive(timer_queue, &(elapsedTime), (TickType_t) 10 != pdPASS));
    i++;
    Serial.println(*elapsedTime); 
  }
}

void TaskRT4(void *p){
  unsigned long startTime = xTaskGetTickCount();
  xQueueReceive(fft_queue, &(copyData), (TickType_t) 10);

//  FFT.Windowing(copyData, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
//  FFT.Compute(copyData, imag, samples, FFT_FORWARD);
//  FFT.ComplexToMagnitude(copyData, imag, samples);
//  double x = FFT.MajorPeak(copyData, samples, samplingFreq);
  
  unsigned long stopTime = xTaskGetTickCount() - startTime;
  xQueueSend(timer_queue, (void *) &stopTime, (TickType_t) 10);
}
