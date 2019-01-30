#include <Arduino.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

void setup() {
    Serial.begin(9600);
    Serial.println("Hello, World!");
}

void loop() { // run over and over
    if (Serial.available()) {
        Serial.print(Serial.read());
        delay(5);
    }
}