#include <NewPing.h>
#include "MeMegaPi.h"

#define TEMP_PIN A7
#define PING_PIN A6
#define MAX_DISTANCE 400

MeMegaPiDCMotor dcmotorleft(PORT4A);
MeMegaPiDCMotor dcmotorright(PORT4B);
// Okay what the fuck
MeStepperOnBoard stepperz(SLOT_1);
MeStepperOnBoard stepperx(SLOT_2);
MeStepperOnBoard steppery(SLOT_3);
int m1 = 1;
int m2 = 1;
NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE);
boolean fullbatteryiswaiting = false;
long motorTimer = 0;
// minus= away from motorshaft + towards motorshaft
// value: between -255 and 255.
uint8_t motorSpeed = 255;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // set the baud rate
  // - left + right
  stepperx.setMaxSpeed(1000);
  stepperx.setAcceleration(2000);
  stepperx.setMicroStep(1);
  stepperx.enableOutputs();
  //- down + up
  stepperz.setMaxSpeed(1000);
  stepperz.setAcceleration(2000);
  stepperz.setMicroStep(1);
  stepperz.enableOutputs();
  // - back + forward
  steppery.setMaxSpeed(1000);
  steppery.setAcceleration(2000);
  steppery.setMicroStep(1);
  steppery.enableOutputs();
  //Y
  pinMode(A8, INPUT_PULLUP);
  //X
  pinMode(A9, INPUT_PULLUP);
  //Z
  pinMode(A10, INPUT_PULLUP);
  //Yellow
  pinMode(A12, OUTPUT);
  //Green
  pinMode(A13, OUTPUT);
  //Red
  pinMode(A11, OUTPUT);
  //Blue
  pinMode(A14, OUTPUT);
  digitalWrite(A11, HIGH);
  digitalWrite(A12, HIGH);
  digitalWrite(A13, HIGH);
  digitalWrite(A14, HIGH);
  delay(500);
  digitalWrite(A11, LOW);
  digitalWrite(A12, LOW);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  initializeMotors();
}

void initializeMotors() {
  dcmotorleft.run(-motorSpeed);
  dcmotorright.run(-motorSpeed);
  delay(500);
  dcmotorleft.run(motorSpeed);
  dcmotorright.run(motorSpeed);
  delay(5000);
  dcmotorleft.stop();
  dcmotorright.stop();
  delay(100);
  dcmotorleft.stop();
  dcmotorright.stop();
  
  while (digitalRead(A9) == LOW) {
    stepperx.move(-18000);
    stepperx.run();
  }
  
  while (digitalRead(A8) == LOW) {
    steppery.move(-18000);
    steppery.run();
  }
  steppery.setCurrentPosition(0);
  stepperx.setCurrentPosition(0);
  while (digitalRead(A10) == LOW) {
    stepperz.move(-18000);
    stepperz.run();
  }
  stepperz.setCurrentPosition(0);
}

void changeBattery() {
  moveMotors(10000, 2000);
  digitalWrite(A11, HIGH);
  digitalWrite(A12, LOW);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  //move plate to push the drone
  steppery.moveTo(2000);
  while (steppery.currentPosition() != 2000 || digitalRead(A8) == LOW) {
    steppery.run();
  }
  dcmotorleft.run(-motorSpeed);
  delay(19000);
  dcmotorleft.stop();
  stepperx.moveTo(2500);
  while (stepperx.currentPosition() != 2500 || digitalRead(A9) == LOW) {
    stepperx.run();
  }
  dcmotorleft.run(motorSpeed);
  delay(19000);
  dcmotorleft.stop();
  //empty battery is out
  stepperx.moveTo(3500);
  while (stepperx.currentPosition() != 3500 ) {
    stepperx.run();
  }
  dcmotorright.run(-motorSpeed);
  delay(19000);
  dcmotorright.stop();
  stepperx.moveTo(0);
  while (stepperx.currentPosition() != 0 || digitalRead(A9) == LOW) {
    stepperx.run();
  }
  dcmotorright.run(motorSpeed);
  delay(19000);
  dcmotorright.stop();
  //full battery is in the drone, system ready to go on
  batteryScan();
}

void batteryScan() {
  stepperz.moveTo(8000);
  while (stepperz.currentPosition() != 8000) {
    stepperz.run();
  }
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
    //wait, simulating dropping the battery to the charger
    delay(1000);
  }
  //go back to loop
  fullbatteryiswaiting = false;
}
void moveMotors(int z, int x) {
  int val = analogRead(TEMP_PIN);
  float mv = (val / 1024.00) * 5000;
  float cel = mv / 10;
  if (cel < 30) {
    stepperz.moveTo(z);
    stepperx.moveTo(x);
    digitalWrite(A11, LOW);
    digitalWrite(A12, LOW);
    digitalWrite(A13, HIGH);
    digitalWrite(A14, LOW);
  } else {
    digitalWrite(A11, HIGH);
    digitalWrite(A12, LOW);
    digitalWrite(A13, LOW);
    digitalWrite(A14, LOW);
  }
  while ((stepperz.currentPosition() != z) && (stepperx.currentPosition() != x)) {
    stepperz.run();
    stepperx.run();
  }
}
void takeFullBattery() {
  //move plate to back, so drone has space to land
  steppery.moveTo(17000);
  while (steppery.currentPosition() != 17000) {
    steppery.run();
  }
  digitalWrite(A11, LOW);
  digitalWrite(A12, HIGH);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  Serial.println("F");
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
    delay(1000);
    //move system to waiting position
    fullbatteryiswaiting = true;
  }
  //move to drone waiting position
}

void loop() {
  // put your main code here, to run repeatedly:
  if (sonar.ping_cm() < 15) {
    changeBattery();
  }
  if (fullbatteryiswaiting != true) {
    takeFullBattery();
  }
  digitalWrite(A11, LOW);
  digitalWrite(A12, LOW);
  digitalWrite(A13, HIGH);
  digitalWrite(A14, LOW);
}
