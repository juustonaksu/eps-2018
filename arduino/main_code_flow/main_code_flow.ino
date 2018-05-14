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
  stepperz.move(-40000);
  stepperx.move(-40000);
  steppery.move(-18000);
  /*while (digitalRead(A10) == LOW) {
    delay(1);
    stepperz.run();
    }
    stepperz.setCurrentPosition(0);
    stepperx.move(-40000);
    while (digitalRead(A9) == LOW) {
    delay(1);
    stepperx.run();
    }
    stepperx.setCurrentPosition(0);
    while (digitalRead(A8) == LOW) {
    steppery.move(-18000);
    steppery.run();
    }
    steppery.setCurrentPosition(0);*/
  while (true) {
    int Z = digitalRead(A10);
    int X = digitalRead(A9);
    int Y = digitalRead(A8);
    if ((Z == HIGH) && (X == HIGH) && (Y == HIGH)) {
      break;
    }
    if (X == LOW) {
      stepperx.run();
    }
    if (Z == LOW) {
      stepperz.run();
    }
    if (Y == LOW) {
      steppery.run();
    }
  }
  stepperx.setCurrentPosition(0);
  steppery.setCurrentPosition(0);
  stepperz.setCurrentPosition(0);
}

void changeBattery() {
  digitalWrite(A11, HIGH);
  digitalWrite(A12, LOW);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  //move plate to push the drone
  steppery.moveTo(2500);
  while (steppery.currentPosition() != 2500) {
    steppery.run();
  }
  dcmotorleft.run(-motorSpeed);
  delay(15000);
  //left stick is out
  dcmotorleft.stop();
  //move the stick to the battery
  stepperx.moveTo(4500);
  while (stepperx.currentPosition() != 4500) {
    stepperx.run();
    delay(1);
  }
  //pull the battery out
  dcmotorleft.run(motorSpeed);
  delay(16000);
  dcmotorleft.stop();
  //empty battery is out, move full battery to place
  stepperx.moveTo(6900);
  while (stepperx.currentPosition() != 6900 ) {
    stepperx.run();
    delay(1);
  }
  //full battery is in correct position, extrude it
  dcmotorright.run(-motorSpeed);
  delay(15000);
  dcmotorright.stop();
  //full battery is inside drone, now you need to move the arm away
  stepperx.moveTo(9000);
  while (stepperx.currentPosition() != 9000) {
    stepperx.run();
    delay(1);
  }
  //arm is out of the drone, put it back to rest position
  dcmotorright.run(motorSpeed);
  delay(16000);
  dcmotorright.stop();

  //full battery is in the drone, system ready to go on
  batteryScan();
}

void batteryScan() {
  stepperz.moveTo(4000);
  while (stepperz.currentPosition() != 4000) {
    stepperz.run();
    delay(1);
  }
  delay(1000);
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
    //take care of the offset
    m2=m2-1600;
    moveMotors(m1, m2);
  }
  //wait, simulating dropping the battery to the charger
  dcmotorleft.run(-motorSpeed);
  delay(19900);
  dcmotorleft.stop();
  //now take the offset away
  int targetpos = stepperx.currentPosition();
  targetpos = targetpos - 1600;
  stepperx.move(-1600);
  while (stepperx.currentPosition() != targetpos ) {
    stepperx.run();
    delay(1);
  }
  dcmotorleft.run(motorSpeed);
  delay(19900);
  dcmotorleft.stop();
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
  while (true) {
    if ((stepperx.currentPosition() == x) && (stepperz.currentPosition() == z)) {
      break;
    } else {
      if (stepperx.currentPosition() != x) {
        stepperx.run();
      }
      if (stepperz.currentPosition() != z) {
        stepperz.run();
      }
    }
  }
}
void takeFullBattery() {
  //move plate to back, so drone has space to land
  digitalWrite(A11, LOW);
  digitalWrite(A12, HIGH);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  steppery.moveTo(17500);
  while (steppery.currentPosition() != 17500) {
    steppery.run();
  }
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
    //wait, simulating dropping the battery to the charger
    dcmotorright.run(-motorSpeed);
    delay(19500);
    dcmotorright.stop();
    //delay for debugging!
    int targetpos = stepperx.currentPosition();
    targetpos = targetpos + 1000;
    stepperx.move(1000);
    while (stepperx.currentPosition() != targetpos ) {
      stepperx.run();
      delay(1);
    }
    delay(5000);
    dcmotorright.run(motorSpeed);
    delay(19900);
    dcmotorright.stop();
    //move system to waiting position
    moveMotors(12150, 6000);
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
