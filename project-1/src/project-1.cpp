#include "Base.h"
#include "Peripherals.h"
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

#define BAUD 9600
#define LED  13

volatile bool buttonState = LOW;
volatile unsigned long lastTime = 0;

void ISR_TOGGLE_LASER() {
    noInterrupts();
    if ((millis() - lastTime) > STICK_M_SW_DELAY) {
        buttonState = !buttonState;
    }
    lastTime = millis();
    interrupts();   // TODO: restore interrupt flags instead
}

int main() {
    init_arduino();
    init_laser();

    // Initialize LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // Configure joystick switch interrupt
    stick_m_on_switch(ISR_TOGGLE_LASER);

    for(;;) {
        digitalWrite(LED, buttonState);
        digitalWrite(LASER_PIN, buttonState);
    }
}