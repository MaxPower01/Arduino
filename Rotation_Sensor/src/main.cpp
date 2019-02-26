#include <Arduino.h>

const int ROTATION_SENSOR = A5;
int rotation = 0;

bool rotationAccess = false;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(ROTATION_SENSOR, INPUT);
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
  ) {
    rotationAccess = true;
  } else {
    rotationAccess = false;
  }
}




void loop() {
  // put your main code here, to run repeatedly:
  
  rotation = analogRead(ROTATION_SENSOR);
  checkRotation();

  Serial.println(rotationAccess);

  // if (rotation > 95 && rotation < 110) {
  //   Serial.println("ON");
  // } else {
  //   Serial.println("OFF");
  // }
  
  delay(1);
}