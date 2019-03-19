// -------------------- FIL DE DÉCLENCHEMENT AU LASER -------------------- //

// Librairies à inclure :
#include <Arduino.h>
#include <Timer.h>
#include <RH_ASK.h>
#include <SPI.h>

// Instances de classes :
RH_ASK rh_driver;
Timer t;

// Pattes du Arduino :
const byte SWITCH = 2;
const byte LASER = 7;
const byte LIGHT_SENSOR = A5;

// Capteur de lumière :
const int THRESHOLD_LIGHT = 300;

// Intensité de la lumière captée :
int light;

// Délais et intervals utilisés dans le sketch :
const long DELAY_SYSTEM_ACTIVATION = 500;
const long DELAY_TRIGGER_ALARM = 100;
const long INTERVAL = 1000;

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;

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
unsigned long timeWhenLightDropped = 0;
unsigned long timeSinceLightDropped = 0;
unsigned long lightHasDropped = false;

// État de l'alarme :
bool somethingTouchedTheLaser = false;
bool alarmTriggered = false;
unsigned long timeWhenAlarmWasTriggered = 0;

// Message à envoyer via le module RF 433Mhz :
const char *msg = "Hello world!";





void setup() {
  // Initialisation de la communication série :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!rh_driver.init())
  {
    Serial.println("init failed");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LASER, OUTPUT);
  pinMode(LIGHT_SENSOR, INPUT);
}





void sendRadioSignal() {
  // rh_driver.send((uint8_t *)msg, strlen(msg));
  // rh_driver.waitPacketSent();
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


void checkLightDetected() {
  // Cette fonction est très similaire à "checkSystemSwitch" :
  if (light <= THRESHOLD_LIGHT)
    {
      if (!lightHasDropped)
      {
        timeWhenLightDropped = timeSinceProgramStarted;
        lightHasDropped = true;
      }

      else
      {
        timeSinceLightDropped = timeSinceProgramStarted - timeWhenLightDropped;

        if (timeSinceLightDropped >= DELAY_TRIGGER_ALARM)
        {
          // Indique que l'alarme :
          somethingTouchedTheLaser = true;
          lightHasDropped = false;
          digitalWrite(LED_BUILTIN, HIGH);
        }
      }
    }

    else
    {
      lightHasDropped = false;
    }
}


void triggerAlarm() {
  digitalWrite(LED_BUILTIN, HIGH);
  sendRadioSignal();
  
  alarmTriggered = true;

  // Debug :
  Serial.println(timeSinceProgramStarted);
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
    // Vérifie la lumière captée :
    checkLightDetected();

    // Si quelque chose a touché le laser sans que l'alarme soit délà active :
    if (somethingTouchedTheLaser && !alarmTriggered)
    {
      // Déclenche l'alarme :
      triggerAlarm();
    }
  }
}