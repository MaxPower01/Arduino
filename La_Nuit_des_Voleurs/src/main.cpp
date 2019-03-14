/* 
  FIL DE DÉCLENCHEMENT AU LASER :

  Le microcontrôleur gère un capteur de lumière et un laser qui pointe vers ce capteur.
  Pour allumer ou éteindre le système (donc, le laser), il faut appuyer sur un bouton.

  Lorsque le faisceau du laser est bloqué et que le capteur de lumière ne reçoit plus assez de lumière, un signal radio est émis grâce au module RF 433Mhz.
  Le signal radio est alors capté par un autre microcontrôleur qui déclenche ensuite l'alarme.

  Un autre bouton permet d'éteindre temporairement le laser afin de pouvoir traverser la zone qu'il séparait.
  Ce bouton n'est fonctionnel que lorsque des capteurs de rotation sont dans des positions précises.
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
#include <SPI.h> // Nécessaire seulement pour compiler le code.

RH_ASK driver;  // Communication radio module RF 433Mhz.

const byte BUTTON_SWITCH = 2;
const byte BUTTON_ENABLE = 3;
const byte LED_GREEN = 5;
const byte LASER = 10;
const byte ROTATION_SENSOR = A1;
const byte LIGHT_SENSOR = A5;

const int DELAY_LASER = 100;
const long INTERVAL = 1000;

const int THRESHOLD_LIGHT = 100;
const int THRESHOLD_ROTATION = 500;

const char *msg = "Hello world!";   // Message à envoyer via le module RF 433Mhz.

bool systemArmed = false;
bool systemPaused = false;
bool rotationAccess = false;

int counter = 0;
int light;
int rotation;

int ledGreenState = LOW;    // État de la LED (pour l'allumer et l'éteindre)




/* ======================================================================================== *\ 
|  ======================================================================> 2. SETUP
\* ======================================================================================== */ 





void setup() {
  // Initialisation de la communication via le port série à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!driver.init())
      Serial.println("init failed");

  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUTTON_SWITCH, INPUT);
  pinMode(BUTTON_ENABLE, INPUT);

  pinMode(LASER, OUTPUT);

  pinMode(LIGHT_SENSOR, INPUT);

  pinMode(ROTATION_SENSOR, INPUT);
}





/* ======================================================================================== *\ 
|  ======================================================================> 3. MÉTHODES
\* ======================================================================================== */

void armSystem() {       
    digitalWrite(LASER, HIGH);
    systemArmed = true;
}


void disarmSystem() {
    systemArmed = false;
    digitalWrite(LASER, LOW);
}


void pauseSystem() {
    systemPaused = true;
    digitalWrite(LASER, LOW);
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
    // "millis()" est une fonction qui calcule le nombre de millisecondes écoulées depuis que le microcontrôleur a commencer l'exécution du sketch :
    unsigned long currentMillis = millis();

    // Assignation des données du capteur de lumière :
    light = analogRead(LIGHT_SENSOR);

    // Démarrage du système :
    if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == false)
    {
        delay(1000);
        armSystem();
    }
    else if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == true)
    {
        delay(1000);
        disarmSystem();
    }

    // Une fois que le système est armé...
    if (systemArmed)
    {
        counter = counter + 1;

        if (digitalRead(BUTTON_ENABLE) == HIGH)
        {
            pauseSystem();
        }
        else if (!systemPaused && light <= LIGHT_THRESHOLD)
        {
            digitalWrite(LED_GREEN, HIGH); 
            driver.send((uint8_t *)msg, strlen(msg));
            driver.waitPacketSent();
            delay(1000);
        }
    }

    // La LED sera toujours allumée ou éteinte selon l'état de la variable "ledGreenState". Cela permet d'éviter l'utilisation de "delay()" :
    digitalWrite(LED_GREEN, ledGreenState);
}