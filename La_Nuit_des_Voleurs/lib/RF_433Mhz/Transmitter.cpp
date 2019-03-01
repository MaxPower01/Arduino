#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;

const char *msg = "001100110011";

void setup()
{
    Serial.begin(9600);   // Debugging only

    pinMode(LED_BUILTIN, OUTPUT);

    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
}