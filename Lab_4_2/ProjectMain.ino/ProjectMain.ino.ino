#include <Stepper.h>
#include <Arduino_FreeRTOS.h>
#include <stdlib.h>


#define IN1_PIN       22
#define IN2_PIN       23
#define IN3_PIN       24
#define IN4_PIN       25

#define STEPS         32
#define STEPSPERREV   2048


void TaskSTP1();
void readAnalog();
// void TaskManager();
// Stepper motor (STEPS, IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);
long step_delay;
long last_step_time;
int xAnalog;
int yAnalog;
int stepsToTake;
int destination_pos;
int curr_pos = 0;
int thisStep;

void setup() {
  DDRA |= 0x0F;//Enables write for all stepper pins

  long step_delay = 60L * 1000L * 1000L / STEPS / 200;

  Serial.begin(9600);

  // motor.setSpeed(200);
  //Start basic step tasks task with RTOS
  // xTaskCreate(
  //   TaskManager,
  //   "Basic Manager",
  //   128,
  //   NULL,
  //   2,
  //   NULL
  // );

  xTaskCreate(
    readAnalog,
    "ReadAnalogs",
    128,
    NULL,
    2,
    NULL
  );
  
  xTaskCreate(
    TaskSTP1
    ,   "FewSteps"
    ,   128
    ,   NULL
    ,   2
    ,   NULL );

  vTaskStartScheduler();

}

void loop() {
  // put your main code here, to run repeatedly:
  // if(millis() < 5) {
  //   motor.step(2048);
  //   // delay(10);
  //   // x++;
  // }
  delay(1);
}

void readAnalog(){
  for(;;){
    destination_pos = analogRead(A0) - 496;
    // Serial.print("Dest: ");
    // Serial.println(destination_pos);
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
}

void TaskSTP1(){
  static int stepNumber = 0;
  int stepsLeft = abs(destination_pos) - abs(curr_pos);

  int dir;
  if(destination_pos - curr_pos >= 0) {dir =  1;}
  if(destination_pos - curr_pos < 0) {dir = -1;}

  while (stepsLeft > 0){
    if(dir == 1){
      stepNumber ++;
      curr_pos ++;
      if(stepNumber == 4){
        stepNumber = 0;//Only between 0 and 3 for step number
      }
    } else {
      if(stepNumber == 0){
        stepNumber = 4;
      }
      curr_pos --;
      stepNumber --;
    }
    stepsLeft --;
    switch (stepNumber) {
      case 0:  // 1010
          PORTA = 0b00000101;      
      break;
      case 1:  // 0110
        PORTA = 0b00000110;
      break;
      case 2:  //0101
          PORTA = 0b00001010;
      break;
      case 3:  //1001
        PORTA = 0b00001001;
      break;
    }
    // Serial.print("Curr: ");
    // Serial.println(curr_pos);
    vTaskDelay(100*step_delay / portTICK_PERIOD_MS);//Delay until next 
  }
}
