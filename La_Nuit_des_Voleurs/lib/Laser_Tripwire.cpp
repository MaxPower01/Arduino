// -------------------- FIL DE DÉCLENCHEMENT AU LASER -------------------- //

// Librairies à inclure :
#include <Arduino.h>
// #include <RH_ASK.h>
// #include <SPI.h>
#include <VirtualWire.h>

// Instances de classes :
// RH_ASK rh_driver;

// Pattes du Arduino :
const byte SWITCH = 2;
const byte LASER = 7;
const int TRANSMIT_PIN = 12;
const byte LIGHT_SENSOR = A5;

// Capteur de lumière :
const int THRESHOLD_LIGHT = 300;

// Intensité de la lumière captée :
int light;

// Délais et intervals utilisés dans le sketch :
const long DELAY_SYSTEM_ACTIVATION = 500;
const long DELAY_TRIGGER_ALARM = 100;
const long INTERVAL = 1000;

// Délai pour allumer le système :
unsigned long timeWhenOnSwitchTouched = 0;
unsigned long timeSinceOnSwitchTouched = 0;
bool onSwitchIsBeingTouched = false;

// Délai pour éteindre le système :
unsigned long timeWhenOffSwitchTouched = 0;
unsigned long timeSinceOffSwitchTouched = 0;
bool offSwitchIsBeingTouched = false;

// État du système :
bool systemArmed = false;
bool systemPaused = false;

// Délai pour déclencher l'alarme :
unsigned long timeWhenLightInputDropped = 0;
unsigned long timeSinceLightInputDropped = 0;
unsigned long lightInputDropped = false;

// État de l'alarme :
bool somethingTouchedTheLaser = false;
bool alarmTriggered = false;
unsigned long timeWhenAlarmWasTriggered = 0;

// Message à envoyer via le module RF 433Mhz :
const char *MESSAGE = "Hello world!";

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;

// Test Virtual wire :
unsigned int setAlarm;
unsigned int test;
uint8_t radioSignalArray[4];





void setup() {
  // Initialisation de la communication série :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  // if (!rh_driver.init())
  // {
  //   Serial.println("init failed");
  // }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LASER, OUTPUT);
  pinMode(LIGHT_SENSOR, INPUT);

  // Test Virtual Wire :
  vw_set_tx_pin(TRANSMIT_PIN);
  vw_setup(2000);	 // Bits per sec

  setAlarm = 0;
  test = 0;
}





void sendRadioMessage() {
  // rh_driver.send((uint8_t *)MESSAGE, strlen(MESSAGE));
  // rh_driver.waitPacketSent();

  updateRadioSignalArray();

  for(size_t i = 0; i < 3; i++)
  {
    vw_send((uint8_t *)radioSignalArray, 4);
    vw_wait_tx();
    delay(200);
  }
}


void armSystem() {
  digitalWrite(LASER, HIGH);
  systemArmed = true;
}


void disarmSystem() {
  digitalWrite(LASER, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  
  somethingTouchedTheLaser = false;
  alarmTriggered = false;
  systemArmed = false;
  setAlarm = 0;
  sendRadioMessage();
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


void checkLightInput() {
  // Fonction presque identique à "checkSystemSwitch" :
  if (light <= THRESHOLD_LIGHT)
    {
      if (!lightInputDropped)
      {
        timeWhenLightInputDropped = timeSinceProgramStarted;
        lightInputDropped = true;
      }

      else
      {
        timeSinceLightInputDropped = timeSinceProgramStarted - timeWhenLightInputDropped;

        if (timeSinceLightInputDropped >= DELAY_TRIGGER_ALARM)
        {
          somethingTouchedTheLaser = true;
          lightInputDropped = false;
        }
      }
    }

    else
    {
      lightInputDropped = false;
    }
}


void triggerAlarm() {
  digitalWrite(LED_BUILTIN, HIGH);
  setAlarm = 1;
  sendRadioMessage();
  alarmTriggered = true;
}

void updateRadioSignalArray() {
  radioSignalArray[0] = (setAlarm) >> 8;
  radioSignalArray[1] = (setAlarm) % 256;
  radioSignalArray[2] = (test) >> 8;
  radioSignalArray[3] = (test) % 256;
}





void loop() {
  // Lecture des données du capteur de lumière :
  light = analogRead(LIGHT_SENSOR);

  // Mise à jour du temps écoulé depuis le début du sketch :
  timeSinceProgramStarted = millis();

  // Allumage/éteignage du système :
  checkSystemSwitch();

  // Si le système est allumé :
  if (systemArmed)
  {
    // Vérifie l'intensité de la lumière captée :
    checkLightInput();

    // Si quelque chose a touché le laser sans que l'alarme soit délà active :
    if (somethingTouchedTheLaser && !alarmTriggered)
    {
      // Déclenche l'alarme :
      triggerAlarm();
    }
  }
}