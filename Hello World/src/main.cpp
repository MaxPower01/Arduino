#include <Arduino.h>

byte count = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.write(45);
  Serial.write(46);
  delay(1);
  Serial.println(Serial.read());


  count = count + 1;
  delay(1000);
}
