/* 
  ALARME :

  ...
*/


/* 
  1. VARIABLES
  2. SETUP
  3. MÉTHODES
  4. LOOP
*/


/* ======================================================================================== *\ 
|  ======================================================================> 1. VARIABLES
\* ======================================================================================== */ 

#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

const int BUZZER_1 = 5;
const int BUZZER_2 = 6;

bool alarmSound = false;





/* ======================================================================================== *\ 
|  ======================================================================> 2. SETUP
\* ======================================================================================== */ 
void setup() {
  Serial.begin(9600); // Debugging only

  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);
  
  if (!driver.init()) {
    Serial.println("init failed");  
  }
}





/* ======================================================================================== *\ 
|  ======================================================================> 3. MÉTHODES
\* ======================================================================================== */

// ...





/* ======================================================================================== *\ 
|  ======================================================================> 4. LOOP
\* ======================================================================================== */

void loop() {
  uint8_t buf[12];
  uint8_t buflen = sizeof(buf);

  // Non-blocking
  if (driver.recv(buf, &buflen)) {
    int i;

    // Message with a good checksum received, dump it.
    Serial.print("Message: ");
    Serial.println((char*)buf);

    alarmSound = true;
  }

  if (alarmSound == true) {
    digitalWrite(BUZZER_1, HIGH);
    digitalWrite(BUZZER_2, HIGH);
    delay(500);
    digitalWrite(BUZZER_1, LOW);
    digitalWrite(BUZZER_2, LOW);
    delay(500);
  }
}