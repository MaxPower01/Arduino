#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Methods
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

// Boutons :
const int BUTTON_RED = 2;
const int BUTTON_WHITE = 3;
const int BUTTON_YELLOW = 4;

// Capteurs de son :
const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A1;
const int SOUND_SENSOR_3 = A2;
const int SOUND_SENSOR_4 = A3;
const int SOUND_SENSOR_5 = A4;
const int SOUND_SENSOR_6 = A5;

// État des boutons :
bool buttonRed = false;
bool buttonWhite = false;
bool buttonYellow = false;

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
  pinMode(BUTTON_RED, INPUT);
  pinMode(BUTTON_WHITE, INPUT);
  pinMode(BUTTON_YELLOW, INPUT);
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

void checkButtons() {
  if (digitalRead(BUTTON_RED) == HIGH) {
    buttonRed = true;
  } else {
    buttonRed = false;
  }

  if (digitalRead(BUTTON_WHITE) == HIGH) {
    buttonWhite = true;
  } else {
    buttonWhite = false;
  }

  if (digitalRead(BUTTON_YELLOW) == HIGH) {
    buttonYellow = true;
  } else {
    buttonYellow = false;
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

void flashLed(byte led) {
    digitalWrite(led, HIGH);
    delay(250);
    digitalWrite(led, LOW);
    delay(250);
}

void triggerAlarm() {
  for(int i = 0; i < alarmLevel; i++) {
    flashLed(LED_BUILTIN);
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
  checkButtons();

  if (buttonRed) {
    armSystem();
  }

  averageSamples();
  watchSamples();

  flashLed(LED_BUILTIN);

  delay(1);
}