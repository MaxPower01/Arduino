// ALARME :

#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK rh_driver;

// Pattes du Arduino :
const byte BUZZER_1 = 2;
const byte BUZZER_2 = 3;

// État de l'alarme :
bool alarmTriggered = false;

// Temps depuis que le sketch a démarré :
unsigned long timeSinceProgramStarted;





void setup() {
  // Initialisation de la communication série :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!rh_driver.init())
  {
    Serial.println("init failed");  
  }

  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);
}





void triggerAlarm() {
  alarmTriggered = true;
}





void loop() {
  uint8_t buf[12];
  uint8_t buflen = sizeof(buf);

  // S'il y a réception d'un message radio :
  if (rh_driver.recv(buf, &buflen)) {
    int i;

    // Serial.print("Message: ");
    // Serial.println((char*)buf);

    triggerAlarm();
  }

  if (alarmTriggered)
  {
    digitalWrite(BUZZER_1, HIGH);
    digitalWrite(BUZZER_2, HIGH);
    delay(500);
    digitalWrite(BUZZER_1, LOW);
    digitalWrite(BUZZER_2, LOW);
    delay(500);
  }
}