#include <arduinoFFT.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

// Defines for Task RT1
#define LED_EXTERN_PIN      47
#define LED_REG_BIT         0X04
#define LED_ON_TIME         100
#define LED_OFF_TIME        200
// Defines for Task RT2
#define CLOCK_RATE          16000000
#define NOTE_PERIOD         150
#define SPKR_OFF            1500
#define OC4A_PIN            DDH3
// Defines for Task RT3
#define LOOP_COUNT          5
#define SAMPLES             128
#define SIG_FREQ            1000
#define SAM_FREQ            5000
#define AMPLITUDE           100

// Function prototypes
void TaskRT1(void *p);
void TaskRT2(void *p);
void TaskRT3p0(void *p);
void TaskRT3p1(void *p);
void TaskRT4(void *p);
int freqConv (int);

int melody[] = {293, 0, 329, 0, 261, 0, 130, 0, 196, 0};//Initial music array with values in Hz not converted yet
double cycles = (((SAMPLES-1) * SIG_FREQ) / SAM_FREQ);
double real[SAMPLES];
double imag[SAMPLES];

arduinoFFT FFT = arduinoFFT();
static QueueHandle_t fft_queue;
static QueueHandle_t timer_queue;
TaskHandle_t xRT3p0Handle;
TaskHandle_t xRT3p1Handle;
TaskHandle_t xRT4Handle;


void setup() {
  Serial.begin(19200);
  while(!Serial);
  
   // Initialize timer 4 register A & B to 0 to clear values
  TCCR4A = 0; 
  TCCR4B = 0;
  // Stetting registers to proper settings for Part 2
  TCCR4A |= 1 << COM4A0;
  TCCR4B |= 1 << WGM42;
  TCCR4B |= 1 << CS40; //16MHz clock
  OCR4A = 0; // Initially 0 
  
  DDRL |= LED_REG_BIT;
  DDRH |= 1 << OC4A_PIN;
  
  xTaskCreate(
  TaskRT1
  ,   "Blink"
  ,   128
  ,   NULL
  ,   2
  ,   NULL );

  xTaskCreate(
  TaskRT2
  ,   "CloseEncounters"
  ,   128
  ,   NULL
  ,   3
  ,   NULL );
    
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

void TaskRT2(void *p){
  static int melodyIndex = 0;
  for(int j = 0; j < 3; j ++){
    while(melodyIndex < sizeof(melody) / sizeof(int)){
      OCR4A = freqConv(melody[melodyIndex]);
      melodyIndex++;
      vTaskDelay(NOTE_PERIOD / portTICK_PERIOD_MS);
    }
    melodyIndex = 0;
    OCR4A = 0;
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void TaskRT3p0(void *p){
  for(uint16_t i = 0; i < SAMPLES; i++){
    real[i] = random(SAMPLES * 5);
    imag[i] = 0;
  }
  
  fft_queue = xQueueCreate(2, sizeof(double));
  
  xTaskCreate(
    TaskRT3p1
    ,   "RT3p1"
    ,   128
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
    for (uint16_t i = 0; i < SAMPLES; i++)
    {
      real[i] = int8_t((AMPLITUDE * (sin((i * (twoPi * cycles)) / SAMPLES))) / 2.0);/* Build data with positive and negative values*/
      imag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }
    
    int start = millis();
    for(int i = 0; i < 5; i++){
      FFT.Compute(real, imag, SAMPLES, FFT_FORWARD);
    }
    int finished = millis() - start;
    Serial.println(finished);
    xQueueSendToBack(timer_queue, (void *) &finished, 100);
  }
}

/**
 * @brief Returns the COMP value that will create the input frequency
 * 
 * This assumes a clock of 16MHz and a prescaler of 1.
 * 
 * @param inputFreq Desired frequency of oscilation for the counter
 * @return int Output COMP value to assign to the counter's compare register
 */
int freqConv (int inputFreq){
  float divdend = 2 * 1 * inputFreq;
  float divsor = CLOCK_RATE;
  float result = divsor / divdend;
  return (int) (result - 1.0);
}
