#include <Arduino.h>
#include "Keyboard.h"

// 1. Variables
// 2. Setup
// 3. Fonctions
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

const int VIBRATION_SENSOR_1 = A0;
const int VIBRATION_SENSOR_2 = A1;
const int VIBRATION_SENSOR_3 = A2;
const int VIBRATION_SENSOR_4 = A3;
const int VIBRATION_SENSOR_5 = A4;

int vibration1;
int vibration2;
int vibration3;
int vibration4;
int vibration5;

// Moyenne d'échantillons :
const int SMOOTHNESS = 5;     // Nombre d'échantillons à prendre en compte
const int THRESHOLD = 50;      // Niveau de sensibilité de la détection

int readings[SMOOTHNESS];     // Lectures provenant des capteurs
int readIndex = 0;            // Index de la lecture en cours
int total = 0;                // Somme des échantillons
int average = 0;              // Moyenne des échantillons

// https://learn.adafruit.com/midi-drum-glove/code

// const int indexFinger = A0; // the piezo is connected to analog pin 9 (aka D9)
// const int middleFinger = A1; // the piezo is connected to analog pin 7 (aka D6)
// const int thumb = A2; // the piezo is connected to analog pin 10 (aka D10)
// const int pinkyFinger = A3; // the piezo is connected to analog pin 11 (aka D12)
 
const int pins[] = {VIBRATION_SENSOR_1, VIBRATION_SENSOR_2, VIBRATION_SENSOR_3, VIBRATION_SENSOR_4};
 
char Keys[] =   {'z','x','c','v'};
 
boolean currentPressed[] = {false, false, false, false};
 
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not





/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */

void setup() {
  // Serial.begin(9600);
  // Serial.println("start");
  Keyboard.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(VIBRATION_SENSOR_1, INPUT);
  pinMode(VIBRATION_SENSOR_2, INPUT);
  pinMode(VIBRATION_SENSOR_3, INPUT);
  pinMode(VIBRATION_SENSOR_4, INPUT);
  pinMode(VIBRATION_SENSOR_5, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  for (int thisReading = 0; thisReading < SMOOTHNESS; thisReading++) {
    readings[thisReading] = 0;
  }
}





/* ============================================================ *\ 
|  ==============> 3. Methods
\* ============================================================ */ 

int smoothing(const int SENSOR) {
  // subtract the last reading:
  total = total - readings[readIndex];

  // read from the sensor:
  readings[readIndex] = analogRead(SENSOR);

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
  return average;
}





/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  vibration1 = smoothing(VIBRATION_SENSOR_1);

  if (vibration1 <= THRESHOLD) {
    keyboard.print("Tu viens de frappe le capteur.");
  }

  delay(1);
}
