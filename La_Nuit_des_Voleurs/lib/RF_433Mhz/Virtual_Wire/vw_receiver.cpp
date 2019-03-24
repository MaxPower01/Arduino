#include <Arduino.h>
#include <VirtualWire.h>

const int RECEIVE_PIN = 11;
bool alarm = false;
int setAlarm, test;

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);       

  // Virtual Wire :
  vw_set_tx_pin(RECEIVE_PIN);
  vw_setup(2000);     
  
  // Start the receiver PLL running
  vw_rx_start();

  setAlarm = 0;
  test = 0;
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint16_t setAlarm, test;


  if (vw_get_message(buf, &buflen)) {
    //Données de type uint8 reçues, conversion en type uint16 :
    if (buflen == 4) {
      setAlarm =  buf[0]; setAlarm = (setAlarm<<8) +  buf[1];
      test =  buf[2]; test = (test<<8) +  buf[3];
    }

    Serial.println(setAlarm);

    if (setAlarm == 1) {
      alarm = true;
    } else if (setAlarm == 0) {
      alarm = false;
    }
  }

  if (alarm) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}