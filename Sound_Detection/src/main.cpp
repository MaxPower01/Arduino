#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Methods
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

// Boutons :
const int BUTTON_RED = 12;
bool buttonRed = false;

// Capteurs de son :
const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A1;
const int SOUND_SENSOR_3 = A2;
const int SOUND_SENSOR_4 = A4;
const int SOUND_SENSOR_5 = A5;

// Lumières LED :
const int LED_GREEN = 2;
const int LED_BLUE = 3;
const int LED_RED = 4;

// Système d'alarme :
bool systemArmed = false;
bool alarm = false;
int alarmLevel = 1;

// Moyenne d'échantillons :
const int INPUTS = 5;
const byte inputPins[INPUTS] = {A0, A1, A2, A4, A5};
const int SMOOTHNESS = 20;          // Nombre d'échantillons à prendre en compte

const int THRESHOLD = 1;                // Niveau de sensibilité de la détection
int readings[INPUTS][SMOOTHNESS];   // Lectures provenant des capteurs
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

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  pinMode(BUTTON_RED, INPUT);

  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);
  pinMode(SOUND_SENSOR_5, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  for (int i = 0; i < INPUTS; i++) {
    for (int thisReading = 0; thisReading < SMOOTHNESS; thisReading++) {
      readings[i][thisReading] = 0;
    }
  }
}





/* ============================================================ *\ 
|  ==============> 3. Methods
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
  if (digitalRead(BUTTON_RED) == HIGH) {
    buttonRed = true;
    
  } else {
    buttonRed = false;
  }
}

void smoothing() {
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
    if (readIndex[i] >= SMOOTHNESS) {
      readIndex[i] = 0;
    }

    // Calcul de la moyenne des échantillons pour le capteur "i" :
    average[i] = total[i] / SMOOTHNESS;
  }
}

void flashLed(byte led) {
    digitalWrite(led, HIGH);
    delay(250);
    digitalWrite(led, LOW);
    delay(250);
}

void triggerAlarm(int led) {
  int previousLed = led - 1;

  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(led, HIGH);
  digitalWrite(previousLed, LOW);
}

void watchSamples() {
  for (int i = 0; i < INPUTS; i++) {
    if (average[i] >= THRESHOLD && systemArmed) {

      // Le niveau d'alarme augmente à chaque fois qu'un son est détecté pendant que le système est armé :
      Serial.println(average[i]);

      systemArmed = false;

      alarmLevel = alarmLevel + 1;

      Serial.println();

      Serial.println(alarmLevel);
      
      triggerAlarm(alarmLevel);
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

  smoothing();

  watchSamples();

  delay(1);
}