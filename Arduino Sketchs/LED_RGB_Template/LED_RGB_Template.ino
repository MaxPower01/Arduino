#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define COMMON_ANODE

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

// LED RGB Common Anode
const int PIN_RED = 9;
const int PIN_GREEN = 10;
const int PIN_BLUE = 11;

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
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  
  pinMode(BUTTON, INPUT);

  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  resetReadings();

  if (!driver.init())
         Serial.println("init failed");

  setColor(0, 0, 0);
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



void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(PIN_RED, red);
  analogWrite(PIN_GREEN, green);
  analogWrite(PIN_BLUE, blue);  
}



void warn() {


  Serial.println(warningLevel);

  switch (warningLevel)
      {
        case 0:
          setColor(0, 0, 0);  // off
          break;

        case 1:
          setColor(255, 255, 0);  // yellow
          const char *msg = "Hello World!";
          driver.send((uint8_t *)msg, strlen(msg));
          driver.waitPacketSent();
          delay(1000);
          break;
        
        case 2:
          setColor(80, 0, 80);  // purple
          break;

        case 3:
          setColor(255, 0, 0);  // red
          break;

        case 4:
          digitalWrite(PIN_RED, LOW); 
          digitalWrite(PIN_GREEN, LOW); 
          digitalWrite(PIN_BLUE, LOW); 
          
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
        Serial.println(warningLevel);
        warn();
                return;
      }
    }
  }
  

  // smoothing();

  // watchSamples();

  delay(10);
}
