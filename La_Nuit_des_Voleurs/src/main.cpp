#include <Arduino.h>

void setup() {
    Serial.begin(9600); // Debugging only
  
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("ON");
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("OFF");
    delay(1000);
}