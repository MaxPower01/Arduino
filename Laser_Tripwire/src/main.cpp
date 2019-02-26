#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Nécessaire seulement pour compiler le code



/* 
  1. Variables
  2. Setup
  3. Méthodes
  4. Loop
    4.1. Activation ou désactivation du fil de déclenchement au laser
    4.2. Lecture des données du capteur de lumière
    4.3. Déclenchement conditionnel de l'alarme
*/



/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

RH_ASK driver;

const int BUTTON_MASTER = 5;
const int BUTTON_SWITCH = 6;
bool buttonSwitchPressed = false;

const int LIGHT_SENSOR = A5;
int light;

const int LASER = 2;

bool systemArmed = false;
bool alarm = false;
int alarmLevel = 1;

// Niveau de sensibilité du capteur de lumière :
const int THRESHOLD = 100;





/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Initialisation de la communication radio
  if (!driver.init())
      Serial.println("init failed");

  // Définition du mode de chacune des pattes utilisées :
  pinMode(BUTTON_MASTER, INPUT);
  pinMode(BUTTON_SWITCH, INPUT);
  pinMode(LASER, OUTPUT);
  pinMode(LIGHT_SENSOR, INPUT);
}





/* ============================================================ *\ 
|  ==============> 3. Méthodes
\* ============================================================ */

void armSystem() {       
  digitalWrite(LASER, HIGH);
  delay(1000);
  systemArmed = true;
}

void disarmSystem() {
  systemArmed = false;
  digitalWrite(LASER, LOW);
  delay(1000);
}





/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  // 4.1. Activation ou désactivation du fil de déclenchement au laser :
  if (digitalRead(BUTTON_MASTER) == HIGH && systemArmed == false)
  {
    armSystem();
  }
  else if (digitalRead(BUTTON_MASTER) == HIGH && systemArmed == true)
  {
    disarmSystem();
  }

  // 4.2. Lecture des données du capteur de lumière :
  light = analogRead(LIGHT_SENSOR);
  Serial.println(light);

  // 4.3. Déclenchement conditionnel de l'alarme :
  if (systemArmed && analogRead(BUTTON_SWITCH) == LOW && light <= THRESHOLD);
  {
    const char *msg = "---Alarme---";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
  }
}