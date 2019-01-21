#include <Arduino.h>

// 1. Variables
// 2. Setup
// 3. Fonctions
// 4. Loop

/* ============================================================ *\ 
|  ==============> 1. Variables
\* ============================================================ */ 

const int BUTTON = 2;
bool buttonPressed = false;
bool systemArmed = true;

const int SOUND_1 = A0;
const int SOUND_2 = A1;

// Size of the array = smoothness
const int soundReadingsNum = 20;

// Define the number of samples to keep track of. The higher the number, the more the readings will be smoothed, but the slower the output will respond to the input. Using a constant rather than a normal variable lets us use this value to determine the size of the readings array.
int soundReadings1[soundReadingsNum];
int soundReadings2[soundReadingsNum];
int soundReadIndex1 = 0;
int soundReadIndex2 = 0;
int soundReadingsTotal1;
int soundReadingsTotal2;
int soundReadingsAverage1;
int soundReadingsAverage2;

const int ROTATION = A2;
int rotationValue;

const int TEMPERATURE = A3;
float temperature;
float temperatureTest;

const int AMBIENT_LIGHT = A4;
int ambientLight;

const int TILT = 11;
bool tilted = false;

const int ULTRASONIC_ECHO = 13;
const int ULTRASONIC_TRIGGER = 12;
int distance;





void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(SOUND_1, INPUT);
  pinMode(SOUND_2, INPUT);
  pinMode(ROTATION, INPUT);
  pinMode(TEMPERATURE, INPUT);
  pinMode(AMBIENT_LIGHT, INPUT);
  pinMode(TILT, INPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(ULTRASONIC_TRIGGER, OUTPUT);

  // Initialize all the sound readings to 0
  for (int thisSoundReading1 = 0; thisSoundReading1 < soundReadingsNum; thisSoundReading1++) {
    soundReadings1[thisSoundReading1] = 0;
  }
  for (int thisSoundReading2 = 1; thisSoundReading2 < soundReadingsNum; thisSoundReading2++) {
    soundReadings2[thisSoundReading2] = 0;
  }
}





void loop() {
  // ----------------------------------------------------------------------------------------
  // Digital Push Button
  // ----------------------------------------------------------------------------------------

  if (digitalRead(BUTTON) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Button pressed.");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }



  // ----------------------------------------------------------------------------------------
  // Analog Sound Sensor v2
  // ----------------------------------------------------------------------------------------

  // Subtract the last reading
  soundReadingsTotal1 = soundReadingsTotal1 - soundReadings1[soundReadIndex1];
  soundReadingsTotal2 = soundReadingsTotal2 - soundReadings2[soundReadIndex2];
  // Read from the sensor
  soundReadings1[soundReadIndex1] = analogRead(SOUND_1);
  soundReadings2[soundReadIndex2] = analogRead(SOUND_2);
  // Add the reading to the total
  soundReadingsTotal1 = soundReadingsTotal1 + soundReadings1[soundReadIndex1];
  soundReadingsTotal2 = soundReadingsTotal2 + soundReadings2[soundReadIndex2];
  // Advance to the next position in the array
  soundReadIndex1 = soundReadIndex1 + 1;
  soundReadIndex2 = soundReadIndex2 + 1;

  if (soundReadIndex1 >= soundReadingsNum) {
    soundReadIndex1 = 0;
  }
  if (soundReadIndex2 >= soundReadingsNum) {
    soundReadIndex2 = 0;
  }

  soundReadingsAverage1 = soundReadingsTotal1 / soundReadingsNum;
  soundReadingsAverage2 = soundReadingsTotal2 / soundReadingsNum;

  if (systemArmed) {
    if (soundReadingsAverage1 > 0 || soundReadingsAverage2 > 0 ) {
      digitalWrite(LED_BUILTIN, HIGH);
      systemArmed = false;
      Serial.println("Sound detected.");
      Serial.println(soundReadingsAverage1);
      Serial.println(soundReadingsAverage2);
      Serial.println("System unarmed.");
    }
  }



  // ----------------------------------------------------------------------------------------
  // Analog Rotation Sensor v2
  // ----------------------------------------------------------------------------------------

  rotationValue = analogRead(ROTATION);
  Serial.println(rotationValue);



  // ----------------------------------------------------------------------------------------
  // Analog LM35 Temperature Sensor v2
  // ----------------------------------------------------------------------------------------

  temperature = analogRead(TEMPERATURE);

  float temperatureCelsius = temperature * 0.48828125;
  float temperatureFahrenheit = (temperatureCelsius * 9) / 5 + 32;



  // ----------------------------------------------------------------------------------------
  // Analog Ambient Light Sensor v2
  // ----------------------------------------------------------------------------------------

  ambientLight = analogRead(AMBIENT_LIGHT);
  Serial.println(ambientLight);



  // ----------------------------------------------------------------------------------------
  // Digital Tilt Sensor v2
  // ----------------------------------------------------------------------------------------

  // The digital tilt sensor detects if it's tilted left or right (0 or 1)
  // Not precise


  if (digitalRead(TILT) == HIGH) {
    tilted = true;
  } else {
    tilted = false;
  }

  Serial.println(tilted);



  // ----------------------------------------------------------------------------------------
  // Ultrasonic Sensor HY-SRF05
  // ----------------------------------------------------------------------------------------

  digitalWrite(ULTRASONIC_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER, LOW);

  const long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  distance = duration / 29 / 2;



  // ----------------------------------------------------------------------------------------
  // Serial
  // ----------------------------------------------------------------------------------------

  Serial.write(1);
  Serial.write("1");
  Serial.write(1);
  Serial.write("This is it");

  delay(1);
}