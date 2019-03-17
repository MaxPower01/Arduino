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

const byte SWITCH = 2;
const byte LASER = 7;
const byte LIGHT_SENSOR = A5;

const int DELAY_LASER = 100;
const long INTERVAL = 1000;

const int THRESHOLD_LIGHT = 300;

const char *msg = "Hello world!";   // Message à envoyer via le module RF 433Mhz.

bool systemArmed = false;
bool systemPaused = false;

unsigned long counter = 0;
int light;
int rotation;

unsigned long systemOnMillis = 0;
unsigned long systemOffMillis = 0;
unsigned long ledMillis = 0;
int laserState = LOW;               // État de la LED (pour l'allumer et l'éteindre)
int ledState = LOW;               // État de la LED (pour l'allumer et l'éteindre)




/* ======================================================================================== *\ 
  |  ======================================================================> 2. SETUP
  \* ======================================================================================== */





void setup() {
  // Initialisation de la communication via le port série à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!driver.init())
    Serial.println("init failed");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH, INPUT);

  pinMode(LASER, OUTPUT);

  pinMode(LIGHT_SENSOR, INPUT);
}





/* ======================================================================================== *\ 
  |  ======================================================================> 3. MÉTHODES
  \* ======================================================================================== */





/* ======================================================================================== *\ 
  |  ======================================================================> 4. LOOP
  \* ======================================================================================== */

void loop() {
  // "millis()" est une fonction qui calcule le nombre de millisecondes écoulées depuis que le microcontrôleur a commencer l'exécution du sketch :
  // "unsigned" = Seulement des chiffres positifs :
  unsigned long currentMillis = millis();
  
  light = analogRead(LIGHT_SENSOR);
  // Serial.println(light);


  // Démarrage du système :
  if (digitalRead(SWITCH) == HIGH && systemArmed == false)
  {
    systemOnMillis = systemOnMillis + 1;
    Serial.println(systemOnMillis);
    
    if (systemOnMillis >= 250)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      systemArmed = true;
      systemOnMillis = 0;
      Serial.println('System on');
    }
  } else if (digitalRead(SWITCH) == HIGH && systemArmed == true)
  {
    systemOffMillis = systemOffMillis + 1;
    Serial.println(systemOffMillis);
    
    if (systemOffMillis >= 250)
    {
      digitalWrite(LED_BUILTIN, LOW);
      systemArmed = false;
      systemOffMillis = 0;
      Serial.println('System off');
    }
  }

  if (systemArmed)
  {
    digitalWrite(LASER, HIGH);
    
    if (light <= THRESHOLD_LIGHT)
    {
      Serial.println('ALARM');
      // driver.send((uint8_t *)msg, strlen(msg));
      // driver.waitPacketSent();
    }
  } else
  {
    digitalWrite(LASER, LOW);
  }
  

  /* if (currentMillis - ledMillis >= INTERVAL)
  {
    previousMillis = ledMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(LED_BUILTIN, ledState);
  } */
}