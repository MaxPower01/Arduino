#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Nécessaire seulement pour compiler le code



/* 
  1. Variables
  2. Setup
  3. Méthodes
  4. Loop
    4.1. Activation ou désactivation du fil de déclenchement au laser
    4.2. Lecture des données des capteurs
    4.3. Déclenchement conditionnel de l'alarme
*/



/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

RH_ASK driver;

const int BUTTON_MASTER = 5;
const int BUTTON_SWITCH = 6;

bool switchPressed = false;

const int LIGHT_SENSOR = A5;
int light;

const int ROTATION_SENSOR = A1;
int rotation = 0;

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
  pinMode(ROTATION_SENSOR, INPUT);
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
}

void checkButtons() {
  if (digitalRead(BUTTON_MASTER) == HIGH && systemArmed == false)
  {
    armSystem();
  }
  else if (digitalRead(BUTTON_MASTER) == HIGH && systemArmed == true)
  {
    disarmSystem();
    delay(1000);
  }

  if (digitalRead(BUTTON_SWITCH) == HIGH) {
    switchPressed = true;
  } else {
    switchPressed = false;
  }
}

void checkRotation() {
  if (
    rotation >= 12 && rotation <= 20 ||
    rotation >= 62 && rotation <= 70 ||
    rotation >= 112 && rotation <= 120 ||
    rotation >= 169 && rotation <= 175 ||
    rotation >= 218 && rotation <= 226 ||
    rotation >= 268 && rotation <= 280 ||
    rotation >= 320 && rotation <= 330 ||
    rotation >= 370 && rotation <= 378 ||
    rotation >= 422 && rotation <= 430 ||
    rotation >= 457 && rotation <= 484 ||
    rotation >= 525 && rotation <= 535 ||
    rotation >= 575 && rotation <= 585 ||
    rotation >= 625 && rotation <= 635 ||
    rotation >= 675 && rotation <= 685 ||
    rotation >= 725 && rotation <= 735 ||
    rotation >= 780 && rotation <= 790 ||
    rotation >= 830 && rotation <= 840 ||
    rotation >= 882 && rotation <= 892 ||
    rotation >= 930 && rotation <= 940 ||
    rotation >= 985 && rotation <= 995
  )
  {
    Serial.println("rotationAccess");
  }
}





/* ============================================================ *\ 
|  ==============> 4. Loop
\* ============================================================ */ 

void loop() {
  const char *msg = "Hello world!";
  
  rotation = analogRead(ROTATION_SENSOR);
  
  checkButtons();

  while(systemArmed)
  {

    while (digitalRead(BUTTON_SWITCH) == LOW)
    {

      if (light < THRESHOLD)
      {
        digitalWrite(LASER, HIGH);
        delay(100);
      }
      
      light = analogRead(LIGHT_SENSOR);

      if (light <= THRESHOLD)
      {
        Serial.println("alarm");

        driver.send((uint8_t *)msg, strlen(msg));
        driver.waitPacketSent();
        delay(1000);
      }
    }
    while (digitalRead(BUTTON_SWITCH) == HIGH)
    {
      digitalWrite(LASER, LOW);
      light = 1000;
    }
  }
  

  
  

  // if (systemArmed)
  // {
  //   if (rotationAccess)
  //   {
  //     Serial.println("rotationAccess");
  //     if (digitalRead(BUTTON_SWITCH) == HIGH) {
  //       systemArmed = false;
  //     }
  //     else
  //     {
  //       systemArmed = true;
  //       if (light <= THRESHOLD)
  //       {
  //       }
  //     }
  //   }
  //   else if (light <= THRESHOLD)
  //   {
  //     driver.send((uint8_t *)msg, strlen(msg));
  //     driver.waitPacketSent();
  //     delay(1000);
  //   }
  // }
}
