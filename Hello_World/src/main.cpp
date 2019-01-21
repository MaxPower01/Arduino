#include <Arduino.h>

const int BUTTON = 2;

bool buttonPressed = false;

// byte helloWorld = 'Hello World Byte';

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {

  if (digitalRead(BUTTON) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.write(0);
    Serial.write(1);
    Serial.write(2);
    Serial.write(3);
    Serial.write("Hello World Write");
    Serial.println();
    // Serial.write(helloWorld);

    Serial.print("Hello World Print");
    Serial.println();

    Serial.println("Hello World Print Line");

    delay(500);

    digitalWrite(LED_BUILTIN, LOW);
  }
}
