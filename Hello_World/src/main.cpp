#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Methods
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

// Boutons :
const int BUTTON = 2;
const int LASER = 3;

// Capteurs de son :
const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A1;
const int SOUND_SENSOR_3 = A2;
const int SOUND_SENSOR_4 = A3;
const int SOUND_SENSOR_5 = A4;
const int SOUND_SENSOR_6 = A5;

// État des boutons :
bool button = false;

// Système d'alarme :
bool systemArmed = false;
bool alarm = false;
int alarmLevel = 0;

// Moyenne d'échantillons :
const int INPUTS = 6;
const byte inputPins[INPUTS] = {A0, A1, A2, A3, A4, A5};

const int READINGS = 20;          // Nombre d'échantillons à prendre en compte
int threshold = 1;                // Niveau de sensibilité de la détection
int readings[INPUTS][READINGS];   // Lectures provenant des capteurs
int readIndex[INPUTS] = {0, 0};   // Index de la lecture en cours
int total[INPUTS] = {0, 0};       // Somme des échantillons
int average[INPUTS] = {0, 0};     // Moyenne des échantillons





/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Définition du mode de chacune des pattes utilisées :
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LASER, OUTPUT);
  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);
  pinMode(SOUND_SENSOR_5, INPUT);
  pinMode(SOUND_SENSOR_6, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  for (int i = 0; i < INPUTS; i++) {
    for (int thisReading = 0; thisReading < READINGS; thisReading++) {
      readings[i][thisReading] = 0;
    }
  }
}





/* ============================================================ *\ 
|  ==============> 4. Methods
\* ============================================================ */ 

void armSystem() {
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  systemArmed = true;
}

void checkButton() {
  if (digitalRead(BUTTON) == HIGH) {
    button = true;
  } else {
    button = false;
  }
}

void averageSamples() {
  for (int i = 0; i < INPUTS; i++) {
    // Soustraction de la dernière lecture :
    total[i] = total[i] - readings[i][readIndex[i]];

    // Lecture d'un échantillon provenant du capteur :
    readings[i][readIndex[i]] = analogRead(inputPins[i]);

    // Ajout de cet échantillon au total :
    total[i] = total[i] + readings[i][readIndex[i]];

    // On passe à la position suivante :
    readIndex[i] = readIndex[i] + 1;

    // Si on est à la fin du tableau, on recommence :
    if (readIndex[i] >= READINGS) {
      readIndex[i] = 0;
    }

    // Calcul de la moyenne des échantillons pour le capteur "i" :
    average[i] = total[i] / READINGS;
  }
}

void flash(int pin, int interval) {
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
    delay(interval);
}

void triggerAlarm() {
  for(int i = 0; i < alarmLevel; i++) {
    flash(LED_BUILTIN, 250);
  }
}

void watchSamples() {
  for (int i = 0; i < INPUTS; i++) {
    if (average[i] >= threshold && systemArmed) {
      // Le niveau d'alarme augmente à chaque fois qu'un son est détecté pendant que le système est armé :
      systemArmed = false;
      alarmLevel = alarmLevel + 1;
      triggerAlarm();
      systemArmed = true;
    }
  }
}





/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  flash(LED_BUILTIN, 10);
  flash(LASER, 10);
  delay(1);
}