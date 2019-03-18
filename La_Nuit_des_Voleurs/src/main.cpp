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
const byte BUTTON_ENABLE = 12;
// const byte LED_GREEN = 5;
const byte LASER = 10;
const byte ROTATION_SENSOR = A1;
const byte LIGHT_SENSOR = A5;

const int DELAY_LASER = 100;
const long INTERVAL = 1000;

const int THRESHOLD_LIGHT = 100;
const int THRESHOLD_ROTATION = 500;

const char *msg = "Hello world!";   // Message à envoyer via le module RF 433Mhz.

unsigned long previousMillis = 0;

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

    pinMode(LED_BUILTIN, OUTPUT);     // Pin 13
    // pinMode(LED_GREEN, OUTPUT);
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
    systemArmed = true;
    Serial.println("armSystem");
    delay(1000);
}


void disarmSystem() {
    systemArmed = false;
    Serial.println("disarmSystem");
    delay(1000);
}


void pauseSystem() {
    systemPaused = true;
    digitalWrite(LASER, LOW);
    digitalWrite(LED_BUILTIN, LOW);
}


void triggerAlarm() {
    digitalWrite(LED_BUILTIN, HIGH); 
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
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
        armSystem();
    }
    // Éteignage du système :
    else if (digitalRead(BUTTON_SWITCH) == HIGH && systemArmed == true)
    {
        disarmSystem();
    }


    // Si le système est armé et que le bouton "pause" n'est pas appuyé :
    if (systemArmed && digitalRead(BUTTON_ENABLE) == LOW)
    {
        digitalWrite(LASER, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);

        /* if (light < THRESHOLD_LIGHT)
        {
            triggerAlarm();
        } */
    }

    // Si le bouton "pause" est appuyé pendant que le système est armé :
    else if (systemArmed && digitalRead(BUTTON_ENABLE) == HIGH)
    {
        pauseSystem();
    }
    

    // La LED sera toujours allumée ou éteinte selon l'état de la variable "ledGreenState". Cela permet d'éviter l'utilisation de "delay()" :
    // digitalWrite(LED_GREEN, ledGreenState);
}