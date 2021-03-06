#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

void setup() {
    Serial.begin(9600); // Debugging only

    pinMode(LED_BUILTIN, OUTPUT);       

    if (!driver.init()) {
        Serial.println("init failed");  
    }
}

void loop() {
    uint8_t buf[12];
    uint8_t buflen = sizeof(buf);

    // Non-blocking
    if (driver.recv(buf, &buflen))
    {
        int i;

        // Message with a good checksum received, dump it.
        Serial.print("Message: ");
        Serial.println((char*)buf);

        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}