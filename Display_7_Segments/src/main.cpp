#include <Arduino.h>
#include "SevSeg.h"

SevSeg sevseg;

void setup() {
  Serial.begin(9600);
  
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};

  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop() {
  sevseg.setNumber(4);
  sevseg.refreshDisplay();
}