#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <VirtualWire.h>

const int transmit_pin = 12;

// Analog pins Joystick
const int joyH = 0; //horizontaal - X
const int joyV = 1; //vertikaal   - Y
const int joyK = 2; //knop        - K

bool test = false;

// Waarden opslaan
unsigned int Hval, Vval, Kval;
uint8_t valArray[6];
unsigned int gebruiker = 2000 * 0;

void setup() {
  // Begin Seriele monitor
  if (test) Serial.begin(9600);
    
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);	 // Bits per sec
}

void loop(){
  // value between 0 en 1023 
  Hval = analogRead(joyH);
  Vval = analogRead(joyV);
  Kval = analogRead(joyK);
  Kval = map(Kval, 0, 1023, 0, 1);  //button joystick is 0 or 1
    
  //convert reads: uint16 to 2 tomes uint8 :
  valArray[0] = (Hval + gebruiker) >> 8;
  valArray[1] = (Hval + gebruiker) % 256;
  valArray[2] = (Vval + gebruiker) >> 8;
  valArray[3] = (Vval + gebruiker) % 256;
  valArray[4] = (Kval + gebruiker) >> 8;
  valArray[5] = (Kval + gebruiker) % 256;
  
  // send bytes. Here 6 bytes, that is 6 uint8 or 3 uint16
  vw_send((uint8_t *)valArray, 6);
  // wait for send to end
  vw_wait_tx();
  // Don't send too much the joystick values
  delay(200);
  
  if (test) {
    toonWaarden();
    delay(50);
  }
  
}

void toonWaarden() {
  Serial.print ("  "); 
  Serial.print (Hval);
  Serial.print ("  --  "); 
  Serial.print (Vval);
  Serial.print ("  --  "); 
  Serial.print (Kval);
  Serial.print ("  -- ");
  Serial.print (valArray[0]);
  Serial.print ("  --  ");
  Serial.print (valArray[1]);
  Serial.print ("  --  ");
  Serial.print (valArray[2]);
  Serial.print ("  --  ");
  Serial.print (valArray[3]);
  Serial.print ("  --  ");
  Serial.print (valArray[4]);
  Serial.print ("  --  ");
  Serial.print (valArray[5]);
  Serial.println ("  --  ");
}