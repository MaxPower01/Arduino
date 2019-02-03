#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Methods
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

// Lasers :
const int LASER_1 = 2;
const int LASER_2 = 4;
const int LASER_3 = 7;
const int LASER_4 = 10;
const int LASER_5 = 13;

// État des boutons :
bool button = false;

// Système d'alarme :
bool systemArmed = false;
bool alarm = false;
int alarmLevel = 0;



/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Définition du mode de chacune des pattes utilisées :
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LASER_1, OUTPUT);
  pinMode(LASER_2, OUTPUT);
  pinMode(LASER_3, OUTPUT);
  pinMode(LASER_4, OUTPUT);
  pinMode(LASER_5, OUTPUT);
}



/* ============================================================ *\ 
|  ==============> 4. Methods
\* ============================================================ */ 

void flash(int digitalPin, int interval) {
    digitalWrite(digitalPin, HIGH);
    delay(interval);
    digitalWrite(digitalPin, LOW);
    delay(interval);
}



/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  delay(500);
}