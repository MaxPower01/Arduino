// -------------------- FIL DE DÉCLENCHEMENT AU LASER (off switch, no keypad) -------------------- //

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
const byte LIGHT_SENSOR_1 = A1;
const byte LIGHT_SENSOR_2 = A2;
const byte LIGHT_SENSOR_3 = A4;
const byte LIGHT_SENSOR_4 = A5;

// Capteur de lumière :
const int THRESHOLD_LIGHT = 300;

// Intensité de la lumière captée :
int light1;
int light2;
int light3;
int light4;

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
unsigned long timeWhenLightInputDropped1 = 0;
unsigned long timeSinceLightInputDropped1 = 0;
unsigned long lightInputDropped1 = false;

unsigned long timeWhenLightInputDropped2 = 0;
unsigned long timeSinceLightInputDropped2 = 0;
unsigned long lightInputDropped2 = false;

unsigned long timeWhenLightInputDropped3 = 0;
unsigned long timeSinceLightInputDropped3 = 0;
unsigned long lightInputDropped3 = false;

unsigned long timeWhenLightInputDropped4 = 0;
unsigned long timeSinceLightInputDropped4 = 0;
unsigned long lightInputDropped4 = false;

// État de l'alarme :
bool somethingTouchedTheLaser1 = false;
bool somethingTouchedTheLaser2 = false;
bool somethingTouchedTheLaser3 = false;
bool somethingTouchedTheLaser4 = false;
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
  pinMode(LIGHT_SENSOR_1, INPUT);
  pinMode(LIGHT_SENSOR_2, INPUT);
  pinMode(LIGHT_SENSOR_3, INPUT);
  pinMode(LIGHT_SENSOR_4, INPUT);

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

  somethingTouchedTheLaser1 = false;
  somethingTouchedTheLaser2 = false;
  somethingTouchedTheLaser3 = false;
  somethingTouchedTheLaser4 = false;
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

void checkLightInput1()
{
  // Fonction presque identique à "checkSystemSwitch" :
  if (light1 <= THRESHOLD_LIGHT)
  {
    if (!lightInputDropped1)
    {
      timeWhenLightInputDropped1 = timeSinceProgramStarted;
      lightInputDropped1 = true;
    }
    else
    {
      timeSinceLightInputDropped1 = timeSinceProgramStarted - timeWhenLightInputDropped1;
      if (timeSinceLightInputDropped1 >= DELAY_TRIGGER_ALARM)
      {
        somethingTouchedTheLaser1 = true;
        lightInputDropped1 = false;
      }
    }
  }
  else
  {
    lightInputDropped1 = false;
  }
}
void checkLightInput2()
{
  // Fonction presque identique à "checkSystemSwitch" :
  if (light2 <= THRESHOLD_LIGHT)
  {
    if (!lightInputDropped2)
    {
      timeWhenLightInputDropped2 = timeSinceProgramStarted;
      lightInputDropped2 = true;
    }
    else
    {
      timeSinceLightInputDropped2 = timeSinceProgramStarted - timeWhenLightInputDropped2;
      if (timeSinceLightInputDropped2 >= DELAY_TRIGGER_ALARM)
      {
        somethingTouchedTheLaser2 = true;
        lightInputDropped2 = false;
      }
    }
  }
  else
  {
    lightInputDropped2 = false;
  }
}
void checkLightInput3()
{
  // Fonction presque identique à "checkSystemSwitch" :
  if (light3 <= THRESHOLD_LIGHT)
  {
    if (!lightInputDropped3)
    {
      timeWhenLightInputDropped3 = timeSinceProgramStarted;
      lightInputDropped3 = true;
    }
    else
    {
      timeSinceLightInputDropped3 = timeSinceProgramStarted - timeWhenLightInputDropped3;
      if (timeSinceLightInputDropped3 >= DELAY_TRIGGER_ALARM)
      {
        somethingTouchedTheLaser3 = true;
        lightInputDropped3 = false;
      }
    }
  }
  else
  {
    lightInputDropped3 = false;
  }
}
void checkLightInput4()
{
  // Fonction presque identique à "checkSystemSwitch" :
  if (light4 <= THRESHOLD_LIGHT)
  {
    if (!lightInputDropped4)
    {
      timeWhenLightInputDropped4 = timeSinceProgramStarted;
      lightInputDropped4 = true;
    }
    else
    {
      timeSinceLightInputDropped4 = timeSinceProgramStarted - timeWhenLightInputDropped4;
      if (timeSinceLightInputDropped4 >= DELAY_TRIGGER_ALARM)
      {
        somethingTouchedTheLaser4 = true;
        lightInputDropped4 = false;
      }
    }
  }
  else
  {
    lightInputDropped4 = false;
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
  light1 = analogRead(LIGHT_SENSOR_1);
  light2 = analogRead(LIGHT_SENSOR_2);
  light3 = analogRead(LIGHT_SENSOR_3);
  light4 = analogRead(LIGHT_SENSOR_4);

  // Mise à jour du temps écoulé depuis le début du sketch :
  timeSinceProgramStarted = millis();

  // Allumage/éteignage du système :
  checkSystemSwitch();

  // Si le système est allumé et que l'alarme n'est pas déclenchée, vérifie l'intensité de la lumière captée :
  if (systemArmed && !alarmTriggered)
  {
    checkLightInput1();
    checkLightInput2();
    checkLightInput3();
    checkLightInput4();

    // Si aucun des capteurs de lumière ne reçoit de lumière, déclenche l'alarme :
    if (somethingTouchedTheLaser1 && somethingTouchedTheLaser2 && somethingTouchedTheLaser3 && somethingTouchedTheLaser4)
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
        somethingTouchedTheLaser1 = false;
        somethingTouchedTheLaser2 = false;
        somethingTouchedTheLaser3 = false;
        somethingTouchedTheLaser4 = false;
      }
    }
  }
}