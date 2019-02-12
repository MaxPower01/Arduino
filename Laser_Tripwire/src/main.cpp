#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Fonctions
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

// Boutons :
const int BUTTON_RED = 12;

bool buttonRed = false;

// Lumières LED :
const int LED_GREEN = 2;
const int LED_BLUE = 3;
const int LED_RED = 4;

// Capteur de lumière :
const int LIGHT_SENSOR = A5;

int light;

// Laser :
const int LASER = 8;

// Système d'alarme :
bool systemArmed = false;

bool alarm = false;
int alarmLevel = 1;

// Moyenne d'échantillons :
const int SMOOTHNESS = 3;     // Nombre d'échantillons à prendre en compte
const int THRESHOLD = 750;  // Niveau de sensibilité de la détection

int readings[SMOOTHNESS];     // Lectures provenant des capteurs
int readIndex = 0;          // Index de la lecture en cours
int total = 0;              // Somme des échantillons
int average = 0;            // Moyenne des échantillons





/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Définition du mode de chacune des pattes utilisées :
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  pinMode(BUTTON_RED, INPUT);

  pinMode(LIGHT_SENSOR, INPUT);

  pinMode(LASER, OUTPUT);
}





/* ============================================================ *\ 
|  ==============> 3. Fonctions
\* ============================================================ */

void armSystem() {
  if (alarmLevel >= 4) {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    
    delay(1000);
    
    digitalWrite(LED_BUILTIN, HIGH);
    
    systemArmed = true;
    alarmLevel = 1;

  } else {
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    systemArmed = true;
  }
}

void checkButtons() {
  // Vérifie si un bouton est appuyé :
  if (digitalRead(BUTTON_RED) == HIGH) {
    buttonRed = true;
  } else {
    buttonRed = false;
  }
}

void smoothing() {
    // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(LIGHT_SENSOR);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= SMOOTHNESS) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  average = total / SMOOTHNESS;
}

void triggerAlarm(int led) {
  int previousLed = led - 1;

  digitalWrite(LASER, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(led, HIGH);
  digitalWrite(previousLed, LOW);
}





/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {

  checkButtons();

  if (buttonRed) {
    digitalWrite(LASER, HIGH);
    armSystem();
  }
  
  light = analogRead(LIGHT_SENSOR);

  // Établissement de la moyenne :
  // averageSamples();
  
  // Serial.print("La moyenne des 5 derniers échantillons est : ");
  // Serial.println(average);
  Serial.println(light);

  if (light <= THRESHOLD && systemArmed) {
    systemArmed = false;
    alarmLevel = alarmLevel + 1;
    
    triggerAlarm(alarmLevel);
  }

  delay(1);
}