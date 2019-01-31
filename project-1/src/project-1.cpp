#include <Arduino.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

#define BAUD    9600
#define LED     13
#define JOY_SW  2    // make sure this is a digital pin that works for interrupts

volatile bool buttonState = LOW;
volatile unsigned long lastTime = 0;
volatile unsigned long debounceDelay = 30;

void ISR_TOGGLE_LASER() {
    noInterrupts();
    if ((millis() - lastTime) > debounceDelay) {
        buttonState = !buttonState;
    }
    lastTime = millis();
    interrupts();   // TODO: restore interrupt flags instead
}

void setup() {
    pinMode(JOY_SW, INPUT_PULLUP);
    pinMode(LED, OUTPUT);

    digitalWrite(LED, LOW);     // initial value

    attachInterrupt(digitalPinToInterrupt(JOY_SW), ISR_TOGGLE_LASER, CHANGE);
}

void loop() {
    digitalWrite(LED, buttonState);
}
