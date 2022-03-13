#include <Stepper.h>
#include <Arduino_FreeRTOS.h>
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
Stepper motor (STEPS, IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);
long step_delay;
long last_step_time;
int xAnalog;
int yAnalog;
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

  xTaskCreate(
    TaskFollowX,
    "Follow X Axis",
    128,
    NULL,
    2,
    NULL
  );

  // xTaskCreate(
  //   TaskServoY,
  //   "Follow Y Axis",
  //   128,
  //   NULL,
  //   2,
  //   NULL
  // );

  vTaskStartScheduler();

}

void loop() {
  delay(1);
}
void TaskFollowX(){
  for(;;){
    int val = analogRead(A0);
    if(val > 2*496){
      val = 2*496;
    } else if(val < 10){
      val = 0;
    }
    val = val - 496;

  if(val - previous > 8 || val - previous < -8){
    motor.step(val - previous);
    previous = val;
  }
    

    // previous = val;
    vTaskDelay(step_delay/ portTICK_PERIOD_MS);//Delay until next
  }
}

// void TaskServoY(){
//   for(;;){
//     int destVal = analogRead(A1) / 2 + 94;

//     OCR1A = destVal;

//     vTaskDelay(10/portTICK_PERIOD_MS);
//   }
// }