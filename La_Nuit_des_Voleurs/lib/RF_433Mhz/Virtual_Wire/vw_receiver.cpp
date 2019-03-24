#include <Arduino.h>
#include <VirtualWire.h>

const int RECEIVE_PIN = 11;

int testValue1, testValue2;

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);       

  // Test Virtual Wire :
  vw_set_tx_pin(RECEIVE_PIN);
  vw_setup(2000);     
  
  // Start the receiver PLL running
  vw_rx_start();

  testValue1 = 0;
  testValue2 = 0;
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint16_t tmp_testValue1, tmp_testValue2;

  
  // non blocking, read values when processed
  if (vw_get_message(buf, &buflen)) {
    //uint8 received, convert to normal uint16:
    if (buflen == 6) {
      tmp_testValue1 =  buf[0]; tmp_testValue1 = (tmp_testValue1<<8) +  buf[1];
      tmp_testValue2 =  buf[2]; tmp_testValue2 = (tmp_testValue2<<8) +  buf[3];
    }
    Serial.println(tmp_testValue1);
    Serial.println(tmp_testValue2);
  }
}