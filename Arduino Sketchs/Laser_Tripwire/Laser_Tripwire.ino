#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Nécessaire seulement pour compiler le code.



/* 
  1. Variables
  2. Setup
  3. Méthodes
  4. Loop
*/



/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

RH_ASK driver;

const int LASER = 2;
const int BUTTON_SWITCH = 5;
const int BUTTON_ENABLE = 6;
const int ROTATION_SENSOR = A1;
const int LIGHT_SENSOR = A5;

const int LIGHT_THRESHOLD = 100;
const char *msg = "Hello world!";

bool buttonEnable = false;
bool systemArmed = false;
bool rotationAccess = false;

int light = 0;
int rotation = 0;





/* ============================================================ *\ 
|  ==============> 2. Setup
\* ============================================================ */ 

void setup() {
  // Initialisation de la communication via le port série à un débit de 9600 bits/seconde.
  Serial.begin(9600);

  // Initialisation de la communication radio.
  if (!driver.init())
      Serial.println("init failed");

  pinMode(LED_BUILTIN, OUTPUT); // Degug only

  pinMode(BUTTON_SWITCH, INPUT);
  pinMode(BUTTON_ENABLE, INPUT);

  pinMode(LASER, OUTPUT);

  pinMode(LIGHT_SENSOR, INPUT);

  pinMode(ROTATION_SENSOR, INPUT);
}





/* ============================================================ *\ 
|  ==============> 3. Méthodes
\* ============================================================ */

void armSystem() {       
  digitalWrite(LED_BUILTIN, HIGH);  // Degug only
  digitalWrite(LASER, HIGH);
  systemArmed = true;
}


void disarmSystem() {
  digitalWrite(LED_BUILTIN, LOW);  // Degug only
  systemArmed = false;
  digitalWrite(LASER, LOW);
}


void pauseSystem() {
  digitalWrite(LED_BUILTIN, LOW);  // Degug only
  light = LIGHT_THRESHOLD * 2;
  digitalWrite(LASER, LOW);
}


void checkButtons() {
  // Lorsque le bouton interrupteur est appuyé, le système s'active.

}


void checkSystem() {
  if (systemArmed)
  {
    if (buttonEnable)
    {
      // Désactive le système.
    }
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
  // Vérification de l'état du bouton #1.
  if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == false)
  {
    armSystem();
    delay(1000);
  }
  else if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == true)
  {
    disarmSystem();
    delay(1000);
  }

  if (systemArmed)
  {
    digitalWrite(LED_BUILTIN, HIGH);  // Degug only

    while(digitalRead(BUTTON_ENABLE) == HIGH)
    {
      pauseSystem();
    }
  }

  // if (systemArmed)
  // {
  //   rotation = analogRead(ROTATION_SENSOR);
  //   light = analogRead(LIGHT_SENSOR);

  //   if (light <= LIGHT_THRESHOLD)
  //   {
  //     Serial.println("Alarm");

  //     driver.send((uint8_t *)msg, strlen(msg));
  //     driver.waitPacketSent();
  //     delay(1000);
  //   }
  // }
}
