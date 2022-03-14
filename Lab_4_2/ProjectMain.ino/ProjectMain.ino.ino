#include <Stepper.h>
#include <Arduino_FreeRTOS.h>
// #include <FreeRTOS.h>
// #include "atomic.h"
#include <stdlib.h>


#define IN1_PIN       22
#define IN2_PIN       23
#define IN3_PIN       24
#define IN4_PIN       25

#define STEPS         32
#define STEPSPERREV   2048


// void TaskSTP1();
// void readAnalog();
void TaskFollowX();
void TaskServoY();
void TaskReadJoystick();
Stepper motor (STEPS, IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);
long step_delay;
long last_step_time;
int xVal;
int yVal;
int xPrev;
int yPrev;
int stepsToTake;
int destination_pos;
int curr_pos = 0;
int thisStep;
int previous = 0;

void setup() {
  DDRA |= 0x0F;//Enables write for all stepper pins

  //Servo Set up for PWM
  TCCR1A |= 0b01010010;
  TCCR1B |= 0b00011011;

  ICR1 = 4999;//Gives 50Hz PWM

  DDRB |= 1<<DDB5;//Output pin is 11 on the arduino



  long step_delay = 60L * 1000L * 1000L / STEPS / 200;

  Serial.begin(9600);

  motor.setSpeed(1000);

  int xVal = 496;
  int yVal = 496;

  xTaskCreate(
    TaskReadJoystick,
    "Read Joystick",
    128,
    NULL,
    0,
    NULL
  );

  xTaskCreate(
    TaskFollowX,
    "Follow X Axis",
    128,
    NULL,
    0,
    NULL
  );

  xTaskCreate(
    TaskServoY,
    "Follow Y Axis",
    128,
    NULL,
    0,
    NULL
  );

  Serial.println("Start of Scheduler");
  vTaskStartScheduler();

}

void loop() {
  delay(1);
}

void TaskReadJoystick(){
  for(;;){
    xVal = analogRead(A0);
    if(xVal > 2*496){
      xVal = 2*496;
    } else if(xVal < 10){
      xVal = 0;
    }
    xVal = xVal - 496;
    yVal = analogRead(A1) / 2 + 94;
    vTaskDelay(15/portTICK_PERIOD_MS);
    // Serial.println("Read Joy Stick");
  }
}


void TaskFollowX(){
  for(;;){
    if(xVal - previous > 8 || xVal - previous < -8){
      // taskENTER_CRITICAL();
      // Serial.println("Stepper go burrrrrrrrrrrrrrrrrrrrr");
      motor.step(xVal - previous);
      previous = xVal;
      // taskEXIT_CRITICAL();
    }
    vTaskDelay(250 / portTICK_PERIOD_MS);//Delay until next
  }
}

void TaskServoY(){
  for(;;){
    // Serial.println("Moving Servo");
    OCR1A = yVal;
    vTaskDelay(5/portTICK_PERIOD_MS);
  }
}