#include <Arduino.h>

const int BUZZER = 6;


void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(BUZZER, LOW);
  delay(500);
  digitalWrite(BUZZER, HIGH);
  delay(500);
}