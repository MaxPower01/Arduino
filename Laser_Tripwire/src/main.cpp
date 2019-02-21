#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

// 1. Variables
// 2. Setup
// 3. Méthodes
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

RH_ASK driver;

const int BUTTON = 5;

const int LIGHT_SENSOR = A5;
int light;

const int LASER = 2;

bool systemArmed = false;

bool alarm = false;
int alarmLevel = 1;

// Moyenne d'échantillons :
const int SMOOTHNESS = 3;     // Nombre d'échantillons à prendre en compte
const int THRESHOLD = 100;  // Niveau de sensibilité de la détection

int readings[SMOOTHNESS];     // Lectures provenant des capteurs
int readIndex = 0;          // Index de la lecture en cours
int total = 0;              // Somme des échantillons
int average = 0;            // Moyenne des échantillons





/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialisation de la communication à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  if (!driver.init())
      Serial.println("init failed");

  // Définition du mode de chacune des pattes utilisées :
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);
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
  
  
  // Activation ou désactivation du fil de déclenchement au laser :
  if (digitalRead(BUTTON) == HIGH && systemArmed == false)
  {
    armSystem();
  }
  else if (digitalRead(BUTTON) == HIGH && systemArmed == true)
  {
    disarmSystem();
  }

  light = analogRead(LIGHT_SENSOR);
  Serial.println(light);

  if (light <= THRESHOLD && systemArmed)
  {
    const char *msg = "Hello World!";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
  }
}