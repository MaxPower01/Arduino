#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Nécessaire seulement pour compiler le code.



/* 
  1. VARIABLES
  2. SETUP
  3. MÉTHODES
  4. LOOP
*/



/* ======================================================================================== *\ 
|  ======================================================================> 1. VARIABLES
\* ======================================================================================== */ 

RH_ASK driver;

// const int LASER = 2;
const int LED_GREEN = 2;  // Debug
const int LED_BLUE = 10;  // Debug
const int BUTTON_SWITCH = 5;
const int BUTTON_ENABLE = 6;
const int ROTATION_SENSOR = A0;
const int LIGHT_SENSOR = A5;

const int ACTIVATION_DELAY = 1000;
const int LIGHT_THRESHOLD = 100;
const char *msg = "Hello world!";

bool buttonEnable = false;
bool systemArmed = false;
bool rotationAccess = false;

int counter = 0;
int light = 0;
int rotation = 0;





/* ======================================================================================== *\ 
|  ======================================================================> 2. SETUP
\* ======================================================================================== */ 

void setup() {
  // Initialisation de la communication via le port série à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!driver.init())
      Serial.println("init failed");

  pinMode(LED_BUILTIN, OUTPUT); // Degug only

  pinMode(BUTTON_SWITCH, INPUT);
  pinMode(BUTTON_ENABLE, INPUT);

  // pinMode(LASER, OUTPUT);
  pinMode(LED_GREEN, OUTPUT); // Degug only
  pinMode(LED_BLUE, OUTPUT);  // Degug only

  pinMode(LIGHT_SENSOR, INPUT);

  pinMode(ROTATION_SENSOR, INPUT);
}





/* ======================================================================================== *\ 
|  ======================================================================> 3. MÉTHODES
\* ======================================================================================== */

void armSystem() {       
  // digitalWrite(LASER, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  systemArmed = true;
  digitalWrite(LED_BUILTIN, HIGH);  // Degug only
}


void disarmSystem() {
  systemArmed = false;
  // digitalWrite(LASER, LOW);
  digitalWrite(LED_GREEN, LOW);     // Degug only
  digitalWrite(LED_BUILTIN, LOW);   // Degug only
}


void pauseSystem() {
  // digitalWrite(LASER, LOW);
  counter = 0;
  digitalWrite(LED_GREEN, LOW);     // Debug
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
    rotationAccess = true;
  }
  else
  {
    rotationAccess = false;
  }
  
}





/* ======================================================================================== *\ 
|  ======================================================================> 4. LOOP
\* ======================================================================================== */ 

void loop() {

  // ------------------------------------------------------------------------------------->

  /* 
    - Si le système est éteint et que le bouton est appuyé, le système s'allume.
    - Si le système est allumé et que le bouton et appuyez, le système s’éteint.
  */
  
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

  // ------------------------------------------------------------------------------------->
  
  /* 
    - Pendant que le système est armé, la variable "counter" incrémente de 1 à chaque boucle et les données du capteur de lumière sont lues.
    - Pendant que le bouton est appuyé, le système se met en pause et la variable “counter” est continuellement remise à 0.
    - Dès que le bouton est relâché, la variable “counter” recommence à s’incrémenter.
    - Le système d'alarme de plus de déclencher que lorsque la variable “counter” est supérieure à “ACTIVATION_DELAY”.
    - Cela permet de donner une certaine marge de manœuvre entre la réactivation du système lorsque le bouton n’est plus appuyé et le possible déclenchement de l’alarme.
    - L'alarme se déclenche dès que le bouton est relâché sans cette précaution.
  */

  if (systemArmed)
  {
    counter = counter + 1;

    rotation = analogRead(ROTATION_SENSOR);
    light = analogRead(LIGHT_SENSOR);

    digitalWrite(LED_GREEN, HIGH);  // Degug only

    checkRotation();

    // Mise en pause du système d'alarme :
    while(digitalRead(BUTTON_ENABLE) == HIGH && rotation >= 1010)
    {
      pauseSystem();
      delay(100);
    }

    // Déclenchement de l'alarme :
    if (light <= LIGHT_THRESHOLD && counter > ACTIVATION_DELAY)
    {
      digitalWrite(LED_BLUE, HIGH);  // Degug only
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
      delay(1000);
    }
  }
}