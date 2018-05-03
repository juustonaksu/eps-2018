

void setup() {
  Serial.begin(9600);
  //Y
  pinMode(A8, INPUT_PULLUP);
  //X
  pinMode(A9, INPUT_PULLUP);
  //Z
  pinMode(A10, INPUT_PULLUP);
}

void loop() {
  int digitalvalue1 = digitalRead(A8);
  int digitalvalue2 = digitalRead(A9);
  int digitalvalue3 = digitalRead(A10);
  //Serial.println(analogvalue);
  Serial.println(digitalvalue1);
  Serial.print(digitalvalue2);
  Serial.print(digitalvalue3);
}
