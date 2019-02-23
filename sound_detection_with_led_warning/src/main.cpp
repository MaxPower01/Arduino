#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

// 1. Variables
// 2. Setup
// 3. Methods
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

RH_ASK driver;

// Boutons :
const int BUTTON = 8;
bool button = false;

// Capteurs de son :
const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A2;
const int SOUND_SENSOR_3 = A3;
const int SOUND_SENSOR_4 = A5;

// Lumières LED :
const int LED_GREEN = 5;
const int LED_YELLOW = 3;
const int LED_RED = 4;

// Système d'alarme :
bool systemArmed = false;
bool alarm = false;

int warningLevel = 0;

// Moyenne d'échantillons :
const int INPUTS = 5;

const byte inputPins[INPUTS] = {A0, A2, A3, A5};

const int SMOOTHNESS = 20;          // Nombre d'échantillons à prendre en compte

int threshold = 2;
int minThreshold = 1;
int maxThreshold = 100;                // Niveau de sensibilité de la détection

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
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  pinMode(BUTTON, INPUT);

  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  resetReadings();

  if (!driver.init())
         Serial.println("init failed");
}





/* ============================================================ *\ 
|  ==============> 3. Methods
\* ============================================================ */ 

void resetReadings() {
  for (int i = 0; i < INPUTS; i++) {
    for (int thisReading = 0; thisReading < SMOOTHNESS; thisReading++) {
      readings[i][thisReading] = 0;
    }
  }
}

void armSystem() {
  delay(1000);
  systemArmed = true;
}



void disarmSystem() {
  systemArmed = false;
  delay(1000);
}



void checkButtons() {
  if (digitalRead(BUTTON) == HIGH) {
    button = true;
  } else {
    button = false;
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



void flashLed(byte led, int rhythm) {
    digitalWrite(led, HIGH);
    delay(rhythm);
    digitalWrite(led, LOW);
    delay(rhythm);
}



void triggerAlarm(int led) {
  int previousLed = led - 1;

  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(led, HIGH);
  digitalWrite(previousLed, LOW);
}



void watchSamples() {
  for (int i = 0; i < INPUTS; i++) {
    if (average[i] >= threshold && systemArmed) {

      // Le niveau d'alarme augmente à chaque fois qu'un son est détecté pendant que le système est armé :
      Serial.print("Warning triggerred : ");
      Serial.println(average[i]);

      disarmSystem();

      warningLevel = warningLevel + 1;      
    }
  }
}



void warn() {
  Serial.println(warningLevel);

  switch (warningLevel)
      {
        case 1:
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_YELLOW, LOW);
          digitalWrite(LED_RED, LOW);
          break;
        
        case 2:
          digitalWrite(LED_GREEN, LOW);
          digitalWrite(LED_YELLOW, HIGH);
          break;

        case 3:
          digitalWrite(LED_YELLOW, LOW);
          digitalWrite(LED_RED, HIGH);
          break;

        case 4:
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_YELLOW, HIGH);
          digitalWrite(LED_RED, HIGH);
          const char *msg = "Hello World!";
          driver.send((uint8_t *)msg, strlen(msg));
          driver.waitPacketSent();
          delay(1000);
          warningLevel = 0;
          break;
      
        default:
          break;
      }
}





/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  checkButtons();

  if (button) {
    armSystem();
  }

  if (systemArmed) {
    for(int i = 0; i < INPUTS; i++)
    {
      if(analogRead(inputPins[i]) > threshold) {
        warningLevel = warningLevel + 1;      
        warn();
        delay(1000);
        return;
      }
    }
  }
  

  // smoothing();

  // watchSamples();

  // warn();

  delay(10);
}