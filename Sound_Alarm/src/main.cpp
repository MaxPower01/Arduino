#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Fonctions
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

const int BUTTON_RED = 2;
const int BUTTON_WHITE = 3;
const int BUTTON_YELLOW = 4;

const int SOUND_SENSOR_1 = A0;
// const int SOUND_SENSOR_2 = A1;
const int SOUND_SENSOR_3 = A2;
const int SOUND_SENSOR_4 = A3;
// const int SOUND_SENSOR_5 = A4;
const int SOUND_SENSOR_6 = A5;

const int INPUTS = 4;
// const int INPUTS = 6;             // Nombre d'entrées
const int READINGS = 20;          // Nombre d'échantillons à prendre en compte
const int SENSITIVITY = 1;        // Niveau de sensibilité de la détection sonore

const byte inputPins[INPUTS] = {A0, A2, A3, A5};
// const byte inputPins[INPUTS] = {A0, A1, A2, A3, A4, A5};

int readings[INPUTS][READINGS];   // Lectures provenant des capteurs
int readIndex[INPUTS] = {0, 0};   // Index de la lecture en cours
int total[INPUTS] = {0, 0};       // Total
int average[INPUTS] = {0, 0};     // Moyenne

bool buttonRed = false;
bool buttonWhite = false;
bool buttonYellow = false;

bool systemArmed = false;

/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialises la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Établis le mode de chacune des pattes utilisées :
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(BUTTON_RED, INPUT);
  // pinMode(BUTTON_WHITE, INPUT);
  // pinMode(BUTTON_YELLOW, INPUT);

  pinMode(SOUND_SENSOR_1, INPUT);
  // pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);
  // pinMode(SOUND_SENSOR_5, INPUT);
  pinMode(SOUND_SENSOR_6, INPUT);

  // Initialises toutes les lectures d'échantillons à 0 :
  for (int i = 0; i < INPUTS; i++) {
    for (int thisReading = 0; thisReading < READINGS; thisReading++) {
      readings[i][thisReading] = 0;
    }
  }
}



void armSystem() {
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  systemArmed = true;
}

void checkButtons() {
  // Vérifie si un bouton est appuyé :
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

  // Actions à effectuer si un bouton est appuyé :
  if (buttonRed) {
    armSystem();
  }
}

// Permet d'établir la moyenne d'un nombre "READINGS" d'échantillons :
void averageReadings() {
  for (int i = 0; i < INPUTS; i++) {
    // Soustrais la dernière lecture :
    total[i] = total[i] - readings[i][readIndex[i]];

    // Lis un échantillon provenant du capteur :
    readings[i][readIndex[i]] = analogRead(inputPins[i]);

    // Ajoutes cet échantillon au total :
    total[i] = total[i] + readings[i][readIndex[i]];

    // Avances à la position suivante dans le tableau :
    readIndex[i] = readIndex[i] + 1;

    // Si nous sommes à la fin du tableau, retournes au début :
    if (readIndex[i] >= READINGS) {
      readIndex[i] = 0;
    }

    // Calcules la moyenne des échantillons :
    average[i] = total[i] / READINGS;
  }
}

// Si le système est armé, une moyenne d'échantillons plus forte que "SENSITIVITY" déclenche l'alarme :
void monitorAverage() {
  if (systemArmed) {
    for (int i = 0; i < INPUTS; i++) {
      if (average[i] >= SENSITIVITY) {
        digitalWrite(LED_BUILTIN, HIGH);
        
        systemArmed = false;
        
        Serial.print("Un son d'une intensité de ");
        Serial.print(average[i]);
        Serial.print(" a été détecté par le capteur #");
        Serial.println(i + 1);
      }
    }
  }
}

/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  checkButtons();

  averageReadings();

  monitorAverage();

  // Donne un peu de repos à la boucle :
  delay(1);
}