// -------------------- ALARME -------------------- //

#include <Arduino.h>
#include <VirtualWire.h>

// Pattes du Arduino :
const int RECEIVE_PIN = 11;
const byte BUZZER_1 = 2;
const byte BUZZER_2 = 3;

// État de l'alarme :
bool alarm = false;

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;

// Interval pour le son du buzzer :
const int INTERVAL_BUZZER = 500;
unsigned long timeWhenAlarmWasActivated = 0;
unsigned long timeSinceAlarmIsOn = 0;
unsigned long timeSinceBuzzerIsOn = 0;
bool alarmIsOn = false;
bool buzzerState = LOW;

// Données à recevoir via le module RF 433Mhz :
int setAlarm, test;





void setup() {
  // Initialisation de la communication série :
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);

  // Virtual Wire :
  vw_set_tx_pin(RECEIVE_PIN);
  vw_setup(2000);

  // Start the receiver PLL running
  vw_rx_start();

  setAlarm = 0;
  test = 0;
}





void loop() {
  timeSinceProgramStarted = millis();

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint16_t setAlarm, test;

  if (vw_get_message(buf, &buflen)) {
    //Données de type uint8 reçues, conversion en type uint16 :
    if (buflen == 4) {
      setAlarm =  buf[0]; setAlarm = (setAlarm << 8) +  buf[1];
      test =  buf[2]; test = (test << 8) +  buf[3];
    }

    Serial.println(setAlarm);

    if (setAlarm == 1) {
      alarm = true;
    }

    else if (setAlarm == 0) {
      alarm = false;
      alarmIsOn = false;
      digitalWrite(BUZZER_1, LOW);
    }
  }

  if (alarm) {
    if (!alarmIsOn) {
      timeWhenAlarmWasActivated = timeSinceProgramStarted;
      alarmIsOn = true;
    }

    else {
      timeSinceAlarmIsOn = timeSinceProgramStarted - timeWhenAlarmWasActivated;

      if (timeSinceAlarmIsOn - timeSinceBuzzerIsOn >= INTERVAL_BUZZER) {
        timeSinceBuzzerIsOn = timeSinceAlarmIsOn;

        if (buzzerState == LOW) {
          buzzerState = HIGH;
        } else {
          buzzerState = LOW;
        }

        digitalWrite(BUZZER_1, buzzerState);
        digitalWrite(LED_BUILTIN, buzzerState);
      }
    }
  }
}