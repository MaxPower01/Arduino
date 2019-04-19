// -------------------- FIL DE DÉCLENCHEMENT AU LASER -------------------- //

#include <Arduino.h>
#include <VirtualWire.h>
#include <Keypad.h>
#include <Password.h>

// Pattes du Arduino :
const byte LASER = 2;
const byte LED_GREEN = 1;
const byte LED_RED = 0;
const int VW_TRANSMIT_PIN = 12;
const byte SWITCH = 13;
const byte LIGHT_SENSOR = A5;

// Capteur de lumière :
const int THRESHOLD_LIGHT = 300;

// Intensité de la lumière captée :
int light;

// Délais et intervals utilisés dans le sketch :
const long DELAY_SYSTEM_ACTIVATION = 250;
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
bool systemArmed = true;
bool systemPaused = false;

// Délai pour déclencher l'alarme :
unsigned long timeWhenLightInputDropped = 0;
unsigned long timeSinceLightInputDropped = 0;
unsigned long lightInputDropped = false;

// État de l'alarme :
bool somethingTouchedTheLaser = false;
bool alarmTriggered = false;
unsigned long timeWhenAlarmWasTriggered = 0;
unsigned long timeSinceAlarmWasTriggered = 0;
bool alarmHasBeenTriggered = false;

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;

// Virtual wire (s = send) :
unsigned int vw_s_alarm, vw_s_value_2, vw_s_value_3, vw_s_value_4;
uint8_t vw_s_array[8];

void setup()
{
  // Serial.begin(9600);

  // Pattes du Arduino :
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LASER, OUTPUT);
  pinMode(LIGHT_SENSOR, INPUT);

  // Modules radio :
  vw_set_tx_pin(VW_TRANSMIT_PIN);
  vw_setup(2000);

  // Valeures envoyées via le module radio :
  vw_s_alarm = 0;
  vw_s_value_2 = 0;
  vw_s_value_3 = 0;
  vw_s_value_4 = 0;

  digitalWrite(LASER, HIGH);
}

void updateVwArray()
{
  vw_s_array[0] = (vw_s_alarm) >> 8;
  vw_s_array[1] = (vw_s_alarm) % 256;
  vw_s_array[2] = (vw_s_value_2) >> 8;
  vw_s_array[3] = (vw_s_value_2) % 256;
  vw_s_array[4] = (vw_s_value_3) >> 8;
  vw_s_array[5] = (vw_s_value_3) % 256;
  vw_s_array[6] = (vw_s_value_4) >> 8;
  vw_s_array[7] = (vw_s_value_4) % 256;
}

void sendVwArray()
{
  updateVwArray();

  for (size_t i = 0; i < 10; i++)
  {
    vw_send((uint8_t *)vw_s_array, 8);
    vw_wait_tx();
    delay(200);
  }
}

void armSystem()
{
  digitalWrite(LASER, HIGH);
  systemArmed = true;
}

void disarmSystem()
{
  digitalWrite(LASER, LOW);

  somethingTouchedTheLaser = false;
  systemArmed = false;
}

void checkSystemSwitch()
{
  // Pendant que le bouton est appuyé, que le système est éteint et que le mot de passe a été entré correctement,
  // si le bouton n'a pas déjà été appuyé, prend en note le temps et signal que le bouton a déjà été appuyé :
  if (digitalRead(SWITCH) == HIGH)
  {
    if (!onSwitchIsBeingTouched)
    {
      timeWhenOnSwitchTouched = timeSinceProgramStarted;
      onSwitchIsBeingTouched = true;
    }
    // Met à jour le temps écoulé depuis que le bouton a été appuyé la première fois.
    // Si le temps écoulé depuis est plus grand que le délai, désactive le système :
    else
    {
      timeSinceOnSwitchTouched = timeSinceProgramStarted - timeWhenOnSwitchTouched;
      if (timeSinceOnSwitchTouched >= DELAY_SYSTEM_ACTIVATION)
      {
        systemArmed = false;
        digitalWrite(LASER, LOW);
      }
    }
  }
  // Dès que le bouton n'est plus appuyé, réinitialiser son état et réactive le système :
  else
  {
    onSwitchIsBeingTouched = false;
    systemArmed = true;
    digitalWrite(LASER, HIGH);
  }
}

void checkLightInput()
{
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

void triggerAlarm()
{
  vw_s_alarm = 1;
  sendVwArray();
  alarmTriggered = true;
  // Serial.print("ALARM");
}

void loop()
{
  // Lecture des données du capteur de lumière :
  light = analogRead(LIGHT_SENSOR);

  // Mise à jour du temps écoulé depuis le début du sketch :
  timeSinceProgramStarted = millis();

  // Allumage/éteignage du système :
  checkSystemSwitch();

  // Si le système est allumé et que l'alarme n'est pas déclenchée, vérifie l'intensité de la lumière captée :
  if (systemArmed && !alarmTriggered)
  {
    checkLightInput();

    // Si quelque chose a touché le laser déclenche l'alarme :
    if (somethingTouchedTheLaser)
    {
      triggerAlarm();
    }
  }
  else if (alarmTriggered)
  {
    if (!alarmHasBeenTriggered)
    {
      timeWhenAlarmWasTriggered = timeSinceProgramStarted;
      alarmHasBeenTriggered = true;
    }
    else
    {
      timeSinceAlarmWasTriggered = timeSinceProgramStarted - timeWhenAlarmWasTriggered;
      if (timeSinceAlarmWasTriggered > 10000)
      {
        alarmHasBeenTriggered = false;
        alarmTriggered = false;
        somethingTouchedTheLaser = false;
      }
    }
  }
}