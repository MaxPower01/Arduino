// -------------------- FIL DE DÉCLENCHEMENT AU LASER -------------------- //

// Librairies :
#include <Arduino.h>
#include <VirtualWire.h>

// Pattes du Arduino :
const byte SWITCH = 2;
const byte LASER = 7;
const int RECEIVE_PIN = 11;
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

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;

// Virtual wire :
unsigned int vw_alarm, vw_value_2, vw_value_3, vw_value_4;
uint8_t vw_array[8];





void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LASER, OUTPUT);
  pinMode(LIGHT_SENSOR, INPUT);

  vw_set_rx_pin(RECEIVE_PIN);
  vw_set_tx_pin(TRANSMIT_PIN);
  vw_setup(2000);

  vw_alarm = 0;
  vw_value_2 = 0;
  vw_value_3 = 0;
  vw_value_4 = 0;
}





void updateVirtualWireArray() {
  vw_array[0] = (vw_alarm) >> 8;
  vw_array[1] = (vw_alarm) % 256;
  vw_array[2] = (vw_value_2) >> 8;
  vw_array[3] = (vw_value_2) % 256;
  vw_array[4] = (vw_value_3) >> 8;
  vw_array[5] = (vw_value_3) % 256;
  vw_array[6] = (vw_value_4) >> 8;
  vw_array[7] = (vw_value_4) % 256;
}


void sendRadioMessage() {
  updateVirtualWireArray();

  for(size_t i = 0; i < 3; i++)
  {
    vw_send((uint8_t *)vw_array, 8);
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
  vw_alarm = 0;
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
  vw_alarm = 1;
  sendRadioMessage();
  alarmTriggered = true;
}





void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint16_t vw_alarm, vw_value_2, vw_value_3, vw_value_4;

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