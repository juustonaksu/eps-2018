#include "MeMegaPi.h"
MeStepperOnBoard stepper(SLOT_1);
MeStepperOnBoard stepper2(SLOT_2);
MeStepperOnBoard stepper3(SLOT_3);
int m1 = 1;
int m2 = 1;
void setup() {
  Serial.begin(9600); // set the baud rate
  Serial.println("Ready"); // print "Ready" once
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(2000);
  stepper.setMicroStep(1);
  stepper.enableOutputs();
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(2000);
  stepper2.setMicroStep(1);
  stepper2.enableOutputs();

  stepper3.setMaxSpeed(1000);
  stepper3.setAcceleration(2000);
  stepper3.setMicroStep(1);
  stepper3.enableOutputs();
  pinMode(28, OUTPUT);
  pinMode(27, OUTPUT);
}
void moveMotors(int move1, int move2) {
  stepper.move(move1);
  stepper2.move(move2);
  stepper3.move(move2);
  Serial.println(move1);
  Serial.println(move2);
}
void loop() {
  if (Serial.available()) {
    for (int i = 0; i < 2; i++) {
      String servo = Serial.readStringUntil('&');
      int int_pos = servo.toInt();
      if (i == 0) {
        m1 = int_pos;
      } else if (i == 1) {
        m2 = int_pos;
      }
    }
    moveMotors(m1, m2);
  }
  stepper.run();
  stepper2.run();
  stepper3.run();
}
