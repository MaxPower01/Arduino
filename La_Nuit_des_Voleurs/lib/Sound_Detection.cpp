// SYSTÈME DE DÉTECTION DU SON :

#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>

#define COMMON_ANODE

RH_ASK rh_driver;

// Pattes du Arduino :
const byte SOUND_SENSOR_1 = A0;
const byte SOUND_SENSOR_2 = A1;
const byte SOUND_SENSOR_3 = A4;
const byte SOUND_SENSOR_4 = A5;
const byte SWITCH = 2;

// Message à envoyer via le module RF 433Mhz :
const char *MESSAGE = "Hello world!";

// Délais et intervals utilisés dans le sketch :
const long DELAY_SYSTEM_ACTIVATION = 500;
const long DELAY_DETECTION = 1000;

// Capteurs de son :
const int SOUND_INPUTS = 4;
const byte SOUND_INPUT_PINS[SOUND_INPUTS] = {SOUND_SENSOR_1, SOUND_SENSOR_2, SOUND_SENSOR_3, SOUND_SENSOR_4};

// Seul de sensibilité de l'alarme :
const int THRESHOLD_SOUND = 10;

// Nombre de lectures des capteurs de son à prendre en compte pour établir une moyenne  :
const int NUMBER_OF_SOUND_READINGS = 10;

// Établissement d'une moyenne de lectures d'intensité du son :
int soundInputReadings[SOUND_INPUTS][NUMBER_OF_SOUND_READINGS];
int soundReadingIndex[SOUND_INPUTS] = {0, 0};
int soundInputsTotal[SOUND_INPUTS] = {0, 0};
int soundInputsAverage[SOUND_INPUTS] = {0, 0};

// État du système :
bool systemArmed = false;
bool systemPaused = false;

// Délai pour allumer le système :
unsigned long timeWhenOnSwitchTouched = 0;
unsigned long timeSinceOnSwitchTouched = 0;
bool onSwitchIsBeingTouched = false;

// Délai pour éteindre le système :
unsigned long timeWhenOffSwitchTouched = 0;
unsigned long timeSinceOffSwitchTouched = 0;
bool offSwitchIsBeingTouched = false;

// État de l'alarme :
bool alarmTriggered = false;
bool soundIsLouderThenThreshold = false;

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;





void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!rh_driver.init())
  {
    Serial.println("init failed");
  }

  pinMode(SOUND_SENSOR_1, INPUT);
  pinMode(SOUND_SENSOR_2, INPUT);
  pinMode(SOUND_SENSOR_3, INPUT);
  pinMode(SOUND_SENSOR_4, INPUT);
  pinMode(SWITCH, INPUT);

  // Initialisation de toutes les lectures d'échantillons de son à 0 :
  for (int i = 0; i < SOUND_INPUTS; i++) {
    for (int thisReading = 0; thisReading < NUMBER_OF_SOUND_READINGS; thisReading++) {
      soundInputReadings[i][thisReading] = 0;
    }
  }
}





void checkSystemSwitch() {
  // ---------------------------------------- ON
  // Pendant que le bouton est appuyé et que le système est éteint :
  if (systemArmed == false && digitalRead(SWITCH) == HIGH)
  {
    // Si le bouton a déjà été appuyé :
    if (!onSwitchIsBeingTouched)
    {
      // Prend en note le temps :
      timeWhenOnSwitchTouched = timeSinceProgramStarted;

      // Signal que le bouton a déjà été appuyé :
      onSwitchIsBeingTouched = true;
    }

    // Sinon :
    else
    {
      // Met à jour le temps écoulé depuis que le bouton a été appuyé la première fois :
      timeSinceOnSwitchTouched = timeSinceProgramStarted - timeWhenOnSwitchTouched;

      // Si le temps écoulé depuis est plus grand que le délai :
      if (timeSinceOnSwitchTouched >= DELAY_SYSTEM_ACTIVATION)
      {
        // Active le système :
        armSystem();

        // Réinitialise l'état du bouton en spécifiant qu'il n'a maintenant plus été appuyé :
        onSwitchIsBeingTouched = false;
      }
    }
  }

  // Dès que le bouton n'est plus appuyé, réinitialiser son état :
  else
  {
    onSwitchIsBeingTouched = false;
  }

  // ---------------------------------------- OFF (fonctionnement identique à "ON")
  if (systemArmed == true && digitalRead(SWITCH) == HIGH)
  {
    if (!offSwitchIsBeingTouched)
    {
      timeWhenOffSwitchTouched = timeSinceProgramStarted;
      offSwitchIsBeingTouched = true;
    }

    else
    {
      timeSinceOffSwitchTouched = timeSinceProgramStarted - timeWhenOffSwitchTouched;

      if (timeSinceOffSwitchTouched >= DELAY_SYSTEM_ACTIVATION)
      {
        disarmSystem();
        offSwitchIsBeingTouched = false;
      }
    }
  }
  
  else
  {
    offSwitchIsBeingTouched = false;
  }
}


void armSystem() {
  systemArmed = true;
}


void disarmSystem() {
  digitalWrite(LED_BUILTIN, LOW);
  systemArmed = false;
  alarmTriggered = false;
  soundIsLouderThenThreshold = false;
}


void smoothInputs() {
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


void checkSoundInputs() {
    // Pour chaque capteur de son :
    for(int i = 0; i < SOUND_INPUTS; i++)
    {
      // Si la moyenne des lectures est supérieure au seuil de sensibilité de l'alarme :
      if(analogRead(SOUND_INPUT_PINS[i]) > THRESHOLD_SOUND)
      {
        soundIsLouderThenThreshold = true;
      }
    }
}


void triggerAlarm() {
  digitalWrite(LED_BUILTIN, HIGH);
}


void sendRadioMessage() {
  rh_driver.send((uint8_t *)MESSAGE, strlen(MESSAGE));
  rh_driver.waitPacketSent();
}





void loop() {
  // Mise à jour du temps écoulé depuis le début du sketch :
  timeSinceProgramStarted = millis();

  // Lecture des données et établissement d'une moyenne par capteur de son : 
  smoothInputs();

  // Allumage/éteignage du système :
  checkSystemSwitch();
  
  // Si le système est allumé :
  if (systemArmed)
  {
    // Vérifie l'intensité du son capté :
    checkSoundInputs();

    // Si le est plus fort que le seul de sensibilité de l'alarme :
    if (soundIsLouderThenThreshold && !alarmTriggered)
    {
      // Déclenche l'alarme :
      triggerAlarm();
    }
  }
}