#include "MeMegaPi.h"

MeStepperOnBoard stepper(SLOT_2);
MeStepperOnBoard stepper2(SLOT_1);
MeStepperOnBoard stepper3(SLOT_3);
int analogvalue;
String a = "";
boolean buttonActive = false;
boolean longPressActive = false;
long buttonTimer = 0;
long longPressTime = 250;
int moveamount = 1000;
int val;
int tempPin = A7;

void setup() {
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(2000);
  stepper.setMicroStep(2);
  stepper.enableOutputs();
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(2000);
  stepper2.setMicroStep(1);
  stepper2.enableOutputs();
  stepper3.setMaxSpeed(1000);
  stepper3.setAcceleration(2000);
  stepper3.setMicroStep(1);
  stepper3.enableOutputs();
  pinMode(30, INPUT_PULLUP);
  pinMode(28, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
}

void loop() {
  val = analogRead(tempPin);
  float mv = ( val / 1024.00) * 5000;
  float cel = mv / 10;
  if (cel > 24) {
    digitalWrite(26, HIGH);
    digitalWrite(27, LOW);
    digitalWrite(28, LOW);
  }
  else if (cel > 23) {
    digitalWrite(28, HIGH);
    digitalWrite(27, LOW);
    digitalWrite(26, LOW);
  } else {
    digitalWrite(26, LOW);
    digitalWrite(28, LOW);
    digitalWrite(27, HIGH);
  }
  analogvalue = digitalRead(30);
  //Serial.println(analogvalue);
  if (analogvalue == LOW) {
    if (cel < 24) {
      stepper.move(moveamount);
      //stepper2.move(moveamount);
      //stepper3.move(moveamount);
      stepper.run();
      //stepper2.run();
      //stepper3.run();
    }
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
    }
  } else {
    if (buttonActive == true) {

      if (longPressActive == true) {
        Serial.println("Long");
        longPressActive = false;
        moveamount = moveamount * -1;
      } else {
        Serial.println("Short");
      }
      buttonActive = false;
    }
  }
}
