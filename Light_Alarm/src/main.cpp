#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Fonctions
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

const int AMBIENT_LIGHT = A5;
const int SENSITIVITY = 200;

int ambientLight;

/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(AMBIENT_LIGHT, INPUT);

}

/* ============================================================ *\ 
|  ==============> 3. Fonctions
\* ============================================================ */

/* void checkButtons() {
  // Vérifie si un bouton est appuyé :
  if (digitalRead(BUTTON_RED) == HIGH) {
    buttonRed = true;
  } else {
    buttonRed = false;
  }

  // Actions à effectuer si un bouton est appuyé :
  if (buttonRed) {
    armSystem();
  }
} */

/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  ambientLight = analogRead(AMBIENT_LIGHT);
  
  if (ambientLight < SENSITIVITY) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(ambientLight);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  }

  Serial.println(ambientLight);

  delay(10);
}