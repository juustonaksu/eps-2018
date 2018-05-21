#include <NewPing.h>
#include "MeMegaPi.h"

#define TEMP_PIN A7
#define PING_PIN A6
#define MAX_DISTANCE 400

MeMegaPiDCMotor dcmotorleft(PORT4A);
MeMegaPiDCMotor dcmotorright(PORT4B);

MeStepperOnBoard stepperz(SLOT_1);
MeStepperOnBoard stepperx(SLOT_2);
MeStepperOnBoard steppery(SLOT_3);

//Variables for stepper motor positions
int m1 = 0;
int m2 = 0;

//Create NewPing object
NewPing sonar(PING_PIN, PING_PIN, MAX_DISTANCE);

boolean fullbatteryiswaiting = false;

/*DC motor speed and orientation
minus = away from motor shaft
plus = towards motor shaft
value range between -255 and 255.*/
uint8_t motorSpeed = 255;

void setup() {
  Serial.begin(9600); // set the serial baud rate
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
  //Y End-stop sensor
  pinMode(A8, INPUT_PULLUP);
  //X End-stop sensor
  pinMode(A9, INPUT_PULLUP);
  //Z End-stop sensor
  pinMode(A10, INPUT_PULLUP);
  //Yellow LED
  pinMode(A12, OUTPUT);
  //Green LED
  pinMode(A13, OUTPUT);
  //Red LED
  pinMode(A11, OUTPUT);
  //Yellow LED with blue tape
  pinMode(A14, OUTPUT);
  //Flash LEDs on startup to ensure correct wiring
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
/*
Initalize all motor positions on startup.
Stepper motor and DC motor positions need to be resetted every time the system
powers on, because the system has no knowledge of the motor states after shutdown.
*/
void initializeMotors() {
  dcmotorleft.run(-motorSpeed);
  dcmotorright.run(-motorSpeed);
  delay(500);
  dcmotorleft.run(motorSpeed);
  dcmotorright.run(motorSpeed);
  delay(19500);
  dcmotorleft.stop();
  dcmotorright.stop();
  //Some arbitary, long enough values to make sure the stepper motors run all the way to end stop sensors
  stepperz.move(-45000);
  stepperx.move(-40000);
  steppery.move(-20000);
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
  //Set position at 0 on end stop sensor
  stepperx.setCurrentPosition(0);
  steppery.setCurrentPosition(0);
  stepperz.setCurrentPosition(0);
}

//Fetch full battery coordinates from RasPi and prepare the system ready for the drone.
void takeFullBattery() {
  //Update LED state
  digitalWrite(A11, LOW);
  digitalWrite(A12, HIGH);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  //Variable for counting time for DC motors without delay, see https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay 
  unsigned long millisbefore = 0;
  millisbefore = millis();
  //Move plate to back, so drone has space to land
  steppery.moveTo(17500);
  //Move Z motor to correct position to reduce operating time
  stepperz.moveTo(1500);
  while (steppery.currentPosition() != 17500) {
    steppery.run();
    stepperz.run();
    if ((millis() - millisbefore) <= 15000) {
      dcmotorright.run(-motorSpeed);
      dcmotorleft.run(-motorSpeed);
    } else {
      dcmotorright.stop();
      dcmotorleft.stop();
    }
  }
  //request full battery coordinates from raspi
  Serial.println("F");
  if (Serial.available()) {
    for (int i = 0; i < 2; i++) {
      //Battery coords come separated with &
      String servo = Serial.readStringUntil('&');
      int int_pos = servo.toInt();
      if (i == 0) {
        m1 = int_pos;
      } else if (i == 1) {
        m2 = int_pos;
      }
    }
    moveMotors(m1, m2);
    //take the full battery
    dcmotorright.run(-motorSpeed);
    delay(5180);
    dcmotorright.stop();
    //delay for checking the correct position of the arm
    delay(2000);
    int targetpos = stepperx.currentPosition();
    targetpos = targetpos + 1000;
    stepperx.move(1000);
    while (stepperx.currentPosition() != targetpos ) {
      stepperx.run();
      delay(1);
    }
    delay(500);
    dcmotorright.run(motorSpeed);
    delay(21500);
    dcmotorright.stop();
    //move battery plate to drone waiting position
    moveMotors(12850, 6000);
    fullbatteryiswaiting = true;
  }
}
//Push the drone closer and change the battery using DC and stepper motors
void changeBattery() {
  //Update LEd state
  digitalWrite(A11, HIGH);
  digitalWrite(A12, LOW);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  //move plate to push the drone
  steppery.moveTo(4000);
  while (steppery.currentPosition() != 4000) {
    steppery.run();
  }
  dcmotorleft.run(-motorSpeed);
  delay(4200);
  //now left stick is out
  dcmotorleft.stop();
  //move left stick to the empty battery (in drone)
  stepperx.moveTo(4500);
  while (stepperx.currentPosition() != 4500) {
    stepperx.run();
    delay(1);
  }
  //debug delay for checking that the stick is in correct position
  delay(2000);
  //pull the empty battery out
  dcmotorleft.run(motorSpeed);
  delay(22000);
  dcmotorleft.stop();
  //empty battery is out, move full battery to place
  stepperx.moveTo(7100);
  while (stepperx.currentPosition() != 7100 ) {
    stepperx.run();
    delay(1);
  }
  //full battery is in correct position, extrude it
  dcmotorright.run(-motorSpeed);
  delay(19800);
  dcmotorright.stop();
  //full battery is inside drone, now you need to move the arm away
  stepperx.moveTo(9000);
  while (stepperx.currentPosition() != 9000) {
    stepperx.run();
    delay(1);
  }
  //arm is out of the drone, put it back to rest position
  dcmotorright.run(motorSpeed);
  delay(20500);
  dcmotorright.stop();
  //full battery is in the drone, system ready to go on
  batteryScan();
}

//Stop the battery close to barcode scanner, wait for battery coords and then move empty battery to correct charging station
void batteryScan() {
  //Descend to the barcode scanner level
  stepperz.moveTo(4000);
  while (stepperz.currentPosition() != 4000) {
    stepperz.run();
    delay(1);
  }
  delay(3000);
  //Read coords from raspi
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
    //take care of the offset between left and right DC motor (raspi coords are for taking full battery)
    m2 = m2 - 1600;
    moveMotors(m1, m2);
  }
  //Placing the battery to the charger
  dcmotorleft.run(-motorSpeed);
  delay(19700);
  dcmotorleft.stop();
  //now take the offset away, except if the stepper motor position would be negative(which would be the case with battery #1)
  int targetpos = stepperx.currentPosition();
  if ((targetpos - 1600) < 0) {
    targetpos=targetpos+1000;
    stepperx.move(1000);
  } else {
    targetpos = targetpos - 1600;
    stepperx.move(-1600);
  }

  while (stepperx.currentPosition() != targetpos ) {
    stepperx.run();
    delay(1);
  }
  dcmotorleft.run(motorSpeed);
  delay(20000);
  dcmotorleft.stop();
  //empty battery is now at charger, go back to loop
  fullbatteryiswaiting = false;
}
//Move Z and X motors to desired positions
void moveMotors(int z, int x) {
  //Read temperature sensor
  int val = analogRead(TEMP_PIN);
  float mv = (val / 1024.00) * 5000;
  float cel = mv / 10;
  //determine if the system is too hot to operate (in Celsius)
  if (cel < 30) {
    stepperz.moveTo(z);
    stepperx.moveTo(x);
    digitalWrite(A11, LOW);
    digitalWrite(A12, LOW);
    digitalWrite(A13, HIGH);
    digitalWrite(A14, LOW);
  } else {
    //Too hot!!
    digitalWrite(A11, HIGH);
    digitalWrite(A12, LOW);
    digitalWrite(A13, LOW);
    digitalWrite(A14, LOW);
  }
  //Move both motors simultaneously
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


void loop() {
  if (sonar.ping_cm() < 15) {
    //there is a drone
    changeBattery();
  }
  if (fullbatteryiswaiting != true) {
    //there is no battery waiting for the drone
    takeFullBattery();
  }
  //Light up the yellow LED
  digitalWrite(A11, LOW);
  digitalWrite(A12, LOW);
  digitalWrite(A13, HIGH);
  digitalWrite(A14, LOW);
}
