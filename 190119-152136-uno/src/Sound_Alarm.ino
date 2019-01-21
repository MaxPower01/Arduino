#include <Arduino.h>

const int BUTTON_RED = 2;
const int BUTTON_WHITE = 3;
const int BUTTON_YELLOW = 4;

int buttonRed = false;
int buttonWhite = false;
int buttonYellow = false;

const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A1;
const int SOUND_SENSOR_3 = A2;
const int SOUND_SENSOR_4 = A3;
const int SOUND_SENSOR_5 = A4;
const int SOUND_SENSOR_6 = A5;

/*
  Définition du nombre d'entrées, du nombre d'échantillons à prendre en compte
  lors de l'établissement de la moyenne ainsi que de le niveau de sensibilité
  sonore du système d'alarme :
*/

const int INPUTS = 6;
const int READINGS = 20;
const int SENSITIVITY = 0;

// Tableau contenant les entrées des capteurs :
const byte inputPin[INPUTS] = {A0, A1, A2, A3, A4, A5};

int readings[INPUTS][READINGS];   // Lectures provenant des capteurs
int readIndex[INPUTS] = {0, 0};   // Index de la lecture en cours
int total[INPUTS] = {0, 0};
int average[INPUTS] = {0, 0};

bool systemArmed = false;





void setup() {
  Serial.begin(9600);

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

  // Initialises toutes les lectures d'échantillons à 0 :
  for (int i = 0; i < INPUTS; i++) {
    for (int thisReading = 0; thisReading < READINGS; thisReading++) {
      readings[i][thisReading] = 0;
    }
  }
}

void checkButtonsState() {
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

void armSystem() {
  Serial.println("Armement du système dans 3 secondes.");
  delay(1000);
  Serial.println("Armement du système dans 2 secondes.");
  delay(1000);
  Serial.println("Armement du système dans 1 secondes.\n");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  systemArmed = true;
  Serial.println("Système armé, un son trop fort va déclencher l'alarme.\n");
}

void averageSoundReadings() {
  for (int i = 0; i < INPUTS; i++) {
    // Soustrais la dernière lecture :
    total[i] = total[i] - readings[i][readIndex[i]];
    // Lis un échantillon provenant du capteur :
    readings[i][readIndex[i]] = analogRead(inputPin[i]);
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

void loop() {
  checkButtonsState();

  if (buttonRed) {
    armSystem();
  }

  averageSoundReadings();

  // Si le système est armé, une moyenne de son plus forte que "SENSITIVITY" déclenche l'alarme :
  if (systemArmed) {
    for (int i = 0; i < INPUTS; i++) {
      if (average[i] > SENSITIVITY) {
        digitalWrite(LED_BUILTIN, HIGH);
        systemArmed = false;
        Serial.print("Un son d'une intensité de ");
        Serial.print(average[i]);
        Serial.print(" a été détecté par le capteur #");
        Serial.print(i + 1);
        Serial.println(".\n");
      }
    }
  }

  // Permets de donner un peu de repos à cette pauvre boucle et d'augmenter la stabilité :
  delay(1);
}
