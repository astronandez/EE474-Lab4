#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 32

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 22, 23, 24, 25);

// the previous reading from the analog input
int previous = 0;

void setup() {
  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(1000);
}

void loop() {
  // get the sensor value

  int val = analogRead(A0);
  if(val > 2*496){
    val = 2*496;
  }
  val = val - 496;

  // move a number of steps equal to the change in the
  // sensor reading
  stepper.step(val - previous);

  // remember the previous value of the sensor
  previous = val;
}