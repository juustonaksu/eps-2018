

void setup() {
  Serial.begin(9600);
  //X
  pinMode(30, INPUT_PULLUP);
  //Z
  pinMode(28, INPUT_PULLUP);
}

void loop() {
  int digitalvalue1 = digitalRead(30);
  int digitalvalue2 = digitalRead(28);
  //Serial.println(analogvalue);
  Serial.println(digitalvalue1);
  Serial.print(digitalvalue2);
}
