#include "Base.h"
#include "Peripherals.h"
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

#define BAUD 9600
#define LED  13

volatile bool buttonState = LOW;
volatile unsigned long lastTime = 0;
volatile unsigned char flags = 0;

void ISR_TOGGLE_LASER() {
    noInterrupts();
    flags = SREG;   // see if the flags are actually set automatically
    if ((millis() - lastTime) > STICK_M_SW_DELAY) {
        buttonState = !buttonState;
    }
    lastTime = millis();
    interrupts();   // TODO: restore interrupt flags instead
}

int main() {
    init_arduino();
    init_laser();
    init_photocell();

    static int photo_value = 0;

    Serial.begin(BAUD);

    // Initialize LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // Configure joystick switch interrupt
    stick_m_on_switch(ISR_TOGGLE_LASER);

    for(;;) {
        // Serial.println(flags);
        photo_value = analogRead(PHOTO_PIN);
        Serial.println(photo_value);
        if (photo_value >= PHOTO_HIT_THRESHOLD) {
            Serial.println("WE'VE BEEN HIT!");
            Serial.println("ending...");
            digitalWrite(LASER_PIN, LOW);
            digitalWrite(LED, LOW);
            Serial.end();
            break;
        }
        digitalWrite(LED, buttonState);
        digitalWrite(LASER_PIN, buttonState);
    }
}