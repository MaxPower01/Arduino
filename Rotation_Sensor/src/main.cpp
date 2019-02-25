#include <Arduino.h>

const int ROTATION_SENSOR = A5;
int rotation = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(ROTATION_SENSOR, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  rotation = analogRead(ROTATION_SENSOR);

  if (rotation > 95 && rotation < 110) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }
  
  delay(1);
}