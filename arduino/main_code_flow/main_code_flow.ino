#include <NewPing.h>
#include "MeMegaPi.h"

#define TEMP_PIN A7
#define PING_PIN A6
#define MAX_DISTANCE 400

MeMegaPiDCMotor dcmotorleft(PORT4A);
MeMegaPiDCMotor dcmotorright(PORT4B);

MeStepperOnBoard stepperx(SLOT_1);
MeStepperOnBoard stepperz(SLOT_2);
MeStepperOnBoard steppery(SLOT_3);
int m1 = 1;
int m2 = 1;
NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE);
boolean fullbatteryiswaiting = false;
long motorTimer = 0;
uint8_t motorSpeed = 200;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // set the baud rate

  stepperx.setMaxSpeed(1000);
  stepperx.setAcceleration(2000);
  stepperx.setMicroStep(1);
  stepperx.enableOutputs();

  stepperz.setMaxSpeed(1000);
  stepperz.setAcceleration(2000);
  stepperz.setMicroStep(1);
  stepperz.enableOutputs();

  steppery.setMaxSpeed(1000);
  steppery.setAcceleration(2000);
  steppery.setMicroStep(1);
  steppery.enableOutputs();
  //Y
  pinMode(30, INPUT_PULLUP);
  //X
  pinMode(29, INPUT_PULLUP);
  //Z
  pinMode(25, INPUT_PULLUP);
  pinMode(28, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  initializeMotors();
}

void initializeMotors() {
  dcmotorleft.run(motorSpeed); /* value: between -255 and 255. */
  dcmotorright.run(motorSpeed); /* value: between -255 and 255. */
  delay(2000);
  dcmotorleft.stop();
  dcmotorright.stop();
  delay(100);
  dcmotorleft.run(-motorSpeed);
  dcmotorright.run(-motorSpeed);
  delay(2000);
  dcmotorleft.stop();
  dcmotorright.stop();
  stepperx.move(-10000);
  stepperz.move(-10000);
  steppery.move(-10000);

  while (digitalRead(29) == HIGH) {
    stepperx.run();
  }
  stepperx.setCurrentPosition(0);
  while (digitalRead(30) == HIGH) {
    steppery.run();
  }
  steppery.setCurrentPosition(0);
  while (digitalRead(25) == HIGH) {
    stepperz.run();
  }
  stepperz.setCurrentPosition(0);
}

void changeBattery() {
  Serial.write("Change");
  steppery.moveTo(500);
  while (stepperx.currentPosition() != 500) {
    steppery.run();
  }
  int dctime = 25000;
  motorTimer = millis();
  //while (millis() - motorTimer < dctime) {
  dcmotorleft.run(motorSpeed);
  //}
  delay(2000);
  dcmotorleft.stop();
  stepperx.moveTo(2500);
  while (stepperx.currentPosition() != 2500) {
    stepperx.run();
  }
  motorTimer = millis();
  while (millis() - motorTimer < dctime) {
    dcmotorleft.run(-motorSpeed);
  }
  dcmotorleft.stop();
  //empty battery is out
  stepperx.moveTo(3500);
  while (stepperx.currentPosition() != 3500) {
    stepperx.run();
  }
  motorTimer = millis();
  while (millis() - motorTimer < dctime) {
    dcmotorright.run(motorSpeed);
  }
  dcmotorright.stop();
  stepperx.moveTo(0);
  while (stepperx.currentPosition() != 0) {
    stepperx.run();
  }
  motorTimer = millis();
  while (millis() - motorTimer < dctime) {
    dcmotorright.run(-motorSpeed);
  }
  dcmotorright.stop();
  //full battery is in the drone, system ready to go on
  batteryScan();
}

void batteryScan() {
  stepperz.moveTo(2000);
  while (stepperx.currentPosition() != 2000) {
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
  }
  //go back to loop
  fullbatteryiswaiting = false;
}
void moveMotors(int z, int x) {
  int val = analogRead(TEMP_PIN);
  float mv = (val / 1024.00) * 5000;
  float cel = mv / 10;
  if (cel < 25) {
    stepperz.moveTo(z);
    stepperx.moveTo(x);
    digitalWrite(26, LOW);
    digitalWrite(27, HIGH);
    digitalWrite(28, LOW);
  } else {
    digitalWrite(26, HIGH);
    digitalWrite(27, LOW);
    digitalWrite(28, LOW);
  }
  while ((stepperz.currentPosition() != z) && (stepperx.currentPosition() != x)) {
    stepperz.run();
    stepperx.run();
  }
}
void takeFullBattery() {
  Serial.write("Gimme coords");
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
    moveMotors(3000, 3000);
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
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  digitalWrite(28, HIGH);
  /*stepperx.run();
    steppery.run();
    stepperz.run();*/
}
