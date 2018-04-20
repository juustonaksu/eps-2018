

void setup() {
  Serial.begin(9600);
  //Y
  pinMode(30, INPUT_PULLUP);
  //X
  pinMode(29, INPUT_PULLUP);
  //Z
  pinMode(25, INPUT_PULLUP);
}

void loop() {
  int digitalvalue1 = digitalRead(30);
  int digitalvalue2 = digitalRead(29);
  int digitalvalue3 = digitalRead(25);
  //Serial.println(analogvalue);
  Serial.println(digitalvalue1);
  Serial.print(digitalvalue2);
  Serial.print(digitalvalue3);
}
