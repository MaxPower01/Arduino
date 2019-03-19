// -------------------- FIL DE DÉCLENCHEMENT AU LASER -------------------- //

#include <Arduino.h>
#include <Timer.h>
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rh_driver;
Timer t;

// Pattes du Arduino :
const byte SWITCH = 2;
const byte LASER = 7;
const byte LIGHT_SENSOR = A5;

const int THRESHOLD_LIGHT = 300;
int light;

bool systemArmed = false;
bool systemPaused = false;

const long SYSTEM_ACTIVATION_DELAY = 500;
const long INTERVAL = 1000;

unsigned long timeSinceProgramStarted;
unsigned long timeWhenOnSwitchTouched = 0;
unsigned long timeSinceOnSwitchTouched = 0;
bool onSwitchIsBeingTouched = false;




// Message à envoyer via RF 433Mhz :
const char *msg = "Hello world!";





void setup() {
  // Initialisation de la communication via le port série à un débit de 9600 bits/seconde :
  Serial.begin(9600);

  // Initialisation de la communication radio :
  if (!rh_driver.init())
    Serial.println("init failed");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH, INPUT);

  pinMode(LASER, OUTPUT);

  pinMode(LIGHT_SENSOR, INPUT);
}

void systemSwitch() {
  // Pendant que le bouton est appuyé :
  if (systemArmed == false && digitalRead(SWITCH) == HIGH)
  {
    // Si le bouton a déjà été appuyé :
    if (!onSwitchIsBeingTouched)
    {
      // Prend en note le temps :
      timeWhenOnSwitchTouched = timeSinceProgramStarted;

      // Signal que le bouton a déjà été appuyé :
      onSwitchIsBeingTouched = true;
    }

    // Sinon :
    else
    {
      // Met à jour le temps écoulé depuis que le bouton a été appuyé la première fois :
      timeSinceOnSwitchTouched = timeSinceProgramStarted - timeWhenOnSwitchTouched;

      // Si le temps écoulé depuis est plus grand que le délai :
      if (timeSinceOnSwitchTouched >= SYSTEM_ACTIVATION_DELAY)
      {
        // Active le système :
        systemArmed = true;

        // Réinitialise l'état du bouton en spécifiant qu'il n'a maintenant plus été appuyé :
        onSwitchIsBeingTouched = false;
      }
    }
  }

  // Dès que le bouton n'est plus appuyé, réinitialiser son état :
  else
  {
    onSwitchIsBeingTouched = false;
  }
}




void loop() {
  light = analogRead(LIGHT_SENSOR);

  // Temps écoulé depuis le début du sketch :
  timeSinceProgramStarted = millis();

  systemSwitch();
  
  /* else if (systemArmed == false && digitalRead(SWITCH) == HIGH)
  {
    counterSystemOff = counterSystemOff + 1;
    
    if (counterSystemOff >= 250)
    {
      digitalWrite(LED_BUILTIN, LOW);
      systemArmed = false;
      counterSystemOff = 0;
      Serial.println('System off');
    }
  } */





  if (systemArmed)
  {
    digitalWrite(LASER, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    
    // if (light <= THRESHOLD_LIGHT)
    // {
      // rh_driver.send((uint8_t *)msg, strlen(msg));
      // rh_driver.waitPacketSent();
    // }
  }

 

  else
  {
    digitalWrite(LASER, LOW);
    digitalWrite(LED_BUILTIN, LOW);
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