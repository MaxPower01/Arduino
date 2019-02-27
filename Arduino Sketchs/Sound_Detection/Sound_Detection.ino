/* 
  DÉTECTION DU SON :

  ...
*/


/* 
  1. VARIABLES
  2. SETUP
  3. MÉTHODES
  4. LOOP
*/


/* ======================================================================================== *\ 
|  ======================================================================> 1. VARIABLES
\* ======================================================================================== */ 

#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define COMMON_ANODE

RH_ASK driver;

const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A2;
const int SOUND_SENSOR_3 = A3;
const int SOUND_SENSOR_4 = A5;

const int BUTTON_SWITCH = 4;

const int LED_GREEN = 10;
const int LED_YELLOW = 9;
const int LED_RED = 8;

const char *msg = "Hello world!"; // Message à envoyer via le module RF 433Mhz.

const int ACTIVATION_DELAY = 1000;
const int DETECTION_DELAY = 1000;

const int INPUTS = 5;
const byte inputPins[INPUTS] = {SOUND_SENSOR_1, SOUND_SENSOR_2, SOUND_SENSOR_3, SOUND_SENSOR_4};

const int THRESHOLD = 2;
const int SMOOTHNESS = 20;          // Nombre d'échantillon à prendre en compte.

int readings[INPUTS][SMOOTHNESS];   // Lectures provenant des capteurs d'une taille de "SMOOTHNESS".
int readIndex[INPUTS] = {0, 0};     // Index de la lecture en cours.
int total[INPUTS] = {0, 0};         // Somme des échantillons.
int average[INPUTS] = {0, 0};       // Moyenne des échantillons.

bool systemArmed = false;

int counter = 0;
int warningLevel = 0;





/* ======================================================================================== *\ 
|  ======================================================================> 2. SETUP
\* ======================================================================================== */ 

void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Définition du mode de chacune des pattes utilisées :
  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);
  
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BUTTON_SWITCH, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  resetReadings();

  if (!driver.init())
  {
    Serial.println("init failed");
  }
}





/* ======================================================================================== *\ 
|  ======================================================================> 3. MÉTHODES
\* ======================================================================================== */

void resetReadings() {
  for (int i = 0; i < INPUTS; i++) {
    for (int thisReading = 0; thisReading < SMOOTHNESS; thisReading++) {
      readings[i][thisReading] = 0;
    }
  }
}


void armSystem() {
  systemArmed = true;
}


void disarmSystem() {
  systemArmed = false;
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


void watchSamples() {
  for (int i = 0; i < INPUTS; i++) {
    if (average[i] >= THRESHOLD && systemArmed) {
      warningLevel = warningLevel + 1;      
    }
  }
}


// void setColor(int red, int green, int blue)
// {
//   #ifdef COMMON_ANODE
//     red = 255 - red;
//     green = 255 - green;
//     blue = 255 - blue;
//   #endif
//   analogWrite(RGB_RED, red);
//   analogWrite(RGB_GREEN, green);
//   analogWrite(RGB_BLUE, blue);  
// }


void warn() {
  switch (warningLevel)
  {
    case 0:
      digitalWrite(LED_GREEN, HIGH);
      counter = 0;
      break;

    case 1:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      counter = 0;
      break;

    case 2:
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, HIGH);
      counter = 0;
      break;        

    case 3:
      digitalWrite(LED_RED, LOW);
      // Envoi du signal radio :
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
      delay(1000); 
      break;
  
    default:
      break;
  }
}





/* ======================================================================================== *\ 
|  ======================================================================> 4. LOOP
\* ======================================================================================== */ 

void loop() {

  counter = counter + 1;

  // ------------------------------------------------------------------------------------->
  
  /* 
    Si le système est éteint et que le bouton est appuyé, le système s'allume.
    Si le système est allumé et que le bouton et appuyez, le système s’éteint.
  */
  
  if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == false)
  {
    delay(ACTIVATION_DELAY);
    armSystem();
  }
  else if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == true)
  {
    disarmSystem();
    delay(ACTIVATION_DELAY);
  }

  // ------------------------------------------------------------------------------------->
  
  if (systemArmed) {
    for(int i = 0; i < INPUTS; i++)
    {
      if(analogRead(inputPins[i]) > THRESHOLD)
      {
        warn();
        delay(DETECTION_DELAY);
        warningLevel = warningLevel + 1;   
      }
    }
  }

  delay(10);
}
