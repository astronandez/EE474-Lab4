/*ProjectMain.cpp
 * @file   Lab_4_Final.cpp
 *   @author    Christian Gordon, Marc Hernandez
 *   @date      15-Mar-2022
 *   @brief     Lab 4 RTOS Project
 *   
 *  This code contains the methods and functions needed for Lab 4
 *  during Winter 2022 EE 474
 */
#include <Stepper.h>
#include <Arduino_FreeRTOS.h>
#include <stdlib.h>
#include "IRremote.h"

//Defines for stepper motor pins
#define IN1_PIN       22
#define IN2_PIN       23
#define IN3_PIN       24
#define IN4_PIN       25
//Stepper motor constants
#define STEPS         32
#define STEPSPERREV   2048


//Task ProtoTypes
void TaskFollowX();
void TaskServoY();
void TaskReadJoystick();
void DecodeIRSig();
//Creates stepper motor instance using the pins for our system
Stepper motor (STEPS, IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);
long step_delay;//How long each step takes
int xVal;//Destination x value
int yVal;//Destination y value
int brightness = 1;//Initial brightness
int previous = 0;//Previous location of the stepper motor

//IR Remote objects
IRrecv irrecv(8);//On pin 8
decode_results IRmessage;

/**
 * @brief Initial code run before beginning the scheduler
 * 
 * Sets up internal Registers to have expected behavior and creates the tasks needed for RT1-4,
 * and initializes the clock.
 * 
 */
void setup() {
  DDRA |= 0x0F;//Enables write for all stepper pins

  //Set up Timer 1 for PWM signals for both The Servo and the Laser brightness
  TCCR1A |= 0b01110010;
  TCCR1B |= 0b00011011;

  ICR1 = 4999;//Gives 50Hz PWM

  DDRB |= 1<<DDB5;//Output pin is 11 on the arduino servo
  DDRB |= 1<<DDB6;//Output on pin 12 on the arduino laser


  long step_delay = 60L * 1000L * 1000L / STEPS / 200;

  Serial.begin(9600);

  motor.setSpeed(1000);

  int xVal = 496;
  int yVal = 496;

  irrecv.enableIRIn();

  xTaskCreate(
    TaskReadJoystick,
    "Read Joystick",
    128,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskFollowX,
    "Follow X Axis",
    128,
    NULL,
    2,
    NULL
  );

  xTaskCreate(
    TaskServoY,
    "Follow Y Axis",
    128,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    DecodeIRSig,
    "Decode IR Signal",
    128,
    NULL,
    2,
    NULL
  );

  Serial.println("Start of Scheduler");
//  delay(500);
  vTaskStartScheduler();

}

void loop() {
  Serial.println("Ooops something is wrong, you shouldn't be here");
  delay(100);
}

/**
 * @brief Reads Position of Joystick
 * 
 * Reads the position of the Joystick and converts the x value into a value between 0 and 992 for the stepper motor to go to
 * Also updates the y value to the respective servo position from 0 to 180 degrees
 */
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

/**
 * @brief Moves the Stepper motor to the desired position
 * 
 * Keeps track of the current location of the stepper motor and moves it towards the destination set by the joystick
 * 
 */
void TaskFollowX(){
  for(;;){
    if(xVal - previous > 8 || xVal - previous < -8){
      motor.step(xVal - previous);
      previous = xVal;
    }
    vTaskDelay(300 / portTICK_PERIOD_MS);//Delay until next
  }
}

/**
 * @brief Sets the position of the servo
 * 
 * Generates the correct output compare value that will generate the correct PWM signal for the servo to 
 * reach the desired location.
 */
void TaskServoY(){
  for(;;){
    // Serial.println("Moving Servo");
    OCR1A = yVal;
    vTaskDelay(15/portTICK_PERIOD_MS);
  }
}


/**
 * @brief When a correct IR signal is recieved update the brightness of the laser
 * 
 * When there is a message recieved, update the OCR1B register to change the brightness
 */
void DecodeIRSig(){
  for(;;){
    // Serial.println("Start of Decode");
    if (irrecv.decode(&IRmessage)) // have we received an IR signal
    {
      translateIR(); 
//      Serial.print("Updating Brightness: to ");
      Serial.println(brightness);
      // OCR1B = yVal;
      OCR1B = 35*brightness;
      irrecv.resume(); // receive the next value
    }  
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

/**
 * @brief Translate IR message and change values based on it
 * 
 * When either up or down buttons are pressed, it will update the brightness value based on the button pressed
 */
void translateIR() {
  // Serial.println("Start of translate");
  switch(IRmessage.value)
  {
  case 0xFF906F:
    if(brightness > 1){
      brightness --;
    }
    break;
  case 0xFFE01F:
    if(brightness < 10){
      brightness ++;
    }
    break;
  default: 
    break;
  }// End Case
} 
