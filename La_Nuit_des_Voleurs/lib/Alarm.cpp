// -------------------- ALARME -------------------- //

#include <Arduino.h>
#include <VirtualWire.h>

// Pattes du Arduino :
const byte BUZZER_1 = 2;
const byte BUZZER_2 = 3;
const int RECEIVE_PIN = 11;
const int TRANSMIT_PIN = 12;

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

// Virtual wire :
unsigned int vw_r_alarm, vw_r_value_2, vw_r_value_3, vw_r_value_4;
uint8_t vw_s_array[8];





void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);

  vw_set_rx_pin(RECEIVE_PIN);
  vw_setup(2000);
  vw_rx_start();
}





void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint16_t vw_r_alarm, vw_r_value_2, vw_r_value_3, vw_r_value_4;
  
  timeSinceProgramStarted = millis();

  if (vw_get_message(buf, &buflen)) {
    //Données de type uint8 reçues, conversion en type uint16 :
    if (buflen == 8) {
      vw_r_alarm =  buf[0]; vw_r_alarm = (vw_r_alarm << 8) +  buf[1];
      vw_r_value_2 =  buf[2]; vw_r_value_2 = (vw_r_value_2 << 8) +  buf[3];
      vw_r_value_3 =  buf[4]; vw_r_value_3 = (vw_r_value_3 << 8) +  buf[5];
      vw_r_value_4 =  buf[6]; vw_r_value_4 = (vw_r_value_4 << 8) +  buf[7];
    }

    Serial.println(vw_r_alarm);

    if (vw_r_alarm == 1) {
      alarm = true;
    }

    else if (vw_r_alarm == 0) {
      alarm = false;
      alarmIsOn = false;
      digitalWrite(BUZZER_1, LOW);
      digitalWrite(LED_BUILTIN, LOW);
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