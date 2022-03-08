#include <Arduino_FreeRTOS.h>
// Defines for Task RT2
#define CLOCK_RATE  16000000
#define NOTE_PERIOD 100
#define SPKR_OFF    1500



int freqConv (int);
void TaskRT2(void *p);

int melody[] = {293, 0, 329, 0, 261, 0, 130, 0, 196, 0};//Initial music array with values in Hz not converted yet

void setup() {
  // Initialize timer 4 register A & B to 0 to clear values
  TCCR4A = 0; 
  TCCR4B = 0;

  // Stetting registers to proper settings for Part 2
  TCCR4A |= 1 << COM4A0;
  TCCR4B |= 1 << WGM42;
  TCCR4B |= 1 << CS40; //16MHz clock
  OCR4A = 0; // Initially 0 
 
  xTaskCreate(
    TaskRT2
    ,   "CloseEncounters"
    ,   128
    ,   NULL
    ,   1
    ,   NULL );
  vTaskStartScheduler();
}

void loop() {

}

void TaskRT2(void *p){
  static int melodyIndex = 0;
  for(;;){
    while(melodyIndex < sizeof(melody) / sizeof(int)){
      OCR4A = freqConv(melody[melodyIndex]);
      melodyIndex++;
      vTaskDelay(NOTE_PERIOD / portTICK_PERIOD_MS);
    }
    melodyIndex = 0;
    OCR4A = 0;
    vTaskDelay(1500 / portTICK_PERIOD_MS);
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