// SYSTÈME DE DÉTECTION DU SON :

#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>

#define COMMON_ANODE

RH_ASK rh_driver;

const int SOUND_SENSOR_1 = A0;
const int SOUND_SENSOR_2 = A1;
const int SOUND_SENSOR_3 = A4;
const int SOUND_SENSOR_4 = A5;

const byte SWITCH = 2;

const int LED_GREEN = 10;
const int LED_YELLOW = 9;
const int LED_RED = 8;

// Message à envoyer via le module RF 433Mhz :
const char *MESSAGE = "Hello world!";

const int DELAY_ACTIVATION = 1000;
const int DELAY_DETECTION = 1000;

const int SOUND_INPUTS = 4;
const byte SOUND_INPUT_PINS[SOUND_INPUTS] = {SOUND_SENSOR_1, SOUND_SENSOR_2, SOUND_SENSOR_3, SOUND_SENSOR_4};

const int THRESHOLD_SOUND = 2;
const int NUMBER_OF_SOUND_READINGS = 20;

int soundInputReadings[SOUND_INPUTS][NUMBER_OF_SOUND_READINGS];
int soundReadingIndex[SOUND_INPUTS] = {0, 0};
int soundInputsTotal[SOUND_INPUTS] = {0, 0};
int soundInputsAverage[SOUND_INPUTS] = {0, 0};

bool systemArmed = false;





void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  if (!rh_driver.init())
  {
    Serial.println("init failed");
  }

  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);

  pinMode(SWITCH, INPUT);

  // Initialisation de toutes les lectures d'échantillons à 0 :
  resetReadings();

}


void resetReadings() {
  for (int i = 0; i < SOUND_INPUTS; i++) {
    for (int thisReading = 0; thisReading < NUMBER_OF_SOUND_READINGS; thisReading++) {
      soundInputReadings[i][thisReading] = 0;
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
  // Pour chaque capteur de son :
  for (int i = 0; i < SOUND_INPUTS; i++) {
    // Soustrait la dernière lecture du total :
    soundInputsTotal[i] = soundInputsTotal[i] - soundInputReadings[i][soundReadingIndex[i]];

    // Lis la donnée provenant du capteur :
    soundInputReadings[i][soundReadingIndex[i]] = analogRead(SOUND_INPUT_PINS[i]);

    // Ajoute cet échantillon au total :
    soundInputsTotal[i] = soundInputsTotal[i] + soundInputReadings[i][soundReadingIndex[i]];

    // Passe à la position suivante dans la liste d'échantillons :
    soundReadingIndex[i] = soundReadingIndex[i] + 1;
    
    // Une fois arrivé à la fin de la liste, recommence :
    if (soundReadingIndex[i] >= NUMBER_OF_SOUND_READINGS) {
      soundReadingIndex[i] = 0;
    }

    // Calcul la moyenne des échantillons de ce capteur :
    soundInputsAverage[i] = soundInputsTotal[i] / NUMBER_OF_SOUND_READINGS;
  }
}


void watchSamples() {
  for (int i = 0; i < SOUND_INPUTS; i++) {
    if (soundInputsAverage[i] >= THRESHOLD_SOUND && systemArmed) {
      // Alarm     
    }
  }
}





void loop() {

  counter = counter + 1;

  // ------------------------------------------------------------------------------------->
  
  /* 
    Si le système est éteint et que le bouton est appuyé, le système s'allume.
    Si le système est allumé et que le bouton et appuyez, le système s’éteint.
  */
  
  if (digitalRead(SWITCH) == HIGH && systemArmed == false)
  {
    delay(DELAY_ACTIVATION);
    armSystem();
  }
  else if (digitalRead(SWITCH) == HIGH && systemArmed == true)
  {
    disarmSystem();
    delay(DELAY_ACTIVATION);
  }

  // -------------------------------------------------------------------------------------> 
  
  if (systemArmed) {
    for(int i = 0; i < SOUND_INPUTS; i++)
    {
      if(analogRead(SOUND_INPUT_PINS[i]) > THRESHOLD_SOUND)
      {
        warn();
        delay(DELAY_DETECTION);
        warningLevel = warningLevel + 1;   
      }
    }
  }

  delay(10);
}