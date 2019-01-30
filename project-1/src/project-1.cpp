#include <Arduino.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

#define LED 13
#define MIN 0
#define MAX 255
#define JOY_SW 2
#define BAUD 9600

volatile bool on = false;
volatile bool isr_triggered = false;

void ISR_TOGGLE_LASER() {
    noInterrupts();
    isr_triggered = true;
    Serial.println("IN ISR!!");
    if (on) {
        digitalWrite(LED, LOW);
        on = false;
    } else {
        digitalWrite(LED, HIGH);
        on = true;
    }
    interrupts();
}

void setup() {
    pinMode(JOY_SW, INPUT_PULLUP);
    Serial.begin(BAUD);

    attachInterrupt(digitalPinToInterrupt(JOY_SW), ISR_TOGGLE_LASER, FALLING);

    for (;;) {
        loop();
    }
}

void loop() {
    if (isr_triggered) {
        noInterrupts();
        isr_triggered = false;
        Serial.println("delaying..");
        delay(1000);
        Serial.println("enabling interrupts again");
        Serial.println();
        interrupts();
    }

}
